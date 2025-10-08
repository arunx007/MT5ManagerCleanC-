//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of a trade instrument of exchange execution type           |
//+------------------------------------------------------------------+
class CTradeInstrumentEE : public CTradeInstrument
  {

private:
   //--- buy order queue
   ExchangePendingOrdersArray m_orders_buy;
   //--- market buy order queue
   ExchangePendingOrdersArray m_orders_buy_market;
   //--- sell order queue
   ExchangePendingOrdersArray m_orders_sell;
   //--- market sell order queue
   ExchangePendingOrdersArray m_orders_sell_market;
   //--- depth of market change attribute
   bool              m_book_modified;
   //--- last deal price
   double            m_last_deal_price;

public:
   //--- constructor/destructor
                     CTradeInstrumentEE();
   virtual          ~CTradeInstrumentEE();
   //--- initialize trade instrument
   virtual void      Initialize(LPCWSTR symbol_name,
                                LPCWSTR curr_base,
                                LPCWSTR curr_profit,
                                uint32_t exec_mode,
                                uint32_t digits,
                                double price_bid_init);
   //--- manage trade instrument description
   virtual bool      SymbolSend(CExchangeContext &context,uint32_t index);
   //--- get all orders for login
   virtual bool      OrdersGet(uint64_t login,ExchangeOrdersArray &orders);

private:
   //--- process the queue of orders
   virtual bool      ProcessOrders(ExchangePendingOrdersArray &pending_orders,CTradeAccounts &accounts,CExchangeContext &context);
   //--- process an order from the queue of orders for processing
   virtual bool      ProcessOrder(ExchangePendingOrder &pending_order,CTradeAccounts &accounts,CExchangeContext &context);
   //--- add volume to position
   virtual bool      PositionAddVolume(uint64_t login,int64_t volume,double price,double &profit);
   //--- process adding an order
   bool              ProcessOrderAdd(ExchangePendingOrder &pending_order,CExchangeContext &context);
   //--- process request for modifying an order
   bool              ProcessOrderModify(ExchangePendingOrder &pending_order,CExchangeContext &context);
   //--- process request for canceling an order
   bool              ProcessOrderCancel(ExchangePendingOrder &pending_order,CExchangeContext &context);
   //--- cancel expired orders
   bool              ProcessOrdersExpired(CExchangeContext &context);
   //--- cancel expired orders
   bool              ProcessOrdersExpired(CExchangeContext &context,ExchangePendingOrdersArray &pending_orders);
   //--- process performing a deal
   bool              ProcessDeals(CTradeAccounts &accounts,CExchangeContext &context);
   //--- perform a deal by orders having the best prices
   bool              ProcessDeal(CTradeAccounts &accounts,
                                 CExchangeContext &context,
                                 ExchangePendingOrder *order_buy,
                                 ExchangePendingOrder *order_buy_market,
                                 ExchangePendingOrder *order_sell,
                                 ExchangePendingOrder *order_sell_market,
                                 bool &deal_performed);
   bool              ProcessDeal(CTradeAccounts &accounts,
                                 ExchangePendingOrder *order_buy,
                                 ExchangePendingOrder *order_sell,
                                 ExchangeDeal &deal_buy,
                                 ExchangeDeal &deal_sell,
                                 ExchangeTick &tick);
   //--- generate data for quote book
   bool              ProcessQuotesBook(CExchangeContext &context);
   //--- add market orders to depth of market
   void              ProcessQuotesBookMarket(ExchangeBook &book,ExchangePendingOrdersArray &orders_market);
   //--- manage orders: add, modify, cancel
   bool              OrderAdd(ExchangePendingOrder &pending_order,ExchangePendingOrder **result_order_pptr);
   bool              OrderModify(ExchangePendingOrder &pending_order,ExchangePendingOrder **result_order_pptr);
   bool              OrderCancel(ExchangePendingOrder &pending_order);
   //--- delete an order from a queue
   bool              OrderDelete(ExchangePendingOrdersArray &pending_orders,ExchangePendingOrder &pending_order);
   //--- get total volume of an order queue
   uint64_t          OrdersVolumeTotal(ExchangePendingOrdersArray &pending_orders);
   //--- get current time of exchange
   int64_t           GetExchangeTime(void);
   //--- sort the array of pointers to orders
   static void       SortOrdersByAddTime(ExchangePendingOrder **pending_orders,uint32_t pending_orders_count);
   //--- search/sort orders for processing
   static int32_t    SortPendingOrdersByPriceTimeAsk(const void *left,const void *right);
   static int32_t    SortPendingOrdersByPriceTimeDesc(const void *left,const void *right);
   static int32_t    SearchPendingOrdersByPrice(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
