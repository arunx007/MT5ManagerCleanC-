//+------------------------------------------------------------------+
//|                           MetaTrader 5 Gateways.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Reports\Turnover.h"
#include "Reports\Profit.h"
#include "Reports\WhiteLabel.h"
//+------------------------------------------------------------------+
//|                                                                  |
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
//| Plug-in About entry function                                     |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportAbout(const uint32_t index,MTReportInfo& info)
  {
//--- Turnover Report
   if(index==CTurnover::Index())
     {
      //--- get report info
      CTurnover::Info(info);
      //--- ok
      return(MT_RET_OK);
     }
//--- Profit Report
   if(index==CProfit::Index())
     {
      //--- get report info
      CProfit::Info(info);
      //--- ok
      return(MT_RET_OK);
     }
//--- White Label Report
   if(index==CWhiteLabel::Index())
     {
      //--- get report info
      CWhiteLabel::Info(info);
      //--- ok
      return(MT_RET_OK);
     }
//--- not found
   return(MT_RET_ERR_NOTFOUND);
  }
//+------------------------------------------------------------------+
//| Plug-in instance creation entry point                            |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES MTReportCreate(const uint32_t index,const uint32_t apiversion,IMTReportContext **context)
  {
//--- checks
   if(!context) return(MT_RET_ERR_PARAMS);
//--- Turnover Report
   if(index==CTurnover::Index())
     {
      //--- create instance
      if((*context=new(std::nothrow) CTurnover())==NULL)
         return(MT_RET_ERR_MEM);
      //--- ok
      return(MT_RET_OK);
     }
//--- Profit Report
   if(index==CProfit::Index())
     {
      //--- create instance
      if((*context=new(std::nothrow) CProfit())==NULL)
         return(MT_RET_ERR_MEM);
      //--- ok
      return(MT_RET_OK);
     }
//--- White Label Report
   if(index==CWhiteLabel::Index())
     {
      //--- create instance
      if((*context=new(std::nothrow) CWhiteLabel())==NULL)
         return(MT_RET_ERR_MEM);
      //--- ok
      return(MT_RET_OK);
     }
//--- not found
   return(MT_RET_ERR_NOTFOUND);
  }
//+------------------------------------------------------------------+
