//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| History split rounding methods                                   |
//+------------------------------------------------------------------+
enum EnSplitRounding
  {
   SPLIT_ROUNDING_STANDARD=0,  // standard mathematical rounding
   SPLIT_ROUNDING_DOWN    =1,  // round half up
   SPLIT_ROUNDING_UP      =2,  // round half down
//--- enumeration borders
   SPLIT_ROUNDING_FIRST   =SPLIT_ROUNDING_STANDARD,
   SPLIT_ROUNDING_LAST    =SPLIT_ROUNDING_UP
  };
//+------------------------------------------------------------------+
//| History bar description                                          |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTChartBar
  {
   int64_t           datetime;          // datetime
   //--- prices
   double            open;              // open price
   double            high;              // high price
   double            low;               // low price
   double            close;             // close price
   //--- volumes
   uint64_t          tick_volume;       // tick volume
   int32_t           spread;            // spread
   uint64_t          volume;            // volume
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Chart events notification interface                              |
//+------------------------------------------------------------------+
class IMTChartSink
  {
public:
   //--- split
   virtual void      OnChartSplit(LPCWSTR symbol,const double new_shares,const double old_shares,const uint32_t rounding_rule,const int64_t datetime_from,const int64_t datetime_to)   {}
   virtual MTAPIRES  HookChartSplit(LPCWSTR symbol,const double new_shares,const double old_shares,const uint32_t rounding_rule,const int64_t datetime_from,const int64_t datetime_to) { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+

