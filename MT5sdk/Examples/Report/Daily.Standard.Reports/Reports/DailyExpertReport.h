//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Daily Expert Report                                              |
//+------------------------------------------------------------------+
class CDailyExpertReport : public IMTReportContext
  {
private:
   enum constants
     {
      REPORT_PERIOD=7,                                // report's days period 
      TOP_NUMBER   =10,                               // total in the top-table
     };
   //--- information about user's special activity
   struct DailyInfo
     {
      //--- 
      uint64_t          login;                           // user's login
      //--- 
      uint64_t          expert_deals;                    // expert deals
      uint64_t          expert_orders;                   // expert orders
      double            expert_profit;                   // expert total
      //--- 
      uint64_t          signal_deals;                    // signal deals
      uint64_t          signal_orders;                   // signal orders
      double            signal_profit;                   // signal total
      //---       
      uint64_t          total_deals;                     // deals total
      uint64_t          total_orders;                    // orders total
      double            total_profit;                    // total profit
     };
   //--- information about user's special activity for chart
   struct DateInfo
     {
      //--- 
      int64_t           date;                            // date
      //--- 
      uint64_t          expert_deals;                    // expert deals
      uint64_t          expert_orders;                   // expert orders
      double            expert_profit;                   // expert total
      //--- 
      uint64_t          signal_deals;                    // signal deals
      uint64_t          signal_orders;                   // signal orders
      double            signal_profit;                   // signal total
      //---       
      uint64_t          total_deals;                     // deals total
      uint64_t          total_orders;                    // orders total
      double            total_profit;                    // total profit
     };
   //--- information about user's special activity for table
#pragma pack(push,1)
   struct TableInfo
     {
      //--- 
      uint64_t          login;                           // user's login
      wchar_t           name[128];                       // user's name
      wchar_t           group[64];                       // user's group
      double            balance;                         // user's balance
      double            floating;                        // user's floating
      //--- 
      uint64_t          expert_deals;                    // expert deals
      uint64_t          expert_orders;                   // expert orders
      double            expert_profit;                   // expert total
      //--- 
      uint64_t          signal_deals;                    // signal deals
      uint64_t          signal_orders;                   // signal orders
      double            signal_profit;                   // signal total
      //---       
      uint64_t          total_deals;                     // deals total
      uint64_t          total_orders;                    // orders total
      double            total_profit;                    // total profit
     };
#pragma pack(pop)

private:
   IMTReportAPI*     m_api;                           // api pointer
   //--- charts
   IMTReportChart*   m_chart_deals;                   // chart of deals number
   IMTReportChart*   m_chart_profit;                  // chart of profit
   IMTReportChart*   m_chart_orders;                  // chart of orders
   //--- chart info arrays
   DailyInfo         m_chart_info[REPORT_PERIOD];     // chart values array
   int64_t           m_chart_titles[REPORT_PERIOD];   // chart titles array
   //--- tops
   DailyInfo         m_deals_signal[TOP_NUMBER];      // <!!!>
   DailyInfo         m_deals_expert[TOP_NUMBER];      // <!!!>
   DailyInfo         m_orders_signal[TOP_NUMBER];     // <!!!>
   DailyInfo         m_orders_expert[TOP_NUMBER];     // <!!!>
   DailyInfo         m_profit_signal[TOP_NUMBER];     // <!!!>
   DailyInfo         m_profit_expert[TOP_NUMBER];     // <!!!>
   //--- helper configs
   IMTUser*          m_user;                          // user
   IMTConGroup*      m_group;                         // group
   IMTDealArray*     m_deals;                         // array of deals
   IMTOrderArray*    m_orders;                        // array of orders
   IMTAccount*       m_account;                       // account
   //--- parameters
   wchar_t           m_currency[32];                  // currency for converting
   uint32_t          m_digits;                        // count of digits after point in money
   wchar_t           m_group_mask[256];               // group-mask for filtration
   //--- static data
   static const MTReportInfo s_info;                  // report info
   static ReportColumn s_columns_daily[];             // column description daily info
   static ReportColumn s_columns_table[];             // column description table

public:
   //--- constructor/destructor
                     CDailyExpertReport(void);
   virtual          ~CDailyExpertReport(void);
   //--- get information about report
   static void       Info(MTReportInfo& info) { info=s_info;          };
   //--- release plug-in
   virtual void      Release(void)            { delete this;          };
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);
private:
   //--- create/release interfaces
   void              Clear(void);
   bool              CreateInterfaces(void);
   //--- get params
   MTAPIRES          GetParameters(void);
   //--- convert money
   MTAPIRES          ConvertMoney(const double money,double &converted);
   //--- fill info
   MTAPIRES          GetReportInfo(void);
   MTAPIRES          ParseDealArray(IMTDealArray* deals,DailyInfo &info);
   MTAPIRES          ParseOrdersArray(IMTOrderArray* deals,DailyInfo &info);
   //--- prepare charts
   MTAPIRES          PrepareChartTitle(IMTReportChart* chart);
   //--- add information values to charts
   MTAPIRES          FillDealChart(void);
   MTAPIRES          FillProfitChart(void);
   MTAPIRES          FillOrdersChart(void);
   //--- write html
   MTAPIRES          WriteAll(void);
   bool              WriteExpert(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteSignal(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              ProcessTable(MTAPISTR& tag,DailyInfo& info,MTAPIRES& retcode);
   //--- work with top
   void              TopModify(DailyInfo& user);
   //--- generate dashboard report
   MTAPIRES          GenerateDashboard(void);
   //--- add chart
   MTAPIRES          AddDashboardChart(IMTDataset *data,LPCWSTR title,uint32_t column);
   //--- create table from array
   MTAPIRES          TableFromArray(IMTReportAPI &api,LPCWSTR title,const ReportColumn *columns,const uint32_t columns_total,const DailyInfo *arr,const uint32_t total);
   //--- sort top
   static int32_t    SortByExpertProfit(const void* left,const void* right);
   static int32_t    SortByExpertOrders(const void* left,const void* right);
   static int32_t    SortByExpertDeals(const void* left,const void* right);
   static int32_t    SortBySignalProfit(const void* left,const void* right);
   static int32_t    SortBySignalOrders(const void* left,const void* right);
   static int32_t    SortBySignalDeals(const void* left,const void* right);
  };
//+------------------------------------------------------------------+