//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\TradeReport.h"
#include "..\Tools\DatasetField.h"
#include "..\Tools\GroupCache.h"
#include "..\Tools\HistorySelect.h"
//+------------------------------------------------------------------+
//| History Deals Report                                             |
//+------------------------------------------------------------------+
class CDealsHistory : public CTradeReport
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_DEAL           =1,
      COLUMN_ID             =2,
      COLUMN_ORDER          =3,
      COLUMN_LOGIN          =4,
      COLUMN_NAME           =5,
      COLUMN_TIME           =6,
      COLUMN_TYPE           =7,
      COLUMN_ENTRY          =8,
      COLUMN_SYMBOL         =9,
      COLUMN_VOLUME         =10,
      COLUMN_PRICE          =11,
      COLUMN_SL             =12,
      COLUMN_TP             =13,
      COLUMN_REASON         =14,
      COLUMN_COMMISSION     =15,
      COLUMN_FEE            =16,
      COLUMN_STORAGE        =17,
      COLUMN_PROFIT         =18,
      COLUMN_DEALER         =19,
      COLUMN_COMMENT        =20,
      COLUMN_CURRENCY       =21,
      COLUMN_DIGITS         =22,
      COLUMN_CURRENCY_DIGITS=23,
      COLUMN_POSITION       =24,
     };
   #pragma pack(push,1)
   //--- user record
   struct UserRecord
     {
      uint64_t          login;               // login
      wchar_t           name[32];            // name
      wchar_t           group[64];           // group
     };
   //--- deal record
   struct DealRecord
     {
      uint64_t          login;               // login
      uint64_t          deal;                // deal
      wchar_t           id[32];              // external id
      uint64_t          order;               // order
      uint64_t          position;            // position
      int64_t           time_create;         // deal time
      uint32_t          action;              // action
      uint32_t          entry;               // entry
      wchar_t           symbol[32];          // symbol
      uint64_t          volume;              // volume
      double            price;               // price
      double            sl;                  // sl
      double            tp;                  // tp
      uint32_t          reason;              // reason
      double            commission;          // commission
      double            fee;                 // fee
      double            storage;             // storage
      double            profit;              // profit
      uint64_t          dealer;              // dealer
      wchar_t           comment[32];         // deal comment
      uint32_t          digits;              // symbol digits
     };
   //--- table record
   struct TableRecord : DealRecord
     {
      wchar_t           name[32];            // user name
      wchar_t           currency[32];        // currency
      uint32_t          currency_digits;     // currency digits
     };
   #pragma pack(pop)
   //--- table record array type
   typedef TMTArray<TableRecord> TableRecordArray;

private:
   //--- API interfaces
   IMTConGroup      *m_group;                // group config
   IMTDatasetRequest *m_user_request;        // user request
   IMTDatasetField  *m_user_request_login;   // user request login field
   IMTDatasetField  *m_user_request_group;   // user request group field
   IMTDataset       *m_user_dataset;         // user dataset
   IMTReportCacheKeySet *m_user_logins;      // user logins
   IMTReportCacheKeySet *m_deal_logins;      // deal logins
   IMTReportCacheKeySet *m_report_logins;    // report request logins
   //--- objects
   CDealSelect       m_deal_select;          // deal select object
   //--- counts and times
   uint64_t          m_user_total;           // total users processed
   uint64_t          m_user_time_select;     // users selection time
   //--- records
   TableRecord       m_record;               // current table record
   DealRecord       &m_deal;                 // reference to current deal record
   UserRecord        m_user;                 // current user record
   bool              m_user_valid;           // current user record valid
   TableRecordArray  m_summaries;            // summaries
   TableRecord      *m_summary;              // current summary record
   //---
   static const MTReportInfo s_info;               // report info
   static const ReportColumn s_columns[];          // column descriptions   
   static const DatasetField s_user_fields[];      // user request fields descriptions
   static const uint32_t s_user_request_limit=100000;  // user request limit
   static const DatasetField s_deal_fields[];      // deal request fields descriptions
   static const uint64_t s_deal_actions[];           // deal request actions
   static const uint32_t s_deal_request_limit=16384;   // deal request limit

public:
   //--- constructor/destructor
                     CDealsHistory(void);
   virtual          ~CDealsHistory(void);
   //--- report information
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(void) override;
   //--- prepare requests
   MTAPIRES          PrepareUserRequest(void);
   MTAPIRES          PrepareDealRequest(void);
   //--- write report parts
   MTAPIRES          WriteGroups(LPCWSTR groups);
   MTAPIRES          WriteLogins(void);
   MTAPIRES          WriteGroup(void);
   MTAPIRES          WriteUsers(uint64_t &login_last);
   MTAPIRES          WriteUsersLogins(IMTReportCacheKeySet *logins);
   MTAPIRES          WriteUsersDeals(void);
   MTAPIRES          WriteDeals(IMTDataset &dataset,uint64_t &login_last);
   MTAPIRES          WriteDealsLogins(IMTDataset &dataset,IMTReportCacheKeySet &logins,CGroupCache &groups);
   //--- read from dataset
   static MTAPIRES   ReadLogins(IMTReportCacheKeySet &logins,IMTDataset &dataset,uint64_t &login_last);
   MTAPIRES          ReadUser(const bool group);
   //--- summary
   MTAPIRES          SummaryInit(void);
   void              SummaryAdd(TableRecord &summary);
   MTAPIRES          SummaryWrite(void);
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

