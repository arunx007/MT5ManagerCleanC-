//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportInterval.h"
#include "..\Tools\ReportMoneySection.h"
//+------------------------------------------------------------------+
//| Money Flow Daily Report base class                               |
//+------------------------------------------------------------------+
class CMoneyFlowDaily : public CCurrencyReport
  {
private:
   //--- constants
   enum EnConstants
     {
      SMOOTH_RANGE_ONE     =62,                       // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =124,                      // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };

protected:
   //--- Money dataset record
   #pragma pack(push,1)
   struct MoneyRecord
     {
      double            in_amount;                    // in total amount
      uint64_t          in_count;                     // in deals count
      double            in_medium;                    // in average
      double            out_amount;                   // out total amount
      uint64_t          out_count;                    // out deals count
      double            out_medium;                   // out average
      double            in_out_delta;                 // in-out delta
      double            in_out_delta_accum;           // in-out delta accumulated
     };
   //--- Money dataset row
   struct MoneyRow
     {
      int64_t           date;                         // date
      MoneyRecord       sections[1];                  // money by sections
     };
   #pragma pack(pop)
   //--- types
   typedef ReportSection Section;                     // section type

protected:
   const bool        m_line_chart;                    // line charts
   CReportInterval   m_interval;                      // report interval object
   //--- static data
   static ReportColumn s_columns_money[9];            // column descriptions money

protected:
                     CMoneyFlowDaily(const MTReportInfo &info,bool line_chart,uint32_t days_week=SMOOTH_RANGE_ONE,uint32_t days_months=SMOOTH_RANGE_TWO);
   virtual          ~CMoneyFlowDaily(void);
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
   //--- prepare money data
   virtual MTAPIRES  PrepareData(IMTDataset &data)=0;
   //--- fill record
   void              FillRecord(MoneyRecord &record,const Section &section,double *in_out_delta_accum) const;
   //--- prepare money money graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column);
   //--- chart columns add overridable
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const=0;
  };
//+------------------------------------------------------------------+
