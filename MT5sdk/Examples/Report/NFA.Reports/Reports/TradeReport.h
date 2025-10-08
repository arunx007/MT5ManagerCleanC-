//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportContext.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| Trade NFA Report                                                 |
//+------------------------------------------------------------------+
class CTradeReport : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_ID                  =1,
      COLUMN_BATCH_DATE          =2,
      COLUMN_TRADE_ID            =3,
      COLUMN_ORDER_ID            =4,
      COLUMN_FDM_ID              =5,
      COLUMN_MKT_SEG_ID          =6,
      COLUMN_TIME_TRADEMATCH     =7,
      COLUMN_PRODUCT_CAT         =8,
      COLUMN_PRODUCT_CODE        =9,
      COLUMN_CONTRACT_YEAR       =10,
      COLUMN_CONTRACT_MONTH      =11,
      COLUMN_CONTRACT_DAY        =12,
      COLUMN_STRIKE              =13,
      COLUMN_VERB                =14,
      COLUMN_BID_PRICE           =15,
      COLUMN_ASK_PRICE           =16,
      COLUMN_QUANTITY            =17,
      COLUMN_REMAINING_QTY       =18,
      COLUMN_FILL_PRICE          =19,
      COLUMN_CONTRA_FILL_PRICE   =20,
      COLUMN_SERVER_ID           =21,
      COLUMN_IMPLIED_VOLATILITY  =22,
      COLUMN_IB_REBATE           =23,
      COLUMN_COMMISSION          =24,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          id;
      int64_t           batch_date;
      wchar_t           trade_id[64];
      wchar_t           order_id[64];
      wchar_t           fdm_id[8];
      wchar_t           mkt_seg_id[32];
      int64_t           time_tradematch;
      wchar_t           product_cat[2];
      wchar_t           product_code[8];
      wchar_t           contract_year[8];
      wchar_t           contract_month[4];
      wchar_t           contract_day[4];
      double            strike;
      wchar_t           verb[2];
      double            bid_price;
      double            ask_price;
      uint64_t          quantity;
      uint64_t          remaining_qty;
      double            fill_price;
      double            contra_fill_price;
      wchar_t           server_id[32];
      double            implied_volatility;
      double            ib_rebate;
      double            commission;
     };
   #pragma pack(pop)

private:
   static MTReportInfo s_info;            // report information     
   static ReportColumn s_columns[];       // column descriptions

public:
   //---
                     CTradeReport(void);
   virtual          ~CTradeReport(void);
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
