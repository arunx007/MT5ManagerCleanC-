//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "ExchangeMsg.h"
#include "ExchangeContextBase.h"
//+------------------------------------------------------------------+
//| Trading start and expiration date constants                      |
//+------------------------------------------------------------------+
#define SYMBOL_TIME_START                  1398902400             // 01.05.2014
#define SYMBOL_TIME_EXPIRATION             1607990400             // 15.12.2020
//+------------------------------------------------------------------+
//| Constants for tick generation                                    |
//+------------------------------------------------------------------+
#define TICK_PRICE_DELTA_MIN              -3
#define TICK_PRICE_DELTA_MAX               5
#define TICK_SPREAD                        30
//+------------------------------------------------------------------+
//| Structure of exchange symbol                                     |
//+------------------------------------------------------------------+
struct ExchangeSymbol
  {
   //--- order execution modes
   enum EnExecutionMode
     {
      EXECUTION_REQUEST       =IMTConSymbol::EXECUTION_REQUEST,   // Request Execution
      EXECUTION_INSTANT       =IMTConSymbol::EXECUTION_INSTANT,   // Instant Execution
      EXECUTION_MARKET        =IMTConSymbol::EXECUTION_MARKET,    // Market Execution
      EXECUTION_EXCHANGE      =IMTConSymbol::EXECUTION_EXCHANGE,  // Exchange Execution
      //--- enumeration borders
      EXECUTION_FIRST         =EXECUTION_REQUEST,
      EXECUTION_LAST          =EXECUTION_EXCHANGE
     };
   wchar_t           symbol[32];                                  // symbol
   wchar_t           path[128];                                   // path
   wchar_t           description[256];                            // description
   wchar_t           page[256];                                   // page
   wchar_t           currency_base[4];                            // base currency
   wchar_t           currency_profit[4];                          // profit currency
   wchar_t           currency_margin[4];                          // margin currency
   uint32_t          digits;                                      // digits
   uint64_t          tick_flags;                                  // tick flags
   uint32_t          calc_mode;                                   // calc mode
   uint32_t          exec_mode;                                   // execution mode
   uint32_t          chart_mode;                                  // chart mode
   uint32_t          fill_flags;                                  // fill flags
   uint32_t          expir_flags;                                 // expiration flags
   double            tick_value;                                  // tick value
   double            tick_size;                                   // tick size
   double            contract_size;                               // contract size
   double            volume_min;                                  // min volume
   double            volume_max;                                  // max volume
   double            volume_step;                                 // volume step
   uint32_t          market_depth;                                // market depth
   uint32_t          margin_flags;                                // margin flags
   double            margin_initial;                              // initial margin
   double            margin_maintenance;                          // maintenance margin
   double            margin_long;                                 // long margin
   double            margin_short;                                // short margin
   double            margin_limit;                                // margin limit
   double            margin_stop;                                 // stop margin
   double            margin_stop_limit;                           // stop limit margin
   double            price_limit_min;                             // price limit min
   double            price_limit_max;                             // price limit max
   uint32_t          index;                                       // symbol index
   double            settlement_price;                            // settlement price
   double            tick_spread;                                 // tick spread
   double            tick_init_price_delta_min;                   // tick price delta minimum
   double            tick_init_price_delta_max;                   // tick price delta maximum
   int64_t           time_start;                                  // trading start date
   int64_t           time_expiration;                             // trading expiration date
   uint32_t          trade_mode;                                  // trade mode
   //--- methods of filling orders
   static void       Fill(ExchangeSymbol &symbol,
                          LPCWSTR symbol_name,
                          LPCWSTR curr_base,
                          LPCWSTR curr_profit,
                          uint32_t exec_mode,
                          uint32_t digits,
                          double settlement_price);
  };
//+------------------------------------------------------------------+
//| Fill request execution symbol                                    |
//+------------------------------------------------------------------+
inline void ExchangeSymbol::Fill(ExchangeSymbol &symbol,
                                 LPCWSTR symbol_name,
                                 LPCWSTR curr_base,
                                 LPCWSTR curr_profit,
                                 uint32_t exec_mode,
                                 uint32_t digits,
                                 double settlement_price)
  {
//--- clear data
   ZeroMemory(&symbol,sizeof(ExchangeSymbol));
//--- fill data
   CMTStr::Copy(symbol.symbol,         _countof(symbol.symbol),          symbol_name);
   CMTStr::Copy(symbol.path,           _countof(symbol.path),            L"SampleExchange");
   CMTStr::FormatStr(symbol.page,      _countof(symbol.page),            L"http://www.google.com/finance?q=%s",symbol_name);
   CMTStr::Copy(symbol.currency_base,  _countof(symbol.currency_base),   curr_base);
   CMTStr::Copy(symbol.currency_profit,_countof(symbol.currency_profit), curr_profit);
   CMTStr::Copy(symbol.currency_margin,_countof(symbol.currency_margin), curr_base);
   symbol.digits                       =digits;
   symbol.tick_flags                   =IMTConSymbol::TICK_ALL;
   symbol.calc_mode                    =IMTConSymbol::TRADE_MODE_FOREX;
   symbol.chart_mode                   =IMTConSymbol::CHART_MODE_BID_PRICE;
   if(exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
     {
      symbol.calc_mode                 =IMTConSymbol::TRADE_MODE_EXCH_FUTURES;
      symbol.chart_mode                =IMTConSymbol::CHART_MODE_LAST_PRICE;
     }
   symbol.exec_mode                    =exec_mode;
   symbol.fill_flags                   =IMTConSymbol::FILL_FLAGS_ALL;
   symbol.expir_flags                  =IMTConSymbol::TIME_FLAGS_ALL;
   if(exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
      symbol.contract_size                =1;
   else
      symbol.contract_size                =100000;
   symbol.volume_min                   =1;
   symbol.volume_max                   =100;
   symbol.volume_step                  =1;
   if(exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
      symbol.market_depth              =10;
   symbol.margin_flags                 =IMTConSymbol::MARGIN_FLAGS_NONE;
   symbol.margin_initial               =0;
   if(exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
     {
      symbol.margin_initial            =0.1*settlement_price;
      symbol.price_limit_max           =settlement_price+symbol.margin_initial;
      symbol.price_limit_min           =settlement_price-symbol.margin_initial;
     }
   symbol.margin_maintenance           =0;
   symbol.margin_long                  =1;
   symbol.margin_short                 =1;
   symbol.margin_limit                 =0;
   if(exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
      symbol.margin_limit              =1;
   symbol.margin_stop                  =0;
   symbol.margin_stop_limit            =0;
   symbol.index                        =0;
   symbol.settlement_price             =settlement_price;
   symbol.tick_spread                  =TICK_SPREAD/SMTMath::DecPow(digits);
   symbol.tick_init_price_delta_min    =TICK_PRICE_DELTA_MIN/SMTMath::DecPow(digits);
   symbol.tick_init_price_delta_max    =TICK_PRICE_DELTA_MAX/SMTMath::DecPow(digits);
   symbol.tick_spread                  =SMTMath::PriceNormalize(symbol.tick_spread,digits);
   symbol.tick_init_price_delta_min    =SMTMath::PriceNormalize(symbol.tick_init_price_delta_min,digits);
   symbol.tick_init_price_delta_max    =SMTMath::PriceNormalize(symbol.tick_init_price_delta_max,digits);
   if(exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
     {
      symbol.time_start                =SYMBOL_TIME_START;
      symbol.time_expiration           =SYMBOL_TIME_EXPIRATION;
      symbol.tick_size                 =0.001;
      symbol.tick_value                =0.01;
     }
   symbol.trade_mode                   =IMTConSymbol::TRADE_FULL;
  }
//+------------------------------------------------------------------+
//| Array of symbols                                                 |
//+------------------------------------------------------------------+
typedef TMTArray<ExchangeSymbol> ExchangeSymbolsArray;
//+------------------------------------------------------------------+
//| Symbol message                                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgSymbol
  {
   //--- message header
   ExchangeMsgHeader header;
   //--- symbol data
   ExchangeSymbol    symbol;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgSymbol &msg);
   static bool       Write(const ExchangeMsgSymbol &msg,CExchangeContextBase &context);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgSymbol::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgSymbol &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get header
   if(!ExchangeMsgHeader::Read(buffer,buffer_size,msg.header))
      return(false);
//--- get symbol index
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_INDEX,msg.symbol.index)
//--- get symbol name
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_SYMBOL,msg.symbol.symbol)
//--- get symbol path
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_PATH,msg.symbol.path)
//--- get description
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_DESCRIPTION,msg.symbol.description)
//--- get page
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_PAGE,msg.symbol.page)
//--- get base currency
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_CURRENCY_BASE,msg.symbol.currency_base)
//--- get profit currency
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_CURRENCY_PROFIT,msg.symbol.currency_profit)
//--- get margin currency
   READ_MSG_TAG_STR(MSG_TAG_SYMBOL_CURRENCY_MARGIN,msg.symbol.currency_margin)
//--- get digits
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_DIGITS,msg.symbol.digits)
//--- get tick flags
   READ_MSG_TAG_UINT64(MSG_TAG_SYMBOL_TICK_FLAGS,msg.symbol.tick_flags)
//--- get calc mode
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_CALC_MODE,msg.symbol.calc_mode)
//--- get execution mode
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_EXEC_MODE,msg.symbol.exec_mode)
//--- get chart mode
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_CHART_MODE,msg.symbol.chart_mode)
//--- get fill flags
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_FILL_FLAGS,msg.symbol.fill_flags)
//--- get expiration flags
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_EXPIR_FLAGS,msg.symbol.expir_flags)
//--- get tick value
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_TICK_VALUE,msg.symbol.tick_value)
//--- get tick size
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_TICK_SIZE,msg.symbol.tick_size)
//--- get contract size
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_CONTRACT_SIZE,msg.symbol.contract_size)
//--- get min volume
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_VOLUME_MIN,msg.symbol.volume_min)
//--- get max volume
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_VOLUME_MAX,msg.symbol.volume_max)
//--- get volume step
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_VOLUME_STEP,msg.symbol.volume_step)
//--- get market depth
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_MARKET_DEPTH,msg.symbol.market_depth)
//--- get margin flags
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_MARGIN_FLAGS,msg.symbol.margin_flags)
//--- get initial margin
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_INITIAL,msg.symbol.margin_initial)
//--- get maintenance margin
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_MAINTENANCE,msg.symbol.margin_maintenance)
//--- get long margin
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_LONG,msg.symbol.margin_long)
//--- get short margin
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_SHORT,msg.symbol.margin_short)
//--- get margin limit
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_LIMIT,msg.symbol.margin_limit)
//--- get stop margin
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_STOP,msg.symbol.margin_stop)
//--- get stop limit margin
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_STOP_LIMIT,msg.symbol.margin_stop_limit)
//--- get settlement price
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_SETTLEMENT_PRICE,msg.symbol.settlement_price)
//--- get price limit max
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_PRICE_LIMIT_MAX,msg.symbol.price_limit_max)
//--- get price limit min
   READ_MSG_TAG_DBL(MSG_TAG_SYMBOL_PRICE_LIMIT_MIN,msg.symbol.price_limit_min)
//--- get trading start date
   READ_MSG_TAG_INT64(MSG_TAG_SYMBOL_TIME_START,msg.symbol.time_start)
//--- get trading expiration date
   READ_MSG_TAG_INT64(MSG_TAG_SYMBOL_TIME_EXPIRATION,msg.symbol.time_expiration)
//--- get trade mode
   READ_MSG_TAG_UINT(MSG_TAG_SYMBOL_TRADE_MODE,msg.symbol.trade_mode)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgSymbol::Write(const ExchangeMsgSymbol &msg,CExchangeContextBase &context)
  {
//--- message buffer
   char  buffer[MSG_MAX_SIZE]={0};
   char *buffer_ptr          =buffer;
   int32_t   buffer_size         =_countof(buffer),len=0;
//--- write header
   if(!ExchangeMsgHeader::Write(msg.header,buffer_ptr,buffer_size,len))
      return(false);
//--- write symbol index
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_INDEX,msg.symbol.index)
//--- write symbol name
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_SYMBOL,msg.symbol.symbol)
//--- write symbol path
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_PATH,msg.symbol.path)
//--- write description
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_DESCRIPTION,msg.symbol.description)
//--- write page
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_PAGE,msg.symbol.page)
//--- write base currency
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_CURRENCY_BASE,msg.symbol.currency_base)
//--- write profit currency
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_CURRENCY_PROFIT,msg.symbol.currency_profit)
//--- write margin currency
   WRITE_MSG_TAG_STR(MSG_TAG_SYMBOL_CURRENCY_MARGIN,msg.symbol.currency_margin)
//--- write digits
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_DIGITS,msg.symbol.digits)
//--- write tick flags
   WRITE_MSG_TAG_UINT64(MSG_TAG_SYMBOL_TICK_FLAGS,msg.symbol.tick_flags)
//--- write calc mode
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_CALC_MODE,msg.symbol.calc_mode)
//--- write execution mode
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_EXEC_MODE,msg.symbol.exec_mode)
//--- write chart mode
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_CHART_MODE,msg.symbol.chart_mode)
//--- write fill flags
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_FILL_FLAGS,msg.symbol.fill_flags)
//--- write expiration flags
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_EXPIR_FLAGS,msg.symbol.expir_flags)
//--- write tick value
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_TICK_VALUE,msg.symbol.tick_value)
//--- write tick size
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_TICK_SIZE,msg.symbol.tick_size)
//--- write contract size
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_CONTRACT_SIZE,msg.symbol.contract_size)
//--- write min volume
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_VOLUME_MIN,msg.symbol.volume_min)
//--- write max volume
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_VOLUME_MAX,msg.symbol.volume_max)
//--- write volume step
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_VOLUME_STEP,msg.symbol.volume_step)
//--- write market depth
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_MARKET_DEPTH,msg.symbol.market_depth)
//--- write margin flags
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_MARGIN_FLAGS,msg.symbol.margin_flags)
//--- write initial margin
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_INITIAL,msg.symbol.margin_initial)
//--- write maintenance margin
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_MAINTENANCE,msg.symbol.margin_maintenance)
//--- write long margin
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_LONG,msg.symbol.margin_long)
//--- write short margin
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_SHORT,msg.symbol.margin_short)
//--- write margin limit
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_LIMIT,msg.symbol.margin_limit)
//--- write stop margin
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_STOP,msg.symbol.margin_stop)
//--- write stop limit margin
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_MARGIN_STOP_LIMIT,msg.symbol.margin_stop_limit)
//--- write settlement price
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_SETTLEMENT_PRICE,msg.symbol.settlement_price)
//--- write price limit max
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_PRICE_LIMIT_MAX,msg.symbol.price_limit_max)
//--- write price limit min
   WRITE_MSG_TAG_DBL(MSG_TAG_SYMBOL_PRICE_LIMIT_MIN,msg.symbol.price_limit_min)
//--- write trading start date
   WRITE_MSG_TAG_INT64(MSG_TAG_SYMBOL_TIME_START,msg.symbol.time_start)
//--- write trading expirarion date
   WRITE_MSG_TAG_INT64(MSG_TAG_SYMBOL_TIME_EXPIRATION,msg.symbol.time_expiration)
//--- write trade mode
   WRITE_MSG_TAG_UINT(MSG_TAG_SYMBOL_TRADE_MODE,msg.symbol.trade_mode)
//--- write character of message end
   WRITE_MSG_SEPARATOR()
//--- calculate message size
   uint32_t msg_buffer_size=_countof(buffer)-buffer_size;
//--- send
   return(context.Send(buffer,msg_buffer_size));
  }
//+------------------------------------------------------------------+
