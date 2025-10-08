//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportContext.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| Adjustments NFA Report                                           |
//+------------------------------------------------------------------+
class CAdjustmentsReport : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_BATCH_DATE            =1,
      COLUMN_FDM_ID                =2,
      COLUMN_TRADE_ID              =3,
      COLUMN_TRADE_DATE            =4,
      COLUMN_ADJUSTMENT_DATETIME   =5,
      COLUMN_ADJUSTMENT_TYPE       =6,
      COLUMN_PRICE                 =7,
      COLUMN_QUANTITY              =8,
      COLUMN_CASH_AMOUNT           =9,
      COLUMN_NOTE                  =10,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      int64_t           batch_date;
      wchar_t           fdm_id[8];
      wchar_t           trade_id[64];
      int64_t           trade_date;
      int64_t           adjustment_datetime;
      wchar_t           adjustment_type[12];
      double            price;
      uint64_t          quantity;
      double            cash_amount;
      wchar_t           note[2000];
     };
   #pragma pack(pop)

private:
   static MTReportInfo s_info;            // report information     
   static ReportColumn s_columns[];       // column descriptions

public:
                     CAdjustmentsReport(void);
   virtual          ~CAdjustmentsReport(void);
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
