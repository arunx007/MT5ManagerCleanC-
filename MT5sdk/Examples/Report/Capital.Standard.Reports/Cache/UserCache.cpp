//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UserCache.h"
#include "UserFilter.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserCache::CUserCacheContext::CUserCacheContext(IMTReportAPI &api,const bool client) :
   CReportCacheKeysTimeContext(api,client ? L"User_Client_Cache" : L"User_Cache",s_cache_version),m_logins(api.KeySetCreate())
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
CUserCache::CUserCacheContext::~CUserCacheContext(void)
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
DatasetField CUserCache::s_fields[]=
   //--- id                                      , select , offset                            , size
  {{ IMTDatasetField::FIELD_USER_LOGIN           , true   , offsetof(UserRecord,login)        },
   { IMTDatasetField::FIELD_USER_CLIENT_ID       , true   , offsetof(UserRecord,client_id)    },
   { IMTDatasetField::FIELD_USER_REGISTRATION    , true   , offsetof(UserRecord,registration) },
   { IMTDatasetField::FIELD_USER_GROUP           , true   , offsetof(UserRecord,group)        , MtFieldSize(UserRecord,group)          },
   { IMTDatasetField::FIELD_USER_LEAD_SOURCE     , true   , offsetof(UserRecord,lead_source)  , MtFieldSize(UserRecord,lead_source)    },
   { IMTDatasetField::FIELD_USER_LEAD_CAMPAIGN   , true   , offsetof(UserRecord,lead_campaign), MtFieldSize(UserRecord,lead_campaign)  },
   { IMTDatasetField::FIELD_USER_COUNTRY         , true   , offsetof(UserRecord,country)      , MtFieldSize(UserRecord,country)        },
   { IMTDatasetField::FIELD_USER_NAME            , true   , offsetof(UserRecord,name)         , MtFieldSize(UserRecord,name)           },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserCache::CUserCache(CUserCacheContext &context,const bool client) :
   CReportCacheKeysTime(context),m_client(client),m_logins(context.Detach(context.m_logins))
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUserCache::~CUserCache(void)
  {
//--- release interfaces
   m_logins.Release();
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CUserCache* CUserCache::Create(IMTReportAPI &api,CReportParameter &params,CCurrencyConverterCache *currency,const bool client,MTAPIRES &res)
  {
//--- create cache context
   CUserCacheContext context(api,client);
   if((res=context.Status())!=MT_RET_OK)
      return(nullptr);
//--- initalize currency convertor
   if(currency)
      if((res=currency->Initialize(&api,params,context.m_cache,offsetof(UserCache,currency)))!=MT_RET_OK)
         return(nullptr);
//--- create user cache object
   CUserCache *users=new(std::nothrow) CUserCache(context,client);
   if(!users)
      res=MT_RET_ERR_MEM;
//--- return new user cache object
   return(users);
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::Update(const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- get logins key set
   MTAPIRES res=m_api.KeySetParamLogins(&m_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- update cache
   if((res=CReportCacheKeysTime::Update(m_logins,from,to))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| update and filter cache                                          |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::UpdateFilter(const int64_t from,const int64_t to)
  {
//--- update cache
   MTAPIRES res=Update(from,to);
   if(res!=MT_RET_OK)
      return(LogErrorTime(res,L"update cache",from,to));
//--- filter cache
   CUserFilter filter(m_api,*this,from,to);
   if((res=ReadAll(filter))!=MT_RET_OK)
      return(LogErrorTime(res,L"filter cache",from,to));
//--- swap logins with filtered
   if((res=filter.LoginsSwap(m_logins))!=MT_RET_OK)
      return(LogErrorTime(res,L"swap %u logins with filtered",from,to,m_logins.Total()));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| write users to cache                                             |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::WriteData(const IMTDataset &users,uint64_t &id_last)
  {
//--- deals total
   const uint32_t total=users.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- create group
   IMTConGroup *group=m_api.GroupCreate();
   if(!group)
      return(MT_RET_ERR_MEM);
//--- iterate users
   CMTStr32 currency;
   UserRecord user={0};
   const uint32_t size=m_client ? offsetof(UserRecord,country) : sizeof(user);
   MTAPIRES res=MT_RET_OK;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read user
      if((res=users.RowRead(pos,&user,size))!=MT_RET_OK)
         break;
      //--- store last user login
      if(id_last<user.login)
         id_last=user.login;
      //--- user currency
      res=GroupCurrency(*group,user.group,currency);
      if(res==MT_RET_ERR_PERMISSIONS)
         res=MT_RET_OK;
      if(res!=MT_RET_OK)
         break;
      //--- write user cache
      if((res=WriteUser(user,currency))!=MT_RET_OK)
         break;
     }
//--- release group
   group->Release();
//--- check result
   if(res!=MT_RET_OK)
      return(LogError(res,L"caching %u users",total));
   return(res);
  }
//+------------------------------------------------------------------+
//| Request users from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::SelectData(IMTDataset &users,IMTDatasetRequest &request,const int64_t from,const int64_t to,const uint64_t id_from)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- fill request
   MTAPIRES res=UserRequest(request,from,to,id_from);
   if(res!=MT_RET_OK)
      return(res);
//--- request from base
   res=m_api.UserSelect(&request,&users);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare users request                                            |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::PrepareRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &keys)
  {
//--- fill request
   MTAPIRES res=MT_RET_OK;
   CDatasetRequest composer(s_fields,_countof(s_fields),request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_USER_LOGIN:         res=composer.FieldAddWhereKeys(keys); break;
         case IMTDatasetField::FIELD_USER_CLIENT_ID:     res=m_client ? composer.FieldAddBetweenUInt(1,UINT64_MAX) : composer.FieldAdd(); break;
         case IMTDatasetField::FIELD_USER_COUNTRY:
         case IMTDatasetField::FIELD_USER_NAME:          res=m_client ? MT_RET_OK : composer.FieldAdd(); break;
         default:                                        res=composer.FieldAdd(); break;
        }
//--- limit the number of rows of the resulting dataset
   request.RowLimit(s_request_limit);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill users request                                               |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::UserRequest(IMTDatasetRequest &request,const int64_t from,const int64_t to,const uint64_t id_from)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- fill request
   MTAPIRES res=MT_RET_OK;
   CDatasetRequest composer(s_fields,_countof(s_fields),request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_USER_LOGIN:         res=composer.FieldSetFromUInt(id_from); break;
         case IMTDatasetField::FIELD_USER_REGISTRATION:  res=composer.FieldSetBetweenInt(from,to); break;
        }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Group currency                                                   |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::GroupCurrency(IMTConGroup &config,const wchar_t (&group)[64],CMTStr32 &currency)
  {
//--- checks
   if(!group[0])
      return(MT_RET_ERR_PARAMS);
//--- check for same group name
   if(LPCWSTR name=config.Group())
      if(!CMTStr::Compare(group,name))
         return(MT_RET_OK);
//--- group configuration
   MTAPIRES res=m_api.GroupGetLight(group,&config);
   if(res!=MT_RET_OK)
      return(res);
//--- group currency
   LPCWSTR group_currency=config.Currency();
   if(!group_currency || !*group_currency)
      return(MT_RET_ERROR);
   currency.Assign(group_currency);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write value to cache                                             |
//+------------------------------------------------------------------+
MTAPIRES CUserCache::WriteUser(const UserRecord &user,const CMTStr32 &currency)
  {
//--- user cache record
   UserCache cache;
   cache.client_id     =user.client_id;
   cache.registration  =user.registration;
//--- fill user cache strings
   MTAPIRES res=MT_RET_OK;
   if((res=m_cache.WriteDictionaryString(offsetof(UserCache,name)         ,user.name         ,cache.name))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(offsetof(UserCache,currency)     ,currency.Str()    ,cache.currency))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(offsetof(UserCache,lead_source)  ,user.lead_source  ,cache.lead_source))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(offsetof(UserCache,lead_campaign),user.lead_campaign,cache.lead_campaign))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(offsetof(UserCache,country)      ,user.country      ,cache.country))!=MT_RET_OK)
      return(res);
//--- write user cache value
   return(m_cache.WriteValue(user.login,&cache,sizeof(cache)));
  }
//+------------------------------------------------------------------+
