//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ClientBalanceCache.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientBalanceCacheBase::CClientBalanceCacheBase(CCurrencyConverterCache &currency) :
   m_api(nullptr),m_users(nullptr),m_balances(nullptr),m_clients(nullptr),m_now(0),m_currency(currency)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientBalanceCacheBase::~CClientBalanceCacheBase(void)
  {
//--- clear
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CClientBalanceCacheBase::Clear(void)
  {
//--- clear api pointer
   m_api=nullptr;
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=nullptr;
     }
//--- release balances
   if(m_balances)
     {
      m_balances->Release();
      m_balances=nullptr;
     }
//--- release clients
   if(m_clients)
     {
      m_clients->Release();
      m_clients=nullptr;
     }
//--- clear filter
   m_client_filter.Clear();
//--- zero report generation time
   m_now=0;
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCacheBase::Initialize(IMTReportAPI &api,CReportParameter &params)
  {
//--- store API interface
   m_api=&api;
//--- user cache
   MTAPIRES res=MT_RET_OK;
   m_users=CUserCache::Create(api,params,&m_currency,true,res);
   if(!m_users)
      return(res);
//--- balance cache
   m_balances=CBalanceCache::Create(api,res);
   if(!m_balances)
      return(res);
//--- client cache
   m_clients=CClientCache::Create(api,res);
   if(!m_clients)
      return(res);
//--- report generation time
   m_now=CReportCache::Now(api,params);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Read client dictionary string                                    |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCacheBase::ReadDictionaryString(const uint32_t dictionary_id,const uint32_t pos,LPCWSTR &string) const
  {
//--- checks
   if(!m_clients)
      return(MT_RET_ERR_PARAMS);
//--- read client dictionary string
   return(m_clients->ReadDictionaryString(dictionary_id,pos,string));
  }
//+------------------------------------------------------------------+
//| Read top names                                                   |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCacheBase::ReadTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names)
  {
//--- check clients
   if(!m_clients)
      return(MT_RET_ERR_PARAMS);
//--- top count
   if(!top.Total())
      return(MT_RET_OK);
//--- begin read clients
   MTAPIRES res=m_clients->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- read clients top names
   res=m_clients->ReadTopNames(dictionary_id,top,names);
//--- end read clients
   const MTAPIRES res_clients=m_clients->ReadEnd();
//--- check error
   if(res!=MT_RET_OK)
      return(res);
//--- return error code
   return(res_clients);
  }
//+------------------------------------------------------------------+
//| Read client balance                                              |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCacheBase::Read(const IMTReportCacheKeySet &logins)
  {
//--- checks
   if(!m_api || !m_users || !m_balances || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- save current time
   const CReportTimer timer;
//--- begin read balances
   MTAPIRES res=m_balances->ReadBegin();
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache begin read balances failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- begin read clients
   if((res=m_clients->ReadBegin())!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache begin read clients failed, error: %s",SMTFormat::FormatError(res));
      m_balances->ReadEnd();
      return(res);
     }
//--- initialize clients filter
   res=m_client_filter.Initialize(*m_api,*m_clients);
//--- read users
   if(res==MT_RET_OK)
      res=m_users->Read(logins,*this,&CClientBalanceCacheBase::UserRead);
//--- end read caches
   const MTAPIRES res_balances=m_balances->ReadEnd();
   const MTAPIRES res_clients=m_clients->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache read failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- check error
   if(res_balances!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache end read balances failed, error: %s",SMTFormat::FormatError(res_balances));
      return(res_balances);
     }
   if(res_clients!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache end read clients failed, error: %s",SMTFormat::FormatError(res_clients));
      return(res_clients);
     }
//--- write statistics to log
   return(m_api->LoggerOut(MTLogOK,L"Client Balance Cache read %u logins in %I64u ms",m_users->Logins().Total(),timer.Elapsed()));
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCacheBase::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_balances || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- check client id
   if(!user.client_id)
      return(MT_RET_OK);
//--- get user balance
   CUserBalance balance;
   const MTAPIRES res=m_balances->ReadBalance(login,balance);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
//--- calculate user balance 
   return(UserBalanceRead(user,balance));
  }
//+------------------------------------------------------------------+
//| User balance read handler                                        |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCacheBase::UserBalanceRead(const CUserCache::UserCache &user,const CUserBalance &balance)
  {
//--- checks
   if(!m_clients)
      return(MT_RET_ERR_PARAMS);
//--- check user balance
   if(!balance.User())
      return(MT_RET_OK);
//--- read client
   const ClientCache *client=nullptr;
   MTAPIRES res=m_clients->ReadClient(user.client_id,client);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
   if(!client)
      return(MT_RET_OK);
//--- filter client
   if(!m_client_filter.ClientMatch(*client))
      return(UserClientFilteredOut(user.client_id));
//--- currency conversion
   double rate=0.0;
   const bool convert=m_currency.NeedConversion(user.currency);
   if(convert)
     {
      //--- get currency conversion rate
      MTAPIRES res=m_currency.CurrencyRate(rate,user.currency);
      if(res!=MT_RET_OK)
         return(res);
     }
//--- call user and client balance read handler
   if((res=UserClientBalanceRead(user,*client,balance,convert ? &rate : nullptr))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientBalanceCache::CClientBalanceCache(CCurrencyConverterCache &currency) :
   CClientBalanceCacheBase(currency),m_user_clients(nullptr),m_logins(nullptr),m_receiver(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientBalanceCache::~CClientBalanceCache(void)
  {
//--- clear
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CClientBalanceCache::Clear(void)
  {
//--- base call
   CClientBalanceCacheBase::Clear();
//--- delete user clients filter
   if(m_user_clients)
     {
      delete m_user_clients;
      m_user_clients=nullptr;
     }
//--- clear user logins pointer
   m_logins=nullptr;
//--- clear receiver pointer
   m_receiver=nullptr;
  }
//+------------------------------------------------------------------+
//| Update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCache::Update(const bool until_now)
  {
//--- checks
   if(!m_api || !m_users || !m_balances || !m_clients)
      return(MT_RET_ERR_PARAMS);
//--- get range
   const int64_t from=m_api->ParamFrom();
   const int64_t to  =m_api->ParamTo();
   const int64_t now =until_now ? m_now : to;
//--- check range
   if(to<from || now<from)
      return(MT_RET_ERR_PARAMS);
//--- update users
   MTAPIRES res=m_users->Update(0,now);
   if(res!=MT_RET_OK)
      return(res);
//--- create user clients filter
   m_user_clients=new(std::nothrow) CUserFilterWithClients(*m_api,*m_users,0,now);
   if(!m_user_clients)
      return(MT_RET_ERR_MEM);
//--- filter users with clients
   if((res=m_users->ReadAll(*m_user_clients))!=MT_RET_OK)
      return(res);
//--- user logins
   m_logins=m_user_clients->Logins();
   if(!m_logins)
      return(MT_RET_OK_NONE);
//--- client ids
   const IMTReportCacheKeySet *clients=m_user_clients->Clients();
   if(!clients)
      return(MT_RET_OK_NONE);
//--- update deposits
   if((res=m_balances->Update(*m_logins,from,now))!=MT_RET_OK)
      return(res);
//--- update clients
   if((res=m_clients->Update(*clients))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Read client balance                                              |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCache::Read(CClientBalanceReceiver *receiver)
  {
//--- checks
   if(!receiver || !m_api || !m_users || !m_balances || !m_clients || !m_logins)
      return(MT_RET_ERR_PARAMS);
//--- store receiver
   m_receiver=receiver;
//--- base call
   return(CClientBalanceCacheBase::Read(*m_logins));
  }
//+------------------------------------------------------------------+
//| user and client balance read handler overridable                 |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceCache::UserClientBalanceRead(const CUserCache::UserCache &user,const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- checks
   if(!m_receiver)
      return(MT_RET_ERR_PARAMS);
//--- call client balance read handler
   return(m_receiver->ClientBalanceRead(client,balance,rate));
  }
//+------------------------------------------------------------------+
//| client balance read handler overridable                          |
//+------------------------------------------------------------------+
MTAPIRES CClientBalanceReceiver::ClientBalanceRead(const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- read balance deals
   for(uint32_t i=0;const DealBalance *deal_balance=balance.Deal(i);i++)
     {
      //--- call client balance deal read handler
      const MTAPIRES res=ClientBalanceDealRead(client,*deal_balance,rate);
      if(res!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+