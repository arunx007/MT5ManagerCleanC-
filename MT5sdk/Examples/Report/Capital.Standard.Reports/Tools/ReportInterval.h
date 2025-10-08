//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Report interval class                                            |
//+------------------------------------------------------------------+
class CReportInterval
  {
public:
   //--- intervals for interpolation
   enum EnInterval
     {
      INTERVAL_HOUR  =0,                              // hourly
      INTERVAL_DAY   =1,                              // daily
      INTERVAL_WEEK  =2,                              // weekly
      INTERVAL_MONTH =3,                              // monthly
      INTERVAL_COUNT =INTERVAL_MONTH                  // count of enumerable
     };

private:
   const uint32_t    m_days_week;                     // number of days for week interval
   const uint32_t    m_days_months;                   // number of days for month interval
   const uint32_t    m_days_hour;                     // number of days for hour interval
   int64_t           m_from;                          // report time range begin
   int64_t           m_to;                            // report time range end
   int64_t           m_from_begin;                    // report time range begin period begin
   tm                m_from_tm;                       // report time range begin parsed time
   uint32_t          m_from_month;                    // report first month index for month interval
   uint32_t          m_week_from;                     // report weeks range begin
   uint32_t          m_week_to;                       // report weeks range end
   uint32_t          m_period;                        // interval period in seconds
   EnInterval        m_interval;                      // interval

public:
                     CReportInterval(const uint32_t days_week,const uint32_t days_months,const uint32_t days_hour=0);
                    ~CReportInterval(void);
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI *api);
   //--- clear
   void              Clear(void);
   //--- report time range
   int64_t           From(void) const                    { return(m_from); }
   int64_t           To(void) const                      { return(m_to); }
   //--- check time in range   
   bool              Contains(const int64_t ctm) const     { return(m_from<=ctm && ctm<=m_to); }
   //--- check weeek in range   
   bool              WeekContains(const uint32_t week) const { return(m_week_from<=week && week<=m_week_to); }
   //--- time period total count
   uint32_t          PeriodTotal(void) const;
   //--- time period index
   int32_t           PeriodIndex(int64_t ctm) const;
   //--- time period start date by index
   int64_t           PeriodDate(const uint32_t pos) const;
   //--- time month period start date by index
   void              PeriodDateMonth(tm &ttm,const uint32_t pos) const;
   //--- format period
   const CMTStr&     FormatPeriod(CMTStr &str,const uint32_t pos) const;
   //--- month index
   static uint32_t   MonthIndex(const int64_t ctm);
   //--- monday week begin
   static int64_t    WeekBeginMonday(const int64_t ctm)    { return(SMTTime::WeekBegin(ctm-SECONDS_IN_DAY)+SECONDS_IN_DAY); }
   //--- monday week number
   static uint32_t   WeekMonday(const int64_t ctm)         { return(uint32_t((ctm+3*SECONDS_IN_DAY)/SECONDS_IN_WEEK)); }
  };
//+------------------------------------------------------------------+
