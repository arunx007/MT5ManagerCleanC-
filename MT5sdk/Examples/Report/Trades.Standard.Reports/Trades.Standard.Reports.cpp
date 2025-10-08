//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Reports\DepositWithdrawal.h"
#include "Reports\StopOutCompensation.h"
#include "Reports\DailyDetailed.h"
#include "Reports\OrdersHistory.h"
#include "Reports\CreditFacility.h"
#include "Reports\Summary.h"
#include "Reports\Agents.h"
#include "Reports\AgentsDetailed.h"
#include "Reports\DealsHistory.h"
#include "Reports\Segregated.h"
#include "Reports\Daily.h"
#include "Reports\MarginCall.h"
#include "Reports\Equity.h"
#include "Reports\ExecutionType.h"
#include "Reports\TradeAccounts.h"
#include "Reports\TradeModification.h"
#include "Reports\PositionsHistory.h"
#include "Reports\FastProfitDeals.h"
#include "Tools\ReportFactory.h"
//+------------------------------------------------------------------+
//| Report factories array                                           |
//+------------------------------------------------------------------+
static const CReportFactory g_factories[]=
{
   CReportFactory::Create<CAgents>(),
   CReportFactory::Create<CCreditFacility>(),
   CReportFactory::Create<CDaily>(),
   CReportFactory::Create<CDailyDetailed>(),
   CReportFactory::Create<CDealsHistory>(),
   CReportFactory::Create<CDepositWithdrawal>(),
   CReportFactory::Create<CEquity>(),
   CReportFactory::Create<CMarginCall>(),
   CReportFactory::Create<COrdersHistory>(),
   CReportFactory::Create<CPositionsHistory>(),
   CReportFactory::Create<CSegregated>(),
   CReportFactory::Create<CSummary>(),
   CReportFactory::Create<CExecutionType>(),
   CReportFactory::Create<CTradeAccounts>(),
   CReportFactory::Create<CTradeModification>(),
   CReportFactory::Create<CAgentsDetailed>(),
   CReportFactory::Create<CStopOutCompensation>(),
   CReportFactory::Create<CFastProfitDeals>(),
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
