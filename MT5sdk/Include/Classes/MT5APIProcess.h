//+------------------------------------------------------------------+
//|                                                     MetaTrader 5 |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <windows.h>
#include "MT5APIStr.h"
//+------------------------------------------------------------------+
//| Process manipulation class                                       |
//+------------------------------------------------------------------+
class CMTProcess
  {
private:
   HANDLE            m_process;
   HANDLE            m_thread;
   HANDLE            m_std_write;
   HANDLE            m_std_read;
   DWORD             m_process_id;
   DWORD             m_thread_id;

public:
   CMTProcess(void) : m_process(INVALID_HANDLE_VALUE),m_thread(INVALID_HANDLE_VALUE),m_std_write(INVALID_HANDLE_VALUE),m_std_read(INVALID_HANDLE_VALUE),m_process_id(0),m_thread_id(0)
     {
     };

   ~CMTProcess(void)
     {
      Close();
     }

   bool Start(CMTStr &command,const DWORD flags=CREATE_NO_WINDOW,const bool inherit=false)
     {
      //--- close previous
      Close();
      //--- intercept in\out for console threads
      STARTUPINFOW         si={};
      PROCESS_INFORMATION  pi={};
      SECURITY_ATTRIBUTES  sa={ sizeof(sa),NULL,TRUE };

      if(flags & CREATE_NEW_CONSOLE)
        {
         HANDLE read_handle=INVALID_HANDLE_VALUE;
         //--- create pipe for in\out interception
         if(::CreatePipe(&read_handle,&m_std_write,&sa,0))
           {
            HANDLE process=::GetCurrentProcess();
            //--- create new read handle
            if(!::DuplicateHandle(process,read_handle,process,&m_std_read,0,FALSE,DUPLICATE_SAME_ACCESS))
              {
               //--- close all handles
               ::CloseHandle(read_handle);
               Close();
               return(false);
              }
            ::CloseHandle(read_handle);
            //--- fill structure
            si.cb         =sizeof(si);
            si.dwFlags    =STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW;
            si.hStdOutput =m_std_write;
            si.hStdInput  =NULL;
            si.hStdError  =m_std_write;
            si.wShowWindow=SW_HIDE;
           }
        }
      //--- create process
      if(CreateProcessW(NULL,command.Buffer(),NULL,NULL,inherit ? TRUE:FALSE,flags,NULL,NULL,&si,&pi)==FALSE)
        {
         Close();
         return(false);
        }
      //--- save handles
      m_process   =pi.hProcess;
      m_process_id=pi.dwProcessId;
      m_thread    =pi.hThread;
      m_thread_id =pi.dwThreadId;
      //--- ok
      return(true);
     }
   //+------------------------------------------------------------------+
   //| Process close                                                    |
   //+------------------------------------------------------------------+
   void Close(void)
     {
      //--- close in\out handles
      if(m_std_write!=INVALID_HANDLE_VALUE)
        {
         CloseHandle(m_std_write);
         m_std_write=INVALID_HANDLE_VALUE;
        }

      if(m_std_read!=INVALID_HANDLE_VALUE)
        {
         CloseHandle(m_std_read);
         m_std_read=INVALID_HANDLE_VALUE;
        }
      //--- close process handle
      if(m_process!=INVALID_HANDLE_VALUE)
        {
         CloseHandle(m_process);
         m_process=INVALID_HANDLE_VALUE;
         m_process_id=0;
        }
      //--- close process thread handle
      if(m_thread!=INVALID_HANDLE_VALUE)
        {
         CloseHandle(m_thread);
         m_thread=INVALID_HANDLE_VALUE;
         m_thread_id=0;
        }
     }

   DWORD Read(void *buffer,const DWORD length)
     {
      //--- check
      if(m_std_write==INVALID_HANDLE_VALUE || m_std_read==INVALID_HANDLE_VALUE || m_process==INVALID_HANDLE_VALUE || buffer==NULL || length<1)
         return(0);
      //--- read
      DWORD readed=0;

      if(::ReadFile(m_std_read,buffer,length,&readed,NULL)==0)
         readed=0;
      //--- read result
      return(readed);
     }

   bool Priority(DWORD priority_class)
     {
      return(m_process!=INVALID_HANDLE_VALUE && SetPriorityClass(m_process,priority_class));
     }

   bool Wait(const DWORD timeout=INFINITE)
     {
      //--- checks
      if(m_process==INVALID_HANDLE_VALUE)
         return(false);
      //--- waiting
      DWORD res=WaitForSingleObject(m_process,timeout);

      return(res==WAIT_OBJECT_0 ? true:false);
     }

   DWORD ThreadID(void) const
     {
      return(m_thread_id);
     }

   DWORD ExitCode(void)
     {
      DWORD exitCode=0;
      //--- checks
      if(m_process!=INVALID_HANDLE_VALUE)
         GetExitCodeProcess(m_process,&exitCode);
      //--- result
      return(exitCode);
     }

   void Terminate(void)
     {
      //--- check
      if(m_process!=INVALID_HANDLE_VALUE)
        {
         TerminateProcess(m_process,0);
         Close();
        }
     }
  };
//+------------------------------------------------------------------+
