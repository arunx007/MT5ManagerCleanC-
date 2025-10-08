//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportColumn.h"
//+------------------------------------------------------------------+
//| add table widget                                                 |
//+------------------------------------------------------------------+
IMTReportDashboardWidget* ReportColumn::TableAdd(IMTReportAPI &api,MTAPIRES &res,IMTDataset *data,LPCWSTR title)
  {
//--- checks
   if(!data || !title)
     {
      res=MT_RET_ERR_PARAMS;
      return(nullptr);
     }
//--- add widget
   IMTReportDashboardWidget *table=api.DashboardWidgetAppend();
   if(!table)
     {
      res=MT_RET_ERR_PARAMS;
      return(nullptr);
     }
//--- set widget type
   if((res=table->Type(IMTReportDashboardWidget::WIDGET_TYPE_TABLE))!=MT_RET_OK)
      return(nullptr);
//--- set title
   if((res=table->Title(title))!=MT_RET_OK)
      return(nullptr);
//--- add dataset
   if((res=table->Data(data))!=MT_RET_OK)
      return(nullptr);
//--- ok
   res=MT_RET_OK;
   return(table);
  }
//+------------------------------------------------------------------+
//| create dataset from array                                        |
//+------------------------------------------------------------------+
IMTDataset* ReportColumn::DatasetFromArray(IMTReportAPI &api,MTAPIRES &res,const ReportColumn *columns,const uint32_t columns_total,CMTArrayBase &arr)
  {
//--- checks
   if(columns==NULL || columns_total==0)
     {
      res=MT_RET_ERR_PARAMS;
      return(nullptr);
     }
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
     {
      res=MT_RET_ERR_MEM;
      return(nullptr);
     }
//--- create dataset columns
   if((res=ReportColumnsAdd(*data,columns,columns_total))!=MT_RET_OK)
      return(nullptr);
//--- write rows
   for(uint32_t i=0,total=arr.Total();i<total;i++)
      if((res=data->RowWrite(arr.At(i),arr.Width()))!=MT_RET_OK)
         return(nullptr);
//--- ok
   res=MT_RET_OK;
   return(data);
  }
//+------------------------------------------------------------------+
//| add report data columns                                          |
//+------------------------------------------------------------------+
MTAPIRES ReportColumn::ReportColumnsAdd(IMTDataset &data,const ReportColumn *columns,const uint32_t columns_total)
  {
//--- checks
   if(!columns || !columns_total)
      return(MT_RET_ERR_PARAMS);
//--- create column
   IMTDatasetColumn *column=data.ColumnCreate();
   if(!column)
      return(MT_RET_ERR_MEM);
//--- iterate columns
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0;i<columns_total;i++)
     {
      //--- fill report data column
      if((res=columns[i].ReportColumnFill(*column))!=MT_RET_OK)
         break;
      //--- add report data column
      if((res=data.ColumnAdd(column))!=MT_RET_OK)
         break;
     }
//--- release column
   column->Release();
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare report table                                             |
//+------------------------------------------------------------------+
MTAPIRES ReportColumn::ReportTablePrepare(IMTReportAPI &api,const ReportColumn *columns,const uint32_t columns_total)
  {
//--- checks
   if(!columns || !columns_total)
      return(MT_RET_ERR_PARAMS);
//--- create column
   IMTDatasetColumn *column=api.TableColumnCreate();
   if(!column)
      return(MT_RET_ERR_MEM);
//--- clear table columns
   api.TableColumnClear();
//--- add column description
   MTAPIRES res=MT_RET_OK;
   for(uint32_t i=0;i<columns_total;i++)
     {
      //--- fill report data column
      if((res=columns[i].ReportColumnFill(*column))!=MT_RET_OK)
         break;
      //--- add column
      if((res=api.TableColumnAdd(column))!=MT_RET_OK)
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
   if(digits_column>0)
     {
      if((res=column.DigitsColumn(uint32_t(digits_column)))!=MT_RET_OK)
         return(res);
     }
   else
      if(digits_column<0)
        {
         if((res=column.Digits(uint32_t(-digits_column)))!=MT_RET_OK)
            return(res);
        }
      else
         if(type==IMTDatasetColumn::TYPE_DOUBLE)
           {
            if((res=column.Digits(2))!=MT_RET_OK)
               return(res);
           }
   if((res=column.Flags(flags))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
