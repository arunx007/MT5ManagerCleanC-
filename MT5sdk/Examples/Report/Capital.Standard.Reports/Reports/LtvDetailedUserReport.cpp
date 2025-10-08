//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvDetailedUserReport.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CLtvDetailedUser::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"Lifetime Value Detailed by Trading Account",  // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_TABLE,                       // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,           DEFAULT_GROUPS             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                           },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_SOURCE,      DEFAULT_LEAD_SOURCE        },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_CAMPAIGN,    DEFAULT_LEAD_CAMPAIGN      },
     },5,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,           DEFAULT_CURRENCY           },
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },2                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn CLtvDetailedUser::s_columns[]=
  {
   //--- id,                  name,                   type,                             width,width_max, offset,                           size,                              digits_column,          flags
     { COLUMN_LOGIN,            L"Login",               IMTDatasetColumn::TYPE_USER_LOGIN,10,   0, offsetof(TableRecord,login),              0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_NAME,             L"Name",                IMTDatasetColumn::TYPE_STRING,    20,   0, offsetof(TableRecord,name),               MtFieldSize(TableRecord,name),     0,                      IMTDatasetColumn::FLAG_LEFT    },
   { COLUMN_REGISTRATION_TIME,L"Registration Time",   IMTDatasetColumn::TYPE_DATETIME,  16, 150, offsetof(TableRecord,registration_time),  0,                                 0,                      0                              },
   { COLUMN_FTD_TIME,         L"First Deposit Time",  IMTDatasetColumn::TYPE_DATETIME,  16, 150, offsetof(TableRecord,deposit_first_time), 0,                                 0,                      0                              },
   { COLUMN_DEPOSIT_COUNT,    L"Deposits",            IMTDatasetColumn::TYPE_UINT32,     6,   0, offsetof(TableRecord,deposit_count),      0,                                 0,                      0                              },
   { COLUMN_FTD,              L"First Deposit",       IMTDatasetColumn::TYPE_MONEY,     10,   0, offsetof(TableRecord,deposit_first),      0,                                 COLUMN_CURRENCY_DIGITS, 0                              },
   { COLUMN_LTV,              L"Lifetime Value",      IMTDatasetColumn::TYPE_MONEY,     10,   0, offsetof(TableRecord,deposit_amount),     0,                                 COLUMN_CURRENCY_DIGITS, 0                              },
   { COLUMN_CURRENCY,         L"Currency",            IMTDatasetColumn::TYPE_STRING,     8,  70, offsetof(TableRecord,currency),           MtFieldSize(TableRecord,currency), 0,                      0                              },
   { COLUMN_CURRENCY_DIGITS,  L"CurrencyDigits",      IMTDatasetColumn::TYPE_UINT32,     0,   0, offsetof(TableRecord,currency_digits),    0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN  },
   { COLUMN_FTD_CURRENCY,     L"FTD",                 IMTDatasetColumn::TYPE_MONEY,     10,   0, offsetof(TableRecord,ftd_currency),       0,                                 0,                      0                              },
   { COLUMN_LTV_CURRENCY,     L"LTV",                 IMTDatasetColumn::TYPE_MONEY,     10,   0, offsetof(TableRecord,ltv_currency),       0,                                 0,                      0                              },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvDetailedUser::CLtvDetailedUser(void) : CCurrencyReport(s_info),m_users(NULL),m_deposits(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvDetailedUser::~CLtvDetailedUser(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CLtvDetailedUser::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CLtvDetailedUser::Create(void)
  {
   return(new (std::nothrow) CLtvDetailedUser());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CLtvDetailedUser::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=NULL;
     }
//--- release deposits
   if(m_deposits)
     {
      m_deposits->Release();
      m_deposits=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Get parameter currency                                           |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedUser::Prepare(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- user cache
   MTAPIRES res=MT_RET_OK;
   m_users=CUserCache::Create(*m_api,m_params,&m_currency,false,res);
   if(!m_users)
      return(res);
//--- deposit cache
   m_deposits=CDepositCache::Create(*m_api,res);
   if(!m_deposits)
      return(res);
//--- create table columns
   if((res=PrepareTable())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| add table columns                                                |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedUser::PrepareTable(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create column
   IMTDatasetColumn *column=m_api->TableColumnCreate();
   if(!column)
      return(MT_RET_ERR_MEM);
//--- iterate columns
   MTAPIRES res;
   for(uint32_t i=0;i<_countof(s_columns);i++)
     {
      //--- fill report data column
      if((res=s_columns[i].ReportColumnFill(*column))!=MT_RET_OK)
         break;
      //--- setup report currency columns
      if(s_columns[i].id==COLUMN_LTV_CURRENCY || s_columns[i].id==COLUMN_FTD_CURRENCY)
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
MTAPIRES CLtvDetailedUser::Calculate(void)
  {
//--- checks
   if(!m_api || !m_users || !m_deposits)
      return(MT_RET_ERR_PARAMS);
//--- get range
   const int64_t from=m_api->ParamFrom();
   const int64_t to  =m_api->ParamTo();
//--- check range
   if(to<from)
      return(MT_RET_ERR_PARAMS);
//--- update users
   MTAPIRES res=m_users->UpdateFilter(from,to);
   if(res!=MT_RET_OK)
      return(res);
//--- update deposits
   if((res=m_deposits->Update(m_users->Logins(),from,CReportCache::Now(*m_api,m_params)))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write result                                                     |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedUser::WriteResult(void)
  {
//--- checks
   if(!m_api || !m_users || !m_deposits)
      return(MT_RET_ERR_PARAMS);
//--- save current time
   const CReportTimer timer_wait;
//--- begin read deposits
   MTAPIRES res=m_deposits->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- save current time
   const uint64_t wait_time_ms=timer_wait.Elapsed();
   const CReportTimer timer_write;
//--- read users
   res=m_users->ReadAll(*this);
//--- end read deposits
   const MTAPIRES res_deposits=m_deposits->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
     {
      m_api->LoggerOut(MTLogErr,L"Writing failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- check error
   if((res=res_deposits)!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Writing failed, deposit cache end read error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- write statistics to log
   return(m_api->LoggerOut(MTLogOK,L"Written %u users of %u in %I64u ms (cache read wait %I64u ms)",m_api->TableRowTotal(),m_users->Logins().Total(),timer_write.Elapsed(),wait_time_ms));
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedUser::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_api || !m_deposits)
      return(MT_RET_ERR_PARAMS);
//--- fill record by user
   TableRecord record;
   record.login=login;
   record.registration_time=user.registration;
//--- fill user name
   LPCWSTR string=nullptr;
   MTAPIRES res=m_users->ReadName(user,string);
   if(res!=MT_RET_OK)
      return(res);
   CMTStr::Copy(record.name,string);
//--- fill user currency
   if((res=m_users->ReadCurrency(user,string))!=MT_RET_OK)
      return(res);
   CMTStr::Copy(record.currency,string);
//--- get user deposit
   const CDepositCache::UserDeposit *deposit=nullptr;
   res=m_deposits->ReadDeposit(record.login,deposit);
   if(res==MT_RET_OK && deposit)
     {
      //--- fill record by deposit
      record.deposit_first_time  =deposit->first_time;
      record.deposit_count       =deposit->count;
      record.deposit_first       =deposit->first;
      record.deposit_amount      =deposit->amount;
      record.currency_digits     =deposit->currency_digits;
      //--- currency conversion
      if(m_currency.NeedConversion(user.currency))
        {
         //--- get currency conversion rate
         double rate=0.0;
         if((res=m_currency.CurrencyRate(rate,user.currency))!=MT_RET_OK)
            return(res);
         //--- convert
         record.ftd_currency     =m_currency.Convert(deposit->first,rate);
         record.ltv_currency     =m_currency.Convert(deposit->amount,rate);
        }
      else
        {
         //--- nothing to convert
         record.ftd_currency     =deposit->first;
         record.ltv_currency     =deposit->amount;
        }
     }
   else
      if(res==MT_RET_OK_NONE)
         return(MT_RET_OK);
      else
         return(res);
//--- write data row
   return(m_api->TableRowWrite(&record,sizeof(record)));
  }
//+------------------------------------------------------------------+
