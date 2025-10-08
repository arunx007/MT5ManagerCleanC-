//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
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
   static IMTReportDashboardWidget* TableAdd(IMTReportAPI &api,MTAPIRES &res,IMTDataset *data,LPCWSTR title);
   //--- create dataset from array
   static IMTDataset* DatasetFromArray(IMTReportAPI &api,MTAPIRES &res,const ReportColumn *columns,uint32_t columns_total,CMTArrayBase &arr);
   //--- add report data columns
   static MTAPIRES   ReportColumnsAdd(IMTDataset &data,const ReportColumn *columns,uint32_t columns_total);
   //--- prepare report table
   static MTAPIRES   ReportTablePrepare(IMTReportAPI &api,const ReportColumn *columns,uint32_t columns_total);
   //--- fill report data column
   MTAPIRES          ReportColumnFill(IMTDatasetColumn &column) const;
  };
//+------------------------------------------------------------------+
