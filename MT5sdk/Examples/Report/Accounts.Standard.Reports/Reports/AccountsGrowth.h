//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportBase.h"
#include "..\Tools\UserSelect.h"
//+------------------------------------------------------------------+
//| Accounts Grow Report                                             |
//+------------------------------------------------------------------+
class CAccountsGrowth : public CReportBase
  {
private:
   //--- constants
   enum constants
     {
      PERIOD_YEARS            =5,               // report period (years)
      PERIOD_MONTHS           =PERIOD_YEARS*12, // report period (months)
      PERIOD_INACTIVE_MONTHS  =3,               // report inactive account period (months)
     };

private:
   #pragma pack(push,1)
   //--- user record
   struct UserRecord
     {
      uint64_t          login;            // login
      int64_t           registration;     // registration time
      int64_t           last_access;      // last access time
     };
   //--- grow dataset record
   struct GrowRecord
     {
      int64_t           date;                   // login
      uint32_t          real;                   // real accounts
      uint32_t          preliminary;            // preliminary accounts
      uint32_t          demo;                   // demo accounts
      uint32_t          contest;                // contest accounts
     };
   //--- month dataset record
   struct MonthRecord
     {
      wchar_t           month[4];                     // month
      uint32_t          real [PERIOD_YEARS];          // real accounts
      uint32_t          preliminary [PERIOD_YEARS];   // preliminary accounts
      uint32_t          demo [PERIOD_YEARS];          // demo accounts
      uint32_t          contest [PERIOD_YEARS];       // contest accounts
     };
   //--- active dataset record
   struct ActiveRecord
     {
      wchar_t           type[16];               // account type
      uint32_t          active;                 // active accounts
     };
   #pragma pack(pop)

private:
   //--- current time
   int64_t           m_ctm;
   tm                m_ctm_ttm;
   int64_t           m_checktime;               // activity check time
   //--- month indexes
   uint32_t          m_month_data;              // start month with data
   uint32_t          m_month_firstyear;         // start month for first year
   //--- real accounts
   uint32_t          m_real[PERIOD_MONTHS];
   uint32_t          m_real_active;
   uint32_t          m_real_total;
   //--- preliminary accounts
   uint32_t          m_preliminary[PERIOD_MONTHS];
   uint32_t          m_preliminary_active;
   uint32_t          m_preliminary_total;
   //--- demo accounts
   uint32_t          m_demo[PERIOD_MONTHS];
   uint32_t          m_demo_active;
   uint32_t          m_demo_total;
   //--- contest accounts
   uint32_t          m_contest[PERIOD_MONTHS];
   uint32_t          m_contest_active;
   uint32_t          m_contest_total;
   //--- group & user
   IMTConGroup      *m_group;                      // group interface
   CUserSelect       m_user_select;                // user select object
   //--- charts
   IMTReportChart   *m_chart_grow;
   IMTReportChart   *m_chart_real;
   IMTReportChart   *m_chart_preliminary;
   IMTReportChart   *m_chart_demo;
   IMTReportChart   *m_chart_contest;
   IMTReportChart   *m_chart_active;
   //---
   static const MTReportInfo s_info;               // report information     
   static const ReportColumn s_columns_grow[];     // column descriptions Grow
   static const ReportColumn s_column_month;       // column description Month
   static const ReportColumn s_column_year;        // column description Year
   static const ReportColumn s_columns_active[];   // column descriptions Active
   static const DatasetField s_user_fields[];      // user request fields descriptions
   static const uint32_t s_request_limit=1000000;      // request limit

public:
   //--- constructor/destructor
                     CAccountsGrowth(void);
   virtual          ~CAccountsGrowth(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(uint32_t type) override;
   //--- report generation HTML
   MTAPIRES          GenerateHtml(void);
   //--- report generation dashboard
   MTAPIRES          GenerateDashboard(void);
   //--- prepare data
   MTAPIRES          PrepareData(void);
   MTAPIRES          PrepareGroup(LPCWSTR group);
   MTAPIRES          PrepareGroupPart(const uint32_t type,IMTDataset &users,uint64_t &login_last);
   //--- prepare charts
   MTAPIRES          PrepareChartGrow(void);
   MTAPIRES          PrepareChartReal(void);
   MTAPIRES          PrepareChartPreliminary(void);
   MTAPIRES          PrepareChartDemo(void);
   MTAPIRES          PrepareChartContest(void);
   MTAPIRES          PrepareChartActive(void);
   IMTReportSeries*  PrepareMonthTitleSeries(void);
   //--- prepare datasets
   MTAPIRES          PrepareDataColumns(IMTDataset *data,const ReportColumn *columns,const uint32_t total);
   MTAPIRES          PrepareDataGrow(IMTDataset *data);
   MTAPIRES          PrepareDataMonth(IMTDataset *data);
   MTAPIRES          PrepareDataActive(IMTDataset *data);
   //--- prepare widget
   MTAPIRES          PrepareWidget(IMTReportAPI *api,IMTDataset *data,uint32_t column,uint32_t column_total,LPCWSTR title,const uint32_t type);
   //--- month management
   bool              TimeToMonthIndex(const int64_t ctm,uint32_t &index);
   bool              MonthIndexToStr(const uint32_t index,CMTStr& str);
   int64_t           MonthIndexToTime(const uint32_t index);
   //--- colors
   uint32_t          GetColorForYear(const uint32_t index);
  };
//+------------------------------------------------------------------+
