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
//| Tick structure                                                   |
//+------------------------------------------------------------------+
typedef MTTick ExchangeTick;
//+------------------------------------------------------------------+
//| Tick message                                                     |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgTick
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- message data
   ExchangeTick      tick;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgTick &msg);
   static bool       Write(const ExchangeMsgTick &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgTick::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgTick &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get symbol
   READ_MSG_TAG_STR(MSG_TAG_TICK_SYMBOL,msg.tick.symbol)
//--- get price source
   READ_MSG_TAG_STR(MSG_TAG_TICK_BANK,msg.tick.bank)
//--- get bid price
   READ_MSG_TAG_DBL(MSG_TAG_TICK_BID,msg.tick.bid)
//--- get ask price
   READ_MSG_TAG_DBL(MSG_TAG_TICK_ASK,msg.tick.ask)
//--- get last price
   READ_MSG_TAG_DBL(MSG_TAG_TICK_LAST,msg.tick.last)
//--- get volume of last deal
   READ_MSG_TAG_UINT64(MSG_TAG_TICK_VOLUME,msg.tick.volume)
//--- get datetime
   READ_MSG_TAG_INT64(MSG_TAG_TICK_DATETIME,msg.tick.datetime)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgTick::Write(const ExchangeMsgTick &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t  buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write symbol
   WRITE_MSG_TAG_STR(MSG_TAG_TICK_SYMBOL,msg.tick.symbol)
//--- write price source
   WRITE_MSG_TAG_STR(MSG_TAG_TICK_BANK,msg.tick.bank)
//--- write bid price
   WRITE_MSG_TAG_DBL(MSG_TAG_TICK_BID,msg.tick.bid)
//--- write ask price
   WRITE_MSG_TAG_DBL(MSG_TAG_TICK_ASK,msg.tick.ask)
//--- write last price
   WRITE_MSG_TAG_DBL(MSG_TAG_TICK_LAST,msg.tick.last)
//--- write volume of last deal 
   WRITE_MSG_TAG_UINT64(MSG_TAG_TICK_VOLUME,msg.tick.volume)
//--- write datetime
   WRITE_MSG_TAG_INT64(MSG_TAG_TICK_DATETIME,msg.tick.datetime)
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
