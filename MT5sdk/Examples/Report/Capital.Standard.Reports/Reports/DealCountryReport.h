//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DealSectionReport.h"
//+------------------------------------------------------------------+
//| Deal by Country report                                           |
//+------------------------------------------------------------------+
class CDealCountryReport : public CDealSectionReport
  {
private:
   static MTReportInfo s_info;                        // static report info

public:
                     CDealCountryReport(void);
   virtual          ~CDealCountryReport(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- factory method
   static IMTReportContext* Create(void);

private:
   //--- prepare graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type) override;
  };
//+------------------------------------------------------------------+
