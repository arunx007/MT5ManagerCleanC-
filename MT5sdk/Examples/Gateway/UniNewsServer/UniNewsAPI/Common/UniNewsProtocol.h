//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "UniNewsProtocolData.h"
#include "UniNewsMsg.h"
#include "UniNewsMsgPing.h"
#include "UniNewsMsgLogin.h"
#include "UniNewsMsgLoginResult.h"
#include "UniNewsMsgLogout.h"
#include "UniNewsMsgSubscribe.h"
#include "UniNewsMsgNews.h"
#include "UniNewsContextBase.h"
//+------------------------------------------------------------------+
//| Class of data exchange protocol                                  |
//+------------------------------------------------------------------+
class CUniNewsProtocol
  {

public:
   //--- constants
   enum En—onstants
     {
      MAX_BUFFER_SIZE=512*1024*1024,    // maximum buffer size
      BUFFER_STEP    =256*1024          // buffer allocation step
     };
   //--- salt for hash calculations
   static const BYTE s_salt[32];

private:
   //--- context
   CUniNewsContextBase &m_context;
   //--- buffer for unprocessed data
   char*             m_buffer;
   uint32_t          m_buffer_total;
   uint32_t          m_buffer_max;

public:
   //--- constructor/destructor
                     CUniNewsProtocol(CUniNewsContextBase &m_context);
                    ~CUniNewsProtocol(void);
   //--- shutdown
   void              Shutdown(void);
   //--- notification of getting new data
   bool              OnReceive(const char *buffer,const uint32_t buffer_size);

private:
   //--- process messages
   bool              OnMessage(const TagHeader *message_tag);
   bool              OnMessageLogin(const TagHeader *message_tag);
   bool              OnMessageLoginResult(const TagHeader *message_tag);
   bool              OnMessageLogout(const TagHeader *message_tag);
   bool              OnMessageSubscribe(const TagHeader *message_tag);
   bool              OnMessageNews(const TagHeader *message_tag);
   bool              OnMessagePing(const TagHeader *message_tag);
   //--- parse incoming data
   bool              BufferParse(const char *buffer,const uint32_t buffer_size,uint32_t &size_processed);
   //--- add unprocessed data into the buffer
   bool              BufferAdd(const char *buffer,const uint32_t buffer_size,uint32_t pos);
  };
//+------------------------------------------------------------------+
