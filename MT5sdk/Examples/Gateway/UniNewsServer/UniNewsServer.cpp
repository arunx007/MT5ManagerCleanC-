//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MTUniNewsServerApp.h"
//+------------------------------------------------------------------+
//| Entry point                                                      |
//+------------------------------------------------------------------+
int32_t wmain(int32_t argc,wchar_t** argv)
  {
//--- display banner
   wprintf_s(L"%s build %d, %s\n"
             L"Copyright 2000-2025, MetaQuotes Ltd.\n",
             ProgramName,ProgramBuild,ProgramBuildDate);
//--- initialize application
   CMTUniNewsServerApp app;
   if(!app.Initialize(argc,argv))
      return(-1);
//--- start application
   app.Run();
//--- exit
   return(0);
  }
//+------------------------------------------------------------------+
