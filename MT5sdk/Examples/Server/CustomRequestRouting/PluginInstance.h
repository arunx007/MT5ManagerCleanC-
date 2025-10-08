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
                        public IMTTradeSink
  {
private:
   MTServerInfo      m_info;
   IMTServerAPI*     m_api;
   MTUInt64Array     m_cleared;

public:
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* server);
   virtual MTAPIRES  Stop(void);
   //--- trade request route hook
   virtual MTAPIRES  HookTradeRequestRuleFilter(IMTRequest*  request,
                                                IMTConRoute* rule,
                                                const IMTConGroup* group) override;
   //--- trade request route hook
   virtual MTAPIRES  HookTradeRequestRuleApply(IMTRequest*  request,
                                               IMTConRoute* rule,
                                               const IMTConGroup* group) override;
  };
//+------------------------------------------------------------------+

