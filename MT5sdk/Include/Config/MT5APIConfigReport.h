//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MT5APIConfigParam.h"
//+------------------------------------------------------------------+
//| Plugin module configuration                                      |
//+------------------------------------------------------------------+
class IMTConReportModule
  {
public:
   //--- snapshot mode flags
   enum EnSnapshots
     {
      SNAPSHOT_NONE          =0x0,       // without snapshots
      SNAPSHOT_USERS         =0x1,       // users database snapshot for request
      SNAPSHOT_USERS_FULL    =0x2,       // full users database snapshot
      SNAPSHOT_ACCOUNTS      =0x4,       // trade account states snapshot
      SNAPSHOT_ACCOUNTS_FULL =0x8,       // trade account states snapshot for request
      SNAPSHOT_ORDERS        =0x10,      // orders database snapshot
      SNAPSHOT_ORDERS_FULL   =0x20,      // orders database snapshot for request
      SNAPSHOT_POSITIONS     =0x40,      // positions database snapshot
      SNAPSHOT_POSITIONS_FULL=0x80,      // positions database snapshot for request
      //---
      SNAPSHOT_FIRST         =SNAPSHOT_NONE,
      SNAPSHOT_LAST          =SNAPSHOT_POSITIONS_FULL,
     };
   //--- types reports
   enum EnTypes
     {
      TYPE_NONE              =0x0,        // no support
      TYPE_HTML              =0x1,        // HTML
      TYPE_TABLE             =0x2,        // binary table
      //---
      TYPE_FIRST             =TYPE_NONE,
      TYPE_LAST              =TYPE_TABLE,
      TYPE_ALL               =TYPE_HTML|TYPE_TABLE
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConReportModule* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- default report name
   virtual LPCWSTR   Name(void) const=0;
   //--- vendor name
   virtual LPCWSTR   Vendor(void) const=0;
   //--- report description
   virtual LPCWSTR   Description(void) const=0;
   //--- report file name
   virtual LPCWSTR   Module(void) const=0;
   //--- report index in file
   virtual uint32_t  Index(void) const=0;
   //--- MT5 server-owner id
   virtual uint64_t    Server(void) const=0;
   //--- plugin version
   virtual uint32_t  Version(void) const=0;
   //--- plugin Server API version
   virtual uint32_t  VersionAPI(void) const=0;
   //--- neccessary Internet Explorer version
   virtual uint32_t  VersionIE(void) const=0;
   //--- available types
   virtual uint32_t  Types(void) const=0;
   //--- neccessary data snapshots
   virtual uint32_t  Snapshots(void) const=0;
   //--- report default parameters
   virtual uint32_t  ParameterTotal(void) const=0;
   virtual MTAPIRES  ParameterNext(const uint32_t pos,IMTConParam* param) const=0;
   virtual MTAPIRES  ParameterGet(LPCWSTR name,IMTConParam* param) const=0;
   //--- report request input params
   virtual uint32_t  InputTotal(void) const=0;
   virtual MTAPIRES  InputNext(const uint32_t pos,IMTConParam* param) const=0;
   virtual MTAPIRES  InputGet(LPCWSTR name,IMTConParam* param) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConReportModule(void) {}
  };
//+------------------------------------------------------------------+
//| Plugin instance configuration                                    |
//+------------------------------------------------------------------+
class IMTConReport
  {
public:
   //--- report mode
   enum EnReportMode
     {
      REPORT_DISABLED=0,
      REPORT_ENABLED =1,
      //--- enumeration borders
      REPORT_FIRST   =REPORT_DISABLED,
      REPORT_LAST    =REPORT_ENABLED,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConReport* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- plugin name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- MT5 server ID
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- plugin report name
   virtual LPCWSTR   Module(void) const=0;
   virtual MTAPIRES  Module(LPCWSTR name)=0;
   //--- plugin mode
   virtual uint32_t  Mode(void) const=0;
   virtual MTAPIRES  Mode(const uint32_t mode)=0;
   //--- plugin parameters
   virtual MTAPIRES  ParameterAdd(IMTConParam* param)=0;
   virtual MTAPIRES  ParameterUpdate(const uint32_t pos,const IMTConParam* param)=0;
   virtual MTAPIRES  ParameterDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ParameterClear(void)=0;
   virtual MTAPIRES  ParameterShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ParameterTotal(void) const=0;
   virtual MTAPIRES  ParameterNext(const uint32_t pos,IMTConParam* param) const=0;
   virtual MTAPIRES  ParameterGet(LPCWSTR name,IMTConParam* param) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConReport(void) {}
  };
//+------------------------------------------------------------------+
//| Plugin events notification interface                             |
//+------------------------------------------------------------------+
class IMTConReportSink
  {
public:
   virtual void      OnReportAdd(const IMTConReport*    /*report*/) {  }
   virtual void      OnReportUpdate(const IMTConReport* /*report*/) {  }
   virtual void      OnReportDelete(const IMTConReport* /*report*/) {  }
   virtual void      OnReportSync(void)                             {  }
  };
//+------------------------------------------------------------------+
