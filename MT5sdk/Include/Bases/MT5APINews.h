//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| News Message interface                                           |
//+------------------------------------------------------------------+
class IMTNews
  {
public:
   //--- news flags
   enum EnNewsFlags
     {
      NEWS_FLAGS_NONE      =0x0000,
      NEWS_FLAGS_PRIORITY  =0x0001,
      NEWS_FLAGS_READ      =0x0002,
      NEWS_FLAGS_NOBODY    =0x0004,
      NEWS_FLAGS_CALENDAR  =0x0008,
      //--- enumeration borders
      NEWS_FLAGS_FIRST        =NEWS_FLAGS_PRIORITY,
      NEWS_FLAGS_ALL          =NEWS_FLAGS_PRIORITY|NEWS_FLAGS_READ|NEWS_FLAGS_NOBODY|NEWS_FLAGS_CALENDAR
     };

public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTNews* news)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- news ID
   virtual uint64_t    ID(void) const=0;
   //--- subject
   virtual LPCWSTR   Subject(void) const=0;
   virtual MTAPIRES  Subject(LPCWSTR subject)=0;
   //--- category
   virtual LPCWSTR   Category(void) const=0;
   virtual MTAPIRES  Category(LPCWSTR category)=0;
   //--- time
   virtual int64_t     Time(void) const=0;
   virtual MTAPIRES  Time(const int64_t datetime)=0;
   //--- language
   virtual uint32_t  Language(void) const=0;
   virtual MTAPIRES  Language(const uint32_t language)=0;
   //--- language
   virtual uint32_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint32_t flags)=0;
   //--- body
   virtual LPCVOID   Body(void) const=0;
   virtual uint32_t  BodySize(void) const=0;
   virtual MTAPIRES  Body(LPCVOID body,const uint32_t body_size)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTNews(void) {}
  };
//+------------------------------------------------------------------+
//| News events notification interface                               |
//+------------------------------------------------------------------+
class IMTNewsSink
  {
public:
   virtual void      OnNews(const IMTNews* /*news*/)                  {                    };
   virtual MTAPIRES  HookNews(const int32_t /*feeder*/,IMTNews* /*news*/) { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+

