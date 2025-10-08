//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UserGroupFilter.h"
#include "..\Tools\RequestParameter.h"
//+------------------------------------------------------------------+
//| initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CUserGroupFilter::Initialize(IMTReportAPI &api,CReportCache &cache,const uint32_t dictionary_id)
  {
//--- clear
   m_groups.Clear();
//--- request parameter
   CRequestParameter param(api);
   LPCWSTR filter=nullptr;
   MTAPIRES res=param.ParamGetGroups(filter,PARAMETER_GROUP_FILTER);
   if(res!=MT_RET_OK)
      return(res);
//--- reserve memory
   if(!m_groups.Reserve(api.GroupTotal()))
      return(MT_RET_ERR_MEM);
//--- begin read cache
   if((res=cache.ReadBegin())!=MT_RET_OK)
      return(res);
//--- group config interface
   IMTConGroup *config_group=api.GroupCreate();
   if(!config_group)
      return(MT_RET_ERR_MEM);
//--- iterate group dictionary and fill group flags
   const bool all=!filter || !CMTStr::Compare(filter,L"*");
   LPCWSTR group=nullptr;
   for(uint32_t i=0;(res=cache.ReadDictionaryString(dictionary_id,i,group))==MT_RET_OK;i++)
      if(BYTE *flag=m_groups.Append())
         *flag=group && api.GroupGetLight(group,config_group)==MT_RET_OK && (all || CMTStr::CheckGroupMask(filter,group));
//--- cleanup
   config_group->Release();
   const MTAPIRES res_end=cache.ReadEnd();
//--- ok
   return(res==MT_RET_ERR_NOTFOUND || res==MT_RET_OK ? res_end : res);
  }
//+------------------------------------------------------------------+
