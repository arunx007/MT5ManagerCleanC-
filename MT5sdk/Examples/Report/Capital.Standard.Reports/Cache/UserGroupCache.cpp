//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UserGroupCache.h"
#include "UserFilter.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| request dataset fields descriptions                              |
//+------------------------------------------------------------------+
DatasetField CUserGroupCache::s_fields[]=
   //--- id                                      , select , offset                     , size
  {{ IMTDatasetField::FIELD_USER_LOGIN           , true   , offsetof(UserRecord,login) },
   { IMTDatasetField::FIELD_USER_REGISTRATION    },
   { IMTDatasetField::FIELD_USER_GROUP           , true   , offsetof(UserRecord,group) , MtFieldSize(UserRecord,group) },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserGroupCache::CUserGroupCache(CReportCacheContext &context) :
   CReportCacheTime(context)
  {
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CUserGroupCache* CUserGroupCache::Create(IMTReportAPI &api,CReportParameter &params,CCurrencyConverterCache &currency,MTAPIRES &res)
  {
//--- create cache context
   CReportCacheContext context(api,L"User_Group_Cache",s_cache_version);
   if((res=context.Status())!=MT_RET_OK)
      return(nullptr);
//--- initalize currency convertor
   if((res=currency.Initialize(&api,params,context.m_cache,offsetof(UserGroup,currency)))!=MT_RET_OK)
      return(nullptr);
//--- create user cache object
   CUserGroupCache *cache=new(std::nothrow) CUserGroupCache(context);
   if(!cache)
      res=MT_RET_ERR_MEM;
//--- return new user cache object
   return(cache);
  }
//+------------------------------------------------------------------+
//| write users to cache                                             |
//+------------------------------------------------------------------+
MTAPIRES CUserGroupCache::WriteData(const IMTDataset &users,uint64_t &id_last)
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
   MTAPIRES res=MT_RET_OK;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read user
      if((res=users.RowRead(pos,&user,sizeof(user)))!=MT_RET_OK)
         break;
      //--- store last id
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
   return(res);
  }
//+------------------------------------------------------------------+
//| Request users from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CUserGroupCache::SelectData(IMTDataset &users,const int64_t from,const int64_t to,const uint64_t id_from)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create request
   IMTDatasetRequest *request=m_api.DatasetRequestCreate();
   if(!request)
      return(MT_RET_ERR_MEM);
//--- fill request
   MTAPIRES res=UserRequest(*request,from,to,id_from);
   if(res!=MT_RET_OK)
     {
      request->Release();
      return(res);
     }
//--- request from base
   res=m_api.UserSelect(request,&users);
//--- release request
   request->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill users request                                               |
//+------------------------------------------------------------------+
MTAPIRES CUserGroupCache::UserRequest(IMTDatasetRequest &request,const int64_t from,const int64_t to,const uint64_t id_from)
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
         case IMTDatasetField::FIELD_USER_LOGIN:         res=composer.FieldAddBetweenUInt(id_from,UINT64_MAX); break;
         case IMTDatasetField::FIELD_USER_REGISTRATION:  res=composer.FieldAddBetweenInt(from,to); break;
         default:                                        res=composer.FieldAdd(); break;
        }
//--- limit the number of rows of the resulting dataset
   request.RowLimit(s_request_limit);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Group currency                                                   |
//+------------------------------------------------------------------+
MTAPIRES CUserGroupCache::GroupCurrency(IMTConGroup &config,const wchar_t (&group)[64],CMTStr32 &currency)
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
MTAPIRES CUserGroupCache::WriteUser(const UserRecord &user,const CMTStr32 &currency)
  {
//--- fill user cache strings
   UserGroup cache;
   MTAPIRES res=MT_RET_OK;
   if((res=m_cache.WriteDictionaryString(offsetof(UserGroup,currency),currency.Str(),cache.currency))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(offsetof(UserGroup,group)   ,user.group    ,cache.group))!=MT_RET_OK)
      return(res);
//--- write user cache value
   return(m_cache.WriteValue(user.login,&cache,sizeof(cache)));
  }
//+------------------------------------------------------------------+
