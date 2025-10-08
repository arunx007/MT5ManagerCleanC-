//+------------------------------------------------------------------+
//|                                          StopOut Reporter Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
extern HMODULE ExtModule=NULL;
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
MTPluginInfo ExtPluginInfo=
  {
   100,
   MTServerAPIVersion,
   L"StopOut Reporter Plugin",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"Plugin for notification by email about Margin Calls and Stop Outs"
   };
//+------------------------------------------------------------------+
//| Plugin default parameters                                        |
//+------------------------------------------------------------------+
MTPluginParam ExtPluginDefaults[]=
  {
     {   MTPluginParam::TYPE_INT,   L"Period (min)",              L"10"                           },
   {   MTPluginParam::TYPE_GROUPS,L"Groups",                    L"*,!demo*,!contest*,!manager*" },
   {   MTPluginParam::TYPE_INT,   L"Pause (hours)",             L"24"                           },
   {   MTPluginParam::TYPE_STRING,L"From",                      L"address@yourserver.com"       },
   {   MTPluginParam::TYPE_STRING,L"Copy To",                   L""                             },
   {   MTPluginParam::TYPE_STRING,L"SMTP Server",               L""                             },
   {   MTPluginParam::TYPE_STRING,L"SMTP Login",                L"smtp-login"                   },
   {   MTPluginParam::TYPE_STRING,L"SMTP Password",             L"smtp-password"                },
  };
//+------------------------------------------------------------------+
//| DLL Main function                                                |
//+------------------------------------------------------------------+
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
  {
   switch(ul_reason_for_call)
     {
      case DLL_PROCESS_ATTACH:
         ExtModule=(HMODULE)hModule;
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
//--- адрес есть?
   if(!plugin) return(MT_RET_ERR_PARAMS);
//--- создадим новый экземпл€р
   if(((*plugin)=new(std::nothrow) CPluginInstance(ExtModule))==NULL)
      return(MT_RET_ERR_MEM);
//---
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
