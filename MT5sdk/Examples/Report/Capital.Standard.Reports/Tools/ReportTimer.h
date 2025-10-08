//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Report timer helper class                                        |
//+------------------------------------------------------------------+
class CReportTimer final
  {
private:
   uint64_t          m_start;    // start time in ms

public:
                     CReportTimer(void) : m_start{::GetTickCount64()} {}
   //--- elapsed time in ms
   uint64_t          Elapsed(void) const { return(::GetTickCount64()-m_start); }
  };
//+------------------------------------------------------------------+
