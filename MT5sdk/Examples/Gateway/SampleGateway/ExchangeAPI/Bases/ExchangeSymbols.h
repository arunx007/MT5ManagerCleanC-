//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Preliminary declarations                                         |
//+------------------------------------------------------------------+
class CGateway;
//+------------------------------------------------------------------+
//| Base of available symbols                                        |
//+------------------------------------------------------------------+
class CExchangeSymbols
  {
private:
   //--- reference to gateway object
   CGateway         &m_gateway;
   //--- array of symbols
   ExchangeSymbolsArray m_symbols;
   //--- lock on array of symbols
   CMTSync           m_sync;

public:
   //--- constructor/destructor
                     CExchangeSymbols(CGateway *gateway);
                    ~CExchangeSymbols(void);
   //--- add symbol
   bool              OnSymbolAdd(const ExchangeSymbol &exchange_symbol);
   //--- add tick
   bool              OnSymbolTickApply(const ExchangeTick &exchange_tick);
   //--- process book transaction
   bool              OnSymbolBookApply(const ExchangeBook &exchange_book);
   //--- process order transaction
   bool              OnSymbolOrderApply(const ExchangeOrder &order);
   //--- process deal transaction
   bool              OnSymbolDealApply(const ExchangeDeal &deal);
   //--- find symbol by name
   bool              SymbolGet(LPCWSTR symbol_name,ExchangeSymbol &symbol);

private:
   //--- check if symbol exists in base (search by name)
   bool              SymbolExists(LPCWSTR symbol);
   //--- copy symbol
   void              SymbolCopy(const ExchangeSymbol &source_symbol,ExchangeSymbol &dest_symbol);
  };
//+------------------------------------------------------------------+
