//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCache.h"
//+------------------------------------------------------------------+
//| Report child cache base class                                    |
//+------------------------------------------------------------------+
class CReportCacheChild : public CReportCache
  {
protected:
   //--- cache context class
   class CReportCacheChildContext : public CReportCacheContext
     {
   public:
      IMTReportCacheKeySet *m_keys_parent;      // parent's key set

   public:
      explicit          CReportCacheChildContext(IMTReportAPI &api,LPCWSTR name,const uint32_t version,const int64_t key_time_to_live=KEY_TIME_TO_LIVE);
      virtual          ~CReportCacheChildContext(void);
     };

protected:
   IMTReportCacheKeySet &m_keys_parent;         // parent's key set

public:
   //--- update cache
   MTAPIRES          Update(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent);

protected:
   explicit          CReportCacheChild(CReportCacheChildContext &context);
   virtual          ~CReportCacheChild(void);
   //--- check cache up to date
   MTAPIRES          CheckUpToDate(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent) const;
   //--- write cache
   MTAPIRES          Write(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent);
   //--- write missing keys to cache
   MTAPIRES          WriteMissing(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_missing,const IMTReportCacheKeySet &keys_parent,const IMTReportCacheKeySet &keys_missing_parent);
   //--- write cache keys
   virtual MTAPIRES  WriteKeys(const IMTReportCacheKeySet &keys,const IMTReportCacheKeySet &keys_parent)=0;
  };
//+------------------------------------------------------------------+
