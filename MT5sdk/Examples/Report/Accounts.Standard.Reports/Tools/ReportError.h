//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Report  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once

//--- PVS Studio helper
//V_FORMATTED_IO_FUNC, class:CReportError, function:Write, format_arg:3, ellipsis_arg:4, propagate_on_virtual

//+------------------------------------------------------------------+
//| Class for generate error html page                               |
//+------------------------------------------------------------------+
class CReportError
  {
private:
   //--- error page html template
   static const wchar_t s_template[];

public:
   //--- generate an error page
   static MTAPIRES   Write(IMTReportAPI *api,LPCWSTR name,LPCWSTR msg,...);

private:
   static MTAPIRES   WriteArgPtr(IMTReportAPI *api,LPCWSTR name,LPCWSTR msg,va_list arg_ptr);
  };
//+------------------------------------------------------------------+
//| Error page template                                              |
//+------------------------------------------------------------------+
__declspec(selectany) const wchar_t CReportError::s_template[]=
   L"<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">"
   L"<html>"
   L"<head>"
      L"<title><mt5:report_name/></title>"
      L"<style type=\"text/css\">"
         L"<!--"
         L"@media screen {"
            L"td { font: 8pt Tahoma,Arial; }"
         L"}"
         L"@media print {"
            L"td { font: 7pt Tahoma,Arial; }"
         L"}"
         L"body {margin:1px;margin:0;padding:0;height:100%;}"
         L"table.message {height:100%;width:100%;margin:0;padding:0;border:none;}"
         L"table.message td {vertical-align:middle;text-align:center;font-size:24pt;}"
         L"//-->"
      L"</style>"
   L"</head>"
   L"<body>"
   L"<table class=\"message\"><tr><td><p style=\"font-family: Tahoma,Helvetica,sans-serif; font-size:24px; color:#bbbbbb;\">"
         L"<mt5:error_message />"
      L"</p></td></tr></table>"
   L"</body>"
   L"</html>";
//+------------------------------------------------------------------+
//| Generate an error page                                           |
//+------------------------------------------------------------------+
inline MTAPIRES CReportError::Write(IMTReportAPI *api,LPCWSTR name,LPCWSTR msg,...)
  {
   MTAPIRES res    =MT_RET_ERROR;
   va_list  arg_ptr;
//--- checks
   if(!api || !name || !msg)
      return(MT_RET_ERR_PARAMS);
//--- formating
   va_start(arg_ptr,msg);
   res=WriteArgPtr(api,name,msg,arg_ptr);
   va_end(arg_ptr);
//--- done
   return(res);
  }
//+------------------------------------------------------------------+
//| Generate error page                                              |
//+------------------------------------------------------------------+
inline MTAPIRES CReportError::WriteArgPtr(IMTReportAPI *api,LPCWSTR name,LPCWSTR msg,va_list arg_ptr)
  {
   MTAPIRES res         =MT_RET_ERROR;
   MTAPISTR tag         ={0};
   uint32_t counter     =0;
   wchar_t  prebuf[2048]={0};
//--- checks
   if(!api || !name || !arg_ptr)
      return(MT_RET_ERR_PARAMS);
//--- clear api for new report page
   api->Clear();
//--- get error msg
   if(_vsnwprintf_s(prebuf,_countof(prebuf),msg,arg_ptr)<1)
      return(MT_RET_ERROR);
//--- load template
   if((res=api->HtmlTplLoad(s_template))!=MT_RET_OK)
      return(res);
//--- set tags
   while(api->HtmlTplNext(tag,&counter)==MT_RET_OK)
     {
      //--- is it name tag?
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
         if((res=api->HtmlWriteSafe(name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
           {
            api->Clear();
            return(res);
           }
      //--- is it error message tag?
      if(CMTStr::CompareNoCase(tag,L"error_message")==0)
         if((res=api->HtmlWriteSafe(prebuf,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
           {
            api->Clear();
            return(res);
           }
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
