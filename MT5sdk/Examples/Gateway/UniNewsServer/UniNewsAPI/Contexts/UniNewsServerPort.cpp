//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UniNewsServerPort.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUniNewsServerPort::CUniNewsServerPort(CUniNewsSource &data_source) : m_data_source(data_source),m_socket(INVALID_SOCKET),m_workflag(false),m_connections(NULL)
  {
//--- clear strings
   m_login[0]=L'\0';
   m_password[0]=L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUniNewsServerPort::~CUniNewsServerPort(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CUniNewsServerPort::Initialize(LPCWSTR address,LPCWSTR login,LPCWSTR password)
  {
//--- check address
   if(!address || !login || !password)
      return(false);
//--- copy login
   CMTStr::Copy(m_login,login);
//--- copy password
   CMTStr::Copy(m_password,password);
//--- check socket
   if(m_socket!=INVALID_SOCKET)
      closesocket(m_socket);
//--- create socket for connections
   if((m_socket=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
     {
      ExtLogger.Out(MTLogErr,L"socket error [%d]",WSAGetLastError());
      return(false);
     }
//--- convert address
   SOCKADDR_IN sa={0};
   if(!ConvertAddress(address,sa))
     {
      ExtLogger.Out(MTLogErr,L"invalid server address specified %s",address);
      closesocket(m_socket); m_socket=INVALID_SOCKET;
      return(false);
     }
//--- bind socket to address
   if(bind(m_socket,(LPSOCKADDR)&sa,sizeof(sa))!=0)
     {
      ExtLogger.Out(MTLogErr,L"bind error on %s [%d]",address,WSAGetLastError());
      closesocket(m_socket); m_socket=INVALID_SOCKET;
      return(false);
     }
//--- set socket listen mode
   if(listen(m_socket,SOMAXCONN)==SOCKET_ERROR)
     {
      ExtLogger.Out(MTLogErr,L"listen error [%u]",GetLastError());
      closesocket(m_socket); m_socket=INVALID_SOCKET;
      return(false);
     }
//--- reset thread workflag
   m_workflag=true;
//--- create listening thread
   if(!m_thread.Start(ServerThreadWrapper,(void*)this,STACK_SIZE_COMMON))
     {
      m_workflag=false;
      closesocket(m_socket); m_socket=INVALID_SOCKET;
      ExtLogger.Out(MTLogErr,L"server thread error [%u]",GetLastError());
      return(false);
     }
//--- successed
   ExtLogger.Out(MTLogOK,L"started on %s",address);
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown server port                                             |
//+------------------------------------------------------------------+
void CUniNewsServerPort::Shutdown(void)
  {
   CUniNewsContext *dead,*next;
//--- check if already stopped
   if(!m_workflag)
      return;
//--- reset workflag
   m_workflag=false;
//--- close server socket
   if(m_socket!=INVALID_SOCKET)
     {
      shutdown(m_socket,2);
      closesocket(m_socket);
      m_socket=INVALID_SOCKET;
     }
//--- wait for thread
   m_thread.Shutdown();
//--- clear contexts
   m_sync.Lock();
   dead=m_connections; m_connections=NULL;
   m_sync.Unlock();
//--- delete all contexts
   while(dead!=NULL)
     {
      next=dead->Next();
      delete dead;
      dead=next;
     }
  }
//+------------------------------------------------------------------+
//| Check all connections activity                                   |
//+------------------------------------------------------------------+
bool CUniNewsServerPort::Check(void)
  {
   CUniNewsContext *next,*last=NULL,*dead=NULL;
   int64_t            ctm=_time64(nullptr);
//--- check all connection contexts
   m_sync.Lock();
   next=m_connections;
   while(next!=NULL)
     {
      //--- check connection context
      if(!next->Check(ctm))
        {
         if(last!=NULL)
           {
            //--- remove context from list
            last->Next(next->Next());
            //--- insert context into dead list
            next->Next(dead);
            dead=next;
            //--- go to the next context
            next=last->Next();
           }
         else
           {
            //--- set new first context
            m_connections=next->Next();
            //--- insert context into dead list
            next->Next(dead);
            dead=next;
            //--- go to the next context
            next=m_connections;
           }
         continue;
        }
      //--- go to the next context
      last=next; next=next->Next();
     }
   m_sync.Unlock();
//--- delete all stuck contexts
   while(dead!=NULL)
     {
      next=dead->Next();
      delete dead;
      dead=next;
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Send news to all connections                                     |
//+------------------------------------------------------------------+
bool CUniNewsServerPort::SendNews(CUniNewsMsgNews *news,LPCWSTR keywords)
  {
//--- check
   if(!news || !keywords)
      return(false);
//--- send news to all connection contexts
   m_sync.Lock();
   CUniNewsContext *next=m_connections;
   while(next!=NULL)
     {
      //--- send news to context
      next->SendNews(news,keywords);
      //--- go to the next context
      next=next->Next();
     }
   m_sync.Unlock();
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Start thread of connections accepting                            |
//+------------------------------------------------------------------+
uint32_t __stdcall CUniNewsServerPort::ServerThreadWrapper(LPVOID param)
  {
//--- check
   CUniNewsServerPort *pThis=reinterpret_cast<CUniNewsServerPort*>(param);
   if(pThis)
      pThis->ServerThread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Convert address                                                  |
//+------------------------------------------------------------------+
bool CUniNewsServerPort::ConvertAddress(LPCWSTR address,SOCKADDR_IN &sa)
  {
   wchar_t addr_w[MAX_PATH];
   char    addr_a[MAX_PATH];
//--- check
   if(!address)
      return(false);
//--- clear sockaddr structure
   ZeroMemory(&sa,sizeof(sa));
//--- copy server address
   CMTStr::Copy(addr_w,address);
//--- find port location
   int32_t pos=CMTStr::FindChar(addr_w,L':');
//--- cut port number from address
   if(pos>0)
     {
      //--- terminate address
      addr_w[pos]=L'\0';
      //--- set port number
      sa.sin_port=htons(_wtoi(&addr_w[pos+1]));
     }
//--- set default port
   if(sa.sin_port==0)
      sa.sin_port=DEFAULT_NEWS_SERVER_PORT;
//--- copy address in ANSI
   CMTStr::Copy(addr_a,addr_w);
//--- convert address
   if((sa.sin_addr.s_addr=inet_addr(addr_a))==INADDR_NONE)
      sa.sin_addr.s_addr=INADDR_ANY;
//--- set address family
   sa.sin_family=AF_INET;
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Thread of accetping incoming connections                         |
//+------------------------------------------------------------------+
__declspec(noinline) void CUniNewsServerPort::ServerThread(void)
  {
   CUniNewsContext *context;
   SOCKADDR_IN      addr;
   SOCKET           incom;
   int32_t              len=sizeof(SOCKADDR_IN);
//--- main loop
   while(m_workflag)
     {
      //--- accept incoming connection
      if((incom=accept(m_socket,(LPSOCKADDR)&addr,&len))==INVALID_SOCKET)
        {
         //--- check if server stopped
         if(!m_workflag)
            break;
         //--- write to log
         ExtLogger.Out(MTLogErr,L"accept error [%d]",WSAGetLastError());
         continue;
        }
      //--- create connection context
      if((context=new(std::nothrow) CUniNewsContext(incom,addr.sin_addr.s_addr,m_data_source))==NULL || !context->Initialize(m_login,m_password))
        {
         //--- delete context
         if(context)
            delete context;
         //--- close connection
         shutdown(incom,2);
         closesocket(incom);
         //--- write to log
         ExtLogger.OutString(MTLogErr,L"failed to initialize new connection context");
         continue;
        }
      //--- insert context into list
      m_sync.Lock();
      context->Next(m_connections);
      m_connections=context;
      m_sync.Unlock();
     }
  }
//+------------------------------------------------------------------+

