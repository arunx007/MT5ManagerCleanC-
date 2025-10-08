//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportLog.h"
//+------------------------------------------------------------------+
//| Trade report base class                                          |
//+------------------------------------------------------------------+
class CTradeReport : public IMTReportContext
  {
protected:
   const uint32_t    m_types;                         // report types mask
   IMTReportAPI     *m_api;                           // report api
   CReportLog        m_log;                           // logger

public:
                     CTradeReport(const MTReportInfo &info);
   virtual          ~CTradeReport(void);
   //--- release
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

protected:
   //--- clear overridable
   virtual void      Clear(void)=0;
   //--- report prepare overridable
   virtual MTAPIRES  Prepare(void)=0;
   //--- report write overridable
   virtual MTAPIRES  Write(void)=0;

private:
   //--- inner and outer clear
   void              ClearAll(void);
   //--- inner clear
   void              ClearInner(void);
  };
//+------------------------------------------------------------------+
