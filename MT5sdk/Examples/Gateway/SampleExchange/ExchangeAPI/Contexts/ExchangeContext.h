//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "ExchangeSocket.h"
//+------------------------------------------------------------------+
//| Preliminary declarations                                         |
//+------------------------------------------------------------------+
class CTradeDispatcher;
//+------------------------------------------------------------------+
//| Class of connection context                                      |
//+------------------------------------------------------------------+
class CExchangeContext : public CExchangeContextBase
  {

public:
   //--- context state
   enum EnExchangeContextState
     {
      STATE_STOPPED           =0x00,                     // stopped thread of processing connection messages
      STATE_STARTED           =0x01,                     // started thread of processing connection messages
      STATE_CONNECTED         =0x02,                     // connection established
      STATE_SYNCHRONIZED      =0x03,                     // connection fully synchronized
      STATE_FIRST             =STATE_STOPPED,
      STATE_LAST              =STATE_CONNECTED
     };

private:
   //--- constants
   enum EnConstants
     {
      THREAD_TIMEOUT          =20,                       // processing thread timeout, sec
      THREAD_SLEEP            =50,                       // thread sleep time, msec
      MAX_BUFFER_SEND         =256*1024*1024,            // maximum size of buffer for sending
      MAX_BUFFER_RECEIVE      =10*1024,                  // maximum size of receipt buffer
      HEARBEAT_TIMEOUT        =10,                       // hearbeat timeout
      LOGOUT_TIME             =2000,                     // logout time, msec
      LOGOUT_WAIT             =50                        // logout message wait time, msec
     };

private:
   //--- connection socket
   CExchangeSocket  *m_socket;
   //--- trade dispatcher
   CTradeDispatcher &m_trade_dispatcher;
   //--- protocol
   CExchangeProtocol m_exchange_protocol;
   //--- connection parameters
   wchar_t           m_address[MAX_PATH];
   uint32_t          m_port;
   //--- buffers of packets for sending
   CMTMemPack        m_buffer_send;
   CMTMemPack        m_buffer_send_tmp;
   //--- synchronizer of access to packets for sending
   CMTSync           m_buffer_send_sync;
   //--- buffer of receipt packets
   char             *m_buffer_receive;
   int32_t           m_buffer_receive_max;
   //--- thread of messages processing
   CMTThread         m_thread;
   //--- flag of processing thread
   volatile long     m_thread_workflag;
   //--- time of last activity of processing thread
   volatile int64_t  m_thread_checktime;
   //--- state of connection context
   volatile long     m_state;
   //--- heartbeat time
   volatile int64_t  m_heartbeat_time;

public:
   //--- constructor/destructor
                     CExchangeContext(CTradeDispatcher &trade_dispatcher);
                    ~CExchangeContext(void);
   //--- initialization/shutdown
   bool              Initialize(LPCWSTR address);
   void              Shutdown(void);
   //--- state of connection context
   uint32_t          StateSet(uint32_t state);
   uint32_t          StateGet(void);
   //--- send data
   virtual bool      Send(const char *buffer,const uint32_t buffer_size);
   //--- update activity time
   int64_t           UpdateTime(const int64_t timeout)     { return(m_thread_checktime=_time64(nullptr)+timeout); }
   //--- check context
   bool              Check(const int64_t curtime);
   //--- process login message
   virtual bool      OnMessageLogin(const ExchangeMsgLogin &msg);
   //--- process logout message
   virtual bool      OnMessageLogout(const ExchangeMsgLogout &msg);
   //--- process message with symbol settings
   virtual bool      OnMessageSymbol(const ExchangeMsgSymbol &msg)       { return(true); }
   //--- process message with tick
   virtual bool      OnMessageTick(const ExchangeMsgTick &msg)           { return(true); }
   //--- process message with book
   virtual bool      OnMessageBook(const ExchangeMsgBook &msg)           { return(true); }
   //--- process message with order
   virtual bool      OnMessageOrder(const ExchangeMsgOrder &msg);
   //--- process message with deal
   virtual bool      OnMessageDeal(const ExchangeMsgDeal &msg)           { return(true); }
   //--- process message with account data request
   virtual bool      OnMessageAccountDataRequest(const ExchangeMsgAccountDataRequest &msg);
   //--- process message with account data
   virtual bool      OnMessageAccountData(const ExchangeMsgAccountData &msg) { return(true); }
   //--- process HeartBeat message
   virtual bool      OnMessageHeartBeat(const ExchangeMsgHeartBeat &msg) { return(true); }
   //--- send symbol
   bool              SendSymbol(const ExchangeSymbol &symbol,uint32_t index);
   //--- send tick
   bool              SendTick(const ExchangeTick &tick);
   //--- send order
   bool              SendOrder(const ExchangeOrder &order);
   //--- send deal
   bool              SendDeal(const ExchangeDeal &deal);
   //--- send book
   bool              SendBook(const ExchangeBook &book);
   //--- send account data
   bool              SendAccountData(const ExchangeAccountData &account_data);
   //--- send HeartBeat
   bool              SendHeartBeat(const int64_t curtime);
   //--- send logout
   bool              SendLogout(void);
   bool              SendLogoutSync(void);

private:
   //--- notification methods
   bool              OnConnect(void);
   bool              OnReceive(const char *buffer,const uint32_t buffer_size);
   //--- thread of messages processing
   static uint32_t __stdcall ProcessThreadWrapper(LPVOID param);
   void              ProcessThread(void);
   //--- send/receive data
   bool              SendCheck(bool &data_processed);
   bool              ReceiveCheck(bool &data_processed);
   //--- initialize buffers of data packets
   bool              InitializeBuffers(void);
   //--- get send queue size
   uint32_t          SendSizeGet(void);
  };
//+------------------------------------------------------------------+
