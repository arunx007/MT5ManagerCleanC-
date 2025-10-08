//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MoneyFlowDailyReport.h"
#include "..\Tools\ReportMatrix.h"
#include "..\Cache\ClientBalanceCache.h"
//+------------------------------------------------------------------+
//| Money Flow Daily by Section report base class                    |
//+------------------------------------------------------------------+
class CMoneyFlowDailySection : public CMoneyFlowDaily, public CClientBalanceReceiver
  {
private:
   //--- Money container types
   typedef TReportMatrix<Section> SectionMatrix;      // money matrix type
   typedef SectionMatrix::TVector SectionVector;      // money vector type
   typedef TReportTop<Section> SectionTop;            // money top type
   typedef TReportVector<double> DoubleVector;        // double vector type

private:
   const uint32_t    m_section_field_offset;          // section field offset in cache record 
   CClientBalanceCache m_client_balance;              // client balance cache
   SectionMatrix     m_money;                         // money matrix
   SectionVector     m_total;                         // money vector for total report period
   SectionTop        m_top;                           // top section indexes

protected:
                     CMoneyFlowDailySection(const MTReportInfo &info,const uint32_t section_field_offset);
   virtual          ~CMoneyFlowDailySection(void);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- request data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- client balance read handler
   virtual MTAPIRES  ClientBalanceRead(const ClientCache &client,const CUserBalance &balance,const double *rate) override;
   //--- record by time and section
   Section*          RecordByTimeAndSection(const int64_t ctm,const uint32_t section);
   //--- prepare money data
   virtual MTAPIRES  PrepareData(IMTDataset &data) override;
   //--- prepare LTV data columns
   MTAPIRES          PrepareDataColumns(IMTDataset &data);
   //--- fill row
   void              FillRow(MoneyRow &row,const SectionVector &sections,DoubleVector &accumulated) const;
   //--- chart columns add
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const override;
  };
//+------------------------------------------------------------------+
