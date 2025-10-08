//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportContext.h"
#include "..\Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| Orderbook NFA Report                                             |
//+------------------------------------------------------------------+
class COrderbookReport : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_ID                  =1,
      COLUMN_BATCH_DATE          =2,
      COLUMN_TRANSACTION_DATETIME=3,
      COLUMN_ORDER_ID            =4,
      COLUMN_FDM_ID              =5,
      COLUMN_MKT_SEG_ID          =6,
      COLUMN_PRODUCT_CODE        =7,
      COLUMN_PRODUCT_CAT         =8,
      COLUMN_CONTRACT_YEAR       =9,
      COLUMN_CONTRACT_MONTH      =10,
      COLUMN_CONTRACT_DAY        =11,
      COLUMN_STRIKE              =12,
      COLUMN_OPTION_TYPE         =13,
      COLUMN_TRANSACTION_TYPE    =14,
      COLUMN_ORDER_TYPE          =15,
      COLUMN_VERB                =16,
      COLUMN_BID_PRICE           =17,
      COLUMN_ASK_PRICE           =18,
      COLUMN_QUANTITY            =19,
      COLUMN_REMAINING_QTY       =20,
      COLUMN_PRICE               =21,
      COLUMN_STOP_PRICE          =22,
      COLUMN_STOP_PRODUCT_CODE   =23,
      COLUMN_TRAIL_AMT           =24,
      COLUMN_LIMIT_OFFSET        =25,
      COLUMN_DURATION            =26,
      COLUMN_EXPIRY_DATE         =27,
      COLUMN_ORDER_ORIGIN        =28,
      COLUMN_MANAGER_ID          =29,
      COLUMN_CUSTACCT_ID         =30,
      COLUMN_SERVER_ID           =31,
      COLUMN_CUST_GROUP          =32,
      COLUMN_LINKED_ORDER_ID     =34,
      COLUMN_LINK_REASON         =35,
      COLUMN_OPEN_CLOSE          =36
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          id;
      int64_t           batch_date;
      int64_t           transaction_datetime;
      wchar_t           order_id[64];
      wchar_t           fdm_id[8];
      wchar_t           mkt_seg_id[32];
      wchar_t           product_code[8];
      wchar_t           product_cat[2];
      wchar_t           contract_year[8];
      wchar_t           contract_month[4];
      wchar_t           contract_day[4];
      double            strike;
      wchar_t           option_type[16];
      wchar_t           transaction_type[32];
      wchar_t           order_type[32];
      wchar_t           verb[2];
      double            bid_price;
      double            ask_price;
      uint64_t          quantity;
      uint64_t          remaining_qty;
      double            price;
      double            stop_price;
      wchar_t           stop_product_code[8];
      double            trail_amt;
      double            limit_offset;
      wchar_t           duration[16];
      int64_t           expiry_date;
      wchar_t           order_origin[8];
      wchar_t           manager_id[32];
      uint64_t          custacct_id;
      wchar_t           server_id[32];
      wchar_t           cust_group[32];
      wchar_t           linked_order_id[64];
      wchar_t           link_reason[32];
      wchar_t           open_close[1];
     };
   #pragma pack(pop)

private:
   static MTReportInfo s_info;            // report information     
   static ReportColumn s_columns[];       // column descriptions

public:
   //---
                     COrderbookReport(void);
   virtual          ~COrderbookReport(void);
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
