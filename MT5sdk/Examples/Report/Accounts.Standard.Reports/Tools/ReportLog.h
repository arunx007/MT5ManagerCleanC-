//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Report log helper class                                          |
//+------------------------------------------------------------------+
class CReportLog
  {
   const LPCWSTR     m_prefix;      // log prefix
   IMTReportAPI     *m_api;         // report api

public:
                     CReportLog(const LPCWSTR prefix=L"") : m_prefix(prefix),m_api(nullptr) {}
   //--- initialization/clear
   void              Initialize(IMTReportAPI *api)    { m_api=api; }
   void              Clear(void)                      { m_api=nullptr; }
   //--- success logging
   template<typename... Args>
   MTAPIRES          Ok(LPCWSTR msg,Args... args) const;
   template<typename... Args>
   MTAPIRES          OkTime(LPCWSTR msg,int64_t from,int64_t to,Args... args) const;
   //--- error logging
   template<typename... Args>
   MTAPIRES          Error(MTAPIRES res,LPCWSTR msg,Args... args) const;
   template<typename... Args>
   MTAPIRES          ErrorTime(MTAPIRES res,LPCWSTR msg,int64_t from,int64_t to,Args... args) const;
   //--- error log type
   static uint32_t   ErrorLogType(const MTAPIRES res) { return(res==MT_RET_OK_NONE || res==MT_RET_ERR_CANCEL ? MTLogOK : MTLogErr); }
  };
//+------------------------------------------------------------------+
//| success logging                                                  |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportLog::Ok(LPCWSTR msg,Args... args) const
  {
//--- check api pointer
   if(!m_api)
      return(MT_RET_ERROR);
//--- log message
   if(!*m_prefix)
      return(m_api->LoggerOut(MTLogOK,msg ? msg : L"something",args...));
//--- log message with prefix
   CMTStr256 str(m_prefix);
   if(msg)
      str.Append(msg);
   return(m_api->LoggerOut(MTLogOK,str.Str(),args...));
  }
//+------------------------------------------------------------------+
//| success logging with time                                        |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportLog::OkTime(LPCWSTR msg,const int64_t from,const int64_t to,Args... args) const
  {
//--- check api pointer
   if(!m_api)
      return(MT_RET_ERROR);
//--- format message
   CMTStr32 sfrom,sto;
   CMTStr256 str;
   str.Format(L"%s%s %s-%s",m_prefix,msg ? msg : L"something",
      SMTFormat::FormatDateTime(sfrom,from,true,true),SMTFormat::FormatDateTime(sto,to,true,true));
//--- log message
   return(m_api->LoggerOut(MTLogOK,str.Str(),args...));
  }
//+------------------------------------------------------------------+
//| error logging                                                    |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportLog::Error(const MTAPIRES res,LPCWSTR msg,Args... args) const
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
   str.Format(L"%s%s %s %s (%u)",m_prefix,msg ? msg : L"something",reason,SMTFormat::FormatError(res),res);
//--- log error message
   m_api->LoggerOut(ErrorLogType(res),str.Str(),args...);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| error logging with time                                          |
//+------------------------------------------------------------------+
template<typename... Args> MTAPIRES CReportLog::ErrorTime(const MTAPIRES res,LPCWSTR msg,const int64_t from,const int64_t to,Args... args) const
  {
//---- check error
   if(res==MT_RET_OK)
      return(res);
//--- check api pointer
   if(!m_api)
      return(MT_RET_ERROR);
//--- format error message
   CMTStr32 sfrom,sto;
   CMTStr256 str;
   str.Format(L"%s%s %s-%s failed, error: %s (%u)",m_prefix,msg ? msg : L"something",
      SMTFormat::FormatDateTime(sfrom,from,true,true),SMTFormat::FormatDateTime(sto,to,true,true),SMTFormat::FormatError(res),res);
//--- log error message
   m_api->LoggerOut(ErrorLogType(res),str.Str(),args...);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
