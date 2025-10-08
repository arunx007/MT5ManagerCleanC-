//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyReport.h"
#include "..\Tools\ReportVector.h"
#include "..\Tools\ReportColumn.h"
#include "..\Cache\UserCache.h"
#include "..\Cache\DealUserCache.h"
//+------------------------------------------------------------------+
//| Lifetime Value Detailed by Trading Account Report class          |
//+------------------------------------------------------------------+
class CRiskAppetiteReport : public CCurrencyReport
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN            =1,
      COLUMN_NAME             =2,
      COLUMN_REGISTRATION_TIME=3,
      COLUMN_SL               =4,
      COLUMN_TP               =5,
      COLUMN_TP_SL_RATIO      =6,
      COLUMN_LOSS             =7,
      COLUMN_PROFIT           =8,
      COLUMN_PROFIT_LOSS_RATIO=9,
     };
   //--- result chart record
   #pragma pack(push,1)
   struct ChartRecord
     {
      double            ratio;                     // ratio
      uint32_t          count_tp_sl;               // count TP/SL ratio
      uint32_t          count_pl;                  // count profit/loss ratio
     };
   //--- result table record
   struct TableRecord
     {
      uint64_t          login;                     // login
      wchar_t           name[128];                 // name
      int64_t           registration_time;         // registration time
      double            sl;                        // SL
      double            tp;                        // TP
      double            tp_sl_ratio;               // TP/SL ratio
      double            loss;                      // loss
      double            profit;                    // profit          
      double            profit_loss_ratio;         // profit/loss ratio
      uint32_t          name_id;                   // name id in dictionary
     };
   #pragma pack(pop)
   //--- Container types
   typedef TReportVector<ChartRecord> ChartRecordVector; // chart record vector
   typedef TMTArray<TableRecord*,512> TableRecordArray;  // top record array

private:
   CUserCache       *m_users;                      // user cache
   CDealUserCache   *m_deals;                      // deals statistics cache
   int64_t           m_from;                       // report time range begin
   int64_t           m_to;                         // report time range end
   uint32_t          m_min_deals;                  // minimum deals count
   uint32_t          m_chart_percentile;           // chart percentile
   uint32_t          m_top_count;                  // top count
   ChartRecordVector m_ratios;                     // profit/loss ratio distribution vector
   uint32_t          m_ratio_accounts;             // accounts with ratio in distribution vector
   TableRecordArray  m_top;                        // top trading accounts
   //--- static data
   static const MTReportInfo s_info;               // static report info
   static const ReportColumn s_columns_chart[];    // chart column descriptions
   static const ReportColumn s_columns_table[];    // table column descriptions
   static const double s_ratio_step;               // ratio step
   static const uint32_t s_ratio_max;                  // ratio max steps
   static const LPCWSTR s_description_chart;       // chart description
   static const LPCWSTR s_description_table;       // table description

public:
   //--- constructor/destructor
                     CRiskAppetiteReport(void);
   virtual          ~CRiskAppetiteReport(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- fabric method
   static IMTReportContext* Create(void);
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);

private:
   //--- clear
   virtual void      Clear(void) override;
   //--- prepare to calculation
   virtual MTAPIRES  Prepare(void) override;
   //--- select data from server and compute statistics
   virtual MTAPIRES  Calculate(void) override;
   //--- calculate all statistics
   MTAPIRES          CalculateStatistics(void);
   //--- prepare all graphs to be shown
   virtual MTAPIRES  PrepareGraphs(void) override;
   //--- prepare ratio graph
   MTAPIRES          PrepareRatioGraph(IMTDataset *data);
   //--- prepare top table
   MTAPIRES          PrepareTable(void);
   //--- add ratios to distribution
   void              AddRatio(double ratio,const double tp_sl_ratio);
   //--- check profit/loss ratio to place in top
   bool              CheckTopRecord(double ratio) const;
   //--- check profit/loss ratio to place in top
   MTAPIRES          AddTopRecord(TableRecord *record);
   //--- write top accounts data
   MTAPIRES          WriteTopData(IMTDataset &data);
   //--- calculate price difference average value
   static double     PriceDiffAverage(const CDealUserCache::DealPrice &aggregate,uint32_t min_deals);
   //--- search function table record by ratio
   static int32_t    SearchTableRecordByRatio(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
