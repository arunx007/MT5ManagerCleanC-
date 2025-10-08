//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportLog.h"
//+------------------------------------------------------------------+
//| Select base class                                                |
//+------------------------------------------------------------------+
class CReportSelect
  {
protected:
   //--- API interfaces
   IMTReportAPI     *m_api;                  // report api
   IMTDatasetRequest *m_request;             // request
   IMTDataset       *m_dataset;              // dataset
   CReportLog        m_log;                  // logger

public:
   //--- constructor/destructor
                     CReportSelect(LPCWSTR log_prefix);
   virtual          ~CReportSelect(void);
   //--- clear
   virtual void      Clear(void);
   //--- intialization
   MTAPIRES          Initialize(IMTReportAPI *api);
   //--- request
   IMTDatasetRequest* Request(void)                            { return(m_request); }
  };
//+------------------------------------------------------------------+

