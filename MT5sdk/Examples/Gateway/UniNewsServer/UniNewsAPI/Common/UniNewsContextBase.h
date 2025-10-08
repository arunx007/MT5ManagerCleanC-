//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Interface of connection context                                  |
//+------------------------------------------------------------------+
class CUniNewsContextBase
  {
public:
   //--- constructor/destructor
                     CUniNewsContextBase() {}
   virtual          ~CUniNewsContextBase() {}
   //--- send message
   virtual bool      MessageSend(CUniNewsMsg *msg)=0;
   //--- process login message
   virtual bool      OnMessageLogin(CUniNewsMsgLogin *msg)                    { return(true); }
   //--- process login result message
   virtual bool      OnMessageLoginResult(CUniNewsMsgLoginResult *msg)        { return(true); }
   //--- process logout message
   virtual bool      OnMessageLogout(CUniNewsMsgLogout *msg)                  { return(true); }
   //--- process subscription message
   virtual bool      OnMessageSubscribe(CUniNewsMsgSubscribe *msg)            { return(true); }
   //--- process news message
   virtual bool      OnMessageNews(CUniNewsMsgNews *msg)                      { return(true); }
   //--- process ping message
   virtual bool      OnMessagePing(CUniNewsMsgPing *msg)                      { return(true); }
  };
//+------------------------------------------------------------------+
