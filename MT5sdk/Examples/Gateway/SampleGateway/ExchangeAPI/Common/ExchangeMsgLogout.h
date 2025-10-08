//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "ExchangeMsg.h"
#include "ExchangeContextBase.h"
//+------------------------------------------------------------------+
//| Logout message                                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgLogout
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgLogout &msg);
   static bool       Write(const ExchangeMsgLogout &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to message structure                    |
//+------------------------------------------------------------------+
inline bool ExchangeMsgLogout::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgLogout &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write message structure to buffer                                |
//+------------------------------------------------------------------+
inline bool ExchangeMsgLogout::Write(const ExchangeMsgLogout &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t   buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
