//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Network\UniFeederSocket.h"
//+------------------------------------------------------------------+
//| Class of external connection data processing                     |
//+------------------------------------------------------------------+
class CDataSource
  {
public:
   //--- state of the external connection
   enum
     {
      STATE_DISCONNECTED=0x00,        // there is no connection
      STATE_CONNECTSTART=0x01,        // starting connection
      STATE_CONNECTED   =0x02,        // connection has been established
     };

private:
   //--- constants
   enum constants
     {
      THREAD_TIMEOUT   =60,           // timeout of thread completion, s
      READ_BUFFER_MAX  =1024*1024,    // maximal buffer size
      READ_BUFFER_STEP =16*1024,      // step of the buffer reallocation
      BUFFER_SIZE      =128*1024,     // size of the parsing buffer
      LOGIN_COUNT_MAX  =3             // maximal number of authorization attempts
     };

private:
   //--- pointer to the gateway interface
   IMTGatewayAPI    *m_gateway;
   //--- data processing thread
   CMTThread         m_thread;
   //--- sign of thread operation
   volatile long     m_workflag;
   //--- sign of initialization (receipt of connection settings)
   volatile long     m_initalized;
   //--- state of the external connection
   volatile long     m_state;
   //--- access to Universal DDE Connector
   CUniFeederSocket *m_unisocket;      // feeder socket
   char              m_address[128];   // server address
   char              m_login[64];      // login
   char              m_password[64];   // password
   char             *m_symbols;        // symbols
   //--- buffer for ticks
   MTTickArray       m_ticks_buffer;

public:
   //--- constructor/destructor
                     CDataSource();
                    ~CDataSource(void);
   //--- start/stop of external connection data processing thread
   bool              Start(IMTGatewayAPI *gateway);
   bool              Shutdown(void);
   //--- initialization of external connection
   bool              Init(const IMTConFeeder *config);
   //--- connection check
   bool              Check(void);
   //--- state of the external connection
   LONG              StateGet(void);
   LONG              StateSet(LONG state);
   void              StateTraffic(uint32_t received_bytes,uint32_t sent_bytes);

private:
   //--- external connection data processing thread
   static uint32_t __stdcall ProcessThreadWrapper(LPVOID param);
   void              ProcessThread(void);
   //--- external connection
   bool              ProcessConnect(void);
   //--- processing of external connection data
   bool              ProcessData(void);
  };
//+------------------------------------------------------------------+
