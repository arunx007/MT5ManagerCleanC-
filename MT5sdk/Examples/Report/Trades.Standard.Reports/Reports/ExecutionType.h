//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#define DEFAULT_CURRENCY L"USD"
//+------------------------------------------------------------------+
//| Execution Type Report                                            |
//+------------------------------------------------------------------+
class CExecutionType : public IMTReportContext
  {
private:
   //--- constants
   enum EnConstants
     {
      REASONS_COUNT        =IMTDeal::DEAL_REASON_LAST+1,
      DAYS_IN_WEEK         =7,
     };
   //--- order of groups to show in report
   enum EnGroupsOrder
     {
      GROUP_REAL,
      GROUP_DEMO,
      GROUP_CONTEST,
      GROUP_COVERAGE,
      GROUP_PRELIMINARY,
      GROUP_COUNT,
     };
   //--- type of chart
   enum EnChartType
     {
      CHART_DEALS_COUNT,   // deals count
      CHART_DEALS_VOLUME,  // deals volume
      CHART_COUNT
     };
   //--- intervals to make all graphs smoother
   enum EnInterval
     {
      INTERVAL_DAY,
      INTERVAL_WEEK,
      INTERVAL_MONTH,
      INTERVAL_COUNT
     };
   //--- group info
   struct GroupRecord
     {
      wchar_t           group[64];        // group name
      double            balance;          // balance
      double            equity;           // equity
      double            profit;           // profit
      wchar_t           currency[64];     // currency
      uint32_t          accounts;         // accounts
      uint32_t          active;           // active accounts for the last 3 months
      BYTE              currency_digits;  // currency digits
     };
   //--- typedef
   typedef TMTArray<GroupRecord> GroupRecordArray;
   //--- dataset records
#pragma pack(push,1)
   //--- group statistic record
   struct GroupStatRecord
     {
      wchar_t           group[64];        // group name
      uint32_t          accounts;         // accounts
      double            active;           // active accounts for the last 3 months
      double            balance;          // balance
      double            profit;           // profit
      double            equity;           // equity
      wchar_t           currency[64];     // currency
      BYTE              currency_digits;  // currency digits
     };
   //--- Deal info
   struct DealInfo
     {
      uint32_t          count;
      double            volume;
     };
   //--- Deal record
   struct DealRecord
     {
      int64_t           date;
      DealInfo          deals[REASONS_COUNT];
     };
#pragma pack(pop)

private:
   IMTReportAPI     *m_api;               // report api
   IMTConParam      *m_currency;          // currency for converting
   uint32_t          m_currency_digits;   // currency digits
   //--- report parameters
   int64_t           m_param_from;        // from
   int64_t           m_param_to;          // to
   //--- 
   uint32_t          m_days_total;        // total months in report to show
   uint32_t          m_months_total;      // total days in report to show
   //--- groups
   GroupRecordArray  m_groups;            // groups (current group type)
   GroupRecordArray  m_groups_totals;     // totals for groups by currency(current group type)
   //--- deals info
   uint32_t         *m_deals_count[GROUP_COUNT][REASONS_COUNT]; //deals count grouped by group_name and by reason
   DOUBLE           *m_deals_volume[GROUP_COUNT][REASONS_COUNT];//deals volume grouped by group_name and by reason
   //--- charts
   IMTReportChart   *m_chart_deals_count[GROUP_COUNT];
   IMTReportChart   *m_chart_deals_volume[GROUP_COUNT];
   //--- static data
   static MTReportInfo s_info;           // report info
   static ReportColumn s_columns[];      // column descriptions   
   static uint32_t   s_reason_colors[REASONS_COUNT];
   static uint32_t   s_reasons_chart[];      // chart reasons
   static uint32_t   s_reasons_table[];      // table reasons
   static ReportColumn s_columns_group[];    // column description group
   static ReportColumn s_columns_deals[];    // column description deals
   static ReportColumn s_columns_deal[];     // column description deal

public:
   //--- constructor/destructor
                     CExecutionType(void);
   virtual          ~CExecutionType(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- release
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- allocate memory for internal data
   bool              AllocateMemory(void);
   //--- clear
   void              Clear(void);
   //--- request currency from report's configuration
   MTAPIRES          GetCurency(void);
   //--- prepare everything before drawing
   MTAPIRES          CalculateStatictics(void);
   //--- calculate about a group
   MTAPIRES          CalculateGroup(const IMTConGroup *group,IMTDealArray *deals,uint64_t &deals_processed);
   //--- calculate about a login
   MTAPIRES          CalculateLogin(const uint64_t login,const IMTConGroup *group,DOUBLE rate,IMTDealArray *deals,uint64_t &deals_processed);
   //--- prepare table for group type
   MTAPIRES          PrepareTableGroupType(const uint32_t type);
   //--- prepare chart for specified group type, interval and type
   MTAPIRES          PrepareGraph(const uint32_t group_type,const uint32_t interval,const uint32_t chart_type,IMTReportChart *&chart);
   //--- add group to list with sorting
   MTAPIRES          PrepareGroupAdd(const GroupRecord &group);
   //--- write header tags
   bool              WriteHeader(const MTAPISTR &tag,MTAPIRES &res);
   //--- iterate over all group types
   bool              WriteGroupType(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,uint32_t &current_group,CMTStr256 &current_group_name,uint32_t &current_deals_count,DOUBLE &current_deals_volume);
   //--- write report about each subgroup
   bool              WriteGroupsReport(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,GroupRecord &current_subgroup,uint32_t &current_subgroup_index);
   //--- write table of deals
   bool              WriteDealsTable(const MTAPISTR &tag,const uint32_t counter,MTAPIRES &res,const uint32_t current_group,const uint32_t current_deals_count,uint32_t &current_month_index);
   //--- proceed deals count tag
   bool              PreceedDealsCountTag(const MTAPISTR &tag,LPCWSTR match,const uint32_t current_group,const uint32_t reason,const uint32_t current_month_index,bool compute_total,MTAPIRES &res);
   //--- convert day index to human readable string
   bool              DayIndexToStr(const uint32_t i,CMTStr256 &str)const;
   //--- convert time to number of day starting from some value
   bool              TimeToDayIndex(const int64_t ctm,int64_t &index)const;
   //--- convert month index to representable string
   bool              MonthIndexToStr(const uint32_t index,CMTStr &str)const;
   //--- convert month index to month and year number
   bool              MonthIndexToMonthYear(const uint32_t index,uint32_t &month,uint32_t &year)const;
   //--- get group type by name
   bool              GetGroupType(LPCWSTR group,uint32_t &type)const;
   //--- get group name
   bool              GetGroupTypeName(const uint32_t type,CMTStr &name)const;
   //--- get reason name by type
   bool              GetReasonTypeName(const uint32_t reason,CMTStr &str)const;
   //--- get deals by day
   bool              GetDealsByDay(const uint32_t group_type,const uint32_t day_index,uint32_t &deals,DOUBLE &volume)const;
   //--- get all deals
   bool              GetDealsTotal(const uint32_t group_type,uint32_t &deals,DOUBLE &volume)const;
   //--- get all deals by type
   bool              GetDealsTotalByType(const uint32_t group_type,const uint32_t reason_type,uint32_t &deals,DOUBLE &volume)const;
   //--- get deals at specified month
   bool              GetDealsByMonth(const uint32_t group_type,const uint32_t reason_type,const uint32_t month_index,uint32_t &deals,DOUBLE &volume)const;
   //--- get all deals at specified month
   bool              GetAllDealsByMonth(const uint32_t group_type,const uint32_t month_index,uint32_t &deals,DOUBLE &volume)const;
   //--- generate dashboard report
   MTAPIRES          GenerateDashboard(const uint32_t interval);
   //--- generate dashboard table groups
   MTAPIRES          AddTableGroups(uint32_t group_type,const CMTStr &group_type_name);
   //--- generate dashboard deals data
   IMTDataset*       DealsData(MTAPIRES &res,const uint32_t group_type,const uint32_t interval,const uint32_t *reasons,const uint32_t reasons_total,const bool column_name) const;
   //--- fill deals data day by day
   MTAPIRES          FillDealDataDay(IMTDataset &data,const uint32_t group_type,const uint32_t *reasons,const uint32_t reasons_total) const;
   //--- fill deals data week by week
   MTAPIRES          FillDealDataWeek(IMTDataset &data,const uint32_t group_type,const uint32_t *reasons,const uint32_t reasons_total) const;
   //--- fill deals data month by month
   MTAPIRES          FillDealDataMonth(IMTDataset &data,const uint32_t group_type,const uint32_t *reasons,const uint32_t reasons_total) const;
   //--- prepare deal graph
   MTAPIRES          PrepareDealGraph(const CMTStr &group_type_name,LPCWSTR title,uint32_t column_id,IMTDataset *data);
   //--- prepare dashboard data summary
   static MTAPIRES   PrepareDataSummary(IMTDatasetSummary &summary,uint32_t column_id,uint32_t line);
   //--- add dashboard data summary uint
   static MTAPIRES   DataSummaryAddUInt(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,uint64_t value);
   //--- add dashboard data summary double
   static MTAPIRES   DataSummaryAddDouble(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,double value);
   //--- add dashboard data summary money
   static MTAPIRES   DataSummaryAddMoney(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,double money,BYTE currency_digits);
   //--- add dashboard data summary string
   static MTAPIRES   DataSummaryAddString(IMTDataset &data,IMTDatasetSummary *summary,uint32_t column_id,uint32_t line,LPCWSTR str);
   //--- sort functions
   static int32_t    SortGroupByCurrency(const void *left,const void *right);
   static int32_t    SortGroupByName(const void *left,const void *right);
   static int32_t    SortGroupByAccounts(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
