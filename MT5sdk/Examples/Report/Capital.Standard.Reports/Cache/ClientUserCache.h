//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheChild.h"
#include "UserCache.h"
#include "UserFilter.h"
//+------------------------------------------------------------------+
//| Client by user cache class                                       |
//+------------------------------------------------------------------+
class CClientUserCache : public CReportCacheChild
  {
public:
   //--- Client structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct ClientUserCache
     {
      int64_t           registration;           // registration time
     };
   #pragma pack(pop)

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=2;
   //--- cache context class
   class CClientUserCacheContext : public CReportCacheChildContext
     {
   public:
      IMTReportCacheKeySet *m_ids;              // client ids key set
      IMTReportCacheKeySet *m_logins;           // user logins key set

   public:
      explicit          CClientUserCacheContext(IMTReportAPI &api);
      virtual          ~CClientUserCacheContext(void);
     };

private:
   IMTReportCacheKeySet &m_ids;                 // client ids key set
   IMTReportCacheKeySet &m_logins;              // user logins key set
   CUserCache       &m_users;                   // user cache reference
   const IMTReportCacheKeySet *m_keys_update;   // update keys
   int64_t           m_filter_from;             // filter time from
   int64_t           m_filter_to;               // filter time to

public:
   //--- create class object
   static CClientUserCache* Create(IMTReportAPI &api,CUserCache &users,MTAPIRES &res);
   //--- update cache
   MTAPIRES          Update(const int64_t from,const int64_t to);
   //--- client ids key set
   const IMTReportCacheKeySet& Ids(void) const                                            { return(m_ids); }
   //--- user logins key set
   const IMTReportCacheKeySet& Logins(void) const                                         { return(m_logins); }
   //--- read client
   MTAPIRES          ReadClient(const uint64_t id,const ClientUserCache *&client) const     { return(CReportCache::ReadData(id,(const void*&)client,sizeof(*client))); }

private:
                     CClientUserCache(CClientUserCacheContext &context,CUserCache &users);
   virtual          ~CClientUserCache(void);
   //--- write cache keys
   virtual MTAPIRES  WriteKeys(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent) override;
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);
   //--- filter cache
   MTAPIRES          Filter(const IMTReportCacheKeySet &ids,const int64_t from,const int64_t to);
   //--- filter client by report parameters
   MTAPIRES          FilterClient(const uint64_t id,const ClientUserCache &client);
  };
//+------------------------------------------------------------------+
