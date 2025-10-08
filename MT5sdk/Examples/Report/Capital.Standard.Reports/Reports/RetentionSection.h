//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportMatrix.h"
#include "..\Tools\ReportTop.h"
#include "..\Tools\ReportDictionary.h"
#include "..\Tools\ReportInterval.h"
#include "..\Tools\CurrencyConverter.h"
#include "..\Cache\ReportCache.h"
//+------------------------------------------------------------------+
//| Retention by section Report part class                           |
//+------------------------------------------------------------------+
class CRetentionSection
  {
private:
   enum constants
     {
      STEP_MAX     =28,                               // time step count
      STEP_DURATION=SECONDS_IN_DAY                    // time step duration
     };

private:
   #pragma pack(push,1)
   //--- Retention dataset record
   struct RetentionRecord
     {
      wchar_t           day[32];                     // week
      double            sections[1];                  // retention by sections
     };
   #pragma pack(pop)
   //--- Uint structure add operation
   class CUIntOperationAdd
     {
   public:
      void              operator()(uint32_t &l,const uint32_t r) const { l+=r; }
     };
   //--- Retention container types
   typedef TReportMatrix<uint32_t> RetentionMatrix;       // retention matrix type
   typedef RetentionMatrix::TVector RetentionVector;  // retention vector type
   typedef TReportTop<uint32_t> RetentionTop;             // retention top type

private:
   RetentionMatrix   m_retention;                     // retention matrix
   RetentionTop      m_top;                           // top section indexes
   CReportDictionary m_names;                         // section names
   //--- static data
   static ReportColumn s_columns_retention[];         // column descriptions retention

public:
   //--- constructor/destructor
                     CRetentionSection(void);
                    ~CRetentionSection(void);
   //--- clear
   void              Clear(void);
   //--- Initialize
   MTAPIRES          Initialize(void);
   //--- add lifetime
   MTAPIRES          LifetimeAdd(const int64_t lifetime,const uint32_t section);
   //--- calculate total
   MTAPIRES          CalculateTotal(CReportParameter &params);
   //--- calculate period sections
   MTAPIRES          CalculatePeriod(const CReportInterval &interval);
   //--- read top sections names
   MTAPIRES          ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id);
   //--- prepare all graphs to be shown
   MTAPIRES          PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type,LPCWSTR description);

private:
   //--- retention by time and section
   uint32_t*         RetentionByTimeAndSection(const int64_t lifetime,const uint32_t section);
   //--- fill period top names
   static MTAPIRES   FillPeriodTopNames(const CReportInterval &interval,const CReportTopBase &top,CReportDictionary &names);
   //--- prepare retention data
   MTAPIRES          PrepareData(IMTDataset &data) const;
   //--- prepare retention data columns
   MTAPIRES          PrepareDataColumns(IMTDataset &data) const;
   //--- fill record
   bool              FillRecord(RetentionRecord &row,const RetentionVector &sections,const RetentionVector &first,const uint32_t other_first) const;
   //--- prepare graph
   MTAPIRES          PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,LPCWSTR description,const uint32_t type,const uint32_t column_id);
   //--- sort retention descending
   static int32_t    SortRetentionDesc(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
