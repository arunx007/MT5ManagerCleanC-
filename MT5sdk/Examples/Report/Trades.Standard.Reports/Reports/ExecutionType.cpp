//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExecutionType.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CExecutionType::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_9,                     // minimal IE version
   L"Execution Types",                             // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",        // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_ACCOUNTS_FULL,                 // snapshot modes
   MTReportInfo::TYPE_HTML|MTReportInfo::TYPE_DASHBOARD, // report types
   L"Trades",                                      // report category
                                                   // request parameters
                         {
        { MTReportParam::TYPE_DATE,    MTAPI_PARAM_FROM,},
      { MTReportParam::TYPE_DATE,    MTAPI_PARAM_TO,  },
     }, 2,                                         // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING, L"Currency", DEFAULT_CURRENCY },
     }, 1,                                         // config parameters total
   {0}                                             // reserved
  };
//+------------------------------------------------------------------+
//| Colors                                                           |
//+------------------------------------------------------------------+
uint32_t CExecutionType::s_reason_colors[REASONS_COUNT]=
  {
   ExtChartColors[8],                              // deal placed manually
   ExtChartColors[0],                              // deal placed by expert
   ExtChartColors[2],                              // deal placed by dealer
   ExtChartColors[4],                              // deal placed due SL
   ExtChartColors[4],                              // deal placed due TP
   ExtChartColors[4],                              // deal placed due SO
   ExtChartColors[1],                              // deal placed due rollover
   ExtChartColors[3],                              // deal placed from external system
   ExtChartColors[5],                              // deal placed due variation margin
   ExtChartColors[6],                              // deal placed by gateway
   ExtChartColors[7],                              // deal placed by signal service
   ExtChartColors[9],                              // deal placed due to settlement
   ExtChartColors[10],                             // deal placed due to position transfer
   };
//+------------------------------------------------------------------+
//| Chart reasons                                                    |
//+------------------------------------------------------------------+
uint32_t CExecutionType::s_reasons_chart[]=
   {
   IMTDeal::DEAL_REASON_CLIENT,
   IMTDeal::DEAL_REASON_EXPERT,
   IMTDeal::DEAL_REASON_DEALER,
   IMTDeal::DEAL_REASON_SL,
   IMTDeal::DEAL_REASON_ROLLOVER,
   IMTDeal::DEAL_REASON_EXTERNAL_CLIENT,
   IMTDeal::DEAL_REASON_VMARGIN,
   IMTDeal::DEAL_REASON_GATEWAY,
   IMTDeal::DEAL_REASON_SIGNAL,
   IMTDeal::DEAL_REASON_SETTLEMENT,
   IMTDeal::DEAL_REASON_TRANSFER,
   IMTDeal::DEAL_REASON_SYNC,
   IMTDeal::DEAL_REASON_EXTERNAL_SERVICE,
   IMTDeal::DEAL_REASON_MIGRATION,
   IMTDeal::DEAL_REASON_MOBILE,
   IMTDeal::DEAL_REASON_WEB,
   IMTDeal::DEAL_REASON_SPLIT,
   };
//+------------------------------------------------------------------+
//| Table reasons                                                    |
//+------------------------------------------------------------------+
uint32_t CExecutionType::s_reasons_table[]=
   {
   IMTDeal::DEAL_REASON_CLIENT,
   IMTDeal::DEAL_REASON_MOBILE,
   IMTDeal::DEAL_REASON_WEB,
   IMTDeal::DEAL_REASON_EXPERT,
   IMTDeal::DEAL_REASON_DEALER,
   IMTDeal::DEAL_REASON_SL,
   IMTDeal::DEAL_REASON_ROLLOVER,
   IMTDeal::DEAL_REASON_EXTERNAL_CLIENT,
   IMTDeal::DEAL_REASON_VMARGIN,
   IMTDeal::DEAL_REASON_GATEWAY,
   IMTDeal::DEAL_REASON_SIGNAL,
   IMTDeal::DEAL_REASON_SETTLEMENT,
   IMTDeal::DEAL_REASON_TRANSFER,
   };
//+------------------------------------------------------------------+
//| Column description Groups                                        |
//+------------------------------------------------------------------+
ReportColumn CExecutionType::s_columns_group[]=
  {
   //--- id,name,             type,                         width,  width_max, offset,                      size,digits_column,flags
     { 1,L"Group",            IMTDatasetColumn::TYPE_STRING ,20,0, offsetof(GroupStatRecord,group)           ,MtFieldSize(GroupStatRecord,group) ,0,0 },
   { 2,  L"Accounts",         IMTDatasetColumn::TYPE_UINT32 ,10,0, offsetof(GroupStatRecord,accounts)        ,0,0,0 },
   { 3,  L"Active %",         IMTDatasetColumn::TYPE_DOUBLE ,10,0, offsetof(GroupStatRecord,active)          ,0,0,0 },
   { 4,  L"Balance",          IMTDatasetColumn::TYPE_MONEY  ,10,0, offsetof(GroupStatRecord,balance)         ,0,8,0 },
   { 5,  L"Floating P/L",     IMTDatasetColumn::TYPE_MONEY  ,10,0, offsetof(GroupStatRecord,profit)          ,0,8,0 },
   { 6,  L"Equity",           IMTDatasetColumn::TYPE_MONEY  ,10,0, offsetof(GroupStatRecord,equity)          ,0,8,0 },
   { 7,  L"Currency",         IMTDatasetColumn::TYPE_STRING , 5,0, offsetof(GroupStatRecord,currency)        ,MtFieldSize(GroupStatRecord,currency),0,IMTDatasetColumn::FLAG_LEFT },
   { 8,  L"Currency digits",  IMTDatasetColumn::TYPE_UINT8  , 5,0, offsetof(GroupStatRecord,currency_digits) ,0,0,IMTDatasetColumn::FLAG_HIDDEN },
  };
//+------------------------------------------------------------------+
//| Column description Deals                                         |
//+------------------------------------------------------------------+
ReportColumn CExecutionType::s_columns_deals[]=
  {
   //--- id,name,          type,                         width,  width_max, offset,                size,digits_column,flags
     { 1,L"Date",          IMTDatasetColumn::TYPE_DATE   ,12,0, offsetof(DealRecord,date)           ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
  };
//+------------------------------------------------------------------+
//| Column description Deal                                          |
//+------------------------------------------------------------------+
ReportColumn CExecutionType::s_columns_deal[]=
  {
   //--- id,name,          type,                             width,  width_max, offset,             size,digits_column,flags
     { 2,L" Count",        IMTDatasetColumn::TYPE_UINT32 , 8,0, offsetof(DealInfo,count)            ,0,0,0 },
   { 3,  L" Volume",       IMTDatasetColumn::TYPE_MONEY  ,10,0, offsetof(DealInfo,volume)           ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExecutionType::CExecutionType(void) : m_api(NULL),m_currency(NULL),m_currency_digits(0),m_param_from(0),m_param_to(0),m_days_total(0),m_months_total(0)
  {
//--- fill data with zeros
   ZeroMemory(m_chart_deals_count, sizeof(m_chart_deals_count));
   ZeroMemory(m_chart_deals_volume,sizeof(m_chart_deals_volume));
   ZeroMemory(m_deals_count,sizeof(m_deals_count));
   ZeroMemory(m_deals_volume,sizeof(m_deals_volume));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExecutionType::~CExecutionType(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CExecutionType::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CExecutionType::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::Generate(const uint32_t type,IMTReportAPI *api)
  {
   uint32_t    interval;
   MTAPIRES    res;
//--- common
   MTAPISTR    tag;
   uint32_t    counter                          =0;
//--- subgroup info at accounts report          
   GroupRecord current_subgroup                 ={};
   uint32_t    current_subgroup_index           =0;
//--- write delimiter after second time being seen
   bool        write_delimeter                  =false;
//--- current group type and name
   uint32_t    current_group_type               =0;
   CMTStr256   current_group_type_name;
//--- current month index
   uint32_t    current_month_index              =0;
//--- current group deals count and volume
   uint32_t    current_group_type_deals_count   =0;
   DOUBLE      current_group_type_deals_volume  =0.0;
//--- checks
   if(api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- type checks
   if(type!=MTReportInfo::TYPE_HTML && type!=MTReportInfo::TYPE_DASHBOARD)
      res=MT_RET_ERR_NOTIMPLEMENT;
//--- remember pointer
   m_api=api;
//--- capture currency from report parameters
   if((res=GetCurency())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- prepare data for charts
   if((res=CalculateStatictics())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- which interval to be used
   if(m_days_total<30)
      interval=INTERVAL_DAY;
   else
     {
      if(m_days_total<60)
         interval=INTERVAL_WEEK;
      else
         interval=INTERVAL_MONTH;
     }
//--- prepare html charts
   if(type==MTReportInfo::TYPE_HTML)
     {
      //--- iterate over each group type
      for(uint32_t g=0;g<GROUP_COUNT;g++)
        {
         uint32_t    deals   =0;
         DOUBLE  volume  =0;
         //--- get total deals count and volume
         if(!GetDealsTotal(g,deals,volume))
            return(MT_RET_ERROR);
         //--- skip empty graphs
         if(deals==0)
            continue;
         //--- prepare deals' count graph
         if((res=PrepareGraph(g,interval,CHART_DEALS_COUNT,m_chart_deals_count[g]))!=MT_RET_OK)
            return(res);
         //--- prepare deals' volume graph
         if((res=PrepareGraph(g,interval,CHART_DEALS_VOLUME,m_chart_deals_volume[g]))!=MT_RET_OK)
            return(res);
        }
      //--- load html template
      if((res=m_api->HtmlTplLoadResource(IDR_EXECUTION_TYPE,RT_HTML))!=MT_RET_OK)
         return(res);
      //--- HtmlTplNext return tag string and counter, that shows how many times tag has been used
      while(m_api->HtmlTplNext(tag,&counter)==MT_RET_OK)
        {
         //--- write common header tags
         if(WriteHeader(tag,res))
           {
            if(res!=MT_RET_OK)
               break;
            continue;
           }
         //--- proceed group's table iterator
         if(WriteGroupType(tag,counter,res,current_group_type,current_group_type_name,current_group_type_deals_count,current_group_type_deals_volume))
           {
            if(res!=MT_RET_OK)
               break;
            continue;
           }
         //--- group type
         if(CMTStr::CompareNoCase(tag,L"group_type")==0)
           {
            //--- write delimiter only after second occurs
            if(current_group_type_deals_count>0&&counter==0)
              {
               if(write_delimeter)
                 {
                  if((res=m_api->HtmlWrite(L"<div class=\"long_vertical_space\" ></div>"))!=MT_RET_OK)
                     break;
                 }
               write_delimeter=true;
               if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  break;
              }
            continue;
           }
         //--- type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            if((res=m_api->HtmlWriteSafe(current_group_type_name.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               break;
            continue;
           }
         //--- line position
         if(CMTStr::CompareNoCase(tag,L"line")==0)
           {
            if((res=m_api->HtmlWrite(L"%u",current_subgroup_index&1))!=MT_RET_OK)
               break;
            continue;
           }
         //--- write whole groups report
         if(WriteGroupsReport(tag,counter,res,current_subgroup,current_subgroup_index))
           {
            if(res!=MT_RET_OK)
               break;
            continue;
           }
         //--- draw chart of deals count
         if(CMTStr::CompareNoCase(tag,L"chart_deals_count")==0)
           {
            if(current_group_type>=GROUP_COUNT||m_chart_deals_count[current_group_type]==NULL)
               continue;
            if((res=m_api->ChartWriteHtml(m_chart_deals_count[current_group_type]))!=MT_RET_OK)
               break;
            continue;
           }
         //--- draw chart of deals volume
         if(CMTStr::CompareNoCase(tag,L"chart_deals_volume")==0)
           {
            if(current_group_type>=GROUP_COUNT||m_chart_deals_volume[current_group_type]==NULL)
               continue;
            if((res=m_api->ChartWriteHtml(m_chart_deals_volume[current_group_type]))!=MT_RET_OK)
               break;
            continue;
           }
         //--- write deals report table
         if(WriteDealsTable(tag,counter,res,current_group_type,current_group_type_deals_count,current_month_index))
           {
            if(res!=MT_RET_OK)
               break;
            continue;
           }
        }//proceed all templates
     }
   else
     {
      //--- generate dashboard
      if((res=GenerateDashboard(interval))!=MT_RET_OK)
        {
         Clear();
         return(res);
        }
     }
//--- clear 
   Clear();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Allocate memory for storing internal data                        |
//+------------------------------------------------------------------+
bool CExecutionType::AllocateMemory(void)
  {
   for(int32_t g=0;g<GROUP_COUNT;g++)
     {
      for(int32_t r=IMTDeal::DEAL_REASON_FIRST;r<=IMTDeal::DEAL_REASON_LAST;r++)
        {
         //--- allocate memory
         if((m_deals_count[g][r]=new(std::nothrow) UINT[m_days_total])==NULL)
            return(false);
         //--- fill with zeros
         ZeroMemory(m_deals_count[g][r], m_days_total*sizeof(m_deals_count[g][r][0]));
         //--- allocate memory
         if((m_deals_volume[g][r]=new(std::nothrow) DOUBLE[m_days_total])==NULL)
            return(false);
         //--- fill with zeros
         ZeroMemory(m_deals_volume[g][r], m_days_total*sizeof(m_deals_volume[g][r][0]));
        }
     }
   return(true);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CExecutionType::Clear(void)
  {
//--- fill with zeros
   for(int32_t g=0;g<GROUP_COUNT;g++)
     {
      for(int32_t r=IMTDeal::DEAL_REASON_FIRST;r<=IMTDeal::DEAL_REASON_LAST;r++)
        {
         delete[] m_deals_count[g][r];
         delete[] m_deals_volume[g][r];
        }
      //--- release
      if(m_chart_deals_count[g]!=NULL)
         m_chart_deals_count[g]->Release();
      //--- release
      if(m_chart_deals_volume[g]!=NULL)
         m_chart_deals_volume[g]->Release();
     }
//--- fill data with zeros
   ZeroMemory(m_chart_deals_count, sizeof(m_chart_deals_count));
   ZeroMemory(m_chart_deals_volume,sizeof(m_chart_deals_volume));
   ZeroMemory(m_deals_count,sizeof(m_deals_count));
   ZeroMemory(m_deals_volume,sizeof(m_deals_volume));
//--- clear groups and groups total
   m_groups.Clear();
   m_groups_totals.Clear();
//--- release currency context
   if(m_currency)
     {
      m_currency->Release();
      m_currency=NULL;
     }
//--- fill with zero
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Get parameter currency                                           |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::GetCurency(void)
  {
   MTAPIRES      res;
   IMTConReport* report=NULL;
//--- check
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create parameter context
   if((m_currency=m_api->ParamCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- get currency parameter
   if((report=m_api->ReportCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- get currency parameter
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
     {
      report->Release();
      return(res);
     }
//--- get currency parameter
   if((res=report->ParameterGet(L"Currency",m_currency))!=MT_RET_OK)
     {
      //--- fill parameter manually
      if((res=m_currency->Name(L"Currency"))!=MT_RET_OK)
        {
         report->Release();
         return(res);
        }

      if((res=m_currency->Type(IMTConParam::TYPE_STRING))!=MT_RET_OK)
        {
         report->Release();
         return(res);
        }

      if((res=m_currency->Value(DEFAULT_CURRENCY))!=MT_RET_OK)
        {
         report->Release();
         return(res);
        }
     }
//--- get currency parameter value
   if(m_currency->Value()==NULL)
     {
      report->Release();
      return(MT_RET_ERR_PARAMS);
     }
//--- setup digits
   m_currency_digits=SMTMath::MoneyDigits(m_currency->Value());
//--- release context
   report->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Request data from server and prepare it for show                 |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::CalculateStatictics(void)
  {
   MTAPIRES      res;
   IMTConGroup  *group    =NULL;
   IMTDealArray *deals    =NULL;
//--- checks
   if(m_api==NULL || m_currency==NULL)
      return(MT_RET_ERR_PARAMS);
//--- get range
   m_param_from=m_api->ParamFrom();
   m_param_to  =m_api->ParamTo();
//--- check
   if(m_param_to<m_param_from)
      return(MT_RET_ERR_PARAMS);
//--- parse range
   tm tm_from,tm_to;
   if(!SMTTime::ParseTime(m_param_from,&tm_from))
      return(MT_RET_ERROR);
   if(!SMTTime::ParseTime(m_param_to,&tm_to))
      return(MT_RET_ERROR);
//--- compute total months count
   m_months_total=(tm_to.tm_year*12+tm_to.tm_mon)+1-(tm_from.tm_year*12+tm_from.tm_mon);
//--- compute numbers of days to proceed
   m_days_total=(uint32_t)(m_param_to-m_param_from+1)/SECONDS_IN_DAY;
//--- if zero -> nothing to do
   if(m_days_total==0)
      return(MT_RET_OK_NONE);
//--- if first day of month, exclude it
   if(tm_to.tm_mday==1)
      m_months_total-=1;
//--- allocate memory and fill arrays with 'empty' values
   if(!AllocateMemory())
      return(MT_RET_ERR_MEM);
//--- create group context
   if((group=m_api->GroupCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- get current time
   uint64_t deals_processed=0;
   const CReportTimer timer;
//--- create context for deals array
   if((deals=m_api->DealCreateArray())==NULL)
     {
      group->Release();
      return(MT_RET_ERR_MEM);
     }
//--- buffer
   CMTStr256 str;
//--- iterate groups
   for(uint32_t pos=0;m_api->GroupNext(pos,group)==MT_RET_OK;pos++)
     {
      if((res=CalculateGroup(group,deals,deals_processed))!=MT_RET_OK)
        {
         deals->Release();
         group->Release();
         return(res);
        }
     }// iterate over each group
//--- write time info to log
   if((res=m_api->LoggerOut(MTLogOK,L"%I64u deal records collected in %I64u ms",deals_processed,timer.Elapsed()))!=MT_RET_OK)
     {
      deals->Release();
      group->Release();
      return(res);
     }
//--- memory free
   deals->Release();
   group->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate statistics for a group                                 |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::CalculateGroup(const IMTConGroup *group,IMTDealArray *deals,uint64_t &deals_processed)
  {
   MTAPIRES res;
   uint64_t  *logins      =NULL;
   uint32_t logins_total=0;
//--- check
   if(m_api==NULL)
      return(MT_RET_ERROR);
//--- compute deal size in currency
   DOUBLE rate=0;
//--- use group currency
   if((res=m_api->TradeRateSell(group->Currency(),m_currency->Value(),rate))!=MT_RET_OK)
      return(res);
//--- check
   if(fabs(rate)<DBL_EPSILON)
      return(MT_RET_OK);
//--- get logins for group
   if((res=m_api->UserLogins(group->Group(),logins,logins_total))!=MT_RET_OK)
     {
      if(logins!=NULL)
         m_api->Free(logins);
      return(res);
     }
//--- check
   if(logins==NULL||logins_total==0)
      return(MT_RET_OK);
//--- iterate logins
   for(uint32_t i=0;i<logins_total;i++)
     {
      //--- calculate for a login
      if((res=CalculateLogin(logins[i],group,rate,deals,deals_processed))!=MT_RET_OK)
        {
         m_api->Free(logins);
         return(res);
        }
     }//iterate over each login
   m_api->Free(logins);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Calculate about a login                                          |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::CalculateLogin(const uint64_t login,const IMTConGroup *group,DOUBLE rate,IMTDealArray *deals,uint64_t &deals_processed)
  {
   MTAPIRES res;
//--- check
   if(m_api==NULL)
      return(MT_RET_ERROR);
//--- get all client's deals
   if((res=m_api->DealGet(login,m_param_from,m_param_to,deals))!=MT_RET_OK)
      return(res);
//--- get group number by name
   uint32_t groupNum=0;
   if(!GetGroupType(group->Group(),groupNum))
      return(MT_RET_OK);
//--- double check
   if(groupNum>=GROUP_COUNT)
      return(MT_RET_ERROR);
//--- get elements count
   const uint32_t dc=deals->Total();
   if(dc==0)
      return(MT_RET_OK);
//--- iterate  over each deal
   for(uint32_t d=0;d<dc;++d)
     {
      //--- get deal
      IMTDeal *deal=deals->Next(d);
      //--- check
      if(deal==NULL)
         return(MT_RET_ERROR);
      //--- only for out buy/sell deals
      if((deal->Entry()!=IMTDeal::ENTRY_OUT&&deal->Entry()!=IMTDeal::ENTRY_INOUT) ||
        !(deal->Action()==IMTDeal::DEAL_BUY||deal->Action()==IMTDeal::DEAL_SELL))
         continue;
      //--- skip out of range
      if(deal->Time()<m_param_from||deal->Time()>=m_param_to)
         continue;
      //--- convert deal time to day index
      int64_t index=0;
      if(!TimeToDayIndex(deal->Time(),index))
         continue;
      //--- double check
      if(index<0)
         return(MT_RET_ERROR);
      //--- double check
      if(index>=m_days_total)
         continue;
      //--- get reason
      const uint32_t reason=deal->Reason();
      //--- double check
      if(reason>=REASONS_COUNT)
         return(MT_RET_ERROR);
      //--- check
      if(deal->Volume()==0 ||
        fabs(deal->ContractSize())<DBL_EPSILON ||
        fabs(deal->RateProfit())<DBL_EPSILON ||
        _isnan(deal->RateProfit()))
         continue;
      //--- compute count
      m_deals_count[groupNum][reason][index]++;
      deals_processed++;
      //--- compute volume
      DOUBLE volume=fabs((deal->Volume()/10000.0)*deal->ContractSize()*deal->RateProfit());
      volume=SMTMath::PriceNormalize(volume*rate,m_currency_digits);
      m_deals_volume[groupNum][reason][index]=SMTMath::MoneyAdd(volume,m_deals_volume[groupNum][reason][index],m_currency_digits);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare information for group type                               |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::PrepareTableGroupType(const uint32_t type)
  {
   const int64_t    checktime     =_time64(nullptr)-3*30*86400;  // 3 month early
   uint32_t       logins_total  =0;
   GroupRecord    group         ={};
   MTAPIRES       res;
   IMTConGroup   *con_group     =NULL;
   IMTAccount    *account       =NULL;
   IMTUser       *user          =NULL;
   uint64_t        *logins        =NULL;
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- clear groups and groups totals
   m_groups.Clear();
   m_groups_totals.Clear();
//--- create context for group
   con_group=m_api->GroupCreate();
   if(con_group==NULL)
      return(MT_RET_ERR_MEM);
//--- create context for user account
   account=m_api->UserCreateAccount();
   if(account==NULL)
     {
      con_group->Release();
      return(MT_RET_ERR_MEM);
     }
//--- create context for user
   user=m_api->UserCreate();
   if(user==NULL)
     {
      account->Release();
      con_group->Release();
      return(MT_RET_ERR_MEM);
     }
//--- iterate over each group
   for(uint32_t pos=0;m_api->GroupNext(pos,con_group)==MT_RET_OK;pos++)
     {
      uint32_t group_type;
      //--- check group type
      if(!GetGroupType(con_group->Group(),group_type))
         continue;
      if(group_type!=type)
         continue;
      //--- clear group record
      ZeroMemory(&group,sizeof(group));
      //--- fill group information
      CMTStr::Copy(group.group   ,con_group->Group());
      CMTStr::Copy(group.currency,con_group->Currency());
      //--- get currency digits
      group.currency_digits=con_group->CurrencyDigits();
      //--- get logins for group
      if((res=m_api->UserLogins(group.group,logins,logins_total))!=MT_RET_OK)
        {
         user->Release();
         account->Release();
         con_group->Release();
         return(res);
        }
      //--- checks logins
      if(logins==NULL)
         continue;
      //--- fill group information
      group.accounts=logins_total;
      //--- iterate through logins
      for(uint32_t i=0;i<logins_total;i++)
        {
         //--- get trade account
         if((res=m_api->UserAccountGet(logins[i],account))!=MT_RET_OK)
           {
            //--- not found is normal retcode (it means that user has not trades)
            if(res!=MT_RET_ERR_NOTFOUND)
              {
               m_api->Free(logins);
               user->Release();
               account->Release();
               con_group->Release();
               return(res);
              }
            //--- get user record
            if((res=m_api->UserGetLight(logins[i],user))!=MT_RET_OK)
              {
               m_api->Free(logins);
               user->Release();
               account->Release();
               con_group->Release();
               return(res);
              }
            //--- check
            if(_isnan(user->Balance()))
              {
               if((res=m_api->LoggerOut(MTLogErr,L"Account '%I64u' is broken",logins[i]))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  user->Release();
                  account->Release();
                  con_group->Release();
                  return(res);
                 }
               continue;
              }
            //--- calculate balance and equity using user record
            group.balance=SMTMath::MoneyAdd(group.balance,user->Balance(),group.currency_digits);
            group.equity =SMTMath::MoneyAdd(group.equity,user->Balance(),group.currency_digits);
            //--- collect 3 months active accounts
            if(checktime<user->LastAccess())
               group.active++;
           }
         else
           {
            //--- checks
            if(_isnan(account->Balance()) ||
              _isnan(account->Equity()) ||
              _isnan(account->Profit()))
              {
               if((res=m_api->LoggerOut(MTLogErr,L"Account '%I64u' is broken",logins[i]))!=MT_RET_OK)
                 {
                  m_api->Free(logins);
                  user->Release();
                  account->Release();
                  con_group->Release();
                  return(res);
                 }
               continue;
              }
            //--- calculate balance and equity using trade account
            group.balance=SMTMath::MoneyAdd(group.balance,account->Balance(),group.currency_digits);
            group.equity =SMTMath::MoneyAdd(group.equity,account->Equity(),group.currency_digits);
            group.profit =SMTMath::MoneyAdd(group.profit,account->Profit(),group.currency_digits);
           }
        }
      //--- add group
      if((res=PrepareGroupAdd(group))!=MT_RET_OK)
        {
         m_api->Free(logins);
         user->Release();
         account->Release();
         con_group->Release();
         return(res);
        }
     }
//--- sort groups  &totals
   m_groups.Sort(SortGroupByName);
   m_groups_totals.Sort(SortGroupByAccounts);
//--- checks totals
   if(!m_groups_totals.Total())
     {
      //--- clear group
      ZeroMemory(&group,sizeof(group));
      //--- add empty group
      if(!m_groups_totals.Add(&group))
        {
         if(logins!=NULL)
            m_api->Free(logins);
         user->Release();
         account->Release();
         con_group->Release();
         return(MT_RET_ERR_MEM);
        }
     }
//--- memory free
   if(logins!=NULL)
      m_api->Free(logins);
   user->Release();
   account->Release();
   con_group->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Deal Volume Proportion Chart                             |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::PrepareGraph(const uint32_t group_type,const uint32_t interval,const uint32_t chart_type,IMTReportChart *&chart)
  {
   MTAPIRES          res;
   IMTReportSeries  *header_series      =NULL;
   IMTReportSeries  *values_series      =NULL;
   uint32_t          day_index          =0;
   CMTStr256         str,group_name;
   uint32_t          deals_total        =0;
   DOUBLE            volume_total       =0;
   uint32_t          deals_per_reason   =0;
   DOUBLE            volume_per_reason  =0.0;
   uint32_t          deals_a_week       =0;
//--- checks
   if(m_api==NULL||group_type>=GROUP_COUNT||interval>=INTERVAL_COUNT)
      return(MT_RET_ERR_PARAMS);
//--- get total deals count for specified group
   if(!GetDealsTotal(group_type,deals_total,volume_total))
      return(MT_RET_ERROR);
//--- if none, skip it
   if(deals_total==0)
      return(MT_RET_OK_NONE);
//--- reset values
   deals_total =0;
   volume_total=0.0;
//--- create chart
   if((chart=m_api->ChartCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- and do clean it
   if((res=chart->Clear())!=MT_RET_OK)
      return(res);
//--- get current time
   const int64_t ctm=m_api->TimeCurrent();
   tm tm_ctm={0};
   if(!SMTTime::ParseTime(ctm,&tm_ctm))
      return(MT_RET_ERROR);
//--- get type name
   if(!GetGroupTypeName(group_type,group_name))
      return(MT_RET_ERR_PARAMS);
//--- set chart type
   if((res=chart->Type(IMTReportChart::TYPE_GRAPH_ACCUMULATION))!=MT_RET_OK)
      return(res);
//--- format string
   str.Assign(L"Deals ");
   str.Append(chart_type==CHART_DEALS_COUNT?L"Count":L"Volume");
   str.Append(L" Graph");
//--- set title
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(res);
//--- prepare title series
   if((header_series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//--- set header type
   if((res=header_series->Type(IMTReportSeries::TYPE_TITLE))!=MT_RET_OK)
     {
      header_series->Release();
      return(res);
     }
//--- month
   if(interval==INTERVAL_MONTH)
     {
      uint32_t    deals;
      DOUBLE  volume;
      //--- iterate over each month
      for(uint32_t i=0;i<m_months_total;i++)
        {
         //--- get all deals for specified group for exact month
         if(!GetAllDealsByMonth(group_type,i,deals,volume))
           {
            header_series->Release();
            return(MT_RET_ERROR);
           }
         //--- if none
         if(deals==0)
            continue;
         //--- convert month index to human readable string
         if(!MonthIndexToStr(i,str))
           {
            header_series->Release();
            return(MT_RET_ERROR);
           }
         //--- append string to series
         if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
           {
            header_series->Release();
            return(res);
           }
        }
     }
   else//if interval is DAY or WEEK
     {
      //--- iterate over days
      for(day_index=0;day_index<m_days_total;day_index++)
        {
         uint32_t    deals;
         DOUBLE  volume;
         //--- get all deals count and volume for exact group and day
         if(!GetDealsByDay(group_type,day_index,deals,volume))
           {
            header_series->Release();
            return(MT_RET_ERROR);
           }
         //--- choose interval
         if(interval==INTERVAL_DAY)
           {
            //--- skip days with no deals at all
            if(deals==0)
               continue;
            //--- convert day index to human readable format
            if(!DayIndexToStr(day_index,str))
              {
               header_series->Release();
               return(MT_RET_ERROR);
              }
            //--- add it to series
            if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
              {
               header_series->Release();
               return(res);
              }
           }
         else
            //--- if interval week
            if(interval==INTERVAL_WEEK)
              {
               //--- remember it for computing of weekly data
               deals_a_week+=deals;
               //--- if this day are seventh
               if((day_index+1)%DAYS_IN_WEEK==0)
                 {
                  //--- skip week with no deals at all
                  if(deals_a_week==0)
                     continue;
                  //--- convert day index to human readable format
                  if(!DayIndexToStr(day_index+1-DAYS_IN_WEEK,str))
                    {
                     header_series->Release();
                     return(MT_RET_ERROR);
                    }
                  //--- add it to series
                  if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
                    {
                     header_series->Release();
                     return(res);
                    }
                  //--- reset week counter
                  deals_a_week=0;
                 }
              }
        }//day iterator
     }
//--- append last week,because at previous step we used only "next week" value
   if(interval==INTERVAL_WEEK&&deals_a_week!=0)
     {
      //--- add last day element
      if(day_index%DAYS_IN_WEEK!=0)
        {
         //--- get start date of last week
         if(!DayIndexToStr(day_index-day_index%DAYS_IN_WEEK,str))
           {
            header_series->Release();
            return(MT_RET_ERROR);
           }
         //--- add to series
         if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
           {
            header_series->Release();
            return(res);
           }
        }
     }
//--- add series to chart (after adding series will be released by chart)
   if((res=chart->SeriesAdd(header_series))!=MT_RET_OK)
     {
      header_series->Release();
      return(res);
     }
//--- reset counters
   deals_a_week=0;
//--- iterate over reason types
   for(uint32_t r=IMTDeal::DEAL_REASON_FIRST;r<=IMTDeal::DEAL_REASON_LAST;r++)
     {
      //--- get human readable reason's type name
      if(!GetReasonTypeName(r,str))
         continue;
      //--- prepare values series
      if((values_series=m_api->ChartCreateSeries())==NULL)
         return(MT_RET_ERR_MEM);
      //--- set series type
      if((res=values_series->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
        {
         values_series->Release();
         return(res);
        }
      //--- format tooltip
      CMTStr256 tt;
      tt.Append(L"Date: %VARIABLE%<BR>");
      //--- for StopLoss reason use common word 'Stop'
      tt.Append(str);
      tt.Append(L": %VALUE% (%NORMALIZED_VALUE% %)");
      //--- set tooltip
      if((res=values_series->Tooltip(tt.Str()))!=MT_RET_OK)
        {
         values_series->Release();
         return(res);
        }
      //--- set color
      if((res=values_series->Color(s_reason_colors[r]))!=MT_RET_OK)
        {
         values_series->Release();
         return(res);
        }
      //--- set title
      if((res=values_series->Title(str.Str()))!=MT_RET_OK)
        {
         values_series->Release();
         return(res);
        }
      //--- reset values
      deals_a_week      =0;
      deals_per_reason  =0;
      volume_per_reason =0.0;
      //--- if month interval
      if(interval==INTERVAL_MONTH)
        {
         //--- iterate over each month
         for(uint32_t i=0;i<m_months_total;i++)
           {
            uint32_t    deals;
            DOUBLE  volume;
            //--- skip totally empty months
            if(!GetAllDealsByMonth(group_type,i,deals,volume))
              {
               header_series->Release();
               return(MT_RET_ERROR);
              }
            //--- skip none
            if(deals==0)
               continue;
            //--- get deals for exact reason, group and month index
            if(!GetDealsByMonth(group_type,r,i,deals,volume))
              {
               values_series->Release();
               return(MT_RET_ERROR);
              }
            //--- special rules for stops
            if(r==IMTDeal::DEAL_REASON_SL)
              {
               //--- compute total or monthly values
               uint32_t d;
               DOUBLE v;
               if(!GetDealsByMonth(group_type,IMTDeal::DEAL_REASON_TP,i,d,v)) // monthly
                 {
                  res=MT_RET_ERROR;
                  return(res);
                 }
               //--- count it
               deals +=d;
               volume+=v;
               //--- compute total or monthly values
               if(!GetDealsByMonth(group_type,IMTDeal::DEAL_REASON_SO,i,d,v)) // monthly
                 {
                  res=MT_RET_ERROR;
                  return(res);
                 }
               //--- count it
               deals +=d;
               volume+=v;
              }
            //--- get all deals for specified group for exact month
            //--- add value to series
            if((res=(chart_type==CHART_DEALS_COUNT)?values_series->ValueAddInt(deals):values_series->ValueAddDouble(volume))!=MT_RET_OK)
              {
               values_series->Release();
               return(res);
              }
           }
        }
      else//if day or week interval
        {
         //--- iterate over each day
         for(day_index=0;day_index<m_days_total;day_index++)
           {
            uint32_t deals;
            DOUBLE   volume;
            // get deals count for exact day for all reasons
            if(!GetDealsByDay(group_type,day_index,deals,volume))
              {
               values_series->Release();
               return(MT_RET_ERROR);
              }
            //--- calculate deals count per reason
            deals_per_reason+=m_deals_count[group_type][r][day_index];
            //--- calculate deals volume per reason
            volume_per_reason=SMTMath::MoneyAdd(volume_per_reason,m_deals_volume[group_type][r][day_index],m_currency_digits);
            //--- if DEAL_REASON_SL, use next two reason types as the same
            if(r==IMTDeal::DEAL_REASON_SL)
              {
               //--- calculate deals count per reason
               deals_per_reason+=m_deals_count[group_type][IMTDeal::DEAL_REASON_TP][day_index];
               deals_per_reason+=m_deals_count[group_type][IMTDeal::DEAL_REASON_SO][day_index];
               //--- calculate deals volume per reason
               volume_per_reason=SMTMath::MoneyAdd(volume_per_reason,m_deals_volume[group_type][IMTDeal::DEAL_REASON_TP][day_index],m_currency_digits);
               volume_per_reason=SMTMath::MoneyAdd(volume_per_reason,m_deals_volume[group_type][IMTDeal::DEAL_REASON_SO][day_index],m_currency_digits);
              }
            //--- choose interval
            if(interval==INTERVAL_DAY)
              {
               //--- skip zeros
               if(deals==0)
                  continue;
               //--- add value to series
               if((res=(chart_type==CHART_DEALS_COUNT)?values_series->ValueAddInt(deals_per_reason):values_series->ValueAddDouble(volume_per_reason))!=MT_RET_OK)
                 {
                  values_series->Release();
                  return(res);
                 }
               //--- reset
               deals_per_reason  =0;
               volume_per_reason =0.0;
              }
            else
               if(interval==INTERVAL_WEEK)
                 {
                  //--- remember for a week
                  deals_a_week+=deals;
                  //--- each seventh day
                  if((day_index+1)%DAYS_IN_WEEK==0)
                    {
                     //--- skip none
                     if(deals_a_week==0)
                        continue;
                     //--- add value to series
                     if((res=(chart_type==CHART_DEALS_COUNT)?values_series->ValueAddInt(deals_per_reason):values_series->ValueAddDouble(volume_per_reason))!=MT_RET_OK)
                       {
                        values_series->Release();
                        return(res);
                       }
                     //--- reset values
                     deals_a_week      =0;
                     deals_per_reason  =0;
                     volume_per_reason =0.0;
                    }
                 }
           }//for each day
         //--- add last week
         if(interval==INTERVAL_WEEK)
           {
            //---  add last element
            if(day_index%DAYS_IN_WEEK!=0&&deals_a_week!=0)
              {
               //--- add value
               if((res=(chart_type==CHART_DEALS_COUNT)?values_series->ValueAddInt(deals_per_reason):values_series->ValueAddDouble(volume_per_reason))!=MT_RET_OK)
                 {
                  values_series->Release();
                  return(res);
                 }
               //--- reset
               deals_a_week      =0;
               deals_per_reason  =0;
               volume_per_reason =0.0;
              }
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=chart->SeriesAdd(values_series))!=MT_RET_OK)
        {
         values_series->Release();
         return(res);
        }
      //--- skip deal reasons that were used before
      if(r==IMTDeal::DEAL_REASON_SL)
         r+=2;
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add group records to groups, totals for groups and total         |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::PrepareGroupAdd(const GroupRecord &group)
  {
   GroupRecord *ptr=NULL;
//--- add to current type list
   if(!m_groups.Add(&group))
      return(MT_RET_ERR_MEM);
//--- search group type totals
   if((ptr=m_groups_totals.Search(&group,SortGroupByCurrency))!=NULL)
     {
      //--- update digits
      ptr->currency_digits=std::min(ptr->currency_digits,group.currency_digits);
      //--- compute values
      ptr->accounts +=group.accounts;
      ptr->active   +=group.active;
      ptr->balance   =SMTMath::MoneyAdd(ptr->balance,group.balance,ptr->currency_digits);
      ptr->equity    =SMTMath::MoneyAdd(ptr->equity,group.equity,ptr->currency_digits);
      ptr->profit    =SMTMath::MoneyAdd(ptr->profit,group.profit,ptr->currency_digits);
     }
   else
     {
      //--- insert new total
      if(m_groups_totals.Insert(&group,SortGroupByCurrency)==NULL)
         return(MT_RET_ERR_MEM);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write common header's tags                                       |
//+------------------------------------------------------------------+
bool CExecutionType::WriteHeader(const MTAPISTR &tag,MTAPIRES &res)
  {
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERROR;
      return(true);
     }
//--- report name
   if(CMTStr::CompareNoCase(tag,L"report_name")==0)
     {
      if((res=m_api->HtmlWriteSafe(s_info.name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- server name
   if(CMTStr::CompareNoCase(tag,L"server")==0)
     {
      MTReportServerInfo info={};
      //--- get info and write server name
      if(m_api->About(info)==MT_RET_OK)
         res=m_api->HtmlWriteSafe(info.platform_name,IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- from
   if(CMTStr::CompareNoCase(tag,L"from")==0)
     {
      CMTStr256 str;
      res=m_api->HtmlWriteString(SMTFormat::FormatDateTime(str,m_api->ParamFrom(),false,false));
      return(true);
     }
//--- to
   if(CMTStr::CompareNoCase(tag,L"to")==0)
     {
      CMTStr256 str;
      res=m_api->HtmlWriteString(SMTFormat::FormatDateTime(str,m_api->ParamTo(),false,false));
      return(true);
     }
//--- go further
   return(false);
  }
//+------------------------------------------------------------------+
//| Write group type and increment counter                           |
//+------------------------------------------------------------------+
bool CExecutionType::WriteGroupType(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,uint32_t &group_counter,CMTStr256 &current_group_name,uint32_t &current_deals_count,DOUBLE &current_deals_volume)
  {
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERROR;
      return(true);
     }
//--- group types
//--- main groups iterator through our group 'GROUP_COUNT' times
   if(CMTStr::CompareNoCase(tag,L"group_types")==0)
     {
      //--- iterate group type
      if(counter>=GROUP_COUNT)
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- save current group counter value
      group_counter=counter;
      //--- get total deals count and volume
      if(!GetDealsTotal(group_counter,current_deals_count,current_deals_volume))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      //--- get type title
      if(!GetGroupTypeName(group_counter,current_group_name))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      //--- prepare groups for current type
      if((res=PrepareTableGroupType(group_counter))!=MT_RET_OK)
         return(true);
      //--- process tags
      if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- ok
      return(true);
     }
//--- go further
   return(false);
  }
//+------------------------------------------------------------------+
//| Write groups report                                              |
//+------------------------------------------------------------------+
bool CExecutionType::WriteGroupsReport(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,GroupRecord &current_subgroup,uint32_t &current_subgroup_index)
  {
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERROR;
      return(true);
     }
//--- groups
   if(CMTStr::CompareNoCase(tag,L"groups")==0)
     {
      //--- iterate groups in group type
      if(counter>=m_groups.Total())
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- get current record
      current_subgroup=m_groups[counter];
      //--- save current record index
      current_subgroup_index=counter;
      //--- process tags
      if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- groups totals
   if(CMTStr::CompareNoCase(tag,L"groups_totals")==0)
     {
      //--- iterate group totals for group type
      if(counter<m_groups_totals.Total())
        {
         //--- get current record
         current_subgroup=m_groups_totals[counter];
         //--- save current record index
         current_subgroup_index=counter;
         //--- process tags
         if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
            return(true);
        }
      return(true);
     }
//--- separator
   if(CMTStr::CompareNoCase(tag,L"sep")==0)
     {
      if(current_subgroup_index!=0)
         return(true);
      //--- process tags
      if((res=m_api->HtmlWriteSafe(L"sep",IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- group name
   if(CMTStr::CompareNoCase(tag,L"group")==0)
     {
      if((res=m_api->HtmlWriteSafe(current_subgroup.group,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- accounts count
   if(CMTStr::CompareNoCase(tag,L"accounts")==0)
     {
      if((res=m_api->HtmlWrite(L"%u",current_subgroup.accounts))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- active percentage
   if(CMTStr::CompareNoCase(tag,L"active")==0)
     {
      //--- if zero count, write zero, that's it
      if(current_subgroup.accounts==0)
        {
         //--- write
         if((res=m_api->HtmlWrite(L"0"))!=MT_RET_OK)
            return(true);
         return(true);
        }
      CMTStr256 str;
      //--- else compute percentage
      if(str.Format(L"%.0lf",(current_subgroup.active*100.0)/current_subgroup.accounts)<0)
        {
         res=MT_RET_ERROR;
         return(true);
        }
      //--- write
      if((res=m_api->HtmlWrite(str.Str()))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- balance
   if(CMTStr::CompareNoCase(tag,L"balance")==0)
     {
      CMTStr256 str;
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,current_subgroup.balance,current_subgroup.currency_digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- equity
   if(CMTStr::CompareNoCase(tag,L"equity")==0)
     {
      CMTStr256 str;
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,current_subgroup.equity,current_subgroup.currency_digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- profit style
   if(CMTStr::CompareNoCase(tag,L"profit_style")==0)
     {
      //--- if profit<0 make it red
      if(current_subgroup.profit<0)
         if((res=m_api->HtmlWriteString(L"style=\"color:red;\""))!=MT_RET_OK)
            return(true);
      return(true);
     }
//--- profit
   if(CMTStr::CompareNoCase(tag,L"profit")==0)
     {
      CMTStr256 str;
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,current_subgroup.profit,current_subgroup.currency_digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- currency
   if(CMTStr::CompareNoCase(tag,L"currency")==0)
     {
      if((res=m_api->HtmlWriteSafe(current_subgroup.currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- go further
   return(false);
  }
//+------------------------------------------------------------------+
//| Write table with deals                                           |
//+------------------------------------------------------------------+
bool CExecutionType::WriteDealsTable(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,const uint32_t current_group,const uint32_t current_deals_count,uint32_t &current_month_index)
  {
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERROR;
      return(true);
     }
//--- deals
   if(CMTStr::CompareNoCase(tag,L"deals_by_months")==0)
     {
      //--- stop after a number of iterations
      if(counter>=m_months_total)
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- remember counter
      current_month_index=counter;
      //--- proceed template
      if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- line position
   if(CMTStr::CompareNoCase(tag,L"deals_line")==0)
     {
      res=m_api->HtmlWrite(L"%u",current_month_index&1);
      return(true);
     }
//--- deals separator
   if(CMTStr::CompareNoCase(tag,L"dsep")==0)
     {
      //--- process tags and use exist value
      if((res=m_api->HtmlWriteSafe(L"sep",IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- deals date
   if(CMTStr::CompareNoCase(tag,L"deals_date")==0)
     {
      //--- convert month index to human readable string
      CMTStr256 str;
      if(!MonthIndexToStr(current_month_index,str))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      //--- write
      if((res=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- proceed different types of reason for exact month
   if(PreceedDealsCountTag(tag,L"deals_by_client",current_group,IMTDeal::DEAL_REASON_CLIENT,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_mobile",current_group,IMTDeal::DEAL_REASON_MOBILE,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_web",current_group,IMTDeal::DEAL_REASON_WEB,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_expert",current_group,IMTDeal::DEAL_REASON_EXPERT,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_dealer",current_group,IMTDeal::DEAL_REASON_DEALER,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_rollover",current_group,IMTDeal::DEAL_REASON_ROLLOVER,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_external",current_group,IMTDeal::DEAL_REASON_EXTERNAL_CLIENT,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_vmargin",current_group,IMTDeal::DEAL_REASON_VMARGIN,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_gateway",current_group,IMTDeal::DEAL_REASON_GATEWAY,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_signal",current_group,IMTDeal::DEAL_REASON_SIGNAL,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_settlement",current_group,IMTDeal::DEAL_REASON_SETTLEMENT,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_transfer",current_group,IMTDeal::DEAL_REASON_TRANSFER,current_month_index,false,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_by_stop",current_group,IMTDeal::DEAL_REASON_SL,current_month_index,false,res))
      return(true);
//--- proceed totals
   if(PreceedDealsCountTag(tag,L"deals_total_client",current_group,IMTDeal::DEAL_REASON_CLIENT,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_mobile",current_group,IMTDeal::DEAL_REASON_MOBILE,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_web",current_group,IMTDeal::DEAL_REASON_WEB,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_expert",current_group,IMTDeal::DEAL_REASON_EXPERT,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_dealer",current_group,IMTDeal::DEAL_REASON_DEALER,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_rollover",current_group,IMTDeal::DEAL_REASON_ROLLOVER,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_external",current_group,IMTDeal::DEAL_REASON_EXTERNAL_CLIENT,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_vmargin",current_group,IMTDeal::DEAL_REASON_VMARGIN,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_gateway",current_group,IMTDeal::DEAL_REASON_GATEWAY,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_signal",current_group,IMTDeal::DEAL_REASON_SIGNAL,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_settlement",current_group,IMTDeal::DEAL_REASON_SETTLEMENT,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_transfer",current_group,IMTDeal::DEAL_REASON_TRANSFER,0,true,res))
      return(true);
   if(PreceedDealsCountTag(tag,L"deals_total_stop",current_group,IMTDeal::DEAL_REASON_SL,0,true,res))
      return(true);
//--- go further
   return(false);
  }
//+------------------------------------------------------------------+
//| Proceed deals count tag                                          |
//+------------------------------------------------------------------+
bool CExecutionType::PreceedDealsCountTag(const MTAPISTR &tag,LPCWSTR match,const uint32_t group_type,const uint32_t reason_type,const uint32_t month_index,bool compute_total,MTAPIRES &res)
  {
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERROR;
      return(true);
     }
//--- check for tag
   if(CMTStr::CompareNoCase(tag,match)==0)
     {
      //--- check
      if(group_type>=GROUP_COUNT||month_index>=m_months_total)
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- variables
      uint32_t deals     =0;
      uint32_t d         =0;
      DOUBLE volume  =0.0;
      DOUBLE v       =0.0;
      //--- what to compute, total or monthly values
      if(compute_total?
        !GetDealsTotalByType(group_type,reason_type,d,v) :          // total
        !GetDealsByMonth(group_type,reason_type,month_index,d,v))   // monthly
        {
         res=MT_RET_ERROR;
         return(true);
        }
      //--- count it
      deals +=d;
      volume+=v;
      //--- special rules for stops
      if(reason_type==IMTDeal::DEAL_REASON_SL)
        {
         //--- compute total or monthly values
         if(compute_total?
           !GetDealsTotalByType(group_type,IMTDeal::DEAL_REASON_TP,d,v):         // total
           !GetDealsByMonth(group_type,IMTDeal::DEAL_REASON_TP,month_index,d,v)) // monthly
           {
            res=MT_RET_ERROR;
            return(true);
           }
         //--- count it
         deals +=d;
         volume+=v;
         //--- compute total or monthly values
         if(compute_total?
           !GetDealsTotalByType(group_type,IMTDeal::DEAL_REASON_SO,d,v):         // total
           !GetDealsByMonth(group_type,IMTDeal::DEAL_REASON_SO,month_index,d,v)) // monthly
           {
            res=MT_RET_ERROR;
            return(true);
           }
         //--- count it
         deals +=d;
         volume+=v;
        }
      //--- buffers
      CMTStr128 dt,str;
      //--- format 
      dt.Append(SMTFormat::FormatMoney(str,(double)deals,0));
      dt.Append(L"\n");
      dt.Append(SMTFormat::FormatMoney(str,volume,m_currency_digits));
      //--- write
      if((res=m_api->HtmlWriteSafe(dt.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- go further
   return(false);
  }
//+------------------------------------------------------------------+
//| convert day index to human readable string                       |
//+------------------------------------------------------------------+
bool CExecutionType::DayIndexToStr(const uint32_t i,CMTStr256 &str)const
  {
//--- parse time
   tm tm_day={0};
   if(!SMTTime::ParseTime(m_api->ParamFrom()+i*SECONDS_IN_DAY,&tm_day))
      return(false);
//--- convert time to human readable format
   if(str.Format(L"%d/%d/%d",tm_day.tm_mday,tm_day.tm_mon+1,tm_day.tm_year+1900)<0)
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Convert ctm to day index                                         |
//+------------------------------------------------------------------+
bool CExecutionType::TimeToDayIndex(const int64_t ctm,int64_t &index)const
  {
//--- checks
   if(ctm<m_param_from)
      return(false);
//--- compute
   index=ctm/SECONDS_IN_DAY-m_param_from/SECONDS_IN_DAY;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Month Index to String Time                                       |
//+------------------------------------------------------------------+
bool CExecutionType::MonthIndexToStr(const uint32_t index,CMTStr &str)const
  {
//--- clear result
   str.Clear();
//--- check
   if(index>=m_months_total)
      return(true);
//--- parse time
   tm ttm={};
   if(!SMTTime::ParseTime(m_param_from,&ttm))
      return(false);
//--- current month
   const uint32_t month_curr=ttm.tm_year*12+ttm.tm_mon;
//--- format time
   if(str.Format(L"%02u.%04u",(month_curr+index)%12+1,(month_curr+index)/12+1900)<0)
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Convert month index to human readable month and year number      |
//+------------------------------------------------------------------+
bool CExecutionType::MonthIndexToMonthYear(const uint32_t index,uint32_t &month,uint32_t &year)const
  {
//--- reset
   month =0;
   year  =0;
//--- check
   if(index>=m_months_total)
      return(true);
//--- parse time
   tm ttm={};
   if(!SMTTime::ParseTime(m_param_from,&ttm))
      return(false);
//--- current month
   const uint32_t month_curr=ttm.tm_year*12+ttm.tm_mon;
//--- compute
   month =(month_curr+index)%12+1;
   year  =(month_curr+index)/12+1900;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get group type by group name                                     |
//+------------------------------------------------------------------+
bool CExecutionType::GetGroupType(LPCWSTR group,uint32_t &type)const
  {
//--- check for empty string
   if(group==NULL)
      return(false);
//--- coverage group
   if(CMTStr::Find(group,L"coverage")>=0)
     {
      type=GROUP_COVERAGE;
      return(true);
     }
//--- demo group
   if(CMTStr::Find(group,L"demo")>=0)
     {
      type=GROUP_DEMO;
      return(true);
     }
//--- contest group
   if(CMTStr::Find(group,L"contest")>=0)
     {
      type=GROUP_CONTEST;
      return(true);
     }
//--- preliminary group
   if(CMTStr::Find(group,L"preliminary")>=0)
     {
      type=GROUP_PRELIMINARY;
      return(true);
     }
//--- skip all managers as well
   if(CMTStr::Find(group,L"manager")>=0)
      return(false);
//--- by default: real
   type=GROUP_REAL;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get group type name                                              |
//+------------------------------------------------------------------+
bool CExecutionType::GetGroupTypeName(const uint32_t type,CMTStr &name)const
  {
//--- clear result
   name.Clear();
//--- check type
   switch(type)
     {
      case GROUP_REAL:
         name.Assign(L"Real");
         return(true);
      case GROUP_PRELIMINARY:
         name.Assign(L"Preliminary");
         return(true);
      case GROUP_DEMO:
         name.Assign(L"Demo");
         return(true);
      case GROUP_CONTEST:
         name.Assign(L"Contest");
         return(true);
      case GROUP_COVERAGE:
         name.Assign(L"Coverage");
         return(true);
      default:
         return(false);
     }
  }
//+------------------------------------------------------------------+
//| Get reason type as string                                        |
//+------------------------------------------------------------------+
bool CExecutionType::GetReasonTypeName(const uint32_t reason,CMTStr &str)const
  {
   str.Clear();
//--- switch deal reason
   switch(reason)
     {
      case IMTDeal::DEAL_REASON_CLIENT:
         str.Assign(L"Client");
         return(true);
      case IMTDeal::DEAL_REASON_EXPERT:
         str.Assign(L"Expert");
         return(true);
      case IMTDeal::DEAL_REASON_DEALER:
         str.Assign(L"Dealer");
         return(true);
      case IMTDeal::DEAL_REASON_SL:
         str.Assign(L"S/L, T/P and Stop-Out");
         return(true);
      case IMTDeal::DEAL_REASON_ROLLOVER:
         str.Assign(L"Rollover");
         return(true);
      case IMTDeal::DEAL_REASON_EXTERNAL_CLIENT:
         str.Assign(L"External system");
         return(true);
      case IMTDeal::DEAL_REASON_VMARGIN:
         str.Assign(L"Variation margin");
         return(true);
      case IMTDeal::DEAL_REASON_GATEWAY:
         str.Assign(L"Gateway");
         return(true);
      case IMTDeal::DEAL_REASON_SIGNAL:
         str.Assign(L"Signal");
         return(true);
      case IMTDeal::DEAL_REASON_SETTLEMENT:
         str.Assign(L"Settlement");
         return(true);
      case IMTDeal::DEAL_REASON_TRANSFER:
         str.Assign(L"Transfer");
         return(true);
      case IMTDeal::DEAL_REASON_SYNC:
         str.Assign(L"Synchronization");
         return(true);
      case IMTDeal::DEAL_REASON_EXTERNAL_SERVICE:
         str.Assign(L"Service in external system");
         return(true);
      case IMTDeal::DEAL_REASON_MIGRATION:
         str.Assign(L"Migration");
         return(true);
      case IMTDeal::DEAL_REASON_MOBILE:
         str.Assign(L"Mobile");
         return(true);
      case IMTDeal::DEAL_REASON_WEB:
         str.Assign(L"Web");
         return(true);
      case IMTDeal::DEAL_REASON_SPLIT:
         str.Assign(L"Split");
         return(true);
      default:
         return(false);
     }
  }
//+------------------------------------------------------------------+
//| Get deals count and volume by day                                |
//+------------------------------------------------------------------+
bool CExecutionType::GetDealsByDay(const uint32_t group_index,const uint32_t day_index,uint32_t &deals,DOUBLE &volume)const
  {
//--- reset
   deals =0;
   volume=0.0;
//--- checks
   if(group_index>=GROUP_COUNT||day_index>=m_days_total)
      return(false);
//--- compute for all reasons
   for(uint32_t r=IMTDeal::DEAL_REASON_FIRST;r<=IMTDeal::DEAL_REASON_LAST;r++)
     {
      //--- compute deals count
      deals+=m_deals_count[group_index][r][day_index];
      //--- compute deals volume
      volume=SMTMath::MoneyAdd(volume,m_deals_volume[group_index][r][day_index],m_currency_digits);
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get deals total count and volume                                 |
//+------------------------------------------------------------------+
bool CExecutionType::GetDealsTotal(const uint32_t group_index,uint32_t &deals,DOUBLE &volume)const
  {
//--- reset
   deals =0;
   volume=0.0;
//--- check
   if(group_index>=GROUP_COUNT)
      return(false);
//--- compute for all reasons
   for(uint32_t r=IMTDeal::DEAL_REASON_FIRST;r<=IMTDeal::DEAL_REASON_LAST;r++)
     {
      for(uint32_t d=0;d<m_days_total;d++)
        {
         //--- compute deals count
         deals+=m_deals_count[group_index][r][d];
         //--- compute deals volume
         volume=SMTMath::MoneyAdd(volume,m_deals_volume[group_index][r][d],m_currency_digits);
        }
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get deals total count and volume                                 |
//+------------------------------------------------------------------+
bool CExecutionType::GetDealsTotalByType(const uint32_t group_index,const uint32_t reason_type,uint32_t &deals,DOUBLE &volume)const
  {
//--- reset
   deals =0;
   volume=0.0;
//--- checks
   if(group_index>=GROUP_COUNT||reason_type>=REASONS_COUNT)
      return(false);
//--- iterate over each day
   for(uint32_t d=0;d<m_days_total;d++)
     {
      //--- compute deals count
      deals+=m_deals_count[group_index][reason_type][d];
      //--- compute deals volume
      volume=SMTMath::MoneyAdd(volume,m_deals_volume[group_index][reason_type][d],m_currency_digits);
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| get deals count and volume at specified month index              |
//+------------------------------------------------------------------+
bool CExecutionType::GetDealsByMonth(const uint32_t group_type,const uint32_t reason_type,const uint32_t month_index,uint32_t &deals,DOUBLE &volume)const
  {
//--- variables
   uint32_t month  =0;
   uint32_t year   =0;
//--- reset
   deals       =0;
   volume      =0.0;
//--- checks
   if(group_type>=GROUP_COUNT||reason_type>=REASONS_COUNT)
      return(false);
//--- convert month index to human readable month and year number
   if(!MonthIndexToMonthYear(month_index,month,year))
      return(false);
//--- iterate over each day
   for(uint32_t d=0;d<m_days_total;d++)
     {
      tm itm={0};
      //--- parse time
      if(!SMTTime::ParseTime(m_param_from+d*SECONDS_IN_DAY,&itm))
         return(false);
      //--- if 'our' month
      if((itm.tm_mon+1)==month&&(itm.tm_year+1900)==year)
        {
         //--- compute deals count
         deals+=m_deals_count[group_type][reason_type][d];
         //--- compute deals volume
         volume=SMTMath::MoneyAdd(volume,m_deals_volume[group_type][reason_type][d],m_currency_digits);
        }
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| get deals count and volume at specified month index              |
//+------------------------------------------------------------------+
bool CExecutionType::GetAllDealsByMonth(const uint32_t group_type,const uint32_t month_index,uint32_t &deals,DOUBLE &volume)const
  {
//--- variables
   uint32_t month  =0;
   uint32_t year   =0;
//--- reset
   deals       =0;
   volume      =0.0;
//--- check
   if(group_type>=GROUP_COUNT)
      return(false);
//--- convert month index to human readable month and year number
   if(!MonthIndexToMonthYear(month_index,month,year))
      return(false);
//--- compute for all reasons
   for(uint32_t r=IMTDeal::DEAL_REASON_FIRST;r<=IMTDeal::DEAL_REASON_LAST;r++)
     {
      //--- iterate over each day
      for(uint32_t d=0;d<m_days_total;d++)
        {
         tm itm={0};
         //--- parse time
         if(!SMTTime::ParseTime(m_param_from+d*SECONDS_IN_DAY,&itm))
            return(false);
         //--- if our month
         if((itm.tm_mon+1)==month&&(itm.tm_year+1900)==year)
           {
            //--- compute deals count
            deals+=m_deals_count[group_type][r][d];
            //--- compute deals volume
            volume=SMTMath::MoneyAdd(volume,m_deals_volume[group_type][r][d],m_currency_digits);
           }
        }
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Generate dashboard report                                        |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::GenerateDashboard(const uint32_t interval)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- group types cycle
   for(uint32_t i=0;i<GROUP_COUNT;i++)
     {
      //--- get total deals count and volume
      uint32_t count=0;
      double volume=0.0;
      if(!GetDealsTotal(i,count,volume))
         return(MT_RET_ERROR);
      //--- check deals count
      if(count==0)
         continue;
      //--- group type name
      CMTStr128 name;
      if(!GetGroupTypeName(i,name))
         continue;
      name.Append(L": ");
      //--- generate dashboard table groups
      MTAPIRES res=AddTableGroups(i,name);
      if(res!=MT_RET_OK)
         return(res);
      //--- prepare deals data
      IMTDataset *data=DealsData(res,i,interval,s_reasons_chart,_countof(s_reasons_chart),false);
      if(!data)
         return(res);
      //--- prepare deal count graph
      if((res=PrepareDealGraph(name,L"Deals Count Report",2,data))!=MT_RET_OK)
         return(res);
      //--- prepare deal volume graph
      if((res=PrepareDealGraph(name,L"Deals Volume Report",3,data))!=MT_RET_OK)
         return(res);
      //--- prepare deals monthly data
      data=DealsData(res,i,INTERVAL_MONTH,s_reasons_table,_countof(s_reasons_table),true);
      if(!data)
         return(res);
      //--- deals table title
      name.Append(L"Deals Report (monthly)");
      //--- add deals table
      if(!ReportColumn::TableAdd(*m_api,res,data,name.Str()))
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Generate dashboard table groups                                  |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::AddTableGroups(uint32_t group_type,const CMTStr &group_type_name)
  {
//--- checks
   if(m_api==NULL || group_type>=GROUP_COUNT)
      return(MT_RET_ERR_PARAMS);
//--- prepare groups for current type
   MTAPIRES res=PrepareTableGroupType(group_type);
   if(res!=MT_RET_OK)
      return(res);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   if((res=ReportColumn::ReportColumnsAdd(*data,s_columns_group,_countof(s_columns_group)))!=MT_RET_OK)
      return(res);
//--- for all groups
   for(uint32_t i=0,total=m_groups.Total();i<total;i++)
     {
      const GroupRecord &info=m_groups[i];
      GroupStatRecord    row={};
      //--- fill record
      CMTStr::Copy(row.group,info.group);
      row.accounts=info.accounts;
      row.active=info.active*100.0/info.accounts;
      row.balance=info.balance;
      row.profit=info.profit;
      row.equity=info.equity;
      CMTStr::Copy(row.currency,info.currency);
      row.currency_digits=info.currency_digits;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- summaries count
   const uint32_t total=m_groups_totals.Total();
   if(total)
     {
      //--- create summary
      IMTDatasetSummary *summary=data->SummaryCreate();
      if(!summary)
         return(MT_RET_ERR_MEM);
      //--- for all summaries
      for(uint32_t i=0;i<total;i++)
        {
         //--- group summary info
         const GroupRecord &info=m_groups_totals[i];
         //--- add summary columns
         if((res=DataSummaryAddUInt(*data,summary,2,i,info.accounts))!=MT_RET_OK)
            break;
         if((res=DataSummaryAddDouble(*data,summary,3,i,info.active*100.0/info.accounts))!=MT_RET_OK)
            break;
         if((res=DataSummaryAddMoney(*data,summary,4,i,info.balance,info.currency_digits))!=MT_RET_OK)
            break;
         if((res=DataSummaryAddMoney(*data,summary,5,i,info.profit,info.currency_digits))!=MT_RET_OK)
            break;
         if((res=DataSummaryAddMoney(*data,summary,6,i,info.equity,info.currency_digits))!=MT_RET_OK)
            break;
         if((res=DataSummaryAddString(*data,summary,7,i,info.currency))!=MT_RET_OK)
            break;
        }
      //--- release summary
      summary->Release();
      //--- check error code
      if(res!=MT_RET_OK)
         return(res);
     }
//--- table title
   CMTStr128 title(group_type_name);
   title.Append(L"Accounts report");
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data,title.Str()) ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard deals data                                    |
//+------------------------------------------------------------------+
IMTDataset* CExecutionType::DealsData(MTAPIRES &res,const uint32_t group_type,const uint32_t interval,const uint32_t *reasons,const uint32_t reasons_total,const bool column_name) const
  {
//--- checks
   if(m_api==NULL || group_type>=GROUP_COUNT || interval>=INTERVAL_COUNT || reasons==NULL || reasons_total==0)
     {
      res=MT_RET_ERR_PARAMS;
      return(nullptr);
     }
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
     {
      res=MT_RET_ERR_MEM;
      return(nullptr);
     }
//--- add dataset columns
   if((res=ReportColumn::ReportColumnsAdd(*data,s_columns_deals,_countof(s_columns_deals)))!=MT_RET_OK)
      return(nullptr);
//--- columns template
   ReportColumn columns[_countof(s_columns_deal)];
   memcpy(&columns,s_columns_deal,sizeof(columns));
   CMTStr64 names[_countof(columns)];
   for(uint32_t j=0;j<_countof(columns);j++)
     {
      columns[j].name=names[j].Str();
      columns[j].offset+=offsetof(DealRecord,deals);
     }
//--- for all reasons
   DealInfo deals_total[REASONS_COUNT]={};
   for(uint32_t i=0;i<reasons_total;i++)
     {
      const uint32_t r=reasons[i];
      //--- get human readable reason's type name
      CMTStr64 str;
      GetReasonTypeName(r,str);
      //--- make columns name
      for(uint32_t j=0;j<_countof(columns);j++)
        {
         names[j].Assign(str);
         if(column_name)
            names[j].Append(s_columns_deal[j].name);
        }
      //--- get deals for exact reason and group
      DealInfo &deal_total=deals_total[r];
      if(!GetDealsTotalByType(group_type,r,deal_total.count,deal_total.volume))
        {
         res=MT_RET_ERROR;
         break;
        }
      //--- if DEAL_REASON_SL, use next two reason types as the same
      if(r==IMTDeal::DEAL_REASON_SL)
        {
         //--- get deals
         DealInfo deal={0};
         if(!GetDealsTotalByType(group_type,IMTDeal::DEAL_REASON_TP,deal.count,deal.volume))
           {
            res=MT_RET_ERROR;
            break;
           }
         //--- calculate deals count per reason
         deal_total.count+=deal.count;
         deal_total.volume=SMTMath::MoneyAdd(deal_total.volume,deal.volume,m_currency_digits);
         //--- get deals
         if(!GetDealsTotalByType(group_type,IMTDeal::DEAL_REASON_SO,deal.count,deal.volume))
           {
            res=MT_RET_ERROR;
            break;
           }
         //--- calculate deals count per reason
         deal_total.count+=deal.count;
         deal_total.volume=SMTMath::MoneyAdd(deal_total.volume,deal.volume,m_currency_digits);
        }
      //--- update hidden flag
      for(uint32_t j=0;j<_countof(columns);j++)
         if(deal_total.count)
            columns[j].flags&=~IMTDatasetColumn::FLAG_HIDDEN;
         else
            columns[j].flags|=IMTDatasetColumn::FLAG_HIDDEN;
      //--- add dataset columns
      if((res=ReportColumn::ReportColumnsAdd(*data,columns,_countof(columns)))!=MT_RET_OK)
         break;
      //--- next deal
      for(uint32_t j=0;j<_countof(columns);j++)
        {
         columns[j].id+=2;
         columns[j].offset+=sizeof(DealInfo);
        }
     }
//--- fill deals data based on interval
   switch(interval)
     {
      case INTERVAL_DAY:
         if((res=FillDealDataDay(*data,group_type,reasons,reasons_total))!=MT_RET_OK)
            return(nullptr);
         break;
      case INTERVAL_WEEK:
         if((res=FillDealDataWeek(*data,group_type,reasons,reasons_total))!=MT_RET_OK)
            return(nullptr);
         break;
      case INTERVAL_MONTH:
         if((res=FillDealDataMonth(*data,group_type,reasons,reasons_total))!=MT_RET_OK)
            return(nullptr);
         break;
     }
//--- create summary
   IMTDatasetSummary *summary=data->SummaryCreate();
   if(!summary)
      return(nullptr);
//--- for all reasons
   for(uint32_t i=0;i<reasons_total;i++)
     {
      const uint32_t r=reasons[i];
      //--- add summary columns
      if((res=DataSummaryAddUInt(*data,summary,i*2+2,0,deals_total[r].count))!=MT_RET_OK)
         break;
      if((res=DataSummaryAddMoney(*data,summary,i*2+3,0,deals_total[r].volume,m_currency_digits))!=MT_RET_OK)
         break;
     }
//--- release summary
   summary->Release();
//--- check error code and return dataset
   return(res==MT_RET_OK ? data : nullptr);
  }
//+------------------------------------------------------------------+
//| Fill Deals data Day By Day                                       |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::FillDealDataDay(IMTDataset &data,const uint32_t group_type,const uint32_t *reasons,const uint32_t reasons_total) const
  {
//--- check
   if(m_api==NULL || reasons==NULL || reasons_total==0)
      return(MT_RET_ERR_PARAMS);
//--- init record
   DealRecord row={0};
//--- iterate over each day
   for(uint32_t i=0;i<m_days_total;i++)
     {
      //--- get all deals count and volume for exact group and day
      DealInfo deal={0};
      if(!GetDealsByDay(group_type,i,deal.count,deal.volume))
         return(MT_RET_ERROR);
      //--- skip empty day
      if(!deal.count)
         continue;
      //--- init date
      row.date=m_api->ParamFrom()+i*SECONDS_IN_DAY;
      //--- iterate over reason types, get deals for exact reason, group and day index
      for(uint32_t j=0;j<reasons_total;j++)
        {
         const uint32_t r=reasons[j];
         //--- get values
         row.deals[j].count=m_deals_count[group_type][r][i];
         row.deals[j].volume=m_deals_volume[group_type][r][i];
         //--- if DEAL_REASON_SL, use next two reason types as the same
         if(r==IMTDeal::DEAL_REASON_SL)
           {
            //--- calculate deals count per reason
            row.deals[j].count+=m_deals_count[group_type][IMTDeal::DEAL_REASON_TP][i];
            row.deals[j].count+=m_deals_count[group_type][IMTDeal::DEAL_REASON_SO][i];
            //--- calculate deals volume per reason
            row.deals[j].volume=SMTMath::MoneyAdd(row.deals[j].volume,m_deals_volume[group_type][IMTDeal::DEAL_REASON_TP][i],m_currency_digits);
            row.deals[j].volume=SMTMath::MoneyAdd(row.deals[j].volume,m_deals_volume[group_type][IMTDeal::DEAL_REASON_SO][i],m_currency_digits);
           }
        }
      //--- write row
      MTAPIRES res=data.RowWrite(&row,offsetof(DealRecord,deals)+reasons_total*sizeof(DealInfo));
      if(res!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill Deals data Week By Week                                     |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::FillDealDataWeek(IMTDataset &data,const uint32_t group_type,const uint32_t *reasons,const uint32_t reasons_total) const
  {
//--- check
   if(m_api==NULL || reasons==NULL || reasons_total==0)
      return(MT_RET_ERR_PARAMS);
//--- iterate over each day
   DealRecord row={0};
   DealInfo deal_week={0};
   uint32_t i=0;
   for(i=0;i<m_days_total;i++)
     {
      //--- get all deals count and volume for exact group and day
      DealInfo deal={0};
      if(!GetDealsByDay(group_type,i,deal.count,deal.volume))
         return(MT_RET_ERROR);
      //--- add week deals count
      deal_week.count+=deal.count;
      //--- iterate over reason types, get deals for exact reason, group and day index
      for(uint32_t j=0;j<reasons_total;j++)
        {
         const uint32_t r=reasons[j];
         //--- accumulate values
         row.deals[j].count+=m_deals_count[group_type][r][i];
         row.deals[j].volume=SMTMath::MoneyAdd(row.deals[r].volume,m_deals_volume[group_type][r][i],m_currency_digits);
         //--- if DEAL_REASON_SL, use next two reason types as the same
         if(r==IMTDeal::DEAL_REASON_SL)
           {
            //--- calculate deals count per reason
            row.deals[j].count+=m_deals_count[group_type][IMTDeal::DEAL_REASON_TP][i];
            row.deals[j].count+=m_deals_count[group_type][IMTDeal::DEAL_REASON_SO][i];
            //--- calculate deals volume per reason
            row.deals[j].volume=SMTMath::MoneyAdd(row.deals[j].volume,m_deals_volume[group_type][IMTDeal::DEAL_REASON_TP][i],m_currency_digits);
            row.deals[j].volume=SMTMath::MoneyAdd(row.deals[j].volume,m_deals_volume[group_type][IMTDeal::DEAL_REASON_SO][i],m_currency_digits);
           }
        }
      //--- only complete weeks
      if((i+1)%DAYS_IN_WEEK==0)
        {
         //--- skip empty week
         if(deal_week.count)
           {
            //--- data date
            row.date=m_api->ParamFrom()+i*SECONDS_IN_DAY;
            //--- write row
            MTAPIRES res=data.RowWrite(&row,offsetof(DealRecord,deals)+reasons_total*sizeof(DealInfo));
            if(res!=MT_RET_OK)
               return(res);
            //--- reset week deals count
            deal_week.count=0;
           }
         //--- reset values
         ZeroMemory(&row.deals,offsetof(DealRecord,deals)+reasons_total*sizeof(DealInfo));
        }
     }
//---  add last element
   if(i%DAYS_IN_WEEK!=0 && deal_week.count)
     {
      //--- data date
      row.date=m_api->ParamFrom()+i*SECONDS_IN_DAY;
      //--- write row
      MTAPIRES res=data.RowWrite(&row,offsetof(DealRecord,deals)+reasons_total*sizeof(DealInfo));
      if(res!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill Deals data Month By Month                                   |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::FillDealDataMonth(IMTDataset &data,const uint32_t group_type,const uint32_t *reasons,const uint32_t reasons_total) const
  {
//--- check
   if(m_api==NULL || reasons==NULL || reasons_total==0)
      return(MT_RET_ERR_PARAMS);
//--- init record
   DealRecord row={0};
   row.date=SMTTime::MonthBegin(m_api->ParamFrom());
//--- iterate over each month
   for(uint32_t i=0;i<m_months_total;i++)
     {
      //--- skip totally empty months
      DealInfo deal_month={0};
      if(!GetAllDealsByMonth(group_type,i,deal_month.count,deal_month.volume))
         return(MT_RET_ERROR);
      //--- skip empty month
      if(deal_month.count)
        {
         //--- iterate over reason types
         for(uint32_t j=0;j<reasons_total;j++)
           {
            const uint32_t r=reasons[j];
            //--- get deals for exact reason, group and month index
            if(!GetDealsByMonth(group_type,r,i,row.deals[j].count,row.deals[j].volume))
               return(MT_RET_ERROR);
            //--- if DEAL_REASON_SL, use next two reason types as the same
            if(r==IMTDeal::DEAL_REASON_SL)
              {
               //--- get deals
               DealInfo deal={0};
               if(!GetDealsByMonth(group_type,IMTDeal::DEAL_REASON_TP,i,deal.count,deal.volume))
                  return(MT_RET_ERROR);
               //--- calculate deals count per reason
               row.deals[j].count+=deal.count;
               row.deals[j].volume=SMTMath::MoneyAdd(row.deals[j].volume,deal.volume,m_currency_digits);
               //--- get deals
               if(!GetDealsByMonth(group_type,IMTDeal::DEAL_REASON_SO,i,deal.count,deal.volume))
                  return(MT_RET_ERROR);
               //--- calculate deals count per reason
               row.deals[j].count+=deal.count;
               row.deals[j].volume=SMTMath::MoneyAdd(row.deals[j].volume,deal.volume,m_currency_digits);
              }
           }
         //--- write row
         MTAPIRES res=data.RowWrite(&row,offsetof(DealRecord,deals)+reasons_total*sizeof(DealInfo));
         if(res!=MT_RET_OK)
            return(res);
        }
      //--- next month
      row.date=SMTTime::MonthBegin(row.date+32*SECONDS_IN_DAY);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare Deal Graph                                               |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::PrepareDealGraph(const CMTStr &group_type_name,LPCWSTR title,uint32_t column_id,IMTDataset *data)
  {
//--- checks
   if(!title || !data || !m_api)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(data->RowTotal()>1 ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_AREA : IMTReportDashboardWidget::WIDGET_TYPE_CHART_BAR);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare chart
   if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
      return(res);
//--- set title
   CMTStr128 str(group_type_name);
   str.Append(title);
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add dataset columns
   for(uint32_t i=0;i<_countof(s_reasons_chart);i++)
      if((res=chart->DataColumnAdd(i*2+column_id))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare dashboard data summary                                   |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::PrepareDataSummary(IMTDatasetSummary &summary,uint32_t column_id,uint32_t line)
  {
//--- clear summary
   MTAPIRES res=summary.Clear();
   if(res!=MT_RET_OK)
      return(res);
//--- column id
   if((res=summary.ColumnID(column_id))!=MT_RET_OK)
      return(res);
//--- line
   return(summary.Line(line));
  }
//+------------------------------------------------------------------+
//| Add dashboard data summary uint                                  |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::DataSummaryAddUInt(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,uint64_t value)
  {
//--- checks
   if(!summary)
      return(MT_RET_ERR_PARAMS);
//--- prepare dashboard data summary
   MTAPIRES res=PrepareDataSummary(*summary,column_id,line);
   if(res!=MT_RET_OK)
      return(res);
//--- set value
   if((res=summary->ValueUInt(value))!=MT_RET_OK)
      return(res);
//--- add summary
   return(data.SummaryAdd(summary));
  }
//+------------------------------------------------------------------+
//| Add dashboard data summary double                                |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::DataSummaryAddDouble(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,double value)
  {
//--- checks
   if(!summary)
      return(MT_RET_ERR_PARAMS);
//--- prepare dashboard data summary
   MTAPIRES res=PrepareDataSummary(*summary,column_id,line);
   if(res!=MT_RET_OK)
      return(res);
//--- set value
   if((res=summary->ValueDouble(value))!=MT_RET_OK)
      return(res);
//--- add summary
   return(data.SummaryAdd(summary));
  }
//+------------------------------------------------------------------+
//| Add dashboard data summary money                                 |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::DataSummaryAddMoney(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,double money,BYTE currency_digits)
  {
//--- checks
   if(!summary)
      return(MT_RET_ERR_PARAMS);
//--- prepare dashboard data summary
   MTAPIRES res=PrepareDataSummary(*summary,column_id,line);
   if(res!=MT_RET_OK)
      return(res);
//--- set value
   if((res=summary->ValueMoney(money))!=MT_RET_OK)
      return(res);
//--- set digits
   if((res=summary->Digits(currency_digits))!=MT_RET_OK)
      return(res);
//--- add summary
   return(data.SummaryAdd(summary));
  }
//+------------------------------------------------------------------+
//| Add dashboard data summary string                                |
//+------------------------------------------------------------------+
MTAPIRES CExecutionType::DataSummaryAddString(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,LPCWSTR str)
  {
//--- checks
   if(!summary || !str)
      return(MT_RET_ERR_PARAMS);
//--- prepare dashboard data summary
   MTAPIRES res=PrepareDataSummary(*summary,column_id,line);
   if(res!=MT_RET_OK)
      return(res);
//--- set value
   if((res=summary->ValueString(str))!=MT_RET_OK)
      return(res);
//--- add summary
   return(data.SummaryAdd(summary));
  }
//+------------------------------------------------------------------+
//| Sort group by currency                                           |
//+------------------------------------------------------------------+
int32_t CExecutionType::SortGroupByCurrency(const void* const left,const void* const right)
  {
   GroupRecord *lft=(GroupRecord*)left;
   GroupRecord *rgh=(GroupRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
//| Sort group by name                                           |
//+------------------------------------------------------------------+
int32_t CExecutionType::SortGroupByName(const void* const left,const void* const right)
  {
   GroupRecord *lft=(GroupRecord*)left;
   GroupRecord *rgh=(GroupRecord*)right;
//--- by name
   return(CMTStr::Compare(lft->group,rgh->group));
  }
//+------------------------------------------------------------------+
//| Sort group by accounts                                           |
//+------------------------------------------------------------------+
int32_t CExecutionType::SortGroupByAccounts(const void* const left,const void* const right)
  {
   GroupRecord *lft=(GroupRecord*)left;
   GroupRecord *rgh=(GroupRecord*)right;
//--- firstly, sort by accounts number
   if(lft->accounts>rgh->accounts)
      return(-1);
   if(lft->accounts<rgh->accounts)
      return(1);
//--- secondly, by equity
   if(lft->equity>rgh->equity)
      return(-1);
   if(lft->equity<rgh->equity)
      return(1);
//--- thirdly, by group name
   return(CMTStr::Compare(lft->group,rgh->group));
  }
//+------------------------------------------------------------------+
