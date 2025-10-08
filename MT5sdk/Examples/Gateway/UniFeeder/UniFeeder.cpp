//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MTDatafeedApp.h"
//+------------------------------------------------------------------+
//| Starting banner                                                  |
//+------------------------------------------------------------------+
void Banner(void)
  {
//--- show the starting banner
   wprintf_s(L"%s %d.%02d  build %d, %s\n"
             L"Copyright 2000-2025, MetaQuotes Ltd.\n",
             ProgramName,ProgramVersion/100,ProgramVersion%100,ProgramBuild,ProgramBuildDate);
  }
//+------------------------------------------------------------------+
//| Entry point                                                      |
//+------------------------------------------------------------------+
int32_t wmain(int32_t argc,wchar_t** argv)
  {
   CMTDatafeedApp datafeed;
//--- show the banner
   Banner();
//--- initialize the datafeed
   if(!datafeed.Initialize(argc,argv))
      return(-1);
//--- start the work of the datafeed
   datafeed.Run();
//--- complete the work of the datafeed
   datafeed.Shutdown();
//--- exit
   return(0);
  }
//+------------------------------------------------------------------+
