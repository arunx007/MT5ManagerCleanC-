//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "UniNewsContext.h"
//+------------------------------------------------------------------+
//| Default parameters                                               |
//+------------------------------------------------------------------+
#define DEFAULT_NEWS_SERVER_ADDRESS       L"127.0.0.1"            // default server address
#define DEFAULT_NEWS_SERVER_PORT          16839                   // default server port
#define DEFAULT_NEWS_SERVER_LOGIN         L"login"                // default user login
#define DEFAULT_NEWS_SERVER_PASSWORD      L"password"             // default user password
//+------------------------------------------------------------------+
//| Server port                                                      |
//+------------------------------------------------------------------+
class CUniNewsServerPort
  {

private:
   //--- connection login and password
   wchar_t           m_login[64];
   wchar_t           m_password[64];
   //--- news source reference
   CUniNewsSource   &m_data_source;
   //--- server socket
   SOCKET            m_socket;
   //--- server thread
   CMTThread         m_thread;
   volatile bool     m_workflag;
   //--- contexts
   CMTSync           m_sync;
   CUniNewsContext  *m_connections;

public:
                     CUniNewsServerPort(CUniNewsSource &data_source);
                    ~CUniNewsServerPort(void);
   //--- initialization/shutdown
   bool              Initialize(LPCWSTR address,LPCWSTR login,LPCWSTR password);
   void              Shutdown(void);
   //--- thread check
   bool              Check(void);
   //--- send news to all connections
   bool              SendNews(CUniNewsMsgNews *news,LPCWSTR keywords);

private:
   //--- convert address
   bool              ConvertAddress(LPCWSTR address,SOCKADDR_IN &sa);
   //--- server thread methods
   void              ServerThread(void);
   static uint32_t __stdcall ServerThreadWrapper(LPVOID);
  };
//+------------------------------------------------------------------+
