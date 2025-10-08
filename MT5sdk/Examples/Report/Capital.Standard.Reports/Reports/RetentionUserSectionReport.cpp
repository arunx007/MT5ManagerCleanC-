//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "RetentionUserSectionReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CRetentionUserSectionReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_ANY,                      // minimal IE version
   L"Retention of Trading Accounts",                  // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",                // description
   MTReportInfo::SNAPSHOT_NONE,                       // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                      // report types
   L"Capital",                                        // report category
                                                      // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,           DEFAULT_GROUPS             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                           },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_SOURCE,      DEFAULT_LEAD_SOURCE        },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_CAMPAIGN,    DEFAULT_LEAD_CAMPAIGN      },
     },5,                                             // request parameters total
     {                                                // config parameters
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT,          DEFAULT_TOP_COUNT          },
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },2                                              // config parameters total
  };
//+------------------------------------------------------------------+
//| Retention by lead source chart description                       |
//+------------------------------------------------------------------+
LPCWSTR CRetentionUserSectionReport::s_section_lead_source=
L"This chart shows the retention of trading accounts.\n"
L"Y-axis - percent of active trading accounts. X axis - relative time in days of the trading account activity.\n"
L"Trading accounts are grouped by lead source. The graph displays the largest groups.\n"
L"The time range of the report filters trading accounts by the time of registration.\n";
//+------------------------------------------------------------------+
//| Retention by lead campaign chart description                     |
//+------------------------------------------------------------------+
LPCWSTR CRetentionUserSectionReport::s_section_lead_campaign=
L"This chart shows the retention of trading accounts.\n"
L"Y-axis - percent of active trading accounts. X axis - relative time in days of the trading account activity.\n"
L"Trading accounts are grouped by lead campaign. The graph displays the largest groups.\n"
L"The time range of the report filters trading accounts by the time of registration.\n";
//+------------------------------------------------------------------+
//| Retention by country chart description                           |
//+------------------------------------------------------------------+
LPCWSTR CRetentionUserSectionReport::s_section_country=
L"This chart shows the retention of trading accounts.\n"
L"Y-axis - percent of active trading accounts. X axis - relative time in days of the trading account activity.\n"
L"Trading accounts are grouped by country. The graph displays the largest groups.\n"
L"The time range of the report filters trading accounts by the time of registration.\n";
//+------------------------------------------------------------------+
//| Retention by registration chart description                      |
//+------------------------------------------------------------------+
LPCWSTR CRetentionUserSectionReport::s_section_registration=
L"This chart shows the retention of trading accounts.\n"
L"Y-axis - percent of active trading accounts. X axis - relative time in days of the trading account activity.\n"
L"Trading accounts are grouped by registration date. The graph displays the largest groups.\n"
L"The time range of the report filters trading accounts by the time of registration.\n";
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CRetentionUserSectionReport::CRetentionUserSectionReport(void) :
   CCurrencyReport(s_info),m_interval(SMOOTH_RANGE_ONE,SMOOTH_RANGE_TWO),m_activity(nullptr),m_users(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CRetentionUserSectionReport::~CRetentionUserSectionReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CRetentionUserSectionReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CRetentionUserSectionReport::Create(void)
  {
   return(new (std::nothrow) CRetentionUserSectionReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CRetentionUserSectionReport::Clear(void)
  {
//--- clear interval
   m_interval.Clear();
//--- release caches
   if(m_activity)
     {
      m_activity->Release();
      m_activity=nullptr;
     }
   if(m_users)
     {
      m_users->Release();
      m_users=nullptr;
     }
//--- clear sections
   m_section_lead_source.Clear();
   m_section_lead_campaign.Clear();
   m_section_country.Clear();
   m_section_registration.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CRetentionUserSectionReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initalize interval
   MTAPIRES res=m_interval.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- initialize sections
   if((res=m_section_lead_source.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_country.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_registration.Initialize())!=MT_RET_OK)
      return(res);
//--- user activity cache
   m_activity=CUserActivityCache::Create(*m_api,false,res);
   if(!m_activity)
      return(res);
//--- user cache
   m_users=CUserCache::Create(*m_api,m_params,nullptr,false,res);
   if(!m_users)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CRetentionUserSectionReport::Calculate(void)
  {
   if(!m_api || !m_activity || !m_users)
      return(MT_RET_ERR_PARAMS);
//--- update user cache
   MTAPIRES res=m_users->UpdateFilter(m_interval.From(),m_interval.To());
   if(res!=MT_RET_OK)
      return(res);
//--- update user activity cache
   if((res=m_activity->Update(m_interval.From(),CReportCache::Now(*m_api,m_params)))!=MT_RET_OK)
      return(res);
//--- begin read activity users
   if((res=m_activity->ReadBegin())!=MT_RET_OK)
      return(res);
//--- read cache
   res=m_users->ReadAll(*this,&CRetentionUserSectionReport::UserRead);
//--- end read activity users
   const MTAPIRES res_users=m_activity->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
   if(res_users!=MT_RET_OK)
      return(res_users);
//--- calculate total
   return(CalculateTotal());
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CRetentionUserSectionReport::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- check registration in range
   if(!m_interval.Contains(user.registration))
      return(MT_RET_OK);
//--- read user activity
   const CUserActivityCache::UserActivityCache *activity=nullptr;
   MTAPIRES res=m_activity->ReadActivity(login,activity);
   if(res!=MT_RET_OK)
      return(res);
   if(!activity)
      return(MT_RET_OK);
//--- timelife
   const int64_t lifetime=activity->last_access-user.registration;
//--- add user to sections
   if((res=m_section_lead_source.LifetimeAdd(lifetime,user.lead_source))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.LifetimeAdd(lifetime,user.lead_campaign))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.LifetimeAdd(lifetime,user.country))!=MT_RET_OK)
      return(res);
   const int32_t period=m_interval.PeriodIndex(user.registration);
   if(period<0)
      return(MT_RET_OK);
   if((res=m_section_registration.LifetimeAdd(lifetime,period))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CRetentionUserSectionReport::CalculateTotal(void)
  {
   if(!m_users)
      return(MT_RET_ERR_PARAMS);
//--- calculate sections total
   MTAPIRES res=m_section_lead_source.CalculateTotal(m_params);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
//--- calculate periods in section
   if((res=m_section_registration.CalculatePeriod(m_interval))!=MT_RET_OK)
      return(res);
//--- begin read users
   if((res=m_users->ReadBegin())!=MT_RET_OK)
      return(res);
//--- initialize sections top names
   const MTAPIRES res_lead_source=m_section_lead_source.ReadTopNames(*m_users,offsetof(CUserCache::UserCache,lead_source));
   const MTAPIRES res_lead_campaign=m_section_lead_campaign.ReadTopNames(*m_users,offsetof(CUserCache::UserCache,lead_campaign));
   const MTAPIRES res_country=m_section_country.ReadTopNames(*m_users,offsetof(CUserCache::UserCache,country));
//--- end read users
   res=m_users->ReadEnd();
//--- check errors
   if(res_lead_source!=MT_RET_OK)
      return(res_lead_source);
   if(res_lead_campaign!=MT_RET_OK)
      return(res_lead_campaign);
   if(res_country!=MT_RET_OK)
      return(res_country);
//--- check error code
   if(res!=MT_RET_OK)
      return(res);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CRetentionUserSectionReport::PrepareGraphs(void)
  {
//--- double check
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare sections graphs
   MTAPIRES res=m_section_registration.PrepareGraphs(*m_api,L"Registration Date",s_section_registration);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.PrepareGraphs(*m_api,L"Lead Campaign",s_section_lead_campaign))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_source.PrepareGraphs(*m_api,L"Lead Source",s_section_lead_source))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.PrepareGraphs(*m_api,L"Country",s_section_country))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+