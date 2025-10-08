//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ClientCache.h"
//+------------------------------------------------------------------+
//| Client filter class                                              |
//+------------------------------------------------------------------+
class CClientFilter
  {
private:
   bool              m_initialized;                // initialization flag
   int64_t           m_lead_source;                // lead source string
   int64_t           m_lead_campaign;              // lead campaign string

public:
                     CClientFilter(void);
                    ~CClientFilter(void);
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI &api,const CClientCache &cache);
   //--- clear
   void              Clear(void);
   //--- match ñlient with filter
   bool              ClientMatch(const CClientCache::ClientCache &client) const;

private:
   //--- get report parameter string value
   static LPCWSTR    ParamGetString(IMTReportAPI &api,LPCWSTR name,IMTConParam *param,LPCWSTR def);
  };
//+------------------------------------------------------------------+
