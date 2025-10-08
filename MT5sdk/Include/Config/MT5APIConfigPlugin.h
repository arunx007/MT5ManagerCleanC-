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
class IMTConPluginModule
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConPluginModule* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- default plugin name
   virtual LPCWSTR   Name(void) const=0;
   //--- vendor name
   virtual LPCWSTR   Vendor(void) const=0;
   //--- plugin description
   virtual LPCWSTR   Description(void) const=0;
   //--- plugin file name
   virtual LPCWSTR   Module(void) const=0;
   //--- MT5 server-owner id
   virtual uint64_t    Server(void) const=0;
   //--- plugin version
   virtual uint32_t  Version(void) const=0;
   //--- plugin Server API version
   virtual uint32_t  VersionAPI(void) const=0;
   //--- plugin default parameters
   virtual uint32_t  ParameterTotal(void) const=0;
   virtual MTAPIRES  ParameterNext(const uint32_t pos,IMTConParam* param) const=0;
   virtual MTAPIRES  ParameterGet(LPCWSTR name,IMTConParam* param) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConPluginModule(void) {}
  };
//+------------------------------------------------------------------+
//| Plugin instance configuration                                    |
//+------------------------------------------------------------------+
class IMTConPlugin
  {
public:
   //--- plugin working flags
   enum EnPluginFlags
     {
      PLUGIN_FLAG_MAN_CONFIG=1,  // allow configure plugin by managers
      PLUGIN_FLAG_PROFILING =2,  // allow plugin profiling
      //--- flags borders
      PLUGIN_FLAG_NONE     =0,
      PLUGIN_FLAG_ALL      =PLUGIN_FLAG_MAN_CONFIG|PLUGIN_FLAG_PROFILING
     };
   //--- plugin mode
   enum EnPluginMode
     {
      PLUGIN_DISABLED   =0,
      PLUGIN_ENABLED    =1,
      //--- enumeration borders
      PLUGIN_FIRST      =PLUGIN_DISABLED,
      PLUGIN_LAST       =PLUGIN_ENABLED,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConPlugin* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- plugin name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- MT5 server ID
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- plugin file name
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
   //--- EnPluginFlags
   virtual uint32_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint32_t flags)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConPlugin(void) {}
  };
//+------------------------------------------------------------------+
//| Plugin events notification interface                             |
//+------------------------------------------------------------------+
class IMTConPluginSink
  {
public:
   virtual void      OnPluginAdd(const IMTConPlugin*    /*plugin*/) {  }
   virtual void      OnPluginUpdate(const IMTConPlugin* /*plugin*/) {  }
   virtual void      OnPluginDelete(const IMTConPlugin* /*plugin*/) {  }
   virtual void      OnPluginSync(void)                             {  }
  };
//+------------------------------------------------------------------+
