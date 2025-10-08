//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
#include <stdint.h>
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
   if((retcode=m_api->TradeSubscribe(this))!=MT_RET_OK)
      m_api->LoggerOut(MTLogOK,L"Trade subscribe failed [%d]",retcode);
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
      m_api->TradeUnsubscribe(this);
      //--- clear API 
      m_api=NULL;
     }
//--- ok
   m_cleared.Shutdown();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Hook on check rule compatibility with request                    |
//| BEFORE rule conditions check                                     |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookTradeRequestRuleFilter(IMTRequest* request,IMTConRoute* rule,const IMTConGroup* group)
  {
//--- check
   if(!request || !rule)
      return(MT_RET_OK_NONE);
//--- check request
   if(request->Action()==IMTRequest::TA_INSTANT ||
      request->Action()==IMTRequest::TA_MARKET  ||
      request->Action()==IMTRequest::TA_EXCHANGE)
      if(request->Type()==IMTOrder::OP_BUY)
        {
         uint32_t   i;
         uint64_t ticket=request->Order();
         //--- has it cleared already?
         for(i=0;i<m_cleared.Total();i++)
            if(m_cleared[i]==ticket)
               break;
         //--- No?
         if(i>=m_cleared.Total())
           {
            rule->Clear();
            rule->Name(L"plugin removes SL and TP from market buy");
            rule->Action(IMTConRoute::ACTION_CLEAR_SLTP);
            //--- remember cleared 
            m_cleared.Add(&ticket);
            //--- we want to add custom action
            return(MT_RET_OK);
           }
         //--- remove cleared buy 
         m_cleared.Delete(i);
         //--- lets confirm it by market price
         rule->Clear();
         rule->Name(L"plugin confirms market buy by market");
         rule->Action(IMTConRoute::ACTION_CONFIRM_MARKET);
         //--- we want to add custom action
         return(MT_RET_OK);
        }
//--- other requests will be processed by routing table
   return(MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| Hook on check rule compatibility with request                    |
//| AFTER rule conditions check                                      |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookTradeRequestRuleApply(IMTRequest*  request,IMTConRoute* rule,const IMTConGroup* group)
  {
//--- check
   if(!request || !rule)
      return(MT_RET_OK_NONE);
//--- check request
   if(request->Action()==IMTRequest::TA_INSTANT ||
      request->Action()==IMTRequest::TA_MARKET  ||
      request->Action()==IMTRequest::TA_EXCHANGE)
      if(request->Type()==IMTOrder::OP_SELL)
        {
         //--- this is confirmation? lets clear SL-TP on order before
         if(rule->Action()==IMTConRoute::ACTION_CONFIRM_MARKET ||
            rule->Action()==IMTConRoute::ACTION_CONFIRM_CLIENT)
           {
            uint32_t   i;
            uint64_t ticket=request->Order();
            //--- has it cleared already?
            for(i=0;i<m_cleared.Total();i++)
               if(m_cleared[i]==ticket)
                  break;
            //--- No?
            if(i>=m_cleared.Total())
              {
               rule->Clear();
               rule->Name(L"plugin removes SL and TP from market sell");
               rule->Action(IMTConRoute::ACTION_CLEAR_SLTP);
               //--- remember cleared 
               m_cleared.Add(&ticket);
               //--- we want to add custom action
               return(MT_RET_OK);
              }
            //--- remove cleared buy 
            m_cleared.Delete(i);
            //--- we want to use current rule from table
            return(MT_RET_OK_NONE);
           }
        }
//--- other requests will be processed by routing table
   return(MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+