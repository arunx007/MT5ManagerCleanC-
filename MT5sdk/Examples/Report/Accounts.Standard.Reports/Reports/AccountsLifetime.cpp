//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "AccountsLifetime.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
const MTReportInfo CAccountsLifetime::s_info
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Accounts Lifetime",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_USERS | MTReportInfo::SNAPSHOT_ACCOUNTS,
   MTReportInfo::TYPE_DASHBOARD,
   L"Accounts",
   // request parameters
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },3                                          // request parameters total
  };
//--- Chart columns descriptions
const ReportColumn CAccountsLifetime::s_columns_chart[]
   //--- id,name,          type,                          width, width_max,offset,             size,                             digits_column,flags
  {
   { 1, L"Country",        IMTDatasetColumn::TYPE_STRING, 20,0, offsetof(ChartRecord,country), MtFieldSize(ChartRecord,country), 0, IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"Lifetime, days", IMTDatasetColumn::TYPE_UINT64, 10,0, offsetof(ChartRecord,lifetime) },
  };
//--- Table column descriptions
const ReportColumn CAccountsLifetime::s_columns_table[]
   //--- id,name,                  type,                          width, width_max,offset,               size,                             digits_column,flags
  {
   { 1,  L"Country",               IMTDatasetColumn::TYPE_STRING, 20, 0, offsetof(TableRecord, country), MtFieldSize(TableRecord,country), 0, IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Lifetime average, days",IMTDatasetColumn::TYPE_UINT64,  5, 0, offsetof(TableRecord, lifetime_average) },
   { 3,  L"Lifetime mobile, days", IMTDatasetColumn::TYPE_UINT64,  5, 0, offsetof(TableRecord, lifetime_mobile)  },
   { 4,  L"Lifetime desktop, days",IMTDatasetColumn::TYPE_UINT64,  5, 0, offsetof(TableRecord, lifetime_desktop) },
   { 5,  L"Lifetime web, days",    IMTDatasetColumn::TYPE_UINT64,  5, 0, offsetof(TableRecord, lifetime_web)     },
   { 6,  L"lifetime others, days", IMTDatasetColumn::TYPE_UINT64,  5, 0, offsetof(TableRecord, lifetime_others)  },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CAccountsLifetime::CAccountsLifetime(void) : CReportBase{s_info}
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CAccountsLifetime::~CAccountsLifetime(void)
  {
   CAccountsLifetime::Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CAccountsLifetime::Info(MTReportInfo &info)
  {
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CAccountsLifetime::Clear(void)
  {
//--- reset interval
   m_from=m_to=0;
//--- report request logins interface
   if(m_logins)
     {
      m_logins->Release();
      m_logins=nullptr;
     }
//--- user interface
   if(m_user)
     {
      m_user->Release();
      m_user=nullptr;
     }
//--- deals array interface
   if(m_deals)
     {
      m_deals->Release();
      m_deals=nullptr;
     }
//--- shutdown statistics
   m_stats.Shutdown();
   m_index.Shutdown();
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- report interval
   m_from=m_api->ParamFrom();
   m_to  =m_api->ParamTo();
//--- check positions history interval
   if(m_from>m_to)
      return(MT_RET_ERR_PARAMS);
//--- report logins
   m_logins=m_api->KeySetCreate();
   if(!m_logins)
      return(MT_RET_ERR_MEM);
//--- create deals array
   m_deals=m_api->DealCreateArray();
   if(!m_deals)
      return(MT_RET_ERR_MEM);
//--- create user
   m_user=m_api->UserCreate();
   if(!m_user)
      return(MT_RET_ERR_MEM);
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Report writing                                                   |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::Write(const uint32_t type)
  {
//--- checks
   if(!m_api || !m_logins)
      return(MT_RET_ERR_PARAMS);
//--- check type
   if(type!=MTReportInfo::TYPE_DASHBOARD)
      return(MT_RET_ERR_PARAMS);
//--- get report request logins
   MTAPIRES res=m_api->KeySetParamLogins(m_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- collect statistics
   if((res=StatisticsCalc())!=MT_RET_OK)
      return(res);
//--- write charts
   if((res=WriteCharts())!=MT_RET_OK)
      return(res);
//--- write table
   if((res=WriteTable())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Statistics calculation                                           |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::StatisticsCalc(void)
  {
//--- checks
   if(!m_api || !m_logins || !m_user || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- logins
   const uint64_t *logins=m_logins->Array();
   uint32_t total=m_logins->Total();
   if(logins)
      for(;total;logins++,total--)
        {
         //--- get user
         if(m_api->UserGet(*logins,m_user)!=MT_RET_OK)
            continue;
         //--- user country
         const LPCWSTR country=m_user->Country();
         if(!country)
            continue;
         //--- user lifetime
         int64_t lifetime_sec=m_user->LastAccess()-m_user->Registration();
         if(lifetime_sec<0)
            lifetime_sec=0;
         //--- convert seconds to days
         const double lifetime=(double)lifetime_sec/SECONDS_IN_DAY;
         //--- determine user terminal type
         EnUserType type=USER_TYPE_ANY;
         uint32_t trades=0;
         if(!DetermineUserTypeByDeals(*logins,type,trades))
            continue;
         //--- add statistics
         MTAPIRES res=StatisticsAdd(country,lifetime,type);
         if(res!=MT_RET_OK)
            return(res);
        }
//--- calculate average statistics values
   for(StatRecord &i : m_stats)
      for(uint32_t j=0;j<_countof(i.data);j++)
         if(i.count[j])
            i.data[j]/=i.count[j];
//--- reserve statistics index
   if(!m_index.Reserve(uint32_t(m_stats.Size())))
      return MT_RET_ERR_MEM;
//--- fill statistics index
   for(StatRecord &i : m_stats)
     {
      StatRecord *ptr=&i;
      m_index.Add(&ptr);
     }
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Write charts                                                     |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::WriteCharts(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- append charts dataset
   IMTDataset *dataset=m_api->DatasetAppend();
   if(!dataset)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*dataset,s_columns_chart,_countof(s_columns_chart));
   if(res!=MT_RET_OK)
      return(res);
//--- sort statistics index
   m_index.Sort(&CAccountsLifetime::StatisticsSortData);
//--- fill dataset
   ChartRecord record{};
   for(uint32_t i=0;i<m_index.Total();i++)
      if(const StatRecord *stat=m_index[i])
         if(stat->count[USER_TYPE_ANY]>=ACCOUNTS_THRESHOLD)
           {
            //--- fill dataset record
            CMTStr::Copy(record.country,stat->country);
            record.lifetime=uint64_t(stat->data[USER_TYPE_ANY]);
            //--- write dataset row
            if((res=dataset->RowWrite(&record,sizeof(record)))!=MT_RET_OK)
               return(res);
           }
//--- append charts
   if((res=WidgetAppend(dataset,L"Trade Accounts Lifetime Average",IMTReportDashboardWidget::WIDGET_TYPE_CHART_GEO))!=MT_RET_OK)
      return(res);
   if((res=WidgetAppend(dataset,L"Trade Accounts Lifetime Average",IMTReportDashboardWidget::WIDGET_TYPE_CHART_BAR))!=MT_RET_OK)
      return(res);
//--- ok
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Write table                                                      |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::WriteTable(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- append charts dataset
   IMTDataset *dataset=m_api->DatasetAppend();
   if(!dataset)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*dataset,s_columns_table,_countof(s_columns_table));
   if(res!=MT_RET_OK)
      return(res);
//--- sort statistics index
   m_index.Sort(&CAccountsLifetime::StatisticsSortCountry);
//--- fill dataset
   TableRecord record{};
   for(uint32_t i=0;i<m_index.Total();i++)
      if(const StatRecord *stat=m_index[i])
        {
         //--- fill dataset record
         CMTStr::Copy(record.country,stat->country);
         record.lifetime_average=stat->count[USER_TYPE_ANY    ]>=ACCOUNTS_THRESHOLD ? uint64_t(stat->data[USER_TYPE_ANY    ]) : 0;
         record.lifetime_mobile =stat->count[USER_TYPE_MOBILE ]>=ACCOUNTS_THRESHOLD ? uint64_t(stat->data[USER_TYPE_MOBILE ]) : 0;
         record.lifetime_desktop=stat->count[USER_TYPE_DESKTOP]>=ACCOUNTS_THRESHOLD ? uint64_t(stat->data[USER_TYPE_DESKTOP]) : 0;
         record.lifetime_web    =stat->count[USER_TYPE_WEB    ]>=ACCOUNTS_THRESHOLD ? uint64_t(stat->data[USER_TYPE_WEB    ]) : 0;
         record.lifetime_others =stat->count[USER_TYPE_OTHERS ]>=ACCOUNTS_THRESHOLD ? uint64_t(stat->data[USER_TYPE_OTHERS ]) : 0;
         //--- write dataset row
         if((res=dataset->RowWrite(&record,sizeof(record)))!=MT_RET_OK)
            return(res);
        }
//--- append table
   if((res=WidgetAppend(dataset,L"Trade Accounts Lifetime Detailed",IMTReportDashboardWidget::WIDGET_TYPE_TABLE))!=MT_RET_OK)
      return(res);
//--- ok
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Append widget                                                    |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::WidgetAppend(IMTDataset *data,const LPCWSTR title,const IMTReportDashboardWidget::EnWidgetType type)
  {
//--- checks
   if(!m_api || !data || !title)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- widget height
   MTAPIRES res=MT_RET_OK;
   if(type!=IMTReportDashboardWidget::WIDGET_TYPE_TABLE)
     {
      res=chart->Height(type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_GEO ? CHART_HEIGHT_PRIMARY : CHART_HEIGHT_SECONDARY);
      if(res!=MT_RET_OK)
         return(res);
     }
//--- chart type
   if((res=chart->Type(type))!=MT_RET_OK)
      return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- ok
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Determine user terminal type                                     |
//+------------------------------------------------------------------+
bool CAccountsLifetime::DetermineUserTypeByDeals(const uint64_t login,EnUserType &type,uint32_t &trades)
  {
//--- reset return values
   type=USER_TYPE_OTHERS;
   trades=0;
//--- checks
   if(!m_api || !m_deals || !m_user)
      return(false);
//--- get deals
   MTAPIRES res=m_api->DealGet(m_user->Login(),m_api->ParamFrom(),m_api->ParamTo(),m_deals);
   if(res!=MT_RET_OK)
      return(false);
//--- default value (if no deals)
   if(m_deals->Total()==0)
      return(true);
//--- deal_reason statistics
   for(uint32_t i=0,deal_total=m_deals->Total();i<deal_total;i++)
      if(IMTDeal *deal=m_deals->Next(i))
         switch(deal->Action())
           {
            //--- buy or sell
            case IMTDeal::DEAL_BUY:
            case IMTDeal::DEAL_SELL:
               //--- calculate trade deals
               trades++;
               //--- identify terminal
               switch(deal->Reason())
                 {
                  case IMTDeal::DEAL_REASON_MOBILE:
                     type=USER_TYPE_MOBILE;
                     break;

                  case IMTDeal::DEAL_REASON_CLIENT:
                  case IMTDeal::DEAL_REASON_EXPERT:
                  case IMTDeal::DEAL_REASON_SIGNAL:
                     type=USER_TYPE_DESKTOP;
                     break;

                  case IMTDeal::DEAL_REASON_WEB:
                     type=USER_TYPE_WEB;
                     break;
                 }
               break;
           }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Add data to statistics                                           |
//+------------------------------------------------------------------+
MTAPIRES CAccountsLifetime::StatisticsAdd(const LPCWSTR country,const double lifetime,const EnUserType type)
  {
   if(!country || type>=USER_TYPE_COUNT)
      return MT_RET_ERR_PARAMS;
//--- find statistics record
   StatRecord *stat=m_stats.Find(country);
   if(!stat)
     {
      //--- insert new statistics record
      StatRecord record{};
      CMTStr::Copy(record.country,country);
      stat=m_stats.Insert(record);
      if(!stat)
         return MT_RET_ERR_MEM;
     }
//--- statistics
   stat->data[type]+=lifetime;
   stat->count[type]++;
//--- total
   stat->data[USER_TYPE_ANY]+=lifetime;
   stat->count[USER_TYPE_ANY]++;
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Sort statistics by data                                          |
//+------------------------------------------------------------------+
int CAccountsLifetime::StatisticsSortData(const void *left,const void *right)
  {
//--- cast pointers
   const StatRecord *l=*reinterpret_cast<StatRecord* const*>(left);
   const StatRecord *r=*reinterpret_cast<StatRecord* const*>(right);
//--- compare
   if(l->data[USER_TYPE_ANY]>r->data[USER_TYPE_ANY])
      return(-1);
   if(l->data[USER_TYPE_ANY]<r->data[USER_TYPE_ANY])
      return(1);
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort statistics by country                                       |
//+------------------------------------------------------------------+
int CAccountsLifetime::StatisticsSortCountry(const void *left,const void *right)
  {
//--- cast pointers
   const StatRecord *l=*reinterpret_cast<StatRecord* const*>(left);
   const StatRecord *r=*reinterpret_cast<StatRecord* const*>(right);
//--- compare
   return CMTStr::CompareNoCase(l->country,r->country);
  }
//+------------------------------------------------------------------+
