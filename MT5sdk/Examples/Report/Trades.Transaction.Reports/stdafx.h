//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
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
//--- Report API
#include "..\..\..\Include\MT5APIReport.h"
#include "..\..\..\Include\MT5APIServer.h"
//+------------------------------------------------------------------+
//| Constants                                                        |
//+------------------------------------------------------------------+
#define Copyright L"Copyright 2000-2025, MetaQuotes Ltd."
//+------------------------------------------------------------------+
//| Macros of a size                                                 |
//+------------------------------------------------------------------+
#define KB uint32_t(1024)
#define MB uint32_t(1024*1024)
//+------------------------------------------------------------------+
//| Macros for calculation member size                               |
//+------------------------------------------------------------------+
#define MtFieldSize(type,member) (sizeof(((type*)(0))->member))
//+------------------------------------------------------------------+
//| Report column description (description of record field)          |
//+------------------------------------------------------------------+
struct ReportColumn
  {
   uint32_t          id;               // unique column id (must be greater than 0)
   LPCWSTR           name;             // visible column title
   uint32_t          type;             // data type and formating of column
   uint32_t          width;            // relative width
   uint32_t          width_max;        // max width in pixel
   uint32_t          offset;           // field offset
   uint32_t          size;             // size in bytes for strings
   uint32_t          digits_column;    // id of column with digits value
   uint64_t          flags;            // flags
  };
//+------------------------------------------------------------------+
//| Default params                                                   |
//+------------------------------------------------------------------+
#define DEFAULT_BASE_DIRECTORY L"Trade Transaction Report"
#define DEFAULT_SHOW_OWNER     L"True"
#define DEFAULT_SHOW_DAILY     L"True"
#define DEFAULT_SHOW_REASON    L"False"
#define DEFAULT_SHOW_RETCODE   L"False"
#define DEFAULT_SHOW_REJECTED  L"False"
//+------------------------------------------------------------------+