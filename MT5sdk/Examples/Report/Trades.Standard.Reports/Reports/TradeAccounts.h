//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade Accounts Report                                            |
//+------------------------------------------------------------------+
class CTradeAccounts : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN          =1,
      COLUMN_NAME           =2,
      COLUMN_LEVERAGE       =3,
      COLUMN_BALANCE        =4,
      COLUMN_CREDIT         =5,
      COLUMN_EQUITY         =6,
      COLUMN_MARGIN         =7,
      COLUMN_MARGIN_FREE    =8,
      COLUMN_MARGIN_LEVEL   =9,
      COLUMN_BLOCKED_COMM   =10,
      COLUMN_BLOCKED_PROFIT =11,
      COLUMN_PROFIT         =12,
      COLUMN_STORAGE        =13,
      COLUMN_FLOATING       =14,
      COLUMN_CURRENCY       =15,
      COLUMN_CURRENCY_DIGITS=16,
      COLUMN_GROUP          =17,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          login;                  // login
      wchar_t           name[128];              // name
      wchar_t           group[64];              // group
      uint32_t          leverage;               // leverage
      double            balance;                // deposit
      double            credit;                 // credit
      double            equity;                 // equity
      double            margin;                 // margin
      double            margin_free;            // free margin
      double            margin_level;           // margin level
      double            blocked_comm;           // blocked commission
      double            blocked_profit;         // blocked profit
      double            profit;                 // raw profit
      double            storage;                // storage
      double            floating;               // floating profit
      wchar_t           currency[32];           // currency
      uint32_t          currency_digits;        // currency digits
     };
   #pragma pack(pop)
   //---
   typedef TMTArray<TableRecord> TableRecordArray;

private:
   //---
   static MTReportInfo s_info;            // report information     
   static ReportColumn s_columns[];       // column descriptions   
   //--- 
   IMTReportAPI     *m_api;               // api
   IMTAccount       *m_account;           // account
   IMTUser          *m_user;              // user
   IMTConGroup      *m_group;             // group
   TableRecordArray  m_summaries;         // summaries

public:
   //--- constructor/destructor
                     CTradeAccounts(void);
   virtual          ~CTradeAccounts(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- clear
   void              Clear(void);
   //--- table management
   MTAPIRES          TablePrepare(void);
   MTAPIRES          TableWrite(const uint64_t login);
   MTAPIRES          TableWriteSummaries(void);
   //--- sort functions
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

