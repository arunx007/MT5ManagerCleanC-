//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "LtvSection.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportInterval.h"
#include "..\Cache\ClientUserBalanceCache.h"
//+------------------------------------------------------------------+
//| LTV Report                                                       |
//+------------------------------------------------------------------+
class CLtvReport : public CCurrencyReport, public CClientUserBalanceReceiver
  {
private:
   //--- constants
   enum EnConstants
     {
      SMOOTH_RANGE_ONE     =8,                     // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =50,                    // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };

private:
   CReportInterval   m_interval;                   // report interval
   CClientUserBalanceCache m_client_balance;       // client balance cache
   CLtvSection       m_section;                    // LTV section
   //--- static data
   static MTReportInfo s_info;                     // static report info

public:
   //--- constructor/destructor
                     CLtvReport(void);
   virtual          ~CLtvReport(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- get report parameters and create caches
   virtual MTAPIRES  Prepare(void) override;
   //--- request data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- client and user balance read handler
   virtual MTAPIRES  ClientUserBalanceRead(const ClientUserCache &client_user,const ClientCache &client,const CUserBalance &balance,const double *rate) override;
   //--- client and user balance deal read handler
   MTAPIRES          BalanceDealRead(const ClientUserCache &client_user,const ClientCache &client,const DealBalance &balance,const uint32_t section,const double *rate);
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
  };
//+------------------------------------------------------------------+
