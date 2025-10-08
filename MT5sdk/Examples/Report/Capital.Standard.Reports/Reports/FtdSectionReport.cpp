//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "FtdSectionReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CFtdSectionReport::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"First Time Deposit Daily",                    // report name
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
CFtdSectionReport::CFtdSectionReport(void) :
   CCurrencyReport(s_info),m_client_ftd(m_currency),m_interval(SMOOTH_RANGE_ONE,SMOOTH_RANGE_TWO),
   m_section_lead_source(m_currency,m_interval),m_section_lead_campaign(m_currency,m_interval),m_section_country(m_currency,m_interval),m_section_value(m_currency,m_interval)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CFtdSectionReport::~CFtdSectionReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CFtdSectionReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CFtdSectionReport::Create(void)
  {
   return(new (std::nothrow) CFtdSectionReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CFtdSectionReport::Clear(void)
  {
//--- clear cache
   m_client_ftd.Clear();
//--- clear interval
   m_interval.Clear();
//--- clear sections
   m_section_lead_source.Clear();
   m_section_lead_campaign.Clear();
   m_section_country.Clear();
   m_section_value.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CFtdSectionReport::Prepare(void)
  {
//--- checks
   if(m_api==NULL)
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
   if((res=m_section_value.Initialize())!=MT_RET_OK)
      return(res);
//--- initialize cache
   if((res=m_client_ftd.Initialize(*m_api,m_params))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CFtdSectionReport::Calculate(void)
  {
//--- update cache
   MTAPIRES res=m_client_ftd.Update(m_interval.From(),m_interval.To());
   if(res!=MT_RET_OK)
      return(res);
//--- read cache
   if((res=m_client_ftd.Read(this))!=MT_RET_OK)
      return(res);
//--- calculate total
   return(CalculateTotal());
  }
//+------------------------------------------------------------------+
//| Client FTD read handler                                          |
//+------------------------------------------------------------------+
MTAPIRES CFtdSectionReport::ClientFtdRead(const CClientCache::ClientCache &client,const int64_t time,const double value)
  {
//--- add deposit to sections
   MTAPIRES res=m_section_lead_source.DepositAdd(time,value,client.lead_source);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.DepositAdd(time,value,client.lead_campaign))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.DepositAdd(time,value,client.country))!=MT_RET_OK)
      return(res);
   if((res=m_section_value.DepositAdd(time,value,SReportMoneyGroup::GroupIndex(value)))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate total data                                             |
//+------------------------------------------------------------------+
MTAPIRES CFtdSectionReport::CalculateTotal(void)
  {
//--- calculate sections total
   MTAPIRES res=m_section_lead_source.CalculateTotal(m_params);
   if(res!=MT_RET_OK)
      return(res);
   if((res=m_section_lead_campaign.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
   if((res=m_section_country.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
   if((res=m_section_value.CalculateTotal(m_params))!=MT_RET_OK)
      return(res);
//--- begin read clients
   const CReportCache *cache=nullptr;
   if((res=m_client_ftd.ClientsReadBegin(cache))!=MT_RET_OK)
      return(res);
   if(!cache)
      return(MT_RET_ERROR);
//--- initialize sections top names
   const MTAPIRES res_lead_source=m_section_lead_source.ReadTopNames(*cache,CClientCache::DICTIONARY_ID_LEAD_SOURCE);
   const MTAPIRES res_lead_campaign=m_section_lead_campaign.ReadTopNames(*cache,CClientCache::DICTIONARY_ID_LEAD_CAMPAIGN);
   const MTAPIRES res_country=m_section_country.ReadTopNames(*cache,CClientCache::DICTIONARY_ID_COUNTRY);
//--- end read clients
   res=m_client_ftd.ClientsReadEnd();
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
//--- fill top sections names by money group
   if((res=m_section_value.MoneyGroupTopNames())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CFtdSectionReport::PrepareGraphs(void)
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
   if((res=m_section_value.PrepareGraphs(*m_api,L"Value"))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+