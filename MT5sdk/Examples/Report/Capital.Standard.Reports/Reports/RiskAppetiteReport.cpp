//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "RiskAppetiteReport.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define MIN_DEALS_PARAMETER               L"Minimum Deals"
#define MIN_DEALS_DEFAULT                 L"10"
#define CHART_PERCENTILE_PARAMETER        L"Distribution percentile, %"
#define CHART_PERCENTILE_DEFAULT          L"80"
#define TOP_COUNT_PARAMETER               L"Top Count"
#define TOP_COUNT_DEFAULT                 L"100"
#define TOP_COUNT_MAX                     100000
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CRiskAppetiteReport::s_info=
  {
   100,                                               // report version
   MTReportAPIVersion,                                // report API version
   MTReportInfo::IE_VERSION_ANY,                      // minimal IE version
   L"Risk Appetite",                                  // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.", // copyright
   L"MetaTrader 5 Report API plug-in",                // description
   MTReportInfo::SNAPSHOT_NONE,                       // snapshot modes
   MTReportInfo::TYPE_DASHBOARD,                      // report types
   L"Trades",                                         // report category
                                                      // request parameters
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS,         DEFAULT_GROUPS           },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM                                     },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO                                       },
      { MTReportParam::TYPE_STRING, MTAPI_PARAM_LEAD_SOURCE,    DEFAULT_LEAD_SOURCE      },
      { MTReportParam::TYPE_STRING, MTAPI_PARAM_LEAD_CAMPAIGN,  DEFAULT_LEAD_CAMPAIGN    },
     },5,                                             // request parameters total
     {                                                // config parameters
      { MTReportParam::TYPE_INT,    PARAMETER_DATA_LATENCY_MAX, DEFAULT_DATA_LATENCY_MAX },
      { MTReportParam::TYPE_INT,    MIN_DEALS_PARAMETER       , MIN_DEALS_DEFAULT        },
      { MTReportParam::TYPE_INT,    CHART_PERCENTILE_PARAMETER, CHART_PERCENTILE_DEFAULT },
      { MTReportParam::TYPE_INT,    TOP_COUNT_PARAMETER       , TOP_COUNT_DEFAULT        },
     },4                                              // config parameters total
  };
//+------------------------------------------------------------------+
//| Chart columns descriptions                                       |
//+------------------------------------------------------------------+
const ReportColumn CRiskAppetiteReport::s_columns_chart[]=
   //--- id, name,                            type,                          width,width_max, offset,                size, digits_column, flags
  {{ 1, L"Ratio",                             IMTDatasetColumn::TYPE_DOUBLE, 10, 0, offsetof(ChartRecord,ratio)      , 0, 0, IMTDatasetColumn::FLAG_PRIMARY },
   { 2, L"TP/SL Ratio Trading Accounts",      IMTDatasetColumn::TYPE_UINT32, 10, 0, offsetof(ChartRecord,count_tp_sl), 0, 0, 0 },
   { 3, L"Profit/Loss Ratio Trading Accounts",IMTDatasetColumn::TYPE_UINT32, 10, 0, offsetof(ChartRecord,count_pl)   , 0, 0, 0 },
  };
//+------------------------------------------------------------------+
//| Table columns descriptions                                       |
//+------------------------------------------------------------------+
const ReportColumn CRiskAppetiteReport::s_columns_table[]=
   //--- id,                  name,                 type,                             width,width_max, offset,                          size,                          digits_column, flags
  {{ COLUMN_LOGIN,            L"Login",             IMTDatasetColumn::TYPE_USER_LOGIN,10,   0, offsetof(TableRecord,login),             0,                             0,             IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_NAME,             L"Name",              IMTDatasetColumn::TYPE_STRING,    20,   0, offsetof(TableRecord,name),              MtFieldSize(TableRecord,name), 0,             IMTDatasetColumn::FLAG_LEFT    },
   { COLUMN_REGISTRATION_TIME,L"Registration Time", IMTDatasetColumn::TYPE_DATETIME,  16, 150, offsetof(TableRecord,registration_time), 0,                             0,             0                              },
   { COLUMN_SL               ,L"Stop Loss, %",      IMTDatasetColumn::TYPE_DOUBLE,    10,   0, offsetof(TableRecord,sl),                0,                            -4,             0                              },
   { COLUMN_TP               ,L"Take Profit, %",    IMTDatasetColumn::TYPE_DOUBLE,    10,   0, offsetof(TableRecord,tp),                0,                            -4,             0                              },
   { COLUMN_TP_SL_RATIO      ,L"TP/SL Ratio",       IMTDatasetColumn::TYPE_DOUBLE,    10,   0, offsetof(TableRecord,tp_sl_ratio),       0,                            -4,             0                              },
   { COLUMN_LOSS             ,L"Loss, %",           IMTDatasetColumn::TYPE_DOUBLE,    10,   0, offsetof(TableRecord,loss),              0,                            -4,             0                              },
   { COLUMN_PROFIT           ,L"Profit, %",         IMTDatasetColumn::TYPE_DOUBLE,    10,   0, offsetof(TableRecord,profit),            0,                            -4,             0                              },
   { COLUMN_PROFIT_LOSS_RATIO,L"Profit/Loss Ratio", IMTDatasetColumn::TYPE_DOUBLE,    10,   0, offsetof(TableRecord,profit_loss_ratio), 0,                            -4,             0                              },
  };
//--- Chart description
const LPCWSTR CRiskAppetiteReport::s_description_chart=
L"Profit/Loss and Take Profit/Stop Loss ratios on accounts (%uth percentile).\n"
L"Average parameter values for each account's profitable and loss-making positions are calculated, and their ratio is used afterwards.\n"
L"Y axis shows how many accounts have a certain ratio of parameter values.";
//--- Table description
const LPCWSTR CRiskAppetiteReport::s_description_table=
L"The first %u accounts with the highest profit-to-loss ratios.\n"
L" * Stop Loss, %% - average difference between positions open price and their Stop Loss.\n"
L" * Take Profit, %% - average difference between positions open price and their Take Profit.\n"
L" * Loss, %% - average difference between open and close prices of loss-making positions.\n"
L" * Profit, %% - average difference between open and close prices of profitable positions.\n"
L" * TP/SL Ratio - ratio of the previously specified Take Profit, %% and Stop Loss, %%.\n"
L" * Profit/Loss Ratio - ratio of the previously specified Profit, %% and Loss, %%.\n";
//+------------------------------------------------------------------+
//| Constants                                                        |
//+------------------------------------------------------------------+
const double CRiskAppetiteReport::s_ratio_step=0.1;           // ratio step
const uint32_t CRiskAppetiteReport::s_ratio_max=1000;             // ratio max steps
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CRiskAppetiteReport::CRiskAppetiteReport(void) :
   CCurrencyReport(s_info),m_users(nullptr),m_deals(nullptr),m_from(0),m_to(0),
   m_min_deals(0),m_chart_percentile(0),m_top_count(0),m_ratios(true),m_ratio_accounts(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CRiskAppetiteReport::~CRiskAppetiteReport(void)
  {
//--- clear common data
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CRiskAppetiteReport::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Fabric method                                                    |
//+------------------------------------------------------------------+
IMTReportContext* CRiskAppetiteReport::Create(void)
  {
   return(new (std::nothrow) CRiskAppetiteReport());
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CRiskAppetiteReport::Clear(void)
  {
//--- release user cache
   if(m_users)
     {
      m_users->Release();
      m_users=nullptr;
     }
//--- release deals statistics cache
   if(m_deals)
     {
      m_deals->Release();
      m_deals=nullptr;
     }
//--- clear ratio vector
   m_ratios.Clear();
//--- clear top array
   for(uint32_t i=0;i<m_top.Total();i++)
      delete m_top[i];
   m_top.Clear();
  }
//+------------------------------------------------------------------+
//| Prepare to calculation                                           |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::Prepare(void)
  {
//--- checks
   if(m_api==nullptr)
      return(MT_RET_ERR_PARAMS);
//--- get range
   m_from=m_api->ParamFrom();
   m_to  =m_api->ParamTo();
   const int64_t now=CReportCache::Now(*m_api,m_params);
   if(m_to>now)
      m_to=now;
//--- check range
   if(m_to<m_from)
      return(MT_RET_ERR_PARAMS);
//--- get minimum deals from report parameter
   int64_t value=0;
   MTAPIRES res=m_params.ValueInt(value,MIN_DEALS_PARAMETER,MIN_DEALS_DEFAULT);
   if(res!=MT_RET_OK)
      return(res);
//--- check minimum deals
   if(value<0 || value>UINT_MAX)
      return(MT_RET_ERR_PARAMS);
   m_min_deals=(uint32_t)value;
//--- get chart percentile from report parameter
   if((res=m_params.ValueInt(value,CHART_PERCENTILE_PARAMETER,CHART_PERCENTILE_DEFAULT))!=MT_RET_OK)
      return(res);
//--- check chart percentile
   if(value<0 || value>100)
      return(MT_RET_ERR_PARAMS);
   m_chart_percentile=(uint32_t)value;
//--- get top count from report parameter
   if((res=m_params.ValueInt(value,TOP_COUNT_PARAMETER,TOP_COUNT_DEFAULT))!=MT_RET_OK)
      return(res);
//--- check top count
   if(value<0 || value>TOP_COUNT_MAX)
      return(MT_RET_ERR_PARAMS);
   m_top_count=(uint32_t)value;
//--- user cache
   m_users=CUserCache::Create(*m_api,m_params,nullptr,false,res);
   if(!m_users)
      return(res);
//--- deals statistics cache
   m_deals=CDealUserCache::Create(*m_api,m_from,m_to,res);
   if(!m_deals)
      return(res);
//---- initialize ratio array
   if(!m_ratios.Initialize(s_ratio_max))
      return(MT_RET_ERR_MEM);
//---- initialize ratios
   for(uint32_t i=0;ChartRecord *record=m_ratios.Item(i);i++)
      record->ratio=double(i)*s_ratio_step;
//--- reserve top
   if(!m_top.Reserve(m_top_count))
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::Calculate(void)
  {
//--- checks
   if(!m_api || !m_users || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- update users
   MTAPIRES res=m_users->UpdateFilter(0,m_to);
   if(res!=MT_RET_OK)
      return(res);
//--- update deals statistics
   if((res=m_deals->Update(m_users->Logins(),m_from,m_to))!=MT_RET_OK)
      return(res);
//--- calculate all statistics
   return(CalculateStatistics());
  }
//+------------------------------------------------------------------+
//| Calculate all statistics                                         |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::CalculateStatistics(void)
  {
//--- checks
   if(!m_api || !m_users || !m_deals)
      return(MT_RET_ERR_PARAMS);
//--- begin read deals statistics
   const CReportTimer timer_wait;
   MTAPIRES res=m_deals->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
   const uint64_t time_wait=timer_wait.Elapsed();
//--- read users
   const CReportTimer timer_read;
   res=m_users->ReadAll(*this);
//--- end read deals statistics
   const MTAPIRES res_deals=m_deals->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
     {
      LogError(res,L"Statistics calculation");
      return(res);
     }
//--- check error
   if((res=res_deals)!=MT_RET_OK)
     {
      LogError(res,L"Statistics calculation finishing");
      return(res);
     }
//--- write statistics to log
   return(m_api->LoggerOut(MTLogOK,L"Calculated statistics for %u accounts in %I64u ms (cache read wait %I64u ms)",m_users->Logins().Total(),timer_read.Elapsed(),time_wait));
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::PrepareGraphs(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_chart,_countof(s_columns_chart));
   if(res!=MT_RET_OK)
      return(res);
//--- search distribution vector values range
   m_ratio_accounts=MulDiv(m_ratio_accounts,m_chart_percentile,100);
   uint32_t first=UINT_MAX,last=0;
   for(uint32_t i=0,accounts=0;const ChartRecord *record=m_ratios.Item(i);i++)
      if(record->count_pl)
        {
         first=std::min(first,i);
         last=std::max(last,i);
         accounts+=record->count_pl;
         if(accounts>m_ratio_accounts)
            break;
        }
//--- write rows
   for(uint32_t i=first;i<=last;i++)
      if(const ChartRecord *record=m_ratios.Item(i))
         if((res=data->RowWrite(record,sizeof(*record)))!=MT_RET_OK)
            return(res);
//--- add ratio chart
   if((res=PrepareRatioGraph(data))!=MT_RET_OK)
      return(res);
//--- prepate top table
   return(PrepareTable());
  }
//+------------------------------------------------------------------+
//| Prepare ratio graph                                              |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::PrepareRatioGraph(IMTDataset *data)
  {
//--- checks
   if(!data || !m_api)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(IMTReportDashboardWidget::WIDGET_TYPE_CHART_BAR);
   if(res!=MT_RET_OK)
      return(res);
//--- set title
   CMTStr1024 str;
   str.Format(L"TP/SL and Profit/Loss Ratio Distribution, %uth percentile",m_chart_percentile);
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(res);
//--- set description
   str.Format(s_description_chart,m_chart_percentile);
   if((res=chart->Description(str.Str()))!=MT_RET_OK)
      return(res);
//--- set height
   if((res=chart->Height(CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- checks
   if(!m_deals)
      return(MT_RET_ERR_PARAMS);
//--- get user deals statistics
   const CDealUserCache::DealStat *stat=nullptr;
   MTAPIRES res=m_deals->ReadStat(login,stat);
   if(res!=MT_RET_OK || !stat)
      return(res==MT_RET_OK_NONE ? MT_RET_OK : res);
//--- calc ratio
   const double sl         =PriceDiffAverage(stat->loss.ordered,1);
   const double tp         =PriceDiffAverage(stat->profit.ordered,1);
   const double tp_sl_ratio=_isnan(tp) || _isnan(sl) || fabs(sl)<DBL_EPSILON ? NAN : tp/sl;
   const double loss       =PriceDiffAverage(stat->loss.actual,m_min_deals);
   const double profit     =PriceDiffAverage(stat->profit.actual,m_min_deals);
   const double ratio      =_isnan(profit) || _isnan(loss) || fabs(loss)<DBL_EPSILON ? NAN : profit/loss;
//--- add profit/loss ratio to distribution
   AddRatio(ratio,tp_sl_ratio);
//--- check ratio
   if(_isnan(ratio))
      return(MT_RET_OK);
//--- check profit/loss ratio to place in top
   if(!CheckTopRecord(ratio))
      return(MT_RET_OK);
//--- fill new record
   TableRecord *record=new(std::nothrow) TableRecord;
   if(!record)
      return(MT_RET_ERR_MEM);
   record->login            =login;
   ZeroMemory(&record->name,sizeof(record->name));
   record->registration_time=user.registration;
   record->name_id          =user.name;
//--- fill record by deals statistics
   record->sl               =sl;
   record->tp               =tp;
   record->tp_sl_ratio      =tp_sl_ratio;
   record->loss             =loss;
   record->profit           =profit;
   record->profit_loss_ratio=ratio;
//--- add top record
   if((res=AddTopRecord(record))!=MT_RET_OK)
     {
      delete record;
      return(res);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add ratios to distribution                                       |
//+------------------------------------------------------------------+
void CRiskAppetiteReport::AddRatio(const double ratio,const double tp_sl_ratio)
  {
//--- count profit/loss ratio
   ChartRecord *record=!_isnan(ratio) ? m_ratios.Item(uint32_t(ratio/s_ratio_step)) : nullptr;
   if(record)
     {
      record->count_pl++;
      m_ratio_accounts++;
     }
//--- count TP/SL ratio
   record=!_isnan(tp_sl_ratio) ? m_ratios.Item(uint32_t(tp_sl_ratio/s_ratio_step)) : nullptr;
   if(record)
      record->count_tp_sl++;
  }
//+------------------------------------------------------------------+
//| Check profit/loss ratio to place in top                          |
//+------------------------------------------------------------------+
bool CRiskAppetiteReport::CheckTopRecord(const double ratio) const
  {
//--- check top
   if(!m_top_count)
      return(false);
//--- check top underflow
   if(m_top.Total()<m_top_count)
      return(true);
//--- compare with last record
   const TableRecord *last=m_top[m_top_count-1];
   return(ratio>last->profit_loss_ratio);
  }
//+------------------------------------------------------------------+
//| Check profit/loss ratio to place in top                          |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::AddTopRecord(TableRecord *record)
  {
//--- check record
   if(!record)
      return(MT_RET_ERR_PARAMS);
//--- find place in top
   TableRecord *const *place=m_top.SearchGreater(&record->profit_loss_ratio,SearchTableRecordByRatio);
//--- insert to top
   if(!(place ? m_top.Insert(m_top.Position(place),&record) : m_top.Add(&record)))
      return(MT_RET_ERR_MEM);
//--- clear top bottom
   if(m_top.Total()>m_top_count)
     {
      //--- delete last top record
      TableRecord *to_delete=m_top[m_top.Total()-1];
      if(m_top.Delete(m_top.Total()-1))
         delete to_delete;
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepate top table                                                |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::PrepareTable(void)
  {
//--- check
   if(!m_api || !m_users)
      return(MT_RET_ERR_PARAMS);
//--- check top count
   if(!m_top_count)
      return(MT_RET_OK);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_table,_countof(s_columns_table));
   if(res!=MT_RET_OK)
      return(res);
//--- begin read users cache
   res=m_users->ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- write top accounts data
   res=WriteTopData(*data);
//--- end read users cache
   const MTAPIRES res_users=m_users->ReadEnd();
//--- check error
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
//--- check error
   if((res=res_users)!=MT_RET_OK)
      return(res);
//--- add top table
   CMTStr128 str;
   str.Format(L"Top-%u Trading Accounts by Profit/Loss Ratio",m_top_count);
   CMTStr1024 desc;
   desc.Format(s_description_table,m_top_count);
   IMTReportDashboardWidget *table=ReportColumn::TableAdd(*m_api,res,data,str.Str(),desc.Str());
   if(!table)
      return(res);
//--- set height
   if((res=table->Height(CHART_HEIGHT_MAIN))!=MT_RET_OK)
      return(res);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write top accounts data                                           |
//+------------------------------------------------------------------+
MTAPIRES CRiskAppetiteReport::WriteTopData(IMTDataset &data)
  {
//--- check
   if(!m_users)
      return(MT_RET_ERR_PARAMS);
//--- write rows
   for(uint32_t i=0;i<m_top.Total();i++)
      if(TableRecord *record=m_top[i])
        {
         //--- fill user name
         LPCWSTR string=nullptr;
         MTAPIRES res=m_users->ReadDictionaryString(offsetof(CUserCache::UserCache,name),record->name_id,string);
         if(res!=MT_RET_OK)
            return(res);
         //--- write row
         CMTStr::Copy(record->name,string);
         if((res=data.RowWrite(record,offsetof(TableRecord,name_id)))!=MT_RET_OK)
            return(res);
        }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate price difference average value                         |
//+------------------------------------------------------------------+
double CRiskAppetiteReport::PriceDiffAverage(const CDealUserCache::DealPrice &aggregate,const uint32_t min_deals)
  {
   return(aggregate.count>=min_deals ? aggregate.price_diff*100.0/aggregate.count : NAN);
  }
//+------------------------------------------------------------------+
//| search function table record by ratio                            |
//+------------------------------------------------------------------+
int32_t CRiskAppetiteReport::SearchTableRecordByRatio(const void *left,const void *right)
  {
   const double ratio=*(const double*)left;
   const TableRecord *record=*(TableRecord *const *)right;
   return(ratio<record->profit_loss_ratio ? 1 : ratio>record->profit_loss_ratio ? -1 : 0);
  }
//+------------------------------------------------------------------+
