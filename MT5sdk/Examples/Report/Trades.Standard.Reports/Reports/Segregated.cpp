//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Segregated.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CSegregated::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Segregated",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_ACCOUNTS,
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
ReportColumn CSegregated::s_columns[]=
  {
   //--- id,                 name,             type,                          width, width_max, offset,                         size,                              digits_column,          flags
     { COLUMN_LOGIN,           L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN, 10, 0,  offsetof(TableRecord,login),           0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_NAME,            L"Name",          IMTDatasetColumn::TYPE_STRING,     20, 0,  offsetof(TableRecord,name),            MtFieldSize(TableRecord,name),     0,                      0                             },
   { COLUMN_DEPOSIT,         L"Deposit",       IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,deposit),         0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_CREDIT,          L"Credit",        IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,credit),          0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_COMMISSION,      L"Commission",    IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,commission),      0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_STORAGE,         L"Swap",          IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,storage),         0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_PROFIT,          L"Profit",        IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,profit),          0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_INTEREST,        L"Interest",      IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,interest),        0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_BALANCE,         L"Balance",       IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,balance),         0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_FLOATING,        L"Floating P/L",  IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,floating),        0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_EQUITY,          L"Equity",        IMTDatasetColumn::TYPE_MONEY,      10, 0,  offsetof(TableRecord,equity),          0,                                 COLUMN_CURRENCY_DIGITS, 0                             },
   { COLUMN_CURRENCY,        L"Currency",      IMTDatasetColumn::TYPE_STRING,     9,  70, offsetof(TableRecord,currency),        MtFieldSize(TableRecord,currency), 0,                      0                             },
   { COLUMN_CURRENCY_DIGITS, L"CurrencyDigits",IMTDatasetColumn::TYPE_UINT32,     0,  0,  offsetof(TableRecord,currency_digits), 0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CSegregated::CSegregated(void) : m_api(NULL),m_daily(NULL),m_user(NULL),m_group(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CSegregated::~CSegregated(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CSegregated::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CSegregated::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CSegregated::Generate(const uint32_t type,IMTReportAPI *api)
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
//---
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
      //--- prepare deals array & trade account & user & group
      if((m_daily  =api->DailyCreateArray())==NULL ||
         (m_user   =api->UserCreate())      ==NULL ||
         (m_group  =api->GroupCreate())     ==NULL)
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
void CSegregated::Clear(void)
  {
//--- user daily information interface
   if(m_daily)
     {
      m_daily->Release();
      m_daily=NULL;
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
MTAPIRES CSegregated::TablePrepare(void)
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
MTAPIRES CSegregated::TableWrite(const uint64_t login)
  {
   TableRecord record={0},*ptr;
   MTAPIRES    res;
   IMTDaily   *daily=NULL;
//--- checks
   if(!m_api || !login || !m_daily || !m_user || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- get user info
   if((res=m_api->UserGetLight(login,m_user))!=MT_RET_OK)
      return(res);
//--- get user group info
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(res);
//--- get daily info
   if((res=m_api->DailyGetLight(login,m_api->ParamFrom(),m_api->ParamTo(),m_daily))!=MT_RET_OK)
      return(res);
//--- fill real values
   if(m_daily->Total() && (daily=m_daily->Next(m_daily->Total()-1))!=NULL)
      if(SMTTime::DayBegin(m_api->ParamTo())==SMTTime::DayBegin(daily->Datetime()))
        {
         record.equity  =daily->ProfitEquity();
         record.floating=SMTMath::MoneyAdd(record.floating,daily->Profit()          ,m_group->CurrencyDigits());
         record.floating=SMTMath::MoneyAdd(record.floating,daily->ProfitStorage()   ,m_group->CurrencyDigits());
         record.balance =daily->Balance();
        }
//--- fill values by period
   for(uint32_t i=0;i<m_daily->Total();i++)
     {
      if((daily=m_daily->Next(i))==NULL)
         continue;
      //--- summary
      record.deposit   =SMTMath::MoneyAdd(record.deposit   ,daily->DailyBalance()    ,m_group->CurrencyDigits());
      record.credit    =SMTMath::MoneyAdd(record.credit    ,daily->DailyCredit()     ,m_group->CurrencyDigits());
      record.commission=SMTMath::MoneyAdd(record.commission,daily->DailyCommInstant(),m_group->CurrencyDigits());
      record.commission=SMTMath::MoneyAdd(record.commission,daily->DailyCommRound()  ,m_group->CurrencyDigits());
      record.storage   =SMTMath::MoneyAdd(record.storage   ,daily->DailyStorage()    ,m_group->CurrencyDigits());
      record.profit    =SMTMath::MoneyAdd(record.profit    ,daily->DailyProfit()     ,m_group->CurrencyDigits());
      record.interest  =SMTMath::MoneyAdd(record.interest  ,daily->DailyInterest()   ,m_group->CurrencyDigits());
     }
//--- prepare record
   record.login=login;
   CMTStr::Copy(record.name,m_user->Name());
   CMTStr::Copy(record.currency,m_group->Currency());
   record.currency_digits=m_group->CurrencyDigits();
//--- write row
   if((res=m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK) return(res);
//--- search summary
   if((ptr=m_summaries.Search(&record,SortSummary))!=NULL)
     {
      //--- update summaries
      ptr->deposit   =SMTMath::MoneyAdd(ptr->deposit   ,record.deposit   ,ptr->currency_digits);
      ptr->credit    =SMTMath::MoneyAdd(ptr->credit    ,record.credit    ,ptr->currency_digits);
      ptr->commission=SMTMath::MoneyAdd(ptr->commission,record.commission,ptr->currency_digits);
      ptr->storage   =SMTMath::MoneyAdd(ptr->storage   ,record.storage   ,ptr->currency_digits);
      ptr->profit    =SMTMath::MoneyAdd(ptr->profit    ,record.profit    ,ptr->currency_digits);
      ptr->interest  =SMTMath::MoneyAdd(ptr->interest  ,record.interest  ,ptr->currency_digits);
      ptr->balance   =SMTMath::MoneyAdd(ptr->balance   ,record.balance   ,ptr->currency_digits);
      ptr->floating  =SMTMath::MoneyAdd(ptr->floating  ,record.floating  ,ptr->currency_digits);
      ptr->equity    =SMTMath::MoneyAdd(ptr->equity    ,record.equity    ,ptr->currency_digits);
     }
   else
     {
      //--- insert new summary
      if(m_summaries.Insert(&record,SortSummary)==NULL) return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CSegregated::TableWriteSummaries(void)
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
      summary->ColumnID(COLUMN_LOGIN);
      summary->MergeColumn(COLUMN_NAME);
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
      //--- total balance
      summary->ColumnID(COLUMN_BALANCE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].balance);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total credit
      summary->ColumnID(COLUMN_CREDIT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].credit);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total commission
      summary->ColumnID(COLUMN_COMMISSION);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].commission);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total storage
      summary->ColumnID(COLUMN_STORAGE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].storage);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total profit
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
      //--- total interest
      summary->ColumnID(COLUMN_INTEREST);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].interest);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total floating P / L
      summary->ColumnID(COLUMN_FLOATING);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].floating);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total equity
      summary->ColumnID(COLUMN_EQUITY);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].equity);
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
int32_t CSegregated::SortSummary(const void *left,const void *right)
  {
   TableRecord *lft=(TableRecord*)left;
   TableRecord *rgh=(TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
  