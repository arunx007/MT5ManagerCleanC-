//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvSectionReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CLtvSectionReport::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"Lifetime Value",                              // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                   // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,           DEFAULT_GROUPS             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                           },
     },3,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,           DEFAULT_CURRENCY           },
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT,          DEFAULT_TOP_COUNT          },
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },3                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvSectionReport::CLtvSectionReport(void) :
   CCurrencyReport(s_info),m_client_balance(m_currency),m_section_lead_source(m_currency),m_section_lead_campaign(m_currency),m_section_country(m_currency)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvSectionReport::~CLtvSectionReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CLtvSectionReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CLtvSectionReport::Create(void)
  {
   return(new (std::nothrow) CLtvSectionReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CLtvSectionReport::Clear(void)
  {
//--- clear cache
   m_client_balance.Clear();
//--- clear sections
   m_section_lead_source.Clear();
   m_section_lead_campaign.Clear();
   m_section_country.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CLtvSectionReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initialize sections
   MTAPIRES res=m_section_lead_source.Initialize(*m_api);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.Initialize(*m_api))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.Initialize(*m_api))!=MT_RET_OK)
      return(res);
//--- initialize cache
   if((res=m_client_balance.Initialize(*m_api,m_params))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CLtvSectionReport::Calculate(void)
  {
//--- update cache
   MTAPIRES res=m_client_balance.Update(true);
   if(res!=MT_RET_OK)
      return(res);
//--- read cache
   if((res=m_client_balance.Read(this))!=MT_RET_OK)
      return(res);
//--- calculate total
   return(CalculateTotal());
  }
//+------------------------------------------------------------------+
//| Client and user balance deal read handler                        |
//+------------------------------------------------------------------+
MTAPIRES CLtvSectionReport::ClientUserBalanceDealRead(const ClientUserCache &client_user,const ClientCache &client,const DealBalance &balance,const double *rate)
  {
//--- check deal deposit balance
   if(balance.value<DBL_EPSILON)
      return(MT_RET_OK_NONE);
//--- deposit value
   const double value=rate ? m_currency.Convert(balance.value,*rate) : balance.value;
//--- add deposit to sections
   const int64_t time=balance.info.day*SECONDS_IN_DAY;
   MTAPIRES res=m_section_lead_source.DepositAdd(client_user.registration,time,client.lead_source,value,balance.count);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.DepositAdd(client_user.registration,time,client.lead_campaign,value,balance.count))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.DepositAdd(client_user.registration,time,client.country,value,balance.count))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CLtvSectionReport::CalculateTotal(void)
  {
//--- calculate sections total
   MTAPIRES res=m_section_lead_source.CalculateTotal(m_params);
   if(res!=MT_RET_OK)
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
//--- return error code
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CLtvSectionReport::PrepareGraphs(void)
  {
//--- double check
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare sections graphs
   MTAPIRES res=m_section_lead_source.PrepareGraphs(*m_api,L"Lead Source");
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.PrepareGraphs(*m_api,L"Lead Campaign"))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.PrepareGraphs(*m_api,L"Country"))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+