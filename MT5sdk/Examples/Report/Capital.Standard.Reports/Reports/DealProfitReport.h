//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DealReport.h"
#include "..\Tools\ReportVector.h"
//+------------------------------------------------------------------+
//| Deals Profit Report class                                        |
//+------------------------------------------------------------------+
class CDealProfitReport : public CDealReport
  {
private:
   //--- types
   typedef TReportVector<Section> SectionVector;      // deal hours vector type

private:
   SectionVector     m_hours;                         // deal hours vector
   //--- static data
   static MTReportInfo s_info;                        // static report info

public:
   //--- constructor/destructor
                     CDealProfitReport(void);
   virtual          ~CDealProfitReport(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- deal hour read handler
   virtual MTAPIRES  DealHourRead(const uint64_t time,const CDealCache::DealKey &key,const CDealCache::DealHour &hour,const double *rate) override;
   //--- record by time
   Section*          RecordByTime(const int64_t ctm);
   //--- prepare data
   virtual MTAPIRES  PrepareData(IMTDataset &data) override;
   //--- prepare data total
   virtual MTAPIRES  PrepareDataTotal(IMTDataset &data) override { return(MT_RET_OK_NONE); }
   //--- prepare graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type) override;
   //--- chart columns add
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const override;
  };
//+------------------------------------------------------------------+
