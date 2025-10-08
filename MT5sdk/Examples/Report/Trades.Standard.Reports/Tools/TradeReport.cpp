//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeReport.h"
#include "ReportTimer.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeReport::CTradeReport(const MTReportInfo &info) :
   m_types(info.types),m_api(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeReport::~CTradeReport(void)
  {
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CTradeReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CTradeReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
//--- check for null
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- check for type
   if(!(type&m_types))
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- remember pointer to report api
   m_api=api;
   m_log.Initialize(api);
//--- start timer
   const CReportTimer timer;
//--- prepare report
   MTAPIRES res=Prepare();
   if(res!=MT_RET_OK)
     {
      res=m_log.Error(res,L"Report preparation");
      //--- clear
      ClearAll();
      return(res);
     }
//--- write report 
   if((res=Write())!=MT_RET_OK)
     {
      res=m_log.Error(res,L"Report writing");
      //--- clear
      ClearAll();
      return(res);
     }
//--- clear
   ClearAll();
//--- log result
   return(api->LoggerOut(MTLogOK,L"Generated in %I64u ms",timer.Elapsed()));
  }
//+------------------------------------------------------------------+
//| Inner and outer clear                                            |
//+------------------------------------------------------------------+
void CTradeReport::ClearAll(void)
  {
//--- outer clear
   Clear();
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Inner clear                                                      |
//+------------------------------------------------------------------+
void CTradeReport::ClearInner(void)
  {
//--- zero api pointer
   m_api=nullptr;
//--- clear logger
   m_log.Clear();
  }
//+------------------------------------------------------------------+
