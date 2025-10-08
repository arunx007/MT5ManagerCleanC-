//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\CurrencyConverterCache.h"
#include "..\Cache\UserCache.h"
#include "..\Cache\DepositCache.h"
//+------------------------------------------------------------------+
//| Lifetime Value Detailed by Trading Account Report class          |
//+------------------------------------------------------------------+
class CLtvDetailedUser : public CCurrencyReport
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN               =1,
      COLUMN_NAME                =2,
      COLUMN_REGISTRATION_TIME   =3,
      COLUMN_FTD_TIME            =4,
      COLUMN_DEPOSIT_COUNT       =5,
      COLUMN_LTV                 =6,
      COLUMN_FTD                 =7,
      COLUMN_CURRENCY            =8,
      COLUMN_CURRENCY_DIGITS     =9,
      COLUMN_LTV_CURRENCY        =10,
      COLUMN_FTD_CURRENCY        =11,
     };
   //--- result table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          login;                     // login
      wchar_t           name[128];                 // name
      int64_t           registration_time;         // registration time
      int64_t           deposit_first_time;        // first deposit time
      uint32_t          deposit_count;             // deposit count
      double            deposit_first;             // first deposit
      double            deposit_amount;            // deposit amount
      wchar_t           currency[32];              // currency
      uint32_t          currency_digits;           // currency digits
      double            ftd_currency;              // FTD in report currency
      double            ltv_currency;              // LTV in report currency
     };
   #pragma pack(pop)

private:
   CUserCache       *m_users;                      // user cache
   CDepositCache    *m_deposits;                   // deposit cache
   //--- static data
   static MTReportInfo s_info;                     // static report info
   static ReportColumn s_columns[];                // column descriptions

public:
   //--- constructor/destructor
                     CLtvDetailedUser(void);
   virtual          ~CLtvDetailedUser(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- request currency from report's configuration
   virtual MTAPIRES  Prepare(void) override;
   //--- add table columns
   MTAPIRES          PrepareTable(void);
   //--- select data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- write result
   virtual MTAPIRES  WriteResult(void) override;
  };
//+------------------------------------------------------------------+
