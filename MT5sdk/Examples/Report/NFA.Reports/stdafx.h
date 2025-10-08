//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once

#define WINVER               0x0601          // Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINNT         0x0601          // Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINDOWS       0x0601          // Change this to the appropriate value to target Windows Me or later.
#define _WIN32_IE            0x0601          // Change this to the appropriate value to target other versions of IE.
//---
#define NOMINMAX
//---
#include <windows.h>
#include <numeric>
//--- API
#include "..\..\..\Include\MT5APIReport.h"
#include "..\..\..\Include\MT5APIServer.h"
//---
#include "Tools\Helpers.h"
#include "resource.h"
//+------------------------------------------------------------------+
//| Constants                                                        |
//+------------------------------------------------------------------+
#define COPYRIGHT L"Copyright 2000-2025, MetaQuotes Ltd."
//+------------------------------------------------------------------+
//| Report names                                                     |
//+------------------------------------------------------------------+
#define REPORT_ORDERBOOK         L"orderbook"
#define REPORT_TRADE             L"trade"
#define REPORT_CUSTOMER          L"customer"
#define REPORT_MANAGER           L"manager"
#define REPORT_CONCORDANCE       L"concordance"
#define REPORT_ADJUSTMENTS       L"adjustments"
#define REPORT_MARKET_EVENTS     L"market_events"
#define REPORT_TICK              L"tick"
//+------------------------------------------------------------------+
//| SL & TP database                                                 |
//+------------------------------------------------------------------+
#define SLTPBaseFile             L"sltp.dat"
#define SLTPBaseHeaderName       L"SLTP"
#define SLTPBaseHeaderVersion    500
//+------------------------------------------------------------------+
//| Macros of a size                                                 |
//+------------------------------------------------------------------+
#define KB uint32_t(1024)
#define MB uint32_t(1024*1024)
//+------------------------------------------------------------------+
//| Stack size for thread                                            |
//+------------------------------------------------------------------+
#define STACK_SIZE_THREAD        (2*MB)
//+------------------------------------------------------------------+
//| Module descriptor (from DllMain)                                 |
//+------------------------------------------------------------------+
extern HMODULE ExtModule;
//+------------------------------------------------------------------+
//| Common default plugin parameter                                  |
//+------------------------------------------------------------------+
#define DEFAULT_BASE_DIRECTORY   L"NFA.Reports"
//+------------------------------------------------------------------+