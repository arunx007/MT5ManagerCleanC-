//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ReportInterval.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CReportInterval::CReportInterval(const uint32_t days_week,const uint32_t days_months,const uint32_t days_hour) :
   m_days_week(days_week),m_days_months(days_months),m_days_hour(days_hour),
   m_from(0),m_to(0),m_from_begin(0),m_from_month(0),m_week_from(0),m_week_to(0),m_period(0),m_interval(INTERVAL_DAY)
  {
   ZeroMemory(&m_from_tm,sizeof(m_from_tm));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CReportInterval::~CReportInterval(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CReportInterval::Initialize(IMTReportAPI *api)
  {
//--- clear
   Clear();
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- get range
   m_from=api->ParamFrom();
   m_to  =api->ParamTo();
//--- correct to week granularity
   if(!m_days_week)
     {
      m_from=WeekBeginMonday(m_from);
      m_to  =WeekBeginMonday(m_to)+SECONDS_IN_WEEK-1;
      //--- get report generation time
      int64_t now=api->TimeGeneration();
      if(!now)
         return(MT_RET_ERR_PARAMS);
      //--- limit to time to report generation time
      now=CReportInterval::WeekBeginMonday(now)-1;
      if(m_to>now)
         m_to=now;
     }
//--- check range
   if(m_from>m_to)
      return(MT_RET_ERR_PARAMS);
//--- parse time range begin
   if(!SMTTime::ParseTime(m_from,&m_from_tm))
      return(MT_RET_ERR_PARAMS);
//--- calculate months indexes
   m_from_month=MonthIndex(m_from);
   if(m_from_month>MonthIndex(m_to))
      return(MT_RET_ERR_PARAMS);
//--- compute numbers of days m_to proceed
   const uint32_t days_total=(uint32_t)(m_to-m_from+1)/SECONDS_IN_DAY;
   if(!days_total)
      return(MT_RET_OK_NONE);
//--- which interval m_to be used
   if(days_total<m_days_hour)
     {
      m_interval=INTERVAL_HOUR;
      m_from_begin=SMTTime::DayBegin(m_from);
      m_period=SECONDS_IN_HOUR;
     }
   else
      if(days_total<m_days_week)
        {
         m_interval=INTERVAL_DAY;
         m_from_begin=SMTTime::DayBegin(m_from);
         m_period=SECONDS_IN_DAY;
        }
      else
         if(days_total<m_days_months)
           {
            m_interval=INTERVAL_WEEK;
            m_from_begin=WeekBeginMonday(m_from);
            m_period=SECONDS_IN_WEEK;
           }
         else
            m_interval=INTERVAL_MONTH;
//--- weeks range
   m_week_from=WeekMonday(m_from);
   m_week_to  =WeekMonday(m_to);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CReportInterval::Clear(void)
  {
//--- clear parameters
   m_from=m_to=m_from_begin=0;
   m_from_month=m_period=0;
   m_week_from=m_week_to=0;
   m_interval=INTERVAL_DAY;
   ZeroMemory(&m_from_tm,sizeof(m_from_tm));
  }
//+------------------------------------------------------------------+
//| time period total count                                          |
//+------------------------------------------------------------------+
uint32_t CReportInterval::PeriodTotal(void) const
  {
//--- check interval
   if(m_from>m_to)
      return(0);
//--- calculate period total
   const int32_t total=PeriodIndex(m_to)-PeriodIndex(m_from)+1;
   if(total<0)
      return(0);
//--- return period total
   return((uint32_t)total);
  }
//+------------------------------------------------------------------+
//| time period index                                                |
//+------------------------------------------------------------------+
int32_t CReportInterval::PeriodIndex(int64_t ctm) const
  {
//--- special case for month interval
   if(m_interval==INTERVAL_MONTH)
      return((int)MonthIndex(ctm)-m_from_month);
//--- check
   if(!m_period)
      return(INT_MIN);
//--- calculate period index
   return((int)((ctm-m_from_begin)/m_period));
  }
//+------------------------------------------------------------------+
//| time period start date by index                                  |
//+------------------------------------------------------------------+
int64_t CReportInterval::PeriodDate(const uint32_t pos) const
  {
//--- calculate period time
   int64_t ctm=0;
   if(m_interval==INTERVAL_MONTH)
     {
      //--- special case for month interval
      tm ttm={};
      PeriodDateMonth(ttm,pos);
      ctm=SMTTime::MakeTime(&ttm);
     }
   else
      ctm=m_from_begin+pos*m_period;
//--- adjust by time range
   return(std::max(m_from,std::min(ctm,m_to)));
  }
//+------------------------------------------------------------------+
//| time month period start date by index                            |
//+------------------------------------------------------------------+
void CReportInterval::PeriodDateMonth(tm &ttm,const uint32_t pos) const
  {
//--- add month and make new time
   ttm=m_from_tm;
   ttm.tm_mon +=pos;
   ttm.tm_year+=ttm.tm_mon/12;
   ttm.tm_mon %=12;
  }
//+------------------------------------------------------------------+
//| month index                                                      |
//+------------------------------------------------------------------+
uint32_t CReportInterval::MonthIndex(const int64_t ctm)
  {
//--- check zero
   if(!ctm)
      return(0);
//--- parse time
   tm ttm={};
   if(!SMTTime::ParseTime(ctm,&ttm))
      return(0);
//--- compute month index
   return(ttm.tm_year*12+ttm.tm_mon);
  }
//+------------------------------------------------------------------+
//| format period                                                    |
//+------------------------------------------------------------------+
const CMTStr& CReportInterval::FormatPeriod(CMTStr &str,const uint32_t pos) const
  {
//--- special case for month interval
   if(m_interval==INTERVAL_MONTH)
     {
      //--- calculate time
      tm ttm={};
      PeriodDateMonth(ttm,pos);
      //--- format year and month
      str.Format(L"%04d.%02d",ttm.tm_year+1900,ttm.tm_mon+1);
      return(str);
     }
//--- calculate time
   const int64_t ctm=PeriodDate(pos);
   if(!ctm)
     {
      str.Clear();
      return(str);
     }
//--- standart formating
   SMTFormat::FormatDateTime(str,ctm,m_interval==INTERVAL_HOUR);
//--- enough for day and hour intervals
   if(m_interval==INTERVAL_DAY || m_interval==INTERVAL_HOUR)
      return(str);
//--- calculate end of period
   int64_t period_end=std::min(ctm+m_period-1,m_to);
   if(ctm==m_from)
      period_end=WeekBeginMonday(period_end)-1;
//--- format end of period
   CMTStr32 end;
   SMTFormat::FormatDateTime(end,period_end,false);
//--- find difference
   uint32_t diff=0,i=0,len=std::min(str.Len(),end.Len());
   for(uint32_t i=0,len=std::min(str.Len(),end.Len());i<len && str[i]==end[i];i++)
      if(str[i]==L'.')
         diff=i+1;
//--- append end
   if(i<len && diff<end.Len())
     {
      str.Append(L'-');
      str.Append(end.Str()+diff);
     }
//--- return string
   return(str);
  }
//+------------------------------------------------------------------+