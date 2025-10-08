//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance: public IMTServerPlugin,
                       public IMTConPluginSink,
                       public IMTConSymbolSink
  {
private:
   //--- constants
   enum constants
     {
      CONNECTION_TIMEOUT=30000, // timeout in ms for IMTManagerAPI::Connect
      RECONNECT_TIMEOUT =500    // timeout in ms for thread reconnection loop
     };

private:
   //--- server API interface
   IMTServerAPI*     m_api;
   //--- manager API
   CMTManagerAPIFactory m_factory;
   IMTManagerAPI*    m_manager;
   //--- plugin config interface
   IMTConPlugin*     m_config;
   //--- plugin parameters
   CMTStr128         m_server;
   uint64_t          m_login;
   CMTStr128         m_password;
   CMTStr128         m_symbols;
   CMTSync           m_sync;
   //--- thread for connection by Manager API
   CMTThread         m_thread;
   //--- thread flags
   volatile bool     m_workflag;
   volatile bool     m_reconnect;

public:
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* server);
   virtual MTAPIRES  Stop(void);
   //--- IMTConPluginSinc interface implementation
   virtual void      OnPluginUpdate(const IMTConPlugin* plugin);
   //--- IMTConSymbolSinc interface implementation
   virtual void      OnSymbolUpdate(const IMTConSymbol* symbol);
   virtual void      OnSymbolSync(void);

private:
   MTAPIRES          ParametersRead(void);
   //--- connection thread functions
   MTAPIRES          ThreadStart(void);
   void              ThreadStop(void);
   void              Thread(void);
   static uint32_t __stdcall ThreadWrapper(void* lpParam);
   //--- manager connection methods
   MTAPIRES          ManagerConnect(void);
   MTAPIRES          ManagerDisconnect(void);
  };
//+------------------------------------------------------------------+
  