//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealReport.h"
//+------------------------------------------------------------------+
//| Column description                                               |
//+------------------------------------------------------------------+
ReportColumn CDealReport::s_columns[12]=
//--- id,              name,                          type,                           width, width_max, offset,                                size,digits_column,flags
  {{ COLUMN_DATE      ,L"Time",                       IMTDatasetColumn::TYPE_DATETIME,20,0,                (uint32_t)offsetof(Row,date)                 ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_VOLUME    ,L"Volume",                     IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,volume)            ,0,0,0 },
   { COLUMN_IN_VOLUME ,L"Profit Deals Volume",        IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_volume)         ,0,0,0 },
   { COLUMN_OUT_VOLUME,L"Loss Deals Volume",          IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,out_volume)        ,0,0,0 },
   { COLUMN_IN_AMOUNT ,L"Profit Amount",              IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_amount)         ,0,0,0 },
   { COLUMN_OUT_AMOUNT,L"Loss Amount",                IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,out_amount)        ,0,0,0 },
   { COLUMN_IN_COUNT  ,L"Profit Deals Count",         IMTDatasetColumn::TYPE_UINT64  ,10,0,Row::Sections()+(uint32_t)offsetof(Record,in_count)          ,0,0,0 },
   { COLUMN_OUT_COUNT ,L"Loss Deals Count",           IMTDatasetColumn::TYPE_UINT64  ,10,0,Row::Sections()+(uint32_t)offsetof(Record,out_count)         ,0,0,0 },
   { COLUMN_IN_MEDIUM ,L"Profit Deals Volume Average",IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_medium)         ,0,0,0 },
   { COLUMN_OUT_MEDIUM,L"Loss Deals Volume Average",  IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,out_medium)        ,0,0,0 },
   { COLUMN_IN_OUT    ,L"Profit-Loss",                IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_out_delta)      ,0,0,0 },
   { COLUMN_IN_OUT_ACC,L"Profit-Loss Accumulated",    IMTDatasetColumn::TYPE_MONEY   ,20,0,Row::Sections()+(uint32_t)offsetof(Record,in_out_delta_accum),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column total description                                         |
//+------------------------------------------------------------------+
ReportColumn CDealReport::s_columns_total[4]=
//--- id,              name,                          type,                           width, width_max, offset,          size,                         digits_column,flags
  {{ COLUMN_SECTION   ,L"Section",                    IMTDatasetColumn::TYPE_STRING  ,20,0,offsetof(RowTotal,section)   ,MtFieldSize(RowTotal,section),0,IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_VOLUME    ,L"Volume",                     IMTDatasetColumn::TYPE_MONEY   ,20,0,offsetof(RowTotal,volume)    ,0,0,0 },
   { COLUMN_IN_VOLUME ,L"Profit Deals Volume",        IMTDatasetColumn::TYPE_MONEY   ,20,0,offsetof(RowTotal,in_volume) ,0,0,0 },
   { COLUMN_OUT_VOLUME,L"Loss Deals Volume",          IMTDatasetColumn::TYPE_MONEY   ,20,0,offsetof(RowTotal,out_volume),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealReport::CDealReport(const MTReportInfo &info,const bool line_chart) :
   CCurrencyReport(info),m_line_chart(line_chart),m_interval(DAYS_UNTIL_WEEK,DAYS_UNTIL_MONTH,DAYS_UNTIL_DAY),m_users(NULL),m_deals(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealReport::~CDealReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDealReport::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=NULL;
     }
//--- release balances
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- clear interval
   m_interval.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initalize interval
   MTAPIRES res=m_interval.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- user cache
   m_users=CUserCache::Create(*m_api,m_params,&m_currency,false,res);
   if(!m_users)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::Calculate(void)
  {
//--- checks
   if(!m_api || !m_users || m_deals)
      return(MT_RET_ERR_PARAMS);
//--- update users with filtration by report parameters
   MTAPIRES res=m_users->UpdateFilter(0,m_interval.To());
   if(res!=MT_RET_OK)
      return(res);
//--- deal cache
   m_deals=CDealCache::Create(*m_api,*m_users,m_params,m_currency,res);
   if(!m_deals)
      return(res);
//--- update balances
   if((res=m_deals->Update(m_interval.From(),m_interval.To()))!=MT_RET_OK)
      return(res);
//--- calculate deals
   return(CalculateDeals());
  }
//+------------------------------------------------------------------+
//| Calculate deals                                                  |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::CalculateDeals(void)
  {
//--- checks
   if(!m_api || !m_users || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- begin read users
   MTAPIRES res=m_users->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- read deals
   res=m_deals->ReadAll(*this,&CDealReport::DealRead);
//--- end read users
   const MTAPIRES res_users=m_users->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- return error code
   return(res_users);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::DealRead(const uint64_t key,const CDealCache::DealHour &hour)
  {
//--- deal hour key
   const CDealCache::DealKey &deal=CDealCache::Key(key);
//--- check deal time
   const int64_t time=deal.hour*SECONDS_IN_HOUR;
   if(time<m_api->ParamFrom() || time>m_api->ParamTo())
      return(MT_RET_OK);
//--- currency conversion
   double rate=0.0;
   const bool convert=m_currency.NeedConversion(deal.currency);
   if(convert)
     {
      //--- get currency conversion rate
      MTAPIRES res=m_currency.CurrencyRate(rate,deal.currency);
      if(res!=MT_RET_OK)
         return(res);
     }
//--- call deal hour read handler
   return(DealHourRead(time,deal,hour,convert ? &rate : nullptr));
  }
//+------------------------------------------------------------------+
//| Add deal hour to report section                                  |
//+------------------------------------------------------------------+
void CDealReport::DealAdd(Section &section,const uint64_t time,const CDealCache::DealHour &hour,const double *rate)
  {
//--- profit deals
   if(hour.profit_count)
      section.in.DealProfitAdd(hour.profit_count,
         rate ? m_currency.Convert(hour.profit_volume,*rate) : hour.profit_volume,
         rate ? m_currency.Convert(hour.profit,*rate) : hour.profit,m_currency);
//--- loss deals
   if(hour.loss_count)
      section.out.DealProfitAdd(hour.loss_count,
         rate ? m_currency.Convert(hour.loss_volume,*rate) : hour.loss_volume,
         rate ? m_currency.Convert(hour.loss,*rate) : hour.loss,m_currency);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::PrepareGraphs(void)
  {
//--- double check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare total graphs
   MTAPIRES res=PrepareGraphsTotal();
   if(res!=MT_RET_OK)
      return(res);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare data
   if((res=PrepareData(*data))!=MT_RET_OK)
      return(res);
//--- prepare graphs
   for(uint32_t i=1;i<_countof(s_columns);i++)
      if((res=PrepareGraph(data,s_columns[i],m_line_chart ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE : IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::PrepareGraphsTotal(void)
  {
//--- double check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare total data
   MTAPIRES res=PrepareDataTotal(*data);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare graphs
   for(uint32_t i=1;i<std::min(2U,(uint32_t)_countof(s_columns_total));i++)
      if((res=PrepareGraph(data,s_columns_total[i],IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDealReport::PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type)
  {
//--- checks
   if(!m_api || !data)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- widget height
   MTAPIRES res=chart->Height(type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_GEO ? CHART_HEIGHT_MAIN : CHART_HEIGHT_PRIMARY);
   if(res!=MT_RET_OK)
      return(res);
//--- chart type
   if((res=chart->Type(type))!=MT_RET_OK)
      return(res);
//--- stacked area chart
   if(type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA)
      if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
         return(res);
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
void CDealReport::FillRecord(Record &record,const Section &section,double *in_out_delta_accum) const
  {
//--- fill record by section
   record.volume      =section.in.amount+section.out.amount;
   record.in_volume   =section.in.amount;
   record.out_volume  =section.out.amount;
   record.in_amount   =section.in.profit;
   record.in_count    =section.in.total;
   record.in_medium   =section.in.Medium(m_currency);
   record.out_amount  =-section.out.profit;
   record.out_count   =section.out.total;
   record.out_medium  =section.out.Medium(m_currency);
   record.in_out_delta=section.InOutProfitDelta(in_out_delta_accum);
   record.in_out_delta_accum=in_out_delta_accum ? *in_out_delta_accum : NAN;
  }
//+------------------------------------------------------------------+
//| Fill row total                                                   |
//+------------------------------------------------------------------+
void CDealReport::FillRowTotal(RowTotal &row,const Section &section) const
  {
//--- fill record by section
   row.volume     =section.in.amount+section.out.amount;
   row.in_volume  =section.in.amount;
   row.out_volume =section.out.amount;
  }
//+------------------------------------------------------------------+
