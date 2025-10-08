//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "LtvDetailedLeadSourceReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CLtvDetailedLeadSource::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_ANY,                   // minimal IE version
   L"Lifetime Value Detailed by Lead Source",      // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_TABLE,                       // report types
   L"Capital",                                     // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,           DEFAULT_GROUPS             },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                           },
     },3,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,           DEFAULT_CURRENCY           },
      { MTReportParam::TYPE_INT,       PARAMETER_DATA_LATENCY_MAX,   DEFAULT_DATA_LATENCY_MAX   },
     },2                                           // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CLtvDetailedLeadSource::CLtvDetailedLeadSource(void) :
   CLtvDetailedSection(s_info,offsetof(ClientCache,lead_source),L"Lead Source")
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CLtvDetailedLeadSource::~CLtvDetailedLeadSource(void)
  {
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CLtvDetailedLeadSource::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CLtvDetailedLeadSource::Create(void)
  {
   return(new (std::nothrow) CLtvDetailedLeadSource());
  }
//+------------------------------------------------------------------+
