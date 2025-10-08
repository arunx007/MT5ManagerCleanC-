//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MoneyFlowDailyReport.h"
//+------------------------------------------------------------------+
//| Column description money                                         |
//+------------------------------------------------------------------+
ReportColumn CMoneyFlowDaily::s_columns_money[9]=
//--- id,name,                               type,                          width, width_max, offset,                                                  size,digits_column,flags
  {{ 1,  L"Date",                            IMTDatasetColumn::TYPE_DATE   ,20,0, offsetof(MoneyRow,date)                                             ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Deposits Amount",                 IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,in_amount)         ,0,0,0                              },
   { 3,  L"Withdrawals Amount",              IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,out_amount)        ,0,0,0                              },
   { 4,  L"Deposits Count",                  IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,in_count)          ,0,0,0                              },
   { 5,  L"Withdrawals Count",               IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,out_count)         ,0,0,0                              },
   { 6,  L"Deposits Average",                IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,in_medium)         ,0,0,0                              },
   { 7,  L"Withdrawals Average",             IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,out_medium)        ,0,0,0                              },
   { 8,  L"Deposits-Withdrawals",            IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,in_out_delta)      ,0,0,0                              },
   { 9,  L"Deposits-Withdrawals Accumulated",IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(MoneyRow,sections)+offsetof(MoneyRecord,in_out_delta_accum),0,0,0                              },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMoneyFlowDaily::CMoneyFlowDaily(const MTReportInfo &info,const bool line_chart,const uint32_t days_week,const uint32_t days_months) :
   CCurrencyReport(info),m_line_chart(line_chart),m_interval(days_week,days_months)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMoneyFlowDaily::~CMoneyFlowDaily(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CMoneyFlowDaily::Clear(void)
  {
//--- clear interval
   m_interval.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDaily::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initalize interval
   MTAPIRES res=m_interval.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDaily::PrepareGraphs(void)
  {
//--- double check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare money data
   MTAPIRES res=PrepareData(*data);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare money graphs
   for(uint32_t i=1;i<_countof(s_columns_money);i++)
      if((res=PrepareGraph(data,s_columns_money[i]))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Money Graph                                              |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDaily::PrepareGraph(IMTDataset *data,const ReportColumn &column)
  {
//--- checks
   if(!m_api || !data)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- widget height
   MTAPIRES res=chart->Height(CHART_HEIGHT_PRIMARY);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare chart
   if(m_line_chart || column.id>=6)
     {
      //--- line chart
      if((res=chart->Type(column.id==8 ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE : IMTReportDashboardWidget::WIDGET_TYPE_CHART_SPLINE))!=MT_RET_OK)
         return(res);
      //--- hide zero values
      if(column.id<8)
         if((res=chart->Flags(chart->Flags()|IMTReportDashboardWidget::WIDGET_FLAG_HIDE_ZEROES))!=MT_RET_OK)
            return(res);
     }
   else
     {
      //--- stacked area chart
      if((res=chart->Type(IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA))!=MT_RET_OK)
         return(res);
      if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
         return(res);
     }
//--- append currency to column name
   CMTStr256 str(column.name);
   if(column.type==IMTDatasetColumn::TYPE_MONEY)
      m_currency.Append(str);
//--- set title
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add data column
   return(ChartColumnsAdd(*chart,column));
  }
//+------------------------------------------------------------------+
//| Fill record                                                      |
//+------------------------------------------------------------------+
void CMoneyFlowDaily::FillRecord(MoneyRecord &record,const Section &section,double *in_out_delta_accum) const
  {
//--- fill record by section
   record.in_amount  =section.in.amount;
   record.in_count   =section.in.total;
   record.in_medium  =section.in.Medium(m_currency);
   record.out_amount =section.out.amount;
   record.out_count  =section.out.total;
   record.out_medium =section.out.Medium(m_currency);
   record.in_out_delta=section.InOutAmountDelta(in_out_delta_accum);
   record.in_out_delta_accum=in_out_delta_accum ? *in_out_delta_accum : NAN;
  }
//+------------------------------------------------------------------+
