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
CPluginInstance::CPluginInstance(void) : m_api(NULL),
                                         m_config(NULL),
                                         m_config_param(NULL),
                                         m_min_freemargin(0),m_min_interest(0),m_max_interest(0)
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
//--- check
   if(!api) return(MT_RET_ERR_PARAMS);
//--- store API pointer
   m_api=api;
//--- subscribe
   if(m_api->TradeSubscribe(this)!=MT_RET_OK || m_api->PluginSubscribe(this)!=MT_RET_OK)
     {
      Stop();
      return(MT_RET_ERROR);
     }
//--- read plugin parameters
   if(ParametersRead()!=MT_RET_OK)
     {
      Stop();
      return(MT_RET_ERROR);
     }
//---
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- clean
   if(m_api)
     {
      //--- free objects
      if(m_config)        { m_config->Release();       m_config=NULL;       }
      if(m_config_param)  { m_config_param->Release(); m_config_param=NULL; }
      //--- unsubscribe
      m_api->TradeUnsubscribe(this);
      m_api->PluginUnsubscribe(this);
      //--- clear API 
      m_api=NULL;
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin config update notification                                |
//+------------------------------------------------------------------+
void CPluginInstance::OnPluginUpdate(const IMTConPlugin* plugin)
  {
//--- check
   if(!plugin || !m_config) return;
//--- check plugin name and server 
   if(CMTStr::Compare(plugin->Name(),m_config->Name())==0 && plugin->Server()==m_config->Server())
     {
      ParametersRead();
     }
  }
//+------------------------------------------------------------------+
//| End of day interest rate calculation                             |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookTradeInterest(const int64_t         datetime,
                                            const IMTConGroup*  group,
                                            const IMTAccount*   account,
                                            const double        original_value,
                                            double&             new_value)
  {
//--- check, if something wrong, don't touch anything
   if(!group || !account) return(MT_RET_OK);
//--- check client's free margin
   return(account->MarginFree()>=m_min_freemargin ? MT_RET_OK : MT_RET_ERROR);
  }
//+------------------------------------------------------------------+
//| End of month interest rate charge                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookTradeInterestCharge(const int64_t    datetime,
                                             const IMTConGroup*  group,
                                             const IMTUser*      user,
                                             const double        original_value,
                                             double&             new_value)
  {
//--- check limits
   if(new_value<m_min_interest) new_value=0;
   if(new_value>m_max_interest) new_value=m_max_interest;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Read plugin parameters                                           |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ParametersRead(void)
  {
   MTAPIRES retcode;
//--- check
   if(!m_api) return(MT_RET_ERR_PARAMS);
//--- check config object
   if(!m_config)
      if((m_config=m_api->PluginCreate())==NULL)
         return(MT_RET_ERR_MEM);
//--- check parameterts object
   if(!m_config_param)
      if((m_config_param=m_api->PluginParamCreate())==NULL)
         return(MT_RET_ERR_MEM);
//--- get plugin configuration
   if((retcode=m_api->PluginCurrent(m_config))!=MT_RET_OK)
      return(retcode);
//--- get parameters
   if((retcode=ParametersGetFloat(L"Minimum Free Margin",m_min_freemargin))!=MT_RET_OK)
      return(retcode);
//--- get parameters
   if((retcode=ParametersGetFloat(L"Minimum Interest Rate",m_min_interest))!=MT_RET_OK)
      return(retcode);
//--- get parameters
   if((retcode=ParametersGetFloat(L"Maximum Interest Rate",m_max_interest,10000))!=MT_RET_OK)
      return(retcode);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get float parameter                                              |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ParametersGetFloat(LPCWSTR name,double& value,double value_default/*=0*/)
  {
   MTAPIRES retcode;
//---
   if(!name || !m_api || !m_config || !m_config_param) return(MT_RET_ERROR);
//--- get parameter and check type
   if((retcode=m_config->ParameterGet(name,m_config_param))!=MT_RET_OK || m_config_param->Type()!=IMTConParam::TYPE_FLOAT)
     {
      //--- add default value
      m_config_param->Name(name);
      m_config_param->ValueFloat(value_default);
      //--- add parameter and update config
      if((retcode=m_config->ParameterAdd(m_config_param))!=MT_RET_OK || (retcode=m_api->PluginAdd(m_config))!=MT_RET_OK)
         return(retcode);
      //--- return
      value=value_default;
      return(MT_RET_OK);
     }
//--- get value
   value=m_config_param->ValueFloat();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
  