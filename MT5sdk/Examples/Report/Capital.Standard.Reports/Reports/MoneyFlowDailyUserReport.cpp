//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MoneyFlowDailyUserReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CMoneyFlowDailyUser::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_9,                     // minimal IE version
   L"Money Flow Daily",                            // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                   // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,  DEFAULT_GROUPS                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                            },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                              },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_SOURCE,   DEFAULT_LEAD_SOURCE              },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_CAMPAIGN, DEFAULT_LEAD_CAMPAIGN            },
     },5,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,              DEFAULT_CURRENCY           },
     },1                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMoneyFlowDailyUser::CMoneyFlowDailyUser(void) :
   CMoneyFlowDaily(s_info,true),m_users(NULL),m_balances(NULL),m_dates(true)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMoneyFlowDailyUser::~CMoneyFlowDailyUser(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CMoneyFlowDailyUser::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CMoneyFlowDailyUser::Create(void)
  {
   return(new (std::nothrow) CMoneyFlowDailyUser());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CMoneyFlowDailyUser::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=NULL;
     }
//--- release balances
   if(m_balances)
     {
      m_balances->Release();
      m_balances=NULL;
     }
//--- clear container
   m_dates.Clear();
//--- base call
   CMoneyFlowDaily::Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailyUser::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at report preparation"));
//--- base call
   MTAPIRES res=CMoneyFlowDaily::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- user cache
   m_users=CUserCache::Create(*m_api,m_params,&m_currency,false,res);
   if(!m_users)
      return(res);
//--- balance cache
   m_balances=CBalanceCache::Create(*m_api,res);
   if(!m_balances)
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
MTAPIRES CMoneyFlowDailyUser::Calculate(void)
  {
//--- checks
   if(!m_api || !m_users || !m_balances)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at calculate"));
//--- update users
   MTAPIRES res=m_users->UpdateFilter(0,m_interval.To());
   if(res!=MT_RET_OK)
      return(LogError(res,L"Update accounts cache"));
//--- update balances
   if((res=m_balances->Update(m_users->Logins(),m_interval.From(),m_interval.To()))!=MT_RET_OK)
      return(LogError(res,L"Update balances cache"));
//--- calculate daily
   return(CalculateDaily());
  }
//+------------------------------------------------------------------+
//| Calculate daily                                                  |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailyUser::CalculateDaily(void)
  {
//--- checks
   if(!m_api || !m_users || !m_balances)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at calculate daily"));
//--- begin read balances
   MTAPIRES res=m_balances->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- read users
   res=m_users->ReadAll(*this);
//--- end read balances
   const MTAPIRES res_balances=m_balances->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(LogError(res,L"Read users"));
//--- return error code
   return(res_balances);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailyUser::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_api || !m_balances)
      return(LogError(MT_RET_ERR_PARAMS,L"Parameters check at user read handler"));
//--- get user balance
   CBalanceCache::CUserBalance balance;
   const MTAPIRES res=m_balances->ReadBalance(login,balance);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(LogError(res,L"Read account %I64u balance",login));
//--- calculate user balance 
   return(CalculateUserBalance(user,balance));
  }
//+------------------------------------------------------------------+
//| Calculate user balance                                           |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailyUser::CalculateUserBalance(const CUserCache::UserCache &user,const CBalanceCache::CUserBalance &balance)
  {
//--- get user balance
   const CBalanceCache::UserBalance *user_balance=balance.User();
   if(!user_balance)
      return(MT_RET_OK);
//--- currency conversion
   double rate=0.0;
   const bool convert=m_currency.NeedConversion(user.currency);
   if(convert)
     {
      //--- get currency conversion rate
      MTAPIRES res=m_currency.CurrencyRate(rate,user.currency);
      if(res!=MT_RET_OK)
         return(LogError(res,L"Get currency %u conversion rate",user.currency));
     }
//--- calculate balance
   for(uint32_t i=0;const CBalanceCache::DealBalance *deal_balance=balance.Deal(i);i++)
     {
      MTAPIRES res=CalculateBalance(*deal_balance,convert ? &rate : nullptr);
      if(res!=MT_RET_OK)
         return(LogError(res,L"Calculate balance"));
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate balance                                                |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlowDailyUser::CalculateBalance(const CBalanceCache::DealBalance &deal,const double *rate)
  {
//--- check deal time
   const int64_t time=deal.info.day*SECONDS_IN_DAY;
   if(time<m_api->ParamFrom() || time>m_api->ParamTo())
      return(MT_RET_OK);
//--- get record by deal time
   Section *section=RecordByTime(time);
   if(!section)
      return(MT_RET_ERR_MEM);
//--- add deal balance to section
   section->DealAdd(rate ? m_currency.Convert(deal.value,*rate) : deal.value,deal.count,m_currency);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| record by time                                                   |
//+------------------------------------------------------------------+
CMoneyFlowDailyUser::Section* CMoneyFlowDailyUser::RecordByTime(const int64_t ctm)
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
MTAPIRES CMoneyFlowDailyUser::PrepareData(IMTDataset &data)
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
MTAPIRES CMoneyFlowDailyUser::PrepareGraph(IMTDataset *data,const ReportColumn &column)
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
MTAPIRES CMoneyFlowDailyUser::ChartColumnsAdd(IMTReportDashboardWidget &chart,const ReportColumn &column) const
  {
//--- add data column
   MTAPIRES res=chart.DataColumnAdd(column.id);
   if((column.id!=2 && column.id!=4 && column.id!=6) || res!=MT_RET_OK)
      return(res);
//--- add withdrawal data column
   return(chart.DataColumnAdd(column.id+1));
  }
//+------------------------------------------------------------------+
