//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Preliminary declarations                                         |
//+------------------------------------------------------------------+
struct ExchangeMsgLogin;
struct ExchangeMsgLogout;
struct ExchangeMsgSymbol;
struct ExchangeMsgTick;
struct ExchangeMsgOrder;
struct ExchangeMsgDeal;
struct ExchangeMsgAccountDataRequest;
struct ExchangeMsgAccountData;
struct ExchangeMsgBook;
struct ExchangeMsgHeartBeat;
//+------------------------------------------------------------------+
//| Interface of connection context                                  |
//+------------------------------------------------------------------+
class CExchangeContextBase
  {
public:
   //--- constructor/destructor
                     CExchangeContextBase() {}
   virtual          ~CExchangeContextBase() {}
   //--- send data
   virtual bool      Send(const char *buffer,const uint32_t buffer_size)    =0;
   //--- process login message
   virtual bool      OnMessageLogin(const ExchangeMsgLogin &msg)        =0;
   //--- process logout message
   virtual bool      OnMessageLogout(const ExchangeMsgLogout &msg)      =0;
   //--- process message with symbol settings
   virtual bool      OnMessageSymbol(const ExchangeMsgSymbol &msg)      =0;
   //--- process message with tick
   virtual bool      OnMessageTick(const ExchangeMsgTick &msg)          =0;
   //--- process message with book
   virtual bool      OnMessageBook(const ExchangeMsgBook &msg)          =0;
   //--- process message with order
   virtual bool      OnMessageOrder(const ExchangeMsgOrder &msg)        =0;
   //--- process message with deal
   virtual bool      OnMessageDeal(const ExchangeMsgDeal &msg)          =0;
   //--- process message with orders request
   virtual bool      OnMessageAccountDataRequest(const ExchangeMsgAccountDataRequest &msg)=0;
   //--- process message with orders list
   virtual bool      OnMessageAccountData(const ExchangeMsgAccountData &msg)=0;
   //--- process HeartBeat message
   virtual bool      OnMessageHeartBeat(const ExchangeMsgHeartBeat &msg)=0;
  };
//+------------------------------------------------------------------+
