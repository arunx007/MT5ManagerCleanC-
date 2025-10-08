//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheSelectTime.h"
#include "..\Tools\DatasetField.h"
//+------------------------------------------------------------------+
//| User activity cache class                                        |
//+------------------------------------------------------------------+
class CUserActivityCache : public CReportCacheSelectTime
  {
public:
   //--- User activity structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct UserActivityCache
     {
      int64_t           last_access;            // last access time
     };
   #pragma pack(pop)

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=2;
   //--- User activity record
   #pragma pack(push,1)
   struct UserActivityRecord
     {
      uint64_t          login;                  // login
      int64_t           last_access;            // last access time
      uint64_t          client_id;              // client id (used in full cache mode only, when m_client==true)
     };
   #pragma pack(pop)

private:
   //--- cache context class
   class CUserActivityCacheContext : public CReportCacheContext
     {
   public:
      IMTReportCacheKeySet *m_logins;           // user logins set

   public:
                        CUserActivityCacheContext(IMTReportAPI &api,const bool client);
      virtual          ~CUserActivityCacheContext(void);
     };

private:
   const bool        m_client;                  // flag determines that cache clients
   IMTReportCacheKeySet &m_logins;              // user logins set (contains all clients ids key set after update in client mode)
   //--- static data
   static DatasetField s_fields[];              // request dataset fields descriptions

public:
   //--- create class object
   static CUserActivityCache* Create(IMTReportAPI &api,const bool client,MTAPIRES &res);
   //--- update cache
   MTAPIRES          Update(const int64_t from,const int64_t to);
   //--- user logins key set
   const IMTReportCacheKeySet& Logins(void) const                                          { return(m_logins); }
   //--- read user activity
   MTAPIRES          ReadActivity(const uint64_t login,const UserActivityCache *&activity) const { return(ReadData(login,(const void*&)activity,sizeof(*activity))); }

private:
                     CUserActivityCache(CUserActivityCacheContext &context,const bool client);
   virtual          ~CUserActivityCache(void);
   //--- write activity to cache
   virtual MTAPIRES  WriteData(const IMTDataset &users) override;
   //--- request activity from base
   virtual MTAPIRES  SelectData(IMTDataset &users,const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to) override;
   //--- fill activity request
   MTAPIRES          UserRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &logins,const int64_t from,const int64_t to);
  };
//+------------------------------------------------------------------+
