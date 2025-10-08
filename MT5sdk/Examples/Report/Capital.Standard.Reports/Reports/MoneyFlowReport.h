//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Default values                                                   |
//+------------------------------------------------------------------+
#define DEFAULT_CURRENCY                  L"USD"      // default: common currency
#ifdef _DEBUG
#define DEFAULT_GROUPS                    L"*"        // default: groups to use
#else
#define DEFAULT_GROUPS                    L"*,!demo*,!contest*"
#endif
#define DEFAULT_COUNTRIES_TO_SHOW         L"10"       // default: max countries to show in money flow summary table
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define PARAMETER_CURRENCY                L"Currency"
#define PARAMETER_COUNTRIES_LIST_SIZE     L"Countries To Show"
//+------------------------------------------------------------------+
//| Common                                                           |
//+------------------------------------------------------------------+
#define COUNTRY_EMPTY_VALUE               L"Unknown"
#define DEPOSITS_KEYWORD                  L"Deposits"
#define WITHDRAWALS_KEYWORD               L"Withdrawals"
#define GRAPH_TOTAL_POSTFIX               L" (all countries)"
#define GRAPH_BY_COUNTY_POSTFIX           L" Proportion (by country)"
//+------------------------------------------------------------------+
//| HTML Template Tags                                               |
//+------------------------------------------------------------------+
//--- common tags
#define TAG_REPORT_NAME                   L"report_name"
#define TAG_SERVER                        L"server"
#define TAG_FROM                          L"from"
#define TAG_TO                            L"to"
#define TAG_TOP_N_COUNTRIES               L"top_n_countries"
#define TAG_COMMON_CURRENCY               L"common_currency"
//--- condition checker
#define TAG_INCOME_EXIST                  L"income_exist"
#define TAG_OUTCOME_EXIST                 L"outcome_exist"
//--- pie charts 
#define TAG_PIE_CHART_INCOME              L"pie_chart_income"
#define TAG_PIE_CHART_OUTCOME             L"pie_chart_outcome"
//--- summary table
#define TAG_TBL_SUMMARY                   L"summary_table"
#define TAG_TBL_COUNTRY_LINE              L"st_country_line"
#define TAG_TBL_MONTH_EMPTY_COUNTER       L"st_months_empty_counter"
#define TAG_TBL_MONTHS_COUNTER            L"st_months_counter"
#define TAG_TBL_MONTH_NAME                L"st_month_name"
#define TAG_TBL_COUNTRIES_COUNTER         L"st_countries_counter"
#define TAG_TBL_COUNTRY_NAME              L"st_country"
#define TAG_TBL_MONTH_INCOME_VALUE        L"st_month_income_value"
#define TAG_TBL_MONTH_OUTCOME_VALUE       L"st_month_outcome_value"
//--- charts
#define TAG_CHART_MONEY_INCOME            L"chart_money_income"
#define TAG_CHART_MONEY_OUTCOME           L"chart_money_outcome"
#define TAG_CHART_MONEY_INCOME_TOTAL      L"chart_money_income_total"
#define TAG_CHART_MONEY_OUTCOME_TOTAL     L"chart_money_outcome_total"
//--- revenue_table
#define TAG_REVENUE_TABLE                 L"revenue_table"
#define TAG_RT_LINE                       L"rt_line"
#define TAG_RT_GROUP_COUNTER              L"rt_group_counter"
#define TAG_RT_MONTH_COUNTER              L"rt_month_counter"
#define TAG_RT_NAME                       L"rt_name"
#define TAG_RT_DATE                       L"rt_date"
#define TAG_RT_DEPOSITS                   L"rt_deposits"
#define TAG_RT_WITHDRAWALS                L"rt_withdrawals"
#define TAG_RT_DELTA                      L"rt_delta"
#define TAG_RT_COUNT                      L"rt_count"
#define TAG_RT_TURNOVER                   L"rt_turnover"
#define TAG_RT_REVENUE                    L"rt_revenue"
#define TAG_RT_CURRENCY                   L"rt_currency"
#define TAG_RT_SEP                        L"rt_sep"
#define TAG_RT_SEP_S                      L"rt_sep_s"
//+------------------------------------------------------------------+
//| History Deals Report                                             |
//+------------------------------------------------------------------+
class CMoneyFlow : public IMTReportContext
  {
private:
   //--- constants
   enum EnConstants
     {
      DAYS_IN_WEEK         =7,                        // number of days in a week
      MAX_COUNTRY_LENGTH   =64,                       // max length of country name
      MAX_MONTHS_TO_SHOW   =12,                       // default: max months to show in graphs and tables
      MAX_GROUP_LENGTH     =64,                       // max length of group name
      MAX_CURRENCY_LENGTH  =12,                       // max length of currency name
      SMOOTH_RANGE_ONE     =30,                       // range between 0..SMOOTH_RANGE_ONE will be drawn day by day
      SMOOTH_RANGE_TWO     =60,                       // range between SMOOTH_RANGE_ONE..SMOOTH_RANGE_TWO will be drawn week by week. After that - month by month
     };
   //--- money flow directions
   enum EnMoneyDirection
     {
      DIRECTION_IN,                                   // deposits
      DIRECTION_OUT,                                  // withdrawals
      DIRECTION_COUNT                                 // count of enumerable
     };
   //--- intervals for interpolation
   enum EnInterval
     {
      INTERVAL_DAY,                                   // display day by day
      INTERVAL_WEEK,                                  // display week by week
      INTERVAL_MONTH,                                 // display month by month
      INTERVAL_COUNT                                  // count of enumerable
     };
   //--- structure to store money flow for each country
   struct CountryMoneyFlow
     {
      WCHAR             country[MAX_COUNTRY_LENGTH];  // country name
      double            total_income_amount;          // total deposits amount at whole period
      double            total_outcome_amount;         // total withdrawals amount at whole period
      uint32_t          values_total;                 // number of elements in each array below
      double           *income_amounts;               // deposits values array
      double           *outcome_amounts;              // withdrawals values array
      uint32_t         *income_counts;                // deposits transactions count
      uint32_t         *outcome_counts;               // withdrawals transactions count
     };
   //--- structure to store money flow and trade overturn for each group
   struct GroupOverturn
     {
      WCHAR             group[MAX_GROUP_LENGTH];      // group name
      WCHAR             currency[MAX_CURRENCY_LENGTH];// group currency
      uint32_t          currency_digits;              // group currency digits
      uint32_t          values_count;                 // number of elements in each array below
      double           *money_in;                     // balance operations: in (in group currency)
      double           *money_out;                    // balance operations: out (in group currency)
      uint32_t         *deals_count;                  // deals count
      double           *deals_overturn;               // deals overturn (in group currency)
     };
   //--- typedefs
   typedef TMTArray<CountryMoneyFlow> CountryMoneyFlowArray;
   typedef TMTArray<GroupOverturn> GroupOverturnArray;

private:
   IMTReportAPI     *m_api;                           // report api
   IMTConReport     *m_report;
   IMTConParam      *m_currency;                      // common currency for converting
   uint32_t          m_currency_digits;               // common currency digits
   IMTConParam      *m_top_n_countries;               // how many countries be shown in top list
   //--- contexts
   IMTConGroup      *m_group;
   IMTDealArray     *m_deals;
   IMTUser          *m_user;
   IMTConSymbol     *m_symbol;
   //--- report parameters
   int64_t           m_param_from;                    // from
   int64_t           m_param_to;                      // to
   //--- interval to get smoother
   uint32_t          m_interval;
   //--- flags that deposits and withdrawals are
   bool              m_isMoneyFlowIn;
   bool              m_isMoneyFlowOut;
   //--- boundaries
   uint32_t          m_days_total;                    // total months in report to show
   uint32_t          m_months_total;                  // total days in report to show
   uint32_t          m_countries_to_show;             // max number of countries to show
   //--- arrays for data
   CountryMoneyFlowArray m_money_flow_by_country;     // money flow by country
   GroupOverturnArray m_revenue_by_group;             // revenue by country
   //--- charts
   IMTReportChart   *m_pie_chart_in;                  // pie chart for deposits
   IMTReportChart   *m_pie_chart_out;                 // pie chart for withdrawals
   IMTReportChart   *m_chart_money_income_total;      // chart for total money deposits
   IMTReportChart   *m_chart_money_income;            // chart for money deposits per each country
   IMTReportChart   *m_chart_money_outcome_total;     // chart for total money withdrawals
   IMTReportChart   *m_chart_money_outcome;           // chart for money withdrawals per each country
   //--- contexts
   IMTReportSeries  *m_series_header;
   IMTReportSeries  *m_series_amount;
   IMTReportSeries  *m_series_count;
   IMTReportSeries  *m_series_average;
   //--- static data
   static MTReportInfo s_info;                        // static report info

public:
   //--- constructor/destructor
                     CMoneyFlow(void);
   virtual          ~CMoneyFlow(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- release
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- clear
   void              Clear(void);
   //--- request data from server and compute statistics
   bool              Calculate(MTAPIRES &res);
   //--- proceed one group
   bool              ProceedGroup(const IMTConGroup *group,MTAPIRES &res, uint64_t &deals_processed);
   //--- proceed one login
   bool              ProceedLogin( const uint64_t login, const IMTConGroup *group,MTAPIRES &res, uint64_t &deals_processed );
   //--- add deal (trade and balance) to specified group's statistics
   bool              GroupDealAdd(LPCWSTR group,LPCWSTR currency,const uint32_t day,const double balance,const double trade_overturn);
   //--- add balance deal to specified country's statistics
   bool              CountryAddMoneyFlow(const uint32_t direction,LPCWSTR country,const uint32_t day,const double money);
   //--- prepare all graphs to be shown
   bool              PrepareGraphs(MTAPIRES &res);
   //--- request currency from report's configuration
   bool              Prepare(MTAPIRES &res);
   //--- prepare pie chart
   bool              PreparePieChart(const uint32_t direction,MTAPIRES &res,IMTReportChart *&chart);
   //--- prepare proportion money graph by country
   bool              PrepareProportionGraph(const uint32_t direction,const uint32_t interval,MTAPIRES &res,IMTReportChart *&chart);
   //--- fill graphs based on interval
   bool              FillProportionGraphDayByDay( const uint32_t direction,MTAPIRES &res,IMTReportSeries *header,IMTReportChart *&chart);
   bool              FillProportionGraphWeekByWeek( const uint32_t direction,MTAPIRES &res,IMTReportSeries *header,IMTReportChart *&chart);
   bool              FillProportionGraphMonthByMonth( const uint32_t direction,MTAPIRES &res,IMTReportSeries *header,IMTReportChart *&chart);
   //--- prepare total money graph
   bool              PrepareTotalGraph(const uint32_t direction,const uint32_t interval,MTAPIRES &res,IMTReportChart *&chart);
   //--- fill graphs based on interval
   bool              FillTotalGraphSeriesDayByDay(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportSeries *series_amount,IMTReportSeries *series_count,IMTReportSeries *series_average)const;
   bool              FillTotalGraphSeriesWeekByWeek(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportSeries *series_amount,IMTReportSeries *series_count,IMTReportSeries *series_average)const;
   bool              FillTotalGraphSeriesMonthByMonth(const uint32_t direction,MTAPIRES &res,IMTReportSeries *header_series,IMTReportSeries *series_amount,IMTReportSeries *series_count,IMTReportSeries *series_average)const;
   //--- proceed html template
   bool              ProceedTemplate(MTAPIRES &res);
   //--- convert time to number of day starting from some value
   bool              TimeToDayIndex(const int64_t ctm,uint32_t &day_index)const;
   //--- convert month index to representable string
   bool              MonthIndexToStr(const uint32_t month_index,CMTStr &str)const;
   //--- convert month index to month and year number
   bool              MonthIndexToMonthYear(const uint32_t month_index,uint32_t &month,uint32_t &year)const;
   //--- select money flow based on parameters
   bool              CountryGetMoneyFlowByMonth(const CountryMoneyFlow &value,const uint32_t direction,const uint32_t month_index,double &money)const;
   //--- get total money flow for a direction, day index and country
   bool              CountryGetTotalMoneyFlowByDay(const uint32_t direction,const CountryMoneyFlow &value,const uint32_t day,double &money)const;
   //--- get total money flow for a direction an day index
   bool              GetTotalMoneyFlowByDay(const uint32_t direction,const uint32_t day,double &total_amount,double &average,uint32_t &count)const;
   //--- get total money flow by country for a direction an day index
   bool              CountryGetTotalMoneyFlowByMonth(const uint32_t direction,const uint32_t month,double &total_amount,double &average,uint32_t &count)const;
   //--- get total money flow
   bool              GetMoneyFlowTotal(double &income,double &outcome)const;
   //--- get revenue by month
   bool              GetRevenueByMonth(const uint32_t group_index,const uint32_t month_index,double &deposits,double &withdrawals,double &delta,uint32_t &deals_count,double &overturn,double &revenue,CMTStr &currency,uint32_t &currency_digits)const;
   //--- write header tags
   bool              WriteCommon(const MTAPISTR &tag,MTAPIRES &res);
   //--- write report about each subgroup
   bool              WriteGroupsRevenueReportTable(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,uint32_t &group_index,uint32_t &current_month_index);
   //--- write summary table
   bool              WriteSummaryTable(MTAPISTR tag,uint32_t counter,uint32_t &current_country_index,uint32_t &current_month_index,MTAPIRES &res);
   //--- sort functions for countries
   static int32_t    SortCountriesByName(const void *left,const void *right);
   static int32_t    SortCountriesByIncome(const void *left,const void *right);
   static int32_t    SortCountriesByOutcome(const void *left,const void *right);
   static int32_t    SortCountriesByTotal(const void *left,const void *right);
   //--- sort functions for groups
   static int32_t    SortGroupsByName(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
