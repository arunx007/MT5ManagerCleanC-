//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "FtdSection.h"
#include "FtdTotalSection.h"
#include "..\Cache\ClientFtdCache.h"
//+------------------------------------------------------------------+
//| FTD by section Report                                            |
//+------------------------------------------------------------------+
class CFtdReport : public CCurrencyReport, public IClientFtdReceiver
  {
private:
   //--- constants
   enum EnConstants
     {
      SMOOTH_RANGE_ONE     =30,                    // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =124,                   // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };

private:
   CClientFtdCache   m_client_ftd;                 // client FTD cache
   CReportInterval   m_interval;                   // report parameters
   CFtdSection       m_section_lead_source;        // FTD by lead source
   CFtdSection       m_section_lead_campaign;      // FTD by lead campaign
   CFtdSection       m_section_country;            // FTD by country
   CFtdTotalSection  m_section_total_lead_source;  // FTD total by lead source
   CFtdTotalSection  m_section_total_lead_campaign;// FTD total by lead campaign
   CFtdTotalSection  m_section_total_country;      // FTD total by country
   //--- static data
   static MTReportInfo s_info;                     // static report info

public:
   //--- constructor/destructor
                     CFtdReport(void);
   virtual          ~CFtdReport(void);
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
   //--- client FTD read handler
   virtual MTAPIRES  ClientFtdRead(const CClientCache::ClientCache &client,const int64_t time,const double value) override;
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
  };
//+------------------------------------------------------------------+
