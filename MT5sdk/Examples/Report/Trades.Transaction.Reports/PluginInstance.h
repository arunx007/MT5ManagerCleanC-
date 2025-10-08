//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "TransactionBase.h"
//--- default params
#define DEFAULT_SYMBOLS     L"*"
#define DEFAULT_GROUPS      L"*,!demo*,!contest*"
#define DEFAULT_DAILY_TIME  L"23:59:59"
#define DEFAULT_DAILY_PATH  L"confirms"
//+------------------------------------------------------------------+
//| Description of an order from dealer with a request execution via |
//| the gateway                                                      |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct DealerOrder
  {
   uint64_t          request;          // request id
   int64_t           ctm;              // time
   uint64_t          order;            // order
   uint32_t          action;           // action
   uint64_t          login;            // client
   uint64_t          dealer;           // dealer
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,
                        public IMTConPluginSink,
                        public IMTTradeSink,
                        public IMTDealSink,
                        public IMTEndOfDaySink
  {
private:
   //--- data type
   typedef TMTArray<DealerOrder,1024> DealerOrderArray;
   //--- constants
   enum constants
     {
      DEALER_ORDERS_TIMEOUT=SECONDS_IN_DAY,
      BASE_WAIT_STANDBY_TIMEOUT=5000,
     };

private:
   CMTSync           m_sync;
   //--- server API interface
   IMTServerAPI     *m_api;
   //--- parameters
   CMTStr256         m_group_mask;
   CMTStr32          m_symbol_mask;
   CMTStr256         m_daily_path;
   int64_t           m_daily_time;
   //--- database data
   CMTStrPath        m_base_name;
   CMTStrPath        m_base_path;
   CTransactionBase  m_base;
   //--- plugin config interface
   IMTConPlugin     *m_config;
   IMTConParam      *m_param;
   IMTConSymbol     *m_symbol;
   IMTConGroup      *m_group;
   IMTUser          *m_user;
   //--- daily reports
   volatile bool     m_daily_workflag;
   CMTThread         m_daily_thread;
   int64_t           m_daily_last;
   wchar_t           m_daily_buf[16*KB];
   //--- array of orders from dealer with a request execution via the gateway
   DealerOrderArray  m_dealer_orders;
   CMTSync           m_dealer_orders_sync;
   int64_t           m_dealer_orders_last;

public:
   //--- constructor/destructor
                     CPluginInstance(void);
   virtual          ~CPluginInstance(void);
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* api);
   virtual MTAPIRES  Stop(void);

private:
   //--- check masks
   bool              CheckGroup(LPCWSTR group);
   bool              CheckSymbol(LPCWSTR symbol);
   //--- read parameter of plugin
   MTAPIRES          ReadParams(void);
   //--- IMTConPluginSinc interface implementation
   virtual void      OnPluginUpdate(const IMTConPlugin* plugin);
   //--- IMTEndOfDay 
   virtual void      OnEODGroupFinish(const int64_t datetime,const int64_t prev_datetime,const IMTConGroup* group);
   virtual void      OnEODFinish(const int64_t datetime,const int64_t prev_datetime);
   //--- IMTDealSink
   virtual void      OnDealPerform(const IMTDeal* deal,IMTAccount* account,IMTPosition* position);
   void              OnDealPerformBalance(const IMTDeal* deal,IMTAccount* account);
   //--- IMTTradeSink interface implementation
   virtual void      OnTradeRequestProcess(const IMTRequest*   request,
                                           const IMTConfirm*   confirm,
                                           const IMTConGroup*  group,
                                           const IMTConSymbol* symbol,
                                           const IMTPosition*  position,
                                           const IMTOrder*     order,
                                           const IMTDeal*      deal);
   //--- trade request process event
   virtual void      OnTradeRequestProcessCloseBy(const IMTRequest*   request,
                                                  const IMTConfirm*   confirm,
                                                  const IMTConGroup*  group,
                                                  const IMTConSymbol* symbol,
                                                  const IMTPosition*  position,
                                                  const IMTOrder*     order,
                                                  const IMTDeal*      deal,
                                                  const IMTDeal*      deal_by);
   //---
   virtual void      OnTradeExecution(const IMTConGateway* gateway,
                                      const IMTExecution*  execution,
                                      const IMTConGroup*   group,
                                      const IMTConSymbol*  symbol,
                                      const IMTPosition*   position,
                                      const IMTOrder*      order,
                                      const IMTDeal*       deal);
   //---
   virtual void      OnTradeRequestAdd(const IMTRequest*   request,
                                       const IMTConGroup*  group,
                                       const IMTConSymbol* symbol,
                                       const IMTPosition*  position,
                                       const IMTOrder*     order);
   virtual void      OnTradeRequestDelete(const IMTRequest* request);
   //---
   virtual void      OnTradeRequestRefuse(const IMTRequest* request);
   //--- process transactions
   bool              ProcessMarket(const IMTDeal *deal,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record);
   bool              ProcessPositionModify(const IMTPosition *position,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record);
   bool              ProcessPendingOrder(const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record);
   bool              ProcessOrderModify(const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record);
   bool              ProcessOrderRemove(const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record);
   bool              ProcessExpiration(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ProcessOrderActivation(const IMTDeal *deal,const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record);
   bool              ProcessSLActivation(const IMTDeal *deal,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ProcessTPActivation(const IMTDeal *deal,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ProcessBalance(const IMTDeal *deal,const IMTConSymbol *symbol,const IMTConGroup *group,TransactionRecord& record);
   bool              ProcessStopLimit(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ProcessOrderStopOut(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ProcessPositionStopOut(const IMTDeal *deal,const IMTConSymbol *symbol,TransactionRecord& record);
   //--- process rejects
   void              ProcessReject(const IMTRequest *request);
   //--- execution transaction
   bool              ExecutionOrderNew(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ExecutionOrderFill(const IMTDeal *deal,const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ExecutionOrderReject(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ExecutionOrderModify(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ExecutionOrderModifyReject(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ExecutionOrderCancel(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   bool              ExecutionOrderCancelReject(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record);
   //--- daily report processing
   void              DailyStart(void);
   void              DailyStop(void);
   bool              DailyProcess(const int64_t ctm,const int64_t prev,const bool eod);
   bool              DailyWritePrepare(CMTFile& file);
   bool              DailyWrite(CMTFile& file,TransactionRecord& record);
   //--- daily thread
   static uint32_t __stdcall DailyThreadWrapper(LPVOID param);
   void              DailyThread(void);
   //--- parameter update
   bool              ParamUpdate(IMTConPlugin *config,IMTConParam *param);
   //--- calculation
   double            CalcAmountDeal(const IMTConSymbol *symbol,const IMTDeal *deal) const;
   double            CalcAmountClosed(const IMTConSymbol *symbol,const IMTDeal *deal) const;
   double            CalcAmountOrder(const IMTConSymbol *symbol,const IMTOrder *order) const;
   double            CalcAmountPosition(const IMTConSymbol *symbol,const IMTPosition *position) const;
   double            CalcMarginAmount(const IMTConSymbol *symbol,const IMTDeal *deal,const uint64_t volume_ext) const;
   //--- mapping dealer request
   void              DealerOrderAdd(const IMTRequest *request,const IMTOrder *order);
   void              DealerOrderDelete(const IMTRequest *request);
   uint64_t          DealerOrderFind(const IMTExecution *execution,const IMTOrder *order);
   void              DealerOrderRefresh(void);
   //--- handling the array of orders from dealer
   static int32_t    SortDealerOrders(const void* left,const void* right);
   static int32_t    SearchDealerOrders(const void* id,const void* order);
  };
//+------------------------------------------------------------------+
