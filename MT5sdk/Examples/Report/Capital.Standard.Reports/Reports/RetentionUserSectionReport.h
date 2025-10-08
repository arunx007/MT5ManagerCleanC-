//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "RetentionSection.h"
#include "..\Cache\UserActivityCache.h"
#include "..\Cache\UserCache.h"
//+------------------------------------------------------------------+
//| Retention users by section Report                                |
//+------------------------------------------------------------------+
class CRetentionUserSectionReport : public CCurrencyReport
  {
private:
   //--- constants
   enum EnConstants
     {
      SMOOTH_RANGE_ONE     =8,                     // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =50,                    // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };

private:
   CReportInterval   m_interval;                   // report interval object
   CUserActivityCache *m_activity;                 // user activity cache
   CUserCache       *m_users;                      // user cache
   CRetentionSection m_section_lead_source;        // Retention by lead source
   CRetentionSection m_section_lead_campaign;      // Retention by lead campaign
   CRetentionSection m_section_country;            // Retention by country
   CRetentionSection m_section_registration;       // Retention by registration
   //--- static data
   static MTReportInfo s_info;                     // static report info
   static LPCWSTR    s_section_lead_source;        // Retention by lead source chart description
   static LPCWSTR    s_section_lead_campaign;      // Retention by lead campaign chart description
   static LPCWSTR    s_section_country;            // Retention by country chart description
   static LPCWSTR    s_section_registration;       // Retention by registration chart description

public:
   //--- constructor/destructor
                     CRetentionUserSectionReport(void);
   virtual          ~CRetentionUserSectionReport(void);
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
   //--- calculate total
   MTAPIRES          CalculateTotal(void);
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
  };
//+------------------------------------------------------------------+
