//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
#include "PluginContext.h"
#include "NFAOrderRecord.h"
#include "NFATradeRecord.h"
#include "SLTPBase.h"
#include "RequestBase.h"
#include "PluginReports.h"
#include "..\Tools\TextBaseWriter.h"
//+------------------------------------------------------------------+
//| Default plugin parameters                                        |
//+------------------------------------------------------------------+
#define DEFAULT_FDM           L""
#define DEFAULT_SYMBOLS       L"*"
#define DEFAULT_GROUPS        L"*,!demo*,!contest*"
#define DEFAULT_SERVER_ID     L""
#define DEFAULT_SFTP_ADDRESS  L""
#define DEFAULT_SFTP_PORT     L""
#define DEFAULT_SFTP_LOGIN    L""
#define DEFAULT_SFTP_PASS     L""
#define DEFAULT_SFTP_KEY_PATH L""
#define DEFAULT_REPORT_TIME   L"17:00:00"
#define DEFAULT_SEND_TIME     L"18:00:00"
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,
                        public IMTConPluginSink,
                        public IMTTradeSink,
                        public IMTEndOfDaySink
  {
private:
   //---
   CMTSync           m_sync;
   IMTServerAPI     *m_api;
   //--- plugin config interface
   IMTConPlugin     *m_config;
   IMTConParam      *m_param;
   IMTConTime       *m_time;
   //--- plugin context
   PluginContext     m_context;
   //--- databases
   CTextBaseWriter   m_orders;
   CTextBaseWriter   m_trades;
   CRequestBase      m_requests;
   CSLTPBase         m_sltp;
   CPluginReports    m_reports;

public:
   //--- constructor/destructor
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI *api);
   virtual MTAPIRES  Stop(void);
   //--- close day
   void              CheckCloseDay(const tm &batch);
   //--- compact SL/TP database
   void              CompactProcess(void);

private:
   //--- IMTConPluginSink
   virtual void      OnPluginUpdate(const IMTConPlugin *plugin);
   //--- IMTTradeSink
   virtual void      OnTradeRequestAdd(const IMTRequest *request,const IMTConGroup *group,const IMTConSymbol *symbol,const IMTPosition *position,const IMTOrder *order);
   virtual void      OnTradeRequestProcess(const IMTRequest *request,const IMTConfirm *confirm,const IMTConGroup *group,const IMTConSymbol *symbol,const IMTPosition *position,const IMTOrder* order,const IMTDeal *deal);
   virtual void      OnTradeExecution(const IMTConGateway *gateway,const IMTExecution *execution,const IMTConGroup *group,const IMTConSymbol *symbol,const IMTPosition *position,const IMTOrder *order,const IMTDeal *deal);
   virtual void      OnTradeRequestDelete(const IMTRequest *request);
   virtual void      OnTradeRequestRefuse(const IMTRequest *request);
   //--- process orders
   void              ProcessMarket(const IMTOrder *order,const IMTDeal *deal,const bool instant,CNFAOrderRecord& rec_order_add,CNFAOrderRecord& rec_order,CNFATradeRecord& rec_trade);
   void              ProcessActivate(const IMTOrder *order,const IMTDeal *deal,CNFAOrderRecord& rec_order,CNFATradeRecord& rec_trade);
   void              ProcessStopLoss(const IMTOrder *order,const IMTDeal *deal,const IMTPosition *position,CNFAOrderRecord& rec_order_add,CNFAOrderRecord& rec_order,CNFAOrderRecord& rec_sl,CNFAOrderRecord& rec_tp,CNFATradeRecord& rec_trade);
   void              ProcessTakeProfit(const IMTOrder *order,const IMTDeal *deal,const IMTPosition *position,CNFAOrderRecord& rec_order_add,CNFAOrderRecord& rec_order,CNFAOrderRecord& rec_sl,CNFAOrderRecord& rec_tp,CNFATradeRecord& rec_trade);
   void              ProcessUpdateSLTP(const IMTPosition *position,const uint32_t origin,CNFAOrderRecord &rec_sl,CNFAOrderRecord &rec_tp);
   //--- checks
   bool              CheckGroupAndSymbol(LPCWSTR group,LPCWSTR symbol);
   //--- helpers     
   void              PrepareTimes(const int64_t trade_time,tm &ttm_batch,tm &ttm_stamp) const;
   //--- writers
   void              WriteOrder(const CNFAOrderRecord &record,tm &ttm_batch,tm &ttm_stamp);
   void              WriteTrade(const CNFATradeRecord &record,tm &ttm_batch,tm &ttm_stamp);
   //--- plugin parameters
   MTAPIRES          ReadParams(void);
   bool              ParamCheck(LPCWSTR name,LPCWSTR default_value,const uint32_t type);
   bool              ParamGetString(LPCWSTR name,CMTStr &out,LPCWSTR default_value);
   bool              ParamGetGroups(LPCWSTR name,CMTStr &out,LPCWSTR default_value);
   bool              ParamGetSymbols(LPCWSTR name,CMTStr &out,LPCWSTR default_value);
   bool              ParamGetTime(LPCWSTR name,int64_t &out,LPCWSTR default_value);
  };
//+------------------------------------------------------------------+
