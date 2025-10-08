//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <cstdint>
#include "Classes\MT5APIStr.h"
#include "Classes\MT5APIMath.h"
#include "Classes\MT5APIFormat.h"
#include "Classes\MT5APITime.h"
#include "Classes\MT5APIStorage.h"
#include "Classes\MT5APISync.h"
#include "Classes\MT5APIThread.h"
#include "Classes\MT5APIProcess.h"
#include "Classes\MT5APIFile.h"
#include "Classes\MT5APIMemPack.h"
//--- C++11 and higher
#if __cplusplus>=201103L || (defined(_MSVC_LANG) && _MSVC_LANG>=201103L)
#include "Classes\MT5APIHashTable.h"
#include "Classes\MT5APIPtr.h"
#endif
//+------------------------------------------------------------------+
