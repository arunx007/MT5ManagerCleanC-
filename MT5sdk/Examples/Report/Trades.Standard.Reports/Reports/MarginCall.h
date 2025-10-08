//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Margin Call Report                                               |
//+------------------------------------------------------------------+
class CMarginCall : public IMTReportContext
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
      COLUMN_FLOATING       =6,
      COLUMN_EQUTITY        =7,
      COLUMN_MARGIN         =8,
      COLUMN_MARGIN_FREE    =9,
      COLUMN_MARGIN_LIMITS  =10,
      COLUMN_MARGIN_LEVEL   =11,
      COLUMN_MARGIN_ADD     =12,
      COLUMN_CURRENCY       =13,
      COLUMN_CURRENCY_DIGITS=14,
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          login;               // login
      wchar_t           name[128];           // client name
      uint32_t          leverage;            // leverage
      double            balance;             // balance
      double            credit;              // credit
      double            floating;            // floating p / l
      double            equity;              // equity
      double            margin;              // margin
      double            margin_free;         // free margin
      wchar_t           margin_limits[64];   // margin limits
      double            margin_level;        // margin level
      double            margin_add;          // additional margin
      wchar_t           currency[32];        // group currency
      uint32_t          currency_digits;     // group currency digits
     };
   #pragma pack(pop)
   //--- array type
   typedef TMTArray<TableRecord> TableRecordArray;

private:
   //---
   static MTReportInfo s_info;            // report information   
   static ReportColumn s_columns[];       // column descriptions     
   //--- 
   IMTReportAPI     *m_api;               // api
   IMTAccount       *m_account;           // trade account
   IMTUser          *m_user;              // user
   IMTConGroup      *m_group;             // group
   TableRecordArray  m_summaries;         // summaries

public:
   //--- constructor/destructor
                     CMarginCall(void);
   virtual          ~CMarginCall(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);
   //--- formating functions
   static LPCWSTR    FormatMarginLimits(CMTStr& str,const IMTConGroup *group);

private:
   //--- clear
   void              Clear(void);
   //--- table management
   MTAPIRES          TablePrepare(void);
   MTAPIRES          TableWrite(const uint64_t login);
   MTAPIRES          TableWriteSummaries(void);
   //--- calc additional margin
   double            CalcMarginAdditional(const IMTConGroup *group,const IMTAccount *account);
   //--- sort functions
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
