//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "LtvDetailedSectionReport.h"
//+------------------------------------------------------------------+
//| LTV by country detailed report class                             |
//+------------------------------------------------------------------+
class CLtvDetailedCountry : public CLtvDetailedSection
  {
private:
   static MTReportInfo s_info;                     // static report info

public:
                     CLtvDetailedCountry(void);
   virtual          ~CLtvDetailedCountry(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
  };
//+------------------------------------------------------------------+
