//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DealReport.h"
#include "..\Tools\ReportMatrix.h"
#include "..\Tools\ReportTop.h"
#include "..\Tools\ReportDictionary.h"
//+------------------------------------------------------------------+
//| Deal by Section report base class                                |
//+------------------------------------------------------------------+
class CDealSectionReport : public CDealReport
  {
private:
   //--- section container types
   typedef TReportMatrix<Section> SectionMatrix;      // section matrix type
   typedef SectionMatrix::TVector SectionVector;      // section vector type
   typedef TReportTop<Section> SectionTop;            // section top type
   typedef TReportVector<double> DoubleVector;        // double vector type

private:
   const CDealCache::EnDealKeyFields m_section_field; // section field in cache
   SectionMatrix     m_table;                         // section matrix
   SectionVector     m_total;                         // section vector for total report period
   SectionTop        m_top;                           // top section indexes
   SectionTop        m_top_total;                     // top section indexes for total report period

protected:
                     CDealSectionReport(const MTReportInfo &info,const CDealCache::EnDealKeyFields section_field);
   virtual          ~CDealSectionReport(void);
   //--- fill top names
   virtual MTAPIRES  FillTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names);

protected:
   //--- prepare graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type) override;

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- request data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- deal hour read handler
   virtual MTAPIRES  DealHourRead(const uint64_t time,const CDealCache::DealKey &key,const CDealCache::DealHour &hour,const double *rate) override;
   //--- record by time and section
   Section*          RecordByTimeAndSection(const int64_t ctm,const uint32_t section);
   //--- prepare data
   virtual MTAPIRES  PrepareData(IMTDataset &data) override;
   //--- prepare data total
   virtual MTAPIRES  PrepareDataTotal(IMTDataset &data) override;
   //--- prepare dataset columns
   MTAPIRES          PrepareDataColumns(IMTDataset &data);
   //--- fill row
   void              FillRow(Row &row,const SectionVector &sections,DoubleVector &accumulated) const;
   //--- chart columns add
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const override;
  };
//+------------------------------------------------------------------+
