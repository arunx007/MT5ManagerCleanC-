//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Equity.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CEquity::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Equity",
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
ReportColumn CEquity::s_columns[]=
  {
   //--- id,                name,             type,                        width, width_max, offset,                         size,                              digits_column,          flags
     { COLUMN_BANK,           L"Bank",          IMTDatasetColumn::TYPE_STRING,    22, 0,   offsetof(TableRecord,bank),           MtFieldSize(TableRecord,bank),     0,                      IMTDatasetColumn::FLAG_PRIMARY,},
   { COLUMN_ACCOUNTS,       L"Accounts",      IMTDatasetColumn::TYPE_UINT32,    8, 0,   offsetof(TableRecord,accounts),       0,                                 0,                      0,                            },
   { COLUMN_BALANCE_PREV,   L"Prev Balance",  IMTDatasetColumn::TYPE_MONEY,     12, 0,   offsetof(TableRecord,balance_prev),   0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CLOSED_PL,      L"Closed P/L",    IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,closed_pl),      0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_DEPOSIT,        L"Deposit",       IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,deposit),        0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_BALANCE,        L"Balance",       IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,balance),        0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_FLOATING_PL,    L"Floating P/L",  IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,floating_pl),    0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CREDIT,         L"Credit",        IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,credit),         0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_EQUITY,         L"Equity",        IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,equity),         0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_MARGIN,         L"Margin",        IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,margin),         0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_FREE,           L"Free Margin",   IMTDatasetColumn::TYPE_MONEY,     10, 0,   offsetof(TableRecord,free_margin),    0,                                 COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CURRENCY,       L"Currency",      IMTDatasetColumn::TYPE_STRING,    9, 70,   offsetof(TableRecord,currency),       MtFieldSize(TableRecord,currency), 0,                      0,                            },
   { COLUMN_CURRENCY_DIGITS,L"CurrencyDigits",IMTDatasetColumn::TYPE_UINT32,    0,  0,   offsetof(TableRecord,currency_digits),0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CEquity::CEquity(void) : m_api(NULL),m_user(NULL),m_group(NULL),m_dailies(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CEquity::~CEquity(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CEquity::Clear(void)
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
//--- banks
   m_banks.Clear();
//--- summaries
   m_summaries.Clear();
//--- API interface
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CEquity::Info(MTReportInfo& info)
  {
//--- get info information  
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CEquity::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CEquity::Generate(const uint32_t type,IMTReportAPI *api)
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
      //--- collect data
      for(uint32_t i=0;i<logins_total;i++)
         if((res=TableCollect(logins[i]))!=MT_RET_OK)
           {
            api->Free(logins);
            Clear();
            return(res);
           }
     }
//--- free logins
   if(logins) api->Free(logins);
//--- write table summaries
   if((res=TableWrite())!=MT_RET_OK || (res=TableWriteSummaries())!=MT_RET_OK)
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
//| Prepare table                                                    |
//+------------------------------------------------------------------+
MTAPIRES CEquity::TablePrepare(void)
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
//| Collect data specified login                                     |
//+------------------------------------------------------------------+
MTAPIRES CEquity::TableCollect(const uint64_t login)
  {
//--- checks
   if(!login || !m_api || !m_dailies)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   uint32_t    daily_total,i;
   TableRecord record={},*ptr;
   MTAPIRES    res;

   if((res=m_api->DailyGetLight(login,m_api->ParamFrom(),m_api->ParamTo(),m_dailies))!=MT_RET_OK)
      return(res);
//--- nothing to do?
   if(!m_dailies->Total())
      return(MT_RET_OK);
//--- get user info
   if((res=m_api->UserGet(login,m_user))!=MT_RET_OK)
      return(res);
//--- get user group info
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(res);
//--- prepare record
   CMTStr::Copy(record.bank    ,m_user->Comment());
   CMTStr::Copy(record.currency,m_group->Currency());
   record.currency_digits=m_group->CurrencyDigits();
   record.accounts=1;
//--- collect data
   for(i=0,daily_total=m_dailies->Total();i<daily_total;i++)
     {
      IMTDaily *daily;
      //--- get daily
      if((daily=m_dailies->Next(i))==NULL)
         continue;
      //--- fill record
      record.balance_prev   =SMTMath::MoneyAdd(record.balance_prev,daily->BalancePrevDay()  ,record.currency_digits);
      record.deposit        =SMTMath::MoneyAdd(record.deposit     ,daily->DailyBalance()    ,record.currency_digits);
      record.balance        =SMTMath::MoneyAdd(record.balance     ,daily->Balance()         ,record.currency_digits);
      record.credit         =SMTMath::MoneyAdd(record.credit      ,daily->Credit()          ,record.currency_digits);
      //--- closed P/L
      record.closed_pl      =SMTMath::MoneyAdd(record.closed_pl   ,daily->DailyProfit()     ,record.currency_digits);
      record.closed_pl      =SMTMath::MoneyAdd(record.closed_pl   ,daily->DailyStorage()    ,record.currency_digits);
      record.closed_pl      =SMTMath::MoneyAdd(record.closed_pl   ,daily->DailyCommInstant(),record.currency_digits);
      //--- floating P/L
      record.floating_pl    =SMTMath::MoneyAdd(record.floating_pl ,daily->Profit()          ,record.currency_digits);
      record.floating_pl    =SMTMath::MoneyAdd(record.floating_pl ,daily->ProfitStorage()   ,record.currency_digits);
      //---
      record.equity         =SMTMath::MoneyAdd(record.equity      ,daily->ProfitEquity()    ,record.currency_digits);
      record.margin         =SMTMath::MoneyAdd(record.margin      ,daily->Margin()          ,record.currency_digits);
      record.free_margin    =SMTMath::MoneyAdd(record.free_margin ,daily->MarginFree()      ,record.currency_digits);
     }
//--- search bank
   if((ptr=m_banks.Search(&record,SortBanks))!=NULL)
     {
      //--- update bank
      ptr->balance_prev   =SMTMath::MoneyAdd(ptr->balance_prev,record.balance_prev,ptr->currency_digits);
      ptr->deposit        =SMTMath::MoneyAdd(ptr->deposit     ,record.deposit     ,ptr->currency_digits);
      ptr->closed_pl      =SMTMath::MoneyAdd(ptr->closed_pl   ,record.closed_pl   ,ptr->currency_digits);
      ptr->balance        =SMTMath::MoneyAdd(ptr->balance     ,record.balance     ,ptr->currency_digits);
      ptr->credit         =SMTMath::MoneyAdd(ptr->credit      ,record.credit      ,ptr->currency_digits);
      ptr->floating_pl    =SMTMath::MoneyAdd(ptr->floating_pl ,record.floating_pl ,ptr->currency_digits);
      ptr->equity         =SMTMath::MoneyAdd(ptr->equity      ,record.equity      ,ptr->currency_digits);
      ptr->margin         =SMTMath::MoneyAdd(ptr->margin      ,record.margin      ,ptr->currency_digits);
      ptr->free_margin    =SMTMath::MoneyAdd(ptr->free_margin ,record.free_margin ,ptr->currency_digits);
      //---
      ptr->accounts+=record.accounts;
     }
   else
     {
      //--- insert new bank
      if(m_banks.Insert(&record,SortBanks)==NULL) return(MT_RET_ERR_MEM);
     }
//--- search summary
   if((ptr=m_summaries.Search(&record,SortSummary))!=NULL)
     {
      //--- update summary
      ptr->balance_prev   =SMTMath::MoneyAdd(ptr->balance_prev,record.balance_prev,ptr->currency_digits);
      ptr->deposit        =SMTMath::MoneyAdd(ptr->deposit     ,record.deposit     ,ptr->currency_digits);
      ptr->closed_pl      =SMTMath::MoneyAdd(ptr->closed_pl   ,record.closed_pl   ,ptr->currency_digits);
      ptr->balance        =SMTMath::MoneyAdd(ptr->balance     ,record.balance     ,ptr->currency_digits);
      ptr->credit         =SMTMath::MoneyAdd(ptr->credit      ,record.credit      ,ptr->currency_digits);
      ptr->floating_pl    =SMTMath::MoneyAdd(ptr->floating_pl ,record.floating_pl ,ptr->currency_digits);
      ptr->equity         =SMTMath::MoneyAdd(ptr->equity      ,record.equity      ,ptr->currency_digits);
      ptr->margin         =SMTMath::MoneyAdd(ptr->margin      ,record.margin      ,ptr->currency_digits);
      ptr->free_margin    =SMTMath::MoneyAdd(ptr->free_margin ,record.free_margin ,ptr->currency_digits);
      //---
      ptr->accounts+=record.accounts;
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
//| Write table                                                      |
//+------------------------------------------------------------------+
MTAPIRES CEquity::TableWrite(void)
  {
   MTAPIRES res;
//--- write rows
   for(uint32_t i=0;i<m_banks.Total();i++)
      if((res=m_api->TableRowWrite(&m_banks[i],sizeof(m_banks[i])))!=MT_RET_OK) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CEquity::TableWriteSummaries(void)
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
      //--- total text
      summary->Clear();
      summary->ColumnID(COLUMN_BANK);
      summary->Line(i);
      summary->ValueString(i==0 ? L"Total" : L"");
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total accounts
      summary->Clear();
      summary->ColumnID(COLUMN_ACCOUNTS);
      summary->Line(i);
      summary->ValueUInt(m_summaries[i].accounts);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total prev balance
      summary->Clear();
      summary->ColumnID(COLUMN_BALANCE_PREV);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].balance_prev);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total closed pl
      summary->Clear();
      summary->ColumnID(COLUMN_CLOSED_PL);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].closed_pl);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total deposit
      summary->Clear();
      summary->ColumnID(COLUMN_DEPOSIT);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].deposit);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }

      //--- total balance
      summary->Clear();
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
      //--- total float pl
      summary->Clear();
      summary->ColumnID(COLUMN_FLOATING_PL);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].floating_pl);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total credit
      summary->Clear();
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
      //--- total equity
      summary->Clear();
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
      //--- total margin
      summary->Clear();
      summary->ColumnID(COLUMN_MARGIN);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].margin);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total free margin
      summary->Clear();
      summary->ColumnID(COLUMN_FREE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].free_margin);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- currency        
      summary->Clear();
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
//| Sort banks                                                       |
//+------------------------------------------------------------------+
int32_t CEquity::SortBanks(const void *left,const void *right)
  {
   TableRecord *lft=(TableRecord*)left;
   TableRecord *rgh=(TableRecord*)right;
   int32_t          res;
//--- sort by bank   
   if((res=CMTStr::CompareNoCase(lft->bank,rgh->bank))!=0) return(res);
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
//| Sort summaries                                                   |
//+------------------------------------------------------------------+
int32_t CEquity::SortSummary(const void *left,const void *right)
  {
   TableRecord *lft=(TableRecord*)left;
   TableRecord *rgh=(TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
