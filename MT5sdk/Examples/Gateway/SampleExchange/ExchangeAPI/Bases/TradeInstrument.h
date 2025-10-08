//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of trade instrument                                        |
//+------------------------------------------------------------------+
class CTradeInstrument
  {

protected:
   //--- structure for storing an order waiting for execution
   struct ExchangePendingOrder
     {
      //--- order
      ExchangeOrder     order;
      //--- order add time
      int64_t           order_add_time;
     };
   //--- type: array of orders waiting for execution
   typedef TMTArray<ExchangePendingOrder> ExchangePendingOrdersArray;

protected:
   //--- trade instrument description
   ExchangeSymbol    m_symbol;
   //--- positions
   ExchangePositionsArray m_positions;
   //--- positions
   CMTSync           m_positions_sync;

private:
   //--- preliminary queue of orders for processing
   ExchangePendingOrdersArray m_orders_income;
   //--- queue of orders for processing
   ExchangePendingOrdersArray m_orders;
   //--- object for synchronizing an entry to the orders array
   CMTSync           m_orders_sync;
   //--- performance frequency
   static int64_t    s_performance_frequency;

public:
                     CTradeInstrument();
   virtual          ~CTradeInstrument();
   //--- initialize trade instrument
   virtual void      Initialize(LPCWSTR symbol_name,
                                LPCWSTR curr_base,
                                LPCWSTR curr_profit,
                                uint32_t exec_mode,
                                uint32_t digits,
                                double settlement_price);
   //--- data processing
   bool              ProcessData(CTradeAccounts &accounts,CExchangeContext &context);
   //--- add a new order to a queue for processing
   bool              OrderAdd(const ExchangeOrder &order);
   //--- manage trade instrument description
   virtual bool      SymbolSend(CExchangeContext &context,uint32_t index);
   LPCWSTR           SymbolGet(void) const;
   //--- get all orders for login
   virtual bool      OrdersGet(uint64_t login,ExchangeOrdersArray &orders);
   //--- get position for login
   virtual bool      PositionGet(uint64_t login,ExchangePosition &position);
   virtual ExchangePosition* PositionGetPtr(uint64_t login);
   //--- get all positions
   virtual bool      PositionsGetAll(ExchangePositionsArray &positions);

protected:
   //--- process the preliminary queue
   bool              ProcessOrdersIncome(CExchangeContext &context);
   //--- process the queue of orders
   virtual bool      ProcessOrders(ExchangePendingOrdersArray &pending_orders,CTradeAccounts &accounts,CExchangeContext &context);
   //--- process an order from the queue of orders for processing
   virtual bool      ProcessOrder(ExchangePendingOrder &pending_order,CTradeAccounts &accounts,CExchangeContext &context)=0;
   //--- process quotes
   virtual bool      ProcessQuotes(CExchangeContext &context) { return(true); };
   //--- add volume to position
   virtual bool      PositionAddVolume(uint64_t login,int64_t volume,double price,double &profit);
   //--- get order direction
   bool              OrderIsBuy(uint32_t type_order);
   //--- get current time
   bool              TimeGet(int64_t &time_pf);
   //--- convert time from milliseconds into ticks
   bool              TimeConvert(const int64_t &time_msc,int64_t &time_pf);
  };
//+------------------------------------------------------------------+
