//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportCacheSelect.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportCacheSelect::CReportCacheSelect(CReportCacheContext &context) :
   CReportCache(context)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportCacheSelect::~CReportCacheSelect(void)
  {
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelect::Update(const IMTReportCacheKeySet &keys)
  {
//--- check for empty keys
   if(!keys.Total())
      return(MT_RET_OK_NONE);
//--- check cache up to date
   MTAPIRES res=CheckUpToDate(keys);
   if(res!=MT_RET_OK_NONE)
      return(res);
//--- begin write
   if((res=WriteBegin())!=MT_RET_OK)
      return(res);
//--- write cache
   const MTAPIRES res_write=Write(keys);
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
MTAPIRES CReportCacheSelect::CheckUpToDate(const IMTReportCacheKeySet &keys) const
  {
//--- begin read
   MTAPIRES res=m_cache.ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- check missing keys in cache
   const MTAPIRES res_keys_missing=m_cache.ReadMissingKeys(&keys,nullptr);
//--- end read
   if((res=m_cache.ReadEnd())!=MT_RET_OK)
      return(res);
//--- check result
   if(res_keys_missing==MT_RET_OK)
      return(MT_RET_OK_NONE);
   if(res_keys_missing==MT_RET_OK_NONE)
      return(MT_RET_OK);
//--- error
   return(res_keys_missing);
  }
//+------------------------------------------------------------------+
//| write cache                                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelect::Write(const IMTReportCacheKeySet &keys)
  {
//--- create key set for missing keys
   IMTReportCacheKeySet *keys_missing=m_api.KeySetCreate();
   if(!keys_missing)
      return(MT_RET_ERR_MEM);
//--- get missing keys in cache
   MTAPIRES res=m_cache.ReadMissingKeys(&keys,keys_missing);
//--- write missing keys to cache
   if(res==MT_RET_OK || res==MT_RET_OK_NONE)
      res=WriteMissing(keys,*keys_missing);
//--- release missing keys
   keys_missing->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| write missing keys to cache                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelect::WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing)
  {
//--- init result
   MTAPIRES res=MT_RET_OK_NONE;
//--- write missing keys in old limits
   if(keys_missing.Total())
      if((res=WriteKeys(keys_missing))!=MT_RET_OK)
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
//| write cache keys                                                 |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelect::WriteKeys(const IMTReportCacheKeySet &keys)
  {
//--- create dataset
   IMTDataset *data=m_api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- select data
   MTAPIRES res=SelectData(*data,keys);
   if(res!=MT_RET_OK)
      return(res);
//--- write data
   if((res=WriteData(*data))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
