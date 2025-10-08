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
//| Deal structure                                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeDeal
  {
   uint64_t          deal_exchange_id; // exchange id
   uint64_t          order;            // order exchange ticket
   wchar_t           symbol[32];       // symbol
   uint64_t          login;            // client's login
   uint32_t          type_deal;        // operation type
   uint64_t          volume;           // deal volume
   uint64_t          volume_remaind;   // non-filled volume
   double            price;            // lot price
   //--- generate deal ID in an external trading system
   static uint64_t   DealExchangeIDNext(void);
   //--- print deal
   static LPCWSTR    Print(const ExchangeDeal &deal,CMTStr &res);
   //--- print deal type
   static LPCWSTR    PrintDealType(const ExchangeDeal &deal,CMTStr &res);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Generate deal ID in an external trading system                   |
//+------------------------------------------------------------------+
inline uint64_t ExchangeDeal::DealExchangeIDNext(void)
  {
//--- deal counter
   static uint64_t s_deal_exchange_count(0);
//--- deal counter change synchronization object
   static CMTSync s_sync;
//--- lock
   s_sync.Lock();
//--- increment deal counter value
   s_deal_exchange_count++;
//--- unlock
   s_sync.Unlock();
//--- return deal counter value
   return(s_deal_exchange_count);
  }
//+------------------------------------------------------------------+
//| Print deal                                                       |
//+------------------------------------------------------------------+
inline LPCWSTR ExchangeDeal::Print(const ExchangeDeal &deal,CMTStr &res)
  {
   CMTStr32   deal_type;
   CMTStrPath tmp;
//--- clear resulting string
   res.Clear();
//--- print deal type
   PrintDealType(deal,deal_type);
//--- generate a resulting string
   res.Format(L"#%I64u %s %.2lf",
              deal.order,
              deal_type.Str(),
              double(deal.volume));
   if(deal.volume_remaind!=0)
     {
      tmp.Format(L"/%.2lf",double(deal.volume_remaind));
      res.Append(tmp);
     }
   tmp.Format(L" %s at market",deal.symbol);
   res.Append(tmp);
//--- return result
   return(res.Str());
  }
//+------------------------------------------------------------------+
//| Print deal type                                                  |
//+------------------------------------------------------------------+
inline LPCWSTR ExchangeDeal::PrintDealType(const ExchangeDeal &deal,CMTStr &res)
  {
//--- form the string value of deal type
   switch(deal.type_deal)
     {
      case IMTDeal::DEAL_BUY           : res.Assign(L"buy");               break;
      case IMTDeal::DEAL_SELL          : res.Assign(L"sell");              break;
      default                          : res.Assign(L"");                  break;
     }
//--- return result
   return(res.Str());
  }
//+------------------------------------------------------------------+
//| Array of deals                                                   |
//+------------------------------------------------------------------+
typedef TMTArray<ExchangeDeal> ExchangeDealsArray;
//+------------------------------------------------------------------+
//| Order message                                                    |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgDeal
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- message data
   ExchangeDeal      deal;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgDeal &msg);
   static bool       Write(const ExchangeMsgDeal &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgDeal::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgDeal &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get deal external id
   READ_MSG_TAG_UINT64(MSG_TAG_DEAL_EXCHANGE_ID,msg.deal.deal_exchange_id)
//--- get deal order
   READ_MSG_TAG_UINT64(MSG_TAG_DEAL_ORDER,msg.deal.order)
//--- get symbol
   READ_MSG_TAG_STR(MSG_TAG_DEAL_SYMBOL,msg.deal.symbol)
//--- get login
   READ_MSG_TAG_UINT64(MSG_TAG_DEAL_LOGIN,msg.deal.login)
//--- get deal action
   READ_MSG_TAG_UINT(MSG_TAG_DEAL_TYPE,msg.deal.type_deal)
//--- get deal volume
   READ_MSG_TAG_UINT64(MSG_TAG_DEAL_VOLUME,msg.deal.volume)
//--- get deal volume remaind
   READ_MSG_TAG_UINT64(MSG_TAG_DEAL_VOLUME_REM,msg.deal.volume_remaind)
//--- get deal price
   READ_MSG_TAG_DBL(MSG_TAG_DEAL_PRICE,msg.deal.price)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgDeal::Write(const ExchangeMsgDeal &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char    buffer[MSG_MAX_SIZE]={0};
   char   *buffer_ptr          =buffer;
   int32_t buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write deal external id
   WRITE_MSG_TAG_UINT64(MSG_TAG_DEAL_EXCHANGE_ID,msg.deal.deal_exchange_id)
//--- write deal order
   WRITE_MSG_TAG_UINT64(MSG_TAG_DEAL_ORDER,msg.deal.order)
//--- write symbol
   WRITE_MSG_TAG_STR(MSG_TAG_DEAL_SYMBOL,msg.deal.symbol)
//--- write login
   WRITE_MSG_TAG_UINT64(MSG_TAG_DEAL_LOGIN,msg.deal.login)
//--- write deal action
   WRITE_MSG_TAG_UINT(MSG_TAG_DEAL_TYPE,msg.deal.type_deal)
//--- write deal volume
   WRITE_MSG_TAG_UINT64(MSG_TAG_DEAL_VOLUME,msg.deal.volume)
//--- write deal volume remaind
   WRITE_MSG_TAG_UINT64(MSG_TAG_DEAL_VOLUME_REM,msg.deal.volume_remaind)
//--- write deal price
   WRITE_MSG_TAG_DBL(MSG_TAG_DEAL_PRICE,msg.deal.price)
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
