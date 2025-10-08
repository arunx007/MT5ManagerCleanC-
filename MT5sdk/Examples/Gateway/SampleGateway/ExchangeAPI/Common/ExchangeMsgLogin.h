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
//| Login message                                                    |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgLogin
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- message data
   wchar_t           login[64];
   wchar_t           password[64];
   //--- result
   MTAPIRES          result;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgLogin &msg);
   static bool       Write(const ExchangeMsgLogin &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to message structure                    |
//+------------------------------------------------------------------+
inline bool ExchangeMsgLogin::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgLogin &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get login
   READ_MSG_TAG_STR(MSG_TAG_LOGIN_LOGIN,msg.login)
//--- get password
   READ_MSG_TAG_STR(MSG_TAG_LOGIN_PASSWORD,msg.password)
//--- get message result
   READ_MSG_TAG_UINT(MSG_TAG_LOGIN_RESULT,msg.result)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write message structure to buffer                                |
//+------------------------------------------------------------------+
inline bool ExchangeMsgLogin::Write(const ExchangeMsgLogin &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t   buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write login
   WRITE_MSG_TAG_STR(MSG_TAG_LOGIN_LOGIN,msg.login)
//--- write password
   WRITE_MSG_TAG_STR(MSG_TAG_LOGIN_PASSWORD,msg.password)
//--- write result
   WRITE_MSG_TAG_UINT(MSG_TAG_LOGIN_RESULT,msg.result)
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-(uint32_t)buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
