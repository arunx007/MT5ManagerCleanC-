//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Liquidity Symbol Liquidity Provider record                       |
//+------------------------------------------------------------------+
class IMTConUltSymbolLP
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConUltSymbolLP* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint32_t  ID(void) const=0;
   virtual MTAPIRES  ID(const uint32_t id)=0;
  };
//+------------------------------------------------------------------+
//| Liquidity Symbol DoM band                                        |
//+------------------------------------------------------------------+
class IMTConUltSymbolBand
  {
public:
   //--- DOM side
   enum EnDomLevelSides
     {
      SIDE_BUY     =0,
      SIDE_SELL    =1,
      //---
      SIDE_FIRST   =SIDE_BUY,
      SIDE_LAST    =SIDE_SELL,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConUltSymbolBand* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- volume
   virtual double    Size(void) const=0;
   virtual MTAPIRES  Size(const double size)=0;
   //--- spread
   virtual int64_t   Spread(void) const=0;
   virtual MTAPIRES  Spread(const int64_t spread)=0;
   //--- EnDomLevelSides
   virtual uint32_t  Side(void) const=0;
   virtual MTAPIRES  Side(const uint32_t side)=0;
  };
//+------------------------------------------------------------------+
//| Liquidity Symbol configuration record                            |
//+------------------------------------------------------------------+
class IMTConUltSymbol
  {
public:
   //--- symbol types
   enum EnSymbolTypes
     {
      ULT_SYMBOL_PRIMARY          =0,        // Prioritized Providers / Only one active 
      ULT_SYMBOL_BEST             =1,        // Best provider
      ULT_SYMBOL_AGGREGATED       =2,        // Aggregated Providers
      ULT_SYMBOL_EXCHANGE         =3,        // Exchange
      //--- enumeration borders
      ULT_SYMBOL_FIRST            =ULT_SYMBOL_PRIMARY,
      ULT_SYMBOL_LAST             =ULT_SYMBOL_EXCHANGE
     };
   //--- symbol modes
   enum EnSymbolModes
     {
      SYMBOL_DISABLED             =0,
      SYMBOL_ENABLED              =1,
      //--- enumeration borders
      SYMBOL_FIRST                =SYMBOL_DISABLED,
      SYMBOL_LAST                 =SYMBOL_ENABLED,
     };
   //--- symbol quotes types
   enum EnSymbolQuotesTypes
     {
      ULT_SYMBOL_QUOTES_PRIMARY   =0,        // Prioritized Providers / Only one active
      ULT_SYMBOL_QUOTES_MIXED     =1,        // Aggregated Providers
      ULT_SYMBOL_QUOTES_MIXED_ORDERS=2,      // Aggregated Providers with own Orders
      //--- enumeration borders
      ULT_SYMBOL_QUOTES_FIRST     =ULT_SYMBOL_QUOTES_PRIMARY,
      ULT_SYMBOL_QUOTES_LAST      =ULT_SYMBOL_QUOTES_MIXED_ORDERS
     };
   //--- coverage flags
   enum EnCoverageFlags
     {
      FLAG_COVERAGE_ON_CLIENT_SYMBOL=0x00000001,
      //--- enumeration borders
      FLAG_COVERAGE_NONE         =0x00000000,
      FLAG_COVERAGE_ALL          =FLAG_COVERAGE_ON_CLIENT_SYMBOL,
     };
   //--- B-Book price modes
   enum EnBbookPriceModes
     {
      BBOOK_PRICE_MODE_ABOOK      =0,
      BBOOK_PRICE_MODE_VWAP       =1,
      //--- enumeration borders
      BBOOK_PRICE_MODE_FIRST      =BBOOK_PRICE_MODE_ABOOK,
      BBOOK_PRICE_MODE_LAST       =BBOOK_PRICE_MODE_VWAP
     };
   //--- limit orders and TP activatio modes
   enum EnLimitActivationMode
     {
      LIMIT_ACTIVATION_MARKET =0,
      LIMIT_ACTIVATION_LIMIT  =1,
      //--- enumeration borders
      LIMIT_ACTIVATION_FIRST  =LIMIT_ACTIVATION_MARKET,
      LIMIT_ACTIVATION_LAST   =LIMIT_ACTIVATION_LIMIT
     };
   //--- execution flags
   enum EnExecutionFlags
     {
      FLAG_EXEC_MERGE_DEALS      =0x00000001,
      //--- enumeration borders
      FLAG_EXEC_NONE             =0x00000000,
      FLAG_EXEC_ALL              =FLAG_EXEC_MERGE_DEALS,
     };
   //--- A-Book reject modes
   enum EnRejectModes
     {
      REJECT_MODE_REJECT    =0,
      REJECT_MODE_BBOOK     =1,
      //--- enumeration borders
      REJECT_MODE_FIRST     =REJECT_MODE_REJECT,
      REJECT_MODE_LAST      =REJECT_MODE_BBOOK
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConUltSymbol* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint32_t  ID(void) const=0;
   virtual MTAPIRES  ID(const uint32_t id)=0;
   //--- id
   virtual uint64_t  Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- basis
   virtual LPCWSTR   Basis(void) const=0;
   virtual MTAPIRES  Basis(LPCWSTR basis)=0;
   //--- path
   virtual LPCWSTR   Path(void) const=0;
   virtual MTAPIRES  Path(LPCWSTR path)=0;
   //--- description
   virtual LPCWSTR   Description(void) const=0;
   virtual MTAPIRES  Description(LPCWSTR path)=0;
   //--- EnSymbolTypes
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t id)=0;
   //--- EnSymbolMode
   virtual uint32_t  Mode(void) const=0;
   virtual MTAPIRES  Mode(const uint32_t mode)=0;
   //--- EnSymbolQuotesTypes
   virtual uint32_t  QuotesType(void) const=0;
   virtual MTAPIRES  QuotesType(const uint32_t mode)=0;
   //--- digits
   virtual uint32_t  QuotesDigits(void) const=0;
   virtual MTAPIRES  QuotesDigits(const uint32_t digits)=0;
   //--- depth
   virtual uint32_t  QuotesDepth(void) const=0;
   virtual MTAPIRES  QuotesDepth(const uint32_t mode)=0;
   //--- min spread 
   virtual int32_t   QuotesSpreadMin(void) const=0;
   virtual MTAPIRES  QuotesSpreadMin(const int32_t spread)=0;
   //--- max spread 
   virtual int32_t   QuotesSpreadMax(void) const=0;
   virtual MTAPIRES  QuotesSpreadMax(const int32_t spread)=0;
   //--- tick source switch timeout
   virtual uint32_t  QuotesSwitchTimeout(void) const=0;
   virtual MTAPIRES  QuotesSwitchTimeout(const uint32_t timeout)=0;
   //--- bid markup
   virtual int32_t   QuotesMarkupBid(void) const=0;
   virtual MTAPIRES  QuotesMarkupBid(const int32_t markup)=0;
   //--- bid markup
   virtual int32_t   QuotesMarkupAsk(void) const=0;
   virtual MTAPIRES  QuotesMarkupAsk(const int32_t markup)=0;
   //--- coverage symbol
   virtual LPCWSTR   CoverageSymbol(void) const=0;
   virtual MTAPIRES  CoverageSymbol(LPCWSTR symbol)=0;
   //--- coverage group
   virtual LPCWSTR   CoverageABookGroup(void) const=0;
   virtual MTAPIRES  CoverageABookGroup(LPCWSTR group)=0;
   //--- coverage login
   virtual uint64_t  CoverageABookLogin(void) const=0;
   virtual MTAPIRES  CoverageABookLogin(const uint64_t login)=0;
   //--- coverage group
   virtual LPCWSTR   CoverageBBookGroup(void) const=0;
   virtual MTAPIRES  CoverageBBookGroup(LPCWSTR group)=0;
   //--- coverage login
   virtual uint64_t  CoverageBBookLogin(void) const=0;
   virtual MTAPIRES  CoverageBBookLogin(const uint64_t login)=0;
   //--- coverage flags EnCoverageFlags
   virtual uint32_t  CoverageFlags(void) const=0;
   virtual MTAPIRES  CoverageFlags(const uint32_t flags)=0;
   //--- EnLimitActivationMode
   virtual uint32_t  LimitActivationMode(void) const=0;
   virtual MTAPIRES  LimitActivationMode(const uint32_t mode)=0;
   //--- limit activation timeout
   virtual uint32_t  LimitActivationTimeout(void) const=0;
   virtual MTAPIRES  LimitActivationTimeout(const uint32_t timeout)=0;
   //--- slippage
   virtual uint32_t  SlippageProfit(void) const=0;
   virtual MTAPIRES  SlippageProfit(const uint32_t slippage)=0;
   //--- slippage
   virtual uint32_t  SlippageLosing(void) const=0;
   virtual MTAPIRES  SlippageLosing(const uint32_t slippage)=0;
   //--- EnBbookPriceModes
   virtual uint32_t  BBookPriceMode(void) const=0;
   virtual MTAPIRES  BBookPriceMode(const uint32_t mode)=0;
   //--- flags
   virtual uint32_t  ExecutionFlags(void) const=0;
   virtual MTAPIRES  ExecutionFlags(const uint32_t flags)=0;
   //--- EnRejectModes
   virtual uint32_t  ExecutionRejectMode(void) const=0;
   virtual MTAPIRES  ExecutionRejectMode(const uint32_t mode)=0;
   //--- quote sessions
   virtual MTAPIRES  SessionQuoteAdd(const uint32_t wday,IMTConSymbolSession* symbol)=0;
   virtual MTAPIRES  SessionQuoteUpdate(const uint32_t wday,const uint32_t pos,const IMTConSymbolSession* session)=0;
   virtual MTAPIRES  SessionQuoteDelete(const uint32_t wday,const uint32_t pos)=0;
   virtual MTAPIRES  SessionQuoteClear(const uint32_t wday)=0;
   virtual MTAPIRES  SessionQuoteShift(const uint32_t wday,const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SessionQuoteTotal(const uint32_t wday) const=0;
   virtual MTAPIRES  SessionQuoteNext(const uint32_t wday,const uint32_t pos,IMTConSymbolSession* session) const=0;
   //--- trade sessions
   virtual MTAPIRES  SessionTradeAdd(const uint32_t wday,IMTConSymbolSession* symbol)=0;
   virtual MTAPIRES  SessionTradeUpdate(const uint32_t wday,const uint32_t pos,const IMTConSymbolSession* session)=0;
   virtual MTAPIRES  SessionTradeDelete(const uint32_t wday,const uint32_t pos)=0;
   virtual MTAPIRES  SessionTradeClear(const uint32_t wday)=0;
   virtual MTAPIRES  SessionTradeShift(const uint32_t wday,const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SessionTradeTotal(const uint32_t wday) const=0;
   virtual MTAPIRES  SessionTradeNext(const uint32_t wday,const uint32_t pos,IMTConSymbolSession* session) const=0;
   //--- quote providers
   virtual MTAPIRES  ProviderQuotesAdd(IMTConUltSymbolLP* config)=0;
   virtual MTAPIRES  ProviderQuotesUpdate(const uint32_t pos,const IMTConUltSymbolLP* config)=0;
   virtual MTAPIRES  ProviderQuotesDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ProviderQuotesClear(void)=0;
   virtual MTAPIRES  ProviderQuotesShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ProviderQuotesTotal(void) const=0;
   virtual MTAPIRES  ProviderQuotesNext(const uint32_t pos,IMTConUltSymbolLP* config) const=0;
   //--- quote providers
   virtual MTAPIRES  ProviderExecutionAdd(IMTConUltSymbolLP* config)=0;
   virtual MTAPIRES  ProviderExecutionUpdate(const uint32_t pos,const IMTConUltSymbolLP* config)=0;
   virtual MTAPIRES  ProviderExecutionDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ProviderExecutionClear(void)=0;
   virtual MTAPIRES  ProviderExecutionShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ProviderExecutionTotal(void) const=0;
   virtual MTAPIRES  ProviderExecutionNext(const uint32_t pos,IMTConUltSymbolLP* config) const=0;
   //--- bands
   virtual MTAPIRES  BandAdd(IMTConUltSymbolBand* config)=0;
   virtual MTAPIRES  BandUpdate(const uint32_t pos,const IMTConUltSymbolBand* config)=0;
   virtual MTAPIRES  BandDelete(const uint32_t pos)=0;
   virtual MTAPIRES  BandClear(void)=0;
   virtual MTAPIRES  BandShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  BandTotal(void) const=0;
   virtual MTAPIRES  BandNext(const uint32_t pos,IMTConUltSymbolBand* config) const=0;
  };
//+------------------------------------------------------------------+