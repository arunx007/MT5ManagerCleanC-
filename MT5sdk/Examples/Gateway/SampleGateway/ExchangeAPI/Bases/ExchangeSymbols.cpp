//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeSymbols.h"
#include "..\..\Bases\Gateway.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeSymbols::CExchangeSymbols(CGateway *gateway)
   : m_gateway(*gateway)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeSymbols::~CExchangeSymbols(void)
  {
  }
//+------------------------------------------------------------------+
//| Add symbol to base                                               |
//+------------------------------------------------------------------+
bool CExchangeSymbols::OnSymbolAdd(const ExchangeSymbol &exchange_symbol)
  {
   bool res=false;
//--- lock
   m_sync.Lock();
//--- find symbol in base
   for(uint32_t i=0;i<m_symbols.Total();i++)
     {
      //--- compare symbol names
      if(CMTStr::Compare(m_symbols[i].symbol,exchange_symbol.symbol)==0)
        {
         //--- symbol found, update it's parameters
         m_symbols[i]=exchange_symbol;
         res=true;
         break;
        }
     }
//--- symbol is not found, add new one
   if(!res)
      res=m_symbols.Add(&exchange_symbol);
//--- notify gateway of added/updated symbol
   if(res)
      m_gateway.OnExchangeSymbolAdd(exchange_symbol);
//--- unlock
   m_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Add tick                                                         |
//+------------------------------------------------------------------+
bool CExchangeSymbols::OnSymbolTickApply(const ExchangeTick &exchange_tick)
  {
//--- find symbol in base
   bool res=SymbolExists(exchange_tick.symbol);
//--- log if tick of the symbol is not found
   if(!res)
     {
      ExtLogger.Out(MTLogErr,L"failed to apply ticks transaction: symbol %s not found",exchange_tick.symbol);
      return(false);
     }
//--- import tick
   MTTick gateway_tick={0};
//--- import symbol
   CMTStr::Copy(gateway_tick.symbol,_countof(gateway_tick.symbol),exchange_tick.symbol);
//--- import price source
   CMTStr::Copy(gateway_tick.bank,_countof(gateway_tick.bank),exchange_tick.bank);
//--- import bid price
   gateway_tick.bid     =exchange_tick.bid;
//--- import ask price
   gateway_tick.ask     =exchange_tick.ask;
//--- import last price
   gateway_tick.last    =exchange_tick.last;
//--- import last deal volume
   gateway_tick.volume  =exchange_tick.volume;
//--- import datetime
   gateway_tick.datetime=exchange_tick.datetime;
//--- send to Gateway API
   return(m_gateway.GatewayTickSend(gateway_tick));
  }
//+------------------------------------------------------------------+
//| Process book transaction                                         |
//+------------------------------------------------------------------+
bool CExchangeSymbols::OnSymbolBookApply(const ExchangeBook &exchange_book)
  {
//--- find symbol in base
   bool res=SymbolExists(exchange_book.symbol);
//--- log if book of the symbol is not found
   if(!res)
     {
      ExtLogger.Out(MTLogErr,L"failed to apply book transaction: symbol %s not found",exchange_book.symbol);
      return(false);
     }
//--- MT5 depth of market
   MTBook book={0};
//--- copy symbol
   CMTStr::Copy(book.symbol,_countof(book.symbol),exchange_book.symbol);
//--- copy time
   book.datetime=exchange_book.datetime;
//--- copy flags
   book.flags=exchange_book.flags;
//--- copy number of depth of market items
   book.items_total=exchange_book.items_total;
//--- copy depth of market items
   for(uint32_t i=0;i<exchange_book.items_total;i++)
      book.items[i]=exchange_book.items[i];
//--- pass book transaction to gateway
   return(m_gateway.GatewayBookSend(book));
  }
//+------------------------------------------------------------------+
//| Process order transaction                                        |
//+------------------------------------------------------------------+
bool CExchangeSymbols::OnSymbolOrderApply(const ExchangeOrder &order)
  {
//--- find symbol in base
   ExchangeSymbol symbol={0};
   bool res=SymbolGet(order.symbol,symbol);
//--- log if order's symbol is not found
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to apply orders transaction: symbol %s not found",order.symbol);
//--- pass order transaction to gateway
   res=res && m_gateway.OnExchangeOrderTrans(order,symbol);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process deal transaction                                         |
//+------------------------------------------------------------------+
bool CExchangeSymbols::OnSymbolDealApply(const ExchangeDeal &deal)
  {
//--- get symbol from base
   ExchangeSymbol symbol={0};
   bool res=SymbolGet(deal.symbol,symbol);
//--- log if order's symbol is not found
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to apply deal transaction: symbol %s not found",deal.symbol);
//--- pass deal transaction to gateway
   res=res && m_gateway.OnExchangeDealTrans(deal,symbol);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Find symbol by name                                              |
//+------------------------------------------------------------------+
bool CExchangeSymbols::SymbolGet(LPCWSTR symbol_name,ExchangeSymbol &symbol)
  {
   bool res=false;
//--- lock
   m_sync.Lock();
   for(uint32_t i=0;i<m_symbols.Total();i++)
     {
      //--- compare symbol names
      if(CMTStr::Compare(m_symbols[i].symbol,symbol_name)==0)
        {
         res=true;
         SymbolCopy(m_symbols[i],symbol);
         break;
        }
     }
//--- unlock
   m_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Check if symbol exists in base (search by name)                  |
//+------------------------------------------------------------------+
bool CExchangeSymbols::SymbolExists(LPCWSTR symbol)
  {
   bool res=false;
//--- lock
   m_sync.Lock();
//--- search
   for(uint32_t i=0;i<m_symbols.Total();i++)
     {
      //--- compare symbol names
      if(CMTStr::Compare(m_symbols[i].symbol,symbol)==0)
        {
         res=true;
         break;
        }
     }
//--- unlock
   m_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Copy symbol                                                      |
//+------------------------------------------------------------------+
void CExchangeSymbols::SymbolCopy(const ExchangeSymbol &source_symbol,ExchangeSymbol &dest_symbol)
  {
   CMTStr::Copy(dest_symbol.symbol,         _countof(dest_symbol.symbol),          source_symbol.symbol);
   CMTStr::Copy(dest_symbol.path,           _countof(dest_symbol.path),            source_symbol.path);
   CMTStr::Copy(dest_symbol.description,    _countof(dest_symbol.description),     source_symbol.description);
   CMTStr::Copy(dest_symbol.page,           _countof(dest_symbol.page),            source_symbol.page);
   CMTStr::Copy(dest_symbol.currency_base,  _countof(dest_symbol.currency_base),   source_symbol.currency_base);
   CMTStr::Copy(dest_symbol.currency_profit,_countof(dest_symbol.currency_profit), source_symbol.currency_profit);
   CMTStr::Copy(dest_symbol.currency_margin,_countof(dest_symbol.currency_margin), source_symbol.currency_margin);
   dest_symbol.digits                   =source_symbol.digits;                    // digits
   dest_symbol.tick_flags               =source_symbol.tick_flags;                // tick flags
   dest_symbol.calc_mode                =source_symbol.calc_mode;                 // calc mode
   dest_symbol.exec_mode                =source_symbol.exec_mode;                 // execution mode
   dest_symbol.fill_flags               =source_symbol.fill_flags;                // fill flags
   dest_symbol.expir_flags              =source_symbol.expir_flags;               // expiration flags
   dest_symbol.tick_value               =source_symbol.tick_value;                // tick value
   dest_symbol.tick_size                =source_symbol.tick_size;                 // tick size
   dest_symbol.contract_size            =source_symbol.contract_size;             // contract size
   dest_symbol.volume_min               =source_symbol.volume_min;                // min volume
   dest_symbol.volume_max               =source_symbol.volume_max;                // max volume
   dest_symbol.volume_step              =source_symbol.volume_step;               // volume step
   dest_symbol.margin_flags             =source_symbol.margin_flags;              // margin flags
   dest_symbol.margin_initial           =source_symbol.margin_initial;            // initial margin
   dest_symbol.margin_maintenance       =source_symbol.margin_maintenance;        // maintenance margin
   dest_symbol.margin_long              =source_symbol.margin_long;               // long margin
   dest_symbol.margin_short             =source_symbol.margin_short;              // short margin
   dest_symbol.margin_limit             =source_symbol.margin_limit;              // margin limit
   dest_symbol.margin_stop              =source_symbol.margin_stop;               // stop margin
   dest_symbol.margin_stop_limit        =source_symbol.margin_stop_limit;         // stop limit margin
   dest_symbol.index                    =source_symbol.index;                     // symbol index
   dest_symbol.settlement_price         =source_symbol.settlement_price;          // settlement price
   dest_symbol.tick_spread              =source_symbol.tick_spread;               // tick spread
   dest_symbol.tick_init_price_delta_min=source_symbol.tick_init_price_delta_min; // tick price delta minimum
   dest_symbol.tick_init_price_delta_max=source_symbol.tick_init_price_delta_max; // tick price delta maximum
   dest_symbol.price_limit_max          =source_symbol.price_limit_max;           // price limit max
   dest_symbol.price_limit_min          =source_symbol.price_limit_min;           // price limit min
   dest_symbol.time_start               =source_symbol.time_start;                // time start
   dest_symbol.time_expiration          =source_symbol.time_expiration;           // time expiration
   dest_symbol.trade_mode               =source_symbol.trade_mode;                // trade mode
  }
//+------------------------------------------------------------------+
