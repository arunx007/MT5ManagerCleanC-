//+------------------------------------------------------------------+
//|                               MetaTrader 5 Single Session Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Preliminary declarations                                         |
//+------------------------------------------------------------------+
class CPluginInstance;
//---
#include "SessionsDispatcher/SessionsDispatcher.h"
//+------------------------------------------------------------------+
//| Plugin instance class                                            |
//+------------------------------------------------------------------+
class CPluginInstance : public IMTServerPlugin,
                        public IMTUserSink
  {

private:
   //--- API pointer
   IMTServerAPI*     m_api;
   //--- sessions dispatcher
   CSessionsDispatcher m_session_dispather;

public:
   //--- constructor/destructor
                     CPluginInstance();
   virtual          ~CPluginInstance();
   //--- IMTServerPlugin interface implementation
   virtual void      Release(void);
   virtual MTAPIRES  Start(IMTServerAPI* server);
   virtual MTAPIRES  Stop();
   //--- getting an array of users online
   bool              OnlineGet(ClientSessionArray &sessions,const String64Array &groups);
   //--- getting an array of groups specified in the plugin parameters
   bool              GroupsGet(String64Array &groups);
   //--- pointer of a client's connection to the server
   virtual void      OnUserLoginExt(const IMTUser* user,const IMTOnline* online) override;
   //--- hook of a client's connection to the server
   virtual MTAPIRES  HookUserLoginExt(const IMTUser* user,const IMTOnline* online) override;
   //--- handler of the event of a client's disconnection from the server
   virtual void      OnUserLogoutExt(const IMTUser* user,const IMTOnline* online) override;

private:
   //--- show plugin parameters
   void              ParametersShow(void);
  };
//+------------------------------------------------------------------+

