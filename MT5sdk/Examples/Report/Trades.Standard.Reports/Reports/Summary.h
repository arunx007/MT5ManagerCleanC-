//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//|  Summary Report                                                  |
//+------------------------------------------------------------------+
class CSummary : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN          =1,
      COLUMN_NAME           =2,
      COLUMN_DEPOSIT        =3,
      COLUMN_WITHDRAW       =4,
      COLUMN_IN_OUT         =5,
      COLUMN_CREDIT         =6,
      COLUMN_ADDITIONAL     =7,
      COLUMN_VOLUME         =8,
      COLUMN_COMMISSION     =9,
      COLUMN_FEE            =10,
      COLUMN_STORAGE        =11,
      COLUMN_PROFIT         =12,
      COLUMN_BALANCE        =13,
      COLUMN_CURRENCY       =14,
      COLUMN_DIGITS         =15,
      COLUMN_CURRENCY_DIGITS=16,
      COLUMN_EQUITY         =17,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          login;                  // login
      wchar_t           name[128];              // name
      double            deposit;                // deposit
      double            withdraw;               // withdraw
      double            inout;                  // in/out
      double            credit;                 // credit
      double            additional;             // additional
      double            commission;             // commission
      double            fee;                    // fee
      double            profit;                 // profit
      double            storage;                // swap
      uint64_t          volume;                 // volume
      double            balance_cur;            // current balance
      double            equity_cur;             // current equity
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
   IMTDealArray     *m_deals;             // deal array
   IMTAccount       *m_account;           // account
   IMTUser          *m_user;              // user
   IMTConGroup      *m_group;             // group
   TableRecordArray  m_summaries;         // summaries

public:
   //--- constructor/destructor
                     CSummary(void);
   virtual          ~CSummary(void);
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

