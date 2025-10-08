//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\TextReader.h"
//+------------------------------------------------------------------+
//| Structure contains report parameters and helper functions        |
//+------------------------------------------------------------------+
struct ReportContext
  {
   CMTStrPath        server_path;
   CMTStrPath        base_directory;
   //--- read from API
   MTAPIRES          Initialize(IMTReportAPI *api);
   //--- generic report reading from CSV files
   template <typename T>
   static MTAPIRES   Generate(IMTReportAPI *api,LPCWSTR name,bool (*Fill)(CTextReader &reader,T &record,const ReportContext &context));
   //--- check date string in the bounds
   static bool       CheckDate(LPCWSTR date_str,int64_t from,int64_t to);
  };
//+------------------------------------------------------------------+
//| Initialize report parameters                                     |
//+------------------------------------------------------------------+
inline MTAPIRES ReportContext::Initialize(IMTReportAPI *api)
  {
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- clear context
   server_path.Clear();
   base_directory.Clear();
//---
   MTAPIRES      res   =MT_RET_OK;
   IMTConReport *report=NULL;
   IMTConParam  *param =NULL;
//--- allocations
   if((report=api->ReportCreate())!=NULL && (param=api->ParamCreate())!=NULL)
     {
      //--- get base directory
      if(api->ReportCurrent(report)==MT_RET_OK && report->ParameterGet(L"Base Directory",param)==MT_RET_OK)
         base_directory.Assign(param->ValueString());
      else
         base_directory.Assign(DEFAULT_BASE_DIRECTORY);
      //--- get server path
      if(GetModuleFileNameW(NULL,server_path.Buffer(),server_path.Max()))
        {
         server_path.Refresh();
         int32_t pos=server_path.FindRChar(L'\\');
         if(pos>0)
            server_path.Trim(pos);
        }
      else
         res=MT_RET_ERROR;
     }
   else
      res=MT_RET_ERR_MEM;
//--- cleanup
   if(report)
      report->Release();
   if(param)
      param ->Release();
//--- result
   return(res);
  }
template<typename T>
//+------------------------------------------------------------------+
//| Template walks over directories, checks the date and read files, |
//| that matches 'name' argument. For each CSV line it calls 'fill'  |
//| function and puts resulting 'T' record into the report via 'api' |
//+------------------------------------------------------------------+
inline MTAPIRES ReportContext::Generate(IMTReportAPI *api,LPCWSTR name,bool (*fill)(CTextReader &reader,T &record,const ReportContext &context))
  {
//--- checks
   if(!api || !name || !fill)
      return(MT_RET_ERR_PARAMS);
//--- initialize context
   MTAPIRES      res;
   ReportContext context;
   if((res=context.Initialize(api))!=MT_RET_OK)
      return(res);
//---
   CMTStrPath      path;
   CMTStr1024      line;
   WIN32_FIND_DATAW find={};
   HANDLE          handle=NULL;
//--- find all subdirectories of base_directory 
   path.Format(L"%s\\Reports\\NFA.Reports\\%s\\*",context.server_path.Str(),context.base_directory.Str());
   if((handle=FindFirstFileW(path.Str(),&find))==INVALID_HANDLE_VALUE)
     {
      api->LoggerOut(MTLogErr,L"report data files doesn't found [%u]",GetLastError());
      return(MT_RET_ERR_NOTFOUND);
     }
//--- walk by subdirectories until generating is successfully
   do
     {
      //--- check is directory and not '.' or '..'
      if(!(find.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) || CMTStr::Compare(find.cFileName,L".")==0 || CMTStr::Compare(find.cFileName,L"..")==0)
         continue;
      //--- skip subdirectories, that out of date bounds
      if(!CheckDate(find.cFileName,api->ParamFrom(),api->ParamTo()))
         continue;
      //--- open file
      path.Format(L"%s\\Reports\\NFA.Reports\\%s\\%s\\%s_%s.txt",context.server_path.Str(),context.base_directory.Str(),find.cFileName,name,find.cFileName);
      CTextReader reader;
      if(!reader.Open(path.Str()))
        {
         api->LoggerOut(MTLogWarn,L"error reading report file [%s][%u]",path.Str(),GetLastError());
         continue;
        }
      //--- skip header
      reader.ReadLine();
      //--- read line-by-line
      while(reader.ReadLine())
        {
         T record={};
         //--- fill record
         if(!fill(reader,record,context))
            continue;
         //--- write record to report
         if((res=api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK)
            break;
        }
      reader.Close();
     }
   while(res==MT_RET_OK && FindNextFileW(handle,&find));
//--- release find handle 
   FindClose(handle);
//---
   return(res);
  }
//+------------------------------------------------------------------+
//| Compare dates                                                    |
//+------------------------------------------------------------------+
inline bool ReportContext::CheckDate(LPCWSTR date_str,const int64_t from,const int64_t to)
  {
//--- parse time and check
   if(date_str)
     {
      tm ttm={};
      if(SPluginHelpers::ParseFileDate(date_str,ttm))
        {
         int64_t date_ctm=SMTTime::MakeTime(&ttm);
         return(date_ctm>=from && date_ctm<=to);
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
