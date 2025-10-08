//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MoneyFlowDailyLeadSourceReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CMoneyFlowDailyLeadSource::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_9,                     // minimal IE version
   L"Money Flow Daily by Lead Source",             // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                   // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,  DEFAULT_GROUPS    },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                       },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                         },
     },3,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,  DEFAULT_CURRENCY  },
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT, DEFAULT_TOP_COUNT },
     },2                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMoneyFlowDailyLeadSource::CMoneyFlowDailyLeadSource(void) :
   CMoneyFlowDailySection(s_info,offsetof(ClientCache,lead_source))
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMoneyFlowDailyLeadSource::~CMoneyFlowDailyLeadSource(void)
  {
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CMoneyFlowDailyLeadSource::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CMoneyFlowDailyLeadSource::Create(void)
  {
   return(new (std::nothrow) CMoneyFlowDailyLeadSource());
  }
//+------------------------------------------------------------------+
