//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Copyright                                                         |
//+------------------------------------------------------------------+
#define Copyright             L"Copyright 2000-2025, MetaQuotes Ltd."
//+------------------------------------------------------------------+
//| Version / build / date of the program                            |
//+------------------------------------------------------------------+
#define ProgramVersion        500
#define ProgramBuild          100
#define ProgramBuildDate      L"05 Apr 2017"
#define ProgramName           L"Sample Gateway"
#define ProgramDescription    L"This is example of MetaTrader 5 gateway."
#ifdef _WIN64
#define ProgramModule         L"MT5GWTSMPL64"
#else
#define ProgramModule         L"MT5GWTSMPL"
#endif
//+------------------------------------------------------------------+
//| Common constants                                                 |
//+------------------------------------------------------------------+
#define STACK_SIZE_COMMON     1048576                  // size of thread stack
//+------------------------------------------------------------------+
