//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportBase.h"
#include "..\Tools\DatasetField.h"
#include "..\Tools\HistorySelect.h"
//+------------------------------------------------------------------+
//| Daily Orders Report                                              |
//+------------------------------------------------------------------+
class CDailyOrderReport : public CReportBase
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_ORDER        =1,
      COLUMN_ID           =2,
      COLUMN_LOGIN        =3,
      COLUMN_NAME         =4,
      COLUMN_TIME_REPORT  =5,
      COLUMN_TIME_SETUP   =6,
      COLUMN_TYPE         =7,
      COLUMN_SYMBOL       =8,
      COLUMN_VOLUME       =9,
      COLUMN_PRICE_ORDER  =10,
      COLUMN_PRICE_SL     =12,
      COLUMN_PRICE_TP     =13,
      COLUMN_PRICE_CURRENT=14,
      COLUMN_TYPE_REASON  =15,
      COLUMN_STATE        =16,
      COLUMN_COMMENT      =17,
      COLUMN_DIGITS       =18
     };
   //--- user record
   #pragma pack(push,1)
   struct UserRecord
     {
      int64_t           datetime;            // data and time
      uint64_t          login;               // login
      wchar_t           name[128];           // name
     };
   //--- order record
   struct OrderRecord
     {
      uint64_t          order;               // order ticket
      wchar_t           id[32];              // order ticket in external system (exchange, ECN, etc)
      int64_t           time_setup;          // order reception from a client into the system in milliseconds
      uint32_t          type;                // order type
      wchar_t           symbol[32];          // order symbol
      uint64_t          volume_initial;      // starting order volume
      uint64_t          volume_current;      // current order volume
      double            price_order;         // order price
      double            price_sl;            // stop-loss price
      double            price_tp;            // take-profit price
      double            price_current;       // current order price
      uint32_t          type_reason;         // order creation reason
      uint32_t          state;               // actual order state
      wchar_t           comment[32];         // order comment
      uint32_t          digits;              // number of digits of order symbol
     };
   //--- table record
   struct TableRecord : UserRecord,OrderRecord
     {
     };
   #pragma pack(pop)

private:
   int64_t           m_ctm;                  // report time
   IMTReportCacheKeySet *m_report_logins;    // report request logins
   IMTDaily         *m_daily;                // daily report interface
   IMTOrder         *m_daily_order;          // order interface
   CDailySelect      m_daily_select;         // daily select object
   //--- records
   TableRecord       m_record;               // current table record
   UserRecord       &m_user;                 // reference to current user record
   OrderRecord      &m_order;                // reference to current order record
   TableRecord       m_summary;              // current summary record
   //---
   static const MTReportInfo s_info;               // report info
   static const ReportColumn s_columns[];          // column descriptions   
   static const DatasetField s_daily_fields[];     // daily request fields descriptions
   static const uint32_t s_daily_request_limit=100000; // daily request limit

public:
   //--- constructor/destructor
                     CDailyOrderReport(void);
   virtual          ~CDailyOrderReport(void);
   //--- report information
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(void) override;
   //--- write report parts
   MTAPIRES          WriteLogins(void);
   MTAPIRES          WriteUsersOrders(IMTDataset &users,uint64_t &login_last);
   MTAPIRES          WriteOrders(void);
   MTAPIRES          WriteOrder(void);
   MTAPIRES          FillOrder(void);
   //--- summary
   void              SummaryAdd(void);
   MTAPIRES          SummaryWrite(void);
  };
//+------------------------------------------------------------------+

