//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
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
//--- default params
#define DEFAULT_CURRENCY           L"USD"
#ifdef _DEBUG
#define DEFAULT_GROUPS             L"*"
#else
#define DEFAULT_GROUPS             L"*,!demo*,!contest*"
#endif
//---
#include <windows.h>
#include <new.h>
#include <stdint.h>
#include <numeric>
//--- Resource.h
#include "resource.h"
//--- Report API
#include "..\..\..\Include\MT5APIReport.h"
#include "Tools\ReportError.h"
#include "Tools\ReportColumn.h"
//+------------------------------------------------------------------+
