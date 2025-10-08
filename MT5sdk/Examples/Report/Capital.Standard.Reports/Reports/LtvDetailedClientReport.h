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
#include "..\Cache\ClientUserCache.h"
#include "..\Cache\DepositCache.h"
#include "..\Cache\ClientCache.h"
#include "..\Cache\ClientFilter.h"
//+------------------------------------------------------------------+
//| Lifetime Value Detailed by Client Report class                   |
//+------------------------------------------------------------------+
class CLtvDetailedClient : public CCurrencyReport
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_ID                  =1,
      COLUMN_NAME                =2,
      COLUMN_COUNTRY             =3,
      COLUMN_REGISTRATION        =4,
      COLUMN_FTD_TIME            =5,
      COLUMN_USER_COUNT          =6,
      COLUMN_DEPOSIT_COUNT       =7,
      COLUMN_LTV                 =8,
      COLUMN_FTD                 =9,
     };
   //--- result table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          id;                        // client id
      wchar_t           name[128];                 // name
      wchar_t           country[64];               // currency
      int64_t           registration;              // registration time
      int64_t           deposit_first_time;        // first deposit time
      uint32_t          user_count;                // user count
      uint32_t          deposit_count;             // deposit count
      double            deposit_first;             // first deposit
      double            deposit_amount;            // deposit amount
     };
   #pragma pack(pop)
   //--- client money structure
   struct ClientMoney
     {
      uint64_t          client_id;                 // client id
      int64_t           registration;              // first deposit time
      int64_t           deposit_first_time;        // first deposit time
      uint32_t          user_count;                // user count
      uint32_t          deposit_count;             // deposit count
      double            deposit_first;             // first deposit
      double            deposit_amount;            // deposit amount
     };
   //--- client money structure array type
   typedef TMTArray<ClientMoney> ClientMoneyArray;

private:
   CUserCache       *m_users;                      // user cache
   CClientUserCache *m_client_users;               // client by user cache
   CDepositCache    *m_deposits;                   // deposit cache
   CClientCache     *m_clients;                    // clients cache
   ClientMoneyArray  m_moneys;                     // client money array
   CClientFilter     m_filter;                     // client filter
   //--- static data
   static MTReportInfo s_info;                     // static report info
   static ReportColumn s_columns[];                // column descriptions

public:
   //--- constructor/destructor
                     CLtvDetailedClient(void);
   virtual          ~CLtvDetailedClient(void);
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
   //--- calculate money
   MTAPIRES          CalculateMoney(void);
   //--- initialize money
   MTAPIRES          InitializeMoney(const IMTReportCacheKeySet &clients);
   //--- client read handler
   MTAPIRES          ClientUserRead(const uint64_t id,const CClientUserCache::ClientUserCache &client);
   //--- write result
   virtual MTAPIRES  WriteResult(void) override;
   //--- write client to result
   MTAPIRES          WriteClient(const ClientMoney &money);
   //--- search static method
   static int32_t    SearchMoneyClient(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
