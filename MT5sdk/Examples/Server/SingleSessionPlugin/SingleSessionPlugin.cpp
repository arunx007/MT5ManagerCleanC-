//+------------------------------------------------------------------+
//|                               MetaTrader 5 Single Session Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTPluginInfo ExtPluginInfo=
  {
   100,
   MTServerAPIVersion,
   L"Single Session Plugin",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"This is MetaTrader 5 Server API Single Session Plugin"
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
//| Plugin About entry function                                      |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTServerAbout(MTPluginInfo& info)
  {
//--- copy information
   info=ExtPluginInfo;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin instance creation entry point                             |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTServerCreate(uint32_t apiversion,IMTServerPlugin **plugin)
  {
//--- check parameters
   if(!plugin) return(MT_RET_ERR_PARAMS);
//--- create plugin instance
   if(((*plugin)=new(std::nothrow) CPluginInstance())==NULL)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
