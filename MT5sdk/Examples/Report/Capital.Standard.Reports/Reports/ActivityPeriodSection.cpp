//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ActivityPeriodSection.h"
//+------------------------------------------------------------------+
//| Column description inactive clients                              |
//+------------------------------------------------------------------+
ReportColumn CActivityPeriodSection::s_columns_inactive[]=
  {
   //--- id,name,  type,                           width, width_max,offset,               size,digits_column,flags
     { 1, L"Date"   ,IMTDatasetColumn::TYPE_DATE    ,20,0, offsetof(ActivityRecord,date)   ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Count"  ,IMTDatasetColumn::TYPE_UINT32  ,20,0, offsetof(ActivityRecord,count)  ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CActivityPeriodSection::CActivityPeriodSection(const CReportInterval &interval) :
   m_interval(interval),m_inactive(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CActivityPeriodSection::~CActivityPeriodSection(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CActivityPeriodSection::Clear(void)
  {
//--- clear objects
   m_inactive.Clear();
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
MTAPIRES CActivityPeriodSection::Initialize(void)
  {
//--- initialize inactive clients vector
   if(!m_inactive.Initialize(m_interval.PeriodTotal()))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add inactive client                                              |
//+------------------------------------------------------------------+
MTAPIRES CActivityPeriodSection::InactiveAdd(const int64_t time)
  {
//--- period index by time
   int32_t period=m_interval.PeriodIndex(time);
   if(period<0)
      period=0;
//--- get counter by section
   uint32_t *counter=m_inactive.Item(period);
   if(!counter)
      return(MT_RET_ERR_MEM);
//--- count client
   (*counter)++;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CActivityPeriodSection::PrepareGraphs(IMTReportAPI &api,LPCWSTR title,const uint32_t y)
  {
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare inactive clients data
   MTAPIRES res=PrepareDataInactive(*data);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graph count by month
   if((res=PrepareGraph(api,data,title,y))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare inactive clients data                                    |
//+------------------------------------------------------------------+
MTAPIRES CActivityPeriodSection::PrepareDataInactive(IMTDataset &data) const
  {
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_inactive,_countof(s_columns_inactive));
   if(res!=MT_RET_OK)
      return(res);
//--- iterate over each month
   ActivityRecord row={};
   for(uint32_t i=0,total=m_inactive.Total();i<total;i++)
     {
      //--- date
      row.date=m_interval.PeriodDate(i);
      //--- count
      const uint32_t *count=m_inactive.Item(i);
      row.count=count ? *count : 0;
      //--- write data row
      if((res=data.RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CActivityPeriodSection::PrepareGraph(IMTReportAPI &api,IMTDataset *data,LPCWSTR title,const uint32_t y)
  {
//--- checks
   if(!data || !title)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=api.DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE);
   if(res!=MT_RET_OK)
      return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- set top position
   if((res=chart->Top(CHART_HEIGHT_PRIMARY*y))!=MT_RET_OK)
      return(res);
//--- set height
   if((res=chart->Height(CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
