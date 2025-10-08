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
class CReportCacheSelect : public CReportCache
  {
public:
   //--- update cache
   MTAPIRES          Update(const IMTReportCacheKeySet &keys);

protected:
   explicit          CReportCacheSelect(CReportCacheContext &context);
   virtual          ~CReportCacheSelect(void);
   //--- check cache up to date
   MTAPIRES          CheckUpToDate(const IMTReportCacheKeySet &keys) const;
   //--- write cache
   MTAPIRES          Write(const IMTReportCacheKeySet &keys);
   //--- write missing keys to cache
   MTAPIRES          WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing);
   //--- write cache keys
   MTAPIRES          WriteKeys(const IMTReportCacheKeySet &keys);
   //--- request data from base
   virtual MTAPIRES  SelectData(IMTDataset &data,const IMTReportCacheKeySet &keys)=0;
   //--- write data to cache
   virtual MTAPIRES  WriteData(const IMTDataset &data)=0;
  };
//+------------------------------------------------------------------+
