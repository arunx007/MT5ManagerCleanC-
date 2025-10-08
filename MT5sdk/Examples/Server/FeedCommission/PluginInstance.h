//+------------------------------------------------------------------+
//|                                           Feed Commission Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,
                        public IMTEndOfDaySink,
                        public IMTConPluginSink,
                        public IMTUserSink
  {
private:
   //--- server API interface
   IMTServerAPI*     m_api;
   CMTSync           m_sync;
   //--- plugin config interface
   IMTConPlugin*     m_config;
   //--- plugin parameters
   CMTStr256         m_symbols;
   CMTStr256         m_group_mask;
   CMTStr128         m_currency;
   double            m_cost;
   double            m_deposit_max;
   double            m_overturn_max;
   bool              m_skip_disabled;
   //--- 
   IMTDealArray     *m_deals;
   IMTConGroup      *m_group;
   IMTConGroupSymbol *m_group_symbol;
   IMTUser          *m_user;
   IMTConSymbol     *m_symbol;

public:
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* api);
   virtual MTAPIRES  Stop(void);
   //--- events
   virtual void      OnUserLogin(LPCWSTR /*ip*/,const IMTUser* /*user*/,const uint32_t /*type*/);
   //--- hooks
   virtual MTAPIRES  HookUserLogin(LPCWSTR /*ip*/,const IMTUser* /*user*/,const uint32_t /*type*/);
   //--- events
   virtual void      OnUserLogout(LPCWSTR /*ip*/,const IMTUser* /*user*/,const uint32_t /*type*/);

private:
   MTAPIRES          ParametersRead(void);
   //--- IMTConPluginSinc interface implementation
   virtual void      OnPluginUpdate(const IMTConPlugin* plugin);
   //--- IMTEndOfDaySink interface implementation
   virtual void      OnEOMFinish(const int64_t datetime,const int64_t prev_datetime);
   //--- calculate cost of deal in currency
   double            CalcAmountRaw(const IMTConSymbol *symbol_deal,const LPCWSTR currency,const IMTDeal *deal) const;
   //--- check symbol by mask
   bool              SymbolCheck(LPCWSTR mask,LPCWSTR group_mask) const;
   //--- convert currency
   double            Convert(const double value,LPCWSTR base,LPCWSTR currency) const;
  };
//+------------------------------------------------------------------+