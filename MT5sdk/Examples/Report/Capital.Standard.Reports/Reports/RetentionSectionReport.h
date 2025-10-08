//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "RetentionSection.h"
#include "..\Tools\CurrencyConverterCache.h"
#include "..\Cache\UserActivityCache.h"
#include "..\Cache\UserCache.h"
#include "..\Cache\ClientUserCache.h"
#include "..\Cache\ClientCache.h"
//+------------------------------------------------------------------+
//| Retention by section Report                                      |
//+------------------------------------------------------------------+
class CRetentionSectionReport : public CCurrencyReport
  {
private:
   //--- constants
   enum EnConstants
     {
      SMOOTH_RANGE_ONE     =30,                    // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =60,                    // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };

private:
   CReportInterval   m_interval;                   // report interval object
   int64_t           m_from;                       // report interval begin
   int64_t           m_now;                        // now time
   CUserActivityCache *m_activity;                 // user activity cache
   CUserCache       *m_users;                      // user cache
   CClientUserCache *m_client_users;               // client by user cache
   CClientCache     *m_clients;                    // client cache
   IMTReportCacheKeySet *m_ids;                    // active clients ids
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
                     CRetentionSectionReport(void);
   virtual          ~CRetentionSectionReport(void);
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
   //--- calculate report sections
   MTAPIRES          CalculateSections(void);
   //--- calculate total
   MTAPIRES          CalculateTotal(void);
   //--- client activity read handler
   MTAPIRES          ActivityReadId(const uint64_t id,const CUserActivityCache::UserActivityCache &activity);
   //--- client activity read handler
   MTAPIRES          ActivityRead(const uint64_t id,const CUserActivityCache::UserActivityCache &activity);
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
  };
//+------------------------------------------------------------------+
