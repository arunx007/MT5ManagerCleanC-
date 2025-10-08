//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Daily Report                                                     |
//+------------------------------------------------------------------+
class CDaily : public IMTReportContext
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_TIME           =1,
      COLUMN_LOGIN          =2,
      COLUMN_NAME           =3,
      COLUMN_BALANCE_PREV   =4,
      COLUMN_DEPOSIT        =5,
      COLUMN_CLOSED_PL      =6,
      COLUMN_BALANCE        =7,
      COLUMN_CREDIT         =8,
      COLUMN_FLOATING_PL    =9,
      COLUMN_EQUITY         =10,
      COLUMN_MARGIN         =11,
      COLUMN_FREE           =12,
      COLUMN_CURRENCY       =13,
      COLUMN_CURRENCY_DIGITS=14
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      int64_t           datetime;         // daily datetime
      uint64_t          login;            // login
      wchar_t           name[32];         // client name
      double            deposit;          // daily balance
      double            balance_prev;     // previus day balance
      double            closed_pl;        // closed p/l
      double            balance;          // balance
      double            credit;           // credit
      double            floating_pl;      // profit
      double            equity;           // profit equity
      double            margin;           // margin
      double            free_margin;      // free margin
      wchar_t           currency[32];     // currency
      uint32_t          currency_digits;  // currency digits
     };
   #pragma pack(pop)

private:
   //---
   static MTReportInfo s_info;            // report information
   static ReportColumn s_columns[];       // column descriptions   
   //---
   IMTReportAPI     *m_api;               // api
   IMTUser          *m_user;              // user interface
   IMTConGroup      *m_group;             // group interface
   IMTDailyArray    *m_dailies;           // daily array interface

public:
   //--- constructor/destructor
                     CDaily(void);
   virtual          ~CDaily(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);
private:
   //--- table support
   MTAPIRES          TablePrepare(void);
   MTAPIRES          TableWrite(const uint64_t login);
   //--- and clear
   void              Clear(void);
  };
//+------------------------------------------------------------------+
