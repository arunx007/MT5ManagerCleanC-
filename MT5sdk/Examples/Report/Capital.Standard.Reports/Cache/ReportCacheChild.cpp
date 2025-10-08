//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportCacheChild.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportCacheChild::CReportCacheChildContext::CReportCacheChildContext(IMTReportAPI &api,LPCWSTR name,const uint32_t version,const int64_t key_time_to_live) :
   CReportCacheContext(api,name,version,key_time_to_live),
   m_keys_parent(api.KeySetCreate())
  {
//--- check status
   if(m_res!=MT_RET_OK)
      return;
//--- check key set
   if(!m_keys_parent)
      m_res=MT_RET_ERR_MEM;
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportCacheChild::CReportCacheChildContext::~CReportCacheChildContext(void)
  {
//--- release interfaces
   if(m_keys_parent)
     {
      m_keys_parent->Release();
      m_keys_parent=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportCacheChild::CReportCacheChild(CReportCacheChildContext &context) :
   CReportCache(context),m_keys_parent(context.Detach(context.m_keys_parent))
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportCacheChild::~CReportCacheChild(void)
  {
//--- release interfaces
   m_keys_parent.Release();
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheChild::Update(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent)
  {
//--- check for empty keys
   if(!keys.Total() || !keys_parent.Total())
      return(MT_RET_OK_NONE);
//--- begin read
   MTAPIRES res=m_cache.ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- check cache up to date
   const MTAPIRES res_up_to_date=CheckUpToDate(keys,keys_parent);
//--- end read
   if((res=m_cache.ReadEnd())!=MT_RET_OK)
      return(res);
//--- check cache up to date result
   if(res_up_to_date!=MT_RET_OK_NONE)
      return(res_up_to_date);
//--- begin write
   if((res=WriteBegin())!=MT_RET_OK)
      return(res);
//--- write cache
   const MTAPIRES res_write=Write(keys,keys_parent);
//--- end write
   res=WriteEnd(res_write==MT_RET_OK);
//--- check write result
   if(res_write!=MT_RET_OK && res_write!=MT_RET_OK_NONE)
      return(res_write);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| check cache up to date                                           |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheChild::CheckUpToDate(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent) const
  {
//--- check missing keys in cache
   MTAPIRES res=m_cache.ReadMissingKeys(&keys,nullptr);
//--- check result
   if(res==MT_RET_OK)
      return(MT_RET_OK_NONE);
   if(res!=MT_RET_OK_NONE)
      return(res);
//--- get cache keyset parameter
   if((res=m_cache.ReadParamKeySet(&m_keys_parent))!=MT_RET_OK)
      return(res);
//--- check keys contains
   res=m_keys_parent.ContainsSet(&keys_parent);
//--- check result
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK_NONE);
   if(res!=MT_RET_OK)
      return(res);
//--- up to date
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| write cache                                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheChild::Write(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent)
  {
//--- get cache keyset parameter
   MTAPIRES res=m_cache.ReadParamKeySet(&m_keys_parent);
   if(res!=MT_RET_OK)
      return(res);
//--- create key set for missing parent keys
   IMTReportCacheKeySet *keys_missing_parent=m_api.KeySetCreate();
   if(!keys_missing_parent)
      return(res);
//--- copy parent keys
   if((res=keys_missing_parent->Assign(&keys_parent))!=MT_RET_OK)
     {
      keys_missing_parent->Release();
      return(res);
     }
//--- remove existing keys from parent keys
   res=keys_missing_parent->RemoveSet(&m_keys_parent);
//--- check result
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
     {
      keys_missing_parent->Release();
      return(res);
     }
//--- create key set for missing keys
   IMTReportCacheKeySet *keys_missing=m_api.KeySetCreate();
   if(!keys_missing)
     {
      keys_missing_parent->Release();
      return(MT_RET_ERR_MEM);
     }
//--- get missing keys in cache
   res=m_cache.ReadMissingKeys(&keys,keys_missing);
//--- write missing keys to cache
   if(res==MT_RET_OK || res==MT_RET_OK_NONE)
      res=WriteMissing(keys,*keys_missing,keys_parent,*keys_missing_parent);
//--- release missing keys
   keys_missing->Release();
//--- check result
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
     {
      keys_missing_parent->Release();
      return(res);
     }
//--- update keyset parameter
   res=m_cache.WriteParamKeySet(keys_missing_parent,true);
//--- release missing parent keys
   keys_missing_parent->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| write missing keys to cache                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheChild::WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing,const IMTReportCacheKeySet &keys_parent,const IMTReportCacheKeySet &keys_missing_parent)
  {
//--- init result
   MTAPIRES res=MT_RET_OK_NONE;
//--- write missing keys in old limits
   if(keys_missing.Total())
      if((res=WriteKeys(keys_missing,keys_parent))!=MT_RET_OK)
         return(res);
//--- new limits
   if(keys_missing_parent.Total())
      if((res=WriteKeys(keys,keys_missing_parent))!=MT_RET_OK)
         return(res);
//--- check result
   if(res!=MT_RET_OK)
      return(res);
//--- write missing keys
   if(keys_missing.Total())
      if((res=m_cache.WriteMissingKeys(&keys_missing))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
