//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Segregated Report                                                |
//+------------------------------------------------------------------+
class CSegregated : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN          =1,
      COLUMN_NAME           =2,
      COLUMN_DEPOSIT        =3,
      COLUMN_CREDIT         =4,
      COLUMN_COMMISSION     =5,
      COLUMN_STORAGE        =6,
      COLUMN_PROFIT         =7,
      COLUMN_INTEREST       =8,
      COLUMN_BALANCE        =9,
      COLUMN_FLOATING       =10,
      COLUMN_EQUITY         =11,
      COLUMN_CURRENCY       =12,
      COLUMN_CURRENCY_DIGITS=13,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          login;
      wchar_t           name[128];
      double            deposit;
      double            credit;
      double            commission;
      double            storage;
      double            profit;
      double            interest;
      double            balance;
      double            floating;
      double            equity;
      wchar_t           currency[32];
      uint32_t          currency_digits;
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
   IMTDailyArray    *m_daily;             // daily information
   IMTUser          *m_user;              // user
   IMTConGroup      *m_group;             // group
   TableRecordArray  m_summaries;         // summaries

public:
   //--- constructor/destructor
                     CSegregated(void);
   virtual          ~CSegregated(void);
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
