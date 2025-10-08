//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
#include "TradeTransactionReport.h"
//+------------------------------------------------------------------+
//| Plugin about information                                         |
//+------------------------------------------------------------------+
MTPluginInfo ExtPluginInfo=
  {
   100,
   MTServerAPIVersion,
   L"Trade Transaction Report",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"Plugin for collect information for Trades.Transaction.Reports"
   };
//+------------------------------------------------------------------+
//| Plugin default parameters                                        |
//+------------------------------------------------------------------+
MTPluginParam ExtPluginDefaults[]=
  {{ MTPluginParam::TYPE_GROUPS ,L"Groups"           ,DEFAULT_GROUPS        },
   { MTPluginParam::TYPE_STRING ,L"Base Directory"   ,DEFAULT_BASE_DIRECTORY},
   { MTPluginParam::TYPE_SYMBOLS,L"Symbols"          ,DEFAULT_SYMBOLS       },
   { MTPluginParam::TYPE_STRING ,L"Daily Report Path",DEFAULT_DAILY_PATH    },
  };
//+------------------------------------------------------------------+
//| DLL Main Function                                                |
//+------------------------------------------------------------------+
BOOL APIENTRY DllMain(HMODULE hModule,DWORD ul_reason_for_call,LPVOID lpReserved)
  {
   switch(ul_reason_for_call)
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
//| Report About entry function                                      |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportAbout(const uint32_t index,MTReportInfo& info)
  {
//--- get report info
   if(index==0)
     {
      CTradeTransactionReport::Info(info);
      return(MT_RET_OK);
     }
//--- not found
   return(MT_RET_ERR_NOTFOUND);
  }
//+------------------------------------------------------------------+
//| Report  instance creation entry point                            |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportCreate(const uint32_t index,const uint32_t apiversion,IMTReportContext **context)
  {
//--- create instance
   if(index==0)
     {
      if((*context=new(std::nothrow) CTradeTransactionReport())==NULL)
         return(MT_RET_ERR_MEM);
      return(MT_RET_OK);
     }
//--- not found
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
   memcpy(info.defaults, ExtPluginDefaults, sizeof(ExtPluginDefaults));
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
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+