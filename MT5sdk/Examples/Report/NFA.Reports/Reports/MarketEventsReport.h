//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportContext.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| MarketEvents NFA Report                                          |
//+------------------------------------------------------------------+
class CMarketEventsReport : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_FDM_ID                =1,
      COLUMN_BATCH_DATE            =2,
      COLUMN_MARKET_EVENT_TIME     =3,
      COLUMN_PRODUCT_CODE          =4,
      COLUMN_PRODUCT_STATE         =5,
      COLUMN_EVENT_TEXT            =6,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      wchar_t           fdm_id[8];
      int64_t           batch_date;
      int64_t           market_event_time;
      wchar_t           product_code[8];
      wchar_t           product_state[24];
      wchar_t           event_text[2000];
     };
   #pragma pack(pop)

private:
   static MTReportInfo s_info;            // report information     
   static ReportColumn s_columns[];       // column descriptions

public:
   //---
                     CMarketEventsReport(void);
   virtual          ~CMarketEventsReport(void);
   //--- IMTReportContext implementation
   //--- get report information structure
   static MTAPIRES   Info(MTReportInfo &info);
   //--- release instance of report class
   virtual void      Release(void);
   //--- generate report
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   static bool       Fill(CTextReader &reader,TableRecord &record,const ReportContext &context);
  };
//+------------------------------------------------------------------+
