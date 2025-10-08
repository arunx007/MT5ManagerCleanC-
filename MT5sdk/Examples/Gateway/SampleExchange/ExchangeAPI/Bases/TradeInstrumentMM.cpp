//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeInstrumentMM.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeInstrumentMM::CTradeInstrumentMM(void)
   : m_tick_bid(0),
     m_tick_trend_total(0),
     m_tick_price_delta_min(0),
     m_tick_price_delta_max(0),
     m_tick_trend_max((uint32_t)GetRandUINT(TREND_TICKS_COUNT_MAX,TREND_TICKS_COUNT_MAX)),
     m_tick_time_next(0),
     m_tick_period_min(0),
     m_tick_period_max(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeInstrumentMM::~CTradeInstrumentMM(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialize trade instrument                                      |
//+------------------------------------------------------------------+
void CTradeInstrumentMM::Initialize(LPCWSTR symbol_name,
                                    LPCWSTR curr_base,
                                    LPCWSTR curr_profit,
                                    uint32_t exec_mode,
                                    uint32_t digits,
                                    double settlement_price)
  {
//--- initialize trade instrument fields
   CTradeInstrument::Initialize(symbol_name,curr_base,curr_profit,exec_mode,digits,settlement_price);
   m_tick_bid=m_symbol.settlement_price;
   m_tick_price_delta_min=m_symbol.tick_init_price_delta_min;
   m_tick_price_delta_max=m_symbol.tick_init_price_delta_max;
//--- get minimum and maximum tick periods
   TimeConvert(TICK_PERIOD_MIN,m_tick_period_min);
   TimeConvert(TICK_PERIOD_MAX,m_tick_period_max);
  }
//+------------------------------------------------------------------+
//| Get all orders for login                                         |
//+------------------------------------------------------------------+
bool CTradeInstrumentMM::OrdersGet(uint64_t login,ExchangeOrdersArray &orders)
  {
   bool res=true;
//--- look for orders
   for(uint32_t i=0;res && i<m_pending_orders.Total();i++)
      if(login==0 || m_pending_orders[i].order.login==login)
         res=res && orders.Add(&m_pending_orders[i].order);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process an order from the queue of waiting orders                |
//+------------------------------------------------------------------+
bool CTradeInstrumentMM::ProcessOrder(ExchangePendingOrder &pending_order,CTradeAccounts &accounts,CExchangeContext &context)
  {
//--- initialize the order state if not initialized previously
   if(pending_order.order.order_state==ExchangeOrder::ORDER_STATE_UNKNOWN)
      pending_order.order.order_state=ExchangeOrder::ORDER_STATE_CONFIRMED;
//--- set current bid and ask 
   pending_order.order.price_tick_bid   =m_tick_bid;
   pending_order.order.price_tick_ask   =m_tick_bid+m_symbol.tick_spread;
//--- set order price and update position for market order
   if(pending_order.order.action==IMTRequest::TA_PRICE ||
      pending_order.order.action==IMTRequest::TA_REQUEST ||
      pending_order.order.action==IMTRequest::TA_INSTANT ||
      pending_order.order.action==IMTRequest::TA_MARKET||
      pending_order.order.action==IMTRequest::TA_EXCHANGE)
     {
      double profit=0.0;
      //--- set price and update positions
      if(OrderIsBuy(pending_order.order.type_order))
        {
         //--- set order price
         pending_order.order.price_order   =pending_order.order.price_tick_ask;
         //--- if it isn't price request, change position
         if(pending_order.order.action!=IMTRequest::TA_PRICE)
            PositionAddVolume(pending_order.order.login,(int64_t)pending_order.order.volume,pending_order.order.price_order,profit);
        }
      else
        {
         //--- set order price
         pending_order.order.price_order   =pending_order.order.price_tick_bid;
         //--- if it isn't price request, change position
         if(pending_order.order.action!=IMTRequest::TA_PRICE)
            PositionAddVolume(pending_order.order.login,-(int64_t)pending_order.order.volume,pending_order.order.price_order,profit);
        }
      //--- apply profit to balance
      if(profit!=0.0)
         accounts.BalanceAdd(pending_order.order.login,profit);
     }
//--- process pending order
   ProcessPendingOrder(pending_order);
//--- set order ID in external system if it is not set yet
   if(pending_order.order.order_exchange_id==0)
      pending_order.order.order_exchange_id=ExchangeOrder::OrderExchangeIDNext();
//--- set result of order processing
   pending_order.order.result           =MT_RET_REQUEST_DONE;
//--- send order
   return(context.SendOrder(pending_order.order));
  }
//+------------------------------------------------------------------+
//| Process quotes                                                   |
//+------------------------------------------------------------------+
bool CTradeInstrumentMM::ProcessQuotes(CExchangeContext &context)
  {
//--- if the context is not synchronized
   if(context.StateGet()!=CExchangeContext::STATE_SYNCHRONIZED)
      return(true);
//--- current time in milliseconds
   int64_t current_time=0;
//--- if it is time for the next tick
   if(!TimeGet(current_time) || current_time>=m_tick_time_next)
     {
      //--- tick
      ExchangeTick tick={0};
      //--- fill tick
      FillTick(tick);
      //--- calculate next tick's time
      m_tick_time_next=current_time+GetRandUINT(m_tick_period_min,m_tick_period_max);
      //--- send tick to context
      return(context.SendTick(tick));
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Add volume to position                                           |
//+------------------------------------------------------------------+
bool CTradeInstrumentMM::PositionAddVolume(uint64_t login,int64_t volume,double price,double &profit)
  {
//--- update volume
   bool res=CTradeInstrument::PositionAddVolume(login,volume,price,profit);
//--- invert price if it is needed
   if(price!=0.0 && CMTStr::Compare(m_symbol.symbol,L"USD",3)==0)
      price=1/price;
//--- calculate deal cost in USD
   profit=-volume*price*m_symbol.contract_size;
//---- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process pending order                                            |
//+------------------------------------------------------------------+
bool CTradeInstrumentMM::ProcessPendingOrder(const ExchangePendingOrder &pending_order)
  {
   bool order_update=false;
   bool order_delete=false;
   bool res         =false;
//--- determine action with pending order
   switch(pending_order.order.action)
     {
      //--- delete order on activations or delete actions
      case IMTRequest::TA_REQUEST:
      case IMTRequest::TA_INSTANT:
      case IMTRequest::TA_MARKET:
      case IMTRequest::TA_EXCHANGE:
      case IMTRequest::TA_TRANSFER:
      case IMTRequest::TA_ACTIVATE:
      case IMTRequest::TA_ACTIVATE_SL:
      case IMTRequest::TA_ACTIVATE_TP:
      case IMTRequest::TA_STOPOUT_ORDER:
      case IMTRequest::TA_EXPIRATION:
      case IMTRequest::TA_DEALER_ORD_REMOVE:
      case IMTRequest::TA_DEALER_ORD_ACTIVATE:
         order_delete=true;
         break;
         //--- update order on modification actions
      case IMTRequest::TA_PENDING:
      case IMTRequest::TA_MODIFY:
      case IMTRequest::TA_SLTP:
      case IMTRequest::TA_REMOVE:
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
      case IMTRequest::TA_DEALER_ORD_PENDING:
      case IMTRequest::TA_DEALER_ORD_MODIFY:
      case IMTRequest::TA_DEALER_ORD_SLIMIT:
         order_update=true;
         break;
         //--- do nothing in other cases
      default:
         break;
     }
//--- if need some action with order, look for it
   if(order_update || order_delete)
     {
      //--- go trough all orders and look for specified order
      for(uint32_t i=0;i<m_pending_orders.Total();i++)
         if(m_pending_orders[i].order.order_exchange_id==pending_order.order.order_exchange_id ||
            m_pending_orders[i].order.order_mt_id==pending_order.order.order_mt_id)
           {
            //--- update order data
            if(order_update)
               m_pending_orders[i]=pending_order;
            //--- delete order
            if(order_delete)
               m_pending_orders.Delete(i);
            //--- return
            res=true;
            break;
           }
     }
//--- if need order update and it was not found, add it to array
   if(order_update && !res)
      res=m_pending_orders.Add(&pending_order);
   else
      res=true;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill tick data by symbol                                         |
//+------------------------------------------------------------------+
void CTradeInstrumentMM::FillTick(ExchangeTick &tick)
  {
//--- fill symbol
   CMTStr::Copy(tick.symbol,_countof(tick.symbol),m_symbol.symbol);
//--- fill price source
   CMTStr::Copy(tick.bank,_countof(tick.bank),ProgramName);
//--- check if trend period is over
   if(m_tick_trend_total>m_tick_trend_max)
     {
      //--- trend period is over, recalculate range for generating random price increment
      double tmp            =m_tick_price_delta_min;
      m_tick_price_delta_min=m_tick_price_delta_max*(-1);
      m_tick_price_delta_max=tmp*(-1);
      //--- reset number of trend ticks
      m_tick_trend_total    =0;
      //--- generate random maximum number of trend ticks
      m_tick_trend_max      =(uint32_t)GetRandUINT(TREND_TICKS_COUNT_MIN,TREND_TICKS_COUNT_MAX);
     }
   else
     {
      //--- increment number of trend ticks
      m_tick_trend_total++;
     }
//--- fill bid price
   tick.bid=m_tick_bid+=GetRandDbl(m_tick_price_delta_min,m_tick_price_delta_max);
//--- fill ask price
   tick.ask=m_tick_bid+m_symbol.tick_spread;
//--- fill last price
   tick.last=TICK_PRICE_LAST;
//--- fill last deal volume
   tick.volume=TICK_VOLUME;
  }
//+------------------------------------------------------------------+
//| Generate random value of the double type in specified range      |
//+------------------------------------------------------------------+
double CTradeInstrumentMM::GetRandDbl(const double min,const double max)
  {
   return((double)rand()/(RAND_MAX+1)*(max-min)+min);
  }
//+------------------------------------------------------------------+
//| Generate random value of the uint32_t type in specified range        |
//+------------------------------------------------------------------+
uint64_t CTradeInstrumentMM::GetRandUINT(const uint64_t min,const uint64_t max)
  {
   return((uint64_t)((double)rand()/(RAND_MAX+1)*(max-min)+min));
  }
//+------------------------------------------------------------------+
