//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheKeysTime.h"
#include "..\Tools\DatasetField.h"
//+------------------------------------------------------------------+
//| Deals cache base class                                           |
//+------------------------------------------------------------------+
class CDealBaseCache : public CReportCacheKeysTime
  {
protected:
   explicit          CDealBaseCache(CReportCacheKeysTimeContext &context) : CReportCacheKeysTime(context) {}
   virtual          ~CDealBaseCache(void)=default;
   //--- write cache range
   virtual MTAPIRES  WriteRange(IMTDatasetRequest &request,int64_t from,int64_t to) override;
   //--- request deals from base
   virtual MTAPIRES  SelectData(IMTDataset &deals,IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from) override;
   //--- fill deals request
   virtual MTAPIRES  DealRequest(IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from)=0;
   //--- end of month
   static int64_t    MonthEnd(int64_t ctm,int64_t to);
   //--- calculate upper power of 2
   static uint32_t   UpperPowerOfTwo(uint32_t v);
   //--- logging
   virtual MTAPIRES  LogSelectError(const MTAPIRES res,const uint32_t keys,const int64_t from,const int64_t to,const uint64_t id_from) override
     {
      return(LogErrorTime(res,L"Deals selection for %u accounts from #%I64u with time",keys,from,to,id_from));
     }
   virtual MTAPIRES  LogWriteError(const MTAPIRES res,const int64_t from,const int64_t to) override
     {
      return(LogErrorTime(res,L"Deals aggregation with time",from,to));
     }
   virtual MTAPIRES  LogWriteOk(const int64_t from,const int64_t to,const uint64_t total,const uint32_t keys,const uint64_t time_select,const uint64_t time_write) override
     {
      return(LogOkTime(L"Selected %I64u deals of %u accounts in %I64u ms, aggregated in %I64u ms, time",from,to,total,keys,time_select,time_write));
     }
  };
//+------------------------------------------------------------------+
