//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MoneyFlowReport.h"
#include "Tools\ReportError.h"
#include "Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
MTReportInfo CMoneyFlow::s_info=
  {
   100,                                            // report version
   MTReportAPIVersion,                             // report API version
   MTReportInfo::IE_VERSION_9,                     // minimal IE version
   L"Money Flow",                                  // report name
   L"Copyright 2000-2025, MetaQuotes Ltd.",      // copyright
   L"MetaTrader 5 Report API plug-in",             // description
   MTReportInfo::SNAPSHOT_NONE,                    // snapshot modes
   MTReportInfo::TYPE_HTML,                        // report types
   L"Money",                                       // report category
                                                   // request parameters
     {{ MTReportParam::TYPE_GROUPS,    MTAPI_PARAM_GROUPS,  DEFAULT_GROUPS                         },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_FROM                                            },
      { MTReportParam::TYPE_DATE,      MTAPI_PARAM_TO                                              },
     },3,                                          // request parameters total
     {                                             // config parameters
      { MTReportParam::TYPE_STRING,    PARAMETER_CURRENCY,              DEFAULT_CURRENCY           },
      { MTReportParam::TYPE_INT,       PARAMETER_COUNTRIES_LIST_SIZE,   DEFAULT_COUNTRIES_TO_SHOW  },
     },2                                           // config parameters total
  };

//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMoneyFlow::CMoneyFlow(void) : m_api(NULL),m_report(NULL),m_currency(NULL),m_currency_digits(0),m_top_n_countries(NULL),
                               m_group(NULL),m_deals(NULL),m_user(NULL),m_symbol(NULL),
                               m_param_from(0),m_param_to(0),m_interval(INTERVAL_DAY),m_isMoneyFlowIn(false),m_isMoneyFlowOut(false),m_days_total(0),m_months_total(0),m_countries_to_show(0),
                               m_pie_chart_in(NULL),m_pie_chart_out(NULL),
                               m_chart_money_income_total(NULL),m_chart_money_income(NULL),
                               m_chart_money_outcome_total(NULL),m_chart_money_outcome(NULL),
                               m_series_header(NULL),m_series_amount(NULL),m_series_count(NULL),m_series_average(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMoneyFlow::~CMoneyFlow(void)
  {
//--- clear common data
   Clear();
//--- release memory
   for(uint32_t i=0,c=m_money_flow_by_country.Total();i<c;i++)
     {
      delete[] m_money_flow_by_country[i].income_amounts;
      delete[] m_money_flow_by_country[i].outcome_amounts;
      delete[] m_money_flow_by_country[i].income_counts;
      delete[] m_money_flow_by_country[i].outcome_counts;
     }
//--- clear array
   m_money_flow_by_country.Clear();
//--- release memory
   for(uint32_t i=0,c=m_revenue_by_group.Total();i<c;i++)
     {
      delete[] m_revenue_by_group[i].money_in;
      delete[] m_revenue_by_group[i].money_out;
      delete[] m_revenue_by_group[i].deals_count;
      delete[] m_revenue_by_group[i].deals_overturn;
     }
//--- clear array
   m_revenue_by_group.Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CMoneyFlow::Info(MTReportInfo &info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CMoneyFlow::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CMoneyFlow::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res;
//--- check for null
   if(api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- check for type
   if(type!=MTReportInfo::TYPE_HTML)
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- remember pointer to report api
   m_api=api;
//--- capture plugin parameters
   if(!Prepare(res))
     {
      res=CReportError::Write(m_api,s_info.name,res);
      //--- clear 
      Clear();
      return(res);
     }
//--- request data from server
   if(!Calculate(res))
     {
      res=CReportError::Write(m_api,s_info.name,res);
      //--- clear 
      Clear();
      return(res);
     }
//--- prepare all graphs to be shown
   if(!PrepareGraphs(res))
     {
      res=CReportError::Write(m_api,s_info.name,res);
      //--- clear 
      Clear();
      return(res);
     }

//--- proceed main HTML template
   if(!ProceedTemplate(res))
   //--- write detailed error message
      res=CReportError::Write(m_api,s_info.name,res);

//--- clear 
   Clear();
//--- ok
   return(res);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CMoneyFlow::Clear(void)
  {
//--- release report
   if(m_report!=NULL)
     {
      m_report->Release();
      m_report=NULL;
     }
//--- release currency
   if(m_currency!=NULL)
     {
      m_currency->Release();
      m_currency=NULL;
     }
//--- release top countries parameter
   if(m_top_n_countries!=NULL)
     {
      m_top_n_countries->Release();
      m_top_n_countries=NULL;
     }
//--- release group
   if(m_group!=NULL)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- release deals
   if(m_deals!=NULL)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- release user
   if(m_user!=NULL)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- release symbol
   if(m_symbol!=NULL)
     {
      m_symbol->Release();
      m_symbol=NULL;
     }
//--- release pie chart deposits
   if(m_pie_chart_in!=NULL)
     {
      m_pie_chart_in->Release();
      m_pie_chart_in=NULL;
     }
//--- release pie chart withdrawals
   if(m_pie_chart_out!=NULL)
     {
      m_pie_chart_out->Release();
      m_pie_chart_out=NULL;
     }
//--- release chart of total deposits
   if(m_chart_money_income_total!=NULL)
     {
      m_chart_money_income_total->Release();
      m_chart_money_income_total=NULL;
     }
//--- release chart of deposits
   if(m_chart_money_income!=NULL)
     {
      m_chart_money_income->Release();
      m_chart_money_income=NULL;
     }
//--- release chart of total withdrawals
   if(m_chart_money_outcome_total!=NULL)
     {
      m_chart_money_outcome_total->Release();
      m_chart_money_outcome_total=NULL;
     }
//--- release chart of withdrawals
   if(m_chart_money_outcome!=NULL)
     {
      m_chart_money_outcome->Release();
      m_chart_money_outcome=NULL;
     }
//--- release
   if(m_series_header!=NULL)
     {
      m_series_header->Release();
      m_series_header=NULL;
     }
//--- release
   if(m_series_amount!=NULL)
     {
      m_series_amount->Release();
      m_series_amount=NULL;
     }
//--- release
   if(m_series_count!=NULL)
     {
      m_series_count->Release();
      m_series_count=NULL;
     }
//--- release
   if(m_series_average!=NULL)
     {
      m_series_average->Release();
      m_series_average=NULL;
     }
//--- fill by zeros
   m_api=NULL;
   m_param_from=m_param_to=m_days_total=m_months_total=m_countries_to_show=0;
  }
//+------------------------------------------------------------------+
//| Request data from server and calculate all statistics            |
//+------------------------------------------------------------------+
bool CMoneyFlow::Calculate(MTAPIRES &res)
  {
//--- variables
   uint64_t deals_processed=0;
//--- checks
   if(m_api==NULL||m_currency==NULL||m_top_n_countries==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- get range
   m_param_from=m_api->ParamFrom();
   m_param_to  =m_api->ParamTo();
//--- paranoid check range
   if(m_param_to<m_param_from)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- parse range
   tm tm_from={0},tm_to={0};
   if(!SMTTime::ParseTime(m_param_from,&tm_from) ||
      !SMTTime::ParseTime(m_param_to,&tm_to))
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- compute total months count
   m_months_total=(tm_to.tm_year*12+tm_to.tm_mon)+1-(tm_from.tm_year*12+tm_from.tm_mon);
//--- compute numbers of days to proceed
   m_days_total=(uint32_t)(m_param_to-m_param_from)/SECONDS_IN_DAY;
//--- if zero -> nothing to do
   if(m_days_total==0)
     {
      res=MT_RET_OK_NONE;
      return(false);
     }
//--- if first day of month, exclude it
   if(tm_to.tm_mday==1)
      m_months_total-=1;
//--- save current time
   const CReportTimer timer;
//--- iterate over each group
   for(uint32_t pos=0;m_api->GroupNext(pos,m_group)==MT_RET_OK;pos++)
     {
      //--- double check
      if(m_group==NULL||m_group->Group()==NULL)
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- check by mask
      if(!CMTStr::CheckGroupMask(m_api->ParamGroups(),m_group->Group()))
         continue;
      //--- proceed one group
      uint64_t d=0;
      if(!ProceedGroup(m_group,res,d))
         return(false);
      //--- remember count of deals were proceeded
      deals_processed+=d;
     }// iterate over each group
//--- write statistics to log
   if((res=m_api->LoggerOut(MTLogOK,L"%I64u deal records collected in %I64u ms",deals_processed,timer.Elapsed()))!=MT_RET_OK)
      return(false);
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| proceed a group of clients                                       |
//+------------------------------------------------------------------+
bool CMoneyFlow::ProceedGroup(const IMTConGroup *group,MTAPIRES &res,uint64_t &deals_processed)
  {
//--- temporary buffer
   uint64_t   *logins=NULL;
//--- count of logins in list
   uint32_t logins_total;
//--- reset counter
   deals_processed=0;
//--- check
   if(m_api==NULL)
      return(false);
//--- get logins for group
   if((res=m_api->UserLogins(group->Group(),logins,logins_total))!=MT_RET_OK)
      return(false);
//--- check for null pointer
   if(logins==NULL)
      return(true);
//--- paranoid check
   if(logins_total==0)
     {
      m_api->Free(logins);
      return(true);
     }
//--- iterate over each login
   for(uint32_t i=0;i<logins_total;i++)
     {
      uint64_t d=0;
      //--- proceed one login 
      if(!ProceedLogin(logins[i],group,res,d))
        {
         m_api->Free(logins);
         return(false);
        }
      //--- remember counter
      deals_processed+=d;
     }//iterate over each login
//--- release memory
   m_api->Free(logins);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| proceed one login                                                |
//+------------------------------------------------------------------+
bool CMoneyFlow::ProceedLogin(const uint64_t login,const IMTConGroup *group,MTAPIRES &res,uint64_t &deals_processed)
  {
//--- check
   if(m_api==NULL||m_deals==NULL||m_currency==NULL)
      return(false);
//--- reset counter
   deals_processed=0;
//--- get deals
   if((res=m_api->DealGet(login,m_param_from,m_param_to,m_deals))!=MT_RET_OK)
      return(false);
//--- get deals count
   const uint32_t dc=m_deals->Total();
//--- skip further processing if none
   if(dc==0)
      return(true);
//--- save count of requested deals
   deals_processed+=dc;
//--- iterate over each deal
   for(uint32_t d=0;d<dc;d++)
     {
      //--- get deal
      IMTDeal *deal=m_deals->Next(d);
      //--- check for null pointer
      if(deal==NULL)
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- check for small value
      if(fabs(deal->Profit())<DBL_EPSILON)
         continue;
      //--- skip out of range
      if(deal->Time()<m_param_from||deal->Time()>=m_param_to)
         continue;
      //--- day index
      uint32_t day_index=0;
      //--- convert time to day index
      if(!TimeToDayIndex(deal->Time(),day_index))
         continue;
      //--- double check
      if(day_index>=m_days_total)
         continue;
      //--- get user info
      if((res=m_api->UserGet(login,m_user))!=MT_RET_OK)
         return(false);
      //--- get group name
      LPCWSTR grp=group->Group();
      //--- check for null group name
      if(grp==NULL)
         continue;
      //--- proceed balance operation
      if((deal->Entry()==IMTDeal::ENTRY_IN) &&
         (deal->Action()==IMTDeal::DEAL_BALANCE))
        {
         //--- check for NaN or zero
         if(_isnan(deal->Profit())||
            fabs(deal->Profit())<DBL_EPSILON)
            continue;
         //--- get conversion rate from deal profit currency to common currency
         double rate=0.0;
         if((res=m_api->TradeRateSell(group->Currency(),m_currency->ValueString(),rate))!=MT_RET_OK)
            return(false);
         //--- if no rate, skip
         if(fabs(rate)<DBL_EPSILON)
            continue;
         //--- here we use common currency
         if(!CountryAddMoneyFlow(deal->Profit()>0.0?DIRECTION_IN:DIRECTION_OUT,m_user->Country(),day_index,rate*deal->Profit()))
           {
            res=MT_RET_ERROR;
            return(false);
           }
         //--- add it. use group currency
         if(!GroupDealAdd(group->Group(),group->Currency(),day_index,deal->Profit(),0.0))
           {
            res=MT_RET_ERROR;
            return(false);
           }
        }
      //--- proceed only for [in]out buy/sell deals
      if((deal->Entry()==IMTDeal::ENTRY_INOUT||deal->Entry()==IMTDeal::ENTRY_OUT) &&
         (deal->Action()==IMTDeal::DEAL_BUY||deal->Action()==IMTDeal::DEAL_SELL))
        {
         //--- checks
         if(_isnan(deal->RateProfit()) ||
            _isnan(deal->ContractSize()) ||
            fabs(deal->ContractSize())<DBL_EPSILON ||
            deal->Volume()==0 ||
            fabs(deal->RateProfit())<DBL_EPSILON)
           {
            //--- write error message
            if((res=m_api->LoggerOut(MTLogErr,L"Invalid deal #%I64u",deal->Deal()))!=MT_RET_OK)
               return(false);
            continue;
           }
         //--- get symbol
         if(m_api->SymbolGetLight(deal->Symbol(),m_symbol)!=MT_RET_OK||m_symbol==NULL)
           {
            if((res=m_api->LoggerOut(MTLogErr,L"Symbol '%s' not found",deal->Symbol()))!=MT_RET_OK)
               return(false);
            continue;
           }
         double amount_raw=0.0;
         //--- calculate raw amount
         switch(m_symbol->CalcMode())
           {
            case IMTConSymbol::TRADE_MODE_FOREX:
            case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
               amount_raw=SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
               break;
            case IMTConSymbol::TRADE_MODE_FUTURES:
            case IMTConSymbol::TRADE_MODE_EXCH_FUTURES:
            case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
               amount_raw=deal->Price()*SMTMath::VolumeToDouble(deal->Volume())*deal->TickValue();
               if(fabs(deal->TickSize())>DBL_EPSILON)
                  amount_raw/=deal->TickSize();
               break;
            default:
               amount_raw=deal->Price()*SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
               break;
           }
         //--- normalize
         amount_raw=SMTMath::PriceNormalize(amount_raw,m_symbol->CurrencyBaseDigits());
         //--- convert it to deposit currency and normalize
         amount_raw=SMTMath::PriceNormalize(amount_raw*deal->RateProfit(), group->CurrencyDigits());
         //--- add it. use group currency
         if(!GroupDealAdd(group->Group(),group->Currency(),day_index,0.0,amount_raw))
           {
            res=MT_RET_ERROR;
            return(false);
           }
        }//deal processor
     }//deals iterator
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Add Money Flow Deal By Day                                       |
//+------------------------------------------------------------------+
bool CMoneyFlow::CountryAddMoneyFlow(const uint32_t direction,LPCWSTR country,const uint32_t day,const double money)
  {
//--- check for valid input data
   if(direction>=DIRECTION_COUNT||day>=m_days_total)
      return(false);
//--- check for small value
   if(fabs(money)<DBL_EPSILON)
      return(true);
//--- use default string for empty country name
   if(country==NULL||country[0]==0)
      country=COUNTRY_EMPTY_VALUE;
//--- country money flow pointer
   CountryMoneyFlow* country_money_flow=m_money_flow_by_country.Search(country,SortCountriesByName);
//--- find country by hash. if fail, add new one
   if(country_money_flow==NULL)
     {
      //--- create temporary object in stack
      CountryMoneyFlow temp={};
      //--- copy country name
      CMTStr::Copy(temp.country,country);
      //--- insert data and get pointer if success
      if((country_money_flow=m_money_flow_by_country.Insert(&temp,SortCountriesByName))==NULL)
         return(false);
      //--- copy size
      country_money_flow->values_total=m_days_total;
      //--- allocate memory
      country_money_flow->income_amounts=new(std::nothrow)double[country_money_flow->values_total];
      if(country_money_flow->income_amounts==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(country_money_flow->income_amounts,country_money_flow->values_total*sizeof(country_money_flow->income_amounts[0]));
      //--- allocate memory
      country_money_flow->outcome_amounts=new(std::nothrow)double[country_money_flow->values_total];
      if(country_money_flow->outcome_amounts==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(country_money_flow->outcome_amounts,country_money_flow->values_total*sizeof(country_money_flow->outcome_amounts[0]));
      //--- allocate memory
      country_money_flow->income_counts=new(std::nothrow)UINT[country_money_flow->values_total];
      if(country_money_flow->income_counts==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(country_money_flow->income_counts,country_money_flow->values_total*sizeof(country_money_flow->income_counts[0]));
      //--- allocate memory
      country_money_flow->outcome_counts=new(std::nothrow)UINT[country_money_flow->values_total];
      if(country_money_flow->outcome_counts==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(country_money_flow->outcome_counts,country_money_flow->values_total*sizeof(country_money_flow->outcome_counts[0]));
     }

//--- append deposits
   if(direction==DIRECTION_IN)
     {
      country_money_flow->income_amounts[day]=SMTMath::MoneyAdd(country_money_flow->income_amounts[day],money,m_currency_digits);
      country_money_flow->total_income_amount=SMTMath::MoneyAdd(country_money_flow->total_income_amount,money,m_currency_digits);
      country_money_flow->income_counts[day]++;
     }
//--- append withdrawals
   else
     {
      country_money_flow->outcome_amounts[day]=SMTMath::MoneyAdd(country_money_flow->outcome_amounts[day],money,m_currency_digits);
      country_money_flow->total_outcome_amount=SMTMath::MoneyAdd(country_money_flow->total_outcome_amount,money,m_currency_digits);
      country_money_flow->outcome_counts[day]++;
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Add Deal (trade or balance) By Day                               |
//+------------------------------------------------------------------+
bool CMoneyFlow::GroupDealAdd(LPCWSTR group,LPCWSTR currency,const uint32_t day,const double balance,const double trade_overturn)
  {
//--- check bounds
   if(day>=m_days_total)
      return(false);
//--- skip zeros
   if(fabs(balance)<DBL_EPSILON&&fabs(trade_overturn)<DBL_EPSILON)
      return(true);
//--- try to find group by hash
   GroupOverturn* group_overturn=m_revenue_by_group.Search(group,SortGroupsByName);
//--- if there is no such group
   if(group_overturn==NULL)
     {
      //--- create temporary object in stack
      GroupOverturn temp={};
      //--- copy group name
      CMTStr::Copy(temp.group,group);
      //--- insert data and get pointer if success
      if((group_overturn=m_revenue_by_group.Insert(&temp,SortGroupsByName))==NULL)
         return(false);
      //--- copy size
      group_overturn->values_count=m_days_total;
      //--- copy currency name
      CMTStr::Copy(group_overturn->currency,currency);
      //--- copy currency digits
      group_overturn->currency_digits=SMTMath::MoneyDigits(currency);
      //--- allocate memory
      group_overturn->deals_count=new(std::nothrow)UINT[group_overturn->values_count];
      if(group_overturn->deals_count==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(group_overturn->deals_count,group_overturn->values_count*sizeof(group_overturn->deals_count[0]));
      //--- allocate memory
      group_overturn->deals_overturn=new(std::nothrow)double[group_overturn->values_count];
      if(group_overturn->deals_overturn==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(group_overturn->deals_overturn,group_overturn->values_count*sizeof(group_overturn->deals_overturn[0]));
      //--- allocate memory
      group_overturn->money_in=new(std::nothrow)double[group_overturn->values_count];
      if(group_overturn->money_in==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(group_overturn->money_in,group_overturn->values_count*sizeof(group_overturn->money_in[0]));
      //--- allocate memory
      group_overturn->money_out=new(std::nothrow)double[group_overturn->values_count];
      if(group_overturn->money_out==NULL)
         return(false);
      //--- fill with zeros
      ZeroMemory(group_overturn->money_out,group_overturn->values_count*sizeof(group_overturn->money_out[0]));
     }

//--- if balance deal
   if(fabs(balance)>DBL_EPSILON)
     {
      if(balance>0.0)
         group_overturn->money_in[day]=SMTMath::MoneyAdd(group_overturn->money_in[day],balance,group_overturn->currency_digits);
      else
         group_overturn->money_out[day]=SMTMath::MoneyAdd(group_overturn->money_out[day],balance,group_overturn->currency_digits);
     }
//--- if trade deal
   if(fabs(trade_overturn)>DBL_EPSILON)
     {
      group_overturn->deals_count[day]++;
      group_overturn->deals_overturn[day]=SMTMath::MoneyAdd(group_overturn->deals_overturn[day],trade_overturn,group_overturn->currency_digits);
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Prepare all graphs to be shown                                   |
//+------------------------------------------------------------------+
bool CMoneyFlow::PrepareGraphs(MTAPIRES &res)
  {
//--- double check
   if(m_api==NULL||m_top_n_countries==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- compute number of countries to show
   m_countries_to_show=std::min((uint32_t)m_top_n_countries->ValueInt(),m_money_flow_by_country.Total());
//--- if zero
   if(m_countries_to_show==0)
     {
      res=MT_RET_ERR_NOTFOUND;
      return(false);
     }
//--- get total money flow for future using - to exclude empty block
   double money_in_total,money_out_total;
   if(!GetMoneyFlowTotal(money_in_total,money_out_total))
     {
      res=MT_RET_ERROR;
      return(false);
     }
//--- check for amount of money
   m_isMoneyFlowIn   =fabs(money_in_total)>DBL_EPSILON;
   m_isMoneyFlowOut  =fabs(money_out_total)>DBL_EPSILON;
//--- if there is no money flow at all
   if(!m_isMoneyFlowIn&&!m_isMoneyFlowOut)
     {
      res=MT_RET_ERR_NOTFOUND;
      return(false);
     }
//--- which interval to be used
   if(m_days_total<SMOOTH_RANGE_ONE)
      m_interval=INTERVAL_DAY;
   else
      if(m_days_total<SMOOTH_RANGE_TWO)
         m_interval=INTERVAL_WEEK;
      else
         m_interval=INTERVAL_MONTH;
//--- if there are money flow in
   if(m_isMoneyFlowIn)
     {
      //--- sort countries by total deposits value
      m_money_flow_by_country.Sort(SortCountriesByIncome);
      //--- create chart context
      if((m_pie_chart_in=m_api->ChartCreate())==NULL ||
         (m_chart_money_income_total=m_api->ChartCreate())==NULL ||
         (m_chart_money_income=m_api->ChartCreate())==NULL)
        {
         res=MT_RET_ERR_MEM;
         return(false);
        }
      //--- prepare pie chart
      if(!PreparePieChart(DIRECTION_IN,res,m_pie_chart_in))
         return(false);
      //--- prepare total graph
      if(!PrepareTotalGraph(DIRECTION_IN,m_interval,res,m_chart_money_income_total))
         return(false);
      //--- prepare proportion graph
      if(!PrepareProportionGraph(DIRECTION_IN,m_interval,res,m_chart_money_income))
         return(false);
     }
//--- if there are money flow out
   if(m_isMoneyFlowOut)
     {
      //--- sort countries by total withdrawals value
      m_money_flow_by_country.Sort(SortCountriesByOutcome);
      //--- create chart context
      if((m_pie_chart_out=m_api->ChartCreate())==NULL ||
         (m_chart_money_outcome_total=m_api->ChartCreate())==NULL ||
         (m_chart_money_outcome=m_api->ChartCreate())==NULL)
        {
         res=MT_RET_ERR_MEM;
         return(false);
        }
      //--- prepare pie chart
      if(!PreparePieChart(DIRECTION_OUT,res,m_pie_chart_out))
         return(false);
      //--- prepare total graph
      if(!PrepareTotalGraph(DIRECTION_OUT,m_interval,res,m_chart_money_outcome_total))
         return(false);
      //--- prepare proportion graph
      if(!PrepareProportionGraph(DIRECTION_OUT,m_interval,res,m_chart_money_outcome))
         return(false);
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Get parameter currency                                           |
//+------------------------------------------------------------------+
bool CMoneyFlow::Prepare(MTAPIRES &res)
  {
//--- checks
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- create contexts
   if((m_report            =m_api->ReportCreate())    ==NULL ||
      (m_currency          =m_api->ParamCreate())     ==NULL ||
      (m_top_n_countries   =m_api->ParamCreate())     ==NULL ||
      (m_group             =m_api->GroupCreate())     ==NULL ||
      (m_deals             =m_api->DealCreateArray()) ==NULL ||
      (m_user              =m_api->UserCreate())      ==NULL ||
      (m_symbol            =m_api->SymbolCreate())    ==NULL)
     {
      res=MT_RET_ERR_MEM;
      return(false);
     }
//--- get currency parameter
   if((res=m_api->ReportCurrent(m_report))!=MT_RET_OK)
      return(false);
//--- get currency parameter
   if((res=m_report->ParameterGet(PARAMETER_CURRENCY,m_currency))!=MT_RET_OK)
     {
      //--- fill parameter manually
      if((res=m_currency->Name(PARAMETER_CURRENCY))!=MT_RET_OK)
         return(false);
      //--- set type
      if((res=m_currency->Type(IMTConParam::TYPE_STRING))!=MT_RET_OK)
         return(false);
      //--- set default value
      if((res=m_currency->Value(DEFAULT_CURRENCY))!=MT_RET_OK)
         return(false);
     }
//--- check value
   if(m_currency->Value()==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- setup digits
   m_currency_digits=SMTMath::MoneyDigits(m_currency->Value());
//--- get currency parameter
   if((res=m_report->ParameterGet(PARAMETER_COUNTRIES_LIST_SIZE,m_top_n_countries))!=MT_RET_OK)
     {
      //--- fill parameter manually
      if((res=m_top_n_countries->Name(PARAMETER_COUNTRIES_LIST_SIZE))!=MT_RET_OK)
         return(false);
      //--- set type
      if((res=m_top_n_countries->Type(IMTConParam::TYPE_INT))!=MT_RET_OK)
         return(false);
      //--- set default value
      if((res=m_top_n_countries->Value(DEFAULT_COUNTRIES_TO_SHOW))!=MT_RET_OK)
         return(false);
     }
//--- double check
   if(m_top_n_countries->ValueInt()<=0)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- ok
   res=MT_RET_OK;
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Prepare Pie Chart                                                |
//+------------------------------------------------------------------+
bool CMoneyFlow::PreparePieChart(const uint32_t direction,MTAPIRES &res,IMTReportChart *&chart)
  {
//--- context for values
   IMTReportSeries *values_series=NULL;
//--- checks
   if(m_api==NULL||chart==NULL||direction>=DIRECTION_COUNT)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- clear chart
   if((res=chart->Clear())!=MT_RET_OK)
      return(false);
//--- set type
   if((res=chart->Type(IMTReportChart::TYPE_PIE))!=MT_RET_OK)
      return(false);
//--- set title
   if((res=chart->Title(direction==DIRECTION_IN?DEPOSITS_KEYWORD:WITHDRAWALS_KEYWORD))!=MT_RET_OK)
      return(false);
//--- set tooltip
   if((res=chart->PieceTooltip(L"%VARIABLE%<BR>%VALUE%"))!=MT_RET_OK)
      return(false);
//--- iterate over each country
   for(uint32_t c=0;c<m_countries_to_show;c++)
     {
      //--- check for empty value and stop if is it
      if(fabs(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount)<DBL_EPSILON)
         break;
      //--- create series
      if((values_series=m_api->ChartCreateSeries())==NULL)
        {
         res=MT_RET_ERR_MEM;
         return(false);
        }
      //--- set series type
      if((res=values_series->Type(IMTReportSeries::TYPE_PIECE))!=MT_RET_OK)
        {
         values_series->Release();
         return(false);
        }
      //--- set title
      if((res=values_series->Title(m_money_flow_by_country[c].country))!=MT_RET_OK)
        {
         values_series->Release();
         return(false);
        }
      //--- set series color
      if((res=values_series->Color(ExtChartColors[c%(sizeof(ExtChartColors)/sizeof(ExtChartColors[0]))]))!=MT_RET_OK)
        {
         values_series->Release();
         return(false);
        }
      //--- add value to series
      if((res=values_series->ValueAddDouble(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount))!=MT_RET_OK)
        {
         values_series->Release();
         return(false);
        }
      //--- add series (after adding series will be released by chart)
      if((res=chart->SeriesAdd(values_series))!=MT_RET_OK)
        {
         values_series->Release();
         return(false);
        }
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Prepare Proportion Graph                                         |
//+------------------------------------------------------------------+
bool CMoneyFlow::PrepareProportionGraph(const uint32_t direction,const uint32_t interval,MTAPIRES &res,IMTReportChart *&chart)
  {
//--- context for header
   IMTReportSeries  *header_series=NULL;
//--- buffer
   CMTStr256         str;
//--- checks
   if(m_api==NULL||chart==NULL||interval>=INTERVAL_COUNT)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- clear chart
   if((res=chart->Clear())!=MT_RET_OK)
      return(false);
//--- get current time
   int64_t ctm=m_api->TimeCurrent();
   tm tm_ctm={0};
//--- parse time
   if(!SMTTime::ParseTime(ctm,&tm_ctm))
     {
      res=MT_RET_ERROR;
      return(false);
     }
//--- set chart type
   if((res=chart->Type(IMTReportChart::TYPE_GRAPH_NORMALIZED))!=MT_RET_OK)
      return(false);
//--- format string
   str.Assign(direction==DIRECTION_IN?DEPOSITS_KEYWORD:WITHDRAWALS_KEYWORD);
   str.Append(GRAPH_BY_COUNTY_POSTFIX);
//--- set title
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(false);
//--- prepare title series
   if((header_series=m_api->ChartCreateSeries())==NULL)
     {
      res=MT_RET_ERR_MEM;
      return(false);
     }
//--- set header series type
   if((res=header_series->Type(IMTReportSeries::TYPE_TITLE))!=MT_RET_OK)
     {
      header_series->Release();
      return(false);
     }
//--- fill header with dates depends on interval
   switch(interval)
     {
      case INTERVAL_DAY:
         if(!FillProportionGraphDayByDay(direction,res,header_series,chart))
           {
            header_series->Release();
            return(false);
           }
         break;
      case INTERVAL_WEEK:
         if(!FillProportionGraphWeekByWeek(direction,res,header_series,chart))
           {
            header_series->Release();
            return(false);
           }
         break;
      case INTERVAL_MONTH:
         if(!FillProportionGraphMonthByMonth(direction,res,header_series,chart))
           {
            header_series->Release();
            return(false);
           }
         break;
      default:
         header_series->Release();
         return(false);
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill Proportion Graph Day By Day                                 |
//+------------------------------------------------------------------+
bool CMoneyFlow::FillProportionGraphDayByDay(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportChart *&chart)
  {
//--- buffer
   CMTStr256 str;
//--- checks
   if(m_api==NULL||direction>=DIRECTION_COUNT)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- iterate over each day
   for(uint32_t i=0;i<m_days_total;i++)
     {
      uint32_t deals_count=0;
      //--- iterate over each country
      for(uint32_t c=0;c<m_countries_to_show;c++)
        {
         //--- check for zero
         if(fabs(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount)<DBL_EPSILON)
            break;

         double ta=0.0,a=0.0;
         uint32_t dc=0;

         //--- get all money flow
         if(!GetTotalMoneyFlowByDay(direction,i,ta,a,dc))
           {
            res=MT_RET_ERROR;
            return(false);
           }

         deals_count+=dc;
        }
      //--- we don't need countries without money flow
      if(deals_count==0)
         continue;
      //--- create tm struct
      tm tm_day={0};
      if(!SMTTime::ParseTime(m_api->ParamFrom()+i*SECONDS_IN_DAY,&tm_day))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- format it to human readable format
      if(str.Format(L"%d/%d/%d",tm_day.tm_mday,tm_day.tm_mon+1,tm_day.tm_year+1900)<0)
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add value
      if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
         return(false);
     }
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(header_series))!=MT_RET_OK)
      return(false);
//--- iterate over each country
   for(uint32_t c=0;c<m_countries_to_show;c++)
     {
      //--- check for zero
      if(fabs(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount)<DBL_EPSILON)
         break;
      //--- context for values
      IMTReportSeries *series_per_country=NULL;
      //--- create series
      if((series_per_country=m_api->ChartCreateSeries())==NULL)
        {
         res=MT_RET_ERR_MEM;
         return(false);
        }
      //--- set type
      if((res=series_per_country->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
        {
         series_per_country->Release();
         return(false);
        }
      //--- format tooltip string
      str.Clear();
      str.Append(L"Date: %VARIABLE%<BR>");
      str.Append(m_money_flow_by_country[c].country);
      str.Append(L": %VALUE% (%NORMALIZED_VALUE% %)");
      //--- set tooltip
      if((res=series_per_country->Tooltip(str.Str()))!=MT_RET_OK)
        {
         series_per_country->Release();
         return(false);
        }
      //--- set color
      if((res=series_per_country->Color(ExtChartColors[c%(sizeof(ExtChartColors)/sizeof(ExtChartColors[0]))]))!=MT_RET_OK)
        {
         series_per_country->Release();
         return(false);
        }
      //--- set title
      if((res=series_per_country->Title(m_money_flow_by_country[c].country))!=MT_RET_OK)
        {
         series_per_country->Release();
         return(false);
        }
      //--- iterate over each day
      for(uint32_t i=0;i<m_days_total;i++)
        {
         uint32_t deals_count=0;
         //--- iterate over each country
         for(uint32_t tc=0;tc<m_countries_to_show;tc++)
           {
            //--- check for zero
            if(fabs(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount)<DBL_EPSILON)
               break;
            //--- temporary values
            double ta=0.0,a=0.0;
            uint32_t dc=0;
            //--- get all money by day
            if(!GetTotalMoneyFlowByDay(direction,i,ta,a,dc))
              {
               series_per_country->Release();
               res=MT_RET_ERROR;
               return(false);
              }
            //--- remember count of deals
            deals_count+=dc;
           }
         //--- we don't need countries without money flow
         if(deals_count==0)
            continue;
         //--- money flow total a day
         double t=0.0;
         //--- get money flow by country by day
         if(!CountryGetTotalMoneyFlowByDay(direction,m_money_flow_by_country[c],i,t))
           {
            series_per_country->Release();
            res=MT_RET_ERROR;
            return(false);
           }
         //--- add value
         if((res=series_per_country->ValueAddDouble(t))!=MT_RET_OK)
           {
            series_per_country->Release();
            res=MT_RET_ERROR;
            return(false);
           }
        }
      //--- add series to chart(after adding series will be released by chart)
      if((res=chart->SeriesAdd(series_per_country))!=MT_RET_OK)
        {
         series_per_country->Release();
         return(false);
        }
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill Proportion Graph Week By Week                               |
//+------------------------------------------------------------------+
bool CMoneyFlow::FillProportionGraphWeekByWeek(const uint32_t direction,MTAPIRES &res,IMTReportSeries *series_header,IMTReportChart *&chart)
  {
//--- buffer
   CMTStr256 str;
//--- day iterator
   uint32_t i;
//--- checks
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- iterate over each day
   for(i=0;i<m_days_total;i++)
     {
      //--- create tm struct
      tm tm_day={0};
      if(!SMTTime::ParseTime(m_api->ParamFrom()+i*SECONDS_IN_DAY,&tm_day))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- format it to human readable format
      if(str.Format(L"%d/%d/%d",tm_day.tm_mday,tm_day.tm_mon+1,tm_day.tm_year+1900)<0)
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add value only each DAYS_IN_WEEK-th value
      if(i%DAYS_IN_WEEK==0)
         if((res=series_header->ValueAdd(str.Str()))!=MT_RET_OK)
            return(false);
     }
//--- add last day element
   if(i%DAYS_IN_WEEK!=0)
      if((res=series_header->ValueAdd(str.Str()))!=MT_RET_OK)
         return(false);
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(series_header))!=MT_RET_OK)
      return(false);
//--- iterate over each country
   for(uint32_t c=0;c<m_countries_to_show;c++)
     {
      //--- check for zero
      if(fabs(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount)<DBL_EPSILON)
         break;
      //--- context pointer
      IMTReportSeries *series_amount=NULL;
      //--- create series
      if((series_amount=m_api->ChartCreateSeries())==NULL)
        {
         res=MT_RET_ERR_MEM;
         return(false);
        }
      //--- set type
      if((res=series_amount->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- format tooltip
      str.Clear();
      str.Append(L"Date: %VARIABLE%<BR>");
      str.Append(m_money_flow_by_country[c].country);
      str.Append(L": %VALUE% (%NORMALIZED_VALUE% %)");
      //--- set tooltip
      if((res=series_amount->Tooltip(str.Str()))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- set color
      if((res=series_amount->Color(ExtChartColors[c%(sizeof(ExtChartColors)/sizeof(ExtChartColors[0]))]))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- set title
      if((res=series_amount->Title(m_money_flow_by_country[c].country))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- money
      double money=0.0;
      //--- iterate over each day
      for(i=0;i<m_days_total;i++)
        {
         double t=0.0;
         //--- get money flow by day
         if(!CountryGetTotalMoneyFlowByDay(direction,m_money_flow_by_country[c],i,t))
           {
            series_amount->Release();
            res=MT_RET_ERROR;
            return(false);
           }
         //--- add value
         money=SMTMath::MoneyAdd(money,t,m_currency_digits);
         //--- only complete week
         if((i+1)%DAYS_IN_WEEK==0)
           {
            //--- add value
            if((res=series_amount->ValueAddDouble(money))!=MT_RET_OK)
              {
               series_amount->Release();
               return(false);
              }
            //--- reset
            money=0.0;
           }
        }
      //---  add last rest data
      if(i%DAYS_IN_WEEK!=0)
        {
         //--- add value
         if((res=series_amount->ValueAddDouble(money))!=MT_RET_OK)
           {
            series_amount->Release();
            return(false);
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=chart->SeriesAdd(series_amount))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill Proportion Graph Month By Month                             |
//+------------------------------------------------------------------+
bool CMoneyFlow::FillProportionGraphMonthByMonth(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportChart *&chart)
  {
//--- buffer
   CMTStr256 str;
//--- checks
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- iterate over each day
   for(uint32_t i=0;i<m_months_total;i++)
     {
      //--- convert month index to human readable date string
      if(!MonthIndexToStr(i,str))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add to header series
      if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
         return(false);
     }
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(header_series))!=MT_RET_OK)
      return(false);
//--- iterate over each country
   for(uint32_t c=0;c<m_countries_to_show;c++)
     {
      //--- check for zero
      if(fabs(direction==DIRECTION_IN?m_money_flow_by_country[c].total_income_amount:m_money_flow_by_country[c].total_outcome_amount)<DBL_EPSILON)
         break;
      //--- context
      IMTReportSeries *series_amount=NULL;
      //--- create series
      if((series_amount=m_api->ChartCreateSeries())==NULL)
        {
         res=MT_RET_ERR_MEM;
         return(false);
        }
      //--- set type
      if((res=series_amount->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- format tooltip
      str.Clear();
      str.Append(L"Date: %VARIABLE%<BR>");
      str.Append(m_money_flow_by_country[c].country);
      str.Append(L": %VALUE% (%NORMALIZED_VALUE% %)");
      //--- set tooltip
      if((res=series_amount->Tooltip(str.Str()))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- set color
      if((res=series_amount->Color(ExtChartColors[c%(sizeof(ExtChartColors)/sizeof(ExtChartColors[0]))]))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- set title
      if((res=series_amount->Title(m_money_flow_by_country[c].country))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
      //--- iterate over each month
      for(uint32_t i=0;i<m_months_total;i++)
        {
         double money=0.0;
         //--- get money flow by country by month
         if(!CountryGetMoneyFlowByMonth(m_money_flow_by_country[c],direction,i,money))
           {
            series_amount->Release();
            res=MT_RET_ERROR;
            return(false);
           }
         //--- add value
         if((res=series_amount->ValueAddDouble(money))!=MT_RET_OK)
           {
            series_amount->Release();
            return(false);
           }
        }
      //--- add series (after adding series will be released by chart)
      if((res=chart->SeriesAdd(series_amount))!=MT_RET_OK)
        {
         series_amount->Release();
         return(false);
        }
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Prepare Total Graph                                              |
//+------------------------------------------------------------------+
bool CMoneyFlow::PrepareTotalGraph(const uint32_t direction,const uint32_t interval,MTAPIRES &res,IMTReportChart *&chart)
  {
//--- buffer
   CMTStr256 str;
//--- checks
   if(chart==NULL||m_api==NULL||direction>=DIRECTION_COUNT||interval>=INTERVAL_COUNT)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- create series
   if((m_series_header=m_api->ChartCreateSeries())==NULL ||
      (m_series_amount=m_api->ChartCreateSeries())==NULL ||
      (m_series_count=m_api->ChartCreateSeries())==NULL ||
      (m_series_average=m_api->ChartCreateSeries())==NULL)
     {
      res=MT_RET_ERR_MEM;
      return(false);
     }
//--- set type
   if((res=m_series_header->Type(IMTReportSeries::TYPE_TITLE))!=MT_RET_OK)
      return(false);
//--- set type
   if((res=m_series_amount->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
      return(false);
//--- format tooltip string
   str.Clear();
   str.Append(L"Date: %VARIABLE%<BR>");
   str.Append(L"Amount: %VALUE%");
//--- set tooltip
   if((res=m_series_amount->Tooltip(str.Str()))!=MT_RET_OK)
      return(false);
//--- set color
   if((res=m_series_amount->Color(ExtChartColors[2]))!=MT_RET_OK)
      return(false);
//--- set title
   if((res=m_series_amount->Title(L"Amount"))!=MT_RET_OK)
      return(false);
//--- set type
   if((res=m_series_count->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
      return(false);
//--- format tooltip string
   str.Clear();
   str.Append(L"Date: %VARIABLE%<BR>");
   str.Append(L"Count: %VALUE%");
//--- set tooltip
   if((res=m_series_count->Tooltip(str.Str()))!=MT_RET_OK)
      return(false);
//--- set color
   if((res=m_series_count->Color(ExtChartColors[7]))!=MT_RET_OK)
      return(false);
//--- set title
   if((res=m_series_count->Title(L"Count"))!=MT_RET_OK)
      return(false);
//--- set type
   if((res=m_series_average->Type(IMTReportSeries::TYPE_AREA))!=MT_RET_OK)
      return(false);
//--- format tooltip string
   str.Clear();
   str.Append(L"Date: %VARIABLE%<BR>");
   str.Append(L"Average: %VALUE%");
//--- set tooltip
   if((res=m_series_average->Tooltip(str.Str()))!=MT_RET_OK)
      return(false);
//--- set color
   if((res=m_series_average->Color(ExtChartColors[4]))!=MT_RET_OK)
      return(false);
//--- set title
   if((res=m_series_average->Title(L"Average"))!=MT_RET_OK)
      return(false);
//--- fill graph based on interval
   switch(interval)
     {
      case INTERVAL_DAY:
         if(!FillTotalGraphSeriesDayByDay(direction,res,m_series_header,m_series_amount,m_series_count,m_series_average))
            return(false);
         break;
      case INTERVAL_WEEK:
         if(!FillTotalGraphSeriesWeekByWeek(direction,res,m_series_header,m_series_amount,m_series_count,m_series_average))
            return(false);
         break;
      case INTERVAL_MONTH:
         if(!FillTotalGraphSeriesMonthByMonth(direction,res,m_series_header,m_series_amount,m_series_count,m_series_average))
            return(false);
         break;
     }
//--- clear chart
   if((res=chart->Clear())!=MT_RET_OK)
      return(false);
//--- prepare chart
   if((res=chart->Type(IMTReportChart::TYPE_GRAPH_ACCUMULATION))!=MT_RET_OK)
      return(false);
//--- format title string
   str.Clear();
   str.Append(direction==DIRECTION_IN?DEPOSITS_KEYWORD:WITHDRAWALS_KEYWORD);
   str.Append(GRAPH_TOTAL_POSTFIX);
//--- set title
   if((res=chart->Title(str.Str()))!=MT_RET_OK)
      return(false);
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(m_series_header))!=MT_RET_OK)
      return(false);
//--- set it to zero to avoid memory release on it
   m_series_header=NULL;
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(m_series_amount))!=MT_RET_OK)
      return(false);
//--- set it to zero to avoid memory release on it
   m_series_amount=NULL;
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(m_series_count))!=MT_RET_OK)
      return(false);
//--- set it to zero to avoid memory release on it
   m_series_count=NULL;
//--- add series (after adding series will be released by chart)
   if((res=chart->SeriesAdd(m_series_average))!=MT_RET_OK)
      return(false);
//--- set it to zero to avoid memory release on it
   m_series_average=NULL;
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill Total Graph Series Day By Day                               |
//+------------------------------------------------------------------+
bool CMoneyFlow::FillTotalGraphSeriesDayByDay(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportSeries *series_amount,IMTReportSeries *series_count,IMTReportSeries *series_average)const
  {
//--- buffer
   CMTStr32 str;
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- iterate over each day
   for(uint32_t d=0;d<m_days_total;d++)
     {
      double total_amount=0.0,average=0.0;
      uint32_t deals_count=0;
      //--- get all money by day
      if(!GetTotalMoneyFlowByDay(direction,d,total_amount,average,deals_count))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- check
      if(deals_count==0)
         continue;
      //--- parse time
      tm tm_day={0};
      if(!SMTTime::ParseTime(m_api->ParamFrom()+d*SECONDS_IN_DAY,&tm_day))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- format it to human readable format
      if(str.Format(L"%d/%d/%d",tm_day.tm_mday,tm_day.tm_mon+1,tm_day.tm_year+1900)<0)
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add value to series
      if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
         return(false);
      //--- add value
      if((res=series_amount->ValueAddDouble(total_amount))!=MT_RET_OK)
         return(false);
      //--- add value
      if((res=series_count->ValueAddInt(deals_count))!=MT_RET_OK)
         return(false);
      //--- average
      average/=deals_count;
      //--- add value
      if((res=series_average->ValueAddDouble(average))!=MT_RET_OK)
         return(false);
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill Total Graph Series Week By Week                             |
//+------------------------------------------------------------------+
bool CMoneyFlow::FillTotalGraphSeriesWeekByWeek(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportSeries *series_amount,IMTReportSeries *series_count,IMTReportSeries *series_average)const
  {
   double   total_amount=0.0;
   double   average     =0.0;
   uint32_t deals_count =0;
   uint32_t i;
   CMTStr32 str;
//--- check
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(false);
     }
//--- iterate over each day to create a header
   for(i=0;i<m_days_total;i++)
     {
      //--- make tm structure
      tm tm_day={0};
      if(!SMTTime::ParseTime(m_api->ParamFrom()+i*SECONDS_IN_DAY,&tm_day))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- format it to string
      if(str.Format(L"%d/%d/%d",tm_day.tm_mday,tm_day.tm_mon+1,tm_day.tm_year+1900)<0)
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add value
      if(i%DAYS_IN_WEEK==0)
         if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
            return(false);
     }
//--- add rest data
   if(i%DAYS_IN_WEEK!=0)
      if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
         return(false);
//--- iterate over each day
   for(i=0;i<m_days_total;i++)
     {
      double t=0.0,a=0.0;
      uint32_t d=0;
      //--- get all money by day
      if(!GetTotalMoneyFlowByDay(direction,i,t,a,d))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- compute
      total_amount=SMTMath::MoneyAdd(total_amount,t,m_currency_digits);
      deals_count+=d;
      average=SMTMath::MoneyAdd(average,a,m_currency_digits);
      //--- only complete weeks
      if((i+1)%DAYS_IN_WEEK==0)
        {
         //--- add value
         if((res=series_amount->ValueAddDouble(total_amount))!=MT_RET_OK)
            return(false);
         //--- add value
         if((res=series_count->ValueAddInt(deals_count))!=MT_RET_OK)
            return(false);
         //--- avoid dividing by zero
         if(deals_count>0)
            average/=deals_count;
         //--- add value
         if((res=series_average->ValueAddDouble(average))!=MT_RET_OK)
            return(false);
         //--- reset values
         total_amount=0.0,average=0.0;
         deals_count=0;
        }
     }
//---  add last element
   if(i%DAYS_IN_WEEK!=0)
     {
      //--- add value
      if((res=series_amount->ValueAddDouble(total_amount))!=MT_RET_OK)
         return(false);
      //--- add value
      if((res=series_count->ValueAddInt(deals_count))!=MT_RET_OK)
         return(false);
      //--- avoid dividing by zero
      if(deals_count>0)
         average/=deals_count;
      //--- add value
      if((res=series_average->ValueAddDouble(average))!=MT_RET_OK)
         return(false);
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill Total Graph Series Month By Month                           |
//+------------------------------------------------------------------+
bool CMoneyFlow::FillTotalGraphSeriesMonthByMonth(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportSeries *series_amount,IMTReportSeries *series_count,IMTReportSeries *series_average)const
  {
//--- buffer
   CMTStr32 str;
//--- iterate through days
   for(uint32_t i=0;i<m_months_total;i++)
     {
      //--- convert month index to human readable string
      if(!MonthIndexToStr(i,str))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add value to series
      if((res=header_series->ValueAdd(str.Str()))!=MT_RET_OK)
         return(false);
      //--- values
      double total=0.0,average=0.0;
      uint32_t deals=0;
      //--- get all money by month
      if(!CountryGetTotalMoneyFlowByMonth(direction,i,total,average,deals))
        {
         res=MT_RET_ERROR;
         return(false);
        }
      //--- add value
      if((res=series_amount->ValueAddDouble(total))!=MT_RET_OK)
         return(false);
      //--- add value
      if((res=series_count->ValueAddInt(deals))!=MT_RET_OK)
         return(false);
      //--- avoid dividing by zero
      if(deals>0)
         average/=deals;
      //--- add value
      if((res=series_average->ValueAddDouble(average))!=MT_RET_OK)
         return(false);
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Proceed HTML template                                            |
//+------------------------------------------------------------------+
bool CMoneyFlow::ProceedTemplate(MTAPIRES &res)
  {
//--- common
   MTAPISTR       tag;
   uint32_t       counter                       =0;
   uint32_t       rt_line_counter               =0;
//--- subgroup info at accounts report
   uint32_t       group_index                   =0;
//--- current month index
   uint32_t       current_month_index           =0;
//--- current country month index
   uint32_t       current_country_month_index   =0;
//--- current country index
   uint32_t       current_country_index         =0;
//--- load html template   
   if((res=m_api->HtmlTplLoadResource(IDR_HTML_MONEY_FLOW_REPORT,RT_HTML))!=MT_RET_OK)
      return(false);
//--- loop of process template tags
//--- tags in html have format <mt5:xxx>
//--- HtmlTplNext return tag string and counter,that shows how many times tag has been used
   while(m_api->HtmlTplNext(tag,&counter)==MT_RET_OK)
     {
      //--- write common tags
      if(WriteCommon(tag,res))
        {
         if(res!=MT_RET_OK)
            break;
         continue;
        }
      //--- write summary table
      if(WriteSummaryTable(tag,counter,current_country_index,current_country_month_index,res))
        {
         if(res!=MT_RET_OK)
            break;
         continue;
        }
      //--- revenue table line tag to highlight even lines
      if(CMTStr::CompareNoCase(tag,TAG_RT_LINE)==0)
        {
         if((res=m_api->HtmlWrite(L"%u",(rt_line_counter++)&1))!=MT_RET_OK)
            break;
         continue;
        }
      //--- write revenue table
      if(WriteGroupsRevenueReportTable(tag,counter,res,group_index,current_month_index))
        {
         if(res!=MT_RET_OK)
            break;
         continue;
        }
      //--- proceed template inside TAG_INCOME_EXIST if there are deposits at all
      if(CMTStr::CompareNoCase(tag,TAG_INCOME_EXIST)==0)
        {
         //--- proceed only once
         if(counter>0||!m_isMoneyFlowIn)
            continue;
         if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
            break;
         continue;
        }
      //--- proceed template inside TAG_INCOME_EXIST if there are withdrawals at all
      if(CMTStr::CompareNoCase(tag,TAG_OUTCOME_EXIST)==0)
        {
         //--- proceed only once
         if(counter>0||!m_isMoneyFlowOut)
            continue;
         if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
            break;
         continue;
        }
      //--- pie chart of deposits
      if(CMTStr::CompareNoCase(tag,TAG_PIE_CHART_INCOME)==0)
        {
         //--- check for null
         if(m_pie_chart_in==NULL)
            continue;
         if((res=m_api->ChartWriteHtml(m_pie_chart_in))!=MT_RET_OK)
            break;
         continue;
        }
      //--- pie chart of withdrawals
      if(CMTStr::CompareNoCase(tag,TAG_PIE_CHART_OUTCOME)==0)
        {
         //--- check for null
         if(m_pie_chart_out==NULL)
            continue;
         if((res=m_api->ChartWriteHtml(m_pie_chart_out))!=MT_RET_OK)
            break;
         continue;
        }
      //--- draw total deposits chart 
      if(CMTStr::CompareNoCase(tag,TAG_CHART_MONEY_INCOME_TOTAL)==0)
        {
         //--- check for null
         if(m_chart_money_income_total==NULL)
            continue;
         if((res=m_api->ChartWriteHtml(m_chart_money_income_total))!=MT_RET_OK)
            break;
         continue;
        }
      //--- draw total withdrawals chart 
      if(CMTStr::CompareNoCase(tag,TAG_CHART_MONEY_OUTCOME_TOTAL)==0)
        {
         //--- check for null
         if(m_chart_money_outcome_total==NULL)
            continue;
         if((res=m_api->ChartWriteHtml(m_chart_money_outcome_total))!=MT_RET_OK)
            break;
         continue;
        }
      //--- draw deposits chart 
      if(CMTStr::CompareNoCase(tag,TAG_CHART_MONEY_INCOME)==0)
        {
         //--- check for null
         if(m_chart_money_income==NULL)
            continue;
         if((res=m_api->ChartWriteHtml(m_chart_money_income))!=MT_RET_OK)
            break;
         continue;
        }
      //--- draw withdrawals chart 
      if(CMTStr::CompareNoCase(tag,TAG_CHART_MONEY_OUTCOME)==0)
        {
         //--- check for null
         if(m_chart_money_outcome==NULL)
            continue;
         if((res=m_api->ChartWriteHtml(m_chart_money_outcome))!=MT_RET_OK)
            break;
         continue;
        }
     }//proceed all templates
   return(res==MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Convert time to day index                                        |
//+------------------------------------------------------------------+
bool CMoneyFlow::TimeToDayIndex(const int64_t ctm,uint32_t &day_index)const
  {
//--- checks
   if(ctm<m_param_from)
      return(false);
//--- paranoid check
   if(SECONDS_IN_DAY==0)
      return(false);
//--- compute
   day_index=(uint32_t)(ctm-m_param_from)/SECONDS_IN_DAY;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Month Index to String Time. Format: YYYY/MM                      |
//+------------------------------------------------------------------+
bool CMoneyFlow::MonthIndexToStr(const uint32_t month_index,CMTStr &str)const
  {
   tm ttm={};
//--- checks
   if(month_index>=m_months_total)
      return(true);
//--- parse time
   if(!SMTTime::ParseTime(m_param_from,&ttm))
      return(false);
//--- current month
   const uint32_t month_curr=ttm.tm_year*12+ttm.tm_mon;
//--- format time
   if(str.Format(L"%04d/%02d",((month_curr)+month_index)/12+1900,((month_curr)+month_index)%12+1)<0)
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Convert month index to human readable month and year number      |
//+------------------------------------------------------------------+
bool CMoneyFlow::MonthIndexToMonthYear(const uint32_t month_index,uint32_t &month,uint32_t &year)const
  {
   tm ttm={};
//--- checks
   if(month_index>=m_months_total)
     {
      //--- failed
      month=year=0;
      return(true);
     }
//--- parse time
   if(!SMTTime::ParseTime(m_param_from,&ttm))
      return(false);
//--- current month
   const uint32_t month_curr=(ttm.tm_year*12+ttm.tm_mon);
//--- compute month
   month=((month_curr+month_index)%12+1);
//--- compute year
   year=((month_curr+month_index)/12+1900);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get All Money By Month                                           |
//+------------------------------------------------------------------+
bool CMoneyFlow::CountryGetTotalMoneyFlowByMonth(const uint32_t direction,const uint32_t month_index,double &total_amount,double &average,uint32_t &count)const
  {
//--- checks
   if(direction>=DIRECTION_COUNT||month_index>=m_months_total)
      return(false);
//--- convert month index to human readable month and year
   uint32_t month=0,year=0;
   if(!MonthIndexToMonthYear(month_index,month,year))
      return(false);
//--- reset
   count=0;
   total_amount=0.0;
   average=0.0;
//--- iterate over each country
   for(const CountryMoneyFlow *iter=(const_cast<CountryMoneyFlowArray&>(m_money_flow_by_country)).First();iter!=NULL;iter=m_money_flow_by_country.Next(iter))
     {
      //--- iterate over each day
      for(uint32_t i=0;i<m_days_total;i++)
        {
         //--- check
         if(i>=iter->values_total)
            return(false);
         tm itm={0};
         //--- parse time
         if(!SMTTime::ParseTime(m_param_from+i*SECONDS_IN_DAY,&itm))
            return(false);
         //--- compute for selected month
         if((itm.tm_mon+1)==month&&(itm.tm_year+1900)==year)
           {
            total_amount=SMTMath::MoneyAdd(total_amount,direction==DIRECTION_IN?iter->income_amounts[i]:iter->outcome_amounts[i],m_currency_digits);
            count+=direction==DIRECTION_IN?iter->income_counts[i]:iter->outcome_counts[i];
           }
        }
     }
//--- avoid dividing by zero
   if(count==0)
      return(true);
//--- compute average
   average=total_amount/count;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get Money Flow By Day                                            |
//+------------------------------------------------------------------+
bool CMoneyFlow::CountryGetTotalMoneyFlowByDay(const uint32_t direction,const CountryMoneyFlow &value,const uint32_t day,double &money)const
  {
//--- checks
   if(direction>=DIRECTION_COUNT  ||
      day>=m_days_total ||
      value.income_amounts==NULL ||
      value.outcome_amounts==NULL||
      day>=value.values_total)
      return(false);
//--- compute
   money=direction==DIRECTION_IN?value.income_amounts[day]:value.outcome_amounts[day];
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get All Money By Day                                             |
//+------------------------------------------------------------------+
bool CMoneyFlow::GetTotalMoneyFlowByDay(const uint32_t direction,const uint32_t day,double &total_amount,double &average,uint32_t &count)const
  {
//--- checks
   if(direction>=DIRECTION_COUNT||day>=m_days_total)
      return(false);
//--- reset
   count=0;
   total_amount=0.0;
   average=0.0;
//--- iterate over each country
   for(const CountryMoneyFlow *iter=(const_cast<CountryMoneyFlowArray&>(m_money_flow_by_country)).First();iter!=NULL;iter=m_money_flow_by_country.Next(iter))
     {
      //--- checks
      if(day>=iter->values_total)
         return(false);
      //--- compute
      total_amount  =SMTMath::MoneyAdd(total_amount,direction==DIRECTION_IN?iter->income_amounts[day]:iter->outcome_amounts[day],m_currency_digits);
      count        +=direction==DIRECTION_IN?iter->income_counts[day]:iter->outcome_counts[day];
     }
//--- avoid dividing by zero
   if(count==0)
      return(true);
//--- compute average
   average=total_amount/count;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get money flow by months                                         |
//+------------------------------------------------------------------+
bool CMoneyFlow::CountryGetMoneyFlowByMonth(const CountryMoneyFlow &value,const uint32_t direction,const uint32_t month_index,double &money)const
  {

   uint32_t month     =0;
   uint32_t year      =0;
//--- reset
   money=0;
//--- convert month index to human readable month and year number
   if(!MonthIndexToMonthYear(month_index,month,year))
      return(false);
//--- paranoid check
   if(m_days_total>value.values_total)
      return(false);
//--- iterate over each day
   for(uint32_t i=0;i<m_days_total;i++)
     {
      tm itm={0};
      //--- parse time
      if(!SMTTime::ParseTime(m_param_from+i*SECONDS_IN_DAY,&itm))
         return(false);
      //--- use only exact day number
      if((itm.tm_mon+1)==month&&(itm.tm_year+1900)==year)
        {
         //--- compute
         money=SMTMath::MoneyAdd(money,direction==DIRECTION_IN?value.income_amounts[i]:value.outcome_amounts[i],m_currency_digits);
        }
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get total money flow                                             |
//+------------------------------------------------------------------+
bool CMoneyFlow::GetMoneyFlowTotal(double &income,double &outcome)const
  {
//--- reset
   income=outcome=0.0;
//--- iterate over each country
   for(uint32_t c=0,cc=m_money_flow_by_country.Total();c<cc;c++)
     {
      //--- paranoid check
      if(m_days_total>m_money_flow_by_country[c].values_total)
         return(false);
      //--- iterate over each day
      for(uint32_t i=0;i<m_days_total;i++)
        {
         //--- compute
         income =SMTMath::MoneyAdd(income,  m_money_flow_by_country[c].income_amounts[i], m_currency_digits);
         outcome=SMTMath::MoneyAdd(outcome, m_money_flow_by_country[c].outcome_amounts[i],m_currency_digits);
        }
     }
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Get Revenue By Month                                             |
//+------------------------------------------------------------------+
bool CMoneyFlow::GetRevenueByMonth(const uint32_t group_index,const uint32_t month_index,double &deposits,double &withdrawals,double &delta,uint32_t &deals_count,double &overturn,double &revenue,CMTStr &currency,uint32_t &currency_digits)const
  {
//--- reset
   delta=overturn=revenue=deposits=withdrawals=0.0;
   deals_count=0;
   currency.Clear();
//--- checks
   if(group_index>=m_revenue_by_group.Total() ||
      m_days_total>m_revenue_by_group[group_index].values_count ||
      month_index>=m_months_total)
      return(false);
//--- set currency
   currency.Assign(m_revenue_by_group[group_index].currency);
//--- set digits
   currency_digits=m_revenue_by_group[group_index].currency_digits;
//--- month and year
   uint32_t month =0;
   uint32_t year  =0;
//--- convert month index to human readable month and year number
   if(!MonthIndexToMonthYear(month_index,month,year))
      return(false);
//--- iterate over each day
   for(uint32_t i=0;i<m_days_total;i++)
     {
      tm itm={0};
      //--- parse time
      if(!SMTTime::ParseTime(m_param_from+i*SECONDS_IN_DAY,&itm))
         return(false);
      //--- compute only for selected month index
      if((itm.tm_mon+1)==month&&(itm.tm_year+1900)==year)
        {
         //--- compute
         deals_count +=m_revenue_by_group[group_index].deals_count[i];
         deposits     =SMTMath::MoneyAdd(deposits,    m_revenue_by_group[group_index].money_in[i],      m_revenue_by_group[group_index].currency_digits);
         withdrawals  =SMTMath::MoneyAdd(withdrawals, m_revenue_by_group[group_index].money_out[i],     m_revenue_by_group[group_index].currency_digits);
         overturn     =SMTMath::MoneyAdd(overturn,    m_revenue_by_group[group_index].deals_overturn[i],m_revenue_by_group[group_index].currency_digits);
        }
     }
//--- compute delta
   delta=deposits+withdrawals;
//--- avoid dividing by zero
   if(overturn>0)
      revenue=delta*1000000.0/overturn;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Write common tags                                                |
//| result:true means that tag has been proceeded successfully       |
//+------------------------------------------------------------------+
bool CMoneyFlow::WriteCommon(const MTAPISTR &tag,MTAPIRES &res)
  {
//--- checks
   if(m_api==NULL||m_top_n_countries==NULL||m_currency==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(true);
     }
//--- report name
   if(CMTStr::CompareNoCase(tag,TAG_REPORT_NAME)==0)
     {
      if((res=m_api->HtmlWriteSafe(s_info.name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- server name
   if(CMTStr::CompareNoCase(tag,TAG_SERVER)==0)
     {
      MTReportServerInfo info={};
      //--- get info and write server name
      if(m_api->About(info)==MT_RET_OK)
         res=m_api->HtmlWriteSafe(info.platform_name,IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- from
   if(CMTStr::CompareNoCase(tag,TAG_FROM)==0)
     {
      CMTStr256   str;
      res=m_api->HtmlWriteString(SMTFormat::FormatDateTime(str,m_api->ParamFrom(),false,false));
      return(true);
     }
//--- to
   if(CMTStr::CompareNoCase(tag,TAG_TO)==0)
     {
      CMTStr256   str;
      res=m_api->HtmlWriteString(SMTFormat::FormatDateTime(str,m_api->ParamTo(),false,false));
      return(true);
     }
//--- top_n_countries
   if(CMTStr::CompareNoCase(tag,TAG_TOP_N_COUNTRIES)==0)
     {
      res=m_api->HtmlWrite(L"%I64d",m_top_n_countries->ValueInt());
      return(true);
     }
//--- common_currency
   if(CMTStr::CompareNoCase(tag,TAG_COMMON_CURRENCY)==0)
     {
      res=m_api->HtmlWriteString(m_currency->Value());
      return(true);
     }
//--- continue scanning
   return(false);
  }
//+------------------------------------------------------------------+
//| Write Groups Revenue Report Table                                |
//+------------------------------------------------------------------+
bool CMoneyFlow::WriteGroupsRevenueReportTable(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,uint32_t &group_index,uint32_t &month_index)
  {
   uint32_t deals_count,digits;
   double deposits,withdrawals,delta,overturn,revenue;
   CMTStr256 str;
//--- checks
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(true);
     }
//--- revenue table
   if(CMTStr::CompareNoCase(tag,TAG_REVENUE_TABLE)==0)
     {
      //--- proceed once
      if(counter==0)
        {
         //m_revenue_by_group.Sort(SortGroupsByHash);
         res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
         return(true);
        }
      return(true);
     }
//--- groups counter
   if(CMTStr::CompareNoCase(tag,TAG_RT_GROUP_COUNTER)==0)
     {
      //--- stop if need
      if(counter>=m_revenue_by_group.Total())
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- save current record index
      group_index=counter;
      //--- process tags
      if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- months counter
   if(CMTStr::CompareNoCase(tag,TAG_RT_MONTH_COUNTER)==0)
     {
      //--- stop if need
      if(counter>=m_months_total)
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- save current record index
      month_index=counter;
      //--- process tags
      if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- separator
   if(CMTStr::CompareNoCase(tag,TAG_RT_SEP)==0)
     {
      //--- smartly write separator
      if(group_index==0)
         return(true);
      if(month_index!=0)
         return(true);
      //--- process tags
      if((res=m_api->HtmlWriteSafe(L"sep",IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- separator with space after
   if(CMTStr::CompareNoCase(tag,TAG_RT_SEP_S)==0)
     {
      //--- smartly write separator
      if(group_index==0)
         return(true);
      if(month_index!=0)
         return(true);
      //--- process tags
      if((res=m_api->HtmlWriteSafe(L"sep ",IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }//--- name
   if(CMTStr::CompareNoCase(tag,TAG_RT_NAME)==0)
     {
      //--- stop if need
      if(group_index>=m_revenue_by_group.Total())
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- checks
      if(group_index>=UINT_MAX)
        {
         if((res=m_api->LoggerOutString(MTLogErr,L"too big group index number"))!=MT_RET_OK)
            return(true);
         res=MT_RET_ERROR;
         return(true);
        }
      //--- write only first line with group name
      if(month_index==0)
        {
         if((res=m_api->HtmlWriteString(m_revenue_by_group[group_index].group))!=MT_RET_OK)
            return(true);
        }
      return(true);
     }
//--- date
   if(CMTStr::CompareNoCase(tag,TAG_RT_DATE)==0)
     {
      //--- convert month index to human readable string
      if(!MonthIndexToStr(month_index,str))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- deposits
   if(CMTStr::CompareNoCase(tag,TAG_RT_DEPOSITS)==0)
     {
      //--- get revenue (and some more) by month
      if(!GetRevenueByMonth(group_index,month_index,deposits,withdrawals,delta,deals_count,overturn,revenue,str,digits))
        {
         res=MT_RET_OK;
         return(true);
        }
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,deposits,digits)))!=MT_RET_OK)
         return(true);

      return(true);
     }
//--- withdrawals
   if(CMTStr::CompareNoCase(tag,TAG_RT_WITHDRAWALS)==0)
     {
      //--- get revenue (and some more) by month
      if(!GetRevenueByMonth(group_index,month_index,deposits,withdrawals,delta,deals_count,overturn,revenue,str,digits))
        {
         res=MT_RET_OK;
         return(true);
        }
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,withdrawals,digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- delta
   if(CMTStr::CompareNoCase(tag,TAG_RT_DELTA)==0)
     {
      //--- get revenue (and some more) by month
      if(!GetRevenueByMonth(group_index,month_index,deposits,withdrawals,delta,deals_count,overturn,revenue,str,digits))
        {
         res=MT_RET_OK;
         return(true);
        }
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,delta,digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- deals count
   if(CMTStr::CompareNoCase(tag,TAG_RT_COUNT)==0)
     {
      //--- get revenue (and some more) by month
      if(!GetRevenueByMonth(group_index,month_index,deposits,withdrawals,delta,deals_count,overturn,revenue,str,digits))
        {
         res=MT_RET_OK;
         return(true);
        }
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,(double)deals_count,0)))!=MT_RET_OK)
         return(true);

      return(true);
     }
//--- overturn
   if(CMTStr::CompareNoCase(tag,TAG_RT_TURNOVER)==0)
     {
      //--- get revenue (and some more) by month
      if(!GetRevenueByMonth(group_index,month_index,deposits,withdrawals,delta,deals_count,overturn,revenue,str,digits))
        {
         res=MT_RET_OK;
         return(true);
        }
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,overturn,digits)))!=MT_RET_OK)
         return(true);

      return(true);
     }
//--- pure revenue
   if(CMTStr::CompareNoCase(tag,TAG_RT_REVENUE)==0)
     {
      //--- get revenue (and some more) by month
      if(!GetRevenueByMonth(group_index,month_index,deposits,withdrawals,delta,deals_count,overturn,revenue,str,digits))
        {
         res=MT_RET_OK;
         return(true);
        }
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,revenue,digits)))!=MT_RET_OK)
         return(true);

      return(true);
     }
//--- currency
   if(CMTStr::CompareNoCase(tag,TAG_RT_CURRENCY)==0)
     {
      if(group_index>=(m_revenue_by_group.Total()))
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- checks
      if(group_index>=UINT_MAX)
        {
         if((res=m_api->LoggerOutString(MTLogErr,L"too big group index number"))!=MT_RET_OK)
            return(true);
         res=MT_RET_ERROR;
         return(true);
        }
      if((res=m_api->HtmlWriteSafe(m_revenue_by_group[group_index].currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- continue
   return(false);
  }
//+------------------------------------------------------------------+
//| Write Summary Table                                              |
//+------------------------------------------------------------------+
bool CMoneyFlow::WriteSummaryTable(MTAPISTR tag,uint32_t counter,uint32_t &current_country_index,uint32_t &current_month_index,MTAPIRES &res)
  {
//--- checks
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(true);
     }
//--- TAG_TBL_SUMMARY
   if(CMTStr::CompareNoCase(tag,TAG_TBL_SUMMARY)==0)
     {
      //--- proceed once
      if(counter==0)
        {
         m_money_flow_by_country.Sort(SortCountriesByTotal);
         res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
         return(true);
        }
      return(true);
     }
//--- line position
   if(CMTStr::CompareNoCase(tag,TAG_TBL_COUNTRY_LINE)==0)
     {
      res=m_api->HtmlWrite(L"%d",current_country_index&1);
      return(true);
     }
//--- skip empty columns at the beginning of the table
   if(CMTStr::CompareNoCase(tag,TAG_TBL_MONTH_EMPTY_COUNTER)==0)
     {
      if(counter>=(MAX_MONTHS_TO_SHOW - std::min((uint32_t)MAX_MONTHS_TO_SHOW,m_months_total)))
        {
         res=MT_RET_OK;
         return(true);
        }
      res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- write only last n columns if its count bigger than some number
   if(CMTStr::CompareNoCase(tag,TAG_TBL_MONTHS_COUNTER)==0)
     {
      if(counter>=std::min((uint32_t)MAX_MONTHS_TO_SHOW,m_months_total))
        {
         res=MT_RET_OK;
         return(true);
        }
      if(m_months_total>MAX_MONTHS_TO_SHOW)
         current_month_index=(counter+(m_months_total-MAX_MONTHS_TO_SHOW));
      else
         current_month_index=counter;
      res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- month name
   if(CMTStr::CompareNoCase(tag,TAG_TBL_MONTH_NAME)==0)
     {
      CMTStr256 str;
      //--- checks
      if(!MonthIndexToStr(current_month_index,str))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      if((res=m_api->HtmlWriteSafe(str.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- countries counter
   if(CMTStr::CompareNoCase(tag,TAG_TBL_COUNTRIES_COUNTER)==0)
     {
      if(counter>=m_countries_to_show)
        {
         res=MT_RET_OK;
         return(true);
        }
      current_country_index=counter;
      res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- country name
   if(CMTStr::CompareNoCase(tag,TAG_TBL_COUNTRY_NAME)==0)
     {
      if(current_country_index>=(m_money_flow_by_country.Total()))
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- checks
      if(current_country_index>=UINT_MAX)
        {
         if((res=m_api->LoggerOutString(MTLogErr,L"too big country index number"))!=MT_RET_OK)
            return(true);
         res=MT_RET_ERROR;
         return(true);
        }
      if((res=m_api->HtmlWriteSafe(m_money_flow_by_country[current_country_index].country,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- month deposits
   if(CMTStr::CompareNoCase(tag,TAG_TBL_MONTH_INCOME_VALUE)==0)
     {
      if(current_country_index>=(m_money_flow_by_country.Total()))
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- checks
      if(current_country_index>=UINT_MAX)
        {
         if((res=m_api->LoggerOutString(MTLogErr,L"too big country index number"))!=MT_RET_OK)
            return(true);
         res=MT_RET_ERROR;
         return(true);
        }
      double money=0.0;
      if(!CountryGetMoneyFlowByMonth(m_money_flow_by_country[current_country_index],DIRECTION_IN,current_month_index,money))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      CMTStr64 str;
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,money,m_currency_digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
//--- month withdrawals
   if(CMTStr::CompareNoCase(tag,TAG_TBL_MONTH_OUTCOME_VALUE)==0)
     {
      if(current_country_index>=(m_money_flow_by_country.Total()))
        {
         res=MT_RET_OK;
         return(true);
        }
      //--- checks
      if(current_country_index>=UINT_MAX)
        {
         if((res=m_api->LoggerOutString(MTLogErr,L"too big country index number"))!=MT_RET_OK)
            return(true);
         res=MT_RET_ERROR;
         return(true);
        }
      double money=0.0;
      if(!CountryGetMoneyFlowByMonth(m_money_flow_by_country[current_country_index],DIRECTION_OUT,current_month_index,money))
        {
         res=MT_RET_ERROR;
         return(true);
        }
      CMTStr64 str;
      if((res=m_api->HtmlWriteString(SMTFormat::FormatMoney(str,money,m_currency_digits)))!=MT_RET_OK)
         return(true);
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Sort countries by hash                                           |
//+------------------------------------------------------------------+
int32_t CMoneyFlow::SortCountriesByName(const void *left,const void *right)
  {
   CountryMoneyFlow *lft=(CountryMoneyFlow*)left;
   CountryMoneyFlow *rgh=(CountryMoneyFlow*)right;
   return(CMTStr::Compare(lft->country,rgh->country));
  }
//+------------------------------------------------------------------+
//| Sort countries by deposits amount                                  |
//+------------------------------------------------------------------+
int32_t CMoneyFlow::SortCountriesByIncome(const void *left,const void *right)
  {
   CountryMoneyFlow *lft=(CountryMoneyFlow*)left;
   CountryMoneyFlow *rgh=(CountryMoneyFlow*)right;
   if(lft->total_income_amount>rgh->total_income_amount)
      return(-1);
   if(lft->total_income_amount<rgh->total_income_amount)
      return(1);
//--- stable sort
   return(SortCountriesByName(left,right));
  }
//+------------------------------------------------------------------+
//| Sort countries by withdrawals amount                                 |
//+------------------------------------------------------------------+
int32_t CMoneyFlow::SortCountriesByOutcome(const void *left,const void *right)
  {
   CountryMoneyFlow *lft=(CountryMoneyFlow*)left;
   CountryMoneyFlow *rgh=(CountryMoneyFlow*)right;
//--- reverse direction because negative values
   if(lft->total_outcome_amount<rgh->total_outcome_amount)
      return(-1);
   if(lft->total_outcome_amount>rgh->total_outcome_amount)
      return(1);
//--- stable sort
   return(SortCountriesByName(left,right));
  }
//+------------------------------------------------------------------+
//| Sort countries by total money flow                               |
//+------------------------------------------------------------------+
int32_t CMoneyFlow::SortCountriesByTotal(const void *left,const void *right)
  {
   CountryMoneyFlow *lft=(CountryMoneyFlow*)left;
   CountryMoneyFlow *rgh=(CountryMoneyFlow*)right;
   if(lft->total_income_amount+lft->total_outcome_amount>rgh->total_income_amount+rgh->total_outcome_amount)
      return(-1);
   if(lft->total_income_amount+lft->total_outcome_amount<rgh->total_income_amount+rgh->total_outcome_amount)
      return(1);
//--- stable sort
   return(SortCountriesByName(left,right));
  }
//+------------------------------------------------------------------+
//| Sort groups by hash                                              |
//+------------------------------------------------------------------+
int32_t CMoneyFlow::SortGroupsByName(const void *left,const void *right)
  {
   GroupOverturn *lft=(GroupOverturn*)left;
   GroupOverturn *rgh=(GroupOverturn*)right;
   return(CMTStr::Compare(lft->group,rgh->group));
  }
//+------------------------------------------------------------------+
