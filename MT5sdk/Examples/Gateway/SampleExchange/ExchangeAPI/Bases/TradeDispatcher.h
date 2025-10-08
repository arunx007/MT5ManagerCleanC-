//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//--- trade instrument
#include "TradeAccounts.h"
#include "TradeInstrument.h"
//+------------------------------------------------------------------+
//| Class of trade dispatcher                                        |
//+------------------------------------------------------------------+
class CTradeDispatcher
  {

private:
   //--- event configuring constants
   enum EnConstants
     {
      DATA_WAIT_TIME=50     // new data wait time, ms
     };

private:
   //--- type: trade instrument array
   typedef TMTArray<CTradeInstrument*> TradeInstrumentsPtrArray;

private:
   //--- trade instrument array
   TradeInstrumentsPtrArray m_trade_instruments;
   //--- object of synchronization of adding to trade instrument array
   CMTSync           m_trade_instruments_sync;
   //--- trade accounts base
   CTradeAccounts    m_trade_accounts;
   //--- event of receiving data
   HANDLE            m_event;

public:
   //--- constructor/destructor
                     CTradeDispatcher();
                    ~CTradeDispatcher(void);
   //--- initialize/shutdown
   bool              Initialize(void);
   void              Shutdown(void);
   //--- data processing
   void              DataWait(void);
   bool              DataProcess(CExchangeContext &context);
   //--- add order to processing queue
   bool              OrderAdd(const ExchangeOrder &order);
   //--- export descriptions of trade instruments available for trading 
   bool              SymbolsSend(CExchangeContext &context);
   //--- send account data for login
   bool              AccountDataSend(CExchangeContext &context,uint64_t login);

private:
   //--- manage trade instruments
   bool              TradeInstrumentAdd(LPCWSTR symbol_name,
                                        LPCWSTR curr_base,
                                        LPCWSTR curr_profit,
                                        uint32_t exec_mode,
                                        uint32_t digits,
                                        double price_bid_init);
   CTradeInstrument* TradeInstrumentCreate(uint32_t execution_mode);
   void              TradeInstrumentFree(CTradeInstrument *trade_instrument);
   //--- manage the event
   bool              EventCreate(void);
   void              EventClose(void);
   bool              EventSet(void);
   bool              EventWait(const DWORD timeout);
   //--- search/sort trade instruments by name
   static int32_t    SortTradeInstrumentsBySymbol(const void *left,const void *right);
   static int32_t    SearchTradeInstrumentsBySymbol(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
