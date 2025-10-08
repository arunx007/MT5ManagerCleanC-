//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealCountryReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CDealCountryReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_9,                        // minimal IE version
   L"Deals Geography",                                // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",                // description
   MTReportInfo::SNAPSHOT_NONE,                       // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                      // report types
   L"Trades",                                         // report category
                                                      // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,        DEFAULT_GROUPS        },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                 },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                   },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_SOURCE,   DEFAULT_LEAD_SOURCE   },
      { MTReportParam::TYPE_STRING,    MTAPI_PARAM_LEAD_CAMPAIGN, DEFAULT_LEAD_CAMPAIGN },
     },5,                                             // request parameters total
     {                                                // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,        DEFAULT_CURRENCY      },
      { MTReportParam::TYPE_INT,       PARAMETER_TOP_COUNT,       DEFAULT_TOP_COUNT     },
     },2                                              // config parameters total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealCountryReport::CDealCountryReport(void) :
   CDealSectionReport(s_info,CDealCache::DEAL_KEY_FIELD_COUNTRY)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealCountryReport::~CDealCountryReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDealCountryReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Factory method                                                   |
//+------------------------------------------------------------------+
IMTReportContext* CDealCountryReport::Create(void)
  {
   return(new (std::nothrow) CDealCountryReport());
  }
//+------------------------------------------------------------------+
//| prepare graph                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDealCountryReport::PrepareGraph(IMTDataset *data,const ReportColumn &column,const IMTReportDashboardWidget::EnWidgetType type)
  {
//--- prepare graph
   return(CDealSectionReport::PrepareGraph(data,column,type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_GEO : type));
  }
//+------------------------------------------------------------------+
