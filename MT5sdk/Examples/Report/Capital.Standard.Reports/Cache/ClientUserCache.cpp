//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ClientUserCache.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientUserCache::CClientUserCacheContext::CClientUserCacheContext(IMTReportAPI &api) :
   CReportCacheChildContext(api,L"Client_User_Cache",s_cache_version),
   m_ids(api.KeySetCreate()),m_logins(api.KeySetCreate())
  {
//--- check status
   if(m_res!=MT_RET_OK)
      return;
//--- check key set
   if(!m_ids || !m_logins)
      m_res=MT_RET_ERR_MEM;
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientUserCache::CClientUserCacheContext::~CClientUserCacheContext(void)
  {
//--- release interfaces
   if(m_ids)
     {
      m_ids->Release();
      m_ids=nullptr;
     }
   if(m_logins)
     {
      m_logins->Release();
      m_logins=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientUserCache::CClientUserCache(CClientUserCacheContext &context,CUserCache &users) :
   CReportCacheChild(context),m_ids(context.Detach(context.m_ids)),m_logins(context.Detach(context.m_logins)),m_users(users),
   m_keys_update(nullptr),m_filter_from(0),m_filter_to(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientUserCache::~CClientUserCache(void)
  {
   m_ids.Release();
   m_logins.Release();
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CClientUserCache* CClientUserCache::Create(IMTReportAPI &api,CUserCache &users,MTAPIRES &res)
  {
//--- create cache context
   CClientUserCacheContext context(api);
   if((res=context.Status())!=MT_RET_OK)
      return(nullptr);
//--- create user cache object
   CClientUserCache *client_users=new(std::nothrow) CClientUserCache(context,users);
   if(!client_users)
      res=MT_RET_ERR_MEM;
//--- return new user cache object
   return(client_users);
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CClientUserCache::Update(const int64_t from,const int64_t to)
  {
//--- filter users with clients
   CUserFilterWithClients user_clients(m_api,m_users,0,to);
   MTAPIRES res=m_users.ReadAll(user_clients);
   if(res!=MT_RET_OK)
      return(res);
//--- user logins
   const IMTReportCacheKeySet *logins=user_clients.Logins();
   if(!logins)
      return(MT_RET_OK_NONE);
//--- client ids
   const IMTReportCacheKeySet *clients=user_clients.Clients();
   if(!clients)
      return(MT_RET_OK_NONE);
//--- update cache
   if((res=CReportCacheChild::Update(*clients,*logins))!=MT_RET_OK)
      return(res);
//--- filter clients by registration time
   if((res=Filter(*clients,from,to))!=MT_RET_OK)
      return(res);
//--- filter user by clients
   CUserFilterByClients client_users(from,m_ids,m_logins);
   if((res=m_users.ReadAll(client_users))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CClientUserCache::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- check keys
   if(!m_keys_update)
      return(MT_RET_ERROR);
//--- search client id in keys
   if(!m_keys_update->Search(user.client_id))
      return(MT_RET_OK);
//--- read client
   const ClientUserCache *client=nullptr;
   MTAPIRES res=ReadClient(user.client_id,client);
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- check registration time
   if(client && client->registration<=user.registration)
      return(MT_RET_OK);
//--- new client
   ClientUserCache client_new;
   client_new.registration=user.registration;
//--- write new client
   return(m_cache.WriteValue(user.client_id,&client_new,sizeof(client_new)));
  }
//+------------------------------------------------------------------+
//| write cache keys                                                 |
//+------------------------------------------------------------------+
MTAPIRES CClientUserCache::WriteKeys(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent)
  {
//--- store keys
   m_keys_update=&keys;
//--- read users
   MTAPIRES res=m_users.Read(keys_parent,*this,&CClientUserCache::UserRead);
   if(res!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| filter cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CClientUserCache::Filter(const IMTReportCacheKeySet &ids,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- clear filtered logins set
   MTAPIRES res=m_ids.Clear();
   if(res!=MT_RET_OK)
      return(res);
//--- check filter
   if(from)
     {
      //--- set filter parameters
      m_filter_from=from;
      m_filter_to=to;
      //--- read clients
      if((res=Read(ids,*this,&CClientUserCache::FilterClient))!=MT_RET_OK)
         return(res);
     }
   else
     {
      //--- assign clients
      if((res=m_ids.Assign(&ids))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Filter client by report parameters                               |
//+------------------------------------------------------------------+
MTAPIRES CClientUserCache::FilterClient(const uint64_t id,const ClientUserCache &client)
  {
//--- registration time filter
   if(client.registration<m_filter_from || client.registration>m_filter_to)
      return(MT_RET_OK);
//--- insert id to set
   return(m_ids.Insert(id));
  }
//+------------------------------------------------------------------+
