//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCache.h"
//+------------------------------------------------------------------+
//| Report cache select base class with time and no keys             |
//+------------------------------------------------------------------+
class CReportCacheKeysTime : public CReportCache
  {
protected:
   //--- cache context class
   class CReportCacheKeysTimeContext : public CReportCacheContext
     {
   public:
      IMTDatasetRequest *m_request;       // dataset request interface pointer

   public:
      explicit          CReportCacheKeysTimeContext(IMTReportAPI &api,LPCWSTR name,uint32_t version);
      virtual          ~CReportCacheKeysTimeContext(void);
     };

private:
   IMTDatasetRequest &m_request;          // dataset request interface reference
   uint32_t          m_keys_total;        // request keys total count for logging

public:
   //--- update cache
   MTAPIRES          Update(const IMTReportCacheKeySet &keys,int64_t from,int64_t to);

protected:
   explicit          CReportCacheKeysTime(CReportCacheKeysTimeContext &context);
   virtual          ~CReportCacheKeysTime(void);
   //--- check cache up to date
   MTAPIRES          CheckUpToDate(const IMTReportCacheKeySet &keys,int64_t from,int64_t to) const;
   //--- write cache
   virtual MTAPIRES  Write(const IMTReportCacheKeySet &keys,int64_t from,int64_t to);
   //--- write missing keys to cache
   MTAPIRES          WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing,int64_t cache_from,int64_t cache_to,int64_t &from,int64_t &to);
   //--- write cache range
   virtual MTAPIRES  WriteRange(IMTDatasetRequest &request,int64_t from,int64_t to);
   //--- write cache range with limited request
   MTAPIRES          WriteRangeLimit(IMTDataset &data,IMTDatasetRequest &request,int64_t from,int64_t to);
   //--- prepare request for all keys
   MTAPIRES          PrepareRequestAll(const IMTReportCacheKeySet &keys,IMTReportCacheKeySet &keys_all);
   //--- prepare request for key set
   MTAPIRES          PrepareRequestKeys(const IMTReportCacheKeySet &keys);
   //--- prepare request
   virtual MTAPIRES  PrepareRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &keys)=0;
   //--- request data from base
   virtual MTAPIRES  SelectData(IMTDataset &data,IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from)=0;
   //--- write data to cache
   virtual MTAPIRES  WriteData(const IMTDataset &data,uint64_t &id_last)=0;
   //--- write missing keys
   virtual MTAPIRES  WriteMissingKeys(const IMTReportCacheKeySet &keys) { return(keys.Total() ? m_cache.WriteMissingKeys(&keys) : MT_RET_OK); }
   //--- logging
   virtual MTAPIRES  LogSelectError(MTAPIRES res,uint32_t keys,int64_t from,int64_t to,uint64_t id_from)=0;
   virtual MTAPIRES  LogWriteError(MTAPIRES res,int64_t from,int64_t to)=0;
   virtual MTAPIRES  LogWriteOk(int64_t from,int64_t to,uint64_t total,uint32_t keys,uint64_t time_select,uint64_t time_write)=0;
  };
//+------------------------------------------------------------------+
