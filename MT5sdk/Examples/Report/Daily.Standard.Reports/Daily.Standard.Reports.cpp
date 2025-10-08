//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Reports\DailyTradeReport.h"
#include "Reports\DailyServerReport.h"
#include "Reports\DailyDealingReport.h"
#include "Reports\DailyExpertReport.h"
#include "Reports\DailyOrderReport.h"
#include "Reports\DailyPositionReport.h"
#include "Tools\ReportFactory.h"
//+------------------------------------------------------------------+
//| Report factories array                                           |
//+------------------------------------------------------------------+
static const CReportFactory g_factories[]=
{
   CReportFactory::Create<CDailyTradeReport>(),    // daily trade report
   CReportFactory::Create<CDailyServerReport>(),   // daily server report
   CReportFactory::Create<CDailyDealingReport>(),  // daily dealing report
   CReportFactory::Create<CDailyExpertReport>(),   // daily export report
   CReportFactory::Create<CDailyOrderReport>(),    // daily order report
   CReportFactory::Create<CDailyPositionReport>(), // daily position report
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
