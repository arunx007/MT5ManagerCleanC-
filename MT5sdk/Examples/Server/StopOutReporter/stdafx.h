//+------------------------------------------------------------------+
//|                                          StopOut Reporter Plugin |
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
//---
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"
#include "..\..\..\Include\MT5APIServer.h"
//+------------------------------------------------------------------+
