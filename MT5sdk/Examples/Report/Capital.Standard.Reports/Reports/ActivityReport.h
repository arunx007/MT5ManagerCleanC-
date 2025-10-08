//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "ActivitySection.h"
#include "ActivityPeriodSection.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportInterval.h"
#include "..\Cache\ClientUserBalanceCache.h"
//+------------------------------------------------------------------+
//| Activity Report                                                  |
//+------------------------------------------------------------------+
class CActivityReport : public CCurrencyReport, public CClientUserBalanceReceiver
  {
private:
   //--- constants
   enum EnConstants
     {
      SMOOTH_RANGE_ONE     =30,                    // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =60,                    // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };

private:
   CReportInterval   m_interval;                   // report interval
   CClientUserBalanceCache m_client_balance;       // client balance cache
   CActivitySection  m_section_active;             // active/inactive total section
   CActivitySection  m_section_lead_source;        // activity by lead source
   CActivitySection  m_section_lead_campaign;      // activity by lead campaign
   CActivitySection  m_section_country;            // activity by country
   CActivityPeriodSection m_section_registration;  // activity by registration
   //--- static data
   static MTReportInfo s_info;                     // static report info

public:
   //--- constructor/destructor
                     CActivityReport(void);
   virtual          ~CActivityReport(void);
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
   //--- calculate total data
   MTAPIRES          CalculateTotal(void);
   //--- client and user balance read handler
   virtual MTAPIRES  ClientUserBalanceRead(const ClientUserCache &client_user,const ClientCache &client,const CUserBalance &balance,const double *rate) override;
   //--- client inactive read handler
   virtual MTAPIRES  ClientInactiveRead(const ClientCache &client) override;
   //--- client user inactive read handler
   virtual MTAPIRES  ClientUserInactiveRead(const ClientUserCache &client_user) override;
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
  };
//+------------------------------------------------------------------+
