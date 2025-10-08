//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MarginCall.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CMarginCall::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Margin Calls",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_ACCOUNTS,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                    // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
     },1            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CMarginCall::s_columns[]=
  {
   //--- id,                 name,               type,                      width, width_max, offset,                         size,                                   digits_column,          flags
     { COLUMN_LOGIN,           L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN,   10, 0,  offsetof(TableRecord,login),           0,                                      0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_NAME,            L"Name",            IMTDatasetColumn::TYPE_STRING,       20, 0,  offsetof(TableRecord,name),            MtFieldSize(TableRecord,name),          0,                      0,                            },
   { COLUMN_LEVERAGE,        L"Leverage",        IMTDatasetColumn::TYPE_USER_LEVERAGE,10, 0,  offsetof(TableRecord,leverage),        0,                                      0,                      0,                            },
   { COLUMN_BALANCE,         L"Balance",         IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,balance),         0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CREDIT,          L"Credit",          IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,credit),          0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_FLOATING,        L"Floating P/L",    IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,floating),        0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_EQUTITY,         L"Equity",          IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,equity),          0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_MARGIN,          L"Margin",          IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,margin),          0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_MARGIN_FREE,     L"Free Margin",     IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,margin_free),     0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_MARGIN_LIMITS,   L"Margin Limits",   IMTDatasetColumn::TYPE_STRING,       10, 0,  offsetof(TableRecord,margin_limits),   MtFieldSize(TableRecord,margin_limits), 0,                      IMTDatasetColumn::FLAG_RIGHT   },
   { COLUMN_MARGIN_LEVEL,    L"Margin Level,  %",IMTDatasetColumn::TYPE_DOUBLE,       10, 0,  offsetof(TableRecord,margin_level),    0,                                      0,                      0                             },
   { COLUMN_MARGIN_ADD,      L"Add. Margin",     IMTDatasetColumn::TYPE_MONEY,        10, 0,  offsetof(TableRecord,margin_add),      0,                                      COLUMN_CURRENCY_DIGITS, 0,                            },
   { COLUMN_CURRENCY,        L"Currency",        IMTDatasetColumn::TYPE_STRING,       9,  70, offsetof(TableRecord,currency),        MtFieldSize(TableRecord,currency),      0,                      0,                            },
   { COLUMN_CURRENCY_DIGITS, L"CurrencyDigits",  IMTDatasetColumn::TYPE_UINT32,       0,  0,  offsetof(TableRecord,currency_digits), 0,                                      0,                      IMTDatasetColumn::FLAG_HIDDEN  }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMarginCall::CMarginCall(void) : m_api(NULL),m_account(NULL),m_user(NULL),m_group(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMarginCall::~CMarginCall(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CMarginCall::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CMarginCall::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CMarginCall::Generate(const uint32_t type,IMTReportAPI *api)
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
      //--- prepare trade account & user & group
      if((m_account=api->UserCreateAccount())==NULL ||
         (m_user   =api->UserCreate())       ==NULL ||
         (m_group  =api->GroupCreate())      ==NULL)
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
void CMarginCall::Clear(void)
  {
//--- trade account interface
   if(m_account)
     {
      m_account->Release();
      m_account=NULL;
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
//--- api
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Prepare table columns                                            |
//+------------------------------------------------------------------+
MTAPIRES CMarginCall::TablePrepare(void)
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
      //--- это уровень маржи?
      if(s_columns[i].id==COLUMN_MARGIN_LEVEL)
         column->Digits(2);
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
MTAPIRES CMarginCall::TableWrite(const uint64_t login)
  {
   CMTStr256   str;
   TableRecord record={0},*ptr;
   MTAPIRES    res;
//--- checks
   if(!m_api || !login || !m_account || !m_user || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- get account
   if((res=m_api->UserAccountGet(login,m_account))!=MT_RET_OK)
      return(res==MT_RET_ERR_NOTFOUND ? MT_RET_OK : res);
//--- check activations
   if(m_account->SOActivation()!=IMTAccount::ACTIVATION_MARGIN_CALL &&
      m_account->SOActivation()!=IMTAccount::ACTIVATION_STOP_OUT)
      return(MT_RET_OK);
//--- get user info
   if((res=m_api->UserGetLight(login,m_user))!=MT_RET_OK)
      return(res);
//--- get user group info
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(res);
//--- fill record
   record.login       =login;
   CMTStr::Copy(record.name,m_user->Name());
   record.leverage    =m_account->MarginLeverage();
   record.balance     =m_account->Balance();
   record.credit      =m_account->Credit();
   record.floating    =m_account->Floating();
   record.equity      =m_account->Equity();
   record.margin      =m_account->Margin();
   record.margin_free =m_account->MarginFree();
   record.margin_level=m_account->MarginLevel();
//---
   CMTStr::Copy(record.margin_limits,FormatMarginLimits(str,m_group));
//---
   record.margin_add =CalcMarginAdditional(m_group,m_account);
//---
   record.currency_digits=m_group->CurrencyDigits();
   CMTStr::Copy(record.currency,m_group->Currency());
//--- write row
   if((res=m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK) return(res);
//--- search summary
   if((ptr=m_summaries.Search(&record,SortSummary))!=NULL)
     {
      //--- update summaries
      ptr->balance     =SMTMath::MoneyAdd(ptr->balance    ,record.balance     ,ptr->currency_digits);
      ptr->credit      =SMTMath::MoneyAdd(ptr->credit     ,record.credit      ,ptr->currency_digits);
      ptr->floating    =SMTMath::MoneyAdd(ptr->floating   ,record.floating    ,ptr->currency_digits);
      ptr->equity      =SMTMath::MoneyAdd(ptr->equity     ,record.equity      ,ptr->currency_digits);
      ptr->margin      =SMTMath::MoneyAdd(ptr->margin     ,record.margin      ,ptr->currency_digits);
      ptr->margin_free =SMTMath::MoneyAdd(ptr->margin_free,record.margin_free ,ptr->currency_digits);
      ptr->margin_add  =SMTMath::MoneyAdd(ptr->margin_add ,record.margin_add  ,ptr->currency_digits);
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
MTAPIRES CMarginCall::TableWriteSummaries(void)
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
      summary->MergeColumn(COLUMN_LEVERAGE);
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
      //--- total floating
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
      summary->ColumnID(COLUMN_EQUTITY);
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
      summary->ColumnID(COLUMN_MARGIN_FREE);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].margin_free);
      summary->Digits(m_summaries[i].currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total add margin
      summary->ColumnID(COLUMN_MARGIN_ADD);
      summary->Line(i);
      summary->ValueMoney(m_summaries[i].margin_add);
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
//| Calculate Additional Margin                                      |
//+------------------------------------------------------------------+
double CMarginCall::CalcMarginAdditional(const IMTConGroup *group,const IMTAccount *account)
  {
   double add=0;
//--- checks
   if(!group || !account)
      return(0);
//--- in money?
   if(group->MarginSOMode()==IMTConGroup::STOPOUT_MONEY)
      add=group->MarginCall()-account->Equity();
   else
      add=group->MarginCall()*account->Margin()/100.0-account->Equity();
//--- result
   return(add);
  }
//+------------------------------------------------------------------+
//| Format margin limits                                             |
//+------------------------------------------------------------------+
LPCWSTR CMarginCall::FormatMarginLimits(CMTStr& str,const IMTConGroup *group)
  {
   CMTStr64 scall,sso;
//--- clear
   str.Clear();
//--- checks
   if(!group)
      return(str.Str());
//--- in money?
   if(group->MarginSOMode()==IMTConGroup::STOPOUT_MONEY)
      str.Format(L"%s / %s",
         SMTFormat::FormatMoney(scall,group->MarginCall(),group->CurrencyDigits()),
         SMTFormat::FormatMoney(sso,group->MarginStopOut(),group->CurrencyDigits()));
   else
      str.Format(L"%.0lf %% / %.0lf %%",group->MarginCall(),group->MarginStopOut());
//--- return str
   return(str.Str());
  }
//+------------------------------------------------------------------+
//| Sort summary                                                     |
//+------------------------------------------------------------------+
int32_t CMarginCall::SortSummary(const void *left,const void *right)
  {
   TableRecord *lft=(TableRecord*)left;
   TableRecord *rgh=(TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
  