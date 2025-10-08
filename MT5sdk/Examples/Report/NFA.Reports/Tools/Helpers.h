//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Namespace for helper functions                                   |
//+------------------------------------------------------------------+
class SPluginHelpers
  {
public:
   enum              constants { TIMEZONE_EST=-5*60 };
   //+---------------------------------------------------------------+
   //| Format date for file name                                     |
   //+---------------------------------------------------------------+
   inline static LPCWSTR FormatFileDate(CMTStr &str,const tm &ttm)
     {
      str.Format(L"%04d%02d%02d",ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday);
      return str.Str();
     }
   //+---------------------------------------------------------------+
   //| Format date for batch record                                  |
   //+---------------------------------------------------------------+
   inline static LPCWSTR FormatBatchDate(CMTStr &str,const tm &ttm)
     {
      str.Format(L"%04d-%02d-%02d",ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday);
      return str.Str();
     }
   //+---------------------------------------------------------------+
   //| Format date for batch record                                  |
   //+---------------------------------------------------------------+
   inline static LPCWSTR FormatBatchDate(CMTStr &str,const SYSTEMTIME &st)
     {
      str.Format(L"%04d-%02d-%02d",st.wYear,st.wMonth,st.wDay);
      return str.Str();
     }
   //+---------------------------------------------------------------+
   //| Format datetime for timestamp records                         |
   //+---------------------------------------------------------------+
   inline static LPCWSTR FormatStampDatetime(CMTStr &str,const tm &ttm)
     {
      str.Format(L"%04d-%02d-%02d %02d:%02d:%02d",ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday,ttm.tm_hour,ttm.tm_min,ttm.tm_sec);
      return str.Str();
     }
   //+---------------------------------------------------------------+
   //| Parse date from file name                                     |
   //+---------------------------------------------------------------+
   inline static bool ParseFileDate(LPCWSTR str,tm &ttm)
     {
      if(swscanf_s(str,L"%04d%02d%02d",&ttm.tm_year,&ttm.tm_mon,&ttm.tm_mday)!=3)
         return(false);
      ttm.tm_year-=1900;
      ttm.tm_mon -=1;
      return(true);
     }
   //+---------------------------------------------------------------+
   //| Parse date from batch record                                  |
   //+---------------------------------------------------------------+
   inline static bool ParseBatchDate(LPCWSTR str,tm &ttm)
     {
      if(swscanf_s(str,L"%04d-%02d-%02d",&ttm.tm_year,&ttm.tm_mon,&ttm.tm_mday)!=3)
         return(false);
      ttm.tm_year-=1900;
      ttm.tm_mon -=1;
      return(true);
     }
   //+---------------------------------------------------------------+
   //| Parse datetime from timestamp record                          |
   //+---------------------------------------------------------------+
   inline static bool ParseBatchDatetime(LPCWSTR str,tm &ttm)
     {
      if(swscanf_s(str,L"%04d-%02d-%02d %02d:%02d:%02d",&ttm.tm_year,&ttm.tm_mon,&ttm.tm_mday,&ttm.tm_hour,&ttm.tm_min,&ttm.tm_sec)!=6)
         return(false);
      ttm.tm_year-=1900;
      ttm.tm_mon -=1;
      return(true);
     }
   //+---------------------------------------------------------------+
   //| Convert EST time to trade time                                |
   //+---------------------------------------------------------------+
   inline static int64_t TimeFromEST(tm &ttm,const int32_t srv_timezone)
     {
      ttm.tm_min+=-TIMEZONE_EST+srv_timezone;
      return(_mkgmtime64(&ttm));
     }
   //+---------------------------------------------------------------+
   //| Convert trade time to EST time                                |
   //+---------------------------------------------------------------+
   inline static int64_t TimeToEST(tm &ttm,int srv_timezone)
     {
      ttm.tm_min+=-srv_timezone+TIMEZONE_EST;
      return(_mkgmtime64(&ttm));
     }
   //+---------------------------------------------------------------+
   //| Convert time to batch time                                    |
   //| Shift result time to weekday (next Monday or previous Friday) |
   //+---------------------------------------------------------------+
   inline static int64_t TimeToBatchDay(tm &ttm,const int64_t batch_time,const bool shift_to_monday)
     {
      //--- check next day
      if(ttm.tm_hour*SECONDS_IN_HOUR+ttm.tm_min*SECONDS_IN_MINUTE+ttm.tm_sec>=batch_time)
         ttm.tm_mday+=1;
      ttm.tm_hour=int(batch_time/SECONDS_IN_HOUR);
      ttm.tm_min =int((batch_time%SECONDS_IN_HOUR)/SECONDS_IN_MINUTE);
      ttm.tm_sec =int((batch_time%SECONDS_IN_HOUR)%SECONDS_IN_MINUTE);
      //--- update time
      _mkgmtime64(&ttm);
      //--- check week day
      switch(ttm.tm_wday)
        {
         //--- sunday
         case 0:
            ttm.tm_mday+=shift_to_monday ? 1 : -2;
            break;
            //--- saturday 
         case 6:
            ttm.tm_mday+=shift_to_monday ? 2 : -1;
            break;
         default:
            break;
        }
      //--- make time
      return(_mkgmtime64(&ttm));
     }
  };
//+------------------------------------------------------------------+
