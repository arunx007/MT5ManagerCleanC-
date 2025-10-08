//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportBase.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportBase::CReportBase(const MTReportInfo &info) :
   m_types(info.types),m_api(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportBase::~CReportBase(void)
  {
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CReportBase::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CReportBase::Generate(const uint32_t type,IMTReportAPI *api)
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
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Inner and outer clear                                            |
//+------------------------------------------------------------------+
void CReportBase::ClearAll(void)
  {
//--- outer clear
   Clear();
//--- inner clear
   ClearInner();
  }
//+------------------------------------------------------------------+
//| Inner clear                                                      |
//+------------------------------------------------------------------+
void CReportBase::ClearInner(void)
  {
//--- zero api pointer
   m_api=nullptr;
//--- clear logger
   m_log.Clear();
  }
//+------------------------------------------------------------------+
