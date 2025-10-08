//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ClientUserBalanceCache.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientUserBalanceCache::CClientUserBalanceCache(CCurrencyConverterCache &currency) :
   CClientBalanceCacheBase(currency),m_client_users(nullptr),m_active_ids(nullptr),m_receiver(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientUserBalanceCache::~CClientUserBalanceCache(void)
  {
//--- clear
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CClientUserBalanceCache::Clear(void)
  {
//--- base call
   CClientBalanceCacheBase::Clear();
//--- release client by user cache
   if(m_client_users)
     {
      m_client_users->Release();
      m_client_users=nullptr;
     }
//--- release keyset
   if(m_active_ids)
     {
      m_active_ids->Release();
      m_active_ids=nullptr;
     }
//--- clear receiver pointer
   m_receiver=nullptr;
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::Initialize(IMTReportAPI &api,CReportParameter &params)
  {
//--- base call
   MTAPIRES res=CClientBalanceCacheBase::Initialize(api,params);
   if(res!=MT_RET_OK)
      return(res);
//--- client by user cache
   m_client_users=CClientUserCache::Create(api,*m_users,res);
   if(!m_client_users)
      return(res);
//--- create keyset
   m_active_ids=m_api->KeySetCreate();
   if(!m_active_ids)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::Update(const bool until_now)
  {
//--- checks
   if(!m_api || !m_users || !m_client_users || !m_balances || !m_clients)
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
//--- update clients by users
   if((res=m_client_users->Update(from,to))!=MT_RET_OK)
      return(res);
//--- update deposits
   if((res=m_balances->Update(m_client_users->Logins(),from,now))!=MT_RET_OK)
      return(res);
//--- update clients
   if((res=m_clients->Update(m_client_users->Ids()))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Begin read clients                                               |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::ClientsReadBegin(const CReportCache *&cache)
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
MTAPIRES CClientUserBalanceCache::ClientsReadEnd(void)
  {
//--- check cache
   if(!m_clients)
      return(MT_RET_ERR_PARAMS);
//--- end read
   return(m_clients->ReadEnd());
  }
//+------------------------------------------------------------------+
//| Read client balance                                              |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::Read(CClientUserBalanceReceiver *receiver)
  {
//--- checks
   if(!receiver || !m_api || !m_users || !m_client_users || !m_balances || !m_clients || !m_active_ids)
      return(MT_RET_ERR_PARAMS);
//--- begin read user clients
   MTAPIRES res=m_client_users->ReadBegin();
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache begin read clients users failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- store receiver
   m_receiver=receiver;
//--- base call
   res=CClientBalanceCacheBase::Read(m_client_users->Logins());
//--- end read user clients
   const MTAPIRES res_client_users=m_client_users->ReadEnd();
   if(res_client_users!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"Client Balance Cache begin read clients users failed, error: %s",SMTFormat::FormatError(res_client_users));
      return(res_client_users);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Read client inactive                                             |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::ReadClientInactive(void)
  {
//--- checks
   if(!m_receiver || !m_client_users || !m_clients || !m_active_ids)
      return(MT_RET_ERR_PARAMS);
//--- read clients
   return(m_clients->Read(m_client_users->Ids(),*this,&CClientUserBalanceCache::ClientRead));
  }
//+------------------------------------------------------------------+
//| Read client user inactive                                        |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::ReadClientUserInactive(void)
  {
//--- checks
   if(!m_receiver || !m_client_users || !m_active_ids)
      return(MT_RET_ERR_PARAMS);
//--- read client users
   return(m_client_users->Read(m_client_users->Ids(),*this,&CClientUserBalanceCache::ClientUserRead));
  }
//+------------------------------------------------------------------+
//| user and client filtered out handler                             |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::UserClientFilteredOut(const uint64_t client_id)
  {
   if(!client_id || !m_active_ids)
      return(MT_RET_OK);
//--- insert client id to set of active clients
   return(m_active_ids->Insert(client_id));
  }
//+------------------------------------------------------------------+
//| user and client balance read handler                             |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::UserClientBalanceRead(const CUserCache::UserCache &user,const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- checks
   if(!m_receiver || !m_client_users || !m_active_ids)
      return(MT_RET_ERR_PARAMS);
//--- read client user
   const CClientUserCache::ClientUserCache *client_user=nullptr;
   MTAPIRES res=m_client_users->ReadClient(user.client_id,client_user);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
   if(!client_user)
      return(MT_RET_ERROR);
//--- call client and user balance read handler
   res=m_receiver->ClientUserBalanceRead(*client_user,client,balance,rate);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
//--- insert client id to set of active clients
   if(res==MT_RET_OK)
      res=m_active_ids->Insert(user.client_id);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| client read handler                                              |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::ClientRead(const uint64_t id,const ClientCache &client)
  {
//--- checks
   if(!m_receiver || !m_active_ids)
      return(MT_RET_ERR_PARAMS);
//--- check client activity
   if(m_active_ids->Search(id))
      return(MT_RET_OK);
//--- filter client
   if(!m_client_filter.ClientMatch(client))
      return(UserClientFilteredOut(id));
//--- call client inactive read handler
   return(m_receiver->ClientInactiveRead(client));
  }
//+------------------------------------------------------------------+
//| client user read handler                                         |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceCache::ClientUserRead(const uint64_t id,const CClientUserCache::ClientUserCache &client_user)
  {
//--- checks
   if(!m_receiver || !m_active_ids)
      return(MT_RET_ERR_PARAMS);
//--- check client activity
   if(m_active_ids->Search(id))
      return(MT_RET_OK);
//--- call client user inactive read handler
   return(m_receiver->ClientUserInactiveRead(client_user));
  }
//+------------------------------------------------------------------+
//| client balance read handler overridable                          |
//+------------------------------------------------------------------+
MTAPIRES CClientUserBalanceReceiver::ClientUserBalanceRead(const ClientUserCache &client_user,const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- read balance deals
   MTAPIRES res_read=MT_RET_OK_NONE;
   for(uint32_t i=0;const DealBalance *deal_balance=balance.Deal(i);i++)
     {
      //--- call client balance deal read handler
      const MTAPIRES res=ClientUserBalanceDealRead(client_user,client,*deal_balance,rate);
      if(res!=MT_RET_OK_NONE)
        {
         if(res!=MT_RET_OK)
            return(res);
         res_read=MT_RET_OK;
        }
     }
//--- ok
   return(res_read);
  }
//+------------------------------------------------------------------+