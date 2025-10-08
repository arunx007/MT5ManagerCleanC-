//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCache.h"
//+------------------------------------------------------------------+
//| Report cache base select base class                              |
//+------------------------------------------------------------------+
class CReportCacheSelectTime : public CReportCache
  {
public:
   //--- update cache
   MTAPIRES          Update(const IMTReportCacheKeySet &keys,const int64_t from,int64_t to);

protected:
   explicit          CReportCacheSelectTime(CReportCacheContext &context);
   virtual          ~CReportCacheSelectTime(void);
   //--- check cache up to date
   MTAPIRES          CheckUpToDate(const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to) const;
   //--- write cache
   MTAPIRES          Write(const IMTReportCacheKeySet &keys,int64_t from,int64_t to);
   //--- write missing keys to cache
   MTAPIRES          WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing,const int64_t cache_from,const int64_t cache_to,int64_t &from,int64_t &to);
   //--- write cache range
   MTAPIRES          WriteRange(const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to);
   //--- request data from base
   virtual MTAPIRES  SelectData(IMTDataset &data,const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to)=0;
   //--- write data to cache
   virtual MTAPIRES  WriteData(const IMTDataset &data)=0;
   //--- write missing keys
   virtual MTAPIRES  WriteMissingKeys(const IMTReportCacheKeySet &keys) { return(keys.Total() ? m_cache.WriteMissingKeys(&keys) : MT_RET_OK); }
  };
//+------------------------------------------------------------------+
