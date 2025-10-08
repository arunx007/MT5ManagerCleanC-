//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Class of trade dispatcher                                        |
//+------------------------------------------------------------------+
class CTradeDispatcher
  {

private:
   //--- enumerate order flags
   enum EnTradeDispatcherConstants
     {
      MT_ORDER_ACTIVATION_FLAG   =0x01,            // order activated on MT side
      MT_ORDER_ACTIVATION_TIMEOUT=5                // activation order expiration time
     };

private:
   //--- Gateway API interface
   IMTGatewayAPI    *m_api_gateway;
   //--- external trading system interface
   CExchangeAPI     *m_api_exchange;
   //--- interfaces for confirming trade requests
   IMTConfirm       *m_confirm_gateway;
   IMTConfirm       *m_confirm_exchange;
   //--- interface for sending operations on the waiting order
   IMTExecution     *m_execution;

public:
   //--- constructor/destructor
                     CTradeDispatcher();
                    ~CTradeDispatcher(void);
   //--- initialization/shutdown
   bool              Initialize(IMTGatewayAPI *api_gateway,CExchangeAPI *api_exchange);
   void              Shutdown(void);
   //---
   //--- methods called by Gateway API
   //---
   //--- process selection of request
   void              OnGatewayDealerLock(const MTAPIRES retcode,const IMTRequest *request);
   //--- process confirmation of request
   void              OnGatewayDealerAnswer(const MTAPIRES retcode,const IMTConfirm *confirm) const;
   //---
   //--- methods of notification from external trading system
   //---
   //--- notify of order transaction in external trading system
   bool              OnExchangeOrderTrans(const ExchangeOrder &exchange_order,const ExchangeSymbol &symbol);
   //--- notify of deal transaction in external trading system
   bool              OnExchangeDealTrans(const ExchangeDeal &exchange_deal,const ExchangeSymbol &symbol);

private:
   //--- process trade requests
   void              GatewayProcess(const IMTRequest *request);
   //--- check if request can be executed automatically
   bool              GatewayProcessAuto(const IMTRequest *request);
   //--- execute request at exchange
   bool              GatewayExecuteExchange(const IMTRequest *request);
   //--- generate responses for request
   bool              SendRequestConfirm(const MTAPIRES retcode,const IMTRequest *request);
   //--- send order confirmation to MT
   bool              SendOrderConfirm(const ExchangeOrder &exchange_order);
   //--- send operation on order to MT
   bool              SendOrderExecution(const ExchangeOrder &exchange_order);
   //--- send a deal to MT
   bool              SendDealExecution(const ExchangeDeal &exchange_deal,const ExchangeSymbol &symbol);
   //--- enter operation type based on the operation in the request
   void              GetOrderActionByRequestAction(const uint32_t &mt_request_action,uint32_t &exhange_order_action);
   //--- get current time of exchange
   int64_t           GetExchangeTime(void);
   //--- log request and response
   bool              LogRequest(const IMTRequest *request) const;
   bool              LogAnswerGateway(const IMTRequest *request,const IMTConfirm *confirm) const;
   bool              LogAnswerExchange(const ExchangeOrder &exchange_order,const IMTConfirm *confirm) const;
   bool              LogExecution(MTAPIRES retcode,const IMTExecution *execution) const;
  };
//+------------------------------------------------------------------+
