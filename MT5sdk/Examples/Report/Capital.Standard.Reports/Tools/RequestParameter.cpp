//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "RequestParameter.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CRequestParameter::CRequestParameter(IMTReportAPI &api) :
   m_api(api),m_param(api.ParamCreate())
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CRequestParameter::~CRequestParameter(void)
  {
//--- release parameter
   if(m_param)
     {
      m_param->Release();
      m_param=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| get report request parameter string value                        |
//+------------------------------------------------------------------+
MTAPIRES CRequestParameter::ParamGetString(LPCWSTR &value,LPCWSTR param_name) const
  {
//--- check arguments
   if(!param_name || !*param_name)
      return(MT_RET_ERR_PARAMS);
//--- get report request parameter
   const MTAPIRES res=ParamGet(param_name);
   if(res!=MT_RET_OK)
      return(res);
//--- get paramter string value
   value=m_param->ValueString();
   return(value ? MT_RET_OK : MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| get report request parameter groups value                         |
//+------------------------------------------------------------------+
MTAPIRES CRequestParameter::ParamGetGroups(LPCWSTR &value,LPCWSTR param_name) const
  {
//--- check arguments
   if(!param_name || !*param_name)
      return(MT_RET_ERR_PARAMS);
//--- get report request parameter
   const MTAPIRES res=ParamGet(param_name);
   if(res!=MT_RET_OK)
      return(res);
//--- get paramter groups value
   value=m_param->ValueGroups();
   return(value ? MT_RET_OK : MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| get report request parameter                                     |
//+------------------------------------------------------------------+
MTAPIRES CRequestParameter::ParamGet(LPCWSTR param_name) const
  {
//--- check arguments
   if(!param_name || !*param_name)
      return(MT_RET_ERR_PARAMS);
//--- check parameter
   if(!m_param)
      return(MT_RET_ERR_MEM);
//--- get report request parameter
   return(m_api.ParamGet(param_name,m_param));
  }
//+------------------------------------------------------------------+
