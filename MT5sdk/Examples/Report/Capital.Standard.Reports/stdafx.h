//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once

#define WINVER               _WIN32_WINNT_WIN7
#define _WIN32_WINNT         _WIN32_WINNT_WIN7
#define _WIN32_WINDOWS       _WIN32_WINNT_WIN7
#define _WIN32_IE            _WIN32_IE_IE90
#define NTDDI_VERSION        NTDDI_WIN7

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
//---
#include <float.h>
#include <windows.h>
#include <shlwapi.h>
#include <stdint.h>
#pragma comment(lib,"shlwapi.lib")
#include <new.h>
#include <numeric>
//--- Resource.h
#include "resource.h"
//--- Report API
#include "..\..\..\Include\MT5APIReport.h"
//+------------------------------------------------------------------+
