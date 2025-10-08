//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
//---
CLogger  ExtLogger;
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLogger::CLogger(void)
  {
   ZeroMemory(m_prebuf,sizeof(m_prebuf));
   m_path[0]='\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLogger::~CLogger(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialize Logger                                                |
//+------------------------------------------------------------------+
bool CLogger::Initialize(void)
  {
   bool res=false;
//--- get application folder
   wchar_t path[MAX_PATH]={0},*ptr=NULL;
   GetModuleFileNameW(NULL,path,_countof(path));
//--- remove filename
   if((ptr=wcsrchr(path,L'\\'))!=NULL)
      *(ptr+1)=L'\0';
//--- add subfolder name
   CMTStr::Append(path,_countof(path),L"Logs");
//--- lock
   m_sync.Lock();
//--- remember path
   CMTStr::Copy(m_path,_countof(m_path),path);
//--- create folder
   CMTStr str_path(path,_countof(path));
   str_path.Assign(m_path,_countof(m_path));
   res=CMTFile::DirectoryCreate(str_path);
//--- unlock
   m_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Print logs                                                       |
//+------------------------------------------------------------------+
void CLogger::Out(int32_t code,LPCWSTR msg,...)
  {
   va_list arg_ptr;
//---
   va_start(arg_ptr,msg);
   OutArgPtr(code,msg,arg_ptr);
   va_end(arg_ptr);
  }
//+------------------------------------------------------------------+
//| Print logs                                                       |
//+------------------------------------------------------------------+
void CLogger::OutArgPtr(int32_t code,LPCWSTR msg,va_list arg_ptr)
  {
   CMTFile    out;
   CMTStrPath path;
   SYSTEMTIME st  ={0};
   int32_t        len =0;
   LPCWSTR    pmsg=NULL;
//--- check
   if(msg==NULL)
      return;
//--- lock
   m_sync.Lock();
//--- current time
   GetLocalTime(&st);
//--- format all records
   len=_snwprintf_s(m_prebuf,_countof(m_prebuf)-1,_TRUNCATE,L"%d\t%02d:%02d:%02d\t",code,st.wHour,st.wMinute,st.wSecond);
   pmsg=m_prebuf+len;
   len+=vswprintf_s(m_prebuf+len,_countof(m_prebuf)-len,msg,arg_ptr);
//--- open file
   path.Format(L"%s\\%04d%02d%02d.log",m_path,st.wYear,st.wMonth,st.wDay);
   if(!out.Open(path.Str(),GENERIC_WRITE,FILE_SHARE_READ,OPEN_EXISTING))
      out.Open(path.Str(),GENERIC_WRITE,FILE_SHARE_READ,CREATE_NEW);
//--- write to file
   if(out.IsOpen())
     {
      if(out.Seek(0,FILE_END)==0)
         out.Write("\xFF\xFE",2);
      out.Write(m_prebuf,len*sizeof(wchar_t));
      out.Write(L"\r\n",4);
      out.Close();
     }
//--- print to stdout
   wprintf_s(L"%s",m_prebuf);
   wprintf_s(L"\r\n");
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Print logs without formatting                                    |
//+------------------------------------------------------------------+
void CLogger::OutString(int32_t code,LPCWSTR msg)
  {
   CMTFile    out;
   CMTStrPath path;
   SYSTEMTIME st={0};
//--- check
   if(msg==NULL)
      return;
//--- lock
   m_sync.Lock();
//--- current time
   GetLocalTime(&st);
//--- format all records
   const int32_t prefix_len=_snwprintf_s(m_prebuf,_countof(m_prebuf)-1,_TRUNCATE,L"%d\t%02d:%02d:%02d\t",code,st.wHour,st.wMinute,st.wSecond);
   if(prefix_len<0)
     {
      m_sync.Unlock();
      return;
     }
//--- copy message string
   const errno_t err=wcsncpy_s(m_prebuf+prefix_len,_countof(m_prebuf)-prefix_len-1,msg,_TRUNCATE);
   if(err!=0)
     {
      m_sync.Unlock();
      return;
     }
//--- итоговая длина
   const uint32_t len=(uint32_t)wcslen(m_prebuf);
//--- open file
   path.Format(L"%s\\%04d%02d%02d.log",m_path,st.wYear,st.wMonth,st.wDay);
   if(!out.Open(path.Str(),GENERIC_WRITE,FILE_SHARE_READ,OPEN_EXISTING))
      out.Open(path.Str(),GENERIC_WRITE,FILE_SHARE_READ,CREATE_NEW);
//--- write to file
   if(out.IsOpen())
     {
      if(out.Seek(0,FILE_END)==0)
         out.Write("\xFF\xFE",2);
      out.Write(m_prebuf,len*sizeof(wchar_t));
      out.Write(L"\r\n",4);
      out.Close();
     }
//--- print to stdout
   wprintf_s(L"%s",m_prebuf);
   wprintf_s(L"\r\n");
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
