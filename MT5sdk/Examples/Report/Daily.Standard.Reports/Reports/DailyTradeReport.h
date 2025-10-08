//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportParameter.h"
//+------------------------------------------------------------------+
//| Daily Trade Report                                               |
//+------------------------------------------------------------------+
class CDailyTradeReport : public IMTReportContext
  {
private:
   //--- daily deals info
   struct DailyDealsInfo
     {
      double            profit;       // daily profit
      double            loss;         // daily loss
      int64_t           date;         // day
      uint32_t          profit_count; // number of profit deals
      uint32_t          loss_count;   // number of loss deals
     };
   //--- positions info
   struct PositionInfo
     {
      double            profit;       // profit/loss of position
      uint64_t          position;     // position id
     };
   //--- deals info
   struct DealInfo
     {
      uint64_t          login;        // user
      double            profit;       // profit/loss of deal
      uint64_t          ticket;       // deal
     };
   //--- account info
   struct AccountInfo
     {
      uint64_t          login;        // user
      double            closed_pl;    // closed profit/loss
      uint32_t          total_deals;  // number of trades
     };
   //--- arrays
   typedef TMTArray<DailyDealsInfo> DailyDealsArray;
   typedef TMTArray<DealInfo,8192> DealsArray;
   typedef TMTArray<PositionInfo,8192> PositionArray;
   typedef TMTArray<AccountInfo,2048> AccountArray;
   //--- constants
   enum constants
     {
      TOP_COUNT_MAX     =100000,
      CURRENCY_DIGITS   =2,
      DAILY_DEALS_PERIOD=SECONDS_IN_WEEK*2,  // period for graph
     };
   //--- dataset records
#pragma pack(push,1)
   //--- daily record
   struct DailyRecord
     {
      int64_t           date;
      double            profit;
      double            loss;
      double            profit_loss;
      uint32_t          profit_count;
      uint32_t          loss_count;
     };
   //--- Total record
   struct TotalRecord
     {
      wchar_t           type[32];
      double            value;
     };
   //--- Deal record
   struct DealRecord
     {
      uint64_t          deal;
      uint64_t          login;
      wchar_t           name[128];
      wchar_t           symbol[32];
      wchar_t           group[64];
      uint32_t          type;
      uint64_t          volume;
      double            price;
      double            swap;
      double            profit;
     };
   //--- Account record
   struct AccountRecord
     {
      uint64_t          login;
      wchar_t           name[128];
      wchar_t           group[64];
      uint32_t          leverage;
      uint32_t          placed_orders;
      uint32_t          orders;
      uint32_t          deals;
      double            balance;
      double            floating_pl;
      double            closed_pl;
     };
   //--- Position record
   struct PositionRecord
     {
      uint64_t          login;
      uint64_t          ticket;
      wchar_t           name[128];
      wchar_t           symbol[32];
      wchar_t           group[64];
      uint32_t          type;
      uint64_t          volume;
      double            open_price;
      double            sl;
      double            tp;
      double            market_price;
      double            swap;
      INT               points;
      double            profit;
     };
#pragma pack(pop)

private:
   IMTReportAPI*     m_api;                  // api interface
   //--- common configs
   CMTStr32          m_currency;             // currency
   uint32_t          m_digits;               // currency digits
   uint32_t          m_top_profit_deals;     // top profit deals count
   uint32_t          m_top_loss_deals;       // top loss deals count
   uint32_t          m_top_profit_positions; // top profit positions count
   uint32_t          m_top_loss_positions;   // top loss positions count
   uint32_t          m_top_profit_accounts;  // top profit accounts count
   IMTUser*          m_user;                 // user 
   IMTConGroup*      m_group;                // group`s config
   IMTPositionArray* m_positions;            // positions array
   IMTOrderArray*    m_orders;               // positions array
   //--- charts
   IMTReportChart*   m_chart_profit;         // profit/loss deals chart
   IMTReportChart*   m_chart_number;         // number of deals chart
   IMTReportChart*   m_chart_position;       // position pie chart
   //--- daily datas
   DailyDealsArray   m_daily_deals;          // array of all daily deals for period
   //--- positions processing
   PositionArray     m_positions_loss;       // array of loss positions
   PositionArray     m_positions_profit;     // array of profit positions
   IMTPosition*      m_position;             // position
   double            m_postion_total_loss;   // total loss from all positions
   double            m_postion_total_profit; // total profit from all positions
   //--- deals processing
   DealsArray        m_deals_loss;           // all loss deals in certain day
   DealsArray        m_deals_profit;         // all profit deals in certain day
   IMTDeal*          m_deal;                 // deal
   //--- accounts processing
   AccountArray      m_accounts;             // all profit accounts in certain day
   //--- static data
   static MTReportInfo s_info;               // report information
   static ReportColumn s_columns_daily[];    // column description daily deal
   static ReportColumn s_columns_total[];    // column description total
   static ReportColumn s_columns_deal[];     // column description deal
   static ReportColumn s_columns_account[];  // column description account
   static ReportColumn s_columns_position[]; // column description position

public:
   //--- constructor/destructor
                     CDailyTradeReport(void);
   virtual          ~CDailyTradeReport(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- create/release interfaces
   void              Clear(void);
   bool              CreateInterfaces(void);
   //--- get report parameters
   MTAPIRES          GetParameters(void);
   //--- get top count report parameter value
   static MTAPIRES   TopCount(CReportParameter &params,uint32_t &top_count,LPCWSTR param_name);
   //--- load info
   MTAPIRES          LoadInfo(void);
   MTAPIRES          LoadPositions(void);
   MTAPIRES          LoadDeals(void);
   MTAPIRES          ComputeAccounts(void);
   MTAPIRES          ComputeDailyDeals(void);
   //--- convert money
   MTAPIRES          ConvertMoney(const double profit,const uint64_t login,double& converted);
   //--- prepare chart
   MTAPIRES          PrepareProfitLossChart(void);
   MTAPIRES          PrepareNumberChart(void);
   MTAPIRES          PreparePositionsChart(void);
   //--- write all html
   MTAPIRES          WriteAll(void);
   bool              WriteTableDeals(MTAPISTR& tag,MTAPIRES& res,const uint32_t counter,bool profit);
   bool              WriteTablePositions(MTAPISTR& tag,MTAPIRES& res,const uint32_t counter,bool profit);
   bool              WriteTopTrades(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteTableAccounts(MTAPISTR& tag,MTAPIRES& res,const uint32_t counter,bool profit);
   bool              WriteTopPosition(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   //--- generate dashboard report
   MTAPIRES          GenerateDashboard(void);
   //--- generate dashboard chart daily data
   IMTDataset*       ChartDailyData(MTAPIRES &res);
   //--- generate dashboard chart total
   MTAPIRES          AddChartTotal(void);
   //--- generate dashboard table deals
   MTAPIRES          AddTableDeal(LPCWSTR title,const DealsArray &deals,uint32_t top_count);
   //--- generate dashboard table accounts
   MTAPIRES          AddTableAccount(void);
   //--- generate dashboard table positions
   MTAPIRES          AddTablePositions(LPCWSTR title,const PositionArray &positions,uint32_t top_count);
   //--- add chart
   MTAPIRES          AddDashboardChart(IMTDataset *data,LPCWSTR title,bool bar,bool acc,uint32_t column_id,uint32_t column_count);
   //--- append currency to string
   void              AppendCurrency(CMTStr &str) const;
   //--- sorting
   static int32_t    SortByProfitPosition(const void *left,const void *right);
   static int32_t    SortByLossDeal(const void *left,const void *right);
   static int32_t    SortByClosedPLAccount(const void *left, const void *right);
   static int32_t    SortByProfitDeal(const void *left,const void *right);
   static int32_t    SortByLoginDeal(const void *left, const void *right);
   static int32_t    SortByLossPosition(const void *left, const void *right);
  };
//+------------------------------------------------------------------+

