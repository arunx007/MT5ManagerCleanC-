//+------------------------------------------------------------------+
//|                                           Feed Commission Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
MTPluginInfo ExtPluginInfo=
  {
   100,
   MTServerAPIVersion,
   L"Feed Commission Plugin",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"Plugin for taking a monthly charge for use of the symbol"
   };
//+------------------------------------------------------------------+
//| Plugin default parameters                                        |
//+------------------------------------------------------------------+
MTPluginParam ExtPluginDefaults[]=
  {
     { MTPluginParam::TYPE_SYMBOLS, L"Symbols"               ,L"*"                            },
   { MTPluginParam::TYPE_GROUPS,  L"Groups"                ,L"*,!demo*,!contest*,!manager*" },
   { MTPluginParam::TYPE_FLOAT,   L"Monthly cost"          ,L"5"                            },
   { MTPluginParam::TYPE_SYMBOLS, L"Currency"              ,L"USD"                          },
   { MTPluginParam::TYPE_BOOL,    L"Skip disabled users"   ,L"True"                         },
   { MTPluginParam::TYPE_FLOAT,   L"Max deposit to charge" ,L"10000"                        },
   { MTPluginParam::TYPE_FLOAT,   L"Max overturn to charge",L"100000"                       },
  };
//+------------------------------------------------------------------+
//| DLL Main function                                                |
//+------------------------------------------------------------------+
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
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
   if(((*plugin)=new(std::nothrow) CPluginInstance())==NULL)
      return(MT_RET_ERR_MEM);
//---
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
