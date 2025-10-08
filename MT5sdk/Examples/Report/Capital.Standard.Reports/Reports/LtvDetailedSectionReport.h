//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportDictionary.h"
#include "..\Tools\ReportTop.h"
#include "..\Tools\ReportMoneySection.h"
#include "..\Cache\ClientBalanceCache.h"
//+------------------------------------------------------------------+
//| LTV by section detailed report base class                        |
//+------------------------------------------------------------------+
class CLtvDetailedSection : public CCurrencyReport, public CClientBalanceReceiver
  {
private:
   //--- result table record
   #pragma pack(push,1)
   struct TableRecord
     {
      wchar_t           section[32];               // section
      double            in_out_medium;             // deposit-withdrawal medium
      double            in_first_medium;           // first deposit medium
      int64_t           in_first_time;             // first deposit time
      uint32_t          in_first_total;            // first deposit count
      double            in_first_amount;           // first deposit amount
      uint32_t          in_total;                  // deposit count
      double            in_amount;                 // deposit amount
      double            in_medium;                 // deposit medium
      uint32_t          out_total;                 // withdrawal count
      double            out_amount;                // withdrawal amount
      double            out_medium;                // withdrawal medium
     };
   #pragma pack(pop)
   //--- section container types
   typedef ReportSection Section;                  // section type
   typedef TReportVector<Section> SectionVector;   // section vector type
   typedef TReportTop<Section> SectionTop;         // section top type

private:
   const uint32_t    m_section_field_offset;       // section field offset in cache record 
   const CMTStr128   m_section_name;               // section name
   CClientBalanceCache m_client_balance;           // client balance cache
   SectionVector     m_sections;                   // sections vector
   CReportDictionary m_names;                      // section names
   SectionTop        m_top;                        // top sections
   //--- static data
   static ReportColumn s_columns[];                // column descriptions

protected:
                     CLtvDetailedSection(const MTReportInfo &info,const uint32_t section_field_offset,LPCWSTR section_name);
   virtual          ~CLtvDetailedSection(void);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- request currency from report's configuration
   virtual MTAPIRES  Prepare(void) override;
   //--- add table columns
   MTAPIRES          PrepareTable(void);
   //--- select data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- client balance read handler
   virtual MTAPIRES  ClientBalanceRead(const ClientCache &client,const CUserBalance &balance,const double *rate) override;
   //--- calculate medium values
   void              CalculateMedium(void);
   //--- write result
   virtual MTAPIRES  WriteResult(void) override;
   //--- write section to result
   MTAPIRES          WriteSection(const uint32_t *pos);
  };
//+------------------------------------------------------------------+
