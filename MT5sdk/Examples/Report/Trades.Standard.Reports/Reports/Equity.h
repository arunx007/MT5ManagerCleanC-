//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Daily Report                                                     |
//+------------------------------------------------------------------+
class CEquity : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_BANK           =1,
      COLUMN_ACCOUNTS       =2,
      COLUMN_BALANCE_PREV   =3,
      COLUMN_CLOSED_PL      =4,
      COLUMN_DEPOSIT        =5,
      COLUMN_BALANCE        =6,
      COLUMN_FLOATING_PL    =7,
      COLUMN_CREDIT         =8,
      COLUMN_EQUITY         =9,
      COLUMN_MARGIN         =10,
      COLUMN_FREE           =11,
      COLUMN_CURRENCY       =12,
      COLUMN_CURRENCY_DIGITS=13
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      wchar_t           bank[32];         // bank name
      uint32_t          accounts;         // amount of accounts
      double            balance_prev;     // previus day balance
      double            closed_pl;        // closed p/l
      double            deposit;          // daily balance
      double            balance;          // balance
      double            floating_pl;      // profit
      double            credit;           // credit
      double            equity;           // profit equity
      double            margin;           // margin
      double            free_margin;      // free margin
      wchar_t           currency[32];     // currency
      uint32_t          currency_digits;  // currency digits
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
   IMTUser          *m_user;              // user interface
   IMTConGroup      *m_group;             // group interface
   IMTDailyArray    *m_dailies;           // daily array interface
   TableRecordArray  m_banks;             // banks
   TableRecordArray  m_summaries;         // summaries

public:
   //--- constructor/destructor
                     CEquity(void);
   virtual          ~CEquity(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);
private:
   //--- collect banks

   //--- table support
   MTAPIRES          TablePrepare(void);
   MTAPIRES          TableCollect(const uint64_t login);
   MTAPIRES          TableWrite(void);
   MTAPIRES          TableWriteSummaries(void);
   //--- and clear
   void              Clear(void);
   //--- sort functions
   static int32_t    SortBanks(const void *left,const void *right);
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
