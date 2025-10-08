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
//| Order structure                                                  |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangePosition
  {
   wchar_t           symbol[32];                // symbol
   uint64_t          login;                     // client's login
   double            price;                     // weighted average opening position price
   int64_t           volume;                    // volume
   uint32_t          digits;                    // decimal places in pricen price
   //--- add volume to position with average price recalculation
   static void       PositionAddVolume(ExchangePosition &position,int64_t volume,double price);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Add volume to position with price recalculation                  |
//+------------------------------------------------------------------+
inline void ExchangePosition::PositionAddVolume(ExchangePosition &position,int64_t volume,double price)
  {
//--- remember old volume
   int64_t old_volume=position.volume;
//--- calculate total cost of position
   double position_cost=position.volume*position.price+volume*price;
//--- add volume
   position.volume+=volume;
//--- if position was grown, recalc it's price
   if(old_volume>0 && volume>0 || old_volume<0 && volume<0)
      position.price=position_cost/position.volume;
//--- if position was flipped, set new price
   if(old_volume>=0 && position.volume<0 || old_volume<=0 && position.volume>0)
      position.price=price;
  }
//+------------------------------------------------------------------+
//| Array of positions                                               |
//+------------------------------------------------------------------+
typedef TMTArray<ExchangePosition> ExchangePositionsArray;
//+------------------------------------------------------------------+
//| Account data structure                                           |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeAccountData
  {
   //--- login
   uint64_t          login;
   //--- balance
   double            balance;
   //--- array of orders
   ExchangeOrdersArray orders;
   //--- array of positions
   ExchangePositionsArray positions;
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Array of accounts data                                           |
//+------------------------------------------------------------------+
typedef TMTArray<ExchangeAccountData> ExchangeAccountsDataArray;
//+------------------------------------------------------------------+
//| Account data request message                                     |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgAccountDataRequest
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- login
   uint64_t          login;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgAccountDataRequest &msg);
   static bool       Write(const ExchangeMsgAccountDataRequest &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgAccountDataRequest::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgAccountDataRequest &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get login
   READ_MSG_TAG_UINT64(MSG_TAG_ACCOUNT_DATA_LOGIN,msg.login)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgAccountDataRequest::Write(const ExchangeMsgAccountDataRequest &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t   buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write login
   WRITE_MSG_TAG_UINT64(MSG_TAG_ACCOUNT_DATA_LOGIN,msg.login)
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
//| Account data message                                             |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgAccountData
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- account data
   ExchangeAccountData account_data;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgAccountData &msg);
   static bool       Write(const ExchangeMsgAccountData &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgAccountData::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgAccountData &msg)
  {
   bool res=true;
   uint32_t orders_amount=0;
   uint32_t positions_amount=0;
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get login
   READ_MSG_TAG_UINT64(MSG_TAG_ACCOUNT_DATA_LOGIN,msg.account_data.login)
//--- get balance
   READ_MSG_TAG_DBL(MSG_TAG_ACCOUNT_DATA_BALANCE,msg.account_data.balance)
//--- get orders amount
   READ_MSG_TAG_UINT(MSG_TAG_ACCOUNT_DATA_ORDERS_AMOUNT,orders_amount)
//--- get positions amount
   READ_MSG_TAG_UINT(MSG_TAG_ACCOUNT_DATA_POSITIONS_AMOUNT,positions_amount)
//--- allocate orders and positions
   if(!msg.account_data.orders.AddEmpty(orders_amount) || !msg.account_data.positions.AddEmpty(positions_amount))
      return(false);
//--- read all orders
   for(uint32_t i=0;res && i<orders_amount;i++)
     {
      //--- get order action type
      READ_MSG_TAG_NUM_UINT(MSG_TAG_ORDER_ACTION_TYPE,msg.account_data.orders[i].order_action,i)
      //--- get order state
      READ_MSG_TAG_NUM_UINT(MSG_TAG_ORDER_STATE,msg.account_data.orders[i].order_state,i)
      //--- get order ticket
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_ORDER_MT_ID,msg.account_data.orders[i].order_mt_id,i)
      //--- get order id in external system
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_ORDER_EXCHANGE_ID,msg.account_data.orders[i].order_exchange_id,i)
      //--- get custom data
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_ORDER_CUSTOM_DATA,msg.account_data.orders[i].order_custom_data,i)
      //--- get request id
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_ORDER_REQUEST_ID,msg.account_data.orders[i].request_mt_id,i)
      //--- get symbol
      READ_MSG_TAG_NUM_STR(MSG_TAG_ORDER_SYMBOL,msg.account_data.orders[i].symbol,i)
      //--- get client's login
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_ORDER_LOGIN,msg.account_data.orders[i].login,i)
      //--- get order type
      READ_MSG_TAG_NUM_UINT(MSG_TAG_ORDER_TYPE_ORDER,msg.account_data.orders[i].type_order,i)
      //--- get order expiration type
      READ_MSG_TAG_NUM_UINT(MSG_TAG_ORDER_TYPE_TIME,msg.account_data.orders[i].type_time,i)
      //--- get action
      READ_MSG_TAG_NUM_UINT(MSG_TAG_ORDER_ACTION,msg.account_data.orders[i].action,i)
      //--- get order price
      READ_MSG_TAG_NUM_DBL(MSG_TAG_ORDER_PRICE_ORDER,msg.account_data.orders[i].price_order,i)
      //--- get Stop Loss level
      READ_MSG_TAG_NUM_DBL(MSG_TAG_ORDER_PRICE_SL,msg.account_data.orders[i].price_SL,i)
      //--- get Take Profit level
      READ_MSG_TAG_NUM_DBL(MSG_TAG_ORDER_PRICE_TP,msg.account_data.orders[i].price_TP,i)
      //--- get symbol bid price in external trading system
      READ_MSG_TAG_NUM_DBL(MSG_TAG_ORDER_PRICE_TICK_BID,msg.account_data.orders[i].price_tick_bid,i)
      //--- get symbol ask price in external trading system
      READ_MSG_TAG_NUM_DBL(MSG_TAG_ORDER_PRICE_TICK_ASK,msg.account_data.orders[i].price_tick_ask,i)
      //--- get order volume
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_ORDER_VOLUME,msg.account_data.orders[i].volume,i)
      //--- get expiration time
      READ_MSG_TAG_NUM_INT64(MSG_TAG_ORDER_EXPIRATION_TIME,msg.account_data.orders[i].expiration_time,i)
      //--- get message processing result
      READ_MSG_TAG_NUM_UINT(MSG_TAG_ORDER_RESULT,msg.account_data.orders[i].result,i)
     }
//--- read all positions
   for(uint32_t i=0;i<positions_amount;i++)
     {
      //--- get symbol
      READ_MSG_TAG_NUM_STR(MSG_TAG_POSITION_SYMBOL,msg.account_data.positions[i].symbol,i)
      //--- get client's login
      READ_MSG_TAG_NUM_UINT64(MSG_TAG_POSITION_LOGIN,msg.account_data.positions[i].login,i)
      //--- get position price
      READ_MSG_TAG_NUM_DBL(MSG_TAG_POSITION_PRICE,msg.account_data.positions[i].price,i)
      //--- get positions volume
      READ_MSG_TAG_NUM_INT64(MSG_TAG_POSITION_VOLUME,msg.account_data.positions[i].volume,i)
      //--- get digits
      READ_MSG_TAG_NUM_UINT(MSG_TAG_POSITION_DIGITS,msg.account_data.positions[i].digits,i)
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgAccountData::Write(const ExchangeMsgAccountData &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t   buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write login
   WRITE_MSG_TAG_UINT64(MSG_TAG_ACCOUNT_DATA_LOGIN,msg.account_data.login)
//--- write balance
   WRITE_MSG_TAG_DBL(MSG_TAG_ACCOUNT_DATA_BALANCE,msg.account_data.balance)
//--- write orders amount
   WRITE_MSG_TAG_UINT(MSG_TAG_ACCOUNT_DATA_ORDERS_AMOUNT,msg.account_data.orders.Total())
//--- write positions amount
   WRITE_MSG_TAG_UINT(MSG_TAG_ACCOUNT_DATA_POSITIONS_AMOUNT,msg.account_data.positions.Total())
//--- write all orders
   for(uint32_t i=0;i<msg.account_data.orders.Total();i++)
     {
      //--- write order state
      WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_ACTION_TYPE,msg.account_data.orders[i].order_action)
      //--- write order state
      WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_STATE,msg.account_data.orders[i].order_state)
      //--- write order ticket
      WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_MT_ID,msg.account_data.orders[i].order_mt_id)
      //--- write order id in external system
      WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_EXCHANGE_ID,msg.account_data.orders[i].order_exchange_id)
      //--- write custom data
      WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_CUSTOM_DATA,msg.account_data.orders[i].order_custom_data)
      //--- write order id in external system
      WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_REQUEST_ID,msg.account_data.orders[i].request_mt_id)
      //--- write symbol
      WRITE_MSG_TAG_STR(MSG_TAG_ORDER_SYMBOL,msg.account_data.orders[i].symbol)
      //--- write client's login
      WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_LOGIN,msg.account_data.orders[i].login)
      //--- write order type
      WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_TYPE_ORDER,msg.account_data.orders[i].type_order)
      //--- write order expiration type
      WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_TYPE_TIME,msg.account_data.orders[i].type_time)
      //--- get action
      WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_ACTION,msg.account_data.orders[i].action)
      //--- write order price
      WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_ORDER,msg.account_data.orders[i].price_order)
      //--- write Stop Loss level
      WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_SL,msg.account_data.orders[i].price_SL)
      //--- write Take Profit level
      WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TP,msg.account_data.orders[i].price_TP)
      //--- write symbol bid price in external trading system
      WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TICK_BID,msg.account_data.orders[i].price_tick_bid)
      //--- write symbol ask price in external trading system
      WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TICK_ASK,msg.account_data.orders[i].price_tick_ask)
      //--- write order volume
      WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_VOLUME,msg.account_data.orders[i].volume)
      //--- write expiration time
      WRITE_MSG_TAG_INT64(MSG_TAG_ORDER_EXPIRATION_TIME,msg.account_data.orders[i].expiration_time)
      //--- write message processing result
      WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_RESULT,msg.account_data.orders[i].result)
     }
//--- write all orders
   for(uint32_t i=0;i<msg.account_data.positions.Total();i++)
     {
      //--- write symbol
      WRITE_MSG_TAG_STR(MSG_TAG_POSITION_SYMBOL,msg.account_data.positions[i].symbol)
      //--- write client's login
      WRITE_MSG_TAG_UINT64(MSG_TAG_POSITION_LOGIN,msg.account_data.positions[i].login)
      //--- write position price
      WRITE_MSG_TAG_DBL(MSG_TAG_POSITION_PRICE,msg.account_data.positions[i].price)
      //--- write position volume
      WRITE_MSG_TAG_INT64(MSG_TAG_POSITION_VOLUME,msg.account_data.positions[i].volume)
      //--- write expiration time
      WRITE_MSG_TAG_UINT(MSG_TAG_POSITION_DIGITS,msg.account_data.positions[i].digits)
     }
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
