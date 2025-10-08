//+------------------------------------------------------------------+
//|                               MetaTrader 5 Single Session Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "SessionsDispatcher.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CSessionsDispatcher::CSessionsDispatcher(CPluginInstance &parent)
   : m_parent(parent)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CSessionsDispatcher::~CSessionsDispatcher(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CSessionsDispatcher::Initialize(IMTServerAPI *api)
  {
   ClientSession     *session_ptr=NULL;
   ClientSessionArray sessions;
   bool               res=true;
//--- check
   if(!api)
      return(false);
//--- getting an array of groups specified in the plugin parameters
   res=m_parent.GroupsGet(m_groups);
//--- getting an array of users online
   res=res && m_parent.OnlineGet(sessions,m_groups);
//--- lock
   m_sync.Lock();
   m_sessions.Clear();
//--- go through all sessions
   for(uint32_t i=0;i<sessions.Total();i++)
     {
      // session crnt
      const ClientSession& session_crnt=sessions[i];
      if(m_sessions.Insert(&session_crnt,SortClientSessionsByLogin)!=NULL)
        {
         api->LoggerOut(MTLogOK,L"%I64u: initialization login from IP: %s, session ID: %I64u, computer id: %s",
                                session_crnt.client_login,
                                session_crnt.client_ip,
                                session_crnt.client_session_id,
                                session_crnt.client_computer_id);
        }
      else
        {
         api->LoggerOut(MTLogErr,L"%I64u: failed to insert initialization login from IP: %s, session ID: %I64u, computer id: %s",
                                 session_crnt.client_login,
                                 session_crnt.client_ip,
                                 session_crnt.client_session_id,
                                 session_crnt.client_computer_id);
        }
     }
//--- unlock
   m_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Group check                                                      |
//+------------------------------------------------------------------+
bool CSessionsDispatcher::GroupCheck(const String64Array &groups,LPCWSTR group)
  {
   bool res=false;
//--- check
   if(!group)
      return(false);
//--- checking the groups count
   if(groups.Total()<1)
      return(true);
//--- go through all groups
   for(uint32_t i=0;i<groups.Total();i++)
      if(CMTStr::CheckGroupMask(groups[i],group))
        {
         res=true;
         break;
        }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Pointer of a client's connection to the server                   |
//+------------------------------------------------------------------+
void CSessionsDispatcher::OnUserLogin(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online)
  {
   ClientSession *session_ptr=NULL;
   ClientSession  session={0};
   IMTOnlineArray *online_array=NULL;
   int64_t          client_login=0;
   MTAPISTR       ip,computer_id;
   MTAPIRES       res=MT_RET_OK;
//--- check
   if(!api || !user || !online || !GroupCheck(m_groups,user->Group()))
      return;
//--- lock
   m_sync.Lock();
//--- if there are no connections, add the record to the session array
   if(ClientSession::Fill(online->Address(ip),
                          online->SessionID(),
                          online->ComputerID(computer_id),
                          user,
                          online->Type(),
                          api->TimeCurrent(),
                          session))
     {
      if(m_sessions.Insert(&session,SortClientSessionsByLogin)!=NULL)
        {
         api->LoggerOut(MTLogOK,L"%I64u: login from IP: %s, session ID: %I64u, computer id: %s, total: %u",
                                user->Login(),
                                online->Address(ip),
                                online->SessionID(),
                                online->ComputerID(computer_id),m_sessions.Total());
         //--- drop connections from other machines
         if(!DropConnections(api,user,online))
           {
            api->LoggerOut(MTLogErr,L"%I64u: failed to drop connections",user->Login());
           }
        }
      else
        {
         api->LoggerOut(MTLogErr,L"%I64u: failed to insert login from IP: %s, session ID: %I64u, computer id: %s, total: %u",
                                 user->Login(),
                                 online->Address(ip),
                                 online->SessionID(),
                                 online->ComputerID(computer_id),m_sessions.Total());
        }
     }
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Hook of a client's connection to the server                      |
//+------------------------------------------------------------------+
MTAPIRES CSessionsDispatcher::HookUserLogin(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online)
  {
   ClientSession *session_ptr=NULL;
   int64_t          client_login=0;
   MTAPISTR       ip,computer_id;
   MTAPIRES       res=MT_RET_OK;
//--- check
   if(!api || !user || !online)
      return(MT_RET_ERROR);
//--- checking the group
   if(!GroupCheck(m_groups,user->Group()))
      return(MT_RET_OK);
//--- getting the login
   client_login=user->Login();
//--- if the session with the current login is already there, reject the connection
   m_sync.Lock();
   if((session_ptr=m_sessions_dropped.SearchLeft(&client_login,SearchClientSessionsByLogin))!=NULL)
     {
      //-- get position of this session
      int32_t pos=m_sessions_dropped.Position(session_ptr);
      if(pos>=0)
        {
         for(uint32_t i=(uint32_t)pos;i<m_sessions_dropped.Total();i++)
           {
            // session crnt
            const ClientSession& session_crnt=m_sessions_dropped[i];
            if(session_crnt.client_login!=client_login)
               break;
            //---
            if(CMTStr::Compare(session_crnt.client_ip,online->Address(ip))==0 &&
               CMTStr::Compare(session_crnt.client_computer_id,online->ComputerID(computer_id))==0)
              {
               if(m_sessions_dropped.Delete(i))
                 {
                  res=MT_RET_ERR_DUPLICATE;
                  api->LoggerOut(MTLogOK,L"%I64u: login declined, IP: %s, session ID: %I64u, computer id: %s, dropped total: %u",
                                         user->Login(),
                                         session_crnt.client_ip,
                                         session_crnt.client_session_id,
                                         session_crnt.client_computer_id,
                                         m_sessions_dropped.Total());
                 }
               else
                 {
                  api->LoggerOut(MTLogOK,L"%I64u: failed to delete dropped connection",user->Login());
                 }
               break;
              }
           }
        }
     }
   m_sync.Unlock();
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Handler of the event of a client's disconnection from the server |
//+------------------------------------------------------------------+
void CSessionsDispatcher::OnUserLogout(IMTServerAPI *api,const IMTUser* user,const IMTOnline* online)
  {
   ClientSession *session_ptr=NULL;
   int64_t          user_login=0;
   MTAPISTR       ip,computer_id;
   bool           found=false;
//--- check
   if(!api || !user || !online || !GroupCheck(m_groups,user->Group()))
      return;
//--- getting the login
   user_login=user->Login();
//--- search in the array by login
   m_sync.Lock();
   if((session_ptr=m_sessions.SearchLeft(&user_login,SearchClientSessionsByLogin))!=NULL)
     {
      //-- get position of this session
      int32_t pos=m_sessions.Position(session_ptr);
      if(pos>=0)
        {
         for(uint32_t i=(uint32_t)pos;i<m_sessions.Total();i++)
           {
            // session crnt
            const ClientSession &session_crnt=m_sessions[i];
            if(session_crnt.client_login!=user_login)
               break;
            //---
            if(CMTStr::Compare(session_crnt.client_ip,online->Address(ip))==0 &&
               CMTStr::Compare(session_crnt.client_computer_id,online->ComputerID(computer_id))==0)
              {
               found=true;
               if(!m_sessions.Delete(pos))
                  api->LoggerOut(MTLogErr,L"%I64u: faield to delete session",user->Login());
               api->LoggerOut(MTLogOK,L"%I64u: logout IP: %s, session ID: %I64u, computer id: %s, total: %u",
                                      user->Login(),
                                      online->Address(ip),
                                      online->SessionID(),
                                      online->ComputerID(computer_id),m_sessions.Total());
               break;
              }
           }
        }
     }
//--- if not found
   if(!found)
     {
      api->LoggerOut(MTLogErr,L"%I64u: failed to delete login IP: %s, session ID: %I64u, computer id: %s, total: %u",
                              user->Login(),
                              online->Address(ip),
                              online->SessionID(),
                              online->ComputerID(computer_id),m_sessions.Total());
     }
//--- unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Drop connections                                                 |
//+------------------------------------------------------------------+
bool CSessionsDispatcher::DropConnections(IMTServerAPI* api,const IMTUser* user,const IMTOnline* online)
  {
   ClientSession   session_dropped={0};
   IMTOnlineArray *online_array=NULL;
   IMTOnline      *online_crnt=NULL;
   MTAPISTR        ip,computer_id;
   MTAPISTR        ip_crnt,computer_id_crnt;
   uint32_t        pos=0;
//--- check
   if(!api || !user || !online)
      return(false);
//--- create array of online
   if((online_array=api->OnlineCreateArray())==NULL)
     {
      api->LoggerOutString(MTLogErr,L"failed to allocate array of online");
      return(false);
     }
//--- get all connections of current user
   if((api->OnlineGet(user->Login(),online_array))!=MT_RET_OK)
     {
      api->LoggerOutString(MTLogErr,L"failed to get online connections of user");
      return(false);
     }
//--- process all conenctions
   while((online_crnt=online_array->Next(pos++))!=NULL)
     {
      //--- skip current connection or connection from the same machine with same IP
      if(CMTStr::Compare(online->Address(ip),online_crnt->Address(ip_crnt))==0 &&
         CMTStr::Compare(online->ComputerID(computer_id),online_crnt->ComputerID(computer_id_crnt))==0)
         continue;
      if(ClientSession::Fill(online_crnt->Address(ip_crnt),
                             online_crnt->SessionID(),
                             online_crnt->ComputerID(computer_id_crnt),
                             user,
                             online_crnt->Type(),
                             0,
                             session_dropped))
        {
         //--- save the dropped connection
         if(m_sessions_dropped.Insert(&session_dropped,SortClientSessionsByLogin)==NULL)
           {
            api->LoggerOut(MTLogErr,L"%I64u: failed to insert dropped connection",user->Login());
            return(false);
           }
        }
      else
        {
         //--- log errors
         api->LoggerOut(MTLogErr,L"%I64u: failed to fill dropped connection",user->Login());
         return(false);
        }
      //--- close the current connections
      MTAPIRES res=api->OnlineDisconnect(online_crnt);
      if(res!=MT_RET_OK)
        {
         //--- log errors
         api->LoggerOut(MTLogErr,L"%I64u: failed to disconnect [%u]",user->Login(),res);
         return(false);
        }
      //--- log
      api->LoggerOut(MTLogErr,L"%I64u: connection dropped, IP: %s, session ID: %I64u, computer id: %s, dropped total: %u",
                              user->Login(),
                              online_crnt->Address(ip_crnt),
                              online_crnt->SessionID(),
                              online_crnt->ComputerID(computer_id_crnt),
                              m_sessions_dropped.Total());
     }
//--- all is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Sorting                                                          |
//+------------------------------------------------------------------+
int32_t CSessionsDispatcher::SortClientSessionsByLogin(const void *left,const void *right)
  {
   const ClientSession &lft=*(const ClientSession*)left;
   const ClientSession &rgh=*(const ClientSession*)right;
//--- compare
   if(lft.client_login>rgh.client_login)
      return(1);
   if(lft.client_login<rgh.client_login)
      return(-1);
//--- equal
   return(1);
  }
//+------------------------------------------------------------------+
//| Searching                                                        |
//+------------------------------------------------------------------+
int32_t CSessionsDispatcher::SearchClientSessionsByLogin(const void *left,const void *right)
  {
   const uint64_t        &lft=*(const int64_t*)left;
   const ClientSession &rgh=*(const ClientSession*)right;
//--- compare
   if(lft>rgh.client_login)
      return(1);
   if(lft<rgh.client_login)
      return(-1);
//--- equal
   return(0);
  }
//+------------------------------------------------------------------+