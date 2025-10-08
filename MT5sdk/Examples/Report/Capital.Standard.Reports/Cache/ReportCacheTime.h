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
class CReportCacheTime : public CReportCache
  {
public:
   //--- update cache
   MTAPIRES          Update(const int64_t from,int64_t to);

protected:
   explicit          CReportCacheTime(CReportCacheContext &context) : CReportCache(context) {}
   virtual          ~CReportCacheTime(void)=default;
   //--- check cache up to date
   MTAPIRES          CheckUpToDate(const int64_t from,const int64_t to) const;
   //--- write cache
   virtual MTAPIRES  Write(int64_t from,int64_t to);
   //--- write missing keys to cache
   MTAPIRES          WriteMissing(const int64_t cache_from,const int64_t cache_to,int64_t &from,int64_t &to);
   //--- write cache range
   virtual MTAPIRES  WriteRange(const int64_t from,const int64_t to);
   //--- write cache range with limited request
   MTAPIRES          WriteRangeLimit(IMTDataset &data,const int64_t from,const int64_t to);
   //--- request data from base
   virtual MTAPIRES  SelectData(IMTDataset &data,int64_t from,int64_t to,uint64_t id_from)=0;
   //--- write data to cache
   virtual MTAPIRES  WriteData(const IMTDataset &data,uint64_t &id_last)=0;
   //--- logging
   virtual MTAPIRES  LogSelectError(MTAPIRES res,int64_t from,int64_t to,uint64_t id_from)=0;
   virtual MTAPIRES  LogWriteError(MTAPIRES res,int64_t from,int64_t to)=0;
   virtual MTAPIRES  LogWriteOk(int64_t from,int64_t to,uint64_t total,uint64_t time_select,uint64_t time_write)=0;
  };
//+------------------------------------------------------------------+
