//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeInstrument.h"
//+------------------------------------------------------------------+
//| Performance frequency                                            |
//+------------------------------------------------------------------+
int64_t CTradeInstrument::s_performance_frequency=0;
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeInstrument::CTradeInstrument(void)
  {
   memset(&m_symbol,0,sizeof(m_symbol));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeInstrument::~CTradeInstrument(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialize trade instrument                                      |
//+------------------------------------------------------------------+
void CTradeInstrument::Initialize(LPCWSTR symbol_name,
                                  LPCWSTR curr_base,
                                  LPCWSTR curr_profit,
                                  uint32_t exec_mode,
                                  uint32_t digits,
                                  double settlement_price)
  {
//--- initialize performance frequency if it has not been done already
   LARGE_INTEGER frequency;
   if(s_performance_frequency==0)
      if(::QueryPerformanceFrequency(&frequency)!=0)
         s_performance_frequency=frequency.QuadPart;
//--- initialize a trade instrument
   ExchangeSymbol::Fill(m_symbol,symbol_name,curr_base,curr_profit,exec_mode,digits,settlement_price);
  }
//+------------------------------------------------------------------+
//| Data processing                                                  |
//+------------------------------------------------------------------+
bool CTradeInstrument::ProcessData(CTradeAccounts &accounts,CExchangeContext &context)
  {
//--- process preliminary queue of orders for processing
   bool res=ProcessOrdersIncome(context);
//--- process queue of orders for processing
   res=res && ProcessOrders(m_orders,accounts,context);
//--- process quote data
   res=res && ProcessQuotes(context);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Add a new order to a queue for processing                        |
//+------------------------------------------------------------------+
bool CTradeInstrument::OrderAdd(const ExchangeOrder &order)
  {
   bool res=true;
//--- create an order waiting for handling
   ExchangePendingOrder pending_order={0};
   pending_order.order=order;
//--- fill in order adding timen
   TimeGet(pending_order.order_add_time);
//--- lock
   m_orders_sync.Lock();
//--- add a new order to the queue
   res=res && m_orders_income.Add(&pending_order);
//--- unlock
   m_orders_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send trade instrument description to gateway                     |
//+------------------------------------------------------------------+
bool CTradeInstrument::SymbolSend(CExchangeContext &context,uint32_t index)
  {
//--- send a trade instrument to gateway
   return(context.SendSymbol(m_symbol,index));
  }
//+------------------------------------------------------------------+
//| Get symbol name                                                  |
//+------------------------------------------------------------------+
LPCWSTR CTradeInstrument::SymbolGet() const
  {
//--- return symbol name
   return(m_symbol.symbol);
  }
//+------------------------------------------------------------------+
//| Get all orders for login                                         |
//+------------------------------------------------------------------+
bool CTradeInstrument::OrdersGet(uint64_t login,ExchangeOrdersArray &orders)
  {
//--- return result
   return(true);
  }
//+------------------------------------------------------------------+
//| Get position for login                                           |
//+------------------------------------------------------------------+
bool CTradeInstrument::PositionGet(uint64_t login,ExchangePosition &position)
  {
   ExchangePosition *position_ptr=NULL;
//--- lock
   m_positions_sync.Lock();
//--- get position pointer
   position_ptr=PositionGetPtr(login);
//--- copy position
   if(position_ptr)
      position=*position_ptr;
//--- unlock
   m_positions_sync.Unlock();
//--- return result
   return(position_ptr!=NULL);
  }
//+------------------------------------------------------------------+
//| Get position pointer for login                                   |
//+------------------------------------------------------------------+
ExchangePosition* CTradeInstrument::PositionGetPtr(uint64_t login)
  {
//--- go through all positions and find one for specified login
   for(uint32_t i=0;i<m_positions.Total();i++)
      if(m_positions[i].login==login)
         return(&m_positions[i]);
//--- return result
   return(NULL);
  }
//+------------------------------------------------------------------+
//| Get all positions                                                |
//+------------------------------------------------------------------+
bool CTradeInstrument::PositionsGetAll(ExchangePositionsArray &positions)
  {
//--- lock
   m_positions_sync.Lock();
//--- copy all position
   positions.Add(m_positions);
//--- unlock
   m_positions_sync.Unlock();
//--- return result
   return(true);
  }
//+------------------------------------------------------------------+
//| Process the incoming queue                                       |
//+------------------------------------------------------------------+
bool CTradeInstrument::ProcessOrdersIncome(CExchangeContext &context)
  {
//--- lock
   m_orders_sync.Lock();
//--- add orders
   bool res=m_orders.Add(m_orders_income);
//--- clear the order queue
   m_orders_income.Clear();
//--- unlock
   m_orders_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process the queue of orders                                      |
//+------------------------------------------------------------------+
bool CTradeInstrument::ProcessOrders(ExchangePendingOrdersArray &pending_orders,CTradeAccounts &accounts,CExchangeContext &context)
  {
   bool res=true;
//--- go through orders
   for(uint32_t i=0;i<pending_orders.Total();i++)
     {
      //--- process an order
      res=res && ProcessOrder(pending_orders[i],accounts,context);
      //--- if an order passed to the final state, delete it from the queue
      if(res)
         pending_orders.Delete(i--);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Add volume to position                                           |
//+------------------------------------------------------------------+
bool CTradeInstrument::PositionAddVolume(uint64_t login,int64_t volume,double price,double &profit)
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
      position_ptr->PositionAddVolume(*position_ptr,volume,price);
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
//| Get order direction                                              |
//+------------------------------------------------------------------+
bool CTradeInstrument::OrderIsBuy(uint32_t type_order)
  {
//--- return result
   return(type_order==IMTOrder::OP_BUY ||
          type_order==IMTOrder::OP_BUY_LIMIT ||
          type_order==IMTOrder::OP_BUY_STOP ||
          type_order==IMTOrder::OP_BUY_STOP_LIMIT);
  }
//+------------------------------------------------------------------+
//| Get current time                                                 |
//+------------------------------------------------------------------+
bool CTradeInstrument::TimeGet(int64_t &time_pf)
  {
   LARGE_INTEGER current_time;
   bool res=false;
//--- get current time
   if(::QueryPerformanceCounter(&current_time)!=0)
     {
      time_pf=current_time.QuadPart;
      res=true;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Convert time from milliseconds into ticks                        |
//+------------------------------------------------------------------+
bool CTradeInstrument::TimeConvert(const int64_t &time_msc,int64_t &time_pf)
  {
   bool res=false;
//--- clear the returned time
   time_pf=0;
//--- if performance frequency initialized
   if(s_performance_frequency!=0)
     {
      //--- convert time into ticks
      time_pf=(int64_t)(s_performance_frequency*((double)time_msc/1000));
      res=true;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
