//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportCacheKeysTime.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportCacheKeysTime::CReportCacheKeysTimeContext::CReportCacheKeysTimeContext(IMTReportAPI &api,LPCWSTR name,const uint32_t version) :
   CReportCacheContext(api,name,version),
   m_request(api.DatasetRequestCreate())
  {
//--- check status
   if(m_res!=MT_RET_OK)
      return;
//--- check key set
   if(!m_request)
      m_res=MT_RET_ERR_MEM;
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportCacheKeysTime::CReportCacheKeysTimeContext::~CReportCacheKeysTimeContext(void)
  {
//--- release interfaces
   if(m_request)
     {
      m_request->Release();
      m_request=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportCacheKeysTime::CReportCacheKeysTime(CReportCacheKeysTimeContext &context) :
   CReportCache(context),m_request(context.Detach(context.m_request)),m_keys_total(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportCacheKeysTime::~CReportCacheKeysTime(void)
  {
//--- release interfaces
   m_request.Release();
  }
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheKeysTime::Update(const IMTReportCacheKeySet &keys,const int64_t from,int64_t to)
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- get report generation time
   const int64_t now=m_api.TimeGeneration();
   if(!now)
      return(MT_RET_ERR_PARAMS);
//--- limit to time to report generation time
   if(to>now)
      to=now;
//--- check time range
   if(from>to)
      return(MT_RET_OK_NONE);
//--- check cache up to date
   MTAPIRES res=CheckUpToDate(keys,from,to);
   if(res!=MT_RET_OK_NONE)
      return(res);
//--- begin write
   if((res=WriteBegin())!=MT_RET_OK)
      return(res);
//--- write cache
   const MTAPIRES res_write=Write(keys,from,to);
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
MTAPIRES CReportCacheKeysTime::CheckUpToDate(const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to) const
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
MTAPIRES CReportCacheKeysTime::Write(const IMTReportCacheKeySet &keys,int64_t from,int64_t to)
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
      //--- prepare request
      if((res=PrepareRequestKeys(keys))!=MT_RET_OK)
         return(LogError(res,L"prepare whole range request for %u keys",keys.Total()));
      //--- write whole range
      if((res=WriteRange(m_request,from,to))!=MT_RET_OK)
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
MTAPIRES CReportCacheKeysTime::WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing,const int64_t cache_from,const int64_t cache_to,int64_t &from,int64_t &to)
  {
//--- init result
   MTAPIRES res=MT_RET_OK_NONE;
//--- write missing keys in old limits
   if(keys_missing.Total())
     {
      //--- prepare request
      if((res=PrepareRequestKeys(keys_missing))!=MT_RET_OK)
         return(LogError(res,L"prepare request for %u keys",keys_missing.Total()));
      if((res=WriteRange(m_request,cache_from,cache_to))!=MT_RET_OK)
         return(LogErrorTime(res,L"write missing keys in old limits",cache_from,cache_to));
     }
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
      //--- prepare request for all keys
      if((res=PrepareRequestAll(keys,*keys_all))!=MT_RET_OK)
        {
         keys_all->Release();
         return(LogErrorTime(MT_RET_ERR_MEM,L"prepare request for all keys",from,to));
        }
      //--- write left part
      if(from<cache_from)
         if((res=WriteRange(m_request,from,cache_from-1))!=MT_RET_OK)
           {
            keys_all->Release();
            return(LogErrorTime(res,L"write left part",from,cache_from-1));
           }
      //--- write right part
      if(to>cache_to)
         if((res=WriteRange(m_request,cache_to+1,to))!=MT_RET_OK)
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
//| prepare request for all keys                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheKeysTime::PrepareRequestAll(const IMTReportCacheKeySet &keys,IMTReportCacheKeySet &keys_all)
  {
//--- get all keys from cache
   MTAPIRES res=m_cache.ReadMissingKeys(nullptr,&keys_all);
   if(res!=MT_RET_OK)
      return(LogError(res,L"get all keys from cache"));
//--- union keys with all keys to make full keys set
   if((res=keys_all.InsertSet(&keys))!=MT_RET_OK)
      return(LogError(res,L"union %u keys with all cache %u keys to make full keys set",keys.Total(),keys_all.Total()));
//--- prepare request
   if((res=PrepareRequestKeys(keys_all))!=MT_RET_OK)
      return(LogError(res,L"prepare request for all %u keys",keys_all.Total()));
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| prepare request for all keys                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheKeysTime::PrepareRequestKeys(const IMTReportCacheKeySet &keys)
  {
//--- clear request
   const MTAPIRES res=m_request.Clear();
   if(res!=MT_RET_OK)
      return(LogError(res,L"request clear"));
//--- store keys total count for logging
   m_keys_total=keys.Total();
//--- prepare request
   return(PrepareRequest(m_request,keys));
  }
//+------------------------------------------------------------------+
//| write cache range                                                |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheKeysTime::WriteRange(IMTDatasetRequest &request,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- write cache range with limited request
   return(WriteRangeLimit(*data,request,from,to));
  }
//+------------------------------------------------------------------+
//| write cache range with limited request                           |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheKeysTime::WriteRangeLimit(IMTDataset &data,IMTDatasetRequest &request,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- update cache step by step
   uint64_t time_select=0,time_write=0;
   uint64_t total=0;
   bool partial=true;
   for(uint64_t id_last=0;partial;id_last++)
     {
      //--- select data
      const CReportTimer timer_select;
      MTAPIRES res=SelectData(data,request,from,to,id_last);
      time_select+=timer_select.Elapsed();
      total+=data.RowTotal();
      partial=res==MT_RET_ERR_PARTIAL;
      if(!partial && res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(LogSelectError(res,m_keys_total,from,to,id_last));
      //--- write data
      const CReportTimer timer_write;
      res=WriteData(data,id_last);
      time_write+=timer_write.Elapsed();
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(LogWriteError(res,from,to));
      //--- clear dataset
      data.Clear();
     }
//--- log success
   return(LogWriteOk(from,to,total,m_keys_total,time_select,time_write));
  }
//+------------------------------------------------------------------+
