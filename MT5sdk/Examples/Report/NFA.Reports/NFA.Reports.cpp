//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Plugin\PluginInstance.h"
#include "Reports\OrderbookReport.h"
#include "Reports\TradeReport.h"
#include "Reports\CustomerReport.h"
#include "Reports\ManagerReport.h"
#include "Reports\ConcordanceReport.h"
#include "Reports\AdjustmentsReport.h"
#include "Reports\MarketEventsReport.h"
#include "Reports\TickReport.h"
//+------------------------------------------------------------------+
//| Module handle                                                    |
//+------------------------------------------------------------------+
HMODULE           ExtModule=NULL;
//+------------------------------------------------------------------+
//| Plugin about information                                         |
//+------------------------------------------------------------------+
MTPluginInfo ExtPluginInfo=
  {
   100,
   MTServerAPIVersion,
   L"NFA Reports",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"Plugin for collect information for National Futures Association Report"
   };
//+------------------------------------------------------------------+
//| Plugin default parameters                                        |
//+------------------------------------------------------------------+
MTPluginParam ExtPluginDefaults[]=
  {{ MTPluginParam::TYPE_SYMBOLS ,L"Symbols"                ,DEFAULT_SYMBOLS       },
   { MTPluginParam::TYPE_GROUPS  ,L"Groups"                 ,DEFAULT_GROUPS        },
   { MTPluginParam::TYPE_STRING  ,L"Forex Dealer Member ID" ,DEFAULT_FDM           },
   { MTPluginParam::TYPE_STRING  ,L"Server ID"              ,DEFAULT_SERVER_ID     },
   { MTPluginParam::TYPE_STRING  ,L"Base Directory"         ,DEFAULT_BASE_DIRECTORY},
   { MTPluginParam::TYPE_STRING  ,L"SFTP Address"           ,DEFAULT_SFTP_ADDRESS  },
   { MTPluginParam::TYPE_STRING  ,L"SFTP Port"              ,DEFAULT_SFTP_PORT     },
   { MTPluginParam::TYPE_STRING  ,L"SFTP Login"             ,DEFAULT_SFTP_LOGIN    },
   { MTPluginParam::TYPE_STRING  ,L"SFTP Password"          ,DEFAULT_SFTP_PASS     },
   { MTPluginParam::TYPE_STRING  ,L"SFTP Private Key Path"  ,DEFAULT_SFTP_KEY_PATH },
   { MTPluginParam::TYPE_TIME    ,L"Report Time"            ,DEFAULT_REPORT_TIME   },
   { MTPluginParam::TYPE_TIME    ,L"Send Time"              ,DEFAULT_SEND_TIME     },
  };
//+------------------------------------------------------------------+
//+ Report context indexes                                           |                
//+------------------------------------------------------------------+
enum EnNFAReports
  {
   NFA_REPORT_INDEX_ORDERBOOK    =0,
   NFA_REPORT_INDEX_TRADE        =1,
   NFA_REPORT_INDEX_CUSTOMER     =2,
   NFA_REPORT_INDEX_TICK         =3,
   NFA_REPORT_INDEX_ADJUSTMENTS  =4,
   NFA_REPORT_INDEX_MANAGER      =5,
   NFA_REPORT_INDEX_CONCORDANCE  =6,
   NFA_REPORT_INDEX_MARKET_EVENTS=7,
  };
//+------------------------------------------------------------------+
//| DLL Main Function                                                |
//+------------------------------------------------------------------+
BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
  {
   switch(ul_reason_for_call)
     {
      case DLL_PROCESS_ATTACH:
         ExtModule=hModule;
         break;
      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
      case DLL_PROCESS_DETACH:
         break;
     }
   return(TRUE);
  }
//+------------------------------------------------------------------+
//| Report About entry function                                      |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportAbout(const uint32_t index,MTReportInfo& info)
  {
//--- return MT_RET_OK for supported reports
   switch(index)
     {
      case NFA_REPORT_INDEX_ORDERBOOK    : return(COrderbookReport   ::Info(info));
      case NFA_REPORT_INDEX_TRADE        : return(CTradeReport       ::Info(info));
      case NFA_REPORT_INDEX_CUSTOMER     : return(CCustomerReport    ::Info(info));
      case NFA_REPORT_INDEX_TICK         : return(CTickReport        ::Info(info));
      case NFA_REPORT_INDEX_ADJUSTMENTS  : return(CAdjustmentsReport ::Info(info));
      case NFA_REPORT_INDEX_MANAGER      : return(CManagerReport     ::Info(info));
      case NFA_REPORT_INDEX_CONCORDANCE  : return(CConcordanceReport ::Info(info));
      case NFA_REPORT_INDEX_MARKET_EVENTS: return(CMarketEventsReport::Info(info));
     }
//--- report type not supported
   return(MT_RET_ERR_NOTFOUND);
  }
//+------------------------------------------------------------------+
//| Report instance creation entry point                             |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportCreate(const uint32_t index,const uint32_t apiversion,IMTReportContext **context)
  {
//--- creation instances for supported reports
   switch(index)
     {
      case NFA_REPORT_INDEX_ORDERBOOK    : return(*context=new (std::nothrow) COrderbookReport())    ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_TRADE        : return(*context=new (std::nothrow) CTradeReport())        ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_CUSTOMER     : return(*context=new (std::nothrow) CCustomerReport())     ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_TICK         : return(*context=new (std::nothrow) CTickReport())         ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_ADJUSTMENTS  : return(*context=new (std::nothrow) CAdjustmentsReport())  ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_MANAGER      : return(*context=new (std::nothrow) CManagerReport())      ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_CONCORDANCE  : return(*context=new (std::nothrow) CConcordanceReport())  ? MT_RET_OK : MT_RET_ERR_MEM;
      case NFA_REPORT_INDEX_MARKET_EVENTS: return(*context=new (std::nothrow) CMarketEventsReport()) ? MT_RET_OK : MT_RET_ERR_MEM;
      default:
         break;
     }
//--- report type not supported
   return(MT_RET_ERR_NOTFOUND);
  }
//+------------------------------------------------------------------+
//| Plugin About entry function                                      |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTServerAbout(MTPluginInfo& info)
  {
//--- copy description
   info=ExtPluginInfo;
//--- copy default parameters values
   memcpy(info.defaults,ExtPluginDefaults,sizeof(ExtPluginDefaults));
   info.defaults_total=_countof(ExtPluginDefaults);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin instance creation entry point                             |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTServerCreate(uint32_t apiversion,IMTServerPlugin **plugin)
  {
//--- checks
   if(!plugin) return(MT_RET_ERR_PARAMS);
//--- create new instance
   if(((*plugin)=new(std::nothrow) CPluginInstance())==NULL)
      return(MT_RET_ERR_MEM);
//---
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+