//+------------------------------------------------------------------+
//|                                         MetaTrader 5 Text Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Logger.h"
//+------------------------------------------------------------------+
//| Global objects                                                   |
//+------------------------------------------------------------------+
CLogger  ExtLogger;
//+------------------------------------------------------------------+
//| Constructor                                                       |
//+------------------------------------------------------------------+
CLogger::CLogger() : m_gateway(NULL)
  {
//--- clear the buffer
   ZeroMemory(m_prebuf,sizeof(m_prebuf));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLogger::~CLogger(void)
  {
  }
//+------------------------------------------------------------------+
//| Set the gateway interface                                        |
//+------------------------------------------------------------------+
void CLogger::SetGateway(IMTGatewayAPI *gateway)
  {
//--- lock
   m_sync.Lock();
//--- remember the gateway interface
   m_gateway=gateway;
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Record to logs                                                   |
//+------------------------------------------------------------------+
void CLogger::Out(int32_t code,LPCWSTR format,...)
  {
   va_list arg_ptr;
//--- lock
   m_sync.Lock();
//--- if there is a gateway interface, record an entry to the journal
   if(m_gateway)
     {
      //--- parse the parameters
      va_start(arg_ptr,format);
      //--- form the string
      _vsnwprintf_s(m_prebuf,_countof(m_prebuf)-1,_TRUNCATE,format,arg_ptr);
      //--- end of the list of arguments
      va_end(arg_ptr);
      //--- record to the journal
      m_gateway->LoggerOutString(code,m_prebuf);
     }
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Record to logs without formatting                                |
//+------------------------------------------------------------------+
void CLogger::OutString(int32_t code,LPCWSTR string)
  {
   if(!string)
      return;
//--- lock
   m_sync.Lock();
//--- if there is a gateway interface, record an entry to the journal
   if(m_gateway)
     {
      //--- record to the journal
      m_gateway->LoggerOutString(code,string);
     }
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
