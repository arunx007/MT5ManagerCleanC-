//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
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
//---
#include <stdio.h>
#include <stddef.h>
#include <malloc.h>
#include <conio.h>
#include <time.h>
#include <math.h>
#include <new.h>
#include <wchar.h>
#include <process.h>
#include <Winsock2.h>
#include <numeric>
//--- resources
#include "resource.h"
//--- for using MT5 tools
#include "..\..\..\Include\MT5APIGateway.h"
//--- tools
#include "Tools\MIMEConverter.h"
#include "Tools\SHA256.h"
//--- common
#include "Common\const.h"
#include "Common\Logger.h"
//--- universal news protocol
#include "UniNewsAPI\Common\UniNewsProtocol.h"
//--- news data source
#include "UniNewsAPI\Bases\UniNewsSource.h"
//--- universal news server port
#include "UniNewsAPI\Contexts\UniNewsServerPort.h"
//+------------------------------------------------------------------+
