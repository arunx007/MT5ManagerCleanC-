//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ActivityReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CActivityReport::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"Clients Activity",                            // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                   // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,           DEFAULT_GROUPS             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                           },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_SOURCE,      DEFAULT_LEAD_SOURCE        },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_CAMPAIGN,    DEFAULT_LEAD_CAMPAIGN      },
     },5,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,           DEFAULT_CURRENCY           },
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT,          DEFAULT_TOP_COUNT          },
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },3                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CActivityReport::CActivityReport(void) :
   CCurrencyReport(s_info),m_interval(SMOOTH_RANGE_ONE,SMOOTH_RANGE_TWO),
   m_client_balance(m_currency),m_section_country(true),m_section_registration(m_interval)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CActivityReport::~CActivityReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CActivityReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CActivityReport::Create(void)
  {
   return(new (std::nothrow) CActivityReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CActivityReport::Clear(void)
  {
//--- clear objects
   m_client_balance.Clear();
   m_interval.Clear();
//--- clear sections
   m_section_active.Clear();
   m_section_lead_source.Clear();
   m_section_lead_campaign.Clear();
   m_section_country.Clear();
   m_section_registration.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::Prepare(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- initalize interval
   MTAPIRES res=m_interval.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- initialize cache
   if((res=m_client_balance.Initialize(*m_api,m_params))!=MT_RET_OK)
      return(res);
//--- initialize sections
   if((res=m_section_active.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_source.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_country.Initialize())!=MT_RET_OK)
      return(res);
   if((res=m_section_registration.Initialize())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::Calculate(void)
  {
//--- update cache
   MTAPIRES res=m_client_balance.Update(true);
   if(res!=MT_RET_OK)
      return(res);
//--- read cache
   if((res=m_client_balance.Read(this))!=MT_RET_OK)
      return(res);
//--- read inactive clients from cache
   if((res=m_client_balance.ReadClientInactive())!=MT_RET_OK)
      return(res);
//--- read inactive clients user from cache
   if((res=m_client_balance.ReadClientUserInactive())!=MT_RET_OK)
      return(res);
//--- calculate total sections
   if((res=CalculateTotal())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::CalculateTotal(void)
  {
//--- calculate sections total
   MTAPIRES res=m_section_active.CalculateTotal(m_params);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_source.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
//--- begin read clients
   const CReportCache *cache=nullptr;
   if((res=m_client_balance.ClientsReadBegin(cache))!=MT_RET_OK)
      return(res);
   if(!cache)
      return(MT_RET_ERROR);
//--- initialize sections top names
   const MTAPIRES res_lead_source=m_section_lead_source.ReadTopNames(*cache,CClientCache::DICTIONARY_ID_LEAD_SOURCE);
   const MTAPIRES res_lead_campaign=m_section_lead_campaign.ReadTopNames(*cache,CClientCache::DICTIONARY_ID_LEAD_CAMPAIGN);
   const MTAPIRES res_country=m_section_country.ReadTopNames(*cache,CClientCache::DICTIONARY_ID_COUNTRY);
//--- end read clients
   res=m_client_balance.ClientsReadEnd();
//--- check errors
   if(res_lead_source!=MT_RET_OK)
      return(res_lead_source);
   if(res_lead_campaign!=MT_RET_OK)
      return(res_lead_campaign);
   if(res_country!=MT_RET_OK)
      return(res_country);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_active.SetSectionName(0,L"Active"))!=MT_RET_OK)
      return(res);
   if((res=m_section_active.SetSectionName(1,L"Inactive"))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::PrepareGraphs(void)
  {
//--- check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare sections graphs
   MTAPIRES res=m_section_active.PrepareGraphs(*m_api,L"Active/Inactive Clients Count",0,0);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_country.PrepareGraphs(*m_api,L" Country",1,0))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_source.PrepareGraphs(*m_api,L" Lead Source",0,1))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.PrepareGraphs(*m_api,L" Lead Campaign",1,1))!=MT_RET_OK)
      return(res);
   return(m_section_registration.PrepareGraphs(*m_api,L"Inactive Clients Count by Registation Date",2));
  }
//+------------------------------------------------------------------+
//| Client and user balance deal read handler overridable            |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::ClientUserBalanceRead(const ClientUserCache &client_user,const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- search for nonzero deposit
   for(uint32_t i=0;const DealBalance *deal_balance=balance.Deal(i);i++)
      if(deal_balance->value>=DBL_EPSILON)
        {
         //--- count active client
         return(m_section_active.InactiveAdd(0));
        }
//--- no deposits
   return(MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| Client inactive read handler                                     |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::ClientInactiveRead(const ClientCache &client)
  {
//--- add inactive client to sections
   MTAPIRES res=m_section_active.InactiveAdd(1);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_source.InactiveAdd(client.lead_source))!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.InactiveAdd(client.lead_campaign))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.InactiveAdd(client.country))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Client user inactive read handler                                |
//+------------------------------------------------------------------+
MTAPIRES CActivityReport::ClientUserInactiveRead(const ClientUserCache &client_user)
  {
//--- add inactive to period section
   return(m_section_registration.InactiveAdd(client_user.registration));
  }
//+------------------------------------------------------------------+