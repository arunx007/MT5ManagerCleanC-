//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportCacheTime.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| update cache                                                     |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheTime::Update(const int64_t from,int64_t to)
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
   MTAPIRES res=CheckUpToDate(from,to);
   if(res!=MT_RET_OK_NONE)
      return(res);
//--- begin write
   if((res=WriteBegin())!=MT_RET_OK)
      return(res);
//--- write cache
   const MTAPIRES res_write=Write(from,to);
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
MTAPIRES CReportCacheTime::CheckUpToDate(const int64_t from,const int64_t to) const
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- begin read
   MTAPIRES res=m_cache.ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- get cache limits
   const int64_t cache_from=m_cache.ReadParamFrom();
   const int64_t cache_to=m_cache.ReadParamTo();
//--- end read
   if((res=m_cache.ReadEnd())!=MT_RET_OK)
      return(res);
//--- check cache params up to date
   if(cache_from<=cache_to && cache_to && from>=cache_from && to<=cache_to)
      return(MT_RET_OK);
//--- not up to date
   return(MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| write cache                                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheTime::Write(int64_t from,int64_t to)
  {
//--- get cache limits
   const int64_t cache_from=m_cache.ReadParamFrom();
   const int64_t cache_to=m_cache.ReadParamTo();
//--- check cache limits
   MTAPIRES res=MT_RET_ERROR;
   if(cache_from<=cache_to && cache_to)
     {
      //--- write missing keys to cache
      res=WriteMissing(cache_from,cache_to,from,to);
      //--- check result
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(MT_RET_OK_NONE);
     }
   else
     {
      //--- write whole range
      if((res=WriteRange(from,to))!=MT_RET_OK)
         return(res);
     }
//--- update limits
   if((res=m_cache.WriteParamFrom(from))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteParamTo(to))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| write missing keys to cache                                      |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheTime::WriteMissing(const int64_t cache_from,const int64_t cache_to,int64_t &from,int64_t &to)
  {
//--- init result
   MTAPIRES res=MT_RET_OK_NONE;
//--- new limits
   from=std::min(from,cache_from);
   to=std::max(to,cache_to);
//--- check limits
   if(from<cache_from || to>cache_to)
     {
      //--- write left part
      if(from<cache_from)
         if((res=WriteRange(from,cache_from-1))!=MT_RET_OK)
            return(res);
      //--- write right part
      if(to>cache_to)
         if((res=WriteRange(cache_to+1,to))!=MT_RET_OK)
            return(res);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| write cache range                                                |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheTime::WriteRange(const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- write cache range with limited request
   return(WriteRangeLimit(*data,from,to));
  }
//+------------------------------------------------------------------+
//| write cache range with limited request                           |
//+------------------------------------------------------------------+
MTAPIRES CReportCacheTime::WriteRangeLimit(IMTDataset &data,const int64_t from,const int64_t to)
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
      MTAPIRES res=SelectData(data,from,to,id_last);
      time_select+=timer_select.Elapsed();
      total+=data.RowTotal();
      partial=res==MT_RET_ERR_PARTIAL;
      if(!partial && res!=MT_RET_OK)
         return(LogSelectError(res,from,to,id_last));
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
   return(LogWriteOk(from,to,total,time_select,time_write));
  }
//+------------------------------------------------------------------+
