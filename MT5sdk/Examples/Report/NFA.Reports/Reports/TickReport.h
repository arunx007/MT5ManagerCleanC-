//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportContext.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| Tick NFA Report                                                  |
//+------------------------------------------------------------------+
class CTickReport : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_FDM_ID                =1,
      COLUMN_BATCH_DATE            =2,
      COLUMN_PRODUCT_CODE          =3,
      COLUMN_QUOTE_DATETIME        =4,
      COLUMN_BID_PRICE             =5,
      COLUMN_ASK_PRICE             =6,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      wchar_t           fdm_id[8];
      int64_t           batch_date;
      wchar_t           product_code[8];
      int64_t           quote_datetime;
      double            bid_price;
      double            ask_price;
     };
   #pragma pack(pop)

private:
   static MTReportInfo s_info;            // report information     
   static ReportColumn s_columns[];       // column descriptions

public:
   //---
                     CTickReport(void);
   virtual          ~CTickReport(void);
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
