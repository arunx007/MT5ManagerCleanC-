//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <sys/types.h>
#include <sys/timeb.h>
//+------------------------------------------------------------------+
//| Seconds macro                                                    |
//+------------------------------------------------------------------+
#define SECONDS_IN_MINUTE    int64_t(60)
#define SECONDS_IN_HOUR      int64_t(60*SECONDS_IN_MINUTE)
#define SECONDS_IN_DAY       int64_t(24*SECONDS_IN_HOUR)
#define SECONDS_IN_WEEK      int64_t(7*SECONDS_IN_DAY)
#define SECONDS_IN_MONTH     int64_t(30*SECONDS_IN_DAY)
//+------------------------------------------------------------------+
//| Minutes macro                                                    |
//+------------------------------------------------------------------+
#define MINUTES_IN_HOUR      (60)
#define MINUTES_IN_DAY       (24*MINUTES_IN_HOUR)
#define MINUTES_IN_WEEK      (7*MINUTES_IN_DAY)
//+------------------------------------------------------------------+
//| Time management functions                                        |
//+------------------------------------------------------------------+
class SMTTime
  {
private:
   static const int64_t s_max_time64_t;
   static const LPCWSTR s_months_names[13];
   static const LPCWSTR s_months_short_names[13];

public:
   //--- time functions
   static bool       ParseTime(const int64_t ctm,tm *ttm);
   static int64_t    MakeTime(tm *ttm);
   static LPCWSTR    MonthName(const uint8_t month);
   static LPCWSTR    MonthNameShort(const uint8_t month);
   static int64_t    WeekBegin(const int64_t ctm);
   static int64_t    DayBegin(const int64_t ctm);
   static int64_t    MonthBegin(const int64_t ctm);
   static int64_t    YearBegin(const int64_t ctm);
   //---  SYSTEMTIME
   static int64_t    STToTime(const SYSTEMTIME &st);
   static SYSTEMTIME& TimeToST(int64_t ctm,SYSTEMTIME& st);
   //--- year, month & time hour
   static uint32_t   Year(const int64_t ctm);
   static uint32_t   Month(const int64_t ctm);
   static uint32_t   Day(const int64_t ctm);
   static uint32_t   Hour(const int64_t ctm);
   static uint32_t   Min(const int64_t ctm);
   static uint32_t   Sec(const int64_t ctm);
  };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const __declspec(selectany) int64_t   SMTTime::s_max_time64_t=0x793406fffi64;
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const __declspec(selectany)LPCWSTR  SMTTime::s_months_names[13]      ={ L"January",L"February",L"March",L"April",L"May",L"June",L"July",L"August",L"September",L"October",L"November",L"December",L"Unknown" };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const __declspec(selectany)LPCWSTR  SMTTime::s_months_short_names[13]={ L"Jan",L"Feb",L"Mar",L"Apr",L"May",L"Jun",L"Jul",L"Aug",L"Sep",L"Oct",L"Nov",L"Dec",L"Unk" };
//+------------------------------------------------------------------+
//| Time parsing                                                     |
//+------------------------------------------------------------------+
inline bool SMTTime::ParseTime(const int64_t ctm,tm *ttm)
  {
//--- check
   if(!ttm) return(false);
//--- check time
   if(ctm<0 || ctm>=s_max_time64_t)
     {
      ZeroMemory(ttm,sizeof(*ttm));
      return(false);
     }
//--- parse
   return(_gmtime64_s(ttm,&ctm)==0);
  }
//+------------------------------------------------------------------+
//| Time conversion                                                  |
//+------------------------------------------------------------------+
inline int64_t SMTTime::MakeTime(tm *ttm)
  {
//--- check
   if(!ttm) return(0);
//--- make time
   return(_mkgmtime64(ttm));
  }
//+------------------------------------------------------------------+
//| Month full name                                                  |
//+------------------------------------------------------------------+
inline LPCWSTR SMTTime::MonthName(const uint8_t month)
  {
   if(month>11) return s_months_names[12];
   else         return s_months_names[month];
  }
//+------------------------------------------------------------------+
//| Month short name                                                 |
//+------------------------------------------------------------------+
inline LPCWSTR SMTTime::MonthNameShort(const uint8_t month)
  {
   if(month>11) return s_months_short_names[12];
   else         return s_months_short_names[month];
  }
//+------------------------------------------------------------------+
//| Week begin calculation (Sunday)                                  |
//+------------------------------------------------------------------+
inline int64_t SMTTime::WeekBegin(const int64_t ctm)
  {
   if(ctm<345600) return(0);
   int64_t wday=(ctm%604800);
   wday+=(wday>=259200)?(-259200):(345600);
   return(ctm-wday);
  }
//+------------------------------------------------------------------+
//| Day begin calculation                                            |
//+------------------------------------------------------------------+
inline int64_t SMTTime::DayBegin(const int64_t ctm)
  {
   return((ctm/SECONDS_IN_DAY)*SECONDS_IN_DAY);
  }
//+------------------------------------------------------------------+
//| Month begin calculation                                          |
//+------------------------------------------------------------------+
inline int64_t SMTTime::MonthBegin(const int64_t ctm)
  {
   tm ttm;
//--- parse datetime
   ParseTime(ctm,&ttm);
//---
   ttm.tm_mday=1;
   ttm.tm_hour=ttm.tm_min=ttm.tm_sec=0;
//--- calc time
   return(_mkgmtime64(&ttm));
  }
//+------------------------------------------------------------------+
//| Year begin calculation                                           |
//+------------------------------------------------------------------+
inline int64_t SMTTime::YearBegin(const int64_t ctm)
  {
   tm ttm;
//--- parse datetime
   ParseTime(ctm,&ttm);
//--- 
   ttm.tm_mday=1;
   ttm.tm_mon =0;
   ttm.tm_hour=ttm.tm_min=ttm.tm_sec=0;
//--- calc time
   return(_mkgmtime64(&ttm));
  }
//+------------------------------------------------------------------+
//| UNIX time to SYSTEMTIME conversion                               |
//+------------------------------------------------------------------+
inline SYSTEMTIME& SMTTime::TimeToST(int64_t ctm,SYSTEMTIME& st)
  {
   tm ttm={};
//--- clean
   ZeroMemory(&st,sizeof(st));
//--- convert
   if(SMTTime::ParseTime(ctm,&ttm))
     {
      st.wYear     =WORD(1900+ttm.tm_year);
      st.wMonth    =WORD(1+ttm.tm_mon);
      st.wDayOfWeek=WORD(ttm.tm_wday);
      st.wDay      =WORD(ttm.tm_mday);
      st.wHour     =WORD(ttm.tm_hour);
      st.wMinute   =WORD(ttm.tm_min);
      st.wSecond   =WORD(ttm.tm_sec);
     }
//---
   return(st);
  }
//+------------------------------------------------------------------+
//| SYSTEMTIME to UNIX time conversion                               |
//+------------------------------------------------------------------+
inline int64_t SMTTime::STToTime(const SYSTEMTIME &st)
  {
   int64_t ctm;
   tm    ttm={};
//---
   ttm.tm_year=st.wYear-1900;
   ttm.tm_mon =st.wMonth-1;
   ttm.tm_mday=st.wDay;
   ttm.tm_wday=st.wDayOfWeek;
   ttm.tm_hour=st.wHour;
   ttm.tm_min =st.wMinute;
   ttm.tm_sec =st.wSecond;
   ttm.tm_isdst=0;
//--- 
   if((ctm=_mkgmtime64(&ttm))<0) ctm=0;
   return(ctm);
  }
//+------------------------------------------------------------------+
//| Year by datetime                                                 |
//+------------------------------------------------------------------+
inline uint32_t SMTTime::Year(const int64_t ctm) { tm ttm; ParseTime(ctm,&ttm); return(uint32_t(ttm.tm_year+1900)); }
//+------------------------------------------------------------------+
//| Month by datetime                                                |
//+------------------------------------------------------------------+
inline uint32_t SMTTime::Month(const int64_t ctm){ tm ttm; ParseTime(ctm,&ttm); return(uint32_t(ttm.tm_mon+1));     }
//+------------------------------------------------------------------+
//| Day by datetime                                                  |
//+------------------------------------------------------------------+
inline uint32_t SMTTime::Day(const int64_t ctm)  { tm ttm; ParseTime(ctm,&ttm); return(uint32_t(ttm.tm_mday));      }
//+------------------------------------------------------------------+
//| Hour by datetime                                                 |
//+------------------------------------------------------------------+
inline uint32_t SMTTime::Hour(const int64_t ctm) { tm ttm; ParseTime(ctm,&ttm); return(uint32_t(ttm.tm_hour));      }
//+------------------------------------------------------------------+
//| Minute by datetime                                               |
//+------------------------------------------------------------------+
inline uint32_t SMTTime::Min(const int64_t ctm)  { tm ttm; ParseTime(ctm,&ttm); return(uint32_t(ttm.tm_min));       }
//+------------------------------------------------------------------+
//| Second by datetime                                               |
//+------------------------------------------------------------------+
inline uint32_t SMTTime::Sec(const int64_t ctm)  { tm ttm; ParseTime(ctm,&ttm); return(uint32_t(ttm.tm_sec));       }
//+------------------------------------------------------------------+
