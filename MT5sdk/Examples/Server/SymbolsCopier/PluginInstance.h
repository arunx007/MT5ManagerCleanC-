//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Plugin implementation                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,
                        public IMTConPluginSink,
                        public IMTConSymbolSink
  {
private:
   //--- server API interface
   IMTServerAPI     *m_api;
   //--- plugin config interface
   IMTConPlugin*     m_config;
   //--- plugin parameters
   CMTStr1024        m_from_path;
   bool              m_from_subgroups;
   CMTStr1024        m_to_path;
   bool              m_to_subgroups;
   CMTStr128         m_extension;

public:
                     CPluginInstance(void);
                    ~CPluginInstance(void);
   //--- IMTServerPlugin methods
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI *server);
   virtual MTAPIRES  Stop(void);
   //--- IMTConPluginSinc interface implementation
   virtual void      OnPluginUpdate(const IMTConPlugin* plugin);
   //--- IMTConSymbolSinc interface implementation
   virtual void      OnSymbolSync(void);
   virtual void      OnSymbolAdd(const IMTConSymbol* symbol);
   virtual void      OnSymbolUpdate(const IMTConSymbol* symbol);
   virtual void      OnSymbolDelete(const IMTConSymbol* symbol);

private:
   static MTAPIRES   StringGet(IMTConPlugin* config,IMTConParam* param,LPCWSTR name);
   static bool       PathDefine(LPCWSTR value,CMTStr& path,bool& subfolders);
   static bool       PathGet(const IMTConSymbol* symbol,CMTStr& path);
   static bool       SubgroupsGet(const CMTStr& path,const CMTStr& root,CMTStr& subgroups);
   static bool       DestinationGet(const CMTStr& root,const CMTStr& subgroups,CMTStr& path);
   MTAPIRES          ParametersRead(void);
  };
//+------------------------------------------------------------------+
