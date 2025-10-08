//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Macros for calc member size                                      |
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
   int32_t           digits_column;    // >0 - id of column with digits value, <0 - digits count
   uint64_t          flags;            // flags
   //--- add table widget
   static IMTReportDashboardWidget* TableAdd(IMTReportAPI &api,MTAPIRES &res,IMTDataset *data,LPCWSTR title,LPCWSTR description=nullptr);
   //--- add report data columns
   static MTAPIRES   ReportColumnsAdd(IMTDataset &data,const ReportColumn *columns,uint32_t total);
   //--- fill report data column
   MTAPIRES          ReportColumnFill(IMTDatasetColumn &column) const;
  };
//+------------------------------------------------------------------+
//| Charts constants                                                 |
//+------------------------------------------------------------------+
enum EnChartConstants
  {
   CHART_HEIGHT_MAIN     =16,          // main chart height
   CHART_HEIGHT_PRIMARY  =13,          // primary chart height
   CHART_HEIGHT_SECONDARY=10,          // secondary chart height
  };
//+------------------------------------------------------------------+
