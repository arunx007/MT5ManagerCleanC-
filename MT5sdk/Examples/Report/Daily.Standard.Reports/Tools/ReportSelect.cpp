//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportSelect.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportSelect::CReportSelect(const LPCWSTR log_prefix) :
   m_api(nullptr),m_request(nullptr),m_dataset(nullptr),m_log(log_prefix)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportSelect::~CReportSelect(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CReportSelect::Clear(void)
  {
//--- reset api interface
   m_api=nullptr;
//--- request interface
   if(m_request)
     {
      m_request->Release();
      m_request=nullptr;
     }
//--- dataset interface
   m_dataset=nullptr;
//--- clear logger
   m_log.Clear();
  }
//+------------------------------------------------------------------+
//| Report initialization                                            |
//+------------------------------------------------------------------+
MTAPIRES CReportSelect::Initialize(IMTReportAPI *api)
  {
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- store api interface
   m_api=api;
//--- initialize logger
   m_log.Initialize(api);
//--- create interfaces
   if(!(m_request=m_api->DatasetRequestCreate()) ||
      !(m_dataset=m_api->DatasetAppend()))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
