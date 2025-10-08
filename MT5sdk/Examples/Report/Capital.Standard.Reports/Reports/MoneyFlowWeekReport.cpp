//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MoneyFlowWeekReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CMoneyFlowWeek::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_9,                     // minimal IE version
   L"Money Flow Weekly",                           // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                   // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    PARAMETER_GROUP_FILTER, DEFAULT_GROUP_FILTER },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                               },
     },3,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,      DEFAULT_CURRENCY    },
     },1                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMoneyFlowWeek::CMoneyFlowWeek(void) :
   CMoneyFlowDaily(s_info,true,0,UINT_MAX),m_users(NULL),m_deals(NULL),m_dates(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMoneyFlowWeek::~CMoneyFlowWeek(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CMoneyFlowWeek::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CMoneyFlowWeek::Create(void)
  {
   return(new (std::nothrow) CMoneyFlowWeek());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CMoneyFlowWeek::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=NULL;
     }
//--- release deals
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- clear container
   m_dates.Clear();
//--- base call
   CMoneyFlowDaily::Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at report preparation"));
//--- base call
   MTAPIRES res=CMoneyFlowDaily::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- user group cache
   m_users=CUserGroupCache::Create(*m_api,m_params,m_currency,res);
   if(!m_users)
      return(res);
//--- deals week aggregates
   m_deals=CDealWeekCache::Create(*m_api,*m_users,m_params,m_currency,res);
   if(!m_deals)
      return(res);
//--- initialize money vector
   if(!m_dates.Initialize(m_interval.PeriodTotal()))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::Calculate(void)
  {
//--- checks
   if(!m_api || !m_deals)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at calculate"));
//--- update deals week aggregates
   MTAPIRES res=m_deals->Update(m_interval.From(),m_interval.To());
   if(res!=MT_RET_OK)
      return(LogError(res,L"Update deals week aggregates"));
//--- initialize group filter
   if((res=m_groups.Initialize(*m_api,*m_deals))!=MT_RET_OK)
      return(LogError(res,L"Initialize group filter"));
//--- calculate deals
   return(CalculateDeals());
  }
//+------------------------------------------------------------------+
//| Calculate deals                                                  |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::CalculateDeals(void)
  {
//--- checks
   if(!m_api || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- read all deals weeks
   return(m_deals->ReadAll(*this,&CMoneyFlowWeek::DealRead));
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::DealRead(const uint64_t key,const CDealWeekCache::DealWeek &week)
  {
//--- check deal week
   const CDealWeekCache::DealKey &deal=CDealWeekCache::Key(key);
   if(!m_interval.WeekContains(deal.week))
      return(MT_RET_OK);
//--- match group with filter
   if(!m_groups.GroupMatch(deal.group))
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
//--- call deal week read handler
   return(DealWeekRead(deal.week*SECONDS_IN_WEEK,deal,week,convert ? &rate : nullptr));
  }
//+------------------------------------------------------------------+
//| deal week read handler                                           |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::DealWeekRead(const uint64_t time,const CDealWeekCache::DealKey &key,const CDealWeekCache::DealWeek &week,const double *rate)
  {
//--- get record by deal time
   Section *section=RecordByTime(time);
   if(!section)
      return(MT_RET_ERR_MEM);
//--- add deal week to section
   DealAdd(*section,time,week,rate);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add deal week to report section                                  |
//+------------------------------------------------------------------+
void CMoneyFlowWeek::DealAdd(Section &section,const uint64_t time,const CDealWeekCache::DealWeek &week,const double *rate)
  {
//--- deposits
   if(week.deposit.count)
      section.in.DealAdd(rate ? m_currency.Convert(week.deposit.value,*rate) : week.deposit.value,week.deposit.count,m_currency);
//--- withdrawals
   if(week.withdrawal.count)
      section.out.DealAdd(-(rate ? m_currency.Convert(week.withdrawal.value,*rate) : week.withdrawal.value),week.withdrawal.count,m_currency);
  }
//+------------------------------------------------------------------+
//| record by time                                                   |
//+------------------------------------------------------------------+
CMoneyFlowWeek::Section* CMoneyFlowWeek::RecordByTime(const int64_t ctm)
  {
//--- calculate period index
   const int32_t period=m_interval.PeriodIndex(ctm);
   if(period<0)
      return(nullptr);
//--- return vector item
   return(m_dates.Item(period));
  }
//+------------------------------------------------------------------+
//| Prepare money data                                               |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::PrepareData(IMTDataset &data)
  {
//--- checks
   if(!m_api)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at data preparation"));
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(data,s_columns_money,_countof(s_columns_money));
   if(res!=MT_RET_OK)
      return(LogError(res,L"Create dataset columns"));
//--- write each date
   double in_out_delta_accum=NAN;
   for(uint32_t i=0,total=m_dates.Total();i<total;i++)
     {
      //--- calculate medium
      const Section *section=m_dates.Item(i);
      if(!section)
         return(LogError(MT_RET_ERROR,L"Get data date item"));
      //--- fill row
      MoneyRow row={0};
      row.date=m_interval.PeriodDate(i);
      FillRecord(row.sections[0],*section,&in_out_delta_accum);
      //--- write record
      if((res=data.RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(LogError(res,L"Write dataset record"));
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| prepare money money graph                                        |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::PrepareGraph(IMTDataset *data,const ReportColumn &column)
  {
//--- skip withdrawal data columns
   if(column.id==3 || column.id==5 || column.id==7)
      return(MT_RET_OK);
//--- prepare money money graph
   return(CMoneyFlowDaily::PrepareGraph(data,column));
  }
//+------------------------------------------------------------------+
//| chart columns add                                                |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowWeek::ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const
  {
//--- add data column
   MTAPIRES res=chart.DataColumnAdd(column.id);
   if((column.id!=2 && column.id!=4 && column.id!=6) || res!=MT_RET_OK)
      return(res);
//--- add withdrawal data column
   return(chart.DataColumnAdd(column.id+1));
  }
//+------------------------------------------------------------------+
