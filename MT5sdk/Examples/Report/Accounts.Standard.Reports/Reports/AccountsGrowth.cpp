//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "AccountsGrowth.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
const MTReportInfo CAccountsGrowth::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_9,
   L"Accounts Growth",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NO_GROUPS_LOGINS,
   MTReportInfo::TYPE_HTML|MTReportInfo::TYPE_DASHBOARD,
   L"Accounts",
   };
//--- Column descriptions Grow
const ReportColumn CAccountsGrowth::s_columns_grow[]=
   //--- id,name,       type,                         width,  width_max, offset,             size,digits_column,flags
  {{ 1,  L"Date",       IMTDatasetColumn::TYPE_DATE,   20,0, offsetof(GrowRecord,date),       0,     0, IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Real",       IMTDatasetColumn::TYPE_UINT32, 10,0, offsetof(GrowRecord,real),       0,     0, 0                             },
   { 3,  L"Preliminary",IMTDatasetColumn::TYPE_UINT32, 10,0, offsetof(GrowRecord,preliminary),0,     0, 0                             },
   { 4,  L"Demo",       IMTDatasetColumn::TYPE_UINT32, 10,0, offsetof(GrowRecord,demo),       0,     0, 0                             },
   { 5,  L"Contest",    IMTDatasetColumn::TYPE_UINT32, 10,0, offsetof(GrowRecord,contest),    0,     0, 0                             },
  };
//--- Column description Month
const ReportColumn CAccountsGrowth::s_column_month=
   //--- id,name,        type,                         width,  width_max, offset,          size,digits_column,flags
   { 1,  L"Month",       IMTDatasetColumn::TYPE_STRING,10,0, offsetof(MonthRecord,month),  MtFieldSize(MonthRecord,month),0,IMTDatasetColumn::FLAG_PRIMARY };
//--- Column description Year
const ReportColumn CAccountsGrowth::s_column_year=
   //--- id,name,        type,                         width,  width_max, offset,          size,digits_column,flags
   { 2,  L"%04u",        IMTDatasetColumn::TYPE_UINT32,10,0, offsetof(MonthRecord,real[0]),0,     0, 0                             };
//--- Column description Active
const ReportColumn CAccountsGrowth::s_columns_active[]=
   //--- id,name,        type,                         width,  width_max, offset,          size,digits_column,flags
  {{ 1,  L"Account type",IMTDatasetColumn::TYPE_STRING,20,0, offsetof(ActiveRecord,type)  ,MtFieldSize(ActiveRecord,type),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Count",       IMTDatasetColumn::TYPE_UINT32,10,0, offsetof(ActiveRecord,active),0,                             0, 0                             },
  };
//--- User request fields descriptions
const DatasetField CAccountsGrowth::s_user_fields[]=
   //--- id                                  , select, offset
  {{ IMTDatasetField::FIELD_USER_LOGIN       , true, offsetof(UserRecord,login) },
   { IMTDatasetField::FIELD_USER_GROUP      },
   { IMTDatasetField::FIELD_USER_REGISTRATION, true, offsetof(UserRecord,registration) },
   { IMTDatasetField::FIELD_USER_LAST_ACCESS , true, offsetof(UserRecord,last_access) },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CAccountsGrowth::CAccountsGrowth(void) :
                                     CReportBase(s_info),
                                     m_ctm(0),m_checktime(0),
                                     m_month_data(0),m_month_firstyear(0),
                                     m_real_active(0),m_real_total(0),
                                     m_preliminary_active(0),m_preliminary_total(0),
                                     m_demo_active(0),m_demo_total(0),
                                     m_contest_active(0),m_contest_total(0),
                                     m_group(nullptr),
                                     m_chart_grow(nullptr),
                                     m_chart_real(nullptr),
                                     m_chart_preliminary(nullptr),
                                     m_chart_demo(nullptr),
                                     m_chart_contest(nullptr),
                                     m_chart_active(nullptr)
  {
//---
   ZeroMemory(&m_ctm_ttm,sizeof(m_ctm_ttm));
//---
   ZeroMemory(m_real       ,sizeof(m_real));
   ZeroMemory(m_preliminary,sizeof(m_preliminary));
   ZeroMemory(m_demo       ,sizeof(m_demo));
   ZeroMemory(m_contest    ,sizeof(m_contest));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CAccountsGrowth::~CAccountsGrowth(void)
  {
   CAccountsGrowth::Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CAccountsGrowth::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- parse current time
   m_ctm=m_api->TimeGeneration();
   if(!SMTTime::ParseTime(m_ctm,&m_ctm_ttm))
      return(MT_RET_ERROR);
//--- activity check time
   m_checktime=m_ctm-PERIOD_INACTIVE_MONTHS*SECONDS_IN_MONTH;
//--- create group interface
   if(!(m_group=m_api->GroupCreate()))
      return(MT_RET_ERR_MEM);
//--- prepare data for charts
   MTAPIRES res=PrepareData();
   if(res!=MT_RET_OK)
      return(res);
//--- first month index for first year
   m_month_firstyear=11-m_ctm_ttm.tm_mon;
//--- first month index with somebody
   for(uint32_t i=m_month_firstyear;i<PERIOD_MONTHS-1;i++)
      if(m_real[i] || m_demo[i] || m_contest[i])
        {
         m_month_data=i;
         break;
        }
//--- return result
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::Write(const uint32_t type)
  {
//--- check report type
   switch(type)
     {
      case MTReportInfo::TYPE_HTML:       return(GenerateHtml());
      case MTReportInfo::TYPE_DASHBOARD:  return(GenerateDashboard());
     }
   return(MT_RET_ERR_NOTIMPLEMENT);
  }
//+------------------------------------------------------------------+
//| Report generation HTML                                           |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::GenerateHtml(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
   MTAPISTR tag={ 0 };
   MTAPIRES res=MT_RET_OK;
//--- prepare charts
   if((res=PrepareChartGrow())       !=MT_RET_OK ||
      (res=PrepareChartReal())       !=MT_RET_OK ||
      (res=PrepareChartPreliminary())!=MT_RET_OK ||
      (res=PrepareChartDemo())       !=MT_RET_OK ||
      (res=PrepareChartContest())    !=MT_RET_OK ||
      (res=PrepareChartActive())     !=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"prepare chart failed [%u]",res);
      return(res);
     }
//--- use template
   if((res=m_api->HtmlTplLoadResource(IDR_ACCOUNTS_GROWTH,RT_HTML))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"HtmlTplLoadResource failed [%u]",res);
      return(res);
     }
//--- process tags
   while(m_api->HtmlTplNext(tag,nullptr)==MT_RET_OK)
     {
      //--- Accounts Grow Chart
      if(CMTStr::CompareNoCase(tag,L"chart_grow")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_grow))!=MT_RET_OK)
            break;
         continue;
        }
      //--- Real Accounts by months Chart
      if(CMTStr::CompareNoCase(tag,L"chart_real")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_real))!=MT_RET_OK)
            break;
         continue;
        }
      //--- Preliminary Accounts by months Chart
      if(CMTStr::CompareNoCase(tag,L"chart_preliminary")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_preliminary))!=MT_RET_OK)
            break;
         continue;
        }
      //--- Demo Accounts by months Chart
      if(CMTStr::CompareNoCase(tag,L"chart_demo")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_demo))!=MT_RET_OK)
            break;
         continue;
        }
      //--- Contest Accounts by months Chart
      if(CMTStr::CompareNoCase(tag,L"chart_contest")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_contest))!=MT_RET_OK)
            break;
         continue;
        }
      //--- Active Accounts Chart
      if(CMTStr::CompareNoCase(tag,L"chart_active")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_active))!=MT_RET_OK)
            break;
         continue;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Report generation dashboard                                      |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::GenerateDashboard(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create dataset Grow
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- prepare data Grow
   MTAPIRES res=PrepareDataGrow(data);
   if(res!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"PrepareDataGrow failed [%u]",res);
      return(res);
     }
//--- prepare widget Grow
   if((res=PrepareWidget(m_api,data,0,0,L"Accounts Grow",IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA))!=MT_RET_OK)
      return(res);
//--- create dataset Month
   if((data=m_api->DatasetAppend())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare data Month
   if((res=PrepareDataMonth(data))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"PrepareDataMonth failed [%u]",res);
      return(res);
     }
//--- prepare widget Real
   CMTStr256 str;
   str.Format(L"Real Accounts for the last %u years by months",PERIOD_YEARS);
   if((res=PrepareWidget(m_api,data,2,PERIOD_YEARS,str.Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE))!=MT_RET_OK)
      return(res);
//--- prepare widget Preliminary
   str.Format(L"Preliminary Accounts for the last %u years by months",PERIOD_YEARS);
   if((res=PrepareWidget(m_api,data,7,PERIOD_YEARS,str.Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE))!=MT_RET_OK)
      return(res);
//--- prepare widget Demo
   str.Format(L"Demo Accounts for the last %u years by months",PERIOD_YEARS);
   if((res=PrepareWidget(m_api,data,12,PERIOD_YEARS,str.Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE))!=MT_RET_OK)
      return(res);
//--- prepare widget Contest
   str.Format(L"Contest Accounts for the last %u years by months",PERIOD_YEARS);
   if((res=PrepareWidget(m_api,data,17,PERIOD_YEARS,str.Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE))!=MT_RET_OK)
      return(res);
//--- create dataset Active
   if((data=m_api->DatasetAppend())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare data Active
   if((res=PrepareDataActive(data))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"PrepareDataActive failed [%u]",res);
      return(res);
     }
//--- prepare widget Active
   str.Format(L"Active Accounts for the last %u months",PERIOD_INACTIVE_MONTHS);
   if((res=PrepareWidget(m_api,data,0,0,str.Str(),IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CAccountsGrowth::Clear(void)
  {
//--- current time
   m_ctm=0;
   ZeroMemory(&m_ctm_ttm,sizeof(m_ctm_ttm));
   m_checktime=0;
   m_month_data=0;
   m_month_firstyear=0;
//--- real accounts
   ZeroMemory(m_real,sizeof(m_real));
   m_real_active=0;
   m_real_total =0;
//--- preliminary accounts
   ZeroMemory(m_preliminary,sizeof(m_preliminary));
   m_preliminary_active=0;
   m_preliminary_total =0;
//--- demo accounts
   ZeroMemory(m_demo,sizeof(m_demo));
   m_demo_active=0;
   m_demo_total =0;
//--- contest accounts
   ZeroMemory(m_contest,sizeof(m_contest));
   m_contest_active=0;
   m_contest_total =0;
//--- group
   if(m_group)
     {
      m_group->Release();
      m_group=nullptr;
     }
//--- release grow chart
   if(m_chart_grow)
     {
      m_chart_grow->Release();
      m_chart_grow=nullptr;
     }
//--- release real chart
   if(m_chart_real)
     {
      m_chart_real->Release();
      m_chart_real=nullptr;
     }
//--- release preliminary chart
   if(m_chart_preliminary)
     {
      m_chart_preliminary->Release();
      m_chart_preliminary=nullptr;
     }
//--- release demo chart
   if(m_chart_demo)
     {
      m_chart_demo->Release();
      m_chart_demo=nullptr;
     }
//--- release contest chart
   if(m_chart_contest)
     {
      m_chart_contest->Release();
      m_chart_contest=nullptr;
     }
//--- release active chart
   if(m_chart_active)
     {
      m_chart_active->Clear();
      m_chart_active=nullptr;
     }
   m_user_select.Clear();
  }
//+------------------------------------------------------------------+
//| Prepare data                                                     |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareData(void)
  {
//--- checks
   if(!m_api || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- prepare user request
   MTAPIRES res=m_user_select.Prepare(m_api,s_user_fields,_countof(s_user_fields),s_request_limit);
   if(res!=MT_RET_OK)
      return(res);
//--- iterate groups
   for(uint32_t pos=0;m_api->GroupNext(pos,m_group)==MT_RET_OK;pos++)
     {
      //--- prepare information for group
      if((res=PrepareGroup(m_group->Group()))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare information for group                                    |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareGroup(LPCWSTR group)
  {
//--- checks
   if(!group || !*group)
      return(MT_RET_ERR_PARAMS);
//--- skip manager or coverage groups
   const uint32_t type=GetGroupType(group);
   if(type==GROUP_MANAGER || type==GROUP_COVERAGE)
      return(MT_RET_OK);
//--- select users by group
   MTAPIRES res=m_user_select.Select(group);
   if(res!=MT_RET_OK)
      return(res);
//--- select users part by part
   for(uint64_t login=0;IMTDataset *users=m_user_select.Next(res,login);login++)
     {
      //--- process datasets
      if((res=PrepareGroupPart(type,*users,login))!=MT_RET_OK)
         return(m_log.Error(res,L"processing"));
      //--- clear datasets rows
      if((res=users->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Prepare information for group part                               |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareGroupPart(const uint32_t type,IMTDataset &users,uint64_t &login_last)
  {
//--- total users
   const uint32_t total=users.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- iterate users
   UserRecord user={};
   for(uint32_t i=0;i<total;i++)
     {
      //--- read user
      MTAPIRES res=users.RowRead(i,&user,sizeof(user));
      if(res!=MT_RET_OK)
         return(res);
      //--- store last login
      if(login_last<user.login)
         login_last=user.login;
      //--- registration time -> month index
      uint32_t month_index=0;
      if(!TimeToMonthIndex(user.registration,month_index))
         continue;
      //--- is demo group?
      switch(type)
        {
         case GROUP_DEMO:
            if(month_index<_countof(m_demo))
              {
               //--- update demo
               m_demo[month_index]++;
               m_demo_total++;
               //--- check activity
               if(m_checktime<user.last_access)
                  m_demo_active++;
              }
            break;
            //--- is preliminary group?
         case GROUP_PRELIMINARY:
            if(month_index<_countof(m_preliminary))
              {
               //--- update demo
               m_preliminary[month_index]++;
               m_preliminary_total++;
               //--- check activity
               if(m_checktime<user.last_access)
                  m_preliminary_active++;
              }
            break;
            //--- is contest group?
         case GROUP_CONTEST:
            if(month_index<_countof(m_contest))
              {
               //--- update contest
               m_contest[month_index]++;
               m_contest_total++;
               //--- check activity
               if(m_checktime<user.last_access)
                  m_contest_active++;
              }
            break;
         case GROUP_REAL:
            if(month_index<_countof(m_real))
              {
               //--- update real
               m_real[month_index]++;
               m_real_total++;
               //--- check activity
               if(m_checktime<user.last_access)
                  m_real_active++;
              }
            break;
        }
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Accounts Grow Chart                                      |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareChartGrow(void)
  {
   IMTReportSeries *series;
   uint32_t         i,users;
   CMTStr256        str;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
   if(m_chart_grow)
      return(MT_RET_OK);
//--- create chart
   if((m_chart_grow=m_api->ChartCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   m_chart_grow->Type(IMTReportChart::TYPE_GRAPH_NORMALIZED);
   m_chart_grow->Title(L"Accounts Grow");
//--- prepare title series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_TITLE);
//--- iterate months
   for(i=m_month_data;i<PERIOD_MONTHS;i++)
     {
      //--- is not current month?
      if(i<PERIOD_MONTHS-1)
        {
         //--- month index to string
         if(!MonthIndexToStr(i,str)) return(MT_RET_ERROR);
        }
      else
        {
         //--- format date for current time
         str.Format(L"%02u.%02u.%04u",m_ctm_ttm.tm_mday,m_ctm_ttm.tm_mon+1,m_ctm_ttm.tm_year+1900);
        }
      //--- add value
      if((res=series->ValueAdd(str.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_grow->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare real series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_AREA);
   series->Color(MTAPI_COLOR_ACOUNTS_REAL);
   series->Tooltip(L"Date: %VARIABLE%<BR>Real: %VALUE% (%NORMALIZED_VALUE% %)");
   series->Title(L"Real");
//--- iterate months
   users=0;
   for(i=m_month_data;i<PERIOD_MONTHS;i++)
     {
      //--- calculate users
      users+=m_real[i];
      //--- add value
      if((res=series->ValueAddInt(users))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_grow->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }

//--- prepare preliminary series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_AREA);
   series->Color(MTAPI_COLOR_ACOUNTS_PRELIMINARY);
   series->Tooltip(L"Date: %VARIABLE%<BR>Preliminary: %VALUE% (%NORMALIZED_VALUE% %)");
   series->Title(L"Preliminary");
//--- iterate months
   users=0;
   for(i=m_month_data;i<PERIOD_MONTHS;i++)
     {
      //--- calculate users
      users+=m_preliminary[i];
      //--- add value
      if((res=series->ValueAddInt(users))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_grow->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }

//--- prepare demo series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_AREA);
   series->Color(MTAPI_COLOR_ACOUNTS_DEMO);
   series->Tooltip(L"Date: %VARIABLE%<BR>Demo: %VALUE% (%NORMALIZED_VALUE% %)");
   series->Title(L"Demo");
//--- zero amount of users
   users=0;
//--- iterate months
   for(i=m_month_data;i<PERIOD_MONTHS;i++)
     {
      //--- calculate users
      users+=m_demo[i];
      //--- add value
      if((res=series->ValueAddInt(users))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_grow->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare contest series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_AREA);
   series->Color(MTAPI_COLOR_ACOUNTS_CONTEST);
   series->Tooltip(L"Date: %VARIABLE%<BR>Contest: %VALUE% (%NORMALIZED_VALUE% %)");
   series->Title(L"Contest");
//--- zero amount of users
   users=0;
//--- iterate months
   for(i=m_month_data;i<PERIOD_MONTHS;i++)
     {
      //--- calculate users
      users+=m_contest[i];
      //--- add value
      if((res=series->ValueAddInt(users))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_grow->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Real Accounts Chart                                      |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareChartReal(void)
  {
   IMTReportSeries *series;
   CMTStr256        str;
   uint32_t         i,j,year,month;
   MTAPIRES         res;
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
   if(m_chart_real)
      return(MT_RET_OK);
//--- create chart
   if((m_chart_real=m_api->ChartCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   m_chart_real->Type(IMTReportChart::TYPE_GRAPH);
   str.Format(L"Real Accounts for the last %u years by months",PERIOD_YEARS);
   m_chart_real->Title(str.Str());
//--- prepare title series
   if((series=PrepareMonthTitleSeries())==nullptr)
      return(MT_RET_ERR_MEM);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_real->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- iterate years
   for(i=0;i<PERIOD_YEARS;i++)
     {
      //--- current year
      year=m_ctm_ttm.tm_year-PERIOD_YEARS+1900+i+1;
      //--- prepare series
      if((series=m_api->ChartCreateSeries())==nullptr)
         return(MT_RET_ERR_MEM);
      series->Type(IMTReportSeries::TYPE_HISTOGRAM);
      series->Color(GetColorForYear(i));
      //--- setup series title
      str.Format(L"%04u",year);
      series->Title(str.Str());
      //--- setup tooltip format
      str.Format(L"Date: %%VARIABLE%% %04u<BR>Increase: %%VALUE%%",year);
      series->Tooltip(str.Str());
      //--- add months
      for(j=0;j<12;j++)
        {
         //--- calculate month index
         month=m_month_firstyear+i*12+j;
         //--- add value
         if((res=series->ValueAddInt(month<_countof(m_real) ? m_real[month] : 0))!=MT_RET_OK)
           {
            series->Release();
            return(res);
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=m_chart_real->SeriesAdd(series))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Real Accounts Chart                                      |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareChartPreliminary(void)
  {
   IMTReportSeries *series;
   CMTStr256        str;
   uint32_t         i,j,year,month;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
   if(m_chart_preliminary)
      return(MT_RET_OK);
//--- create chart
   if((m_chart_preliminary=m_api->ChartCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   m_chart_preliminary->Type(IMTReportChart::TYPE_GRAPH);
   str.Format(L"Preliminary Accounts for the last %u years by months",PERIOD_YEARS);
   m_chart_preliminary->Title(str.Str());
//--- prepare title series
   if((series=PrepareMonthTitleSeries())==nullptr)
      return(MT_RET_ERR_MEM);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_preliminary->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- iterate years
   for(i=0;i<PERIOD_YEARS;i++)
     {
      //--- current year
      year=m_ctm_ttm.tm_year-PERIOD_YEARS+1900+i+1;
      //--- prepare series
      if((series=m_api->ChartCreateSeries())==nullptr)
         return(MT_RET_ERR_MEM);

      series->Type(IMTReportSeries::TYPE_HISTOGRAM);
      series->Color(GetColorForYear(i));
      //--- setup series title
      str.Format(L"%04u",year);
      series->Title(str.Str());
      //--- setup tooltip format
      str.Format(L"Date: %%VARIABLE%% %04u<BR>Increase: %%VALUE%%",year);
      series->Tooltip(str.Str());
      //--- add months
      for(j=0;j<12;j++)
        {
         //--- calculate month index
         month=m_month_firstyear+i*12+j;
         //--- add value
         if((res=series->ValueAddInt(month<_countof(m_preliminary) ? m_preliminary[month] : 0))!=MT_RET_OK)
           {
            series->Release();
            return(res);
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=m_chart_preliminary->SeriesAdd(series))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Demo Accounts Chart                                      |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareChartDemo(void)
  {
   IMTReportSeries *series;
   CMTStr256        str;
   uint32_t         i,j,year,month;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(m_chart_demo)
      return(MT_RET_OK);
//--- create chart
   if((m_chart_demo=m_api->ChartCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   m_chart_demo->Type(IMTReportChart::TYPE_GRAPH);
   str.Format(L"Demo Accounts for the last %u years by months",PERIOD_YEARS);
   m_chart_demo->Title(str.Str());
//--- prepare title series
   if((series=PrepareMonthTitleSeries())==nullptr)
      return(MT_RET_ERR_MEM);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_demo->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- iterate years
   for(i=0;i<PERIOD_YEARS;i++)
     {
      //--- current year
      year=m_ctm_ttm.tm_year-PERIOD_YEARS+1900+i+1;
      //--- prepare series
      if((series=m_api->ChartCreateSeries())==nullptr)
         return(MT_RET_ERR_MEM);
      series->Type(IMTReportSeries::TYPE_HISTOGRAM);
      series->Color(GetColorForYear(i));
      //--- setup series title
      str.Format(L"%04u",year);
      series->Title(str.Str());
      //--- setup tooltip format
      str.Format(L"Date: %%VARIABLE%% %04u<BR>Increase: %%VALUE%%",year);
      series->Tooltip(str.Str());
      //--- add months
      for(j=0;j<12;j++)
        {
         //--- calculate month index
         month=m_month_firstyear+i*12+j;
         //--- add value
         if((res=series->ValueAddInt(month<_countof(m_demo) ? m_demo[month] : 0))!=MT_RET_OK)
           {
            series->Release();
            return(res);
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=m_chart_demo->SeriesAdd(series))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Contest Accounts Chart                                   |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareChartContest(void)
  {
   IMTReportSeries *series;
   CMTStr256        str;
   uint32_t         i,j,year,month;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(m_chart_contest)
      return(MT_RET_OK);
//--- create chart
   if((m_chart_contest=m_api->ChartCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   m_chart_contest->Type(IMTReportChart::TYPE_GRAPH);
   str.Format(L"Contest Accounts for the last %u years by months",PERIOD_YEARS);
   m_chart_contest->Title(str.Str());
//--- prepare title series
   if((series=PrepareMonthTitleSeries())==nullptr)
      return(MT_RET_ERR_MEM);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_contest->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- iterate years
   for(i=0;i<PERIOD_YEARS;i++)
     {
      //--- current year
      year=m_ctm_ttm.tm_year-PERIOD_YEARS+1900+i+1;
      //--- prepare series
      if((series=m_api->ChartCreateSeries())==nullptr)
         return(MT_RET_ERR_MEM);
      series->Type(IMTReportSeries::TYPE_HISTOGRAM);
      series->Color(GetColorForYear(i));
      //--- setup series title
      str.Format(L"%04u",year);
      series->Title(str.Str());
      //--- setup tooltip format
      str.Format(L"Date: %%VARIABLE%% %04u<BR>Increase: %%VALUE%%",year);
      series->Tooltip(str.Str());
      //--- add months
      for(j=0;j<12;j++)
        {
         //--- calculate month index
         month=m_month_firstyear+i*12+j;
         //--- add value
         if((res=series->ValueAddInt(month<_countof(m_contest) ? m_contest[month] : 0))!=MT_RET_OK)
           {
            series->Release();
            return(res);
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=m_chart_contest->SeriesAdd(series))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Active Accounts Chart                                    |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareChartActive(void)
  {
   IMTReportSeries *series;
   CMTStr256        str;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
   if(m_chart_active)
      return(MT_RET_OK);
//--- create chart
   if((m_chart_active=m_api->ChartCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   m_chart_active->Type(IMTReportChart::TYPE_PIE);
   str.Format(L"Active Accounts for the last %u months",PERIOD_INACTIVE_MONTHS);
   m_chart_active->Title(str.Str());
//--- setup pie tooltip format
   m_chart_active->PieceTooltip(L"%VARIABLE%<BR>%VALUE%");
//--- prepare real series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Real");
   series->Color(MTAPI_COLOR_ACOUNTS_REAL);
   series->ValueAddInt(m_real_active);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_active->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare preliminary series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Preliminary");
   series->Color(MTAPI_COLOR_ACOUNTS_PRELIMINARY);
   series->ValueAddInt(m_preliminary_active);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_active->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare demo series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Demo");
   series->Color(MTAPI_COLOR_ACOUNTS_DEMO);
   series->ValueAddInt(m_demo_active);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_active->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare contest series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Contest");
   series->Color(MTAPI_COLOR_ACOUNTS_CONTEST);
   series->ValueAddInt(m_contest_active);
//--- add series (after adding series will be released by chart)
   if((res=m_chart_active->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare inactive series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(MT_RET_ERR_MEM);

   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Inactive");
   series->Color(MTAPI_COLOR_ACOUNTS_OTHER);
   series->ValueAddInt(m_real_total-m_real_active+m_demo_total-m_demo_active+m_contest_total-m_contest_active);
   series->Tooltip(L"%VARIABLE%<BR>%VALUE%");
//--- add series (after adding series will be released by chart)
   if((res=m_chart_active->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Dataset Columns                                          |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareDataColumns(IMTDataset *data,const ReportColumn *columns,const uint32_t total)
  {
   IMTDatasetColumn        *column;
   MTAPIRES                res;
//--- checks
   if(data==nullptr || columns==nullptr || total==0)
      return(MT_RET_ERR_PARAMS);
//--- create column
   if((column=data->ColumnCreate())==nullptr)
      return(MT_RET_ERR_MEM);
//--- add column description for TableRecord
   for(uint32_t i=0;i<total;i++)
     {
      //--- clear
      column->Clear();
      //--- fill info
      column->ColumnID(columns[i].id);
      column->Name(columns[i].name);
      column->Type(columns[i].type);
      column->Width(columns[i].width);
      column->WidthMax(columns[i].width_max);
      column->Offset(columns[i].offset);
      column->Size(columns[i].size);
      column->DigitsColumn(columns[i].digits_column);
      column->Flags(columns[i].flags);
      //--- add column
      if((res=data->ColumnAdd(column))!=MT_RET_OK)
        {
         column->Release();
         return(res);
        }
     }
//--- release column
   column->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Dataset Accounts Grow                                    |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareDataGrow(IMTDataset *data)
  {
   GrowRecord              row={0};
   uint32_t                i;
   MTAPIRES                res;
//--- checks
   if(data==nullptr)
      return(MT_RET_ERR_PARAMS);
//--- create dataset columns
   if((res=PrepareDataColumns(data,s_columns_grow,_countof(s_columns_grow)))!=MT_RET_OK)
      return(res);
//--- iterate months
   for(i=m_month_data;i<PERIOD_MONTHS;i++)
     {
      //--- is not current month?
      if(i<PERIOD_MONTHS-1)
         row.date=MonthIndexToTime(i);
      else
         row.date=m_ctm;
      //--- check date
      if(!row.date)
         return(MT_RET_ERROR);
      //--- calculate users
      row.real       +=m_real[i];
      row.preliminary+=m_preliminary[i];
      row.demo       +=m_demo[i];
      row.contest    +=m_contest[i];
      //--- write data row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Dataset Accounts by Month                                |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareDataMonth(IMTDataset *data)
  {
//--- checks
   if(data==nullptr)
      return(MT_RET_ERR_PARAMS);
//--- create dataset column month
   const static LPCWSTR months[12]={L"Jan",L"Feb",L"Mar",L"Apr",L"May",L"Jun",L"Jul",L"Aug",L"Sep",L"Oct",L"Nov",L"Dec"};
   uint32_t             type,i,j,year,month;
   MTAPIRES             res;

   if((res=PrepareDataColumns(data,&s_column_month,1))!=MT_RET_OK)
      return(res);
//--- iterate account types
   ReportColumn column_year=s_column_year;
   for(type=0;type<4;type++)
     {
      //--- iterate years
      for(i=0;i<PERIOD_YEARS;i++)
        {
         //--- current year
         year=m_ctm_ttm.tm_year-PERIOD_YEARS+1900+i+1;
         //--- year formatting
         CMTStr16 str;
         str.Format(s_column_year.name,year);
         column_year.name=str.Str();
         //--- create dataset column year
         if((res=PrepareDataColumns(data,&column_year,1))!=MT_RET_OK)
            return(res);
         //--- next column
         column_year.id++;
         column_year.offset+=sizeof(MonthRecord::real[0]);
        }
     }
//--- add data rows
   for(j=0;j<12;j++)
     {
      MonthRecord row;
      CMTStr::Copy(row.month,months[j]);
      //--- iterate years
      for(i=0;i<PERIOD_YEARS;i++)
        {
         //--- calculate month index
         month=m_month_firstyear+i*12+j;
         row.real       [i]=month<_countof(m_real)        ? m_real       [month] : 0;
         row.preliminary[i]=month<_countof(m_preliminary) ? m_preliminary[month] : 0;
         row.demo       [i]=month<_countof(m_demo)        ? m_demo       [month] : 0;
         row.contest    [i]=month<_countof(m_contest)     ? m_contest    [month] : 0;
        }
      //--- write data row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Dataset Active Accounts                                  |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareDataActive(IMTDataset *data)
  {
//--- checks
   if(data==nullptr)
      return(MT_RET_ERR_PARAMS);
//--- create dataset columns
   ActiveRecord row={};
   MTAPIRES     res;

   if((res=PrepareDataColumns(data,s_columns_active,_countof(s_columns_active)))!=MT_RET_OK)
      return(res);
//--- Real
   CMTStr::Copy(row.type,L"Real");
   row.active=m_real_active;
//--- write data row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
//--- Preliminary
   CMTStr::Copy(row.type,L"Preliminary");
   row.active=m_preliminary_active;
//--- write data row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
//--- Demo
   CMTStr::Copy(row.type,L"Demo");
   row.active=m_demo_active;
//--- write data row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
//--- Contest
   CMTStr::Copy(row.type,L"Contest");
   row.active=m_contest_active;
//--- write data row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
//--- Inactive
   CMTStr::Copy(row.type,L"Inactive");
   row.active=m_real_total-m_real_active+m_demo_total-m_demo_active+m_contest_total-m_contest_active;
//--- write data row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare dashboard widget                                         |
//+------------------------------------------------------------------+
MTAPIRES CAccountsGrowth::PrepareWidget(IMTReportAPI *api,IMTDataset *data,uint32_t column,uint32_t column_total,LPCWSTR title,const uint32_t type)
  {
   IMTReportDashboardWidget *widget=nullptr;
   MTAPIRES                  res=MT_RET_OK;
//--- checks
   if(!api || !data || !title)
      return(MT_RET_ERR_PARAMS);
//--- create widget
   if((widget=api->DashboardWidgetAppend())==nullptr)
     {
      api->LoggerOutString(MTLogErr,L"DashboardWidgetAppend failed");
      return(MT_RET_ERR_MEM);
     }
//--- prepare chart widget
   if((res=widget->Height(type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE ? CHART_HEIGHT_SECONDARY : CHART_HEIGHT_PRIMARY))!=MT_RET_OK)
     {
      api->LoggerOut(MTLogErr,L"IMTReportDashboardWidget::Height failed [%u]",res);
      return(res);
     }
   if((res=widget->Title(title))!=MT_RET_OK)
     {
      api->LoggerOut(MTLogErr,L"IMTReportDashboardWidget::Title failed [%u]",res);
      return(res);
     }
   if((res=widget->Type(type))!=MT_RET_OK)
     {
      api->LoggerOut(MTLogErr,L"IMTReportDashboardWidget::Type failed [%u]",res);
      return(res);
     }
   if(type==IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA)
      if((res=widget->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
        {
         api->LoggerOut(MTLogErr,L"IMTReportDashboardWidget::ChartStackType failed [%u]",res);
         return(res);
        }
//--- set data to chart widget
   if((res=widget->Data(data))!=MT_RET_OK)
     {
      api->LoggerOut(MTLogErr,L"IMTReportDashboardWidget::Data failed [%u]",res);
      return(res);
     }
//--- add data columns
   for(;column_total;column++,column_total--)
      if((res=widget->DataColumnAdd(column))!=MT_RET_OK)
        {
         api->LoggerOut(MTLogErr,L"IMTReportDashboardWidget::DataColumnAdd failed [%u]",res);
         return(res);
        }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare title series for "Accounts by month" charts              |
//+------------------------------------------------------------------+
IMTReportSeries* CAccountsGrowth::PrepareMonthTitleSeries(void)
  {
   IMTReportSeries *series=nullptr;
   const LPCWSTR    months[12]={ L"Jan",L"Feb",L"Mar",L"Apr",L"May",L"Jun",L"Jul",L"Aug",L"Sep",L"Oct",L"Nov",L"Dec" };
//--- checks
   if(!m_api)
      return(nullptr);
//--- create series
   if((series=m_api->ChartCreateSeries())==nullptr)
      return(nullptr);
//--- prepare chart
   series->Type(IMTReportSeries::TYPE_TITLE);
//--- iterate months
   for(uint32_t i=0;i<_countof(months);i++)
      if(series->ValueAdd(months[i])!=MT_RET_OK)
        {
         series->Release();
         return(nullptr);
        }
//--- return series
   return(series);
  }
//+------------------------------------------------------------------+
//| Time to Month Index                                              |
//+------------------------------------------------------------------+
bool CAccountsGrowth::TimeToMonthIndex(const int64_t ctm,uint32_t &index)
  {
   tm   ttm={};
   uint32_t month_curr,month_ctm,month_delta;
//--- parse time
   if(!SMTTime::ParseTime(ctm,&ttm))
      return(false);
//--- current month
   month_curr=m_ctm_ttm.tm_year*12+m_ctm_ttm.tm_mon;
//--- ctm month
   month_ctm=ttm.tm_year*12+ttm.tm_mon;
//--- checks
   if(month_ctm<=month_curr)
     {
      //--- month delta
      month_delta=month_curr-month_ctm;
      //--- checks
      if(month_delta<PERIOD_MONTHS)
        {
         //--- calculate month index
         index=PERIOD_MONTHS-month_delta-1;
         //--- ok
         return(true);
        }
     }
//--- fail
   return(false);
  }
//+------------------------------------------------------------------+
//| Month Index to String Time                                       |
//+------------------------------------------------------------------+
bool CAccountsGrowth::MonthIndexToStr(const uint32_t index,CMTStr& str)
  {
   uint32_t month_curr,month_begin;
//--- clear result
   str.Clear();
//--- checks
   if(index<PERIOD_MONTHS)
     {
      //--- current month
      month_curr=m_ctm_ttm.tm_year*12+m_ctm_ttm.tm_mon;
      //--- start month
      month_begin=month_curr-PERIOD_MONTHS+1;
      //--- format time
      str.Format(L"%02u.%04u",(month_begin+index)%12+1,(month_begin+index)/12+1900);
      //--- ok
      return(true);
     }
//--- failed
   return(true);
  }
//+------------------------------------------------------------------+
//| Month Index to Time                                              |
//+------------------------------------------------------------------+
int64_t CAccountsGrowth::MonthIndexToTime(const uint32_t index)
  {
   uint32_t month_curr,month_begin;
   tm   ttm={};
//--- checks
   if(index<PERIOD_MONTHS)
     {
      //--- current month
      month_curr=m_ctm_ttm.tm_year*12+m_ctm_ttm.tm_mon;
      //--- start month
      month_begin=month_curr-PERIOD_MONTHS+1;
      //--- make time
      ttm.tm_mon=(month_begin+index)%12;
      ttm.tm_year=(month_begin+index)/12;
      //--- ok
      return(SMTTime::MakeTime(&ttm));
     }
//--- failed
   return(0);
  }
//+------------------------------------------------------------------+
//| Color for the year                                               |
//+------------------------------------------------------------------+
uint32_t CAccountsGrowth::GetColorForYear(const uint32_t index)
  {
   static uint32_t colors[]={ 0x353B37,0x59615C,0x758F7F,0xAEBFB4,0x84B761 };
//--- check current year
   if(index==PERIOD_YEARS-1)
      return(colors[_countof(colors)-1]);
//--- check first year
   if(index==0)
      return(colors[0]);
//--- caculate step & real index
   uint32_t step      =_countof(colors)/PERIOD_YEARS;
   uint32_t real_index=index*std::max(1U,step);
//--- return color
   return(real_index<_countof(colors) ? colors[real_index] : colors[_countof(colors)-1]);
  }
//+------------------------------------------------------------------+
