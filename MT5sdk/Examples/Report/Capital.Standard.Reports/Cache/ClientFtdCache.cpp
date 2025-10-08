//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ClientFtdCache.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientFtdCache::CClientFtdCache(CCurrencyConverterCache &currency) :
   m_api(nullptr),m_users(nullptr),m_client_users(nullptr),m_balances(nullptr),m_clients(nullptr),m_ids(nullptr),
   m_from(0),m_to(0),m_clients_from(0),m_currency(currency),m_receiver(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientFtdCache::~CClientFtdCache(void)
  {
//--- clear
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CClientFtdCache::Clear(void)
  {
//--- clear api pointer
   m_api=nullptr;
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=nullptr;
     }
//--- release client by user cache
   if(m_client_users)
     {
      m_client_users->Release();
      m_client_users=nullptr;
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
//--- release ids
   if(m_ids)
     {
      m_ids->Release();
      m_ids=nullptr;
     }
//--- clear filter
   m_client_filter.Clear();
//--- clear client ftd array
   m_ftds.Clear();
//--- zero fields
   m_from=0;
   m_to=0;
   m_clients_from=0;
   m_receiver=nullptr;
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::Initialize(IMTReportAPI &api,CReportParameter &params)
  {
//--- store API interface
   m_api=&api;
//--- user cache
   MTAPIRES res=MT_RET_OK;
   m_users=CUserCache::Create(api,params,&m_currency,true,res);
   if(!m_users)
      return(res);
//--- client by user cache
   m_client_users=CClientUserCache::Create(api,*m_users,res);
   if(!m_client_users)
      return(res);
//--- balance cache
   m_balances=CBalanceCache::Create(api,res);
   if(!m_balances)
      return(res);
//--- client cache
   m_clients=CClientCache::Create(api,res);
   if(!m_clients)
      return(res);
//--- create client ids keyset
   m_ids=api.KeySetCreate();
   if(!m_ids)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::Update(const int64_t from,const int64_t to)
  {
//--- checks
   if(!m_api || !m_users || !m_client_users || !m_balances || !m_clients || !m_ids || to<from)
      return(MT_RET_ERR_PARAMS);
//--- update users
   MTAPIRES res=m_users->Update(0,to);
   if(res!=MT_RET_OK)
      return(res);
//--- update clients by users
   if((res=m_client_users->Update(0,to))!=MT_RET_OK)
      return(res);
//--- update deposits
   if((res=m_balances->Update(m_client_users->Logins(),from,to))!=MT_RET_OK)
      return(res);
//--- read balance clients
   if((res=ReadBalanceClients(from,to))!=MT_RET_OK)
      return(res);
//--- read first client registration time
   if((res=ReadClientFrom(from,to))!=MT_RET_OK)
      return(res);
//--- update deposits
   if(m_clients_from<from)
      if((res=m_balances->Update(m_client_users->Logins(),m_clients_from,to))!=MT_RET_OK)
         return(res);
//--- calculate ftd
   if((res=CalculateFtd())!=MT_RET_OK)
      return(res);
//--- update clients
   if((res=m_clients->Update(*m_ids))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Begin read clients                                               |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ClientsReadBegin(const CReportCache *&cache)
  {
//--- check cache
   if(!m_clients)
      return(MT_RET_ERR_PARAMS);
//--- return cache
   cache=m_clients;
//--- begin read
   return(m_clients->ReadBegin());
  }
//+------------------------------------------------------------------+
//| End read clients                                                 |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ClientsReadEnd(void)
  {
//--- check cache
   if(!m_clients)
      return(MT_RET_ERR_PARAMS);
//--- end read
   return(m_clients->ReadEnd());
  }
//+------------------------------------------------------------------+
//| read client ftd                                                  |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::Read(IClientFtdReceiver *receiver)
  {
//--- checks
   if(!receiver || !m_clients || !m_ids)
      return(MT_RET_ERR_PARAMS);
//--- check empty
   if(!m_ftds.Total())
      return(MT_RET_OK_NONE);
//--- begin read clients
   MTAPIRES res=m_clients->ReadBegin();
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Ftd Cache begin read clients failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- store receiver
   m_receiver=receiver;
//--- initialize clients filter
   res=m_client_filter.Initialize(*m_api,*m_clients);
//--- read clients
   if(res==MT_RET_OK)
      res=m_clients->Read(*m_ids,*this,&CClientFtdCache::ClientRead);
//--- end read clients
   const MTAPIRES res_client=m_clients->ReadEnd();
//--- check read error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- check error
   if(res_client!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Ftd Cache begin read clients failed, error: %s",SMTFormat::FormatError(res_client));
      return(res_client);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| read balance clients                                             |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ReadBalanceClients(const int64_t from,const int64_t to)
  {
//--- checks
   if(!m_users || !m_client_users || !m_balances || !m_ids || from<0 || to<0)
      return(MT_RET_ERR_PARAMS);
//--- store limits
   m_from=(uint32_t)(from/SECONDS_IN_DAY);
   m_to=(uint32_t)(to/SECONDS_IN_DAY);
//--- begin read balances
   MTAPIRES res=m_balances->ReadBegin();
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Ftd begin read balance failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- read user balances
   res=m_users->Read(m_client_users->Logins(),*this,&CClientFtdCache::UserRead);
//--- end read balances
   const MTAPIRES res_balance=m_balances->ReadEnd();
//--- check read error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- check error
   if(res_balance!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Ftd Cache begin read balance failed, error: %s",SMTFormat::FormatError(res_balance));
      return(res_balance);
     }
//--- logging
   m_api->LoggerOut(MTLogOK,L"Client Ftd Cache balances read for %u clients",m_ids->Total());
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| read first client registration time                              |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ReadClientFrom(const int64_t from,const int64_t to)
  {
//--- checks
   if(!m_client_users || !m_ids || to<from)
      return(MT_RET_ERR_PARAMS);
//--- initialize time
   m_clients_from=from;
//--- read client users
   MTAPIRES res=m_client_users->Read(*m_ids,*this,&CClientFtdCache::ClientUserRead);
   if(res!=MT_RET_OK)
      return(res);
//--- check time
   if(m_clients_from>to)
      return(MT_RET_ERROR);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_balances)
      return(MT_RET_ERR_PARAMS);
//--- check client id
   if(!user.client_id)
      return(MT_RET_OK);
//--- get user balance
   CBalanceCache::CUserBalance balance;
   const MTAPIRES res=m_balances->ReadBalance(login,balance);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
//--- calculate user balance 
   return(UserBalanceRead(login,user,balance));
  }
//+------------------------------------------------------------------+
//| User balance read handler                                        |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::UserBalanceRead(const uint64_t login,const CUserCache::UserCache &user,const CBalanceCache::CUserBalance &balance)
  {
//--- check
   if(!m_ids)
      return(MT_RET_ERR_PARAMS);
//--- check user balance
   if(!balance.User())
      return(MT_RET_OK);
//--- search for depsit and store client id
   for(uint32_t i=0;const CBalanceCache::DealBalance *deal=balance.Deal(i);i++)
      if(deal->info.day>=m_from && deal->info.day<=m_to && deal->info.deposit)
         return(m_ids->Insert(user.client_id));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Client user read handler                                         |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ClientUserRead(const uint64_t id,const CClientUserCache::ClientUserCache &client)
  {
//--- minimize registration date
   if(m_clients_from>client.registration)
      m_clients_from=client.registration;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| calculate ftd                                                    |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::CalculateFtd(void)
  {
//--- checks
   if(!m_users || !m_client_users || !m_balances || !m_ids)
      return(MT_RET_ERR_PARAMS);
//--- initialize ftd
   MTAPIRES res=InitializeFtd();
   if(res!=MT_RET_OK)
      return(res);
//--- clear client ids
   m_ids->Clear();
//--- clients total
   const uint32_t total=m_ftds.Total();
   if(!total)
      return(MT_RET_OK_NONE);
//--- read balance clients ftd
   if((res=ReadBalanceClientsFtd())!=MT_RET_OK)
      return(res);
//--- client ftd buffer
   const ClientFtd *ftd=(const ClientFtd *)m_ftds.At(0);
   if(!ftd)
      return(MT_RET_ERROR);
//--- insert clients with ftd in time range
   for(const ClientFtd *end=ftd+total;ftd!=end;ftd++)
      if(ftd->day>=m_from && ftd->day<=m_to && ftd->value>DBL_EPSILON)
         if((res=m_ids->Insert(ftd->id))!=MT_RET_OK)
            return(res);
//--- logging
   m_api->LoggerOut(MTLogOK,L"Client Ftd Cache calculated for %u clients",m_ids->Total());
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| initialize ftd                                                   |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::InitializeFtd(void)
  {
//--- check
   if(!m_ids)
      return(MT_RET_ERR_PARAMS);
//--- clear ftds
   m_ftds.Clear();
//--- clients total
   const uint32_t total=m_ids->Total();
   if(!total)
      return(MT_RET_OK_NONE);
//--- clients ids
   const uint64_t *ids=m_ids->Array();
   if(!ids)
      return(MT_RET_ERROR);
//--- resize money
   if(!m_ftds.Reserve(total))
      return(MT_RET_ERR_MEM);
//--- zero money
   if(!m_ftds.Zero())
      return(MT_RET_ERROR);
//--- fill array with ids
   for(const uint64_t *end=ids+total;ids!=end;ids++)
      if(ClientFtd *ftd=m_ftds.Append())
         ftd->id=*ids;
      else
         return(MT_RET_ERROR);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| read balance clients ftd                                         |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ReadBalanceClientsFtd(void)
  {
//--- checks
   if(!m_users || !m_client_users || !m_balances || !m_ids)
      return(MT_RET_ERR_PARAMS);
//--- begin read balances
   MTAPIRES res=m_balances->ReadBegin();
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Ftd begin read balance failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- read user balances
   res=m_users->Read(m_client_users->Logins(),*this,&CClientFtdCache::UserFtdRead);
//--- end read balances
   const MTAPIRES res_balance=m_balances->ReadEnd();
//--- check read error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- check error
   if(res_balance!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Ftd Cache begin read balance failed, error: %s",SMTFormat::FormatError(res_balance));
      return(res_balance);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| user ftd read handler                                            |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::UserFtdRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_balances)
      return(MT_RET_ERR_PARAMS);
//--- check client id
   if(!user.client_id)
      return(MT_RET_OK);
//--- search for client ftd
   ClientFtd *ftd=m_ftds.Search(&user.client_id,SearchFtdClient);
   if(!ftd)
      return(MT_RET_OK);
//--- get user balance
   CBalanceCache::CUserBalance balance;
   const MTAPIRES res=m_balances->ReadBalance(login,balance);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
//--- calculate user ftd
   return(UserBalanceFtdRead(user,balance,*ftd));
  }
//+------------------------------------------------------------------+
//| User ftd balance read handler                                    |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::UserBalanceFtdRead(const CUserCache::UserCache &user,const CBalanceCache::CUserBalance &balance,ClientFtd &ftd)
  {
//--- check user balance
   if(!balance.User())
      return(MT_RET_OK);
//--- read balance deals
   bool updated=false;
   for(uint32_t i=0;const CBalanceCache::DealBalance *deal=balance.Deal(i);i++)
      if(deal->info.deposit)
        {
         //--- update first deposit
         if(!ftd.day || ftd.day>deal->info.day)
           {
            ftd.day    =deal->info.day;
            ftd.value  =deal->value;
            updated=true;
           }
        }
//--- currency conversion
   if(updated && m_currency.NeedConversion(user.currency))
     {
      //--- get currency conversion rate
      double rate=0.0;
      const MTAPIRES res=m_currency.CurrencyRate(rate,user.currency);
      if(res!=MT_RET_OK)
         return(res);
      //--- convert currency
      ftd.value=m_currency.Convert(ftd.value,rate);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Client read handler                                              |
//+------------------------------------------------------------------+
MTAPIRES CClientFtdCache::ClientRead(const uint64_t id,const CClientCache::ClientCache &client)
  {
//--- check
   if(!m_receiver)
      return(MT_RET_ERR_PARAMS);
//--- filter client
   if(!m_client_filter.ClientMatch(client))
      return(MT_RET_OK);
//--- search for client ftd
   ClientFtd *ftd=m_ftds.Search(&id,SearchFtdClient);
   if(!ftd)
      return(MT_RET_ERROR);
//--- call receiver handler
   return(m_receiver->ClientFtdRead(client,ftd->day*SECONDS_IN_DAY,ftd->value));
  }
//+------------------------------------------------------------------+
//| search static method                                             |
//+------------------------------------------------------------------+
int32_t CClientFtdCache::SearchFtdClient(const void *left,const void *right)
  {
//--- types conversion
   const uint64_t     lft=*(const uint64_t*)left;
   const ClientFtd *rgh=(const ClientFtd*)right;
//--- compare client ids
   if(lft<rgh->id)
      return(-1);
   if(lft>rgh->id)
      return(1);
   return(0);
  }
//+------------------------------------------------------------------+