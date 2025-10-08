//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "LtvDetailedSectionReport.h"
//+------------------------------------------------------------------+
//| LTV by lead source detailed report class                         |
//+------------------------------------------------------------------+
class CLtvDetailedLeadSource : public CLtvDetailedSection
  {
private:
   static MTReportInfo s_info;                     // static report info

public:
                     CLtvDetailedLeadSource(void);
   virtual          ~CLtvDetailedLeadSource(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
  };
//+------------------------------------------------------------------+
