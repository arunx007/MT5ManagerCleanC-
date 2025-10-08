//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UserActivityCache.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserActivityCache::CUserActivityCacheContext::CUserActivityCacheContext(IMTReportAPI &api,const bool client) :
   CReportCacheContext(api,client ? L"Client_User_Activity_Cache" : L"User_Activity_Cache",s_cache_version),m_logins(api.KeySetCreate())
  {
//--- check status
   if(m_res!=MT_RET_OK)
      return;
//--- check pointers
   if(!m_logins)
      m_res=MT_RET_ERR_MEM;
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUserActivityCache::CUserActivityCacheContext::~CUserActivityCacheContext(void)
  {
//--- release interfaces
   if(m_logins)
     {
      m_logins->Release();
      m_logins=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| request dataset fields descriptions                              |
//+------------------------------------------------------------------+
DatasetField CUserActivityCache::s_fields[]=
   //--- id                                      , select , offset                                    , size
  {{ IMTDatasetField::FIELD_USER_LOGIN           , true   , offsetof(UserActivityRecord,login)        },
   { IMTDatasetField::FIELD_USER_LAST_ACCESS     , true   , offsetof(UserActivityRecord,last_access)  },
   { IMTDatasetField::FIELD_USER_CLIENT_ID       , true   , offsetof(UserActivityRecord,client_id)    },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserActivityCache::CUserActivityCache(CUserActivityCacheContext &context,const bool client) :
   CReportCacheSelectTime(context),m_client(client),m_logins(context.Detach(context.m_logins))
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUserActivityCache::~CUserActivityCache(void)
  {
//--- release interfaces
   m_logins.Release();
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CUserActivityCache* CUserActivityCache::Create(IMTReportAPI &api,const bool client,MTAPIRES &res)
  {
//--- create cache context
   CUserActivityCacheContext context(api,client);
   if((res=context.Status())!=MT_RET_OK)
      return(nullptr);
//--- create user cache object
   CUserActivityCache *users=new(std::nothrow) CUserActivityCache(context,client);
   if(!users)
      res=MT_RET_ERR_MEM;
//--- return new user cache object
   return(users);
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CUserActivityCache::Update(const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- get logins key set
   MTAPIRES res=m_api.KeySetParamLogins(&m_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- update cache
   if((res=CReportCacheSelectTime::Update(m_logins,from,to))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| write users to cache                                             |
//+------------------------------------------------------------------+
MTAPIRES CUserActivityCache::WriteData(const IMTDataset &users)
  {
//--- deals total
   const uint32_t total=users.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- save current time
   const CReportTimer timer;
//--- iterate users
   UserActivityRecord user={0};
   UserActivityCache cache={0};
   const uint32_t size=m_client ? sizeof(user) : offsetof(UserActivityRecord,client_id);
   MTAPIRES res=MT_RET_OK;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read user
      if((res=users.RowRead(pos,&user,size))!=MT_RET_OK)
         break;
      //--- check client last access time
      if(m_client)
        {
         //--- skip user with no client
         if(!user.client_id)
            continue;
         //--- read client activity
         const UserActivityCache *client=nullptr;
         MTAPIRES res=ReadActivity(user.client_id,client);
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(res);
         //--- check last access time
         if(client && client->last_access>=user.last_access)
            continue;
        }
      //--- copy last access time
      cache.last_access=user.last_access;
      //--- write user cache
      if((res=m_cache.WriteValue(m_client ? user.client_id : user.login,&cache,sizeof(cache)))!=MT_RET_OK)
         break;
     }
//--- check result
   if(res!=MT_RET_OK)
     {
      m_api.LoggerOut(ErrorLogType(res),L"User activity caching failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- write statistics to log
   return(m_api.LoggerOut(MTLogOK,L"Cached %u users activity in %I64u ms",total,timer.Elapsed()));
  }
//+------------------------------------------------------------------+
//| Request users from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CUserActivityCache::SelectData(IMTDataset &users,const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create request
   IMTDatasetRequest *request=m_api.DatasetRequestCreate();
   if(!request)
      return(MT_RET_ERR_MEM);
//--- fill request
   MTAPIRES res=UserRequest(*request,keys,from,to);
   if(res!=MT_RET_OK)
     {
      request->Release();
      return(res);
     }
//--- save current time
   const CReportTimer timer;
//--- request from base
   if((res=m_api.UserSelect(request,&users))!=MT_RET_OK)
      m_api.LoggerOut(MTLogAtt,L"User selection failed, error: %s",SMTFormat::FormatError(res));
   else
      res=m_api.LoggerOut(MTLogOK,L"Selected %u users in %I64u ms",users.RowTotal(),timer.Elapsed());
//--- release request
   request->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill users request                                               |
//+------------------------------------------------------------------+
MTAPIRES CUserActivityCache::UserRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &logins,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- fill request
   MTAPIRES res=MT_RET_OK;
   CDatasetRequest composer(s_fields,m_client ? _countof(s_fields) : _countof(s_fields)-1,request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_USER_LOGIN:         res=composer.FieldAddWhereKeys(logins); break;
         case IMTDatasetField::FIELD_USER_LAST_ACCESS:   res=composer.FieldAddBetweenInt(from,to); break;
         default:                                        res=composer.FieldAdd(); break;
        }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
