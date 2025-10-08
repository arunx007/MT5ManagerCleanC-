//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Macros for member size calculation                               |
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
   uint32_t          digits;           // id of column with digits value
   uint64_t          flags;            // flags
   //--- add report data columns
   static MTAPIRES   ReportColumnsAdd(IMTReportAPI *api,const ReportColumn *columns,const uint32_t total);
   //--- fill report data column
   MTAPIRES          ReportColumnFill(IMTDatasetColumn &column) const;
  };
//+------------------------------------------------------------------+
