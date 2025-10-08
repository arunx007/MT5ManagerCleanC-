//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvDetailedSectionReport.h"
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CLtvDetailedSection::s_columns[]=
  {
   //--- id, name,                  type,                             width,width_max, offset,                          size,                              digits_column,          flags
     { 1,  L"Name",                   IMTDatasetColumn::TYPE_STRING,    15,   0, offsetof(TableRecord,section),           MtFieldSize(TableRecord,section),  0,                      0                             },
   { 2,  L"FTD",                    IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,in_first_medium),   0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN },
   { 3,  L"LTV",                    IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,in_out_medium),     0,                                 0,                      0                             },
   { 4,  L"First Deposit Time",     IMTDatasetColumn::TYPE_DATETIME,  16, 150, offsetof(TableRecord,in_first_time),     0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN },
   { 5,  L"First Deposits",         IMTDatasetColumn::TYPE_UINT32,     6,   0, offsetof(TableRecord,in_first_total),    0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN },
   { 6,  L"First Deposit Amount",   IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,in_first_amount),   0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN },
   { 7,  L"Deposits",               IMTDatasetColumn::TYPE_UINT32,     6,   0, offsetof(TableRecord,in_total),          0,                                 0,                      0                             },
   { 8,  L"Deposit Amount",         IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,in_amount),         0,                                 0,                      0                             },
   { 9,  L"Deposit Average",        IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,in_medium),         0,                                 0,                      0                             },
   { 10, L"Withdrawals",            IMTDatasetColumn::TYPE_UINT32,     6,   0, offsetof(TableRecord,out_total),         0,                                 0,                      0                             },
   { 11, L"Withdrawal Amount",      IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,out_amount),        0,                                 0,                      0                             },
   { 12, L"Withdrawal Average",     IMTDatasetColumn::TYPE_MONEY,     12,   0, offsetof(TableRecord,out_medium),        0,                                 0,                      0                             },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvDetailedSection::CLtvDetailedSection(const MTReportInfo &info,const uint32_t section_field_offset,LPCWSTR section_name) :
   CCurrencyReport(info),m_section_field_offset(section_field_offset),m_section_name(section_name),m_client_balance(m_currency)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvDetailedSection::~CLtvDetailedSection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CLtvDetailedSection::Clear(void)
  {
//--- clear cache
   m_client_balance.Clear();
//--- clear vectors
   m_sections.Clear();
   m_names.Clear();
   m_top.Clear();
  }
//+------------------------------------------------------------------+
//| Get parameter currency                                           |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedSection::Prepare(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- check section field offset in cache record
   if(m_section_field_offset+sizeof(uint32_t)>sizeof(ClientCache))
      return(MT_RET_ERROR);
//--- initialize cache
   MTAPIRES res=m_client_balance.Initialize(*m_api,m_params);
   if(res!=MT_RET_OK)
      return(res);
//--- create table columns
   if((res=PrepareTable())!=MT_RET_OK)
      return(res);
//--- initialize sections
   if(!m_sections.Initialize(0x1000))
      return(MT_RET_ERR_MEM);
//--- initialize names
   if(!m_names.Initialize(0x1000))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| add table columns                                                |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedSection::PrepareTable(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create column
   IMTDatasetColumn *column=m_api->TableColumnCreate();
   if(!column)
      return(MT_RET_ERR_MEM);
//--- iterate columns
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0;i<_countof(s_columns);i++)
     {
      //--- fill report data column
      if((res=s_columns[i].ReportColumnFill(*column))!=MT_RET_OK)
         break;
      //--- set first column name
      if(!i)
         if((res=column->Name(m_section_name.Str()))!=MT_RET_OK)
            break;
      //--- setup report currency columns
      if(s_columns[i].type==IMTDatasetColumn::TYPE_MONEY)
         if((res=m_currency.UpdateColumn(*column))!=MT_RET_OK)
            break;
      //--- add report data column
      if((res=m_api->TableColumnAdd(column))!=MT_RET_OK)
         break;
     }
//--- release column
   column->Release();
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedSection::Calculate(void)
  {
//--- update cache
   MTAPIRES res=m_client_balance.Update(true);
   if(res!=MT_RET_OK)
      return(res);
//--- read cache
   if((res=m_client_balance.Read(this))!=MT_RET_OK)
      return(res);
//--- calculate medium values
   CalculateMedium();
//--- calulate top
   if((res=m_top.Initialize(m_sections,Section::SortInAmountDesc))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Client balance read handler                                      |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedSection::ClientBalanceRead(const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- get section position from user structure
   const uint32_t section_pos=*(const uint32_t*)(((const char*)&client)+m_section_field_offset);
//--- get section
   Section *section=m_sections.Item(section_pos);
   if(!section)
      return(MT_RET_ERR_MEM);
//--- store section name
   if(!m_names.String(section_pos))
     {
      //--- get section name
      LPCWSTR name=nullptr;
      const MTAPIRES res=m_client_balance.ReadDictionaryString(m_section_field_offset,section_pos,name);
      if(res!=MT_RET_OK)
         return(res);
      //--- store section name
      if(!m_names.String(section_pos,name && *name ? name : L"None"))
         return(MT_RET_ERR_MEM);
     }
//--- calculate balance
   uint32_t day_in_first=0;
   const DealBalance *deal_in_first=nullptr;
   for(uint32_t i=0;const DealBalance *deal=balance.Deal(i);i++)
     {
      const double value=rate ? m_currency.Convert(deal->value,*rate) : deal->value;
      section->DealAdd(value,deal->count,m_currency);
      if((!day_in_first || day_in_first>deal->info.day) && deal->info.deposit)
        {
         //--- first in aggregate
         day_in_first=deal->info.day;
         deal_in_first=deal;
        }
     }
//--- add first deposit deal
   if(deal_in_first)
      section->DealInFirstAdd(deal_in_first->value,deal_in_first->count,deal_in_first->info.day*SECONDS_IN_DAY,m_currency);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| calculate medium values                                          |
//+------------------------------------------------------------------+
void CLtvDetailedSection::CalculateMedium(void)
  {
//--- iterate all sections and calculate medium
   for(uint32_t i=0,total=m_sections.Total();i<total;i++)
      if(Section *section=m_sections.Item(i))
         section->CalculateMedium(m_currency);
  }
//+------------------------------------------------------------------+
//| Write result                                                     |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedSection::WriteResult(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- check section count
   const uint32_t total=m_top.Total();
   if(!total)
      return(MT_RET_OK_NONE);
//--- iterate top
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
     {
      //--- write section
      MTAPIRES res=WriteSection(m_top.Pos(i));
      if(res!=MT_RET_OK)
         break;
     }
//--- write statistics to log
   return(m_api->LoggerOut(MTLogOK,L"Written %u of %u sections",m_api->TableRowTotal(),total));
  }
//+------------------------------------------------------------------+
//| Write section to result                                          |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedSection::WriteSection(const uint32_t *pos)
  {
//--- checks
   if(!m_api || !pos)
      return(MT_RET_ERR_PARAMS);
//--- get section
   const Section *section=m_sections.Item(*pos);
   if(!section)
      return(MT_RET_ERROR);
//--- skip empty section
   if(!section->in_first.total && !section->in.total && !section->out.total)
      return(MT_RET_OK);
//--- get section name
   LPCWSTR name=m_names.String(*pos);
   if(!name)
      return(MT_RET_ERROR);
//--- fill record by section
   TableRecord record;
   CMTStr::Copy(record.section,name);
   record.in_out_medium    =section->in_out_medium;
   record.in_first_medium  =section->in_first.medium;
   record.in_first_time    =section->in_first_time;
   record.in_first_total   =section->in_first.total;
   record.in_first_amount  =section->in_first.amount;
   record.in_total         =section->in.total;
   record.in_amount        =section->in.amount;
   record.in_medium        =section->in.medium;
   record.out_total        =section->out.total;
   record.out_amount       =section->out.amount;
   record.out_medium       =section->out.medium;
//--- write data row
   return(m_api->TableRowWrite(&record,sizeof(record)));
  }
//+------------------------------------------------------------------+
