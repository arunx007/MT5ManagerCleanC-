//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Agents.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CAgents::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Agents",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                   // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },3            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CAgents::s_columns[]=
  {
   //--- id,                 name,             type,                         width,  width_max, offset,                          size,                              digits_column,          flags
     { COLUMN_DEAL,            L"Deal",          IMTDatasetColumn::TYPE_UINT64,     10, 100, offsetof(TableRecord,deal),            0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_LOGIN,           L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN, 10, 100, offsetof(TableRecord,login),           0,                                 0,                      0                             },
   { COLUMN_NAME,            L"Name",          IMTDatasetColumn::TYPE_STRING,     20, 0,   offsetof(TableRecord,name),            MtFieldSize(TableRecord,name),     0,                      0                             },
   { COLUMN_TIME,            L"Time",          IMTDatasetColumn::TYPE_DATETIME,   10, 130, offsetof(TableRecord,time_create),     0,                                 0,                      0                             },
   { COLUMN_TYPE,            L"Type",          IMTDatasetColumn::TYPE_DEAL_ACTION,10, 130, offsetof(TableRecord,type),            0,                                 0,                      0                             },
   { COLUMN_COMMENT,         L"Comment",       IMTDatasetColumn::TYPE_STRING,     10, 0  , offsetof(TableRecord,comment),         MtFieldSize(TableRecord,comment),  0,                      0                             },
   { COLUMN_PROFIT,          L"Profit",        IMTDatasetColumn::TYPE_MONEY,      10, 100, offsetof(TableRecord,profit) ,         0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_CURRENCY,        L"Currency",      IMTDatasetColumn::TYPE_STRING,      9, 100, offsetof(TableRecord,currency),        MtFieldSize(TableRecord,currency), 0,                      0                             },
   { COLUMN_CURRENCY_DIGITS, L"CurrencyDigits",IMTDatasetColumn::TYPE_UINT32,      0, 0  , offsetof(TableRecord,currency_digits), 0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CAgents::CAgents(void) : m_api(NULL),m_deals(NULL),m_user(NULL),m_group(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CAgents::~CAgents(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CAgents::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CAgents::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CAgents::Generate(const uint32_t type,IMTReportAPI *api)
  {
   uint64_t  *logins=NULL;
   uint32_t logins_total=0;
   MTAPIRES res;
//--- clear
   Clear();
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(type!=MTReportInfo::TYPE_TABLE)
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- save api pointer
   m_api=api;
//--- prepare table
   if((res=TablePrepare())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- get report logins
   if((res=api->ParamLogins(logins,logins_total))!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- checks logins
   if(logins && logins_total)
     {
      //--- prepare deals array & user & group
      if((m_deals=api->DealCreateArray())==NULL ||
         (m_user =api->UserCreate())     ==NULL ||
         (m_group=api->GroupCreate())    ==NULL)
        {
         api->Free(logins);
         Clear();
         return(MT_RET_ERR_MEM);
        }
      //--- write table
      for(uint32_t i=0;i<logins_total;i++)
         if((res=TableWrite(logins[i]))!=MT_RET_OK)
           {
            api->Free(logins);
            Clear();
            return(res);
           }
     }
//--- free logins
   if(logins) api->Free(logins);
//--- write table summaries
   if((res=TableWriteSummaries())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- clear 
   Clear();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Cleanup                                                          |
//+------------------------------------------------------------------+
void CAgents::Clear(void)
  {
//--- deals interface
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- user interface
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- group interface
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- summaries
   m_summaries.Clear();
//--- API interface
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Prepare table columns                                            |
//+------------------------------------------------------------------+
MTAPIRES CAgents::TablePrepare(void)
  {
   IMTDatasetColumn *column;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create column
   if((column=m_api->TableColumnCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- clear table columns
   m_api->TableColumnClear();
//--- add column description for TableRecord
   for(uint32_t i=0;i<_countof(s_columns);i++)
     {
      //--- clear
      column->Clear();
      //--- fill info
      column->ColumnID(s_columns[i].id);
      column->Name(s_columns[i].name);
      column->Type(s_columns[i].type);
      column->Width(s_columns[i].width);
      column->WidthMax(s_columns[i].width_max);
      column->Offset(s_columns[i].offset);
      column->Size(s_columns[i].size);
      column->DigitsColumn(s_columns[i].digits_column);
      column->Flags(s_columns[i].flags);
      //--- add column
      if((res=m_api->TableColumnAdd(column))!=MT_RET_OK)
        {
         column->Release();
         return(res);
        }
     }
//--- release column
   column->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write table                                                      |
//+------------------------------------------------------------------+
MTAPIRES CAgents::TableWrite(const uint64_t login)
  {
   uint32_t      deal_total,i;
   IMTDeal      *deal;
   TableRecord   record,summary={0},*ptr;
   bool          summary_exist=false;
   MTAPIRES      res;
//--- checks
   if(!m_api || !login || !m_deals || !m_user || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   if((res=m_api->DealGet(login,m_api->ParamFrom(),m_api->ParamTo(),m_deals))!=MT_RET_OK)
      return(res);
//--- get user info
   if((res=m_api->UserGetLight(login,m_user))!=MT_RET_OK)
      return(res);
//--- get user group info
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(res);
//--- prepare summary
   CMTStr::Copy(summary.currency,m_group->Currency());
   summary.currency_digits=m_group->CurrencyDigits();
//---
   for(i=0,deal_total=m_deals->Total();i<deal_total;i++)
     {
      //--- get deal
      if((deal=m_deals->Next(i))==NULL) continue;
      //--- is agent?
      if(deal->Action()==IMTDeal::DEAL_AGENT_DAILY    ||
         deal->Action()==IMTDeal::DEAL_AGENT_MONTHLY  ||
         deal->Action()==IMTDeal::DEAL_AGENT)
        {
         //--- clear record
         ZeroMemory(&record,sizeof(record));
         //--- fill record
         record.deal           =deal->Deal();
         record.login          =deal->Login();
         CMTStr::Copy(record.name,m_user->Name());
         record.time_create    =deal->Time();
         record.type           =deal->Action();
         CMTStr::Copy(record.comment,deal->Comment());
         record.profit         =deal->Profit();
         CMTStr::Copy(record.currency,m_group->Currency());
         record.currency_digits=deal->DigitsCurrency();
         //--- write row
         if((res=m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK) return(res);
         //--- calc summary
         summary.profit=SMTMath::MoneyAdd(summary.profit,deal->Profit(),summary.currency_digits);
         //--- exist summary
         summary_exist=true;
        }
     }
//--- check summary
   if(!summary_exist) return(MT_RET_OK);
//--- search summary
   if((ptr=m_summaries.Search(&summary,SortSummary))!=NULL)
     {
      //--- update summaries
      ptr->profit=SMTMath::MoneyAdd(ptr->profit,summary.profit,ptr->currency_digits);
     }
   else
     {
      //--- insert new summary
      if(m_summaries.Insert(&summary,SortSummary)==NULL) return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CAgents::TableWriteSummaries(void)
  {
   IMTDatasetSummary *summary=NULL;
   uint32_t          i;
   MTAPIRES          res;
//--- checks
   if(!m_api) return(MT_RET_ERR_MEM);
//--- create summary
   if((summary=m_api->TableSummaryCreate())==NULL)
      return(MT_RET_ERR_MEM);
//---
   for(i=0;i<m_summaries.Total();i++)
     {
      //--- clear summary
      summary->Clear();
      //--- total text
      summary->ColumnID(COLUMN_DEAL);
      summary->MergeColumn(COLUMN_COMMENT);
      summary->Line(i);
      summary->ValueString(i==0 ? L"Total" : L"");
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- total amount
      summary->ColumnID(COLUMN_PROFIT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].profit);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- currency
      summary->ColumnID(COLUMN_CURRENCY);
      summary->Line(i);
      summary->ValueString(m_summaries[i].currency);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
     }
//--- release summary
   summary->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Sort summary                                                     |
//+------------------------------------------------------------------+
int32_t CAgents::SortSummary(const void *left,const void *right)
  {
   TableRecord *lft=(TableRecord*)left;
   TableRecord *rgh=(TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+

