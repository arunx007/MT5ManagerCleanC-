//+------------------------------------------------------------------+
//|                                                     MetaTrader 5 |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <process.h>
//+------------------------------------------------------------------+
//| Thread manipulation class                                        |
//+------------------------------------------------------------------+
class CMTThread
  {
private:
   HANDLE            m_thread;

public:
   CMTThread(void) : m_thread(NULL)
     {
     }

   ~CMTThread(void)
     {
      //--- wait for shutdown
      Shutdown();
      //--- close handle
      if(m_thread)
        {
         CloseHandle(m_thread);
         m_thread=NULL;
        }
     }

   bool Start(unsigned (__stdcall *thread_func)(void*),void *thread_param,const uint32_t stack_size)
     {
      DWORD id=0;
      //--- thread has been started...
      if(m_thread)
        {
         GetExitCodeThread(m_thread,&id);
         //--- still active
         if(id==STILL_ACTIVE)
            return(false);
         //--- close handle
         CloseHandle(m_thread);
         m_thread=NULL;
        }
      //--- start thread
      m_thread=(HANDLE)_beginthreadex(NULL,stack_size,thread_func,(void*)thread_param,STACK_SIZE_PARAM_IS_A_RESERVATION,(uint32_t*)&id);
      if(m_thread==0 || m_thread==INVALID_HANDLE_VALUE)
        {
         m_thread=NULL;
         return(false);
        }
      //--- ok
      return(true);
     }

   bool Shutdown(const uint32_t timeout=INFINITE)
     {
      //--- check
      if(!m_thread)
         return(true);
      //--- wait for thread shutdown
      return(WaitForSingleObject(m_thread,timeout)==WAIT_OBJECT_0);
     }

   void Terminate(void)
     {
      //--- check
      if(m_thread)
        {
         //--- kill thread
         TerminateThread(m_thread,0);
         //--- close handle
         CloseHandle(m_thread);
         m_thread=NULL;
        }
     }

   HANDLE Handle(void) const
     {
      return(m_thread);
     }

   bool IsBusy(void)
     {
      //--- check
      if(!m_thread)
         return(false);
      //--- request state
      DWORD res=0;

      GetExitCodeThread(m_thread,&res);
      if(res==STILL_ACTIVE)
         return(true);
      //--- close handle
      CloseHandle(m_thread);
      m_thread=NULL;
      //--- thread finished
      return(false);
     }

   bool Priority(int32_t priority)
     {
      return(m_thread && SetThreadPriority(m_thread,priority));
     }
  };
//+------------------------------------------------------------------+
