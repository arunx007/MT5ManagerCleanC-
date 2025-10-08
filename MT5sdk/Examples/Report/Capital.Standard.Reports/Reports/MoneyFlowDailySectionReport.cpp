//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MoneyFlowDailySectionReport.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMoneyFlowDailySection::CMoneyFlowDailySection(const MTReportInfo &info,const uint32_t section_field_offset) :
   CMoneyFlowDaily(info,false),m_section_field_offset(section_field_offset),m_client_balance(m_currency),m_money(true),m_total(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMoneyFlowDailySection::~CMoneyFlowDailySection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CMoneyFlowDailySection::Clear(void)
  {
//--- clear cache
   m_client_balance.Clear();
//--- clear containers
   m_money.Clear();
   m_total.Clear();
   m_top.Clear();
//--- base call
   CMoneyFlowDaily::Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailySection::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- check section field offset in cache record
   if(m_section_field_offset+sizeof(uint32_t)>sizeof(CUserCache::UserCache))
      return(MT_RET_ERROR);
//--- base call
   MTAPIRES res=CMoneyFlowDaily::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- initialize cache
   if((res=m_client_balance.Initialize(*m_api,m_params))!=MT_RET_OK)
      return(res);
//--- initialize money matrix
   if(!m_money.Initialize(m_interval.PeriodTotal(),0x100))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailySection::Calculate(void)
  {
//--- update cache
   MTAPIRES res=m_client_balance.Update(false);
   if(res!=MT_RET_OK)
      return(res);
//--- read cache
   if((res=m_client_balance.Read(this))!=MT_RET_OK)
      return(res);
//--- calulate total vector
   Section::COperationAdd operation(m_currency);
   if(!m_money.OperationColumns(m_total,operation))
      return(MT_RET_ERR_MEM);
//--- calulate top
   if((res=m_top.InitializeOther(m_total,Section::SortInAmountDesc,operation,m_params))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Client balance read handler                                      |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailySection::ClientBalanceRead(const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- get section position from user structure
   const uint32_t section_pos=*(const uint32_t*)(((const char*)&client)+m_section_field_offset);
//--- calculate balance
   for(uint32_t i=0;const DealBalance *deal=balance.Deal(i);i++)
     {
      //--- balance time
      const int64_t time=deal->info.day*SECONDS_IN_DAY;
      if(time>=m_api->ParamFrom() && time<=m_api->ParamTo())
        {
         //--- get record by deal time
         Section *section=RecordByTimeAndSection(time,section_pos);
         if(!section)
            return(MT_RET_ERR_MEM);
         //--- add deal balance to section
         section->DealAdd(rate ? m_currency.Convert(deal->value,*rate) : deal->value,deal->count,m_currency);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| record by time and section                                       |
//+------------------------------------------------------------------+
CMoneyFlowDailySection::Section* CMoneyFlowDailySection::RecordByTimeAndSection(const int64_t ctm,const uint32_t section)
  {
//--- calculate period index
   const int32_t period=m_interval.PeriodIndex(ctm);
   if(period<0)
      return(nullptr);
//--- return matrix item
   return(m_money.Item(period,section));
  }
//+------------------------------------------------------------------+
//| Prepare money data                                               |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailySection::PrepareData(IMTDataset &data)
  {
//--- check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create first dataset column
   MTAPIRES res=PrepareDataColumns(data);
   if(res!=MT_RET_OK)
      return(res);
//--- check sections
   const uint32_t section_count=m_top.Total();
   if(!section_count)
      return(MT_RET_OK_NONE);
//--- initialize accumulated vector
   DoubleVector accumulated(true);
   if(!accumulated.InitializeValue(section_count,NAN))
      return(MT_RET_ERR_MEM);
//--- row buffer
   CMTMemPack pack;
   const uint32_t row_size=sizeof(MoneyRow)+(section_count-1)*sizeof(MoneyRecord);
   if(!pack.Reallocate(row_size))
      return(MT_RET_ERR_MEM);
   MoneyRow *row=(MoneyRow *)pack.Buffer();
   if(!row)
      return(MT_RET_ERR_MEM);
//--- write each date
   for(uint32_t i=0,total=m_money.Width();i<total;i++)
     {
      //--- zero data row
      ZeroMemory(row,row_size);
      //--- get sections
      const SectionVector *sections=m_money.Column(i);
      if(!sections)
         return(MT_RET_ERROR);
      //--- calculate date
      row->date=m_interval.PeriodDate(i);
      //--- fill data row
      FillRow(*row,*sections,accumulated);
      //--- write data row
      if((res=data.RowWrite(pack.Buffer(),row_size))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare dataset columns                                          |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailySection::PrepareDataColumns(IMTDataset &data)
  {
//--- create first dataset column
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_money,1);
   if(res!=MT_RET_OK)
      return(res);
//---- initialize dataset sections columns
   CMTStr256 section;
   ReportColumn columns[_countof(s_columns_money)-1];
   for(uint32_t i=0;i<_countof(columns);i++)
     {
      columns[i]=s_columns_money[i+1];
      columns[i].name=section.Str();
     }
//--- read top names
   CReportDictionary names;
   if((res=m_client_balance.ReadTopNames(m_section_field_offset,m_top,names))!=MT_RET_OK)
      return(res);
//--- iterate sections
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
     {
      //--- get section name
      section.Assign(names.String(i));
      //--- add columns to dataset
      if((res=ReportColumn::ReportColumnsAdd(data,columns,_countof(columns)))!=MT_RET_OK)
         break;
      //--- next date
      for(uint32_t i=0;i<_countof(columns);i++)
        {
         columns[i].id+=_countof(s_columns_money)-1;
         columns[i].offset+=sizeof(MoneyRecord);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill row                                                         |
//+------------------------------------------------------------------+
void CMoneyFlowDailySection::FillRow(MoneyRow &row,const SectionVector &sections,DoubleVector &accumulated) const
  {
//--- iterate accumulated sections
   static const Section empty={0};
   for(uint32_t i=0,total=m_top.Total();i<total;i++)
      if(const uint32_t *pos=m_top.Pos(i))
        {
         //--- destination section
         MoneyRecord &dst=row.sections[i];
         //--- check other section
         if(*pos==SectionTop::TOP_POS_OTHER)
           {
            //--- calculate other
            Section other={0};
            Section::COperationAdd operation(m_currency);
            m_top.CalculateOther(other,sections,operation);
            //--- copy other
            FillRecord(dst,other,accumulated.Item(i));
           }
         else
           {
            //--- fill section row
            const Section *src=sections.Item(*pos);
            if(!src)
               src=&empty;
            FillRecord(dst,*src,accumulated.Item(i));
           }
        }
  }
//+------------------------------------------------------------------+
//| chart columns add                                                |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailySection::ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const
  {
//--- add data columns for top
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0,total=m_top.Total();i<total && res==MT_RET_OK;i++)
      res=chart.DataColumnAdd(column.id+i*(_countof(s_columns_money)-1));
   return(res);
  }
//+------------------------------------------------------------------+
