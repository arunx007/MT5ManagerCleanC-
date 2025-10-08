//+------------------------------------------------------------------+
//|                           MetaTrader 5 Gateways.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once

#define WINVER               0x0601          // Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINNT         0x0601          // Change this to the appropriate value to target other versions of Windows.
#define _WIN32_WINDOWS       0x0601          // Change this to the appropriate value to target Windows Me or later.
#define _WIN32_IE            0x0601          // Change this to the appropriate value to target other versions of IE.

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
//---
#include <stdint.h>
#include <windows.h>
#include <new.h>
#include <stddef.h>
#include <numeric>
//--- Resource.h
#include "resource.h"
//--- Report API
#include "..\..\..\Include\MT5APIReport.h"
//--- own includes
#include "Tools\GatewayUtils.h"
#include "Tools\ReportError.h"
//+------------------------------------------------------------------+
//| Default currency                                                 |
//+------------------------------------------------------------------+
#define DEFAULT_CURRENCY L"USD"
//+------------------------------------------------------------------+
