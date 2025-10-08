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
struct ExchangeOrder
  {
   //--- enumerate operations on the order
   enum EnOrderActions
     {
      ORDER_ACTION_ADD   =1,                    // add a new order
      ORDER_ACTION_MODIFY=2,                    // modify an order
      ORDER_ACTION_CANCEL=3                     // cancel an order
     };
   //--- enumerate order states
   enum EnOrderStates
     {
      ORDER_STATE_UNKNOWN          =0,          // undefined state
      ORDER_STATE_CONFIRMED        =1,          // order confirmed
      ORDER_STATE_REQUEST_PLACED   =2,          // order generation request placed
      ORDER_STATE_NEW              =3,          // create a new order
      ORDER_STATE_REJECT_NEW       =4,          // order rejected
      ORDER_STATE_DEAL             =5,          // a deal by order sent
      ORDER_STATE_REQUEST_MODIFY   =6,          // order modification request received
      ORDER_STATE_MODIFY           =7,          // order modified
      ORDER_STATE_REJECT_MODIFY    =8,          // order modification rejected
      ORDER_STATE_REQUEST_CANCEL   =9,          // order cancelation request received
      ORDER_STATE_CANCEL           =10,         // order canceled
      ORDER_STATE_REJECT_CANCEL    =11,         // order cancelation rejected
      ORDER_STATE_COMPLETED        =20          // operation on the order complete, can be removed from the queue
     };
   uint32_t          order_action;              // order action
   uint32_t          order_state;               // order state
   uint64_t          order_mt_id;               // order ticket
   uint64_t          order_exchange_id;         // order id in external system
   uint64_t          order_custom_data;         // custom data
   uint64_t          request_mt_id;             // request_id
   wchar_t           symbol[32];                // symbol
   uint64_t          login;                     // client's login
   uint32_t          type_order;                // order type
   uint32_t          type_time;                 // expiration type
   uint32_t          action;                    // action type
   double            price_order;               // order price
   double            price_SL;                  // Stop Loss level
   double            price_TP;                  // Take Profit level
   double            price_tick_bid;            // symbol bid price in external trading system
   double            price_tick_ask;            // symbol ask price in external trading system
   uint64_t          volume;                    // volume
   int64_t           expiration_time;           // expiration time
   MTAPIRES          result;                    // result of order processing
   //--- generate order ID in an external trading system
   static uint64_t   OrderExchangeIDNext(void);
   //--- print order
   static LPCWSTR    Print(const ExchangeOrder &order,CMTStr &res);
   //--- print order type
   static LPCWSTR    PrintOrderType(const ExchangeOrder &order,CMTStr &res);
   //--- print order action type
   static LPCWSTR    PrintOrderAction(const ExchangeOrder &order,CMTStr &res);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Generate order ID in an external trading system                  |
//+------------------------------------------------------------------+
inline uint64_t ExchangeOrder::OrderExchangeIDNext(void)
  {
//--- order counter
   static uint64_t s_order_exchange_count(0);
//--- order counter change synchronization object
   static CMTSync s_sync;
//--- lock
   s_sync.Lock();
//--- increment deal counter value
   s_order_exchange_count++;
//--- unlock
   s_sync.Unlock();
//--- return order counter value
   return(s_order_exchange_count);
  }
//+------------------------------------------------------------------+
//| Print order                                                      |
//+------------------------------------------------------------------+
inline LPCWSTR ExchangeOrder::Print(const ExchangeOrder &order,CMTStr &res)
  {
   CMTStr32   order_type;
   CMTStr32   order_action;
   CMTStrPath tmp;
//--- clear resulting string
   res.Clear();
//--- print order type
   PrintOrderType(order,order_type);
//--- print order action type
   PrintOrderAction(order,order_action);
//--- print depending on order action type
   if(order.action==IMTRequest::TA_PRICE)
     {
      //--- print price requesting order
      res.Format(L"prices for %s %.2lf",order.symbol,double(order.volume));
     }
   else
     {
      //--- print orders of other action types
      res.Format(L"#%I64u %s %s %.2lf %s at ",
            order.order_mt_id,
            order_action.Str(),
            order_type.Str(),
            double(order.volume),
            order.symbol);
      //--- print price
      if(order.price_order!=0)
         tmp.Format(L"%.5lf",order.price_order);
      else
         tmp.Assign(L"market");
      //---
      res.Append(tmp);
      //--- print Stop Loss if it is set
      if(order.price_SL!=0)
        {
         tmp.Format(L" sl: %.5lf",order.price_SL);
         res.Append(tmp);
        }
      //--- print Take Profit if it is set
      if(order.price_TP!=0)
        {
         tmp.Format(L" tp: %.5lf",order.price_TP);
         res.Append(tmp);
        }
     }
//--- return result
   return(res.Str());
  }
//+------------------------------------------------------------------+
//| Print order type                                                 |
//+------------------------------------------------------------------+
inline LPCWSTR ExchangeOrder::PrintOrderType(const ExchangeOrder &order,CMTStr &res)
  {
//--- form the string value of order type
   switch(order.type_order)
     {
      case IMTOrder::OP_BUY            : res.Assign(L"buy");               break;
      case IMTOrder::OP_SELL           : res.Assign(L"sell");              break;
      case IMTOrder::OP_BUY_LIMIT      : res.Assign(L"buy limit");         break;
      case IMTOrder::OP_SELL_LIMIT     : res.Assign(L"sell limit");        break;
      case IMTOrder::OP_BUY_STOP       : res.Assign(L"buy stop");          break;
      case IMTOrder::OP_SELL_STOP      : res.Assign(L"sell stop");         break;
      case IMTOrder::OP_BUY_STOP_LIMIT : res.Assign(L"buy stop limit");    break;
      case IMTOrder::OP_SELL_STOP_LIMIT: res.Assign(L"sell stop limit");   break;
      default                          : res.Assign(L"");                  break;
     }
//--- return result
   return(res.Str());
  }
//+------------------------------------------------------------------+
//| Print order type                                                 |
//+------------------------------------------------------------------+
inline LPCWSTR ExchangeOrder::PrintOrderAction(const ExchangeOrder &order,CMTStr &res)
  {
//--- form the string value of order action type
   switch(order.action)
     {
      case IMTRequest::TA_PRICE              : res.Assign(L"prices for");                 break;
      case IMTRequest::TA_REQUEST            : res.Assign(L"request");                    break;
      case IMTRequest::TA_INSTANT            : res.Assign(L"instant");                    break;
      case IMTRequest::TA_MARKET             : res.Assign(L"market");                     break;
      case IMTRequest::TA_EXCHANGE           : res.Assign(L"exchange");                   break;
      case IMTRequest::TA_PENDING            : res.Assign(L"pending");                    break;
      case IMTRequest::TA_SLTP               : res.Assign(L"modify");                     break;
      case IMTRequest::TA_MODIFY             : res.Assign(L"modify");                     break;
      case IMTRequest::TA_REMOVE             : res.Assign(L"cancel");                     break;
      case IMTRequest::TA_ACTIVATE           : res.Assign(L"activate");                   break;
      case IMTRequest::TA_ACTIVATE_SL        : res.Assign(L"activate stop loss");         break;
      case IMTRequest::TA_ACTIVATE_TP        : res.Assign(L"activate take profit");       break;
      case IMTRequest::TA_ACTIVATE_STOPLIMIT : res.Assign(L"activate stop-limit order");  break;
      case IMTRequest::TA_STOPOUT_ORDER      : res.Assign(L"delete stop-out order");      break;
      case IMTRequest::TA_STOPOUT_POSITION   : res.Assign(L"close stop-out position");    break;
      case IMTRequest::TA_EXPIRATION         : res.Assign(L"expire");                     break;
      default                                : res.Assign(L"order");                      break;
     }
//--- return result
   return(res.Str());
  }
//+------------------------------------------------------------------+
//| Array of orders                                                  |
//+------------------------------------------------------------------+
typedef TMTArray<ExchangeOrder> ExchangeOrdersArray;
//+------------------------------------------------------------------+
//| Order message                                                    |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgOrder
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- message data
   ExchangeOrder     order;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgOrder &msg);
   static bool       Write(const ExchangeMsgOrder &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgOrder::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgOrder &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get order action type
   READ_MSG_TAG_UINT(MSG_TAG_ORDER_ACTION_TYPE,msg.order.order_action)
//--- get order state
   READ_MSG_TAG_UINT(MSG_TAG_ORDER_STATE,msg.order.order_state)
//--- get order ticket
   READ_MSG_TAG_UINT64(MSG_TAG_ORDER_MT_ID,msg.order.order_mt_id)
//--- get order id in external system
   READ_MSG_TAG_UINT64(MSG_TAG_ORDER_EXCHANGE_ID,msg.order.order_exchange_id)
//--- get custom data
   READ_MSG_TAG_UINT64(MSG_TAG_ORDER_CUSTOM_DATA,msg.order.order_custom_data)
//--- get request id
   READ_MSG_TAG_UINT64(MSG_TAG_ORDER_REQUEST_ID,msg.order.request_mt_id)
//--- get symbol
   READ_MSG_TAG_STR(MSG_TAG_ORDER_SYMBOL,msg.order.symbol)
//--- get client's login
   READ_MSG_TAG_UINT64(MSG_TAG_ORDER_LOGIN,msg.order.login)
//--- get order type
   READ_MSG_TAG_UINT(MSG_TAG_ORDER_TYPE_ORDER,msg.order.type_order)
//--- get order expiration type
   READ_MSG_TAG_UINT(MSG_TAG_ORDER_TYPE_TIME,msg.order.type_time)
//--- get action
   READ_MSG_TAG_UINT(MSG_TAG_ORDER_ACTION,msg.order.action)
//--- get order price
   READ_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_ORDER,msg.order.price_order)
//--- get Stop Loss level
   READ_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_SL,msg.order.price_SL)
//--- get Take Profit level
   READ_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TP,msg.order.price_TP)
//--- get symbol bid price in external trading system
   READ_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TICK_BID,msg.order.price_tick_bid)
//--- get symbol ask price in external trading system
   READ_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TICK_ASK,msg.order.price_tick_ask)
//--- get order volume
   READ_MSG_TAG_UINT64(MSG_TAG_ORDER_VOLUME,msg.order.volume)
//--- get expiration time
   READ_MSG_TAG_INT64(MSG_TAG_ORDER_EXPIRATION_TIME,msg.order.expiration_time)
//--- get message processing result
   READ_MSG_TAG_UINT(MSG_TAG_ORDER_RESULT,msg.order.result)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgOrder::Write(const ExchangeMsgOrder &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t   buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write order state
   WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_ACTION_TYPE,msg.order.order_action)
//--- write order state
   WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_STATE,msg.order.order_state)
//--- write order ticket
   WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_MT_ID,msg.order.order_mt_id)
//--- write order id in external system
   WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_EXCHANGE_ID,msg.order.order_exchange_id)
//--- write custom data
   WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_CUSTOM_DATA,msg.order.order_custom_data)
//--- write order id in external system
   WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_REQUEST_ID,msg.order.request_mt_id)
//--- write symbol
   WRITE_MSG_TAG_STR(MSG_TAG_ORDER_SYMBOL,msg.order.symbol)
//--- write client's login
   WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_LOGIN,msg.order.login)
//--- write order type
   WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_TYPE_ORDER,msg.order.type_order)
//--- write order expiration type
   WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_TYPE_TIME,msg.order.type_time)
//--- get action
   WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_ACTION,msg.order.action)
//--- write order price
   WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_ORDER,msg.order.price_order)
//--- write Stop Loss level
   WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_SL,msg.order.price_SL)
//--- write Take Profit level
   WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TP,msg.order.price_TP)
//--- write symbol bid price in external trading system
   WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TICK_BID,msg.order.price_tick_bid)
//--- write symbol ask price in external trading system
   WRITE_MSG_TAG_DBL(MSG_TAG_ORDER_PRICE_TICK_ASK,msg.order.price_tick_ask)
//--- write order volume
   WRITE_MSG_TAG_UINT64(MSG_TAG_ORDER_VOLUME,msg.order.volume)
//--- write expiration time
   WRITE_MSG_TAG_INT64(MSG_TAG_ORDER_EXPIRATION_TIME,msg.order.expiration_time)
//--- write message processing result
   WRITE_MSG_TAG_UINT(MSG_TAG_ORDER_RESULT,msg.order.result)
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
