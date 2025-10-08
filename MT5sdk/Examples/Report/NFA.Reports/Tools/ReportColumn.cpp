//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportColumn.h"
//+------------------------------------------------------------------+
//| add report data columns                                          |
//+------------------------------------------------------------------+
MTAPIRES ReportColumn::ReportColumnsAdd(IMTReportAPI *api,const ReportColumn *columns,const uint32_t total)
  {
//--- checks
   if(!api || !columns || !total)
      return(MT_RET_ERR_PARAMS);
//--- create column
   IMTDatasetColumn *column=api->TableColumnCreate();
   if(!column)
      return(MT_RET_ERR_MEM);
//--- iterate columns
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0;i<total;i++)
     {
      //--- fill report data column
      if((res=columns[i].ReportColumnFill(*column))!=MT_RET_OK)
         break;
      //--- add report data column
      if((res=api->TableColumnAdd(column))!=MT_RET_OK)
         break;
     }
//--- release column
   column->Release();
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| fill report data column                                          |
//+------------------------------------------------------------------+
MTAPIRES ReportColumn::ReportColumnFill(IMTDatasetColumn &column) const
  {
//--- clear
   MTAPIRES res;
   if((res=column.Clear())!=MT_RET_OK)
      return(res);
//--- fill info
   if((res=column.ColumnID(id))!=MT_RET_OK)
      return(res);
   if((res=column.Name(name))!=MT_RET_OK)
      return(res);
   if((res=column.Type(type))!=MT_RET_OK)
      return(res);
   if((res=column.Width(width))!=MT_RET_OK)
      return(res);
   if((res=column.WidthMax(width_max))!=MT_RET_OK)
      return(res);
   if((res=column.Offset(offset))!=MT_RET_OK)
      return(res);
   if((res=column.Size(size))!=MT_RET_OK)
      return(res);
   if((res=column.Digits(digits))!=MT_RET_OK)
      return(res);
   if((res=column.Flags(flags))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
