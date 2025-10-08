//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL)
  {
   ZeroMemory(&m_info,sizeof(m_info));
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
MTAPIRES CPluginInstance::Start(IMTServerAPI* api)
  {
   MTAPIRES retcode;
//--- check
   if(!api) return(MT_RET_ERR_PARAMS);
//--- store API pointer
   m_api=api;
//--- receive server information
   if((retcode=m_api->About(m_info))!=MT_RET_OK)
      m_api->LoggerOut(MTLogOK,L"Server info failed [%d]",retcode);
//--- subscribe plugin on deal events
   if((retcode=m_api->DealSubscribe(this))!=MT_RET_OK)
      m_api->LoggerOut(MTLogOK,L"Deal subscribe failed [%d]",retcode);
//--- print plugin parameters
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- unsubscribe from all events
   if(m_api)
     {
      m_api->DealUnsubscribe(this);
      //--- clear API 
      m_api=NULL;
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void CPluginInstance::OnDealPerform(const IMTDeal*  deal, IMTAccount* account,  IMTPosition* position)
  {
//--- check
   if(!deal || !m_api)
      return;
//--- notify
   MTAPISTR str;

   m_api->LoggerOut(MTLogOK,L"OnDealPerform: new deal: %s",deal->Print(str));
   m_api->LoggerOut(MTLogOK,L"OnDealPerform: Position: ticket = %I64u, user login = %I64u, action = %u, volume = %I64u, dealer = %I64u, reason = %u", position->Position(), position->Login(), position->Action(), position->Volume(), position->Dealer(), position->Reason());
//---
   IMTOrder* order_test=m_api->OrderCreate();
   IMTDeal*  record=NULL;
   MTAPIRES  retcode;
   int64_t     delete_flag=454;

   retcode=m_api->OrderGet(deal->Order(), order_test);
   if(retcode==MT_RET_OK)
     {
      m_api->LoggerOut(MTLogOK, L"OnDealPerform: order get success %I64u, state = %u, type = %u, init volume = %I64u, current volume = %I64u, time setup = %I64d, time done = %I64d", order_test->Order(), order_test->State(), order_test->Type(), order_test->VolumeInitial(), order_test->VolumeCurrent(), order_test->TimeSetup(), order_test->TimeDone());
     }
   else
     {
      m_api->LoggerOut(MTLogOK, L"OnDealPerform: order get failed: [%d]", retcode);
      if(order_test)
        {
         order_test->Release();
         order_test=nullptr;
        }
      return;
     }
//---
   if(order_test->State()==IMTOrder::ORDER_STATE_PARTIAL)
     {
      //---
      IMTRequest* dealer_request=m_api->TradeRequestCreate();
      dealer_request->Clear();
      dealer_request->Action(IMTRequest::TA_DEALER_ORD_REMOVE);
      dealer_request->SourceLogin(3);
      dealer_request->Login(deal->Login());
      dealer_request->Symbol(deal->Symbol());
      dealer_request->Order(deal->Order());
      dealer_request->Type(order_test->Type());
      dealer_request->Volume(deal->Volume());
         /*
         dealer_request->Clear();
         dealer_request->Action(IMTRequest::TA_REMOVE); // TA_DEALER_ORD_REMOVE
         dealer_request->Login(order_test->Login());
         dealer_request->Symbol(order_test->Symbol());
         dealer_request->Order(order_test->Order());
         dealer_request->Type(order_test->Type());
         */
      //---
      retcode=m_api->TradeRequest(dealer_request);
      if(retcode==MT_RET_OK)
         m_api->LoggerOutString(MTLogOK, L"OnDealPerform: trade request send successfully");
      else
         m_api->LoggerOut(MTLogOK, L"OnDealPerform: trade request send failed: [%d]", retcode);
      //---
      if(dealer_request)
        {
         dealer_request->Release();
         dealer_request=nullptr;
        }
     }
//---
   if(order_test)
     {
      order_test->Release();
      order_test=nullptr;
     }
  }
//+------------------------------------------------------------------+

