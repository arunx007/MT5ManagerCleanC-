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
#include "..\Tools\ReportMatrix.h"
#include "..\Tools\ReportTop.h"
#include "..\Tools\ReportDictionary.h"
#include "..\Cache\UserGroupCache.h"
#include "..\Cache\DealWeekCache.h"
#include "..\Cache\UserGroupFilter.h"
//+------------------------------------------------------------------+
//| Deal weekly report class                                         |
//+------------------------------------------------------------------+
class CDealWeekReport : public CCurrencyReport
  {
protected:
   //--- dataset column ids
   enum EnColumnId
     {
      COLUMN_DATE       =1,                           // date
      COLUMN_GROUP      =1,                           // group name
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
      wchar_t           group[64];                    // group name
      double            volume;                       // total volume
      double            in_volume;                    // in volume
      double            out_volume;                   // out volume
     };
   #pragma pack(pop)
   //--- types
   typedef ReportSection Section;                     // section type
   typedef TReportMatrix<Section> SectionMatrix;      // section matrix type
   typedef SectionMatrix::TVector SectionVector;      // section vector type
   typedef TReportTop<Section> SectionTop;            // section top type
   typedef TReportVector<double> DoubleVector;        // double vector type

protected:
   CReportInterval   m_interval;                      // report interval object
   CUserGroupCache  *m_users;                         // user group cache
   CDealWeekCache   *m_deals;                         // deals week aggregates
   CUserGroupFilter  m_groups;                        // group filter
   SectionMatrix     m_table;                         // section matrix
   SectionVector     m_total;                         // section vector for total report period
   SectionTop        m_top;                           // top section indexes
   SectionTop        m_top_total;                     // top section indexes for total report period
   //--- static data
   static MTReportInfo s_info;                        // report info
   static ReportColumn s_columns[12];                 // column descriptions
   static ReportColumn s_columns_total[4];            // column total descriptions

public:
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- factory method
   static IMTReportContext* Create(void);
   //--- deal read handler
   MTAPIRES          DealRead(const uint64_t key,const CDealWeekCache::DealWeek &week);

protected:
                     CDealWeekReport(void);
   virtual          ~CDealWeekReport(void);
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
   //--- record by time and section
   Section*          RecordByTimeAndSection(const int64_t ctm,const uint32_t section);
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
   //--- prepare total graphs
   MTAPIRES          PrepareGraphsTotal(void);
   //--- prepare data
   MTAPIRES          PrepareData(IMTDataset &data);
   //--- prepare data total
   MTAPIRES          PrepareDataTotal(IMTDataset &data);
   //--- fill top names
   MTAPIRES          FillTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names);
   //--- fill row
   void              FillRow(Row &row,const SectionVector &sections,DoubleVector &accumulated) const;
   //--- fill record
   void              FillRecord(Record &record,const Section &section,double *in_out_delta_accum) const;
   //--- fill row total
   bool              FillRowTotal(RowTotal &row,const Section &section) const;
   //--- prepare graph
   MTAPIRES          PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type);
   //--- prepare dataset columns
   MTAPIRES          PrepareDataColumns(IMTDataset &data);
   //--- chart columns add overridable
   MTAPIRES          ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const;
   //--- add deal week to report section
   void              DealAdd(Section &section,const uint64_t time,const CDealWeekCache::DealWeek &week,const double *rate);
  };
//+------------------------------------------------------------------+
