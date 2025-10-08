//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\CurrencyConverterCache.h"
//+------------------------------------------------------------------+
//| Default values                                                   |
//+------------------------------------------------------------------+
#ifdef _DEBUG
#define DEFAULT_GROUPS                    L"*"        // default: groups to use
#else
#define DEFAULT_GROUPS                    L"*,!demo*,!contest*"
#endif
#define DEFAULT_LEAD_SOURCE               L"*"        // default: lead source
#define DEFAULT_LEAD_CAMPAIGN             L"*"        // default: lead campaign
//+------------------------------------------------------------------+
//| Report with currency base class                                  |
//+------------------------------------------------------------------+
class CCurrencyReport : public IMTReportContext
  {
protected:
   const uint32_t    m_types;                         // report types mask
   IMTReportAPI     *m_api;                           // report api
   CReportParameter  m_params;                        // report parameters
   CCurrencyConverterCache m_currency;                // currency converter

public:
                     CCurrencyReport(const MTReportInfo &info);
   virtual          ~CCurrencyReport(void);
   //--- release
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

protected:
   //--- clear overridable
   virtual void      Clear(void)=0;
   //--- get report parameters and create caches overridable
   virtual MTAPIRES  Prepare(void)=0;
   //--- request data from server and compute statistics overridable
   virtual MTAPIRES  Calculate(void)=0;
   //--- prepare all graphs to be shown overridable
   virtual MTAPIRES  PrepareGraphs(void)  { return(MT_RET_ERR_NOTIMPLEMENT); }
   //--- write result overridable
   virtual MTAPIRES  WriteResult(void)    { return(MT_RET_ERR_NOTIMPLEMENT); }
   //--- error logging
   template<typename... Args>
   MTAPIRES          LogError(const MTAPIRES res,LPCWSTR msg,Args... args) const;

private:
   //--- inner and outer clear
   void              ClearAll(void);
   //--- inner clear
   void              ClearInner(void);
   //--- inner prepare
   MTAPIRES          PrepareInner(void);
   //--- error log type
   static uint32_t   ErrorLogType(const MTAPIRES res) { return(res==MT_RET_OK_NONE || res==MT_RET_ERR_CANCEL ? MTLogOK : MTLogErr); }
  };
//+------------------------------------------------------------------+
//| error logging                                                    |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CCurrencyReport::LogError(const MTAPIRES res,LPCWSTR msg,Args... args) const
  {
//---- check error
   if(res==MT_RET_OK)
      return(res);
//--- check api pointer
   if(!m_api)
      return(MT_RET_ERROR);
//--- reason depending on error code
   LPCWSTR reason=L"failed, error:";
   switch(res)
     {
      case MT_RET_OK_NONE:
         reason=L"finished, reason:";
         break;
      case MT_RET_ERR_CANCEL:
         reason=L"canceled, reason:";
         break;
     }
//--- format error message
   CMTStr256 str;
   str.Format(L"%s %s %s (%u)",msg ? msg : L"something",reason,SMTFormat::FormatError(res),res);
//--- log error message
   m_api->LoggerOut(ErrorLogType(res),str.Str(),args...);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
