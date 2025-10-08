//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportParameter.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportParameter::CReportParameter(void) :
   m_report(nullptr),m_param(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportParameter::~CReportParameter(void)
  {
//--- clear
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CReportParameter::Clear(void)
  {
//--- release report configuration
   if(m_report)
     {
      m_report->Release();
      m_report=nullptr;
     }
//--- release parameter
   if(m_param)
     {
      m_param->Release();
      m_param=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CReportParameter::Initialize(IMTReportAPI &api)
  {
//--- clear
   Clear();
//--- create report configuration
   m_report=api.ReportCreate();
   if(!m_report)
      return(MT_RET_ERR_MEM);
//--- create parameter
   m_param=api.ParamCreate();
   if(!m_param)
      return(MT_RET_ERR_MEM);
//--- get report configuration
   const MTAPIRES res=api.ReportCurrent(m_report);
   if(res!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| get report parameter int value                                   |
//+------------------------------------------------------------------+
MTAPIRES CReportParameter::ValueInt(int64_t &value,LPCWSTR param_name,LPCWSTR value_default)
  {
//--- checks
   if(!m_report || !m_param || !param_name || !*param_name || !value_default || !*value_default)
      return(MT_RET_ERR_PARAMS);
//--- get parameter
   const MTAPIRES res=ParamGet(param_name,IMTConParam::TYPE_INT,value_default);
   if(res!=MT_RET_OK)
      return(res);
//--- get parameter value
   value=m_param->ValueInt();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| get report parameter string value                                |
//+------------------------------------------------------------------+
MTAPIRES CReportParameter::ValueString(LPCWSTR &value,LPCWSTR param_name,LPCWSTR value_default)
  {
//--- checks
   if(!m_report || !m_param || !param_name || !*param_name || !value_default)
      return(MT_RET_ERR_PARAMS);
//--- get parameter
   const MTAPIRES res=ParamGet(param_name,IMTConParam::TYPE_STRING,value_default);
   if(res!=MT_RET_OK)
      return(res);
//--- get parameter value
   value=m_param->ValueString();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| get report parameter                                             |
//+------------------------------------------------------------------+
MTAPIRES CReportParameter::ParamGet(LPCWSTR param_name,const uint32_t type,LPCWSTR value_default)
  {
//--- checks
   if(!m_report || !m_param || !param_name || !*param_name || !value_default)
      return(MT_RET_ERR_PARAMS);
//--- get parameter
   MTAPIRES res=m_report->ParameterGet(param_name,m_param);
   if(res!=MT_RET_OK)
     {
      //--- fill parameter manually
      if((res=m_param->Name(param_name))!=MT_RET_OK)
         return(res);
      //--- set type
      if((res=m_param->Type(type))!=MT_RET_OK)
         return(res);
      //--- set default value
      if((res=m_param->Value(value_default))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
