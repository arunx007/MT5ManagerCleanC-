//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "stdafx.h"
#include "TradeDispatcher.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeDispatcher::CTradeDispatcher(void)
   : m_api_gateway(NULL),
     m_api_exchange(NULL),
     m_confirm_gateway(NULL),
     m_confirm_exchange(NULL),
     m_execution(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeDispatcher::~CTradeDispatcher(void)
  {
//--- shutdown everything
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CTradeDispatcher::Initialize(IMTGatewayAPI *api_gateway,CExchangeAPI *api_exchange)
  {
   bool res=true;
//--- check
   if(!api_gateway || !api_exchange)
      return(false);
//--- shutdown dispatcher
   Shutdown();
//--- remember pointers to Gateway API and Exchange API
   m_api_gateway =api_gateway;
   m_api_exchange=api_exchange;
//--- create interfaces to confirm requests by gateway and external system
   m_confirm_gateway =m_api_gateway->DealerConfirmCreate();
   m_confirm_exchange=m_api_gateway->DealerConfirmCreate();
   m_execution=m_api_gateway->DealerExecutionCreate();
//--- check
   if(!m_confirm_gateway || !m_confirm_exchange)
     {
      ExtLogger.OutString(MTLogErr,L"failed to create confirm interface instance");
      res=false;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CTradeDispatcher::Shutdown(void)
  {
//--- release confirmation object
   if(m_confirm_gateway)
     {
      m_confirm_gateway->Release();
      m_confirm_gateway=NULL;
     }
//--- release confirmation object
   if(m_confirm_exchange)
     {
      m_confirm_exchange->Release();
      m_confirm_exchange=NULL;
     }
//--- clear pointers to Gateway API
   m_api_gateway=NULL;
  }
//+------------------------------------------------------------------+
//| Process selection of request                                     |
//+------------------------------------------------------------------+
void CTradeDispatcher::OnGatewayDealerLock(const MTAPIRES retcode,const IMTRequest *request)
  {
   MTAPISTR str={0};
//--- check
   if(!request)
     {
      ExtLogger.OutString(MTLogErr,L"invalid request interface");
      return;
     }
//--- check response code
   if(retcode!=MT_RET_OK)
     {
      if(retcode==MT_RET_OK_NONE)
         ExtLogger.Out(MTLogWarn,L"request #%u does not exist on server",request->ID());
      else
         ExtLogger.Out(MTLogErr,L"request #%u, failed to select %s(%d)",request->ID(),request->Print(str),retcode);
      return;
     }
//--- log full info about received request
   LogRequest(request);
//--- process request
   GatewayProcess(request);
  }
//+------------------------------------------------------------------+
//| Process confirmation of request                                  |
//+------------------------------------------------------------------+
void CTradeDispatcher::OnGatewayDealerAnswer(const MTAPIRES retcode,const IMTConfirm *confirm) const
  {
   MTAPISTR str={0};
//--- check
   if(!confirm)
      return;
//--- log result
   if(retcode!=MT_RET_OK)
      ExtLogger.Out(MTLogErr,L"failed to confirm request %s(%d)",confirm->Print(str),retcode);
  }
//+------------------------------------------------------------------+
//| Notify of order transaction in external trading system           |
//+------------------------------------------------------------------+
bool CTradeDispatcher::OnExchangeOrderTrans(const ExchangeOrder &exchange_order,const ExchangeSymbol &symbol)
  {
   bool res=true;
//--- analyze order state
   switch(exchange_order.order_state)
     {
      case ExchangeOrder::ORDER_STATE_CONFIRMED:
        {
         //--- send confirmation only
         res=res && SendOrderConfirm(exchange_order);
         break;
        }
      case ExchangeOrder::ORDER_STATE_REQUEST_PLACED:
      case ExchangeOrder::ORDER_STATE_REQUEST_MODIFY:
      case ExchangeOrder::ORDER_STATE_REQUEST_CANCEL:
        {
         //--- send confirmation and execution
         res=res && SendOrderConfirm(exchange_order);
         res=res && SendOrderExecution(exchange_order);
         break;
        }
      case ExchangeOrder::ORDER_STATE_NEW:
      case ExchangeOrder::ORDER_STATE_MODIFY:
      case ExchangeOrder::ORDER_STATE_CANCEL:
      case ExchangeOrder::ORDER_STATE_REJECT_NEW:
      case ExchangeOrder::ORDER_STATE_REJECT_MODIFY:
      case ExchangeOrder::ORDER_STATE_REJECT_CANCEL:
        {
         //--- send execution only
         res=res && SendOrderExecution(exchange_order);
         break;
        }
      default:
        {
         res=false;
        }
     }
   return(res);
  }
//+------------------------------------------------------------------+
//| Notify of deal transaction in external trading system            |
//+------------------------------------------------------------------+
bool CTradeDispatcher::OnExchangeDealTrans(const ExchangeDeal &exchange_deal,const ExchangeSymbol &symbol)
  {
   return(SendDealExecution(exchange_deal,symbol));
  }
//+------------------------------------------------------------------+
//| Process requests                                                 |
//+------------------------------------------------------------------+
void CTradeDispatcher::GatewayProcess(const IMTRequest *request)
  {
//--- check pointer to interface of trade request
   if(!request)
     {
      ExtLogger.OutString(MTLogErr,L"invalid request interface");
      return;
     }
//--- check external trading system API
   if(!m_api_exchange)
     {
      ExtLogger.OutString(MTLogErr,L"invalid Exchange API");
      return;
     }
//--- symbol
   ExchangeSymbol symbol={0};
//--- check the presence of a symbol a request is assigned to
   if(!m_api_exchange->SymbolGet(request->Symbol(),symbol))
     {
      ExtLogger.Out(MTLogErr,L"symbol %s not found",request->Symbol());
      //--- send request error
      SendRequestConfirm(MT_RET_REQUEST_ERROR,request);
      return;
     }
//--- check if a request can be confirmed automatically
   if(GatewayProcessAuto(request))
     {
      //--- confirm request
      SendRequestConfirm(MT_RET_REQUEST_DONE,request);
      return;
     }
//--- execute request at an exchange
   if(!GatewayExecuteExchange(request))
     {
      //--- unable to send request to external system, send request error
      SendRequestConfirm(MT_RET_REQUEST_ERROR,request);
     }
  }
//+------------------------------------------------------------------+
//| Check if request can be executed automatically                   |
//+------------------------------------------------------------------+
bool CTradeDispatcher::GatewayProcessAuto(const IMTRequest *request)
  {
   bool stop_order=false,confirm_auto=false;
//--- check
   if(!request)
      return(false);
//--- generate the flag on whether it is a request for a stop order or for a stop limit order
   if(request->Type()==IMTOrder::OP_BUY_STOP ||
      request->Type()==IMTOrder::OP_SELL_STOP ||
      request->Type()==IMTOrder::OP_BUY_STOP_LIMIT ||
      request->Type()==IMTOrder::OP_SELL_STOP_LIMIT)
     {
      stop_order=true;
     };
//--- analyze action types to generate the flag on whether a request can be confirmed automatically
   uint32_t action=request->Action();
   switch(action)
     {
      //--- only pending stop orders are confirmed automatically
      case IMTRequest::TA_PENDING:
        {
         confirm_auto=stop_order;
         break;
        }
      //--- automatically confirm SL and TP modification
      case IMTRequest::TA_SLTP:
        {
         confirm_auto=true;
         break;
        }
      //--- modification
      case IMTRequest::TA_MODIFY:
      case IMTRequest::TA_DEALER_ORD_MODIFY:
        {
         //--- modify automatically if it is a stop order
         if(stop_order)
            confirm_auto=true;
         else
           {
            //--- automatically confirm SL and TP modification
            uint64_t flags=request->Flags();
            if(flags==IMTRequest::TA_FLAG_CHANGED_SL ||
               flags==IMTRequest::TA_FLAG_CHANGED_TP ||
               flags==(IMTRequest::TA_FLAG_CHANGED_SL|IMTRequest::TA_FLAG_CHANGED_TP))
               confirm_auto=true;
           }
         break;
        }
      //--- only stop orders are removed automatically
      case IMTRequest::TA_REMOVE:
      case IMTRequest::TA_DEALER_ORD_REMOVE:
        {
         confirm_auto=stop_order;
         break;
        }
     }
//--- return result
   return(confirm_auto);
  }
//+------------------------------------------------------------------+
//| Execute request at exchange                                      |
//+------------------------------------------------------------------+
bool CTradeDispatcher::GatewayExecuteExchange(const IMTRequest *request)
  {
//--- check
   if(!m_api_exchange)
      return(false);
//--- send order
   ExchangeOrder exchange_order={0};
//--- order action type
   uint32_t request_action=request->Action();
//--- enter the type of operation
   GetOrderActionByRequestAction(request_action,exchange_order.order_action);
//--- order ticket
   exchange_order.order_mt_id=request->Order();
//--- order exchange ticket
   exchange_order.order_exchange_id=_wtoi(request->OrderExternalID());
//--- order id in external system
   exchange_order.request_mt_id=request->ID();
//--- symbol
   CMTStr::Copy(exchange_order.symbol,_countof(exchange_order.symbol),request->Symbol());
//--- client's login
   exchange_order.login=request->Login();
//--- order type
   exchange_order.type_order=request->Type();
//--- expiration type
   exchange_order.type_time=request->TypeTime();
//--- action
   exchange_order.action=request_action;
//--- price
   exchange_order.price_order=request->PriceOrder();
//--- Stop Loss level
   exchange_order.price_SL=request->PriceSL();
//--- Take Profit level
   exchange_order.price_TP=request->PriceTP();
//--- volume
   exchange_order.volume=uint64_t(SMTMath::VolumeToDouble(request->Volume()));
//--- expiration time
   exchange_order.expiration_time=request->TimeExpiration();
//--- if Take Profit activation order arrived
   if(exchange_order.action==IMTRequest::TA_ACTIVATE_TP)
     {
      //--- send pending order
      exchange_order.action=IMTRequest::TA_PENDING;
      //--- order expires in 5 seconds
      exchange_order.expiration_time=GetExchangeTime()+MT_ORDER_ACTIVATION_TIMEOUT;
     }
//--- if Stop Loss or Stop order activation order arrived
   if(exchange_order.action==IMTRequest::TA_ACTIVATE_SL || exchange_order.action==IMTRequest::TA_ACTIVATE)
     {
      //---  order activated on MT side
      if(exchange_order.action==IMTRequest::TA_ACTIVATE)
         exchange_order.order_custom_data=MT_ORDER_ACTIVATION_FLAG;
      //--- send market order
      exchange_order.action=IMTRequest::TA_EXCHANGE;
      //--- reset order price
      exchange_order.price_order=0;
      //--- order expires in 5 seconds
      exchange_order.expiration_time=GetExchangeTime()+MT_ORDER_ACTIVATION_TIMEOUT;
     }
//--- if Stop Limit order activation order arrived
   if(exchange_order.action==IMTRequest::TA_ACTIVATE_STOPLIMIT)
     {
      //--- send pending order
      exchange_order.action=IMTRequest::TA_PENDING;
      //--- set order activation price
      exchange_order.price_order=request->PriceTrigger();
     }
//--- send order to exchange
   return(m_api_exchange->SendOrder(exchange_order));
  }
//+------------------------------------------------------------------+
//| Request confirmation                                             |
//+------------------------------------------------------------------+
bool CTradeDispatcher::SendRequestConfirm(const MTAPIRES retcode,const IMTRequest *request)
  {
   MTAPISTR str={0};
   bool     res=false;
//--- check
   if(!m_api_gateway || !m_confirm_gateway || !request)
     {
      ExtLogger.OutString(MTLogErr,L"failed to confirm trade request");
      return(false);
     }
//--- set confirmation data
   res=m_confirm_gateway->Clear()==MT_RET_OK;
   res=res && (m_confirm_gateway->ID(request->ID())==MT_RET_OK);
   res=res && (m_confirm_gateway->Volume(request->Volume())==MT_RET_OK);
   res=res && (m_confirm_gateway->Price(request->PriceOrder())==MT_RET_OK);
   res=res && (m_confirm_gateway->Retcode(retcode)==MT_RET_OK);
//--- send confirmation
   if(res && m_api_gateway->DealerAnswerAsync(m_confirm_gateway)!=MT_RET_OK)
     {
      ExtLogger.Out(MTLogErr,L"'%I64u': request #%u failed to send confirm %s (%d)",request->Login(),request->ID(),m_confirm_gateway->Print(str),res);
      res=false;
     }
   else
      LogAnswerGateway(request,m_confirm_gateway);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send order confirmation to MT                                    |
//+------------------------------------------------------------------+
bool CTradeDispatcher::SendOrderConfirm(const ExchangeOrder &exchange_order)
  {
   MTAPISTR str={0};
   bool     res=false;
//--- check
   if(!m_api_gateway || !m_confirm_exchange)
     {
      ExtLogger.OutString(MTLogErr,L"failed to confirm trade request");
      return(false);
     }
//--- set confirmation data
   res=m_confirm_exchange->Clear()==MT_RET_OK;
   res=res && (m_confirm_exchange->ID((uint32_t)exchange_order.request_mt_id)==MT_RET_OK);
   CMTStr::FormatStr(str,L"%I64u",exchange_order.order_exchange_id);
   res=res && (m_confirm_exchange->OrderID(str)==MT_RET_OK);
   res=res && (m_confirm_exchange->Volume(SMTMath::VolumeToInt(double(exchange_order.volume)))==MT_RET_OK);
   res=res && (m_confirm_exchange->Price(exchange_order.price_order)==MT_RET_OK);
   res=res && (m_confirm_exchange->TickBid(exchange_order.price_tick_bid)==MT_RET_OK);
   res=res && (m_confirm_exchange->TickAsk(exchange_order.price_tick_ask)==MT_RET_OK);
   res=res && (m_confirm_exchange->Retcode(exchange_order.result)==MT_RET_OK);
//--- send confirmation to MT5
   if(res && m_api_gateway->DealerAnswerAsync(m_confirm_exchange)!=MT_RET_OK)
     {
      ExtLogger.Out(MTLogErr,L"'%I64u': request #%I64u failed to send confirm %s (%d)",exchange_order.login,exchange_order.order_exchange_id,m_confirm_exchange->Print(str),res);
      res=false;
     }
   else
      LogAnswerExchange(exchange_order,m_confirm_exchange);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send execution to MT                                             |
//+------------------------------------------------------------------+
bool CTradeDispatcher::SendOrderExecution(const ExchangeOrder &exchange_order)
  {
//--- check validity of MT5 and IMTExecution interfaces
   if(!m_api_gateway || !m_execution)
     {
      ExtLogger.OutString(MTLogErr,L"failed to send execution");
      return(false);
     }
   bool res=true;
//--- clear fields before use
   res=(m_execution->Clear()==MT_RET_OK);
//--- enter client's login
   res=res && (m_execution->Login(exchange_order.login)==MT_RET_OK);
//--- enter order ID
   res=res && (m_execution->Order(exchange_order.order_mt_id)==MT_RET_OK);
//--- enter the symbol corresponding to the order
   res=res && (m_execution->Symbol(exchange_order.symbol)==MT_RET_OK);
//--- enter order type
   res=res && (m_execution->OrderType(exchange_order.type_order)==MT_RET_OK);
//--- if a valid ID in an external system is received
   if(exchange_order.order_exchange_id)
     {
      //--- ID in an external trading system
      CMTStr32 external_id;
      //--- generate ID string
      external_id.Format(L"%I64u",exchange_order.order_exchange_id);
      //--- enter ID in an external trading system
      res=res && (m_execution->OrderExternalID(external_id.Str())==MT_RET_OK);
     }
//--- depending on the order state 
   switch(exchange_order.order_state)
     {
      case ExchangeOrder::ORDER_STATE_REQUEST_PLACED:
        {
         //--- send execution with TE_ORDER_NEW_REQUEST type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_NEW_REQUEST)==MT_RET_OK);
         //--- enter order activation condition
         res=res && (m_execution->OrderActivationFlags(IMTOrder::ACTIV_FLAGS_NO_LIMIT|
                                                       IMTOrder::ACTIV_FLAGS_NO_EXPIRATION)==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_NEW:
        {
         //--- send execution with TE_ORDER_NEW type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_NEW)==MT_RET_OK);
         //--- enter order price
         res=res && (m_execution->OrderPrice(exchange_order.price_order)==MT_RET_OK);
         //--- enter order volume
         res=res && (m_execution->OrderVolume(SMTMath::VolumeToInt(double(exchange_order.volume)))==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_REQUEST_MODIFY:
        {
         //--- send execution with TE_ORDER_MODIFY_REQUEST type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_MODIFY_REQUEST)==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_MODIFY:
        {
         //--- send execution with TE_ORDER_MODIFY type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_MODIFY)==MT_RET_OK);
         //--- enter order price if it changed
         if(exchange_order.price_order!=0)
            res=res && (m_execution->OrderPrice(exchange_order.price_order)==MT_RET_OK);
         if(exchange_order.price_SL!=0)
            res=res && (m_execution->OrderPriceSL(exchange_order.price_SL)==MT_RET_OK);
         if(exchange_order.price_TP!=0)
            res=res && (m_execution->OrderPriceTP(exchange_order.price_TP)==MT_RET_OK);
         if(exchange_order.expiration_time!=0)
            res=res && (m_execution->OrderTimeExpiration(exchange_order.expiration_time)==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_REQUEST_CANCEL:
        {
         //--- send execution with TE_ORDER_CANCEL_REQUEST type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_CANCEL_REQUEST)==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_CANCEL:
        {
         if((exchange_order.order_custom_data&MT_ORDER_ACTIVATION_FLAG)==0)
           {
            //--- send execution with TE_ORDER_CANCEL type
            res=res && (m_execution->Action(IMTExecution::TE_ORDER_CANCEL)==MT_RET_OK);
           }
         else
           {
            //--- send execution with TE_ORDER_MODIFY type
            res=res && (m_execution->Action(IMTExecution::TE_ORDER_MODIFY)==MT_RET_OK);
            //--- reset order activation type to activate non-filled volume
            res=res && (m_execution->OrderActivationMode(IMTOrder::ACTIVATION_NONE)==MT_RET_OK);
           }
         break;
        }
      case ExchangeOrder::ORDER_STATE_REJECT_NEW:
        {
         //--- send execution with TE_ORDER_CANCEL type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_REJECT)==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_REJECT_MODIFY:
        {
         //--- send execution with TE_ORDER_CANCEL type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_MODIFY_REJECT)==MT_RET_OK);
         break;
        }
      case ExchangeOrder::ORDER_STATE_REJECT_CANCEL:
        {
         //--- send execution with TE_ORDER_CANCEL type
         res=res && (m_execution->Action(IMTExecution::TE_ORDER_CANCEL_REJECT)==MT_RET_OK);
         break;
        }
      default:
        {
         res=false;
         break;
        }
     }
//--- send execution to MT5
   MTAPIRES result=m_api_gateway->DealerExecuteAsync(m_execution);
//--- logging
   LogExecution(result,m_execution);
//--- return the result
   return(res && result==MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Send a deal to MT                                                |
//+------------------------------------------------------------------+
bool CTradeDispatcher::SendDealExecution(const ExchangeDeal &exchange_deal,const ExchangeSymbol &symbol)
  {
   CMTStr32 str;
//--- clear fields before use
   bool res=(m_execution->Clear()==MT_RET_OK);
//--- enter client's login
   res=res && (m_execution->Login(exchange_deal.login)==MT_RET_OK);
//--- enter operation type
   res=res && (m_execution->Action(IMTExecution::TE_ORDER_FILL)==MT_RET_OK);
//--- enter order exchange ID   
   str.Format(L"%I64u",exchange_deal.order);
   res=res && (m_execution->OrderExternalID(str.Str())==MT_RET_OK);
//--- enter symbol
   res=res && (m_execution->Symbol(exchange_deal.symbol)==MT_RET_OK);
//--- enter deal operation type
   res=res && (m_execution->DealAction(exchange_deal.type_deal)==MT_RET_OK);
//--- enter deal volume
   res=res && (m_execution->DealVolume(SMTMath::VolumeToInt(double(exchange_deal.volume)))==MT_RET_OK);
//--- enter non-filled deal volume
   res=res && (m_execution->DealVolumeRemaind(SMTMath::VolumeToInt(double(exchange_deal.volume_remaind)))==MT_RET_OK);
//--- enter lot price
   res=res && (m_execution->DealPrice(exchange_deal.price)==MT_RET_OK);
//--- enter order activation condition
   res=res && (m_execution->OrderActivationFlags(IMTOrder::ACTIV_FLAGS_NONE)==MT_RET_OK);
//--- if a valid ID in an external system is received
   if(exchange_deal.deal_exchange_id!=0)
     {
      //--- ID in an external trading system
      CMTStr32 external_id;
      //--- generate ID string
      external_id.Format(L"%d",exchange_deal.deal_exchange_id);
      //--- enter ID in an external trading system
      res=res && (m_execution->DealExternalID(external_id.Str())==MT_RET_OK);
     }
//--- send deal to MT5
   MTAPIRES result=m_api_gateway->DealerExecuteAsync(m_execution);
//--- logging
   LogExecution(result,m_execution);
//--- return the result
   return(res && result==MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Enter operation type based on the operation in the request       |
//+------------------------------------------------------------------+
void CTradeDispatcher::GetOrderActionByRequestAction(const uint32_t &mt_request_action,uint32_t &exhange_order_action)
  {
//--- analyze action type
   switch(mt_request_action)
     {
      case IMTRequest::TA_PRICE             :
      case IMTRequest::TA_INSTANT           :
      case IMTRequest::TA_MARKET            :
      case IMTRequest::TA_EXCHANGE          :
      case IMTRequest::TA_PENDING           :
      case IMTRequest::TA_ACTIVATE          :
      case IMTRequest::TA_ACTIVATE_SL       :
      case IMTRequest::TA_ACTIVATE_TP       :
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
      case IMTRequest::TA_STOPOUT_POSITION  :
      case IMTRequest::TA_DEALER_POS_EXECUTE:
      case IMTRequest::TA_DEALER_ORD_PENDING:
        {
         //--- add a new order
         exhange_order_action=ExchangeOrder::ORDER_ACTION_ADD;
         break;
        }
      case IMTRequest::TA_MODIFY            :
      case IMTRequest::TA_DEALER_ORD_MODIFY :
        {
         //--- modify an order
         exhange_order_action=ExchangeOrder::ORDER_ACTION_MODIFY;
         break;
        }
      case IMTRequest::TA_EXPIRATION        :
      case IMTRequest::TA_REMOVE            :
      case IMTRequest::TA_STOPOUT_ORDER     :
      case IMTRequest::TA_DEALER_ORD_REMOVE :
        {
         //--- delete an order
         exhange_order_action=ExchangeOrder::ORDER_ACTION_CANCEL;
         break;
        }
      default:
         break;
     }
  }
//+------------------------------------------------------------------+
//| Get current time of exchange                                     |
//+------------------------------------------------------------------+
int64_t CTradeDispatcher::GetExchangeTime(void)
  {
//--- use UTC for exchange time
   return(_time64(nullptr));
  }
//+------------------------------------------------------------------+
//| Log request                                                      |
//+------------------------------------------------------------------+
bool CTradeDispatcher::LogRequest(const IMTRequest *request) const
  {
   MTAPISTR str={0};
//--- check
   if(!request)
      return(false);
//--- notify of request received
   if(request->Order())
      ExtLogger.Out(MTLogOK,L"'%I64u': request #%u received (#%I64u %s)",request->Login(),request->ID(),request->Order(),request->Print(str));
   else
      ExtLogger.Out(MTLogOK,L"'%I64u': request #%u received (%s)",request->Login(),request->ID(),request->Print(str));
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Log response                                                     |
//+------------------------------------------------------------------+
bool CTradeDispatcher::LogAnswerGateway(const IMTRequest *request,const IMTConfirm *confirm) const
  {
   MTAPISTR str={0},tmp={0};
   CMTStrPath txt;
//--- check
   if(!request || !confirm)
      return(false);
//--- format message
   if(request->Order())
      txt.Format(L"%s (#%I64u %s)",confirm->Print(str),request->Order(),request->Print(tmp));
   else
      txt.Format(L"%s (%s)",confirm->Print(str),request->Print(tmp));
//--- log formatted message
   ExtLogger.Out(MTLogOK,L"'%I64u': request #%u answered - %s",request->Login(),request->ID(),txt.Str());
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Log response                                                     |
//+------------------------------------------------------------------+
bool CTradeDispatcher::LogAnswerExchange(const ExchangeOrder &exchange_order,const IMTConfirm *confirm) const
  {
   MTAPISTR str={0};
//--- check
   if(!confirm)
      return(false);
//--- print formatted message
   ExtLogger.Out(MTLogOK,L"'%I64u': request #%I64u answered - %s",exchange_order.login,exchange_order.request_mt_id,confirm->Print(str));
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Execution logging                                                |
//+------------------------------------------------------------------+
bool CTradeDispatcher::LogExecution(MTAPIRES retcode,const IMTExecution *execution) const
  {
   MTAPISTR str={0};
//--- check
   if(!execution)
      return(false);
//--- exit if no trading actions are performed
   if(retcode==MT_RET_OK_NONE)
      return(true);
//--- check result
   if(retcode==MT_RET_OK)
      ExtLogger.Out(MTLogOK,L"'%I64u': execution send complete - %s",execution->Login(),execution->Print(str));
   else
      ExtLogger.Out(MTLogErr,L"'%I64u': execution send failed - %s(%d)",execution->Login(),execution->Print(str),retcode);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
