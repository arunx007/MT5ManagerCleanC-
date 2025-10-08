//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,
                        public IMTConSymbolSink,
                        public IMTDealSink
  {
private:
   MTServerInfo      m_info;
   IMTServerAPI*     m_api;

public:
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* server);
   virtual MTAPIRES  Stop(void);
   //--- 
   virtual void      OnDealPerform(const IMTDeal* deal, IMTAccount* account,IMTPosition* position);

private:
   void              ParametersShow(void);
  };
//+------------------------------------------------------------------+

