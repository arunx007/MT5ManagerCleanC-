//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#define WINVER               _WIN32_WINNT_WIN7
#define _WIN32_WINNT         _WIN32_WINNT_WIN7
#define _WIN32_WINDOWS       _WIN32_WINNT_WIN7
#define _WIN32_IE            _WIN32_IE_IE90
#define NTDDI_VERSION        NTDDI_WIN7

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NOMINMAX
//--- standard inclusions
#include <stdio.h>
#include <tchar.h>
#include <stddef.h>
#include <time.h>
#include <malloc.h>
#include <new.h>
#include <Winsock2.h>
//--- API
#include "..\..\..\Include\MT5APIGateway.h"
//--- common
#include "Common\const.h"
#include "Common\Logger.h"
//+------------------------------------------------------------------+
