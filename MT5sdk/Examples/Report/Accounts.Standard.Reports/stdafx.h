//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
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
#include <windows.h>
#include <new.h>
#include <stddef.h>
#include <stdint.h>
#include <numeric>
//--- Resource.h
#include "resource.h"
//--- Report API
#include "..\..\..\Include\MT5APIReport.h"
#include "Tools\ReportError.h"
#include "Tools\ReportColumn.h"
//+------------------------------------------------------------------+
//| Charts constants                                                 |
//+------------------------------------------------------------------+
static constexpr uint32_t CHART_HEIGHT_PRIMARY  =13;   // primary chart height
static constexpr uint32_t CHART_HEIGHT_SECONDARY=10;   // secondary chart height
//+------------------------------------------------------------------+
