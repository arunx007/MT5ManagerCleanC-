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
//| LTV by section Report part class                                 |
//+------------------------------------------------------------------+
class CLtvSection
  {
private:
   #pragma pack(push,1)
   //--- LTV section record
   struct LtvSectionRecord
     {
      double            value;                     // ltv
      double            amount;                    // amount
      uint64_t          count;                     // count
     };
   //--- LTV dataset record
   struct LtvRecord
     {
      wchar_t           month[32];                 // month
      LtvSectionRecord  sections[1];               // LTV by sections
     };
   #pragma pack(pop)
   //--- LTV structure
   struct Ltv
     {
      double            amount;                    // amount
      uint64_t          count;                     // count
     };
   //--- LTV structure add operation
   class CLtvOperationAdd
     {
   private:
      const CCurrencyConverter &m_currency;        // currency converter reference

   public:
                        CLtvOperationAdd(const CCurrencyConverter &currency) : m_currency(currency) {}
      void              operator()(Ltv &l,const Ltv &r) const;
     };
   //--- LTV container types
   typedef TReportMatrix<Ltv> LtvMatrix;           // LTV matrix type
   typedef LtvMatrix::TVector LtvVector;           // LTV vector type
   typedef TReportTop<Ltv> LtvTop;                 // LTV top type
   typedef TReportVector<double> DoubleVector;     // double vector type

private:
   CCurrencyConverter &m_currency;                 // currency converter reference
   LtvMatrix         m_ltv;                        // LTV matrix
   LtvVector         m_total;                      // LTV total vector
   LtvTop            m_top;                        // top section indexes
   CReportDictionary m_names;                      // section names
   //--- static data
   static ReportColumn s_columns_ltv[];            // column descriptions LTV

public:
   //--- constructor/destructor
   explicit          CLtvSection(CCurrencyConverter &currency);
                    ~CLtvSection(void);
   //--- clear
   void              Clear(void);
   //--- Initialize
   MTAPIRES          Initialize(IMTReportAPI &api);
   //--- add deposit
   MTAPIRES          DepositAdd(const int64_t registration,const int64_t time,const uint32_t section,const double value,const uint32_t count);
   //--- calculate total
   MTAPIRES          CalculateTotal(CReportParameter &params);
   //--- calculate period sections
   MTAPIRES          CalculatePeriod(const CReportInterval &interval);
   //--- read top sections names
   MTAPIRES          ReadTopNames(const CReportCache &cache,const uint32_t dictionary_id);
   //--- prepare all graphs to be shown
   MTAPIRES          PrepareGraphs(IMTReportAPI &api,LPCWSTR section_type);

private:
   //--- Ltv by time and section
   Ltv*              LtvByTimeAndSection(const int64_t registration,const int64_t ctm,const uint32_t section);
   //--- fill period top names
   static MTAPIRES   FillPeriodTopNames(const CReportInterval &interval,const CReportTopBase &top,CReportDictionary &names);
   //--- prepare LTV data
   MTAPIRES          PrepareData(IMTDataset &data,int64_t limit) const;
   //--- prepare LTV data columns
   MTAPIRES          PrepareDataColumns(IMTDataset &data) const;
   //--- fill record
   bool              FillRecord(LtvRecord &row,const LtvVector &sections,DoubleVector &accumulated,const int64_t limit) const;
   //--- fill record section
   void              FillRecordSection(LtvSectionRecord &record,const Ltv *ltv,double &accum) const;
   //--- prepare graph
   MTAPIRES          PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t type,const uint32_t column_id);
   //--- format ordinal number
   static const CMTStr& FormatOrdinalNumber(CMTStr &str,const uint32_t number);
   //--- sort Ltv amount descending
   static int32_t    SortLtvAmountDesc(const void *left,const void *right);
   //--- sort Ltv descending
   static int32_t    SortLtvDesc(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
