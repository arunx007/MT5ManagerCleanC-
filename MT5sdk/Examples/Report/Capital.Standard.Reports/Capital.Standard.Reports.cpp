//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Reports\FtdTotalSectionReport.h"
#include "Reports\FtdSectionReport.h"
#include "Reports\FtdReport.h"
#include "Reports\LtvReport.h"
#include "Reports\LtvSectionReport.h"
#include "Reports\LtvDetailedLeadSourceReport.h"
#include "Reports\LtvDetailedLeadCampaignReport.h"
#include "Reports\LtvDetailedCountryReport.h"
#include "Reports\LtvDetailedClientReport.h"
#include "Reports\LtvDetailedUserReport.h"
#include "Reports\MoneyFlowDailyUserReport.h"
#include "Reports\MoneyFlowDailyLeadSourceReport.h"
#include "Reports\MoneyFlowDailyLeadCampaignReport.h"
#include "Reports\MoneyFlowDailyCountryReport.h"
#include "Reports\MoneyFlowWeekReport.h"
#include "Reports\ActivityReport.h"
#include "Reports\RetentionSectionReport.h"
#include "Reports\RetentionUserSectionReport.h"
#include "Reports\DealProfitReport.h"
#include "Reports\DealReasonReport.h"
#include "Reports\DealCountryReport.h"
#include "Reports\DealWeekReport.h"
#include "Reports\RiskAppetiteReport.h"
#include "Tools\ReportFactory.h"
//+------------------------------------------------------------------+
//| Report factories array                                           |
//+------------------------------------------------------------------+
static const CReportFactory g_factories[]=
{
   CReportFactory::Create<CFtdTotalSectionReport>(),        // ftd total by section
   CReportFactory::Create<CFtdSectionReport>(),             // ftd by section
   CReportFactory::Create<CFtdReport>(),                    // ftd
   CReportFactory::Create<CLtvSectionReport>(),             // ltv by section
   CReportFactory::Create<CLtvReport>(),                    // ltv
   CReportFactory::Create<CLtvDetailedLeadSource>(),        // ltv detailed by lead source
   CReportFactory::Create<CLtvDetailedLeadCampaign>(),      // ltv detailed by lead campaign
   CReportFactory::Create<CLtvDetailedCountry>(),           // ltv detailed by country
   CReportFactory::Create<CLtvDetailedClient>(),            // ltv detailed by client
   CReportFactory::Create<CLtvDetailedUser>(),              // ltv detailed by user
   CReportFactory::Create<CMoneyFlowDailyUser>(),           // money flow daily
   CReportFactory::Create<CMoneyFlowDailyLeadSource>(),     // money flow daily by lead source
   CReportFactory::Create<CMoneyFlowDailyLeadCampaign>(),   // money flow daily by lead campaign
   CReportFactory::Create<CMoneyFlowDailyCountry>(),        // money flow daily by country
   CReportFactory::Create<CMoneyFlowWeek>(),                // money flow weekly
   CReportFactory::Create<CActivityReport>(),               // clients activity
   CReportFactory::Create<CRetentionSectionReport>(),       // retention clients by section
   CReportFactory::Create<CRetentionUserSectionReport>(),   // retention users by section
   CReportFactory::Create<CDealProfitReport>(),             // deal profit
   CReportFactory::Create<CDealReasonReport>(),             // deal reason
   CReportFactory::Create<CDealCountryReport>(),            // deal country
   CReportFactory::Create<CDealWeekReport>(),               // deal weekly report
   CReportFactory::Create<CRiskAppetiteReport>(),           // users risk appetite
   };
//+------------------------------------------------------------------+
//| DLL Main Function                                                |
//+------------------------------------------------------------------+
BOOL APIENTRY DllMain(HMODULE hModule,DWORD reason,LPVOID lpReserved)
  {
//---
   switch(reason)
     {
      case DLL_PROCESS_ATTACH:
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
         break;
     }
   return(TRUE);
  }
//+------------------------------------------------------------------+
//| Plug-in About entry function                                     |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportAbout(const uint32_t index,MTReportInfo& info)
  {
//--- check index
   if(index>=_countof(g_factories))
      return(MT_RET_ERR_NOTFOUND);
//--- report information
   return(g_factories[index].About(info));
  }
//+------------------------------------------------------------------+
//| Plug-in instance creation entry point                            |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportCreate(const uint32_t index,const uint32_t apiversion,IMTReportContext **context)
  {
//--- check index
   if(index>=_countof(g_factories))
      return(MT_RET_ERR_NOTFOUND);
//--- create report instance
   return(g_factories[index].Create(context));
  }
//+------------------------------------------------------------------+
