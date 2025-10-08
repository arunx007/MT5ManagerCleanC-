//+------------------------------------------------------------------+
//|                               MetaTrader 5 Single Session Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Plugin parameters                                                |
//+------------------------------------------------------------------+
#define PLUGIN_PARAM_GROUPS         L"Groups"
//+------------------------------------------------------------------+
//| Disable: 'this' : used in base member initializer list           |
//+------------------------------------------------------------------+
#pragma warning(push)
#pragma warning(disable: 4355)
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void)
   : m_api(NULL),
     m_session_dispather(*this)
  {
  }
#pragma warning(pop)
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
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- store API pointer
   m_api=api;
//--- subscribe
   MTAPIRES res=m_api->UserSubscribe(this);

   if(res!=MT_RET_OK)
      m_api->LoggerOut(MTLogErr,L"Single Session Plugin: UserSubscribe failed [%u]",res);
//--- initialize of sessions dispatcher
   if(res==MT_RET_OK)
      if(!m_session_dispather.Initialize(m_api))
         res=MT_RET_ERROR;
//--- print plugin parameters
   ParametersShow();
   return(res);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- unsubscribe from all events
   if(m_api)
     {
      //--- unsubscribe from all events
      m_api->UserUnsubscribe(this);
      m_api=NULL;
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Getting an array of users online                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::OnlineGet(ClientSessionArray &sessions,const String64Array &groups)
  {
   ClientSession     session={0};
   IMTOnline        *online_ptr=NULL;
   IMTUser          *user_ptr=NULL;
   uint32_t          online_total=0;
   MTAPISTR          ip={0};
   MTAPISTR          computer_id={0};
   bool              res=true;
//--- check
   if(!m_api)
      return(false);
//--- clear array
   sessions.Clear();
//--- create interfaces
   if((online_ptr=m_api->OnlineCreate())==NULL)
      return(false);
   if((user_ptr=m_api->UserCreate())==NULL)
     {
      online_ptr->Release();
      return(false);
     }
//--- count of users online
   online_total=m_api->OnlineTotal();
//--- go through all users
   for(uint32_t i=0;res && i<online_total;i++)
     {
      //--- getting the next user
      res=(m_api->OnlineNext(i,online_ptr)==MT_RET_OK);
      //--- skip the groups
      if(res && !m_session_dispather.GroupCheck(groups,online_ptr->Group()))
         continue;
      //--- getting the user settings
      res=res && (m_api->UserGet(online_ptr->Login(),user_ptr)==MT_RET_OK);
      //--- filling of structure
      res=res && ClientSession::Fill(online_ptr->Address(ip),
                                     online_ptr->SessionID(),
                                     online_ptr->ComputerID(computer_id),
                                     user_ptr,
                                     online_ptr->Type(),
                                     m_api->TimeCurrent(),
                                     session);
      //--- add the session to array
      res=res && sessions.Add(&session);
     }
//--- release interfaces
   online_ptr->Release();
   user_ptr->Release();
   return(res);
  }
//+------------------------------------------------------------------+
//| Getting an array of groups specified in the plugin parameters    |
//+------------------------------------------------------------------+
bool CPluginInstance::GroupsGet(String64Array &groups)
  {
   IMTConPlugin *plugin=NULL;
   IMTConParam  *param=NULL;
//--- check
   if(!m_api)
      return(false);
//--- create interfaces
   if((plugin=m_api->PluginCreate())==NULL)
      return(false);
   if((param=m_api->PluginParamCreate())==NULL)
     {
      //--- release
      plugin->Release();
      return(false);
     }
//--- getting the plugin
   bool res=(m_api->PluginCurrent(plugin)==MT_RET_OK);
//--- clear array
   groups.Clear();
//--- go through all parameters
   for(uint32_t i=0;res && plugin->ParameterNext(i,param)==MT_RET_OK;i++)
     {
      //--- if this is a group setting, add the record to the array
      if(CMTStr::Compare(param->Name(),PLUGIN_PARAM_GROUPS)==0)
        {
         //--- copy the group to a temp string
         wchar_t str[64]={};
         CMTStr::Copy(str,_countof(str),param->Value());
         //--- add the group to the array
         res=groups.Add(&str);
        }
     }
//--- release interfaces
   plugin->Release();
   param->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Pointer of a client's connection to the server                   |
//+------------------------------------------------------------------+
void CPluginInstance::OnUserLoginExt(const IMTUser* user,const IMTOnline* online)
  {
//--- notify the session dispatcher
   return(m_session_dispather.OnUserLogin(m_api,user,online));
  }
//+------------------------------------------------------------------+
//| Hook of a client's connection to the server                      |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookUserLoginExt(const IMTUser* user,const IMTOnline* online)
  {
//--- notify the session dispatcher
   return(m_session_dispather.HookUserLogin(m_api,user,online));
  }
//+------------------------------------------------------------------+
//| Handler of the event of a client's disconnection from the server |
//+------------------------------------------------------------------+
void CPluginInstance::OnUserLogoutExt(const IMTUser* user,const IMTOnline* online)
  {
//--- notify the session dispatcher
   m_session_dispather.OnUserLogout(m_api,user,online);
  }
//+------------------------------------------------------------------+
//| Show plugin parameters                                           |
//+------------------------------------------------------------------+
void CPluginInstance::ParametersShow(void)
  {
   IMTConPlugin* plugin=NULL;
   IMTConParam*  param =NULL;
   uint32_t      i;
//--- check API
   if(!m_api)
      return;
//--- receive current plugin description
   if((plugin=m_api->PluginCreate())==NULL)
      return;
   if(m_api->PluginCurrent(plugin)!=MT_RET_OK)
     {
      plugin->Release();
      return;
     }
//--- print plugin name
   m_api->LoggerOut(MTLogOK,L"Plugin: %s",plugin->Name());
//--- create parameters object
   if((param=m_api->PluginParamCreate())!=NULL)
     {
      m_api->LoggerOutString(MTLogOK,L"Parameters: ");
      //--- enumerate parameters
      for(i=0;plugin->ParameterNext(i,param)==MT_RET_OK;i++)
         m_api->LoggerOut(MTLogOK,L"Name: %s, Value: %s",param->Name(),param->Value());
      //--- release parameter
      param->Release();
     }
//--- release plugin config
   plugin->Release();
  }
//+------------------------------------------------------------------+

