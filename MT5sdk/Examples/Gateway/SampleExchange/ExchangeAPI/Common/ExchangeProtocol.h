//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "ExchangeProtocolData.h"
#include "ExchangeContextBase.h"
#include "ExchangeMsgLogin.h"
#include "ExchangeMsgLogout.h"
#include "ExchangeMsgSymbol.h"
#include "ExchangeMsgTick.h"
#include "ExchangeMsgOrder.h"
#include "ExchangeMsgDeal.h"
#include "ExchangeMsgAccountData.h"
#include "ExchangeMsgHeartBeat.h"
#include "ExchangeMsgBook.h"
//+------------------------------------------------------------------+
//| Class of data exchange protocol                                  |
//+------------------------------------------------------------------+
class CExchangeProtocol
  {
private:
   //--- constants
   enum EnConstants
     {
      BUFFER_MAX_SIZE           =1024*1024,        // maximum size of receipt buffer
      BUFFER_REALLOC_STEP       =10*1024           // buffer reallocation step
     };

private:
   //--- context
   CExchangeContextBase &m_context;
   //--- data receipt buffer
   char             *m_buffer;
   uint32_t          m_buffer_datasize;
   uint32_t          m_buffer_maxsize;

public:
   //--- constructor/destructor
                     CExchangeProtocol(CExchangeContextBase &m_context);
                    ~CExchangeProtocol(void);
   //--- shutdown
   void              Shutdown(void);
   //--- notify of data receipt
   bool              OnReceive(const char *buffer,const uint32_t buffer_size);

private:
   //--- process messages
   bool              OnMessage(const char *buffer,const uint32_t buffer_size);
   //--- process login message
   bool              OnMessageLogin(const char *buffer,const uint32_t buffer_size);
   //--- process logout message
   bool              OnMessageLogout(const char *buffer,const uint32_t buffer_size);
   //--- process message with symbol settings
   bool              OnMessageSymbol(const char *buffer,const uint32_t buffer_size);
   //--- process message with tick
   bool              OnMessageTick(const char *buffer,const uint32_t buffer_size);
   //--- process book message
   bool              OnMessageBook(const char *buffer,const uint32_t buffer_size);
   //--- process message with order
   bool              OnMessageOrder(const char *buffer,const uint32_t buffer_size);
   //--- process deal message
   bool              OnMessageDeal(const char *buffer,const uint32_t buffer_size);
   //--- process message with orders request
   bool              OnMessageAccountDataRequest(const char *buffer,const uint32_t buffer_size);
   //--- process message with orders list
   bool              OnMessageAccountData(const char *buffer,const uint32_t buffer_size);
   //--- process HeartBeat message
   bool              OnMessageHeartBeat(const char *buffer,const uint32_t buffer_size);
   //--- process buffer data
   bool              BufferAdd(const char *buffer,const uint32_t buffer_size);
   bool              BufferParse(void);
   bool              BufferAllocate(const uint32_t buffer_size);
   void              BufferClear(void);
  };
//+------------------------------------------------------------------+
