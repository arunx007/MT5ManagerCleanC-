//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealReasonReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CDealReasonReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_9,                        // minimal IE version
   L"Deals Initiators",                               // report name
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
CDealReasonReport::CDealReasonReport(void) :
   CDealSectionReport(s_info,CDealCache::DEAL_KEY_FIELD_REASON)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealReasonReport::~CDealReasonReport(void)
  {
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDealReasonReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Factory method                                                   |
//+------------------------------------------------------------------+
IMTReportContext* CDealReasonReport::Create(void)
  {
   return(new (std::nothrow) CDealReasonReport());
  }
//+------------------------------------------------------------------+
//| Fill top names                                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealReasonReport::FillTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names)
  {
//--- top count
   const uint32_t total=top.Total();
   if(!total)
      return(MT_RET_OK);
//--- initialize top names
   if(!names.Initialize(total))
      return(MT_RET_ERR_MEM);
//--- initialize top names
   for(uint32_t i=0;i<total;i++)
     {
      //--- get top pos
      const uint32_t *pos=top.Pos(i);
      if(!pos)
         return(MT_RET_ERROR);
      //--- check other
      if(*pos==CReportTopBase::TOP_POS_OTHER)
        {
         //--- add other top name
         if(!names.String(i,L"Other"))
            return(MT_RET_ERR_MEM);
        }
      else
        {
         //--- get top name
         LPCWSTR name=GetReasonTypeName(*pos);
         //--- store top name
         if(!names.String(i,name && *name ? name : L"None"))
            return(MT_RET_ERR_MEM);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get reason type as string                                        |
//+------------------------------------------------------------------+
LPCWSTR CDealReasonReport::GetReasonTypeName(const uint32_t reason)
  {
   switch(reason)
     {
      case IMTDeal::DEAL_REASON_CLIENT:
         return(L"Client");
      case IMTDeal::DEAL_REASON_EXPERT:
         return(L"Expert");
      case IMTDeal::DEAL_REASON_DEALER:
         return(L"Dealer");
      case IMTDeal::DEAL_REASON_SL:
         return(L"Stop loss");
      case IMTDeal::DEAL_REASON_TP:
         return(L"Take profit");
      case IMTDeal::DEAL_REASON_SO:
         return(L"Stop-Out");
      case IMTDeal::DEAL_REASON_ROLLOVER:
         return(L"Rollover");
      case IMTDeal::DEAL_REASON_EXTERNAL_CLIENT:
         return(L"External system");
      case IMTDeal::DEAL_REASON_VMARGIN:
         return(L"Variation margin");
      case IMTDeal::DEAL_REASON_GATEWAY:
         return(L"Gateway");
      case IMTDeal::DEAL_REASON_SIGNAL:
         return(L"Signal");
      case IMTDeal::DEAL_REASON_SETTLEMENT:
         return(L"Settlement");
      case IMTDeal::DEAL_REASON_TRANSFER:
         return(L"Transfer");
      case IMTDeal::DEAL_REASON_SYNC:
         return(L"Synchronization");
      case IMTDeal::DEAL_REASON_EXTERNAL_SERVICE:
         return(L"Service in external system");
      case IMTDeal::DEAL_REASON_MIGRATION:
         return(L"Migration");
      case IMTDeal::DEAL_REASON_MOBILE:
         return(L"Mobile");
      case IMTDeal::DEAL_REASON_WEB:
         return(L"Web");
      case IMTDeal::DEAL_REASON_SPLIT:
         return(L"Split");
      default:
         return(nullptr);
     }
  }
//+------------------------------------------------------------------+
