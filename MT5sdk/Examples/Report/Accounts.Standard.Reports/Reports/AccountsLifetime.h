//+------------------------------------------------------------------+
//|                           MetaTrader 5 Accounts.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportBase.h"
//+------------------------------------------------------------------+
//| Accounts Lifetime Report                                         |
//+------------------------------------------------------------------+
class CAccountsLifetime : public CReportBase
  {
private:
   //--- constants
   static constexpr uint32_t ACCOUNTS_THRESHOLD=10; // minimum threshold (number of accounts) for averaging
   //--- enumeration of user types in statistics
   enum EnUserType : uint32_t
     {
      USER_TYPE_ANY     =0,
      USER_TYPE_MOBILE  =1,
      USER_TYPE_DESKTOP =2,
      USER_TYPE_WEB     =3,
      USER_TYPE_OTHERS  =4,
      USER_TYPE_COUNT
     };

private:
#pragma pack(push,1)
   //--- chart record
   struct ChartRecord
     {
      wchar_t           country[32];      // country
      uint64_t          lifetime;         // total average lifetime in days
     };
   //--- table record
   struct TableRecord
     {
      wchar_t           country[32];      // country
      uint64_t          lifetime_average; // total average lifetime in days
      uint64_t          lifetime_mobile;  // mobile average lifetime in days
      uint64_t          lifetime_desktop; // desktop average lifetime in days
      uint64_t          lifetime_web;     // web average lifetime in days
      uint64_t          lifetime_others;  // others average lifetime in days
     };
#pragma pack(pop)
   //--- statistics hash table structure
   struct StatRecord
     {
      wchar_t           country[32];            // country
      double            data[USER_TYPE_COUNT];  // statistics
      uint32_t          count[USER_TYPE_COUNT]; // statistics count

      struct Hash
        {
         uint64_t          operator()(const StatRecord &item) const  { return operator()(item.country); }
         uint64_t          operator()(const LPCWSTR country) const   { return MTCalculateFNV1(country,CMTStr::Len(country)); }
        };
      struct Equal
        {
         bool              operator()(const StatRecord &l,const StatRecord &r) const   { return operator()(l,r.country); }
         bool              operator()(const StatRecord &l,const LPCWSTR r) const       { return CMTStr::Compare(l.country,r)==0; }
        };
     };
   using             StatRecordTable=TMTHashTable<StatRecord,StatRecord::Hash,StatRecord::Equal>;  // statistics hash table
   using             StatRecordIndex=TMTArray<StatRecord*>;                                        // statistics index array

private:
   int64_t           m_from=0;            // report time interval begin
   int64_t           m_to=0;              // report time interval end
   //--- API interfaces
   IMTReportCacheKeySet *m_logins=nullptr;// report request logins
   IMTUser*          m_user=nullptr;      // user
   IMTDealArray*     m_deals=nullptr;     // deals array
   //--- statistics
   StatRecordTable   m_stats;             // statistics hash table
   StatRecordIndex   m_index;             // statistics index array
   //---
   static const MTReportInfo s_info;
   static const ReportColumn s_columns_chart[]; // charts columns descriptions
   static const ReportColumn s_columns_table[]; // table columns descriptions

public:
   //--- constructor/destructor
                     CAccountsLifetime(void);
   virtual          ~CAccountsLifetime(void);
   //--- report information
   static void       Info(MTReportInfo &info);
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(uint32_t type) override;
   //--- statistics calculation
   MTAPIRES          StatisticsCalc(void);
   //--- add data to statistics
   MTAPIRES          StatisticsAdd(LPCWSTR country,double lifetime,EnUserType type);
   //--- determine user terminal type
   bool              DetermineUserTypeByDeals(const uint64_t login,EnUserType &type,uint32_t &trades);
   //--- write charts
   MTAPIRES          WriteCharts(void);
   //--- write table
   MTAPIRES          WriteTable(void);
   //--- append widget
   MTAPIRES          WidgetAppend(IMTDataset *data,LPCWSTR title,IMTReportDashboardWidget::EnWidgetType type);
   //--- sort statistics
   static int32_t    StatisticsSortData(const void *left,const void *right);
   static int32_t    StatisticsSortCountry(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
