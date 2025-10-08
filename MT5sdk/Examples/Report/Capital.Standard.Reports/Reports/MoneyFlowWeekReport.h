//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MoneyFlowDailyReport.h"
#include "..\Cache\UserGroupCache.h"
#include "..\Cache\DealWeekCache.h"
#include "..\Cache\UserGroupFilter.h"
#include "..\Tools\ReportVector.h"
//+------------------------------------------------------------------+
//| Money Flow Daily by users Report class                           |
//+------------------------------------------------------------------+
class CMoneyFlowWeek : public CMoneyFlowDaily
  {
private:
   //--- types
   typedef TReportVector<Section> SectionVector;      // money vector type

private:
   CUserGroupCache  *m_users;                         // user group cache
   CDealWeekCache   *m_deals;                         // deal week cache
   CUserGroupFilter  m_groups;                        // group filter
   SectionVector     m_dates;                         // money vector
   //--- static data
   static MTReportInfo s_info;                        // static report info

public:
   //--- constructor/destructor
                     CMoneyFlowWeek(void);
   virtual          ~CMoneyFlowWeek(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
   //--- deal read handler
   MTAPIRES          DealRead(const uint64_t key,const CDealWeekCache::DealWeek &week);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- request data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- calculate deals
   MTAPIRES          CalculateDeals(void);
   //--- deal week read handler
   MTAPIRES          DealWeekRead(const uint64_t time,const CDealWeekCache::DealKey &key,const CDealWeekCache::DealWeek &week,const double *rate);
   //--- record by time
   Section*          RecordByTime(const int64_t ctm);
   //--- add deal week to report section
   void              DealAdd(Section &section,const uint64_t time,const CDealWeekCache::DealWeek &week,const double *rate);
   //--- prepare money data
   virtual MTAPIRES  PrepareData(IMTDataset &data) override;
   //--- prepare money money graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column) override;
   //--- chart columns add
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const override;
  };
//+------------------------------------------------------------------+
