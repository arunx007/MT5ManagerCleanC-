//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeDispatcher.h"
#include "TradeInstrumentEE.h"
#include "TradeInstrumentMM.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeDispatcher::CTradeDispatcher(void)
   : m_event(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeDispatcher::~CTradeDispatcher(void)
  {
//--- close the event
   EventClose();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CTradeDispatcher::Initialize(void)
  {
//--- add Exchange execution type trade instrument
   bool res=TradeInstrumentAdd(L"MQ-FUT",L"USD",L"USD",ExchangeSymbol::EXECUTION_EXCHANGE,3,79.2350);
//--- add Instant execution type trade instrument
   res=res && TradeInstrumentAdd(L"GBPUSD.TEST",L"GPB",L"USD",ExchangeSymbol::EXECUTION_INSTANT,5,1.57670);
//--- add Market execution type trade instrument
   res=res && TradeInstrumentAdd(L"USDCHF.TEST",L"USD",L"CHF",ExchangeSymbol::EXECUTION_MARKET,5,0.94878);
//--- add Request execution type trade instrument
   res=res && TradeInstrumentAdd(L"EURUSD.TEST",L"EUR",L"USD",ExchangeSymbol::EXECUTION_REQUEST,5,1.26591);
//--- create the event
   res=res && EventCreate();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CTradeDispatcher::Shutdown(void)
  {
//--- lock
   m_trade_instruments_sync.Lock();
//--- deallocate array of trade instruments
   for(uint32_t i=0;i<m_trade_instruments.Total();i++)
     {
      if(m_trade_instruments[i])
        {
         TradeInstrumentFree(m_trade_instruments[i]);
         m_trade_instruments[i]=NULL;
        }
     }
//--- clear trade instrument array
   m_trade_instruments.Clear();
//--- close the event
   EventClose();
//--- unlock
   m_trade_instruments_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Wait for data                                                    |
//+------------------------------------------------------------------+
void CTradeDispatcher::DataWait(void)
  {
//--- wait for the event activation
   EventWait(DATA_WAIT_TIME);
  }
//+------------------------------------------------------------------+
//| Data processing                                                  |
//+------------------------------------------------------------------+
bool CTradeDispatcher::DataProcess(CExchangeContext &context)
  {
   bool res=true;
//--- lock
   m_trade_instruments_sync.Lock();
//--- go through trade instruments
   for(uint32_t i=0;i<m_trade_instruments.Total();i++)
     {
      //--- process data on each trade instrument
      if(m_trade_instruments[i])
         res=res && m_trade_instruments[i]->ProcessData(m_trade_accounts,context);
     }
//--- unlock
   m_trade_instruments_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Add order to processing queue                                    |
//+------------------------------------------------------------------+
bool CTradeDispatcher::OrderAdd(const ExchangeOrder &order)
  {
   bool res=false;
//--- lock
   m_trade_instruments_sync.Lock();
//--- search for a trade instrument
   CTradeInstrument **trade_instrument_pptr=m_trade_instruments.Search(&order.symbol,SearchTradeInstrumentsBySymbol);
//--- if found, send order to the trade instrument for processing
   if(trade_instrument_pptr && *trade_instrument_pptr)
      res=(*trade_instrument_pptr)->OrderAdd(order);
//--- unlock
   m_trade_instruments_sync.Unlock();
//--- set the new order receiving event
   EventSet();
//--- if not found, log in the journal 
   if(!res)
      ExtLogger.Out(MTLogErr,L"order transaction error: symbol %s not found",order.symbol);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Export descriptions of trade instruments available for trading   |
//+------------------------------------------------------------------+
bool CTradeDispatcher::SymbolsSend(CExchangeContext &context)
  {
   bool res=true;
//--- lock
   m_trade_instruments_sync.Lock();
//--- go through trade instruments
   for(uint32_t i=0;i<m_trade_instruments.Total();i++)
     {
      //--- export trade instrument description
      if(m_trade_instruments[i])
         res=res && m_trade_instruments[i]->SymbolSend(context,m_trade_instruments.Total()-i-1);
     }
//--- unlock
   m_trade_instruments_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send account data for login                                      |
//+------------------------------------------------------------------+
bool CTradeDispatcher::AccountDataSend(CExchangeContext &context,uint64_t login)
  {
   ExchangeAccountData account_data={0};
   bool                res=true;
//--- set login
   account_data.login=login;
//--- clear balance
   account_data.balance=0.0;
//--- lock
   m_trade_instruments_sync.Lock();
//--- if login specified get all orders and positions for it
   if(login>0)
     {
      ExchangePosition position={0};
      //--- set balance
      account_data.balance=m_trade_accounts.BalanceGet(login);
      //--- go through all trade instruments
      for(uint32_t i=0;res && i<m_trade_instruments.Total();i++)
         if(m_trade_instruments[i])
           {
            //--- get orders for this instrument
            res=res && m_trade_instruments[i]->OrdersGet(login,account_data.orders);
            //--- get position for this instrument
            if(m_trade_instruments[i]->PositionGet(login,position))
               res=res && account_data.positions.Add(&position);
           }
     }
   else
     {
      //--- if login not specified, get all positions for all instruments
      for(uint32_t i=0;res && i<m_trade_instruments.Total();i++)
         if(m_trade_instruments[i])
            res=res && m_trade_instruments[i]->PositionsGetAll(account_data.positions);
     }
//--- unlock
   m_trade_instruments_sync.Unlock();
//--- send account data
   res=res && context.SendAccountData(account_data);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Add a trade instrument                                           |
//+------------------------------------------------------------------+
bool CTradeDispatcher::TradeInstrumentAdd(LPCWSTR symbol_name,
                                          LPCWSTR curr_base,
                                          LPCWSTR curr_profit,
                                          uint32_t exec_mode,
                                          uint32_t digits,
                                          double price_bid_init)
  {
   bool res=false;
//--- lock
   m_trade_instruments_sync.Lock();
//--- if a trade instrument not found
   if(m_trade_instruments.Search(symbol_name,SearchTradeInstrumentsBySymbol)==NULL)
     {
      //--- create instrument object
      CTradeInstrument *trade_instrument_ptr=TradeInstrumentCreate(exec_mode);
      //--- if created successfully
      if(trade_instrument_ptr)
        {
         //--- initialize
         trade_instrument_ptr->Initialize(symbol_name,curr_base,curr_profit,exec_mode,digits,price_bid_init);
         //--- add an instrument
         res=m_trade_instruments.Insert(&trade_instrument_ptr,SortTradeInstrumentsBySymbol)!=NULL;
        }
      //--- delete an instrument object if it cannot be added
      if(!res && trade_instrument_ptr)
         TradeInstrumentFree(trade_instrument_ptr);
     }
//--- unlock
   m_trade_instruments_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Create a trade instrument instance                               |
//+------------------------------------------------------------------+
CTradeInstrument* CTradeDispatcher::TradeInstrumentCreate(uint32_t execution_mode)
  {
   CTradeInstrument* trade_instrument=NULL;
//--- create a trade instrument according to execution type
   switch(execution_mode)
     {
      case ExchangeSymbol::EXECUTION_EXCHANGE:
        {
         //--- create a trade instrument of exchange execution type
         trade_instrument=new(std::nothrow) CTradeInstrumentEE();
         break;
        }
      case ExchangeSymbol::EXECUTION_MARKET:
      case ExchangeSymbol::EXECUTION_REQUEST:
      case ExchangeSymbol::EXECUTION_INSTANT:
        {
         //--- create a trade instrument for Market Maker market
         trade_instrument=new(std::nothrow) CTradeInstrumentMM();
         break;
        }
      default:
         break;
     }
//--- return result
   return(trade_instrument);
  }
//+------------------------------------------------------------------+
//| Delete a trade instrument                                        |
//+------------------------------------------------------------------+
void CTradeDispatcher::TradeInstrumentFree(CTradeInstrument *trade_instrument)
  {
//--- delete a trade instrument
   if(trade_instrument)
      delete trade_instrument;
  }
//+------------------------------------------------------------------+
//| Create the event                                                 |
//+------------------------------------------------------------------+
bool CTradeDispatcher::EventCreate(void)
  {
//--- close the event
   EventClose();
//--- create the event
   m_event=CreateEvent(NULL,FALSE,FALSE,NULL);
//--- return result
   return(m_event!=NULL);
  }
//+------------------------------------------------------------------+
//| Close the event                                                 |
//+------------------------------------------------------------------+
void CTradeDispatcher::EventClose(void)
  {
//--- check
   if(m_event)
     {
      //--- close the event
      CloseHandle(m_event);
      m_event=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Set the event                                                    |
//+------------------------------------------------------------------+
bool CTradeDispatcher::EventSet(void)
  {
//--- set the event
   return(m_event && SetEvent(m_event));
  }
//+------------------------------------------------------------------+
//| Wait for the event                                               |
//+------------------------------------------------------------------+
bool CTradeDispatcher::EventWait(const DWORD timeout)
  {
//--- wait for the event
   return(WaitForSingleObject(m_event,timeout)==WAIT_OBJECT_0);
  }
//+------------------------------------------------------------------+
//| Sort trade instruments by name                                   |
//+------------------------------------------------------------------+
int32_t CTradeDispatcher::SortTradeInstrumentsBySymbol(const void *left,const void *right)
  {
   const CTradeInstrument *left_instrument_ptr=*(const CTradeInstrument **)left;
   const CTradeInstrument *right_instrument_ptr=*(const CTradeInstrument **)right;
//--- compare symbols in trade instruments
   return(CMTStr::Compare((left_instrument_ptr)->SymbolGet(),(right_instrument_ptr)->SymbolGet()));
  }
//+------------------------------------------------------------------+
//| Search trade instruments by name                                 |
//+------------------------------------------------------------------+
int32_t CTradeDispatcher::SearchTradeInstrumentsBySymbol(const void *left,const void *right)
  {
   LPCWSTR symbol_name=(LPCWSTR)left;
   const CTradeInstrument *instrument_ptr=*(const CTradeInstrument **)right;
//--- compare symbols in trade instruments
   return(CMTStr::Compare(symbol_name,(instrument_ptr)->SymbolGet()));
  }
//+------------------------------------------------------------------+
