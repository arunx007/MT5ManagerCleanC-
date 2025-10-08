//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| History synchronization config                                   |
//+------------------------------------------------------------------+
class IMTConHistorySync
  {
public:
   //--- mode enumeration
   enum EnHistoryMode
     {
      HISTORY_DISABLED =0,
      HISTORY_ENABLED  =1,
      //--- enumeration borders
      HISTORY_FIRST    =HISTORY_DISABLED,
      HISTORY_LAST     =HISTORY_ENABLED,
     };
   //--- synchronization modex
   enum EnHistorySyncMode
     {
      MODE_REPLACE     =0, // syncronization with full previous data replace
      MODE_MERGE       =1, // syncronization with merge with previous data
      //--- enumeration borders
      MODE_FIRST       =MODE_REPLACE,
      MODE_LAST        =MODE_MERGE,
     };
   //--- server types
   enum EnHistorySyncServer
     {
      SERVER_MT4       =0, // MT4
      SERVER_MT5       =1, // MT5
      //--- enumeration borders
      SERVER_FIRST     =SERVER_MT4,
      SERVER_LAST      =SERVER_MT5,
     };
   //--- synchronization flags
   enum EnHistorySyncFlags
     {
      FLAG_SESSIONS     =1, // check quote session due synchronization
      FLAG_SYNONYMS     =2, // synchronize synonim symbols history
      //--- enumeration borders
      FLAG_NONE          =0,
      FLAG_ALL          =FLAG_SESSIONS|FLAG_SYNONYMS,
     };
   //--- synchronization data
   enum EnHistoryData
     {
      DATA_HISTORY_CHARTS=0, // charts only
      DATA_HISTORY_TICKS =1, // ticks only
      DATA_HISTORY_ALL   =2, // charts and ticks
      //--- enumeration borders
      DATA_HISTORY_FIRST =DATA_HISTORY_CHARTS,
      DATA_HISTORY_LAST  =DATA_HISTORY_ALL
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConHistorySync* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- server address
   virtual LPCWSTR   Server(void) const=0;
   virtual MTAPIRES  Server(LPCWSTR server)=0;
   //--- server type
   virtual uint32_t  ServerType(void) const=0;
   virtual MTAPIRES  ServerType(const uint32_t type)=0;
   //--- mode
   virtual uint32_t  Mode(void) const=0;
   virtual MTAPIRES  Mode(const uint32_t mode)=0;
   //--- synchronization mode
   virtual uint32_t  ModeSync(void) const=0;
   virtual MTAPIRES  ModeSync(const uint32_t type)=0;
   //--- time correction in minutes, 0 - autodetect
   virtual int32_t   TimeCorrection(void) const=0;
   virtual MTAPIRES  TimeCorrection(const int32_t correction)=0;
   //--- synchronized history start
   virtual int64_t     From(void) const=0;
   virtual MTAPIRES  From(const int64_t from)=0;
   //--- synchronized history finish
   virtual int64_t     To(void) const=0;
   virtual MTAPIRES  To(const int64_t to)=0;
   //--- synchronized symbols list
   virtual MTAPIRES  SymbolAdd(LPCWSTR path)=0;
   virtual MTAPIRES  SymbolUpdate(const uint32_t pos,LPCWSTR path)=0;
   virtual MTAPIRES  SymbolShift(const uint32_t pos,const int32_t shift)=0;
   virtual MTAPIRES  SymbolDelete(const uint32_t pos)=0;
   virtual uint32_t  SymbolTotal(void) const=0;
   virtual LPCWSTR   SymbolNext(const uint32_t pos) const=0;
   //--- synchronization flags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- synchronization data
   virtual uint32_t  HistoryData(void) const=0;
   virtual MTAPIRES  HistoryData(const uint32_t data)=0;
   //--- login for MetaTrader 5 server
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- password for MetaTrader 5 server
   virtual LPCWSTR   Password(void) const=0;
   virtual MTAPIRES  Password(LPCWSTR password)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConHistorySync(void) {}
  };
//+------------------------------------------------------------------+
//| History config events notification interface                     |
//+------------------------------------------------------------------+
class IMTConHistorySyncSink
  {
public:
   virtual void      OnHistorySyncAdd(const IMTConHistorySync*    /*config*/) {  }
   virtual void      OnHistorySyncUpdate(const IMTConHistorySync* /*config*/) {  }
   virtual void      OnHistorySyncDelete(const IMTConHistorySync* /*config*/) {  }
   virtual void      OnHistorySyncSync(void)                                  {  }
  };
//+------------------------------------------------------------------+
