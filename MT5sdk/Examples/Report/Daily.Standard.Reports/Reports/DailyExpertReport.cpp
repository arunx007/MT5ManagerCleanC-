//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyExpertReport.h"
//+------------------------------------------------------------------+
//| Report colors                                                    |
//+------------------------------------------------------------------+
#define COLOR_TOTAL      0x0382CE
#define COLOR_EXPERTS    0xE1441D
#define COLOR_SIGNALS    0x52C600
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
const MTReportInfo CDailyExpertReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_9,
   L"Daily Expert Advisors",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_ACCOUNTS_FULL,
   MTReportInfo::TYPE_HTML|MTReportInfo::TYPE_DASHBOARD,
   L"Daily",
                    // params
     {{ MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM                    },
     },1,           // params_total
     {              // configs
      { MTReportParam::TYPE_STRING, L"Currency",       DEFAULT_CURRENCY },
      { MTReportParam::TYPE_STRING, L"Groups",         DEFAULT_GROUPS   },
     },2            // configs_total
  };
//+------------------------------------------------------------------+
//| Column description Daily Info                                    |
//+------------------------------------------------------------------+
ReportColumn CDailyExpertReport::s_columns_daily[]=
  {
   //--- id,name,             type,                         width,  width_max, offset,                size,digits_column,flags
     { 1,L"Date",             IMTDatasetColumn::TYPE_DATE   ,20,0, offsetof(DateInfo,date)            ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Expert Advisors",  IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DateInfo,expert_deals)    ,0,0,0 },
   { 3,  L"Expert Advisors",  IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DateInfo,expert_orders)   ,0,0,0 },
   { 4,  L"Expert Advisors",  IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(DateInfo,expert_profit)   ,0,0,0 },
   { 5,  L"Trading Signals",  IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DateInfo,signal_deals)    ,0,0,0 },
   { 6,  L"Trading Signals",  IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DateInfo,signal_orders)   ,0,0,0 },
   { 7,  L"Trading Signals",  IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(DateInfo,signal_profit)   ,0,0,0 },
   { 8,  L"Total",            IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DateInfo,total_deals)     ,0,0,0 },
   { 9,  L"Total",            IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DateInfo,total_orders)    ,0,0,0 },
   {10,  L"Total",            IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(DateInfo,total_profit)    ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Table                                         |
//+------------------------------------------------------------------+
ReportColumn CDailyExpertReport::s_columns_table[]=
  {
   //--- id,name,           type,                            width, width_max, offset,                size,digits_column,flags
     { 1,  L"Login",        IMTDatasetColumn::TYPE_USER_LOGIN, 5,0, offsetof(TableInfo,login)           ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Name",           IMTDatasetColumn::TYPE_STRING    ,15,0, offsetof(TableInfo,name)            ,MtFieldSize(TableInfo,name),0,0 },
   { 3,  L"Group",          IMTDatasetColumn::TYPE_STRING    ,10,0, offsetof(TableInfo,group)           ,MtFieldSize(TableInfo,group),0,0 },
   { 4,  L"Balance",        IMTDatasetColumn::TYPE_MONEY     , 5,0, offsetof(TableInfo,balance)         ,0,0,0 },
   { 5,  L"Floating",       IMTDatasetColumn::TYPE_MONEY     , 5,0, offsetof(TableInfo,floating)        ,0,0,0 },
   { 6,  L"Experts Deals",  IMTDatasetColumn::TYPE_UINT64    , 5,0, offsetof(TableInfo,expert_deals)    ,0,0,0 },
   { 7,  L"Experts Orders", IMTDatasetColumn::TYPE_UINT64    , 5,0, offsetof(TableInfo,expert_orders)   ,0,0,0 },
   { 8,  L"Experts Profit", IMTDatasetColumn::TYPE_MONEY     , 5,0, offsetof(TableInfo,expert_profit)   ,0,0,0 },
   { 9,  L"Signals Deals",  IMTDatasetColumn::TYPE_UINT64    , 5,0, offsetof(TableInfo,signal_deals)    ,0,0,0 },
   {10,  L"Signals Orders", IMTDatasetColumn::TYPE_UINT64    , 5,0, offsetof(TableInfo,signal_orders)   ,0,0,0 },
   {11,  L"Signals Profit", IMTDatasetColumn::TYPE_MONEY     , 5,0, offsetof(TableInfo,signal_profit)   ,0,0,0 },
   {12,  L"Total Deals",    IMTDatasetColumn::TYPE_UINT64    , 5,0, offsetof(TableInfo,total_deals)     ,0,0,0 },
   {13,  L"Total Orders",   IMTDatasetColumn::TYPE_UINT64    , 5,0, offsetof(TableInfo,total_orders)    ,0,0,0 },
   {14,  L"Total Profit",   IMTDatasetColumn::TYPE_MONEY     , 5,0, offsetof(TableInfo,total_profit)    ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyExpertReport::CDailyExpertReport(void) : m_api(NULL),
                                               m_chart_deals(NULL),m_chart_profit(NULL),m_chart_orders(NULL),
                                               m_user(NULL),m_group(NULL),m_deals(NULL),m_orders(NULL),m_account(NULL),
                                               m_digits(2)
  {
//--- clear all structs
//--- tops
   ZeroMemory(&m_deals_signal ,sizeof(m_deals_signal ));
   ZeroMemory(&m_deals_expert ,sizeof(m_deals_expert ));
   ZeroMemory(&m_orders_signal,sizeof(m_orders_signal));
   ZeroMemory(&m_orders_expert,sizeof(m_orders_expert));
   ZeroMemory(&m_profit_signal,sizeof(m_profit_signal));
   ZeroMemory(&m_profit_expert,sizeof(m_profit_expert));
//--- chart
   ZeroMemory(m_chart_info  ,sizeof(m_chart_info));
   ZeroMemory(m_chart_titles,sizeof(m_chart_titles));
//--- parameters
   ZeroMemory(m_currency  ,sizeof(m_currency));
   ZeroMemory(m_group_mask,sizeof(m_group_mask));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyExpertReport::~CDailyExpertReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Generation report                                                |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res=MT_RET_OK;
//--- checks
   if(api==NULL)
      return(MT_RET_ERR_PARAMS);
   if(type!=MTReportInfo::TYPE_HTML && type!=MTReportInfo::TYPE_DASHBOARD)
      res=MT_RET_ERR_NOTIMPLEMENT;
//--- save api pointer
   m_api=api;
   Clear();
//--- create interfaces
   if(res==MT_RET_OK)
      if(!CreateInterfaces())
         res=MT_RET_ERR_MEM;
//--- get currency from arguments
   if(res==MT_RET_OK)
      res=GetParameters();
//--- get all info
   if(res==MT_RET_OK)
      res=GetReportInfo();
//--- prepare html charts
   if(type==MTReportInfo::TYPE_HTML)
     {
      //--- prepare chart of deals
      if(res==MT_RET_OK)
         res=FillDealChart();
      //--- prepare chart of deals
      if(res==MT_RET_OK)
         res=FillOrdersChart();
      //--- prepare chart of profit
      if(res==MT_RET_OK)
         res=FillProfitChart();
     }
//--- write all to dashboard or html
   if(res==MT_RET_OK)
     {
      if(type==MTReportInfo::TYPE_DASHBOARD)
         res=GenerateDashboard();
      else
         res=WriteAll();
     }
//--- show error page if error exists
   if(res!=MT_RET_OK && type==MTReportInfo::TYPE_HTML)
      res=CReportError::Write(api,L"Daily Expert Report",L"Report generation failed. For more information see server's journal.");
//--- done
   Clear();
   return(res);
  }
//+------------------------------------------------------------------+
//| Write all to html by templates                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::WriteAll(void)
  {
   MTAPISTR      tag;
   CMTStr256     str;
   uint32_t      counter;
   MTAPIRES      res   =MT_RET_OK;
   IMTConReport *report=NULL;
//--- create report interface
   if((report=m_api->ReportCreate())==NULL) return(MT_RET_ERR_MEM);
//--- get report interface
   report->Clear();
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
      return(res);
//--- use template
   if((res=m_api->HtmlTplLoadResource(IDR_HTML_DAILY_EXPERT_REPORT,RT_HTML))!=MT_RET_OK)
     {
      report->Release();
      return(res);
     }
//--- process tags
   while((res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- print the report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         if((res=m_api->HtmlWriteSafe(report->Name(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- print default currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((res=m_api->HtmlWriteSafe(m_currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- chart of profit 
      if(CMTStr::CompareNoCase(tag,L"chart_profit")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_profit))!=MT_RET_OK)
            break;
         continue;
        }
      //--- chart of deals number
      if(CMTStr::CompareNoCase(tag,L"chart_deals")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_deals))!=MT_RET_OK)
            break;
         continue;
        }
      //--- chart of orders number
      if(CMTStr::CompareNoCase(tag,L"chart_orders")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_orders))!=MT_RET_OK)
            break;
         continue;
        }
      //--- date
      if(CMTStr::CompareNoCase(tag,L"date")==0)
        {
         str.Format(L"%02u.%02u.%04u",
             SMTTime::Day(m_api->ParamFrom()),
             SMTTime::Month(m_api->ParamFrom()),
             SMTTime::Year(m_api->ParamFrom()));
         if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
            break;
         continue;
        }
      //--- write experts tops
      if(WriteExpert(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write signals tops
      if(WriteSignal(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
     }
   report->Release();
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write expert's tables                                            |
//+------------------------------------------------------------------+
bool CDailyExpertReport::WriteExpert(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr128 str;
   uint32_t  curr_pos =0;
//--- checks
   if(m_api==NULL || m_user==NULL)
     {
      retcode=MT_RET_ERR_PARAMS;
      return(true);
     }
//--- if no tops of expert's deals
   if((CMTStr::CompareNoCase(tag,L"no_deals_expert")==0)  && (counter==0) && !m_deals_expert[0].login)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no tops of expert's orders
   if((CMTStr::CompareNoCase(tag,L"no_orders_expert")==0) && (counter==0) && !m_orders_expert[0].login)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no tops of expert's profit
   if((CMTStr::CompareNoCase(tag,L"no_profit_expert")==0) && (counter==0) && !m_profit_expert[0].login)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- write expert's deals
   while(CMTStr::CompareNoCase(tag,L"table_deals_expert")==0 && counter<_countof(m_deals_expert))
     {
      //--- check logins
      if(!m_deals_expert[counter].login) break;
      //--- save current user counter
      curr_pos=counter;
      //--- get user info
      if((retcode=m_api->UserGetLight(m_deals_expert[curr_pos].login,m_user))!=MT_RET_OK)
         return(false);
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
         if(ProcessTable(tag,m_deals_expert[curr_pos],retcode))
            break;
     }
//--- write expert's orders
   while(CMTStr::CompareNoCase(tag,L"table_orders_expert")==0 && counter<_countof(m_orders_expert))
     {
      //--- check logins
      if(!m_orders_expert[counter].login) break;
      //--- save current user counter
      curr_pos=counter;
      //--- get user info
      if((retcode=m_api->UserGetLight(m_orders_expert[curr_pos].login,m_user))!=MT_RET_OK)
         return(false);
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
         if(ProcessTable(tag,m_orders_expert[curr_pos],retcode))
            break;
     }
//--- write expert's profit
   while(CMTStr::CompareNoCase(tag,L"table_profit_expert")==0 && counter<_countof(m_profit_expert))
     {
      //--- check logins
      if(!m_profit_expert[counter].login) break;
      //--- save current user counter
      curr_pos=counter;
      //--- get user info
      if((retcode=m_api->UserGetLight(m_profit_expert[curr_pos].login,m_user))!=MT_RET_OK)
         return(false);
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
         if(ProcessTable(tag,m_profit_expert[curr_pos],retcode))
            break;
     }
//--- done
   return(false);
  }
//+------------------------------------------------------------------+
//| Write signal's tables                                            |
//+------------------------------------------------------------------+
bool CDailyExpertReport::WriteSignal(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr128 str;
   uint32_t  curr_pos =0;
//--- checks
   if(m_api==NULL || m_user==NULL)
     {
      retcode=MT_RET_ERR_PARAMS;
      return(true);
     }
//--- if no tops of signal's deals
   if((CMTStr::CompareNoCase(tag,L"no_deals_signal")==0) && (counter==0)  && !m_deals_signal[0].login)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no tops of signal's orders
   if((CMTStr::CompareNoCase(tag,L"no_orders_signal")==0) && (counter==0) && !m_orders_signal[0].login)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no tops of signal's profit
   if((CMTStr::CompareNoCase(tag,L"no_profit_signal")==0) && (counter==0) && !m_profit_signal[0].login)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- write signal's deals
   while(CMTStr::CompareNoCase(tag,L"table_deals_signal")==0 && counter<_countof(m_deals_signal))
     {
      //--- check logins
      if(!m_deals_signal[counter].login) break;
      //--- save current user counter
      curr_pos=counter;
      //--- get user info
      if((retcode=m_api->UserGetLight(m_deals_signal[curr_pos].login,m_user))!=MT_RET_OK)
         return(false);
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
         if(ProcessTable(tag,m_deals_signal[curr_pos],retcode))
            break;
     }
//--- write signal's orders
   while(CMTStr::CompareNoCase(tag,L"table_orders_signal")==0 && counter<_countof(m_orders_signal))
     {
      //--- check logins
      if(!m_orders_signal[counter].login) break;
      //--- save current user counter
      curr_pos=counter;
      //--- get user info
      if((retcode=m_api->UserGetLight(m_orders_signal[curr_pos].login,m_user))!=MT_RET_OK)
         return(false);
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
         if(ProcessTable(tag,m_orders_signal[curr_pos],retcode))
            break;
     }
//--- write signal's profit
   while(CMTStr::CompareNoCase(tag,L"table_profit_signal")==0 && counter<_countof(m_profit_signal))
     {
      //--- check logins
      if(!m_profit_signal[counter].login) break;
      //--- save current user counter
      curr_pos=counter;
      //--- get user info
      if((retcode=m_api->UserGetLight(m_profit_signal[curr_pos].login,m_user))!=MT_RET_OK)
         return(false);
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
         if(ProcessTable(tag,m_profit_signal[curr_pos],retcode))
            break;
     }
//--- done
   return(false);
  }
//+------------------------------------------------------------------+
//| Clear all interfaces                                             |
//+------------------------------------------------------------------+
void CDailyExpertReport::Clear(void)
  {
//--- helper configs
   if(m_user   !=NULL) { m_user->Release();    m_user   =NULL; }
   if(m_group  !=NULL) { m_group->Release();   m_group  =NULL; }
   if(m_deals  !=NULL) { m_deals->Release();   m_deals  =NULL; }
   if(m_orders !=NULL) { m_orders->Release();  m_orders =NULL; }
   if(m_account!=NULL) { m_account->Release(); m_account=NULL; }
//--- charts
   if(m_chart_deals !=NULL) { m_chart_deals->Release();  m_chart_deals =NULL; }
   if(m_chart_profit!=NULL) { m_chart_profit->Release(); m_chart_profit=NULL; }
   if(m_chart_orders!=NULL) { m_chart_orders->Release(); m_chart_orders=NULL; }
  }
//+------------------------------------------------------------------+
//| Create interfaces                                                |
//+------------------------------------------------------------------+
bool CDailyExpertReport::CreateInterfaces(void)
  {
//--- check
   Clear();
   if(m_api==NULL)
      return(false);
//--- helper configs
   if((m_user   =m_api->UserCreate())       ==NULL) return(false);
   if((m_group  =m_api->GroupCreate())      ==NULL) return(false);
   if((m_deals  =m_api->DealCreateArray())  ==NULL) return(false);
   if((m_orders =m_api->OrderCreateArray()) ==NULL) return(false);
   if((m_account=m_api->UserCreateAccount())==NULL) return(false);
//--- charts
   if((m_chart_deals  =m_api->ChartCreate())==NULL) return(false);
   if((m_chart_profit =m_api->ChartCreate())==NULL) return(false);
   if((m_chart_orders =m_api->ChartCreate())==NULL) return(false);
//--- get time period
   for(uint32_t i=0;i<_countof(m_chart_titles);i++)
      m_chart_titles[i]=m_api->ParamFrom()-SECONDS_IN_DAY*(REPORT_PERIOD-1-i);
//--- done
   return(true);
  }
//+------------------------------------------------------------------+
//| Get array of deals                                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::GetReportInfo(void)
  {
   MTAPIRES    res      =MT_RET_ERROR;
   DailyInfo   user_info={0};
   double      rate     =0;
//--- checks
   if(m_api==NULL || m_group==NULL || m_deals==NULL || m_orders==NULL)
      return(MT_RET_ERR_PARAMS);
//--- get all groups
   for(uint32_t pos=0;m_api->GroupNext(pos,m_group)==MT_RET_OK;pos++)
     {
      //--- check group by filter
      if(CMTStr::CheckGroupMask(m_group_mask,m_group->Group()))
        {
         uint64_t *logins=NULL;
         uint32_t    logins_total=0;
         //--- get logins list
         if((res=m_api->UserLogins(m_group->Group(),logins,logins_total))!=MT_RET_OK)
           {
            if(logins) m_api->Free(logins);
            return(res);
           }
         //--- get each deals
         for(uint32_t i=0;i<logins_total;i++)
           {
            //--- by each day in period
            for(uint32_t day=0;day<REPORT_PERIOD;day++)
              {
               //--- get deals of user
               if((res=m_api->DealGet(logins[i],
                                      m_chart_titles[day],
                                      m_chart_titles[day]+SECONDS_IN_DAY,
                                      m_deals))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  return(res);
                 }
               //--- get orders of user
               if((res=m_api->HistoryGet(logins[i],
                                         m_chart_titles[day],
                                         m_chart_titles[day]+SECONDS_IN_DAY,
                                         m_orders))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  return(res);
                 }
               //--- parse deals
               if((res=ParseDealArray(m_deals,user_info))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  return(res);
                 }
               //--- parse orders
               if((res=ParseOrdersArray(m_orders,user_info))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  return(res);
                 }
               //--- convert users profit
               if((res=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  return(res);
                 }
               //--- expert profit
               user_info.expert_profit=SMTMath::PriceNormalize(user_info.expert_profit*rate,m_digits);
               //--- signal profit
               user_info.signal_profit=SMTMath::PriceNormalize(user_info.signal_profit*rate,m_digits);
               //--- total profit
               user_info.total_profit=SMTMath::PriceNormalize(user_info.total_profit*rate,m_digits);
               //--- add to total
               m_chart_info[day].expert_deals +=user_info.expert_deals;
               m_chart_info[day].expert_orders+=user_info.expert_orders;
               m_chart_info[day].expert_profit =SMTMath::MoneyAdd(m_chart_info[day].expert_profit,user_info.expert_profit,m_digits);
               m_chart_info[day].signal_deals +=user_info.signal_deals;
               m_chart_info[day].signal_orders+=user_info.signal_orders;
               m_chart_info[day].signal_profit =SMTMath::MoneyAdd(m_chart_info[day].signal_profit,user_info.signal_profit,m_digits);
               m_chart_info[day].total_deals  +=user_info.total_deals;
               m_chart_info[day].total_orders +=user_info.total_orders;
               m_chart_info[day].total_profit  =SMTMath::MoneyAdd(m_chart_info[day].total_profit,user_info.total_profit,m_digits);
               //--- if this day is last
               if(day==REPORT_PERIOD-1)
                 {
                  //--- add to top of users
                  user_info.login=logins[i];
                  TopModify(user_info);
                 }
               //--- clean user_info struct
               ZeroMemory(&user_info,sizeof(user_info));
              }
           }
        }
     }
//--- sort top of users after all modifications
   SMTSearch::QuickSort(m_deals_signal ,_countof(m_deals_signal ),SortBySignalDeals );
   SMTSearch::QuickSort(m_deals_expert ,_countof(m_deals_expert ),SortByExpertDeals );
   SMTSearch::QuickSort(m_orders_signal,_countof(m_orders_signal),SortBySignalOrders);
   SMTSearch::QuickSort(m_orders_expert,_countof(m_orders_expert),SortByExpertOrders);
   SMTSearch::QuickSort(m_profit_signal,_countof(m_profit_signal),SortBySignalProfit);
   SMTSearch::QuickSort(m_profit_expert,_countof(m_profit_expert),SortByExpertProfit);
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Modify tops users                                                |
//+------------------------------------------------------------------+
void CDailyExpertReport::TopModify(DailyInfo& user)
  {
   DailyInfo *lowest=NULL;
//--- checks
   if(!user.login) return;
//--- add expert's deals
   if(user.expert_deals>0)
     {
      lowest=m_deals_expert;
      //--- find lowest item
      for(uint32_t i=1;i<_countof(m_deals_expert);i++)
         if(SortByExpertDeals(&m_deals_expert[i],lowest))
            lowest=&m_deals_expert[i];
      //--- add to top
      if(SortByExpertDeals(lowest,&user))
         memcpy_s(lowest,sizeof(*lowest),&user,sizeof(user));
     }
//--- add expert's orders
   if(user.expert_orders>0)
     {
      lowest=m_orders_expert;
      //--- find lowest item
      for(uint32_t i=1;i<_countof(m_orders_expert);i++)
         if(SortByExpertOrders(&m_orders_expert[i],lowest))
            lowest=&m_orders_expert[i];
      //--- add to top
      if(SortByExpertOrders(lowest,&user))
         memcpy_s(lowest,sizeof(*lowest),&user,sizeof(user));
     }
//--- add expert's profit
   if(user.expert_profit>0)
     {
      lowest=m_profit_expert;
      //--- find lowest item
      for(uint32_t i=1;i<_countof(m_profit_expert);i++)
         if(SortByExpertProfit(&m_profit_expert[i],lowest))
            lowest=&m_profit_expert[i];
      //--- add to top
      if(SortByExpertProfit(lowest,&user))
         memcpy_s(lowest,sizeof(*lowest),&user,sizeof(user));
     }
//--- add signal's deals
   if(user.signal_deals>0)
     {
      lowest=m_deals_signal;
      //--- find lowest item
      for(uint32_t i=1;i<_countof(m_deals_signal);i++)
         if(SortBySignalDeals(&m_deals_signal[i],lowest))
            lowest=&m_deals_signal[i];
      //--- add to top
      if(SortBySignalDeals(lowest,&user))
         memcpy_s(lowest,sizeof(*lowest),&user,sizeof(user));
     }
//--- add signal's orders
   if(user.signal_orders>0)
     {
      lowest=m_orders_signal;
      //--- find lowest item
      for(uint32_t i=1;i<_countof(m_orders_signal);i++)
         if(SortBySignalOrders(&m_orders_signal[i],lowest))
            lowest=&m_orders_signal[i];
      //--- add to top
      if(SortBySignalOrders(lowest,&user))
         memcpy_s(lowest,sizeof(*lowest),&user,sizeof(user));
     }
//--- add signal's profit
   if(user.signal_profit>0)
     {
      lowest=m_profit_signal;
      //--- find lowest item
      for(uint32_t i=1;i<_countof(m_profit_signal);i++)
         if(SortBySignalProfit(&m_profit_signal[i],lowest))
            lowest=&m_profit_signal[i];
      //--- add to top
      if(SortBySignalProfit(lowest,&user))
         memcpy_s(lowest,sizeof(*lowest),&user,sizeof(user));
     }
  }
//+------------------------------------------------------------------+
//| Parse deal array and fill structures                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::ParseDealArray(IMTDealArray* deals,DailyInfo &info)
  {
   IMTDeal* deal  =NULL;
   double   profit=0;
//--- checks
   if(deals==NULL || m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- parse
   for(uint32_t i=0;i<deals->Total();i++)
     {
      if((deal=deals->Next(i))==NULL)
         continue;
      if((deal->Action()!=IMTDeal::DEAL_BUY) &&
         (deal->Action()!=IMTDeal::DEAL_SELL))
         continue;
      //--- add total deals
      info.total_deals++;
      //--- get profit in given currency
      profit=deal->Profit();
      //--- check deal reason
      switch(deal->Reason())
        {
         //--- if it's expert-deal 
         case IMTDeal::DEAL_REASON_EXPERT:
            //--- add total expert-deal
            info.expert_deals++;
            //--- add total expert-profit
            info.expert_profit=SMTMath::MoneyAdd(info.expert_profit,profit,deal->DigitsCurrency());
            break;
            //--- if it's signal-deal   
         case IMTDeal::DEAL_REASON_SIGNAL:
            //--- add total signal-deal
            info.signal_deals++;
            //--- add total signal-profit
            info.signal_profit=SMTMath::MoneyAdd(info.signal_profit,profit,deal->DigitsCurrency());
            break;
        }
      info.total_profit=SMTMath::MoneyAdd(info.total_profit,profit,deal->DigitsCurrency());
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Parse orders array and fill structures                           |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::ParseOrdersArray(IMTOrderArray* orders,DailyInfo &info)
  {
//--- checks
   if(orders==NULL || m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- parse
   for(uint32_t i=0;i<orders->Total();i++)
     {
      IMTOrder *order;
      //---
      if((order=orders->Next(i))==NULL)
         continue;
      //--- add total orders
      info.total_orders++;
      switch(order->Reason())
        {
         //--- if it's expert-order add total
         case IMTOrder::ORDER_REASON_EXPERT:
            info.expert_orders++;
            break;
            //--- if it's signal-order add total
         case IMTOrder::ORDER_REASON_SIGNAL:
            info.signal_orders++;
            break;
        }
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill deal count chart                                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::FillDealChart(void)
  {
   MTAPIRES         res   =MT_RET_ERROR;
   IMTReportSeries *series=NULL;
//--- checks
   if(m_api==NULL || m_chart_deals==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare tentative chart
   m_chart_deals->Title(L"Deals of Clients");
   PrepareChartTitle(m_chart_deals);
//--- prepare total values
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_TOTAL);
   series->Title(L"Total");
   series->Tooltip(L"Count of deals: %VALUE%<BR>Date: %VARIABLE%");
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddInt(m_chart_info[i].total_deals))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_deals->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare values of expert's
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_EXPERTS);
   series->Title(L"Expert Advisors");
   series->Tooltip(L"Count of deals: %VALUE%<BR>Date: %VARIABLE%");
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddInt(m_chart_info[i].expert_deals))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_deals->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare values of signal's
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_SIGNALS);
   series->Title(L"Trading Signals");
   series->Tooltip(L"Count of deals: %VALUE%<BR>Date: %VARIABLE%");
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddInt(m_chart_info[i].signal_deals))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_deals->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill chart of profit                                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::FillProfitChart(void)
  {
   MTAPIRES         res   =MT_RET_ERROR;
   IMTReportSeries *series=NULL;
   CMTStr64         str;
//--- checks
   if(m_api==NULL || m_chart_profit==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_chart_profit->Title(L"Profit and Loss of Clients");
   m_chart_profit->Digits(m_digits);
   PrepareChartTitle(m_chart_profit);
//--- prepare total values
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_TOTAL);
   series->Title(L"Total");
   str.Format(L"Profit/Loss: %%VALUE%%, %s<BR>Date: %%VARIABLE%%",m_currency);
   series->Tooltip(str.Str());
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddDouble(m_chart_info[i].total_profit))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare values of experts
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_EXPERTS);
   series->Title(L"Expert Advisors");
   str.Format(L"Profit/Loss: %%VALUE%%, %s<BR>Date: %%VARIABLE%%",m_currency);
   series->Tooltip(str.Str());
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddDouble(m_chart_info[i].expert_profit))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare values of signals
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_SIGNALS);
   series->Title(L"Trading Signals");
   str.Format(L"Profit/Loss: %%VALUE%%, %s<BR>Date: %%VARIABLE%%",m_currency);
   series->Tooltip(str.Str());
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddDouble(m_chart_info[i].signal_profit))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill chart of orders                                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::FillOrdersChart(void)
  {
   MTAPIRES         res   =MT_RET_ERROR;
   IMTReportSeries *series=NULL;
//--- checks
   if(m_api==NULL || m_chart_orders==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_chart_orders->Title(L"Orders of Clients");
   PrepareChartTitle(m_chart_orders);
//--- prepare total values
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_TOTAL);
   series->Title(L"Total");
   series->Tooltip(L"Count of orders: %VALUE%<BR>Date: %VARIABLE%");
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddInt(m_chart_info[i].total_orders))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_orders->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare values of expert's
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_EXPERTS);
   series->Title(L"Expert Advisors");
   series->Tooltip(L"Count of orders: %VALUE%<BR>Date: %VARIABLE%");
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddInt(m_chart_info[i].expert_orders))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_orders->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare values of signal's
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set properties
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_SIGNALS);
   series->Title(L"Trading Signals");
   series->Tooltip(L"Count of orders: %VALUE%<BR>Date: %VARIABLE%");
//--- add values
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
      if((res=series->ValueAddInt(m_chart_info[i].signal_orders))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
//--- add series
   if((res=m_chart_orders->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare bar chart (set titles and some general sets)             |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::PrepareChartTitle(IMTReportChart* chart)
  {
   MTAPIRES         res   =MT_RET_ERROR;
   IMTReportSeries *series=NULL;
   CMTStr32         str;
//--- checks
   if(m_api==NULL || chart==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare chart
   chart->Type(IMTReportChart::TYPE_GRAPH);
//--- prepare title series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_TITLE);
//---
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
     {
      str.Clear();
      //--- format date for current time
      str.Format(L"%02u.%02u.%04u",
         SMTTime::Day(m_chart_titles[i]),
         SMTTime::Month(m_chart_titles[i]),
         SMTTime::Year(m_chart_titles[i]));
      //--- add value
      if((res=series->ValueAdd(str.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series
   if((res=chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Process tables tags                                              |
//+------------------------------------------------------------------+
bool CDailyExpertReport::ProcessTable(MTAPISTR& tag,DailyInfo& info,MTAPIRES& retcode)
  {
   CMTStr128 str;
   double    converted=0;
//--- checks
   if(m_api==NULL || m_user==NULL || m_account==NULL || info.login==0)
     {
      retcode=MT_RET_ERR_PARAMS;
      return(true);
     }
//--- print login
   if(CMTStr::CompareNoCase(tag,L"login")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.login))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print name
   if(CMTStr::CompareNoCase(tag,L"name")==0)
     {
      if((retcode=m_api->HtmlWriteSafe(m_user->Name(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print group
   if(CMTStr::CompareNoCase(tag,L"group")==0)
     {
      if((retcode=m_api->HtmlWriteSafe(m_user->Group(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print leverage
   if(CMTStr::CompareNoCase(tag,L"floating")==0)
     {
      if((retcode=m_api->UserAccountGet(info.login,m_account))!=MT_RET_OK)
        {
         if(retcode==MT_RET_ERR_NOTFOUND)
            SMTFormat::FormatMoney(str,(double)0,m_digits);
         else
            return(true);
        }
      else
        {
         ConvertMoney(m_account->Floating(),converted);
         SMTFormat::FormatMoney(str,converted,m_digits);
        }
      if((retcode=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print profit
   if(CMTStr::CompareNoCase(tag,L"expert_profit")==0)
     {
      SMTFormat::FormatMoney(str,info.expert_profit,m_digits);
      if((retcode=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print profit
   if(CMTStr::CompareNoCase(tag,L"signal_profit")==0)
     {
      SMTFormat::FormatMoney(str,info.signal_profit,m_digits);
      if((retcode=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print total profit
   if(CMTStr::CompareNoCase(tag,L"total_profit")==0)
     {
      SMTFormat::FormatMoney(str,info.total_profit,m_digits);
      if((retcode=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print orders
   if(CMTStr::CompareNoCase(tag,L"expert_orders")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.expert_orders))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print orders
   if(CMTStr::CompareNoCase(tag,L"signal_orders")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.signal_orders))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print total orders
   if(CMTStr::CompareNoCase(tag,L"total_orders")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.total_orders))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print profit
   if(CMTStr::CompareNoCase(tag,L"expert_deals")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.expert_deals))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print profit
   if(CMTStr::CompareNoCase(tag,L"signal_deals")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.signal_deals))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print total profit
   if(CMTStr::CompareNoCase(tag,L"total_deals")==0)
     {
      if((retcode=m_api->HtmlWrite(L"%I64u",info.total_deals))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- print user's balance
   if(CMTStr::CompareNoCase(tag,L"balance")==0)
     {
      ConvertMoney(m_user->Balance(),converted);
      SMTFormat::FormatMoney(str,converted,m_digits);
      if((retcode=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(false);
     }
//--- done
   return(true);
  }
//+------------------------------------------------------------------+
//| Get currency from argument                                       |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::GetParameters(void)
  {
   MTAPIRES      res;
   IMTConReport* report   =NULL;
   IMTConParam*  parameter=NULL;
//--- get currency parameter
   if((report=m_api->ReportCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- create parameter interface
   if((parameter=m_api->ParamCreate())==NULL)
     {
      report->Release();
      return(MT_RET_ERR_MEM);
     }
//--- get report
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
     {
      parameter->Release();
      report->Release();
      return(res);
     }
//--- get currency parameter
   if((res=report->ParameterGet(L"Currency",parameter))!=MT_RET_OK)
   //--- fill parameter manually
      CMTStr::Copy(m_currency,DEFAULT_CURRENCY);
   else
      CMTStr::Copy(m_currency,parameter->Value());
//--- set digits parameter
   m_digits=SMTMath::MoneyDigits(m_currency);
//--- get groups parameter
   if((res=report->ParameterGet(L"Groups",parameter))!=MT_RET_OK)
   //--- fill parameter manually
      CMTStr::Copy(m_group_mask,L"real\\*");
   else
      CMTStr::Copy(m_group_mask,parameter->Value());
   report->Release();
   parameter->Release();
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Convert money by m_user currency                                 |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::ConvertMoney(const double money,double &converted)
  {
   MTAPIRES retcode=MT_RET_ERROR;
//--- clear parameters
   converted=0;
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- converting
   if((retcode=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(retcode);
   if((retcode=m_api->TradeRateSell(m_group->Currency(),m_currency,converted))!=MT_RET_OK)
      return(retcode);
   converted*=money;
//--- done
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Generate dashboard report                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::GenerateDashboard(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_daily,_countof(s_columns_daily));
   if(res!=MT_RET_OK)
      return(res);
//--- iterate chart items
   for(uint32_t i=0;i<REPORT_PERIOD;i++)
     {
      //--- fill record
      DateInfo info;
      info.date=m_chart_titles[i];
      info.expert_deals =m_chart_info[i].expert_deals;
      info.expert_orders=m_chart_info[i].expert_orders;
      info.expert_profit=m_chart_info[i].expert_profit;
      info.signal_deals =m_chart_info[i].signal_deals;
      info.signal_orders=m_chart_info[i].signal_orders;
      info.signal_profit=m_chart_info[i].signal_profit;
      info.total_deals  =m_chart_info[i].total_deals;
      info.total_orders =m_chart_info[i].total_orders;
      info.total_profit =m_chart_info[i].total_profit;
      //--- write rows
      if((res=data->RowWrite(&info,sizeof(info)))!=MT_RET_OK)
         return(res);
     }
//--- add deals charts
   if((res=AddDashboardChart(data,L"Deals of Clients",2))!=MT_RET_OK)
      return(res);
//--- add deals tables
   if((TableFromArray(*m_api,L"Users with the highest number of orders placed by Expert Advisors Top Deals",s_columns_table,_countof(s_columns_table),m_deals_expert,_countof(m_deals_expert)))!=MT_RET_OK)
      return(res);
   if((res=TableFromArray(*m_api,L"Users with the highest number of deals performed by Trading Signals",s_columns_table,_countof(s_columns_table),m_deals_signal,_countof(m_deals_signal)))!=MT_RET_OK)
      return(res);
//--- add profit charts
   if((res=AddDashboardChart(data,L"Profit and Loss of Clients",4))!=MT_RET_OK)
      return(res);
//--- add deals tables
   if((res=TableFromArray(*m_api,L"Users with the highest profit earned by Expert Advisors",s_columns_table,_countof(s_columns_table),m_profit_expert,_countof(m_profit_expert)))!=MT_RET_OK)
      return(res);
   if((res=TableFromArray(*m_api,L"Users with the highest profit earned by Trading Signals",s_columns_table,_countof(s_columns_table),m_profit_signal,_countof(m_profit_signal)))!=MT_RET_OK)
      return(res);
//--- add orders charts
   if((res=AddDashboardChart(data,L"Orders of Clients",3))!=MT_RET_OK)
      return(res);
//--- add orders tables
   if((res=TableFromArray(*m_api,L"Users with the highest number of deals performed by Expert Advisors",s_columns_table,_countof(s_columns_table),m_orders_expert,_countof(m_orders_expert)))!=MT_RET_OK)
      return(res);
   if((res=TableFromArray(*m_api,L"Users with the highest number of orders placed by Trading Signals",s_columns_table,_countof(s_columns_table),m_orders_signal,_countof(m_orders_signal)))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| create table from array                                          |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::TableFromArray(IMTReportAPI &api,LPCWSTR title,const ReportColumn *columns,const uint32_t columns_total,const DailyInfo *arr,const uint32_t total)
  {
//--- checks
   if(title==NULL || columns==NULL || columns_total==0 || arr==NULL || total==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,columns,columns_total);
   if(res!=MT_RET_OK)
      return(res);
//--- write rows
   for(uint32_t i=0;i<total;i++,arr++)
      if(arr->login)
        {
         //--- get user info
         if((res=api.UserGetLight(arr->login,m_user))!=MT_RET_OK)
            return(res);
         //--- fill record
         TableInfo info;
         info.login=arr->login;
         CMTStr::Copy(info.group,m_user->Name());
         CMTStr::Copy(info.group,m_user->Group());
         ConvertMoney(m_user->Balance(),info.balance);
         //--- floating
         if((res=m_api->UserAccountGet(info.login,m_account))!=MT_RET_OK)
           {
            if(res==MT_RET_ERR_NOTFOUND)
               info.floating=0.0;
            else
               return(res);
           }
         else
            ConvertMoney(m_account->Floating(),info.floating);
         //--- other fields
         info.expert_deals =arr->expert_deals;
         info.expert_orders=arr->expert_orders;
         info.expert_profit=arr->expert_profit;
         info.signal_deals =arr->signal_deals;
         info.signal_orders=arr->signal_orders;
         info.signal_profit=arr->signal_profit;
         info.total_deals  =arr->total_deals;
         info.total_orders =arr->total_orders;
         info.total_profit =arr->total_profit;
         //--- write rows
         if((res=data->RowWrite(&info,sizeof(info)))!=MT_RET_OK)
            return(res);
        }
//--- add table
   return(ReportColumn::TableAdd(api,res,data,title) ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Add Chart                                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyExpertReport::AddDashboardChart(IMTDataset *data,LPCWSTR title,uint32_t column)
  {
//--- checks
   if(m_api==NULL || data==NULL || title==NULL)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE);
   if(res!=MT_RET_OK)
      return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add data columns
   for(uint32_t i=0;i<3;i++)
      if((res=chart->DataColumnAdd(column+i*3))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Sort of expert's top array by count of profit                    |
//+------------------------------------------------------------------+
int32_t CDailyExpertReport::SortByExpertProfit(const void* left,const void* right)
  {
   DailyInfo* lft=(DailyInfo*)left;
   DailyInfo* rgh=(DailyInfo*)right;
//--- 
   if((lft->login==0) || (rgh->login==0))
     {
      if((lft->login!=0) && (rgh->login==0)) return(-1);
      if((lft->login==0) && (rgh->login!=0)) return(1);
      return(0);
     }
//--- 
   if((lft->expert_profit)<(rgh->expert_profit)) return(1);
   if((lft->expert_profit)>(rgh->expert_profit)) return(-1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort of signal's top array by count of profit                    |
//+------------------------------------------------------------------+
int32_t CDailyExpertReport::SortBySignalProfit(const void* left,const void* right)
  {
   DailyInfo* lft=(DailyInfo*)left;
   DailyInfo* rgh=(DailyInfo*)right;
//--- 
   if((lft->login==0) || (rgh->login==0))
     {
      if((lft->login!=0) && (rgh->login==0)) return(-1);
      if((lft->login==0) && (rgh->login!=0)) return(1);
      return(0);
     }
//--- 
   if((lft->signal_profit)<(rgh->signal_profit)) return(1);
   if((lft->signal_profit)>(rgh->signal_profit)) return(-1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort of expert's top array by orders                             |
//+------------------------------------------------------------------+
int32_t CDailyExpertReport::SortByExpertOrders(const void* left,const void* right)
  {
   DailyInfo* lft=(DailyInfo*)left;
   DailyInfo* rgh=(DailyInfo*)right;
//--- 
   if((lft->login==0) || (rgh->login==0))
     {
      if((lft->login!=0) && (rgh->login==0)) return(-1);
      if((lft->login==0) && (rgh->login!=0)) return(1);
      return(0);
     }
//--- 
   if((lft->expert_orders)<(rgh->expert_orders)) return(1);
   if((lft->expert_orders)>(rgh->expert_orders)) return(-1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort of signal's top array by orders                             |
//+------------------------------------------------------------------+
int32_t CDailyExpertReport::SortBySignalOrders(const void* left,const void* right)
  {
   DailyInfo* lft=(DailyInfo*)left;
   DailyInfo* rgh=(DailyInfo*)right;
//--- 
   if((lft->login==0) || (rgh->login==0))
     {
      if((lft->login!=0) && (rgh->login==0)) return(-1);
      if((lft->login==0) && (rgh->login!=0)) return(1);
      return(0);
     }
//--- 
   if((lft->signal_orders)<(rgh->signal_orders)) return(1);
   if((lft->signal_orders)>(rgh->signal_orders)) return(-1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort of expert's top array by deals                              |
//+------------------------------------------------------------------+
int32_t CDailyExpertReport::SortByExpertDeals(const void* left,const void* right)
  {
   DailyInfo* lft=(DailyInfo*)left;
   DailyInfo* rgh=(DailyInfo*)right;
//--- 
   if((lft->login==0) || (rgh->login==0))
     {
      if((lft->login!=0) && (rgh->login==0)) return(-1);
      if((lft->login==0) && (rgh->login!=0)) return(1);
      return(0);
     }
//--- 
   if((lft->expert_deals)<(rgh->expert_deals)) return(1);
   if((lft->expert_deals)>(rgh->expert_deals)) return(-1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort of signal's top array by deals                              |
//+------------------------------------------------------------------+
int32_t CDailyExpertReport::SortBySignalDeals(const void* left,const void* right)
  {
   DailyInfo* lft=(DailyInfo*)left;
   DailyInfo* rgh=(DailyInfo*)right;
//--- 
   if((lft->login==0) || (rgh->login==0))
     {
      if((lft->login!=0) && (rgh->login==0)) return(-1);
      if((lft->login==0) && (rgh->login!=0)) return(1);
      return(0);
     }
//--- 
   if((lft->signal_deals)<(rgh->signal_deals)) return(1);
   if((lft->signal_deals)>(rgh->signal_deals)) return(-1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+