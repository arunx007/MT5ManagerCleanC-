//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvDetailedClientReport.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CLtvDetailedClient::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"Lifetime Value Detailed by Client",           // report name
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
ReportColumn CLtvDetailedClient::s_columns[]=
  {
   //--- id,                  name,                   type,                             width,width_max, offset,                           size,                              digits_column,          flags
     { COLUMN_ID,               L"Client Id",           IMTDatasetColumn::TYPE_CLIENT_ID, 10,   0, offsetof(TableRecord,id),                 0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_NAME,             L"Name",                IMTDatasetColumn::TYPE_STRING,    20,   0, offsetof(TableRecord,name),               MtFieldSize(TableRecord,name),     0,                      IMTDatasetColumn::FLAG_LEFT    },
   { COLUMN_COUNTRY,          L"Country",             IMTDatasetColumn::TYPE_STRING,    15,   0, offsetof(TableRecord,country),            MtFieldSize(TableRecord,country),  0,                      0                              },
   { COLUMN_REGISTRATION,     L"Registration Time",   IMTDatasetColumn::TYPE_DATETIME,  16, 150, offsetof(TableRecord,registration),       0,                                 0,                      0                              },
   { COLUMN_FTD_TIME,         L"First Deposit Time",  IMTDatasetColumn::TYPE_DATETIME,  16, 150, offsetof(TableRecord,deposit_first_time), 0,                                 0,                      0                              },
   { COLUMN_USER_COUNT,       L"Trading Accounts",    IMTDatasetColumn::TYPE_UINT32,     8,   0, offsetof(TableRecord,user_count),         0,                                 0,                      0                              },
   { COLUMN_DEPOSIT_COUNT,    L"Deposits",            IMTDatasetColumn::TYPE_UINT32,     6,   0, offsetof(TableRecord,deposit_count),      0,                                 0,                      0                              },
   { COLUMN_FTD,              L"First Deposit",       IMTDatasetColumn::TYPE_MONEY,     10,   0, offsetof(TableRecord,deposit_first),      0,                                 0,                      0                              },
   { COLUMN_LTV,              L"Lifetime Value",      IMTDatasetColumn::TYPE_MONEY,     10,   0, offsetof(TableRecord,deposit_amount),     0,                                 0,                      0                              },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvDetailedClient::CLtvDetailedClient(void) :
   CCurrencyReport(s_info),m_users(NULL),m_client_users(NULL),m_deposits(NULL),m_clients(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvDetailedClient::~CLtvDetailedClient(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CLtvDetailedClient::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CLtvDetailedClient::Create(void)
  {
   return(new (std::nothrow) CLtvDetailedClient());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CLtvDetailedClient::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=NULL;
     }
//--- release client by user cache
   if(m_client_users)
     {
      m_client_users->Release();
      m_client_users=NULL;
     }
//--- release deposits
   if(m_deposits)
     {
      m_deposits->Release();
      m_deposits=NULL;
     }
//--- release clients
   if(m_clients)
     {
      m_clients->Release();
      m_clients=NULL;
     }
//--- clear objects
   m_moneys.Clear();
   m_filter.Clear();
  }
//+------------------------------------------------------------------+
//| Get parameter currency                                           |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::Prepare(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- user cache
   MTAPIRES res=MT_RET_OK;
   m_users=CUserCache::Create(*m_api,m_params,&m_currency,true,res);
   if(!m_users)
      return(res);
//--- client by user cache
   m_client_users=CClientUserCache::Create(*m_api,*m_users,res);
   if(!m_client_users)
      return(res);
//--- deposit cache
   m_deposits=CDepositCache::Create(*m_api,res);
   if(!m_deposits)
      return(res);
//--- client cache
   m_clients=CClientCache::Create(*m_api,res);
   if(!m_clients)
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
MTAPIRES CLtvDetailedClient::PrepareTable(void)
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
MTAPIRES CLtvDetailedClient::Calculate(void)
  {
//--- checks
   if(!m_api || !m_users || !m_client_users || !m_deposits || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- get range
   const int64_t from=m_api->ParamFrom();
   const int64_t to  =m_api->ParamTo();
   const int64_t now =CReportCache::Now(*m_api,m_params);
//--- check range
   if(to<from || now<from)
      return(MT_RET_ERR_PARAMS);
//--- update users
   MTAPIRES res=m_users->Update(0,now);
   if(res!=MT_RET_OK)
      return(res);
//--- update clients by users
   if((res=m_client_users->Update(from,to))!=MT_RET_OK)
      return(res);
//--- update deposits
   if((res=m_deposits->Update(m_client_users->Logins(),from,now))!=MT_RET_OK)
      return(res);
//--- update clients
   if((res=m_clients->Update(m_client_users->Ids()))!=MT_RET_OK)
      return(res);
//--- calculate money
   if((res=CalculateMoney())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate money                                                  |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::CalculateMoney(void)
  {
//--- checks
   if(!m_api || !m_users || !m_client_users || !m_deposits || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- initialize money
   MTAPIRES res=InitializeMoney(m_client_users->Ids());
   if(res!=MT_RET_OK)
      return(res);
//--- save current time
   const CReportTimer timer_wait;
//--- begin read deposits
   if((res=m_deposits->ReadBegin())!=MT_RET_OK)
      return(res);
//--- save current time
   const uint64_t wait_time_ms=timer_wait.Elapsed();
   const CReportTimer timer_write;
//--- read users
   res=m_users->Read(m_client_users->Logins(),*this,&CLtvDetailedClient::UserRead);
//--- end read deposits
   const MTAPIRES res_deposits=m_deposits->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
     {
      m_api->LoggerOut(MTLogErr,L"Calculation failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- check error
   if((res=res_deposits)!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Calculation failed, deposit cache end read error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- write statistics to log
   return(m_api->LoggerOut(MTLogOK,L"Calculated %u clients of %u users in %I64u ms (cache read wait %I64u ms)",m_moneys.Total(),m_users->Logins().Total(),timer_write.Elapsed(),wait_time_ms));
  }
//+------------------------------------------------------------------+
//| Initialize money                                                 |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::InitializeMoney(const IMTReportCacheKeySet &clients)
  {
//--- clear money
   m_moneys.Clear();
//--- clients total
   const uint32_t total=clients.Total();
   if(!total)
      return(MT_RET_ERR_PARAMS);
//--- resize money
   if(!m_moneys.Reserve(total))
      return(MT_RET_ERR_MEM);
//--- zero money
   if(!m_moneys.Zero())
      return(MT_RET_ERROR);
//--- read clients
   MTAPIRES res=m_client_users->Read(clients,*this,&CLtvDetailedClient::ClientUserRead);
   if(res!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| client read handler                                              |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::ClientUserRead(const uint64_t id,const CClientUserCache::ClientUserCache &client)
  {
//--- append client money
   ClientMoney *money=m_moneys.Append();
   if(!money)
      return(MT_RET_ERROR);
//--- initialize client money
   money->client_id     =id;
   money->registration  =client.registration;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_api || !m_deposits)
      return(MT_RET_ERR_PARAMS);
//--- get user deposit
   const CDepositCache::UserDeposit *deposit=nullptr;
   MTAPIRES res=m_deposits->ReadDeposit(login,deposit);
   if((res==MT_RET_OK && !deposit) || res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
   if(!deposit)
      return(MT_RET_ERROR);
//--- currency conversion
   double first=deposit->first;
   double amount=deposit->amount;
//--- get currency conversion needed
   if(m_currency.NeedConversion(user.currency))
     {
      //--- get currency conversion rate
      double rate=0.0;
      if((res=m_currency.CurrencyRate(rate,user.currency))!=MT_RET_OK)
         return(res);
      //--- convert
      first =m_currency.Convert(first,rate);
      amount=m_currency.Convert(amount,rate);
     }
//--- search for client money
   ClientMoney *money=m_moneys.Search(&user.client_id,SearchMoneyClient);
   if(!money)
      return(MT_RET_ERROR);
//--- update first deposit in record
   if(!money->deposit_first_time || money->deposit_first_time>deposit->first_time)
     {
      money->deposit_first_time =deposit->first_time;
      money->deposit_first      =first;
     }
//--- add deposit to record
   money->user_count++;
   money->deposit_count        +=deposit->count;
   money->deposit_amount       +=amount;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write result                                                     |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::WriteResult(void)
  {
//--- checks
   if(!m_api || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- check client count
   const uint32_t total=m_moneys.Total();
   if(!total)
      return(MT_RET_OK_NONE);
//--- save current time
   const CReportTimer timer_wait;
//--- begin read clients
   MTAPIRES res=m_clients->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- initialize filter
   res=m_filter.Initialize(*m_api,*m_clients);
//--- save current time
   const uint64_t wait_time_ms=timer_wait.Elapsed();
   const CReportTimer timer_write;
//--- iterate clients
   for(uint32_t pos=0,total=m_moneys.Total();pos<total && res==MT_RET_OK;pos++)
      res=WriteClient(m_moneys[pos]);
//--- end read clients
   const MTAPIRES res_clients=m_clients->ReadEnd();
//--- check error
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Writing failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- check error
   if((res=res_clients)!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Writing failed, client cache end read error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- write statistics to log
   return(m_api->LoggerOut(MTLogOK,L"Written %u clients of %u in %I64u ms (cache read wait %I64u ms)",m_api->TableRowTotal(),total,timer_write.Elapsed(),wait_time_ms));
  }
//+------------------------------------------------------------------+
//| Write client to result                                           |
//+------------------------------------------------------------------+
MTAPIRES CLtvDetailedClient::WriteClient(const ClientMoney &money)
  {
//--- checks
   if(!m_api || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- skip empty money
   if(!money.deposit_count)
      return(MT_RET_OK);
//--- read client
   const CClientCache::ClientCache *client=nullptr;
   MTAPIRES res=m_clients->ReadClient(money.client_id,client);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
   if(!client)
      return(MT_RET_OK);
//--- filter client
   if(!m_filter.ClientMatch(*client))
      return(MT_RET_OK);
//--- fill record by client
   TableRecord record;
   record.id                  =money.client_id;
   record.registration        =money.registration;
   record.deposit_first_time  =money.deposit_first_time;
   record.user_count          =money.user_count;
   record.deposit_count       =money.deposit_count;
   record.deposit_first       =money.deposit_first;
   record.deposit_amount      =money.deposit_amount;
//--- client name
   LPCWSTR string=nullptr;
   if((res=m_clients->ReadName(*client,string))!=MT_RET_OK)
      return(res);
   CMTStr::Copy(record.name   ,string);
//--- client country
   if((res=m_clients->ReadCountry(client->country,string))!=MT_RET_OK)
      return(res);
   CMTStr::Copy(record.country,string);
//--- write data row
   return(m_api->TableRowWrite(&record,sizeof(record)));
  }
//+------------------------------------------------------------------+
//| search static method                                             |
//+------------------------------------------------------------------+
int32_t CLtvDetailedClient::SearchMoneyClient(const void *left,const void *right)
  {
//--- types conversion
   const uint64_t       lft=*(uint64_t*)left;
   const ClientMoney *rgh=(ClientMoney*)right;
//--- compare client ids
   if(lft<rgh->client_id)
      return(-1);
   if(lft>rgh->client_id)
      return(1);
   return(0);
  }
//+------------------------------------------------------------------+
