//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "RetentionSectionReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CRetentionSectionReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_ANY,                      // minimal IE version
   L"Retention of Clients",                           // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",                // description
   MTReportInfo::SNAPSHOT_NONE,                       // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                      // report types
   L"Capital",                                        // report category
                                                      // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,           DEFAULT_GROUPS             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                           },
     },3,                                             // request parameters total
     {                                                // config parameters
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT,          DEFAULT_TOP_COUNT          },
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },2                                              // config parameters total
  };
//+------------------------------------------------------------------+
//| Retention by lead source chart description                       |
//+------------------------------------------------------------------+
LPCWSTR CRetentionSectionReport::s_section_lead_source=
L"This chart shows the retention of clients.\n"
L"Y-axis - percent of active clients. X axis - relative time in days of the client activity.\n"
L"Clients are grouped by lead source. The graph displays the largest groups.\n"
L"The time range of the report filters clients by the time of registration.\n";
//+------------------------------------------------------------------+
//| Retention by lead campaign chart description                     |
//+------------------------------------------------------------------+
LPCWSTR CRetentionSectionReport::s_section_lead_campaign=
L"This chart shows the retention of clients.\n"
L"Y-axis - percent of active trading accounts. X axis - relative time in days of the client activity.\n"
L"Trading accounts are grouped by lead campaign. The graph displays the largest groups.\n"
L"The time range of the report filters clients by the time of registration.\n";
//+------------------------------------------------------------------+
//| Retention by country chart description                           |
//+------------------------------------------------------------------+
LPCWSTR CRetentionSectionReport::s_section_country=
L"This chart shows the retention of clients.\n"
L"Y-axis - percent of active clients. X axis - relative time in days of the client activity.\n"
L"Clients are grouped by country. The graph displays the largest groups.\n"
L"The time range of the report filters clients by the time of registration.\n";
//+------------------------------------------------------------------+
//| Retention by registration chart description                      |
//+------------------------------------------------------------------+
LPCWSTR CRetentionSectionReport::s_section_registration=
L"This chart shows the retention of clients.\n"
L"Y-axis - percent of active clients. X axis - relative time of the client activity.\n"
L"Clients are grouped by registration date. The graph displays the largest groups.\n"
L"The time range of the report filters clients by the time of registration.\n";
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CRetentionSectionReport::CRetentionSectionReport(void) :
   CCurrencyReport(s_info),m_interval(SMOOTH_RANGE_ONE,SMOOTH_RANGE_TWO),m_from(0),m_now(0),
   m_activity(nullptr),m_users(nullptr),m_client_users(nullptr),m_clients(nullptr),m_ids(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CRetentionSectionReport::~CRetentionSectionReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CRetentionSectionReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CRetentionSectionReport::Create(void)
  {
   return(new (std::nothrow) CRetentionSectionReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CRetentionSectionReport::Clear(void)
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
   if(m_client_users)
     {
      m_client_users->Release();
      m_client_users=nullptr;
     }
   if(m_clients)
     {
      m_clients->Release();
      m_clients=nullptr;
     }
//--- release ids
   if(m_ids)
     {
      m_ids->Release();
      m_ids=nullptr;
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
MTAPIRES CRetentionSectionReport::Prepare(void)
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
   m_activity=CUserActivityCache::Create(*m_api,true,res);
   if(!m_activity)
      return(res);
//--- user cache
   m_users=CUserCache::Create(*m_api,m_params,nullptr,true,res);
   if(!m_users)
      return(res);
//--- client by user cache
   m_client_users=CClientUserCache::Create(*m_api,*m_users,res);
   if(!m_client_users)
      return(res);
//--- user cache
   m_clients=CClientCache::Create(*m_api,res);
   if(!m_clients)
      return(res);
//--- create client ids keyset
   m_ids=m_api->KeySetCreate();
   if(!m_ids)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSectionReport::Calculate(void)
  {
//--- checks
   if(!m_api || !m_activity || !m_users || !m_client_users || !m_clients || !m_ids)
      return(MT_RET_ERR_PARAMS);
//--- init time
   m_from=m_interval.From();
   m_now=CReportCache::Now(*m_api,m_params);
//--- update user activity cache
   MTAPIRES res=m_activity->Update(m_from,m_now);
   if(res!=MT_RET_OK)
      return(res);
//--- update users
   if((res=m_users->Update(0,m_interval.To()))!=MT_RET_OK)
      return(res);
//--- update clients by users
   if((res=m_client_users->Update(0,m_interval.To()))!=MT_RET_OK)
      return(res);
//--- read all activity cache to collect active clients ids
   if((res=m_activity->ReadAll(*this,&CRetentionSectionReport::ActivityReadId))!=MT_RET_OK)
      return(res);
//--- update clients
   if((res=m_clients->Update(*m_ids))!=MT_RET_OK)
      return(res);
//--- calculate total
   return(CalculateSections());
  }
//+------------------------------------------------------------------+
//| Calculate report sections                                        |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSectionReport::CalculateSections(void)
  {
//--- checks
   if(!m_api || !m_activity || !m_client_users || !m_clients || !m_ids)
      return(MT_RET_ERR_PARAMS);
//--- begin read client by users
   MTAPIRES res=m_client_users->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- begin read clients
   if((res=m_clients->ReadBegin())!=MT_RET_OK)
      return(res);
//--- read cache
   res=m_activity->Read(*m_ids,*this,&CRetentionSectionReport::ActivityRead);
//--- end read clients
   const MTAPIRES res_clients=m_clients->ReadEnd();
//--- end read clients
   const MTAPIRES res_client_users=m_client_users->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
   if(res_clients!=MT_RET_OK)
      return(res_clients);
   if(res_client_users!=MT_RET_OK)
      return(res_client_users);
//--- calculate total
   return(CalculateTotal());
  }
//+------------------------------------------------------------------+
//| client activity read handler                                     |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSectionReport::ActivityReadId(const uint64_t id,const CUserActivityCache::UserActivityCache &activity)
  {
//--- check id and last access time
   if(!id || activity.last_access<m_from || activity.last_access>m_now)
      return(MT_RET_OK);
//--- insert id to set
   return(m_ids->Insert(id));
  }
//+------------------------------------------------------------------+
//| client activity read handler                                     |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSectionReport::ActivityRead(const uint64_t id,const CUserActivityCache::UserActivityCache &activity)
  {
//--- read client user cache
   const CClientUserCache::ClientUserCache *client_user=nullptr;
   MTAPIRES res=m_client_users->ReadClient(id,client_user);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
   if(!client_user)
      return(MT_RET_OK);
//--- check registration in range
   if(!m_interval.Contains(client_user->registration))
      return(MT_RET_OK);
//--- read client
   const CClientCache::ClientCache *client=nullptr;
   res=m_clients->ReadClient(id,client);
   if(res==MT_RET_OK_NONE)
      return(MT_RET_OK);
   if(res!=MT_RET_OK)
      return(res);
   if(!client)
      return(MT_RET_OK);
//--- timelife
   const int64_t lifetime=activity.last_access-client_user->registration;
//--- add user to sections
   if((res=m_section_lead_source.LifetimeAdd(lifetime,client->lead_source))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.LifetimeAdd(lifetime,client->lead_campaign))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.LifetimeAdd(lifetime,client->country))!=MT_RET_OK)
      return(res);
   const int32_t period=m_interval.PeriodIndex(client_user->registration);
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
MTAPIRES CRetentionSectionReport::CalculateTotal(void)
  {
   if(!m_clients)
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
   if((res=m_clients->ReadBegin())!=MT_RET_OK)
      return(res);
//--- initialize sections top names
   const MTAPIRES res_lead_source=m_section_lead_source.ReadTopNames(*m_clients,CClientCache::DICTIONARY_ID_LEAD_SOURCE);
   const MTAPIRES res_lead_campaign=m_section_lead_campaign.ReadTopNames(*m_clients,CClientCache::DICTIONARY_ID_LEAD_CAMPAIGN);
   const MTAPIRES res_country=m_section_country.ReadTopNames(*m_clients,CClientCache::DICTIONARY_ID_COUNTRY);
//--- end read users
   res=m_clients->ReadEnd();
//--- check errors
   if(res_lead_source!=MT_RET_OK)
      return(res_lead_source);
   if(res_lead_campaign!=MT_RET_OK)
      return(res_lead_campaign);
   if(res_country!=MT_RET_OK)
      return(res_country);
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CRetentionSectionReport::PrepareGraphs(void)
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