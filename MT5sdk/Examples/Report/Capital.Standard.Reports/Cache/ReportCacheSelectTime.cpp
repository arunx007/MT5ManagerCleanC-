//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportCacheSelectTime.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportCacheSelectTime::CReportCacheSelectTime(CReportCacheContext &context) :
   CReportCache(context)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportCacheSelectTime::~CReportCacheSelectTime(void)
  {
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelectTime::Update(const IMTReportCacheKeySet &keys,const int64_t from,int64_t to)
  {
//--- checks
   if(from>to || !to)
      return(LogErrorTime(MT_RET_ERR_PARAMS,L"check time",from,to));
//--- check for empty keys
   if(!keys.Total())
      return(MT_RET_OK_NONE);
//--- get report generation time
   const int64_t now=m_api.TimeGeneration();
   if(!now)
      return(LogError(MT_RET_ERR_PARAMS,L"get report generation time"));
//--- limit to time to report generation time
   if(to>now)
      to=now;
//--- check time range
   if(from>to)
      return(MT_RET_OK_NONE);
//--- check cache up to date
   MTAPIRES res=CheckUpToDate(keys,from,to);
   if(res!=MT_RET_OK_NONE)
      return(LogErrorTime(res,L"check cache up to date",from,to));
//--- begin write
   if((res=WriteBegin())!=MT_RET_OK)
      return(LogErrorTime(res,L"begin write",from,to));
//--- write cache
   const MTAPIRES res_write=Write(keys,from,to);
//--- end write
   res=WriteEnd(res_write==MT_RET_OK);
//--- check write result
   if(res_write!=MT_RET_OK && res_write!=MT_RET_OK_NONE)
      return(LogErrorTime(res_write,L"write cache",from,to));
//--- return result
   return(LogErrorTime(res,L"end write",from,to));
  }
//+------------------------------------------------------------------+
//| check cache up to date                                           |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelectTime::CheckUpToDate(const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to) const
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- begin read
   MTAPIRES res=m_cache.ReadBegin();
   if(res!=MT_RET_OK)
      return(LogErrorTime(res,L"begin read",from,to));
//--- check missing keys in cache
   const MTAPIRES res_keys_missing=m_cache.ReadMissingKeys(&keys,nullptr);
   if(res_keys_missing!=MT_RET_OK && res_keys_missing!=MT_RET_OK_NONE)
     {
      m_cache.ReadEnd();
      return(LogErrorTime(res_keys_missing,L"check missing keys in cache",from,to));
     }
//--- get cache limits
   const int64_t cache_from=m_cache.ReadParamFrom();
   const int64_t cache_to=m_cache.ReadParamTo();
//--- end read
   if((res=m_cache.ReadEnd())!=MT_RET_OK)
      return(res);
//--- check cache params up to date
   if(cache_from<=cache_to && cache_to && from>=cache_from && to<=cache_to && res_keys_missing==MT_RET_OK_NONE)
      return(MT_RET_OK);
//--- not up to date
   return(MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| write cache                                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelectTime::Write(const IMTReportCacheKeySet &keys,int64_t from,int64_t to)
  {
//--- get cache limits
   const int64_t cache_from=m_cache.ReadParamFrom();
   const int64_t cache_to=m_cache.ReadParamTo();
//--- check cache limits
   MTAPIRES res=MT_RET_ERROR;
   if(cache_from<=cache_to && cache_to)
     {
      //--- create key set for missing keys
      IMTReportCacheKeySet *keys_missing=m_api.KeySetCreate();
      if(!keys_missing)
         return(MT_RET_ERR_MEM);
      //--- get missing keys in cache
      res=LogErrorTime(m_cache.ReadMissingKeys(&keys,keys_missing),L"get missing keys in cache",from,to);
      //--- write missing keys to cache
      if(res==MT_RET_OK || res==MT_RET_OK_NONE)
         res=LogErrorTime(WriteMissing(keys,*keys_missing,cache_from,cache_to,from,to),L"write missing keys to cache",from,to);
      //--- release missing keys
      keys_missing->Release();
      //--- check result
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(res);
     }
   else
     {
      //--- write whole range
      if((res=WriteRange(keys,from,to))!=MT_RET_OK)
         return(LogErrorTime(res,L"write whole range",from,to));
      //--- write missing keys
      if((res=WriteMissingKeys(keys))!=MT_RET_OK)
         return(LogErrorTime(res,L"write missing keys",from,to));
     }
//--- update limits
   if((res=m_cache.WriteParamFrom(from))!=MT_RET_OK)
      return(LogErrorTime(res,L"write param from",from,to));
   if((res=m_cache.WriteParamTo(to))!=MT_RET_OK)
      return(LogErrorTime(res,L"write param to",from,to));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| write missing keys to cache                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelectTime::WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing,const int64_t cache_from,const int64_t cache_to,int64_t &from,int64_t &to)
  {
//--- init result
   MTAPIRES res=MT_RET_OK_NONE;
//--- write missing keys in old limits
   if(keys_missing.Total())
      if((res=WriteRange(keys_missing,cache_from,cache_to))!=MT_RET_OK)
         return(LogErrorTime(res,L"write missing keys in old limits",cache_from,cache_to));
//--- new limits
   from=std::min(from,cache_from);
   to=std::max(to,cache_to);
//--- check limits
   if(from<cache_from || to>cache_to)
     {
      //--- create key set for all keys
      IMTReportCacheKeySet *keys_all=m_api.KeySetCreate();
      if(!keys_all)
         return(LogErrorTime(MT_RET_ERR_MEM,L"create key set for all keys",from,to));
      //--- get all keys from cache
      if((res=m_cache.ReadMissingKeys(nullptr,keys_all))!=MT_RET_OK)
        {
         keys_all->Release();
         return(LogErrorTime(res,L"get all keys from cache",from,to));
        }
      //--- union keys with all keys to make full keys set
      if((res=keys_all->InsertSet(&keys))!=MT_RET_OK)
        {
         keys_all->Release();
         return(LogErrorTime(res,L"union keys with all keys to make full keys set",from,to));
        }
      //--- write left part
      if(from<cache_from)
         if((res=WriteRange(*keys_all,from,cache_from-1))!=MT_RET_OK)
           {
            keys_all->Release();
            return(LogErrorTime(res,L"write left part",from,cache_from-1));
           }
      //--- write right part
      if(to>cache_to)
         if((res=WriteRange(*keys_all,cache_to+1,to))!=MT_RET_OK)
           {
            keys_all->Release();
            return(LogErrorTime(res,L"write right part",cache_to+1,to));
           }
      //--- release set
      keys_all->Release();
     }
//--- check result
   if(res!=MT_RET_OK)
      return(LogErrorTime(res,L"write check result",from,to));
//--- write missing keys
   if((res=WriteMissingKeys(keys_missing))!=MT_RET_OK)
      return(LogErrorTime(res,L"write missing keys",from,to));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| write cache range                                                |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheSelectTime::WriteRange(const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(LogErrorTime(MT_RET_ERR_PARAMS,L"check params",from,to));
//--- create dataset
   IMTDataset *data=m_api.DatasetAppend();
   if(!data)
      return(LogErrorTime(MT_RET_ERR_MEM,L"create dataset",from,to));
//--- select data
   MTAPIRES res=SelectData(*data,keys,from,to);
   if(res!=MT_RET_OK)
      return(LogErrorTime(res,L"select data",from,to));
//--- write data
   if((res=WriteData(*data))!=MT_RET_OK)
      return(LogErrorTime(res,L"write data",from,to));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
