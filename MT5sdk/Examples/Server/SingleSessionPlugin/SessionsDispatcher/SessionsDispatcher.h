//+------------------------------------------------------------------+
//|                               MetaTrader 5 Single Session Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "ClientSession.h"
//+------------------------------------------------------------------+
//| Sessions dispatcher                                              |
//+------------------------------------------------------------------+
class CSessionsDispatcher
  {

private:
   //--- reference to the parent
   CPluginInstance  &m_parent;
   //--- groups with which the plugin works
   String64Array     m_groups;
   //--- array of client sessions
   ClientSessionArray m_sessions;
   //--- array of dropped client sessions
   ClientSessionArray m_sessions_dropped;
   //--- synchronizer
   CMTSync           m_sync;

public:
                     CSessionsDispatcher(CPluginInstance &parent);
   virtual          ~CSessionsDispatcher();
   //--- initialization/shutdown
   bool              Initialize(IMTServerAPI *api);
   //--- group check
   bool              GroupCheck(const String64Array &groups,LPCWSTR group);
   //--- pointer of a client's connection to the server
   virtual void      OnUserLogin(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online);
   //--- hook of a client's connection to the server
   virtual MTAPIRES  HookUserLogin(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online);
   //--- handler of the event of a client's disconnection from the server
   virtual void      OnUserLogout(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online);

private:
   //--- drop connections
   bool              DropConnections(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online);
   //--- sorting/searching
   static int32_t    SortClientSessionsByLogin(const void *left,const void *right);
   static int32_t    SearchClientSessionsByLogin(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
