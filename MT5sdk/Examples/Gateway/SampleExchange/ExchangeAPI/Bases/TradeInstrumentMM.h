//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of a trade instrument for Market Maker market              |
//+------------------------------------------------------------------+
class CTradeInstrumentMM : public CTradeInstrument
  {

private:
   //--- tick generation constant parameters
   enum EnConstants
     {
      TICK_VOLUME                   =0,      // tick volume
      TICK_PRICE_LAST               =0,      // price of last deal
      TICK_PERIOD_MIN               =200,    // minimum tick period (in milliseconds)
      TICK_PERIOD_MAX               =1000,   // maximum tick period (in milliseconds)
      TREND_TICKS_COUNT_MIN         =5,      // minimum number of ticks with constant rise/drop in prices
      TREND_TICKS_COUNT_MAX         =15      // maximum number of ticks with constant rise/drop in prices
     };

private:
   //--- pending orders
   ExchangePendingOrdersArray m_pending_orders;
   //--- tick data
   double            m_tick_bid;             // last bid price
   double            m_tick_price_delta_min; // lower range limit for generating random price increment
   double            m_tick_price_delta_max; // upper range limit for generating random price increment
   uint32_t          m_tick_trend_total;     // number of trend ticks
   uint32_t          m_tick_trend_max;       // maximum number of trend ticks
   int64_t           m_tick_period_min;      // minimum tick period (in ticks)
   int64_t           m_tick_period_max;      // maximum tick period (in ticks)
   int64_t           m_tick_time_next;       // next tick time

public:
   //--- constructor/destructor
                     CTradeInstrumentMM();
   virtual          ~CTradeInstrumentMM();
   //--- initialize trade instrument
   virtual void      Initialize(LPCWSTR symbol_name,
                                LPCWSTR curr_base,
                                LPCWSTR curr_profit,
                                uint32_t exec_mode,
                                uint32_t digits,
                                double price_bid_init);
   //--- get all orders for login
   virtual bool      OrdersGet(uint64_t login,ExchangeOrdersArray &orders);

private:
   //--- process an order from the queue of orders for processing
   virtual bool      ProcessOrder(ExchangePendingOrder &pending_order,CTradeAccounts &accounts,CExchangeContext &context);
   //--- process quotes
   virtual bool      ProcessQuotes(CExchangeContext &context);
   //--- add volume to position
   virtual bool      PositionAddVolume(uint64_t login,int64_t volume,double price,double &profit);
   //--- process pending order
   bool              ProcessPendingOrder(const ExchangePendingOrder &pending_order);
   //--- fill tick data by symbol
   void              FillTick(ExchangeTick &tick);
   //--- generate random value of the double type in specified range
   static double     GetRandDbl(const double min,const double max);
   //--- Generate random value of the uint32_t type in specified range
   static uint64_t   GetRandUINT(const uint64_t min,const uint64_t max);
  };
//+------------------------------------------------------------------+
