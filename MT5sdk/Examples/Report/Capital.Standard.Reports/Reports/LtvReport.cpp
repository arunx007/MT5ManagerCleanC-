//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CLtvReport::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"Lifetime Value Daily",                        // report name
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
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },2                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvReport::CLtvReport(void) :
   CCurrencyReport(s_info),m_interval(SMOOTH_RANGE_ONE,SMOOTH_RANGE_TWO),m_client_balance(m_currency),m_section(m_currency)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvReport::~CLtvReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CLtvReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CLtvReport::Create(void)
  {
   return(new (std::nothrow) CLtvReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CLtvReport::Clear(void)
  {
//--- clear objects
   m_client_balance.Clear();
   m_section.Clear();
   m_interval.Clear();
  }
//+------------------------------------------------------------------+
//| Get report parameters and create caches                          |
//+------------------------------------------------------------------+
MTAPIRES CLtvReport::Prepare(void)
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
//--- initialize section
   if((res=m_section.Initialize(*m_api))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CLtvReport::Calculate(void)
  {
//--- update cache
   MTAPIRES res=m_client_balance.Update(true);
   if(res!=MT_RET_OK)
      return(res);
//--- read cache
   if((res=m_client_balance.Read(this))!=MT_RET_OK)
      return(res);
//--- calculate periods in section
   if((res=m_section.CalculatePeriod(m_interval))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CLtvReport::PrepareGraphs(void)
  {
//--- check
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare section graphs
   return(m_section.PrepareGraphs(*m_api,L"Client Registration Date"));
  }
//+------------------------------------------------------------------+
//| Client and user balance deal read handler overridable            |
//+------------------------------------------------------------------+
MTAPIRES CLtvReport::ClientUserBalanceRead(const ClientUserCache &client_user,const ClientCache &client,const CUserBalance &balance,const double *rate)
  {
//--- calculate section
   int32_t section=m_interval.PeriodIndex(client_user.registration);
   if(section<0)
      section=0;
//--- calculate balance
   MTAPIRES res_read=MT_RET_OK_NONE;
   for(uint32_t i=0;const DealBalance *deal_balance=balance.Deal(i);i++)
     {
      //--- call client balance read handler
      const MTAPIRES res=BalanceDealRead(client_user,client,*deal_balance,(uint32_t)section,rate);
      if(res!=MT_RET_OK_NONE)
        {
         if(res!=MT_RET_OK)
            return(res);
         res_read=MT_RET_OK;
        }
     }
//--- ok
   return(res_read);
  }
//+------------------------------------------------------------------+
//| Client and user balance deal read handler                        |
//+------------------------------------------------------------------+
MTAPIRES CLtvReport::BalanceDealRead(const ClientUserCache &client_user,const ClientCache &client,const DealBalance &balance,const uint32_t section,const double *rate)
  {
//--- check deal deposit balance
   if(balance.value<DBL_EPSILON)
      return(MT_RET_OK_NONE);
//--- deposit value
   const double value=rate ? m_currency.Convert(balance.value,*rate) : balance.value;
//--- add deposit to section
   return(m_section.DepositAdd(client_user.registration,balance.info.day*SECONDS_IN_DAY,section,value,balance.count));
  }
//+------------------------------------------------------------------+