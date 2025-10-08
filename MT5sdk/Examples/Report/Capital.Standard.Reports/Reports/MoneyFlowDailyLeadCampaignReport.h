//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MoneyFlowDailySectionReport.h"
//+------------------------------------------------------------------+
//| Money Flow Daily by Lead Campaign report                         |
//+------------------------------------------------------------------+
class CMoneyFlowDailyLeadCampaign : public CMoneyFlowDailySection
  {
private:
   static MTReportInfo s_info;                        // static report info

public:
                     CMoneyFlowDailyLeadCampaign(void);
   virtual          ~CMoneyFlowDailyLeadCampaign(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
  };
//+------------------------------------------------------------------+
