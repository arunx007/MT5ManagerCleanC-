//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MoneyFlowDailyReport.h"
#include "..\Cache\UserCache.h"
#include "..\Cache\BalanceCache.h"
#include "..\Tools\ReportVector.h"
//+------------------------------------------------------------------+
//| Money Flow Daily by users Report class                           |
//+------------------------------------------------------------------+
class CMoneyFlowDailyUser : public CMoneyFlowDaily
  {
private:
   //--- types
   typedef TReportVector<Section> SectionVector;      // money vector type

private:
   CUserCache       *m_users;                         // user cache
   CBalanceCache    *m_balances;                      // balance cache
   SectionVector     m_dates;                         // money vector
   //--- static data
   static MTReportInfo s_info;                        // static report info

public:
   //--- constructor/destructor
                     CMoneyFlowDailyUser(void);
   virtual          ~CMoneyFlowDailyUser(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- request data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- calculate daily
   MTAPIRES          CalculateDaily(void);
   //--- calculate user balance
   MTAPIRES          CalculateUserBalance(const CUserCache::UserCache &user,const CBalanceCache::CUserBalance &balance);
   //--- calculate balance
   MTAPIRES          CalculateBalance(const CBalanceCache::DealBalance &deal,const double *rate);
   //--- record by time
   Section*          RecordByTime(const int64_t ctm);
   //--- prepare money data
   virtual MTAPIRES  PrepareData(IMTDataset &data) override;
   //--- prepare money money graph
   virtual MTAPIRES  PrepareGraph(IMTDataset *data,const ReportColumn &column) override;
   //--- chart columns add
   virtual MTAPIRES  ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const override;
  };
//+------------------------------------------------------------------+
