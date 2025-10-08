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
                        public IMTConPluginSink,
                        public IMTTradeSink
  {
private:
   //--- plugin data
   MTServerInfo      m_info;
   IMTServerAPI*     m_api;
   //--- temp objects
   IMTConPlugin*     m_config;
   IMTConParam*      m_config_param;
   //--- parameters
   double            m_min_freemargin;
   double            m_min_interest;
   double            m_max_interest;

public:
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- plugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* server);
   virtual MTAPIRES  Stop(void);
   //--- plugin configuration update
   virtual void      OnPluginUpdate(const IMTConPlugin* plugin);
   //--- end of day interest rate calculation
   virtual MTAPIRES  HookTradeInterest(const int64_t         datetime,
                                       const IMTConGroup*  group,
                                       const IMTAccount*   account,
                                       const double        original_value,
                                       double&             new_value);
   //--- end of month interest rate charge
   virtual MTAPIRES  HookTradeInterestCharge(const int64_t         datetime,
                                             const IMTConGroup*  group,
                                             const IMTUser*      user,
                                             const double        original_value,
                                             double&             new_value);
private:
   MTAPIRES          ParametersRead(void);
   MTAPIRES          ParametersGetFloat(LPCWSTR name,double& value,double value_default=0);
   MTAPIRES          ParametersGetInt(LPCWSTR name,int64_t& value,int64_t value_default=0);
   MTAPIRES          ParametersGetString(LPCWSTR name,CMTStr& value,LPCWSTR value_default=L"");
  };
//+------------------------------------------------------------------+

