//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealProfitReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CDealProfitReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_9,                        // minimal IE version
   L"Deals Profit",                                   // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",                // description
   MTReportInfo::SNAPSHOT_NONE,                       // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                      // report types
   L"Trades",                                         // report category
                                                      // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,        DEFAULT_GROUPS        },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                 },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                   },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_SOURCE,   DEFAULT_LEAD_SOURCE   },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_CAMPAIGN, DEFAULT_LEAD_CAMPAIGN },
     },5,                                             // request parameters total
     {                                                // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,        DEFAULT_CURRENCY      },
     },1                                              // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealProfitReport::CDealProfitReport(void) :
   CDealReport(s_info,true),m_hours(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealProfitReport::~CDealProfitReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDealProfitReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CDealProfitReport::Create(void)
  {
   return(new (std::nothrow) CDealProfitReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDealProfitReport::Clear(void)
  {
//--- clear container
   m_hours.Clear();
//--- base call
   CDealReport::Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CDealProfitReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- base call
   MTAPIRES res=CDealReport::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- initialize vector
   if(!m_hours.Initialize(m_interval.PeriodTotal()))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| deal hour read handler                                           |
//+------------------------------------------------------------------+
MTAPIRES CDealProfitReport::DealHourRead(const uint64_t time,const CDealCache::DealKey &key,const CDealCache::DealHour &hour,const double *rate)
  {
//--- get record by deal time
   Section *section=RecordByTime(time);
   if(!section)
      return(MT_RET_ERR_MEM);
//--- add deal hour to section
   DealAdd(*section,time,hour,rate);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| record by time                                                   |
//+------------------------------------------------------------------+
CDealProfitReport::Section* CDealProfitReport::RecordByTime(const int64_t ctm)
  {
//--- calculate period index
   const int32_t period=m_interval.PeriodIndex(ctm);
   if(period<0)
      return(nullptr);
//--- return vector item
   return(m_hours.Item(period));
  }
//+------------------------------------------------------------------+
//| Prepare data                                                     |
//+------------------------------------------------------------------+
MTAPIRES CDealProfitReport::PrepareData(IMTDataset &data)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns,_countof(s_columns));
   if(res!=MT_RET_OK)
      return(res);
//--- write each date
   double in_out_delta_accum=NAN;
   for(uint32_t i=0,total=m_hours.Total();i<total;i++)
     {
      //--- calculate medium
      const Section *section=m_hours.Item(i);
      if(!section)
         return(MT_RET_ERROR);
      //--- fill row
      Row row={0};
      row.date=m_interval.PeriodDate(i);
      FillRecord(row.sections[0],*section,&in_out_delta_accum);
      //--- write record
      if((res=data.RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| prepare graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDealProfitReport::PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type)
  {
//--- skip loss data columns
   if(column.id==COLUMN_IN_VOLUME || column.id==COLUMN_OUT_VOLUME || column.id==COLUMN_OUT_AMOUNT || column.id==COLUMN_OUT_COUNT || column.id==COLUMN_OUT_MEDIUM)
      return(MT_RET_OK);
//--- prepare graph
   return(CDealReport::PrepareGraph(data,column,type));
  }
//+------------------------------------------------------------------+
//| chart columns add                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealProfitReport::ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const
  {
//--- add data column
   MTAPIRES res=chart.DataColumnAdd(column.id);
   if(res!=MT_RET_OK)
      return(res);
//--- add data columns
   switch(column.id)
     {
      case COLUMN_VOLUME:
         //--- add profit and loss volume columns
         if((res=chart.DataColumnAdd(column.id+1))!=MT_RET_OK)
            break;
         res=chart.DataColumnAdd(column.id+2);
         break;
      case COLUMN_IN_AMOUNT:
      case COLUMN_IN_COUNT:
      case COLUMN_IN_MEDIUM:
         //--- add loss data columns
         res=chart.DataColumnAdd(column.id+1);
         break;
      default: break;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
