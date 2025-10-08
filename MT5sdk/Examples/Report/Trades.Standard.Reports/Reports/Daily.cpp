//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Daily.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CDaily::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Daily Reports",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                   // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },
   3              // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CDaily::s_columns[]=
  {
   //--- id,                name,             type,                        width, width_max, offset,                         size,                              digits_column,          flags
     { COLUMN_TIME,           L"Time",          IMTDatasetColumn::TYPE_DATETIME,  15, 150, offsetof(TableRecord,datetime),       0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_LOGIN,          L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN,10, 0,   offsetof(TableRecord,login),          0,                                 0,                      0,                            },
   { COLUMN_NAME,           L"Name",          IMTDatasetColumn::TYPE_STRING,    20, 0,   offsetof(TableRecord,name),           MtFieldSize(TableRecord,name),     0,                      0,                            },
   { COLUMN_BALANCE_PREV,   L"Prev Balance",  IMTDatasetColumn::TYPE_MONEY,     12, 0,   offsetof(TableRecord,balance_prev),   0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_DEPOSIT,        L"Deposit",       IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,deposit),        0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CLOSED_PL,      L"Closed P/L",    IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,closed_pl),      0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_BALANCE,        L"Balance",       IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,balance),        0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CREDIT,         L"Credit",        IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,credit),         0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_FLOATING_PL,    L"Floating P/L",  IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,floating_pl),    0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_EQUITY,         L"Equity",        IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,equity),         0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_MARGIN,         L"Margin",        IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,margin),         0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_FREE,           L"Free Margin",   IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,free_margin),    0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CURRENCY,       L"Currency",      IMTDatasetColumn::TYPE_STRING,    9, 70,   offsetof(TableRecord,currency),       MtFieldSize(TableRecord,currency), 0,                      0,                            },
   { COLUMN_CURRENCY_DIGITS,L"CurrencyDigits",IMTDatasetColumn::TYPE_UINT32,    0,  0,   offsetof(TableRecord,currency_digits),0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDaily::CDaily(void) : m_api(NULL),m_user(NULL),m_group(NULL),m_dailies(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDaily::~CDaily(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDaily::Clear(void)
  {
//--- group config interface
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- daily array interface
   if(m_dailies)
     {
      m_dailies->Release();
      m_dailies=NULL;
     }
//--- user interface
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- API interface
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDaily::Info(MTReportInfo& info)
  {
//--- get info information  
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CDaily::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CDaily::Generate(const uint32_t type,IMTReportAPI *api)
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
      if((m_dailies=m_api->DailyCreateArray())==NULL ||
         (m_user   =m_api->UserCreate())      ==NULL ||
         (m_group  =api->GroupCreate())       ==NULL)
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
//--- clear 
   Clear();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare table                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDaily::TablePrepare(void)
  {
   IMTDatasetColumn *column;
   MTAPIRES         res;
//--- checks
   if(!m_api) return(MT_RET_ERR_PARAMS);
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
//| Write table for specified login                                  |
//+------------------------------------------------------------------+
MTAPIRES CDaily::TableWrite(const uint64_t login)
  {
   IMTDaily   *daily=NULL;
   uint32_t    daily_total,i;
   TableRecord record={0};
   MTAPIRES    res;
//--- checks
   if(!login || !m_api || !m_dailies) return(MT_RET_ERR_PARAMS);
//--- get deals
   if((res=m_api->DailyGetLight(login,m_api->ParamFrom(),m_api->ParamTo(),m_dailies))!=MT_RET_OK)
      return(res);
//--- nothing to do?
   if(!m_dailies->Total()) return(MT_RET_OK);
//--- get user info
   if((res=m_api->UserGetLight(login,m_user))!=MT_RET_OK) return(res);
//--- get user group info
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK) return(res);
//---
   for(i=0,daily_total=m_dailies->Total();i<daily_total;i++)
     {
      //--- get daily
      if((daily=m_dailies->Next(i))==NULL) continue;
      //--- clear record
      ZeroMemory(&record,sizeof(record));
      //--- fill record
      record.datetime       =daily->Datetime();
      record.login          =daily->Login();
      CMTStr::Copy(record.name,    m_user->Name());
      record.balance_prev   =daily->BalancePrevDay();
      record.deposit        =daily->DailyBalance();
      //--- closed P/L
      record.closed_pl      =daily->DailyProfit();
      record.closed_pl      =SMTMath::MoneyAdd(record.closed_pl,daily->DailyStorage(),daily->CurrencyDigits());
      record.closed_pl      =SMTMath::MoneyAdd(record.closed_pl,daily->DailyCommInstant(),daily->CurrencyDigits());
      //---
      record.balance        =daily->Balance();
      record.credit         =daily->Credit();
      //--- floating P/L
      record.floating_pl    =daily->Profit();
      record.floating_pl    =SMTMath::MoneyAdd(record.floating_pl,daily->ProfitStorage(),daily->CurrencyDigits());
      //---
      record.equity         =daily->ProfitEquity();
      record.margin         =daily->Margin();
      record.free_margin    =daily->MarginFree();
      CMTStr::Copy(record.currency,m_group->Currency());
      record.currency_digits=daily->CurrencyDigits();
      //--- write row
      if((res=m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK) return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
