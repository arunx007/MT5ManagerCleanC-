//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "LtvSection.h"
#include "..\Tools\CurrencyConverterCache.h"
#include "..\Cache\ClientUserBalanceCache.h"
//+------------------------------------------------------------------+
//| LTV by section Report                                            |
//+------------------------------------------------------------------+
class CLtvSectionReport : public CCurrencyReport, public CClientUserBalanceReceiver
  {
private:
   CClientUserBalanceCache m_client_balance;       // client balance cache
   CLtvSection       m_section_lead_source;        // LTV by lead source
   CLtvSection       m_section_lead_campaign;      // LTV by lead campaign
   CLtvSection       m_section_country;            // LTV by country
   //--- static data
   static MTReportInfo s_info;                     // static report info

public:
   //--- constructor/destructor
                     CLtvSectionReport(void);
   virtual          ~CLtvSectionReport(void);
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
   //--- client and user balance deal read handler
   virtual MTAPIRES  ClientUserBalanceDealRead(const ClientUserCache &client_user,const ClientCache &client,const DealBalance &balance,const double *rate) override;
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
  };
//+------------------------------------------------------------------+
