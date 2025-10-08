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
//| Book structure                                                   |
//+------------------------------------------------------------------+
typedef MTBook     ExchangeBook;
typedef MTBookItem ExchangeBookItem;
//+------------------------------------------------------------------+
//| Book message                                                     |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgBook
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- message data
   ExchangeBook      book;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgBook &msg);
   static bool       Write(const ExchangeMsgBook &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgBook::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgBook &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get symbol
   READ_MSG_TAG_STR(MSG_TAG_BOOK_SYMBOL,msg.book.symbol)
//--- get flags
   READ_MSG_TAG_UINT64(MSG_TAG_BOOK_FLAGS,msg.book.flags)
//--- get datetime
   READ_MSG_TAG_INT64(MSG_TAG_BOOK_DATETIME,msg.book.datetime)
//--- get items total
   READ_MSG_TAG_UINT(MSG_TAG_BOOK_ITEMS_TOTAL,msg.book.items_total)
//--- temp variable
   char tmp[64]={0};
//--- go through the depth of market elements
   for(uint32_t i=0;i<msg.book.items_total;i++)
     {
      //--- generate a tag name for the item type
      _snprintf_s(tmp,_TRUNCATE,MSG_TAG_BOOK_ITEM_TYPE,i);
      //--- get item type
      READ_MSG_TAG_UINT(tmp,msg.book.items[i].type)
      //--- generate a tag name for the item price
      _snprintf_s(tmp,_TRUNCATE,MSG_TAG_BOOK_ITEM_PRICE,i);
      //--- get item price
      READ_MSG_TAG_DBL(tmp,msg.book.items[i].price)
      //--- generate a tag name for the item volume
      _snprintf_s(tmp,_TRUNCATE,MSG_TAG_BOOK_ITEM_VOLUME,i);
      //--- get item volume
      READ_MSG_TAG_INT64(tmp,msg.book.items[i].volume)
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgBook::Write(const ExchangeMsgBook &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t  buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write symbol
   WRITE_MSG_TAG_STR(MSG_TAG_BOOK_SYMBOL,msg.book.symbol)
//--- write flags
   WRITE_MSG_TAG_UINT64(MSG_TAG_BOOK_FLAGS,msg.book.flags)
//--- write datetime
   WRITE_MSG_TAG_INT64(MSG_TAG_BOOK_DATETIME,msg.book.datetime)
//--- write items total
   WRITE_MSG_TAG_UINT(MSG_TAG_BOOK_ITEMS_TOTAL,msg.book.items_total)
//--- temp variable
   char tmp[64]={0};
//--- go through the depth of market elements
   for(uint32_t i=0;i<msg.book.items_total;i++)
     {
      //--- generate a tag name for the item type
      _snprintf_s(tmp,_TRUNCATE,MSG_TAG_BOOK_ITEM_TYPE,i);
      //--- write item type
      WRITE_MSG_TAG_UINT(tmp,msg.book.items[i].type)
      //--- generate a tag name for the item price
      _snprintf_s(tmp,_TRUNCATE,MSG_TAG_BOOK_ITEM_PRICE,i);
      //--- write item price
      WRITE_MSG_TAG_DBL(tmp,msg.book.items[i].price)
      //--- generate a tag name for the item volume
      _snprintf_s(tmp,_TRUNCATE,MSG_TAG_BOOK_ITEM_VOLUME,i);
      //--- write item volume
      WRITE_MSG_TAG_INT64(tmp,msg.book.items[i].volume)
     }
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
