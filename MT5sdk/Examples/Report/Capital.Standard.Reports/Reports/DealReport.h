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
#include "..\Cache\UserCache.h"
#include "..\Cache\DealCache.h"
//+------------------------------------------------------------------+
//| Deal Report base class                                           |
//+------------------------------------------------------------------+
class CDealReport : public CCurrencyReport
  {
private:
   //--- constants
   enum EnConstants
     {
      DAYS_UNTIL_DAY    =8,                           // range between 0..DAYS_UNTIL_DAY will be drawn by hour
      DAYS_UNTIL_WEEK   =62,                          // range between DAYS_UNTIL_DAY..DAYS_UNTIL_WEEK will be drawn day by day
      DAYS_UNTIL_MONTH  =124,                         // range between DAYS_UNTIL_WEEK..DAYS_UNTIL_MONTH will be drawn week by week. After that - month by month
     };

protected:
   //--- dataset column ids
   enum EnColumnId
     {
      COLUMN_DATE       =1,                           // date
      COLUMN_SECTION    =1,                           // section name
      COLUMN_VOLUME     =2,                           // total volume
      COLUMN_IN_VOLUME  =3,                           // in volume
      COLUMN_OUT_VOLUME =4,                           // out volume
      COLUMN_IN_AMOUNT  =5,                           // in total amount
      COLUMN_OUT_AMOUNT =6,                           // out total amount
      COLUMN_IN_COUNT   =7,                           // in deals count
      COLUMN_OUT_COUNT  =8,                           // out deals count
      COLUMN_IN_MEDIUM  =9,                           // in average volume
      COLUMN_OUT_MEDIUM =10,                          // out average volume
      COLUMN_IN_OUT     =11,                          // in-out delta
      COLUMN_IN_OUT_ACC =12,                          // in-out delta accumulated
     };

protected:
   //--- dataset record
   #pragma pack(push,1)
   struct Record
     {
      double            volume;                       // total volume
      double            in_volume;                    // in volume
      double            out_volume;                   // out volume
      double            in_amount;                    // in total amount
      uint64_t          in_count;                     // in deals count
      double            in_medium;                    // in average
      double            out_amount;                   // out total amount
      uint64_t          out_count;                    // out deals count
      double            out_medium;                   // out average
      double            in_out_delta;                 // in-out delta
      double            in_out_delta_accum;           // in-out delta accumulated
     };
   //--- dataset row
   struct Row
     {
      int64_t           date;                         // date
      Record            sections[1];                  // sections
      //--- sections array offset
      static uint32_t   Sections(void) { return(offsetof(Row,sections)); }
     };
   //--- total dataset row
   struct RowTotal
     {
      wchar_t           section[32];                  // section name
      double            volume;                       // total volume
      double            in_volume;                    // in volume
      double            out_volume;                   // out volume
     };
   #pragma pack(pop)
   //--- types
   typedef ReportSection Section;                     // section type

protected:
   const bool        m_line_chart;                    // line charts
   CReportInterval   m_interval;                      // report interval object
   CUserCache       *m_users;                         // user cache
   CDealCache       *m_deals;                         // deal cache
   //--- static data
   static ReportColumn s_columns[12];                 // column descriptions
   static ReportColumn s_columns_total[4];            // column total descriptions

public:
   //--- deal read handler
   MTAPIRES          DealRead(const uint64_t key,const CDealCache::DealHour &hour);

protected:
                     CDealReport(const MTReportInfo &info,const bool line_chart);
   virtual          ~CDealReport(void);
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- request data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- calculate deals
   MTAPIRES          CalculateDeals(void);
   //--- deal hour read handler
   virtual MTAPIRES  DealHourRead(const uint64_t time,const CDealCache::DealKey &key,const CDealCache::DealHour &hour,const double *rate)=0;
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
   //--- prepare total graphs
   MTAPIRES          PrepareGraphsTotal(void);
   //--- prepare data
   virtual MTAPIRES  PrepareData(IMTDataset &data)=0;
   //--- prepare data total
   virtual MTAPIRES  PrepareDataTotal(IMTDataset &data)=0;
   //--- fill record
   void              FillRecord(Record &record,const Section &section,double *in_out_delta_accum) const;
   //--- fill row total
   void              FillRowTotal(RowTotal &row,const Section &section) const;
   //--- prepare graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type);
   //--- chart columns add overridable
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const=0;
   //--- add deal hour to report section
   void              DealAdd(Section &section,const uint64_t time,const CDealCache::DealHour &hour,const double *rate);
  };
//+------------------------------------------------------------------+
