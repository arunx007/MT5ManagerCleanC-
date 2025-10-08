//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Holidays config interface                                        |
//+------------------------------------------------------------------+
class IMTConHoliday
  {
public:
   //--- holiday modes
   enum EnHolidayMode
     {
      HOLIDAY_DISABLED=0,
      HOLIDAY_ENABLED =1,
      //--- enumeration borders
      HOLIDAY_FIRST   =HOLIDAY_DISABLED,
      HOLIDAY_LAST    =HOLIDAY_ENABLED,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConHoliday* holiday)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- holiday description
   virtual LPCWSTR   Description(void) const=0;
   virtual MTAPIRES  Description(LPCWSTR descr)=0;
   //--- EnHolidayMode
   virtual uint32_t  Mode(void) const=0;
   virtual MTAPIRES  Mode(const uint32_t mode)=0;
   //--- holiday year (for example: 2010, 0 - every year)
   virtual uint32_t  Year(void) const=0;
   virtual MTAPIRES  Year(const uint32_t year)=0;
   //--- holiday month (1-January, 12-December)
   virtual uint32_t  Month(void) const=0;
   virtual MTAPIRES  Month(const uint32_t month)=0;
   //--- holiday day (1-31)
   virtual uint32_t  Day(void) const=0;
   virtual MTAPIRES  Day(const uint32_t day)=0;
   //--- holiday working time from (in minutes - 100 means 01:40)
   virtual uint32_t  WorkFrom(void) const=0;
   virtual MTAPIRES  WorkFrom(const uint32_t from)=0;
   virtual uint32_t  WorkFromHours(void) const=0;
   virtual uint32_t  WorkFromMinutes(void) const=0;
   //--- holiday working time to (in minutes - 100 means 01:40)
   virtual uint32_t  WorkTo(void) const=0;
   virtual MTAPIRES  WorkTo(const uint32_t from)=0;
   virtual uint32_t  WorkToHours(void) const=0;
   virtual uint32_t  WorkToMinutes(void) const=0;
   //--- holiday symbols list
   virtual MTAPIRES  SymbolAdd(LPCWSTR path)=0;
   virtual MTAPIRES  SymbolUpdate(const uint32_t pos,LPCWSTR path)=0;
   virtual MTAPIRES  SymbolDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SymbolClear(void)=0;
   virtual MTAPIRES  SymbolShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SymbolTotal(void) const=0;
   virtual LPCWSTR   SymbolNext(const uint32_t pos) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConHoliday(void) {}
  };
//+------------------------------------------------------------------+
//| Holiday config events notification interface                     |
//+------------------------------------------------------------------+
class IMTConHolidaySink
  {
public:
   virtual void      OnHolidayAdd(const IMTConHoliday*    /*config*/) {  }
   virtual void      OnHolidayUpdate(const IMTConHoliday* /*config*/) {  }
   virtual void      OnHolidayDelete(const IMTConHoliday* /*config*/) {  }
   virtual void      OnHolidaySync(void)                              {  }
  };
//+------------------------------------------------------------------+

