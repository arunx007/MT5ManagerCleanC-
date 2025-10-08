//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MoneyFlowDailySectionReport.h"
//+------------------------------------------------------------------+
//| Money Flow Daily by Lead Source report                           |
//+------------------------------------------------------------------+
class CMoneyFlowDailyLeadSource : public CMoneyFlowDailySection
  {
private:
   static MTReportInfo s_info;                        // static report info

public:
                     CMoneyFlowDailyLeadSource(void);
   virtual          ~CMoneyFlowDailyLeadSource(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
  };
//+------------------------------------------------------------------+
