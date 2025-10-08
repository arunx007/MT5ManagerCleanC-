//+------------------------------------------------------------------+
//|                                         MetaTrader 5 NFA.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL),m_config(NULL),m_param(NULL),m_time(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPluginInstance::~CPluginInstance(void)
  {
   Stop();
  }
//+------------------------------------------------------------------+
//| Plugin release function                                          |
//+------------------------------------------------------------------+
void CPluginInstance::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Plugin start notification function                               |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Start(IMTServerAPI *api)
  {
   MTAPIRES retcode=MT_RET_OK;
//---
   Stop();
   if(!api)
      return(MT_RET_ERR_PARAMS);
   m_api=api;
//--- create persistent API interfaces
   if((m_config=m_api->PluginCreate())==NULL)
      return(MT_RET_ERR_MEM);
   if((m_param=m_api->PluginParamCreate())==NULL)
      return(MT_RET_ERR_MEM);
   if((m_time=m_api->TimeCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- read parameters
   if((retcode=ReadParams())!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogOK,L"read params error [%u]",retcode);
      return(retcode);
     }
//--- subscribe
   if((retcode=m_api->PluginSubscribe(this))!=MT_RET_OK || (retcode=m_api->TradeSubscribe(this))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogOK,L"subscribe error [%u]",retcode);
      return(retcode);
     }
//--- init databases
   if(!m_sltp.Initialize(m_api,m_context))
     {
      m_api->LoggerOutString(MTLogOK,L"initialize SL/TP base error");
      return(MT_RET_ERROR);
     }
   m_requests.Clear();
//--- start report generator
   if(!m_reports.Initialize(this,m_api,m_context))
     {
      m_api->LoggerOutString(MTLogOK,L"initialize reports error");
      return(MT_RET_ERROR);
     }
//---
   m_api->LoggerOutString(MTLogOK,L"plugin started");
//---
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- shutdown report thread
   m_reports.Shutdown();
//--- shutdown databases
   m_sync.Lock();
   m_sltp.Shutdown();
   m_requests.Clear();
//--- unsubscribe 
   if(m_api)
     {
      m_api->PluginUnsubscribe(this);
      m_api->TradeUnsubscribeEOD(this);
      m_api=NULL;
     }
//--- cleanup
   if(m_config) { m_config->Release(); m_config=NULL; }
   if(m_param)  { m_param->Release();  m_param =NULL; }
   if(m_time)   { m_time->Release();   m_time  =NULL; }
//--- ok
   m_sync.Unlock();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Check past batch day is closed. Thread-safe                      |
//+------------------------------------------------------------------+
void CPluginInstance::CheckCloseDay(const tm &ttm_batch)
  {
   m_sync.Lock();
   m_orders.CloseDay(ttm_batch);
   m_trades.CloseDay(ttm_batch);
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Compact process. Thread-safe                                     |
//+------------------------------------------------------------------+
void CPluginInstance::CompactProcess(void)
  {
   m_sync.Lock();
   m_sltp.Compact();
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
void CPluginInstance::OnPluginUpdate(const IMTConPlugin *plugin)
  {
//--- check parameters
   if(plugin==NULL || m_api==NULL || m_config==NULL)
      return;
//--- check is our plugin
   if(CMTStr::Compare(plugin->Name(),m_config->Name())!=0 || plugin->Server()!=m_config->Server())
      return;
//--- shutdown report thread
   m_reports.Shutdown();
//--- shutdown workers
   m_sync.Lock();
   m_sltp.Shutdown();
//--- read params
   ReadParams();
//--- init with new parameters
   m_reports.Initialize(this,m_api,m_context);
   m_sltp.Initialize(m_api,m_context);
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Server API event                                                 |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestAdd(const IMTRequest *request,const IMTConGroup *group,const IMTConSymbol *symbol,const IMTPosition *position,const IMTOrder *order)
  {
   if(!request || !group || !symbol || !order || !m_api)
      return;
//--- check
   if(!CheckGroupAndSymbol(group->Group(),symbol->Symbol()))
      return;
//---
   CNFAOrderRecord rec_order;
   switch(request->Action())
     {
      case IMTRequest::TA_ACTIVATE:
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
      case IMTRequest::TA_DEALER_ORD_ACTIVATE:
         rec_order.FillPending(CNFAOrderRecord::TRANS_UNDEFINED,order,CNFAOrderRecord::ORIGIN_AUTO);
         m_sync.Lock();
         m_requests.Add(request->ID(),rec_order);
         m_sync.Unlock();
         break;
      default:
         return;
     }
  }
//+------------------------------------------------------------------+
//| Server API event                                                 |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestProcess(const IMTRequest *request,const IMTConfirm *confirm,const IMTConGroup *group,const IMTConSymbol *symbol,const IMTPosition *position,const IMTOrder* order,const IMTDeal *deal)
  {
   if(!request || !group || !symbol || !m_api)
      return;
//--- check
   if(!CheckGroupAndSymbol(group->Group(),symbol->Symbol()))
      return;
//---
   CNFAOrderRecord rec_order_add,rec_order,rec_sl,rec_tp;
   CNFATradeRecord rec_trade;
//--- get time
   int64_t time_server=m_api->TimeCurrent(),time_add=order ? order->TimeSetup() : 0,time_process=deal ? deal->Time() : 0;
//--- lock
   m_sync.Lock();
//--- parse actions
   switch(request->Action())
     {
      case IMTRequest::TA_REQUEST:
      case IMTRequest::TA_INSTANT:
      case IMTRequest::TA_MARKET:
      case IMTRequest::TA_EXCHANGE:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE || request->ResultRetcode()==MT_RET_REQUEST_DONE_PARTIAL)
           {
            ProcessMarket(order,deal,request->Action()==IMTRequest::TA_INSTANT,rec_order_add,rec_order,rec_trade);
            ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_CUST,rec_sl,rec_tp);
           }
         break;
      case IMTRequest::TA_PENDING:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_ADDED,order,CNFAOrderRecord::ORIGIN_CUST);
         break;
      case IMTRequest::TA_SLTP:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_CUST,rec_sl,rec_tp);
         break;
      case IMTRequest::TA_MODIFY:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_MODIFY,order,CNFAOrderRecord::ORIGIN_CUST);
         break;
      case IMTRequest::TA_REMOVE:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_CANCELLED,order,CNFAOrderRecord::ORIGIN_CUST);
         break;
         //--- Server actions
      case IMTRequest::TA_ACTIVATE:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE || request->ResultRetcode()==MT_RET_REQUEST_DONE_PARTIAL)
           {
            ProcessActivate(order,deal,rec_order,rec_trade);
            ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_SYSTEM,rec_sl,rec_tp);
           }
         break;
      case IMTRequest::TA_ACTIVATE_SL:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE || request->ResultRetcode()==MT_RET_REQUEST_DONE_PARTIAL)
            ProcessStopLoss(order,deal,position,rec_order_add,rec_order,rec_sl,rec_tp,rec_trade);
         break;
      case IMTRequest::TA_ACTIVATE_TP:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE || request->ResultRetcode()==MT_RET_REQUEST_DONE_PARTIAL)
            ProcessTakeProfit(order,deal,position,rec_order_add,rec_order,rec_sl,rec_tp,rec_trade);
         break;
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
           {
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_MODIFY,order,CNFAOrderRecord::ORIGIN_AUTO);
            rec_order.SetOrderType(CNFAOrderRecord::ORDER_LIMIT);
           }
         break;
      case IMTRequest::TA_STOPOUT_ORDER:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_DELETED,order,CNFAOrderRecord::ORIGIN_SYSTEM);
         break;
      case IMTRequest::TA_STOPOUT_POSITION:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
           {
            rec_order_add.FillMarket(CNFAOrderRecord::TRANS_DLR_OFFSET,false,order,CNFAOrderRecord::ORIGIN_SYSTEM);
            rec_order.FillMarket(CNFAOrderRecord::TRANS_ORDER_FILL,false,order,CNFAOrderRecord::ORIGIN_SYSTEM);
            rec_trade.Fill(rec_order.OrderID(),deal);
            ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_SYSTEM,rec_sl,rec_tp);
           }
         break;
      case IMTRequest::TA_EXPIRATION:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_CANCELLED,order,CNFAOrderRecord::ORIGIN_AUTO);
         break;
         //--- Dealer actions
      case IMTRequest::TA_DEALER_POS_EXECUTE:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE || request->ResultRetcode()==MT_RET_REQUEST_DONE_PARTIAL)
           {
            ProcessMarket(order,deal,symbol && symbol->ExecMode()==IMTConSymbol::EXECUTION_INSTANT,rec_order_add,rec_order,rec_trade);
            ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_DLR,rec_sl,rec_tp);
           }
         break;
      case IMTRequest::TA_DEALER_ORD_PENDING:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order_add.FillPending(CNFAOrderRecord::TRANS_ORDER_ADDED,order,CNFAOrderRecord::ORIGIN_DLR);
         break;
      case IMTRequest::TA_DEALER_POS_MODIFY:
         ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_DLR,rec_sl,rec_tp);
         break;
      case IMTRequest::TA_DEALER_ORD_MODIFY:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_MODIFY,order,CNFAOrderRecord::ORIGIN_DLR);
         break;
      case IMTRequest::TA_DEALER_ORD_REMOVE:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_CANCELLED,order,CNFAOrderRecord::ORIGIN_DLR);
         break;
      case IMTRequest::TA_DEALER_ORD_ACTIVATE:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE || request->ResultRetcode()==MT_RET_REQUEST_DONE_PARTIAL || request->ResultRetcode()==MT_RET_REQUEST_PLACED)
           {
            ProcessActivate(order,deal,rec_order,rec_trade);
            ProcessUpdateSLTP(position,CNFAOrderRecord::ORIGIN_DLR,rec_sl,rec_tp);
           }
         break;
      case IMTRequest::TA_DEALER_ORD_SLIMIT:
         if(request->ResultRetcode()==MT_RET_REQUEST_DONE)
           {
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_MODIFY,order,CNFAOrderRecord::ORIGIN_DLR);
            rec_order.SetOrderType(CNFAOrderRecord::ORDER_LIMIT);
           }
         break;
      default:
         m_sync.Unlock();
         return;
     }
//--- prepare time setup for order
   if(!time_add)
      time_add=time_server;
   tm ttm_batch_add={},ttm_stamp_add={};
   PrepareTimes(time_add,ttm_batch_add,ttm_stamp_add);
//--- prepare execution time
   if(!time_process)
      time_process=time_server;
   tm ttm_stamp={},ttm_batch={};
   PrepareTimes(time_process,ttm_batch,ttm_stamp);
//--- setup bid/ask
   rec_order_add.SetBidAsk(request);
   rec_order.SetBidAsk(request);
   rec_sl.SetBidAsk(request);
   rec_tp.SetBidAsk(request);
   rec_trade.SetBidAsk(request);
//--- write records
   WriteOrder(rec_order_add,ttm_batch_add,ttm_stamp_add);
   WriteOrder(rec_order,ttm_batch,ttm_stamp);
   WriteOrder(rec_sl,ttm_batch,ttm_stamp);
   WriteOrder(rec_tp,ttm_batch,ttm_stamp);
   WriteTrade(rec_trade,ttm_batch,ttm_stamp);
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Server API event                                                 |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeExecution(const IMTConGateway *gateway,const IMTExecution *execution,const IMTConGroup *group,const IMTConSymbol *symbol,const IMTPosition *position,const IMTOrder *order,const IMTDeal *deal)
  {
   if(!execution || !order || !m_api)
      return;
//--- check
   if(!CheckGroupAndSymbol(execution->Group(),execution->Symbol()))
      return;
//---
   CNFAOrderRecord rec_order_add,rec_order,rec_sl,rec_tp;
   CNFATradeRecord rec_trade;
//--- get time
   int64_t time_process=deal ? deal->Time() : m_api->TimeCurrent();
//---
   m_sync.Lock();
//--- check action
   switch(execution->Action())
     {
      case IMTExecution::TE_ORDER_NEW:
         //--- skip SL/TP
         if(order && order->Reason()!=IMTOrder::ORDER_REASON_SL && order->Reason()!=IMTOrder::ORDER_REASON_TP)
           {
            //--- market or pending?
            if(order->Type()==IMTOrder::OP_BUY || order->Type()==IMTOrder::OP_SELL)
               rec_order.FillMarket(CNFAOrderRecord::TRANS_ORDER_ADDED,false,order,CNFAOrderRecord::ORIGIN_AUTO);
            else
               rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_ADDED,order,CNFAOrderRecord::ORIGIN_AUTO);
            //--- time
            time_process=order->TimeSetup();
           }
         break;
      case IMTExecution::TE_ORDER_FILL:
         if(order)
           {
            //--- is SL or TP?
            if(order->Reason()==IMTOrder::ORDER_REASON_SL || order->Reason()==IMTOrder::ORDER_REASON_TP)
              {
               if(order->Reason()==IMTOrder::ORDER_REASON_SL)
                  ProcessStopLoss(order,deal,position,rec_order_add,rec_order,rec_sl,rec_tp,rec_trade);
               else
                  ProcessTakeProfit(order,deal,position,rec_order_add,rec_order,rec_sl,rec_tp,rec_trade);
              }
            else
              {
               //--- market or pending?
               if(order->Type()==IMTOrder::OP_BUY || order->Type()==IMTOrder::OP_SELL)
                  rec_order.FillMarket(CNFAOrderRecord::TRANS_ORDER_FILL,false,order,CNFAOrderRecord::ORIGIN_AUTO);
               else
                  rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_FILL,order,CNFAOrderRecord::ORIGIN_AUTO);
               rec_trade.Fill(rec_order.OrderID(),deal);
               //--- check position
               ProcessUpdateSLTP(position,rec_order.Origin(),rec_sl,rec_tp);
              }
           }
         break;
      case IMTExecution::TE_ORDER_MODIFY:
         rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_MODIFY,order,CNFAOrderRecord::ORIGIN_AUTO);
         break;
      case IMTExecution::TE_ORDER_CANCEL:
         rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_CANCELLED,order,CNFAOrderRecord::ORIGIN_AUTO);
         break;
      default:
         m_sync.Unlock();
         return;
     }
//--- prepare execution time
   tm ttm_stamp={},ttm_batch={};
   PrepareTimes(time_process,ttm_batch,ttm_stamp);
//--- setup bid/ask
   MTTickShort tick={};
   m_api->TickLast(execution->Symbol(),tick);
   rec_order_add.SetBidAsk(tick);
   rec_order.SetBidAsk(tick);
   rec_sl.SetBidAsk(tick);
   rec_tp.SetBidAsk(tick);
   rec_trade.SetBidAsk(tick);
//--- write records
   WriteOrder(rec_order_add,ttm_batch,ttm_stamp);
   WriteOrder(rec_order,ttm_batch,ttm_stamp);
   WriteOrder(rec_sl,ttm_batch,ttm_stamp);
   WriteOrder(rec_tp,ttm_batch,ttm_stamp);
   WriteTrade(rec_trade,ttm_batch,ttm_stamp);
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Server API event                                                 |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestDelete(const IMTRequest *request)
  {
   if(!request || !m_api)
      return;
//--- find initial request
   CNFAOrderRecord rec_order={};
   m_sync.Lock();
   bool req_exist=m_requests.Remove(request->ID(),rec_order);
   m_sync.Unlock();
   if(!req_exist)
      return;
//--- check
   if(!CheckGroupAndSymbol(request->Group(),request->Symbol()))
      return;
//--- check action and result
   switch(request->Action())
     {
      //--- Server actions
      case IMTRequest::TA_ACTIVATE:
         if(request->ResultRetcode()==MT_RET_REQUEST_REJECT_CANCEL || request->ResultRetcode()==MT_RET_REQUEST_NO_MONEY)
            rec_order.SetTransType(CNFAOrderRecord::TRANS_ORDER_DELETED);
         break;
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
         if(request->ResultRetcode()==MT_RET_REQUEST_REJECT_CANCEL || request->ResultRetcode()==MT_RET_REQUEST_NO_MONEY)
            rec_order.SetTransType(CNFAOrderRecord::TRANS_ORDER_DELETED);
         break;
      case IMTRequest::TA_DEALER_ORD_ACTIVATE:
         if(request->ResultRetcode()==MT_RET_REQUEST_REJECT_CANCEL)
            rec_order.SetTransType(CNFAOrderRecord::TRANS_ORDER_DELETED);
         break;
     }
//--- check empty
   if(rec_order.Empty())
      return;
//--- fill
   tm ttm_stamp={},ttm_batch={};
   PrepareTimes(m_api->TimeCurrent(),ttm_batch,ttm_stamp);
   rec_order.SetBidAsk(request);
//--- writing synchronized
   m_sync.Lock();
   WriteOrder(rec_order,ttm_batch,ttm_stamp);
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Server API event                                                 |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestRefuse(const IMTRequest *request)
  {
   if(!request || !m_api)
      return;
//--- analize action and retcode
   IMTOrder *order=NULL;
   CNFAOrderRecord rec_order;
   switch(request->Action())
     {
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
         order=m_api->OrderCreate();
         if(order && m_api->HistoryGet(request->Order(),order))
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_DELETED,order,CNFAOrderRecord::ORIGIN_SYSTEM);
         break;
      default:
         return;
     }
//--- release order if it was created
   if(order)
      order->Release();
//--- check
   if(rec_order.Empty())
      return;
//--- fill common
   tm ttm_stamp={},ttm_batch={};
   PrepareTimes(m_api->TimeCurrent(),ttm_batch,ttm_stamp);
   rec_order.SetBidAsk(request);
//--- write
   m_sync.Lock();
   WriteOrder(rec_order,ttm_batch,ttm_stamp);
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Process market order                                             |
//+------------------------------------------------------------------+
void CPluginInstance::ProcessMarket(const IMTOrder *order,const IMTDeal *deal,const bool instant,CNFAOrderRecord& rec_order_add,CNFAOrderRecord& rec_order,CNFATradeRecord& rec_trade)
  {
//--- checks
   if(order && deal)
     {
      //--- full fill?
      if(deal->VolumeExt()==order->VolumeInitialExt())
        {
         rec_order_add.FillMarket(CNFAOrderRecord::TRANS_ORDER_ADDED,instant,order,CNFAOrderRecord::ORIGIN_AUTO);
         rec_order.FillMarket(CNFAOrderRecord::TRANS_ORDER_FILL,instant,order,CNFAOrderRecord::ORIGIN_AUTO);
         rec_trade.Fill(rec_order.OrderID(),deal);
        }
      else
        {
         //--- first fill?
         if(order->VolumeCurrentExt()+deal->VolumeExt()==order->VolumeInitialExt())
            rec_order_add.FillMarket(CNFAOrderRecord::TRANS_ORDER_ADDED,instant,order,CNFAOrderRecord::ORIGIN_AUTO);
         //--- full fill
         if(order->VolumeCurrentExt()==0)
            rec_order.FillMarket(CNFAOrderRecord::TRANS_ORDER_FILL,instant,order,CNFAOrderRecord::ORIGIN_AUTO);
         else
            rec_order.FillMarket(CNFAOrderRecord::TRANS_PARTIAL_FILL,instant,order,CNFAOrderRecord::ORIGIN_AUTO);
         rec_order.SetPartial(order,deal);
         rec_trade.Fill(rec_order.OrderID(),deal);
         rec_trade.SetPartial(order,deal);
        }
     }
  }
//+------------------------------------------------------------------+
//| Process activate pending order                                   |
//+------------------------------------------------------------------+
void CPluginInstance::ProcessActivate(const IMTOrder *order,const IMTDeal *deal,CNFAOrderRecord& rec_order,CNFATradeRecord& rec_trade)
  {
//--- checks
   if(order && deal)
     {
      //--- full fill?
      if(deal->VolumeExt()==order->VolumeInitialExt())
        {
         rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_FILL,order,CNFAOrderRecord::ORIGIN_AUTO);
         rec_trade.Fill(rec_order.OrderID(),deal);
        }
      else
        {
         //--- full fill
         if(order->VolumeCurrentExt()==0)
            rec_order.FillPending(CNFAOrderRecord::TRANS_ORDER_FILL,order,CNFAOrderRecord::ORIGIN_AUTO);
         else
            rec_order.FillPending(CNFAOrderRecord::TRANS_PARTIAL_FILL,order,CNFAOrderRecord::ORIGIN_AUTO);
         rec_order.SetPartial(order,deal);
         rec_trade.Fill(rec_order.OrderID(),deal);
         rec_trade.SetPartial(order,deal);
        }
     }
  }
//+------------------------------------------------------------------+
//| Process activate Stop Loss                                       |
//+------------------------------------------------------------------+
void CPluginInstance::ProcessStopLoss(const IMTOrder *order,const IMTDeal *deal,const IMTPosition *position,CNFAOrderRecord& rec_order_add,CNFAOrderRecord& rec_order,CNFAOrderRecord& rec_sl,CNFAOrderRecord& rec_tp,CNFATradeRecord& rec_trade)
  {
//--- checks
   if(order && deal && position)
     {
      SLTPRecord sltp={};
      if(m_sltp.RecordGet(position->Position(),sltp))
        {
         //--- full fill?
         if(deal->VolumeExt()==order->VolumeInitialExt())
           {
            rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_FILL,sltp.sl_id,sltp.sl_price,deal->VolumeExt(),deal->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
            rec_trade.Fill(rec_sl.OrderID(),deal);
            if(sltp.tp_price>0)
               rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_CANCELLED,sltp.tp_id,sltp.tp_price,sltp.volume_ext,sltp.contract_size,position,CNFAOrderRecord::ORIGIN_AUTO);
           }
         else
           {
            //--- full fill
            if(order->VolumeCurrentExt()==0)
              {
               rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_FILL,sltp.sl_id,sltp.sl_price,deal->VolumeExt(),deal->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
               if(sltp.tp_price>0)
                  rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_CANCELLED,sltp.tp_id,sltp.tp_price,sltp.volume_ext,sltp.contract_size,position,CNFAOrderRecord::ORIGIN_AUTO);
              }
            else
              {
               rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_PARTIAL_FILL,sltp.sl_id,sltp.sl_price,deal->VolumeExt(),deal->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
               if(sltp.tp_price>0)
                  rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_MODIFY,sltp.tp_id,sltp.tp_price,position->VolumeExt(),position->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
              }
            rec_sl.SetPartial(order,deal);
            rec_trade.Fill(rec_sl.OrderID(),deal);
            rec_trade.SetPartial(order,deal);
           }
         //--- update stops
         sltp.tp_price     =position->PriceTP();
         sltp.sl_price     =position->PriceSL();
         sltp.volume_ext   =position->VolumeExt();
         sltp.contract_size=position->ContractSize();
         //--- update base
         if(sltp.volume_ext>0)
            m_sltp.RecordUpdate(sltp);
         else
            m_sltp.RecordDelete(sltp.position_id);
        }
      else
        {
         m_api->LoggerOut(MTLogErr,L"SL record doesn't exist for position #%I64u, process as market closing",position->Position());
         ProcessMarket(order,deal,false,rec_order_add,rec_order,rec_trade);
        }
     }
  }
//+------------------------------------------------------------------+
//| Process activate Stop Loss                                       |
//+------------------------------------------------------------------+
void CPluginInstance::ProcessTakeProfit(const IMTOrder *order,const IMTDeal *deal,const IMTPosition *position,CNFAOrderRecord& rec_order_add,CNFAOrderRecord& rec_order,CNFAOrderRecord& rec_sl,CNFAOrderRecord& rec_tp,CNFATradeRecord& rec_trade)
  {
//--- checks
   if(order && deal && position)
     {
      SLTPRecord sltp={};
      if(m_sltp.RecordGet(position->Position(),sltp))
        {
         //--- full fill?
         if(deal->VolumeExt()==order->VolumeInitialExt())
           {
            rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_FILL,sltp.tp_id,sltp.tp_price,deal->VolumeExt(),deal->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
            rec_trade.Fill(rec_tp.OrderID(),deal);
            if(sltp.sl_price>0)
               rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_CANCELLED,sltp.sl_id,sltp.sl_price,sltp.volume_ext,sltp.contract_size,position,CNFAOrderRecord::ORIGIN_AUTO);
           }
         else
           {
            //--- full fill
            if(order->VolumeCurrentExt()==0)
              {
               rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_FILL,sltp.tp_id,sltp.tp_price,deal->VolumeExt(),deal->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
               if(sltp.sl_price>0)
                  rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_CANCELLED,sltp.sl_id,sltp.sl_price,sltp.volume_ext,sltp.contract_size,position,CNFAOrderRecord::ORIGIN_AUTO);
              }
            else
              {
               rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_PARTIAL_FILL,sltp.tp_id,sltp.tp_price,deal->VolumeExt(),deal->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
               if(sltp.sl_price>0)
                  rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_MODIFY,sltp.sl_id,sltp.sl_price,position->VolumeExt(),position->ContractSize(),position,CNFAOrderRecord::ORIGIN_AUTO);
              }
            rec_tp.SetPartial(order,deal);
            rec_trade.Fill(rec_tp.OrderID(),deal);
            rec_trade.SetPartial(order,deal);
           }
         //--- update stops
         sltp.tp_price     =position->PriceTP();
         sltp.sl_price     =position->PriceSL();
         sltp.volume_ext   =position->VolumeExt();
         sltp.contract_size=position->ContractSize();
         //--- update base
         if(sltp.volume_ext>0)
            m_sltp.RecordUpdate(sltp);
         else
            m_sltp.RecordDelete(sltp.position_id);
        }
      else
        {
         m_api->LoggerOut(MTLogErr,L"TP record doesn't exist for position #%I64u, process as market closing",position->Position());
         ProcessMarket(order,deal,false,rec_order_add,rec_order,rec_trade);
        }
     }
  }
//+------------------------------------------------------------------+
//| Process update SL/TP for position                                |
//+------------------------------------------------------------------+
void CPluginInstance::ProcessUpdateSLTP(const IMTPosition *position,const uint32_t origin,CNFAOrderRecord &rec_sl,CNFAOrderRecord &rec_tp)
  {
   if(position && position->Position())
     {
      SLTPRecord stop_new={},stop_old={};
      bool       updated=false;
      //--- prepare current stop
      stop_new.position_id  =position->Position();
      stop_new.origin       =origin;
      stop_new.sl_id        =1;
      stop_new.sl_price     =position->PriceSL();
      stop_new.tp_id        =1;
      stop_new.tp_price     =position->PriceTP();
      stop_new.volume_ext   =position->VolumeExt();
      stop_new.contract_size=position->ContractSize();
      stop_new.time_create  =position->TimeCreate();
      //--- get stop record
      if(m_sltp.RecordGet(position->Position(),stop_old))
        {
         //--- restore SL/TP id
         stop_new.sl_id=stop_old.sl_id;
         stop_new.tp_id=stop_old.tp_id;
        }
      else
        {
         if(stop_new.sl_price>0 || stop_new.tp_price>0)
           {
            if(stop_new.sl_price>0)
               rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_ADDED,stop_new.sl_id,stop_new.sl_price,stop_new.volume_ext,stop_new.contract_size,position,origin);
            if(stop_new.tp_price>0)
               rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_ADDED,stop_new.tp_id,stop_new.tp_price,stop_new.volume_ext,stop_new.contract_size,position,origin);
            //--- add new record
            m_sltp.RecordUpdate(stop_new,true);
           }
         return;
        }
      //--- check position volume
      if(!stop_new.volume_ext && stop_old.volume_ext)
        {
         if(stop_old.sl_price>0)
            rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_CANCELLED,stop_old.sl_id,stop_old.sl_price,stop_old.volume_ext,stop_old.contract_size,position,origin);
         if(stop_old.tp_price>0)
            rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_CANCELLED,stop_old.tp_id,stop_old.tp_price,stop_old.volume_ext,stop_old.contract_size,position,origin);
         m_sltp.RecordUpdate(stop_new,false);
         return;
        }
      //--- check SL
      if(stop_new.sl_price>0)
        {
         //--- changed SL price or Volume?
         if(stop_old.sl_price>0 && stop_old.volume_ext>0)
           {
            if(stop_new.sl_price!=stop_old.sl_price || stop_new.volume_ext!=stop_old.volume_ext)
              {
               rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_MODIFY,stop_new.sl_id,stop_new.sl_price,stop_new.volume_ext,stop_new.contract_size,position,origin);
               updated=true;
              }
           }
         else
           {
            //--- add new SL order
            stop_new.sl_id++;
            rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_ADDED,stop_new.sl_id,stop_new.sl_price,stop_new.volume_ext,stop_new.contract_size,position,origin);
            updated=true;
           }
        }
      else
        {
         //--- cancel existing SL order
         if(stop_old.sl_price>0 && stop_old.volume_ext>0)
           {
            rec_sl.FillStopLoss(CNFAOrderRecord::TRANS_ORDER_CANCELLED,stop_old.sl_id,stop_old.sl_price,stop_old.volume_ext,stop_old.contract_size,position,origin);
            updated=true;
           }
        }
      //--- check TP
      if(stop_new.tp_price>0)
        {
         //--- changed TP price or Volume?
         if(stop_old.tp_price>0 && stop_old.volume_ext>0)
           {
            if(stop_new.tp_price!=stop_old.tp_price || stop_new.volume_ext!=stop_old.volume_ext)
              {
               rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_MODIFY,stop_new.tp_id,stop_new.tp_price,stop_new.volume_ext,stop_new.contract_size,position,origin);
               updated=true;
              }
           }
         else
           {
            //--- add new TP order
            stop_new.tp_id++;
            rec_sl.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_ADDED,stop_new.tp_id,stop_new.tp_price,stop_new.volume_ext,stop_new.contract_size,position,origin);
            updated=true;
           }
        }
      else
        {
         //--- cancel existing SL order
         if(stop_old.tp_price>0 && stop_old.volume_ext>0)
           {
            rec_tp.FillTakeProfit(CNFAOrderRecord::TRANS_ORDER_CANCELLED,stop_old.tp_id,stop_old.tp_price,stop_old.volume_ext,stop_old.contract_size,position,origin);
            updated=true;
           }
        }
      //--- update base
      if(updated)
         m_sltp.RecordUpdate(stop_new);
     }
  }
//+------------------------------------------------------------------+
//| Write order record                                               |
//+------------------------------------------------------------------+
void CPluginInstance::WriteOrder(const CNFAOrderRecord &record,tm &ttm_batch,tm &ttm_stamp)
  {
//--- orderbook file header
   static LPCWSTR header=
   L"ID|BATCH_DATE|TRANSACTION_DATETIME|ORDER_ID|FDM_ID|MKT_SEG_ID|"
   "PRODUCT_CODE|PRODUCT_CAT|CONTRACT_YEAR|CONTRACT_MONTH|"
   "CONTRACT_DAY|STRIKE|OPTION_TYPE|TRANSACTION_TYPE|ORDER_TYPE|"
   "VERB|BID_PRICE|ASK_PRICE|QUANTITY|REMAINING_QTY|PRICE|STOP_PRICE|"
   "STOP_PRODUCT_CODE|TRAIL_AMT|LIMIT_OFFSET|DURATION|EXPIRY_DATE|"
   "ORDER_ORIGIN|MANAGER_ID|CUSTACCT_ID|SERVER_ID|CUST_GROUP|"
   "LINKED_ORDER_ID|LINK_REASON|OPEN_CLOSE";
//--- check
   if(!m_api || record.Empty())
      return;
//--- format record and write to file
   CMTStr512 buf;
   record.Print(buf,ttm_batch,ttm_stamp,m_context);
   if(!m_orders.WriteRecord(m_context.server_path.Str(),m_context.base_directory.Str(),REPORT_ORDERBOOK,ttm_batch,header,buf.Str()))
      m_api->LoggerOutString(MTLogErr,m_orders.LastError());
  }
//+------------------------------------------------------------------+
//| Write trade record                                               |
//+------------------------------------------------------------------+
void CPluginInstance::WriteTrade(const CNFATradeRecord &record,tm &ttm_batch,tm &ttm_stamp)
  {
//--- trades file header
   static LPCWSTR header=
   L"ID|BATCH_DATE|TRADE_ID|ORDER_ID|FDM_ID|MKT_SEG_ID|"
   "TIME_TRADEMATCH|PRODUCT_CAT|PRODUCT_CODE|CONTRACT_YEAR|CONTRACT_MONTH|"
   "CONTRACT_DAY|STRIKE|VERB|BID_PRICE|ASK_PRICE|QUANTITY|"
   "REMAINING_QTY|FILL_PRICE|CONTRA_FILL_PRICE|"
   "SERVER_ID|IMPLIED_VOLATILITY|IB_REBATE|COMMISSION";
//---
   if(!m_api || record.Empty())
      return;
//--- format record and write to file
   CMTStr512 buf;
   record.Print(buf,ttm_batch,ttm_stamp,m_context);
   if(!m_trades.WriteRecord(m_context.server_path.Str(),m_context.base_directory.Str(),REPORT_TRADE,ttm_batch,header,buf.Str()))
      m_api->LoggerOutString(MTLogErr,m_orders.LastError());
  }
//+------------------------------------------------------------------+
//| Filtration by group and symbol                                   |
//+------------------------------------------------------------------+
bool CPluginInstance::CheckGroupAndSymbol(LPCWSTR group,LPCWSTR symbol)
  {
   bool res=false;
   if(group && symbol)
     {
      m_sync.Lock();
      res=CMTStr::CheckGroupMask(m_context.groups.Str(),group) && CMTStr::CheckGroupMask(m_context.symbols.Str(),symbol);
      m_sync.Unlock();
     }
   return(res);
  }
//+------------------------------------------------------------------+
//| Preparing times for NFA records                                  |
//+------------------------------------------------------------------+
void CPluginInstance::PrepareTimes(const int64_t trade_time,tm &ttm_batch,tm &ttm_stamp) const
  {
//--- convert trade timestamp to EST 
   SMTTime::ParseTime(trade_time,&ttm_stamp);
   SPluginHelpers::TimeToEST(ttm_stamp,m_context.server_timezone);
//--- make batch datetime
   ttm_batch=ttm_stamp;
   SPluginHelpers::TimeToBatchDay(ttm_batch,m_context.report_time,true);
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ReadParams(void)
  {
//--- check pointers
   if(!m_api || !m_config || !m_param ||!m_time)
      return(MT_RET_ERR_PARAMS);
//--- get current plugin configuration
   MTAPIRES retcode;
   if((retcode=m_api->PluginCurrent(m_config))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"get plugin configuration error [%s (%u)]",SMTFormat::FormatError(retcode),retcode);
      return(retcode);
     }
//--- get current time configuration
   if((retcode=m_api->TimeGet(m_time))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"get time config error [%s (%u)]",SMTFormat::FormatError(retcode),retcode);
      return(retcode);
     }
//--- clear parameters
   m_context.Clear();
//--- get parameters
   if(!ParamGetString (L"Forex Dealer Member ID",m_context.fdm_id          ,DEFAULT_FDM           )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"Server ID"             ,m_context.server_id       ,DEFAULT_SERVER_ID     )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetSymbols(L"Symbols"               ,m_context.symbols         ,DEFAULT_SYMBOLS       )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetGroups (L"Groups"                ,m_context.groups          ,DEFAULT_GROUPS        )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"Base Directory"        ,m_context.base_directory  ,DEFAULT_BASE_DIRECTORY)) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"SFTP Address"          ,m_context.sftp_address    ,DEFAULT_SFTP_ADDRESS  )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"SFTP Port"             ,m_context.sftp_port       ,DEFAULT_SFTP_PORT     )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"SFTP Login"            ,m_context.sftp_login      ,DEFAULT_SFTP_LOGIN    )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"SFTP Password"         ,m_context.sftp_password   ,DEFAULT_SFTP_PASS     )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetString (L"SFTP Private Key Path" ,m_context.sftp_key_path   ,DEFAULT_SFTP_KEY_PATH )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetTime   (L"Report Time"           ,m_context.report_time     ,DEFAULT_REPORT_TIME   )) return(MT_RET_ERR_PARAMS);
   if(!ParamGetTime   (L"Send Time"             ,m_context.send_time       ,DEFAULT_SEND_TIME     )) return(MT_RET_ERR_PARAMS);
//--- cache timezone
   m_context.server_timezone=m_time->TimeZone();
//--- get server path
   if(GetModuleFileNameW(NULL,m_context.server_path.Buffer(),m_context.server_path.Max()))
     {
      m_context.server_path.Refresh();
      int32_t pos=m_context.server_path.FindRChar(L'\\');
      if(pos>0)
         m_context.server_path.Trim(pos);
     }
//--- fine
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Create parameter it not exists and  check it's type              |
//+------------------------------------------------------------------+
bool CPluginInstance::ParamCheck(LPCWSTR name,LPCWSTR default_value,const uint32_t type)
  {
   const MTAPIRES retcode=m_config->ParameterGet(name,m_param);
   switch(retcode)
     {
      case MT_RET_OK: // parameter found
         if(m_param->Type()!=type) // update parameter type
           {
            IMTConParam *tmp;
            if((tmp=m_api->PluginParamCreate())!=NULL)
              {
               //--- find the parameter by name
               for(uint32_t pos=0;m_config->ParameterNext(pos,tmp)==MT_RET_OK; pos++)
                 {
                  //--- check name
                  if(CMTStr::Compare(m_param->Name(),tmp->Name())==0)
                    {
                     //--- update and return result
                     m_config->ParameterUpdate(pos,m_param);
                     tmp->Release();
                     return(m_api->PluginAdd(m_config)==MT_RET_OK);
                    }
                 }
               //--- release
               tmp->Release();
              }
            //--- not found
            return(false);
           }
         return(true);
      case MT_RET_ERR_NOTFOUND: // parameter not found - create it!
         m_param->Clear();
         m_param->Name(name);
         m_param->Type(type);
         m_param->Value(default_value);
         return(m_config->ParameterAdd(m_param)==MT_RET_OK && m_api->PluginAdd(m_config)==MT_RET_OK);
      default: // API error
         m_api->LoggerOut(MTLogErr,L"get parameter \"%s\" error [%u]",name,retcode);
         break;
     }
//---
   return(false);
  }
//+------------------------------------------------------------------+
//| GetStringParam                                                   |
//+------------------------------------------------------------------+
   bool CPluginInstance::ParamGetString(LPCWSTR name,CMTStr &out,LPCWSTR default_value)
  {
   if(ParamCheck(name,default_value,IMTConParam::TYPE_STRING))
     {
      out.Assign(m_param->ValueString());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| GetGroupsParam                                                   |
//+------------------------------------------------------------------+
   bool CPluginInstance::ParamGetGroups(LPCWSTR name,CMTStr &out,LPCWSTR default_value)
  {
   if(ParamCheck(name,default_value,IMTConParam::TYPE_GROUPS))
     {
      out.Assign(m_param->ValueGroups());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| GetSymbolsParam                                                  |
//+------------------------------------------------------------------+
   bool CPluginInstance::ParamGetSymbols(LPCWSTR name,CMTStr &out,LPCWSTR default_value)
  {
   if(ParamCheck(name,default_value,IMTConParam::TYPE_SYMBOLS))
     {
      out.Assign(m_param->ValueSymbols());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| GetIntParam                                                       |
//+------------------------------------------------------------------+
   bool CPluginInstance::ParamGetTime(LPCWSTR name,int64_t &out,LPCWSTR default_value)
  {
   if(ParamCheck(name,default_value,IMTConParam::TYPE_TIME))
     {
      out=m_param->ValueTime();
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+

