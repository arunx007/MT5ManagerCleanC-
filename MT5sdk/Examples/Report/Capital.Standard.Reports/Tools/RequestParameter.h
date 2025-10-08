//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Report request parameter class                                   |
//+------------------------------------------------------------------+
class CRequestParameter
  {
private:
   IMTReportAPI     &m_api;                        // report API interface
   IMTConParam      *m_param;                      // parameter interface

public:
   explicit          CRequestParameter(IMTReportAPI &api);
                    ~CRequestParameter(void);
   //--- get report request parameter string value
   MTAPIRES          ParamGetString(LPCWSTR &value,LPCWSTR param_name) const;
   //--- get report request parameter groups value
   MTAPIRES          ParamGetGroups(LPCWSTR &value,LPCWSTR param_name) const;

private:
   //--- get report request parameter
   MTAPIRES          ParamGet(LPCWSTR param_name) const;
  };
//+------------------------------------------------------------------+
