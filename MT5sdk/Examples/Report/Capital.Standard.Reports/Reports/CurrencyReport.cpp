//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "CurrencyReport.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CCurrencyReport::CCurrencyReport(const MTReportInfo &info) :
   m_types(info.types),m_api(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CCurrencyReport::~CCurrencyReport(void)
  {
//--- clear inner
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CCurrencyReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CCurrencyReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
//--- check for null
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- check for type
   if(!(type&m_types))
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- remember pointer to report api
   m_api=api;
//--- save current time
   const CReportTimer timer;
//--- inner prepare
   MTAPIRES res=PrepareInner();
   if(res!=MT_RET_OK)
     {
      res=LogError(res,L"Report parameters preparation");
      //--- clear
      Clear();
      return(res);
     }
//--- capture plugin parameters
   if((res=Prepare())!=MT_RET_OK)
     {
      res=LogError(res,L"Report generation preparation");
      //--- clear
      ClearAll();
      return(res);
     }
//--- request data from server
   res=Calculate();
   if(res==MT_RET_OK_NONE)
     {
      //--- log no data
      m_api->LoggerOutString(MTLogOK,L"Calculation finished: no data");
     }
   else
      if(res!=MT_RET_OK)
        {
         res=LogError(res,L"Report calculation");
         //--- clear
         ClearAll();
         return(res);
        }
//--- depending on report type
   if(type==MTReportInfo::TYPE_DASHBOARD)
     {
      //--- prepare all graphs to be shown
      if((res=PrepareGraphs())!=MT_RET_OK)
        {
         res=LogError(res,L"Report graphs preparation");
         //--- clear
         ClearAll();
         return(res);
        }
     }
   else
     {
      //--- write result
      if((res=WriteResult())!=MT_RET_OK)
        {
         res=LogError(res,L"Report result writing");
         //--- clear
         ClearAll();
         return(res);
        }
     }
//--- clear
   ClearAll();
//--- log result
   return(api->LoggerOut(MTLogOK,L"Generated in %I64u ms",timer.Elapsed()));
  }
//+------------------------------------------------------------------+
//| Inner and outer clear                                            |
//+------------------------------------------------------------------+
void CCurrencyReport::ClearAll(void)
  {
//--- outer clear
   Clear();
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Inner clear                                                      |
//+------------------------------------------------------------------+
void CCurrencyReport::ClearInner(void)
  {
//--- zero api pointer
   m_api=nullptr;
//--- clear objects
   m_currency.Clear();
   m_params.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CCurrencyReport::PrepareInner(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initalize report parameters
   MTAPIRES res=m_params.Initialize(*m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
