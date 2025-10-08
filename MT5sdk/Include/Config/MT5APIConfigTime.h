//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Time configuration                                               |
//+------------------------------------------------------------------+
class IMTConTime
  {
public:
   //--- day working mode
   enum EnTimeTableMode
     {
      TIME_MODE_DISABLED=0,   // work enabled
      TIME_MODE_ENABLED =1,   // work disabled
      //---
      TIME_MODE_FIRST   =TIME_MODE_DISABLED,
      TIME_MODE_LAST    =TIME_MODE_ENABLED
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConTime* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- daylight correction mode
   virtual bool      Daylight(void) const=0;
   virtual MTAPIRES  Daylight(const bool enable)=0;
   //--- server timezone in minutes (0=GMT;-60=GMT-1;60=GMT+1)
   virtual int32_t   TimeZone(void) const=0;
   virtual MTAPIRES  TimeZone(const int32_t zone)=0;
   //--- time synchronization server address (TIME or NTP protocol)
   virtual LPCWSTR   TimeServer(void) const=0;
   virtual MTAPIRES  TimeServer(LPCWSTR server)=0;
   //--- time schedule (wday -> 0-Sunday,6-Saturday, hour-> 0-24, mode-> EnTimeTableMode)
   virtual MTAPIRES  TimeTableGet(const uint32_t wday,const uint32_t hour,uint32_t& mode) const=0;
   virtual MTAPIRES  TimeTableSet(const uint32_t wday,const uint32_t hour,const uint32_t mode)=0;
   //--- current daylight state
   virtual int32_t   DaylightState(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConTime(void) {}
  };
//+------------------------------------------------------------------+
//| Time config events notification interface                        |
//+------------------------------------------------------------------+
class IMTConTimeSink
  {
public:
   virtual void      OnTimeUpdate(const IMTConTime* /*config*/) {  }
   virtual void      OnTimeSync(void)                           {  }
  };
//+------------------------------------------------------------------+
