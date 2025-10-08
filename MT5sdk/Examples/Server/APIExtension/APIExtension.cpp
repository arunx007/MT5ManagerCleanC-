//+------------------------------------------------------------------+
//|                                       MetaTrader 5 API Extension |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTPluginInfo ExtPluginInfo=
  {
   100,
   MTServerAPIVersion,
   L"API Extension",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"This is MetaTrader 5 Server API plugin example"
   };
//+------------------------------------------------------------------+
//| Dll Main Function                                               |
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
//| Plugin About entry function                                      |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTServerAbout(MTPluginInfo& info)
  {
   info=ExtPluginInfo;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin instance creation entry point                             |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTServerCreate(uint32_t apiversion,IMTServerPlugin **plugin)
  {
//--- check parameters
   if(!plugin) return(MT_RET_ERR_PARAMS);
//--- create plugin object
   if(((*plugin)=new(std::nothrow) CPluginInstance())==NULL)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+