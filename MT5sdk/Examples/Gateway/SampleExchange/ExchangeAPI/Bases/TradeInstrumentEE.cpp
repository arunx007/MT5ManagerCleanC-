//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeInstrumentEE.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeInstrumentEE::CTradeInstrumentEE(void)
   : m_book_modified(false),
     m_last_deal_price(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeInstrumentEE::~CTradeInstrumentEE(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialize trade instrument                                      |
//+------------------------------------------------------------------+
void CTradeInstrumentEE::Initialize(LPCWSTR symbol_name,
                                    LPCWSTR curr_base,
                                    LPCWSTR curr_profit,
                                    uint32_t exec_mode,
                                    uint32_t digits,
                                    double settlement_price)
  {
//--- set last deal price equal to settlement price
   m_last_deal_price=settlement_price;
//--- initialize trade instrument fields
   CTradeInstrument::Initialize(symbol_name,curr_base,curr_profit,exec_mode,digits,settlement_price);
  }
//+------------------------------------------------------------------+
//| Manage trade instrument description                              |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::SymbolSend(CExchangeContext &context,uint32_t index)
  {
//--- base call
   bool res=CTradeInstrument::SymbolSend(context,index);
//--- set depth of market modification attribute
   m_book_modified=true;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get all orders for login                                         |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::OrdersGet(uint64_t login,ExchangeOrdersArray &orders)
  {
   bool res=true;
//--- look for buy orders
   for(uint32_t i=0;res && i<m_orders_buy.Total();i++)
      if(login==0 || m_orders_buy[i].order.login==login)
         res=res && orders.Add(&m_orders_buy[i].order);
//--- look for market buy orders
   for(uint32_t i=0;res && i<m_orders_buy_market.Total();i++)
      if(login==0 || m_orders_buy_market[i].order.login==login)
         res=res && orders.Add(&m_orders_buy_market[i].order);
//--- look for sell orders
   for(uint32_t i=0;res && i<m_orders_sell.Total();i++)
      if(login==0 || m_orders_sell[i].order.login==login)
         res=res && orders.Add(&m_orders_sell[i].order);
//--- look for market sell orders
   for(uint32_t i=0;res && i<m_orders_sell_market.Total();i++)
      if(login==0 || m_orders_sell_market[i].order.login==login)
         res=res && orders.Add(&m_orders_sell_market[i].order);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process the queue of orders                                      |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrders(ExchangePendingOrdersArray &pending_orders,CTradeAccounts &accounts,CExchangeContext &context)
  {
//--- process orders
   bool res=CTradeInstrument::ProcessOrders(pending_orders,accounts,context);
//--- cancel expired orders
   res=res && ProcessOrdersExpired(context);
//--- perform deals by received order queue (auction)
   res=res && ProcessDeals(accounts,context);
//--- generate depth of market
   res=res && ProcessQuotesBook(context);
//--- reset depth of market modification attribute
   m_book_modified=false;
//--- return result
   return(true);
  }
//+------------------------------------------------------------------+
//| Process an order from the queue of orders for processing         |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrder(ExchangePendingOrder &pending_order,CTradeAccounts &accounts,CExchangeContext &context)
  {
   bool res=true;
//--- depending on order action type
   switch(pending_order.order.order_action)
     {
      case ExchangeOrder::ORDER_ACTION_ADD:
        {
         //--- process adding new orders
         res=ProcessOrderAdd(pending_order,context);
         break;
        }
      case ExchangeOrder::ORDER_ACTION_MODIFY:
        {
         //--- process order modification
         res=ProcessOrderModify(pending_order,context);
         break;
        }
      case ExchangeOrder::ORDER_ACTION_CANCEL:
        {
         //--- process order cancelation
         res=ProcessOrderCancel(pending_order,context);
         break;
        }
      default:
        {
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Add volume to position                                           |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::PositionAddVolume(uint64_t login,int64_t volume,double price,double &profit)
  {
   ExchangePosition *position_ptr=NULL;
   bool              res         =false;
   profit=0.0;
//--- lock
   m_positions_sync.Lock();
//--- get position
   position_ptr=PositionGetPtr(login);
//--- change position volume
   if(position_ptr)
     {
      //--- check if position is decreasing
      if(position_ptr->volume>0 && volume<0 || position_ptr->volume<0 && volume>0)
        {
         //--- calc closed volume
         int64_t closed_volume=volume;
         if(abs(position_ptr->volume)<abs(volume))
            closed_volume=-position_ptr->volume;
         //--- calc profit
         profit=-(position_ptr->volume*position_ptr->price+closed_volume*price)*m_symbol.tick_value/m_symbol.tick_size;
        }
      //--- add volume to position
      position_ptr->PositionAddVolume(*position_ptr,volume,price);
     }
   else
     {
      //--- if position wasn't found, create new one
      ExchangePosition position={0};
      //--- set login
      position.login=login;
      //--- copy symbol
      CMTStr::Copy(position.symbol,m_symbol.symbol);
      //--- set volume
      position.volume=volume;
      //--- set price
      position.price=price;
      //--- set digits
      position.digits=m_symbol.digits;
      //--- add position to array
      res=m_positions.Add(&position);
     }
//--- unlock
   m_positions_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process adding an order                                          |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrderAdd(ExchangePendingOrder &pending_order,CExchangeContext &context)
  {
//--- set the order to ORDER_STATE_REQUEST_PLACED state
   pending_order.order.order_state=ExchangeOrder::ORDER_STATE_REQUEST_PLACED;
//--- set order ID in external system
   pending_order.order.order_exchange_id=ExchangeOrder::OrderExchangeIDNext();
//--- set order processing result
   pending_order.order.result=MT_RET_REQUEST_PLACED;
//--- send order acceptance confirmation
   bool res=context.SendOrder(pending_order.order);
//--- add order if order acceptance confirmation has been sent
   if(res)
     {
      //--- added order pointer
      ExchangePendingOrder *result_order=NULL;
      //--- add order
      if(OrderAdd(pending_order,&result_order) && result_order)
        {
         //--- order has been added, set it to ORDER_STATE_NEW state
         result_order->order.order_state=ExchangeOrder::ORDER_STATE_NEW;
        }
      else
        {
         //--- order not added, notify client of the order rejection
         result_order=&pending_order;
         //--- set the order to ORDER_STATE_REJECT_NEW state
         result_order->order.order_state=ExchangeOrder::ORDER_STATE_REJECT_NEW;
        }
      //--- set result of order processing
      result_order->order.result=MT_RET_OK_NONE;
      //--- send order
      res=context.SendOrder(result_order->order);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process request fot modifying the order                          |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrderModify(ExchangePendingOrder &pending_order,CExchangeContext &context)
  {
   bool res=true;
//--- set the order to ORDER_STATE_REQUEST_MODIFY state
   pending_order.order.order_state=ExchangeOrder::ORDER_STATE_REQUEST_MODIFY;
//--- set result of order processing
   pending_order.order.result=MT_RET_REQUEST_PLACED;
//--- send order acceptance confirmation
   res=res && context.SendOrder(pending_order.order);
//--- modify order if order acceptance confirmation has been sent
   if(res)
     {
      //--- modified order pointer
      ExchangePendingOrder *result_order=NULL;
      //--- modify order
      if(OrderModify(pending_order,&result_order))
        {
         //--- order has been modified, set it to ORDER_STATE_MODIFY state
         result_order->order.order_state=ExchangeOrder::ORDER_STATE_MODIFY;
        }
      else
        {
         //--- order not modified, notify client of the order rejection
         result_order=&pending_order;
         //--- set the order to ORDER_STATE_REJECT_MODIFY state
         result_order->order.order_state=ExchangeOrder::ORDER_STATE_REJECT_MODIFY;
        }
      //--- set result of order processing
      result_order->order.result=MT_RET_OK_NONE;
      //--- send order
      res=res && context.SendOrder(result_order->order);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process request for canceling an order                           |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrderCancel(ExchangePendingOrder &pending_order,CExchangeContext &context)
  {
   bool res=true;
//--- set the order to ORDER_STATE_REQUEST_CANCEL state
   pending_order.order.order_state=ExchangeOrder::ORDER_STATE_REQUEST_CANCEL;
//--- set result of order processing
   pending_order.order.result=MT_RET_REQUEST_PLACED;
//--- send order acceptance confirmation
   res=res && context.SendOrder(pending_order.order);
//--- cancel order if order acceptance confirmation has been sent
   if(res)
     {
      //--- cancel order
      if(OrderCancel(pending_order))
        {
         //--- order has been canceled, set it to ORDER_STATE_CANCEL state
         pending_order.order.order_state=ExchangeOrder::ORDER_STATE_CANCEL;
        }
      else
        {
         //--- order not canceled, notify client of the order rejection
         pending_order.order.order_state=ExchangeOrder::ORDER_STATE_REJECT_CANCEL;
        }
      //--- set result of order processing
      pending_order.order.result=MT_RET_OK_NONE;
      //--- send order
      res=res && context.SendOrder(pending_order.order);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Cancel expired orders                                            |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrdersExpired(CExchangeContext &context)
  {
   bool res=true;
//--- delete expired sell orders
   res=res && ProcessOrdersExpired(context,m_orders_buy);
//--- delete expired market sell orders
   res=res && ProcessOrdersExpired(context,m_orders_buy_market);
//--- delete expired buy orders
   res=res && ProcessOrdersExpired(context,m_orders_sell);
//--- delete expired market buy orders
   res=res && ProcessOrdersExpired(context,m_orders_sell_market);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Cancel expired orders                                            |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessOrdersExpired(CExchangeContext &context,ExchangePendingOrdersArray &pending_orders)
  {
   bool res=true;
//--- go through order queue
   for(uint32_t i=0;i<pending_orders.Total();i++)
     {
      //--- current order
      ExchangePendingOrder &pending_order=pending_orders[i];
      //--- if expiration time is not set, skip an order
      if(pending_order.order.expiration_time==0)
         continue;
      //--- get current time
      int64_t current_time=GetExchangeTime();
      //--- if an order has expired
      if(pending_order.order.expiration_time<=current_time)
        {
         //--- set the order to ORDER_STATE_CANCEL state
         pending_order.order.order_state      =ExchangeOrder::ORDER_STATE_CANCEL;
         //--- set result of order processing
         pending_order.order.result           =MT_RET_OK_NONE;
         //--- send notification of an order cancelation
         res=res && context.SendOrder(pending_orders[i].order);
         //--- delete an order from the queue
         pending_orders.Delete(i--);
         //--- depth of market modified
         m_book_modified=true;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process performing a deal                                        |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessDeals(CTradeAccounts &accounts,CExchangeContext &context)
  {
   bool res=true;
//--- exit if depth of market is not modified
   if(!m_book_modified)
      return(res);
//--- buy order
   ExchangePendingOrder *order_buy=NULL;
//--- market buy order
   ExchangePendingOrder *order_buy_market=NULL;
//--- sell order
   ExchangePendingOrder *order_sell=NULL;
//--- market sell order
   ExchangePendingOrder *order_sell_market=NULL;
//--- deal performing flag
   bool deal_performed=false;
   do
     {
      //--- reset orders
      order_buy=order_buy_market=order_sell=order_sell_market=NULL;
      //--- get the earliest sell order
      if((m_orders_buy.Total()>0))
         order_buy=&m_orders_buy[m_orders_buy.Total()-1];
      //--- get the earliest market sell order
      if(m_orders_buy_market.Total()>0)
         order_buy_market=&m_orders_buy_market[m_orders_buy_market.Total()-1];
      //--- get the earliest buy order
      if(m_orders_sell.Total()>0)
         order_sell=&m_orders_sell[m_orders_sell.Total()-1];
      //--- get the earliest market buy order
      if(m_orders_sell_market.Total()>0)
         order_sell_market=&m_orders_sell_market[m_orders_sell_market.Total()-1];
      //--- process a possible deal by received orders
      res=ProcessDeal(accounts,context,order_buy,order_buy_market,order_sell,order_sell_market,deal_performed);
      //--- delete the earliest orders if they are in their final state
      if(res && order_buy && order_buy->order.order_state==ExchangeOrder::ORDER_STATE_COMPLETED)
         res=m_orders_buy.Delete(m_orders_buy.Total()-1);
      if(res && order_buy_market && order_buy_market->order.order_state==ExchangeOrder::ORDER_STATE_COMPLETED)
         res=m_orders_buy_market.Delete(m_orders_buy_market.Total()-1);
      if(res && order_sell && order_sell->order.order_state==ExchangeOrder::ORDER_STATE_COMPLETED)
         res=m_orders_sell.Delete(m_orders_sell.Total()-1);
      if(res && order_sell_market && order_sell_market->order.order_state==ExchangeOrder::ORDER_STATE_COMPLETED)
         res=m_orders_sell_market.Delete(m_orders_sell_market.Total()-1);
     } while(deal_performed);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Perform a deal by orders having the best prices                  |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessDeal(CTradeAccounts &accounts,
                                     CExchangeContext &context,
                                     ExchangePendingOrder *order_buy,
                                     ExchangePendingOrder *order_buy_market,
                                     ExchangePendingOrder *order_sell,
                                     ExchangePendingOrder *order_sell_market,
                                     bool &deal_performed)
  {
//--- reset deal performing flag
   deal_performed=false;
//--- orders for performing a deal
   ExchangePendingOrder *orders[4]={0};
   orders[0]=order_buy;
   orders[1]=order_buy_market;
   orders[2]=order_sell;
   orders[3]=order_sell_market;
//--- sort order array by adding time
   SortOrdersByAddTime(orders,_countof(orders));
//--- data
   ExchangePendingOrder *order_buy_crnt_ptr=NULL,*order_sell_crnt_ptr=NULL;
   ExchangeDeal          deal_buy={0},deal_sell={0};
   ExchangeTick          tick={0};
   bool                  res=true;
//--- go through the order array sorted by adding time. The earliest order has 0 index
   for(uint32_t i=0;i<_countof(orders) && !deal_performed;i++)
     {
      //--- reset pointers to current orders
      order_buy_crnt_ptr=order_sell_crnt_ptr=NULL;
      //--- skip NULL orders
      if(!orders[i])
         continue;
      //--- analyze initial order direction
      bool order_initial_buy=OrderIsBuy(orders[i]->order.type_order);
      //--- remember a pointer to an initial order
      if(order_initial_buy)
         order_buy_crnt_ptr=orders[i];
      else
         order_sell_crnt_ptr=orders[i];
      //--- search for an oppositely directed order for a possible deal
      for(uint32_t j=i+1;j<_countof(orders) && !deal_performed;j++)
        {
         //--- skip NULL orders
         if(!orders[j])
            continue;
         if(order_initial_buy)
           {
            //--- initial order is a buy one. So, search for a sell order
            if(OrderIsBuy(orders[j]->order.type_order))
               continue;
            //--- sell order found. Remember it for performing a deal with a buyer
            order_sell_crnt_ptr=orders[j];
           }
         else
           {
            //--- initial order is a sell one. So, search for a buy order
            if(!OrderIsBuy(orders[j]->order.type_order))
               continue;
            //--- buy order found. Remember it for performing a deal with a seller
            order_buy_crnt_ptr=orders[j];
           }
         //--- check if a deal between a seller and a buyer can be performed
         if(deal_performed=ProcessDeal(accounts,order_buy_crnt_ptr,order_sell_crnt_ptr,deal_buy,deal_sell,tick))
           {
            //---send deals
            res=res && context.SendDeal(deal_buy);
            res=res && context.SendDeal(deal_sell);
            //--- send tick
            res=res && context.SendTick(tick);
            //--- deal performed, update state of the orders
            order_buy_crnt_ptr->order.volume-=deal_buy.volume;
            order_sell_crnt_ptr->order.volume-=deal_sell.volume;
            //--- set the sell order to the final state if it has been completely filled
            if(order_buy_crnt_ptr->order.volume==0)
               order_buy_crnt_ptr->order.order_state=ExchangeOrder::ORDER_STATE_COMPLETED;
            //--- set the buy order to the final state if it has been completely filled
            if(order_sell_crnt_ptr->order.volume==0)
               order_sell_crnt_ptr->order.order_state=ExchangeOrder::ORDER_STATE_COMPLETED;
            //--- remeber the last deal price
            m_last_deal_price=deal_buy.price;
           }
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Perform a deal by orders having the best prices                  |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessDeal(CTradeAccounts &accounts,
                                     ExchangePendingOrder *order_buy,
                                     ExchangePendingOrder *order_sell,
                                     ExchangeDeal &deal_buy,
                                     ExchangeDeal &deal_sell,
                                     ExchangeTick &tick)
  {
   bool deal_performed=false;
//--- check
   if(!order_buy || !order_sell)
      return(deal_performed);
//--- clear data
   ZeroMemory(&deal_buy,sizeof(ExchangeDeal));
   ZeroMemory(&deal_sell,sizeof(ExchangeDeal));
   ZeroMemory(&tick,sizeof(ExchangeTick));
//--- if seller's price is less or equal to buyer's price or at least one of the orders is a market one
   if((order_sell->order.price_order<=order_buy->order.price_order) ||
       order_sell->order.price_order==0 ||
       order_buy->order.price_order==0)
     {
      //--- calculate deal price
      double deal_price=m_last_deal_price;
      if(order_sell->order.price_order!=0 && order_sell->order.order_exchange_id<order_buy->order.order_exchange_id)
         deal_price=order_sell->order.price_order;
      else if(order_buy->order.price_order!=0 && order_buy->order.order_exchange_id<order_sell->order.order_exchange_id)
            deal_price=order_buy->order.price_order;
         else
            return(false);
      //--- calculate the maximum possible deal volume
      uint64_t deal_volume=0;
      if(order_sell->order.volume!=0)
         deal_volume=order_sell->order.volume;
      if(order_buy->order.volume!=0 && order_buy->order.volume < order_sell->order.volume)
         deal_volume=order_buy->order.volume;
      //---
      //--- fill in a buy deal
      //---
      //--- copy a symbol
      CMTStr::Copy(deal_buy.symbol, _countof(deal_buy.symbol), m_symbol.symbol);
      //--- deal type
      deal_buy.type_deal=        IMTDeal::DEAL_BUY;
      //--- fill in a deal volume
      deal_buy.volume=           deal_volume;
      //--- fill in a deal price
      deal_buy.price=            deal_price;
      //--- generate ID
      deal_buy.deal_exchange_id= ExchangeDeal::DealExchangeIDNext();
      //--- fill in a login
      deal_buy.login=            order_buy->order.login;
      //--- fill in an order ID
      deal_buy.order=            order_buy->order.order_exchange_id;
      //--- fill in non-filled volume in an order
      deal_buy.volume_remaind=   order_buy->order.volume-deal_volume;
      //---
      //--- fill in a sell deal
      //---
      //--- copy a symbol
      CMTStr::Copy(deal_sell.symbol,_countof(deal_sell.symbol),m_symbol.symbol);
      //--- deal type
      deal_sell.type_deal=       IMTDeal::DEAL_SELL;
      //--- fill in a deal volume
      deal_sell.volume=          deal_volume;
      //--- fill in a deal price
      deal_sell.price=           deal_price;
      //--- generate ID
      deal_sell.deal_exchange_id=ExchangeDeal::DealExchangeIDNext();
      //--- fill in a login
      deal_sell.login=           order_sell->order.login;
      //--- fill in an order ID
      deal_sell.order=           order_sell->order.order_exchange_id;
      //--- fill in non-filled volume in an order
      deal_sell.volume_remaind=  order_sell->order.volume-deal_volume;
      //---
      //--- fill in tick
      //---
      //--- fill in symbol
      CMTStr::Copy(tick.symbol,_countof(tick.symbol),m_symbol.symbol);
      //--- fill in price source
      CMTStr::Copy(tick.bank,_countof(tick.bank),ProgramName);
      //--- fill in last price
      tick.last=deal_price;
      //--- fill in last deal volume
      tick.volume=deal_volume;
      //--- deal performed
      deal_performed=true;
      //--- profit
      double profit=0.0;
      //--- apply deal to buyer position
      PositionAddVolume(deal_buy.login,(int64_t)deal_buy.volume,deal_buy.price,profit);
      //--- apply profit to buyer balance
      accounts.BalanceAdd(deal_buy.login,profit);
      //--- apply deal to seller position
      PositionAddVolume(deal_sell.login,-(int64_t)deal_sell.volume,deal_sell.price,profit);
      //--- apply profit to seller balance
      accounts.BalanceAdd(deal_sell.login,profit);
     }
//--- return result
   return(deal_performed);
  }
//+------------------------------------------------------------------+
//| Generate data for quote book                                     |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::ProcessQuotesBook(CExchangeContext &context)
  {
//--- exit if depth of market is not modified
   if(!m_book_modified)
      return(true);
//--- depth of market
   ExchangeBook book={0};
//--- fill in depth of market symbol
   CMTStr::Copy(book.symbol,_countof(book.symbol),m_symbol.symbol);
//--- fill in depth of market flags
   book.flags=ExchangeBook::FLAG_NONE;
//--- reset the number of depth of market items
   book.items_total=0;
//---
//--- fill in depth of market buy items
//---
//--- counter of depth of market buy items
   uint32_t buy_items_total=0;
//--- go through buy orders
   for(uint32_t i=m_orders_buy.Total();i>0;i--)
     {
      //--- if item array is empty or current order cannot be aggregated with the previous item
      if(book.items_total==0 || m_orders_buy[i-1].order.price_order!=book.items[book.items_total-1].price)
        {
         //--- fill in depth of market item
         book.items[book.items_total].type=ExchangeBookItem::ItemBuy;
         book.items[book.items_total].price=m_orders_buy[i-1].order.price_order;
         book.items[book.items_total].volume=m_orders_buy[i-1].order.volume;
         //--- increment the counter of depth of market buy items and the total amount of depth of market items
         book.items_total++;
         buy_items_total++;
         //--- stop going through if depth of market already has the necessary amount of items
         if(book.items_total==m_symbol.market_depth)
            break;
        }
      else
        {
         //--- increment the volume of the last item by the volume of the current order
         book.items[book.items_total-1].volume+=m_orders_buy[i-1].order.volume;
        }
     }
//---
//--- fill in depth of market sell items
//---
//--- counter of depth of market sell items
   uint32_t sell_items_total=0;
//--- go through sell orders
   for(uint32_t i=m_orders_sell.Total();i>0;i--)
     {
      //--- if item array is empty or current order cannot be aggregated with the previous item
      if(sell_items_total==0 || m_orders_sell[i-1].order.price_order!=book.items[book.items_total-1].price)
        {
         //--- fill in depth of market item
         book.items[book.items_total].type=ExchangeBookItem::ItemSell;
         book.items[book.items_total].price=m_orders_sell[i-1].order.price_order;
         book.items[book.items_total].volume=m_orders_sell[i-1].order.volume;
         //--- increment the counter of depth of market sell items and the total amount of depth of market items
         sell_items_total++;
         book.items_total++;
         //--- stop going through if depth of market already has the necessary amount of items
         if(sell_items_total==m_symbol.market_depth)
            break;
        }
      else
        {
         //--- increment the volume of the last item by the volume of the current order
         book.items[book.items_total-1].volume+=m_orders_sell[i-1].order.volume;
        }
     }
//--- add aggregated market buy orders to depth of market if necessary
   if(sell_items_total==0 && m_orders_buy_market.Total())
      ProcessQuotesBookMarket(book,m_orders_buy_market);
//--- add aggregated market sell orders to depth of market if necessary
   if(buy_items_total==0 && m_orders_sell_market.Total())
      ProcessQuotesBookMarket(book,m_orders_sell_market);
//--- send depth of market to gateway
   return(context.SendBook(book));
  }
//+------------------------------------------------------------------+
//| Add market orders to depth of market                             |
//+------------------------------------------------------------------+
void CTradeInstrumentEE::ProcessQuotesBookMarket(ExchangeBook &book,ExchangePendingOrdersArray &orders_market)
  {
//--- exit if there are no market orders
   if(orders_market.Total()==0)
      return;
//--- define direction of market orders in the queue
   uint32_t pending_orders_type=(OrderIsBuy(orders_market[0].order.type_order)) ? ExchangeBookItem::ItemBuy : ExchangeBookItem::ItemSell;
//--- get the total volume of market orders
   uint64_t orders_volume_total=OrdersVolumeTotal(orders_market);
//--- number of depth of market items having a common direction with the market orders in the queue
   uint32_t current_trend_book_tems_total=0;
//--- index, according to which a new item is inserted into depth of market
   uint32_t insert_index=0;
//--- go through depth of market items
   for(uint32_t i=0;i<book.items_total;i++)
     {
      //--- skip items having a type different from market orders' one
      if(book.items[i].type!=pending_orders_type)
         continue;
      //--- if item price is equal to market order price
      if(book.items[i].price==m_last_deal_price)
        {
         //--- increment item volume by the total volume of market orders
         book.items[i].volume+=orders_volume_total;
         //--- exit
         return;
        }
      else
        {
         //--- find element having the worst price and remeber its index
         if(current_trend_book_tems_total==0 ||
            ((pending_orders_type==ExchangeBookItem::ItemBuy && book.items[i].price<book.items[insert_index].price) ||
             (pending_orders_type==ExchangeBookItem::ItemSell && book.items[i].price>book.items[insert_index].price)))
           {
            insert_index=i;
           }
        }
      //--- increment the number of depth of market items having a necessary direction
      current_trend_book_tems_total++;
     }
//--- if depth of market is not filled yet, insert an item to the end of depth of market
   if(current_trend_book_tems_total<m_symbol.market_depth)
      insert_index=book.items_total++;
//--- if the price of the worst depth of market item is worse than the price of market orders
   if(book.items[insert_index].price==0 ||
      ((pending_orders_type==ExchangeBookItem::ItemBuy && book.items[insert_index].price<m_last_deal_price) ||
       (pending_orders_type==ExchangeBookItem::ItemSell && book.items[insert_index].price>m_last_deal_price)))
     {
      //--- fill in depth of market item
      book.items[insert_index].type=pending_orders_type;
      book.items[insert_index].price=m_last_deal_price;
      book.items[insert_index].volume=orders_volume_total;
     }
  }
//+------------------------------------------------------------------+
//| Add an order                                                     |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::OrderAdd(ExchangePendingOrder &pending_order,ExchangePendingOrder **result_order_pptr)
  {
   bool res=true;
//--- check
   if(result_order_pptr==NULL)
      return(false);
//--- reset the result
   *result_order_pptr=NULL;
//--- check if the order price is correct
   if(pending_order.order.price_order &&
      (m_symbol.price_limit_max!=0 && pending_order.order.price_order > m_symbol.price_limit_max ||
      m_symbol.price_limit_min!=0 && pending_order.order.price_order < m_symbol.price_limit_min))
      return(false);
//--- depending on the order direction
   if(OrderIsBuy(pending_order.order.type_order))
     {
      //--- if the order price is not zero, insert it to the queue of limit buy orders
      if(pending_order.order.price_order!=0)
         *result_order_pptr=m_orders_buy.Insert(&pending_order,SortPendingOrdersByPriceTimeAsk);
      //--- if the price is zero, insert it to the queue of market buy orders
      else
         *result_order_pptr=m_orders_buy_market.Insert(&pending_order,SortPendingOrdersByPriceTimeAsk);
     }
   else
     {
      //--- if the order price is not zero, insert it to the queue of limit sell orders
      if(pending_order.order.price_order!=0)
         *result_order_pptr=m_orders_sell.Insert(&pending_order,SortPendingOrdersByPriceTimeDesc);
      //--- if the price is zero, insert it to the queue of market sell orders
      else
         *result_order_pptr=m_orders_sell_market.Insert(&pending_order,SortPendingOrdersByPriceTimeDesc);
     }
//--- if adding ended with an error   
   if(*result_order_pptr==NULL)
     {
      res=false;
     }
   else
     {
      //--- set depth of market modification attribute
      m_book_modified=true;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Modify an order                                                  |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::OrderModify(ExchangePendingOrder &pending_order,ExchangePendingOrder **result_order_pptr)
  {
   bool res=false;
//--- check
   if(result_order_pptr==NULL)
      return(false);
//--- reset the result
   *result_order_pptr=NULL;
//--- check if the order price is correct if it has been modified
   if(pending_order.order.price_order!=0 &&
      (m_symbol.price_limit_max!=0 && pending_order.order.price_order > m_symbol.price_limit_max ||
      m_symbol.price_limit_min!=0 && pending_order.order.price_order < m_symbol.price_limit_min))
      return(false);
//--- select a queue for searching for a modified order
   ExchangePendingOrdersArray &pending_orders=(OrderIsBuy(pending_order.order.type_order)) ? m_orders_buy : m_orders_sell;
//--- go through the queue
   for(uint32_t i=0;i<pending_orders.Total();i++)
     {
      //--- if order ID is equal to ID of a modification order
      if(pending_orders[i].order.order_exchange_id==pending_order.order.order_exchange_id)
        {
         //--- modify order price
         if(pending_order.order.price_order!=0)
            pending_orders[i].order.price_order=pending_order.order.price_order;
         //--- modify Stop Loss order price
         if(pending_order.order.price_SL!=0)
            pending_orders[i].order.price_SL=pending_order.order.price_SL;
         //--- modify Take Profit order price
         if(pending_order.order.price_TP!=0)
            pending_orders[i].order.price_TP=pending_order.order.price_TP;
         //--- modify expiration time
         pending_orders[i].order.expiration_time=pending_order.order.expiration_time;
         //--- sort the array after modifying the prices
         if(OrderIsBuy(pending_order.order.type_order))
            pending_orders.Sort(SortPendingOrdersByPriceTimeAsk);
         else
            pending_orders.Sort(SortPendingOrdersByPriceTimeDesc);
         res=true;
         break;
        }
     }
//--- if order is modified, find it in the queue
   if(res)
     {
      //--- set depth of market modification attribute
      m_book_modified=true;
      //--- go through the queue
      for(uint32_t i=0;i<pending_orders.Total();i++)
        {
         //--- return an order with a necessary ID
         if(pending_orders[i].order.order_exchange_id==pending_order.order.order_exchange_id)
           {
            *result_order_pptr=&pending_orders[i];
            break;
           }
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Cancel an order                                                  |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::OrderCancel(ExchangePendingOrder &pending_order)
  {
   bool res=false;
//--- select a queue to find an order to be canceled
   ExchangePendingOrdersArray &pending_orders=(OrderIsBuy(pending_order.order.type_order)) ? m_orders_buy : m_orders_sell;
//--- select a queue to find a market order to be canceled
   ExchangePendingOrdersArray &pending_orders_market=(OrderIsBuy(pending_order.order.type_order)) ? m_orders_buy_market : m_orders_sell_market;
//--- delete from order queue
   res=OrderDelete(pending_orders,pending_order);
//--- if not found, delete from market order queue
   if(!res)
      res=OrderDelete(pending_orders_market,pending_order);
//--- if deleted, set depth of market modification attribute
   if(res)
      m_book_modified=true;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Delete an order from a queue                                     |
//+------------------------------------------------------------------+
bool CTradeInstrumentEE::OrderDelete(ExchangePendingOrdersArray &pending_orders,ExchangePendingOrder &pending_order)
  {
   bool res=false;
//--- go through a queue
   for(uint32_t i=0;i<pending_orders.Total();i++)
     {
      //--- if order ID is equal to ID of a deletion order
      if(pending_orders[i].order.order_exchange_id==pending_order.order.order_exchange_id)
        {
         //--- delete an order
         res=pending_orders.Delete(i);
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get total volume of an order queue                               |
//+------------------------------------------------------------------+
uint64_t CTradeInstrumentEE::OrdersVolumeTotal(ExchangePendingOrdersArray &pending_orders)
  {
//--- total volume of orders
   uint64_t volume_total=0;
//--- go through order queue
   for(uint32_t i=0;i<pending_orders.Total();i++)
     {
      //--- increment total volume by the current order volume
      volume_total+=pending_orders[i].order.volume;
     }
//--- return total volume of orders
   return(volume_total);
  }
//+------------------------------------------------------------------+
//| Get current time of exchange                                     |
//+------------------------------------------------------------------+
int64_t CTradeInstrumentEE::GetExchangeTime(void)
  {
//--- use UTC for exchange time
   return(_time64(nullptr));
  }
//+------------------------------------------------------------------+
//| Sort the array of pointers to orders                             |
//+------------------------------------------------------------------+
void CTradeInstrumentEE::SortOrdersByAddTime(ExchangePendingOrder **pending_orders,uint32_t pending_orders_count)
  {
//--- check
   if(!pending_orders || pending_orders_count<1)
      return;
   ExchangePendingOrder *tmp_order=NULL;
//--- sort order by time, NULL items are placed to the end of an array
   for(uint32_t i=0;i<pending_orders_count;i++)
     {
      for(uint32_t j=i+1;j<pending_orders_count;j++)
        {
         if(pending_orders[i]==NULL ||
            (pending_orders[j]!=NULL && pending_orders[i]->order_add_time>pending_orders[j]->order_add_time))
           {
            tmp_order=pending_orders[i];
            pending_orders[i]=pending_orders[j];
            pending_orders[j]=tmp_order;
           }
        }
     }
  }
//+------------------------------------------------------------------+
//| Sort orders for processing by price and time ascending           |
//+------------------------------------------------------------------+
int32_t CTradeInstrumentEE::SortPendingOrdersByPriceTimeAsk(const void *left,const void *right)
  {
//--- convert pointers into a necessary type
   const ExchangePendingOrder &left_pending_order_ptr=*(const ExchangePendingOrder *)left;
   const ExchangePendingOrder &right_pending_order_ptr=*(const ExchangePendingOrder *)right;
//--- compare order prices
   if(left_pending_order_ptr.order.price_order>right_pending_order_ptr.order.price_order)
      return(1);
   if(left_pending_order_ptr.order.price_order<right_pending_order_ptr.order.price_order)
      return(-1);
//--- compare order adding time
   if(left_pending_order_ptr.order_add_time>right_pending_order_ptr.order_add_time)
      return(-1);
   else
      return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort orders for processing by price and time descending          |
//+------------------------------------------------------------------+
int32_t CTradeInstrumentEE::SortPendingOrdersByPriceTimeDesc(const void *left,const void *right)
  {
//--- convert pointers into a necessary type
   const ExchangePendingOrder &left_pending_order_ptr=*(const ExchangePendingOrder *)left;
   const ExchangePendingOrder &right_pending_order_ptr=*(const ExchangePendingOrder *)right;
//--- compare order prices
   if(left_pending_order_ptr.order.price_order>right_pending_order_ptr.order.price_order)
      return(-1);
   if(left_pending_order_ptr.order.price_order<right_pending_order_ptr.order.price_order)
      return(1);
//--- compare order adding time
   if(left_pending_order_ptr.order_add_time>right_pending_order_ptr.order_add_time)
      return(-1);
   else
      return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Search for orders for processing by price                        |
//+------------------------------------------------------------------+
int32_t CTradeInstrumentEE::SearchPendingOrdersByPrice(const void *left,const void *right)
  {
//--- convert pointers into a necessary type
   double price=*(double*)left;
   const ExchangePendingOrder &pending_order_ptr=*(const ExchangePendingOrder *)right;
//--- if the necessary price exceeds the order price
   if(price>pending_order_ptr.order.price_order)
      return(1);
//--- if the necessary price is less than the order price
   if(price<pending_order_ptr.order.price_order)
      return(-1);
//--- the necessary price is equal to the order price
   return(0);
  }
//+------------------------------------------------------------------+
