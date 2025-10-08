//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeSocket.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeSocket::CExchangeSocket(void)
   : m_socket_server(INVALID_SOCKET),
     m_socket_client(INVALID_SOCKET),
     m_socket_client_timeout(DEFAULT_SOCKET_TIMEOUT),
     m_socket_client_event(WSA_INVALID_EVENT)
  {
//--- clear strings
   m_error_desc[0]=L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeSocket::~CExchangeSocket(void)
  {
//--- shutdown everything
   ClientClose();
   ServerClose();
  }
//+------------------------------------------------------------------+
//| Receive incoming connection                                      |
//+------------------------------------------------------------------+
bool CExchangeSocket::ClientConnect(LPCWSTR address,const uint32_t port)
  {
   bool res=false;
//--- check
   if(!address || port==0)
      return(res);
//--- shutdown client socket
   ClientClose();
//--- shutdown server socket
   ServerClose();
//--- initialize server socket
   res=ServerPrepare(address,port);
//--- wait for client connection
   res=res && ClientAccept();
//--- shutdown server socket
   ServerClose();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Read data from socket                                            |
//+------------------------------------------------------------------+
bool CExchangeSocket::ClientRead(char *buf,const uint32_t buf_max,uint32_t &buf_readed)
  {
//--- check
   if(!buf || buf_max<1 || m_socket_client==INVALID_SOCKET)
      return(false);
//--- read data
   buf_readed=recv(m_socket_client,buf,buf_max,0);
//--- read nothing
   if(buf_readed==SOCKET_ERROR)
     {
      //--- reset amount of read data
      buf_readed=0;
      //--- analyze error code
      DWORD error_code=WSAGetLastError();
      if(error_code!=WSAEWOULDBLOCK)
        {
         ErrorSet(error_code);
         ClientClose();
         return(false);
        }
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Send data                                                        |
//+------------------------------------------------------------------+
bool CExchangeSocket::ClientSend(const char *buf,const uint32_t buf_size)
  {
   WSAOVERLAPPED send_overlapped;
   WSABUF        send_buffer;
   DWORD         bytes=0;
//--- check
   if(!buf || buf_size<1 || m_socket_client==INVALID_SOCKET)
      return(false);
//--- create socket event, if not already
   if(m_socket_client_event==WSA_INVALID_EVENT)
     {
      //--- create
      if((m_socket_client_event=WSACreateEvent())==WSA_INVALID_EVENT)
        {
         //--- remember error description
         ErrorSet(WSAGetLastError());
         return(false);
        }
     }
//--- send data
   send_buffer.buf       =(char*)buf;
   send_buffer.len       =buf_size;
   send_overlapped.hEvent=m_socket_client_event;
//--- send and check that everything has been send
   if(WSASend(m_socket_client,&send_buffer,1,&bytes,0,&send_overlapped,NULL)==0 && bytes==send_buffer.len)
      return(true);
//--- analyze error code
   DWORD error_code=WSAGetLastError();
   if(error_code!=WSA_IO_PENDING)
     {
      ErrorSet(error_code);
      ClientClose();
      return(false);
     }
//--- wait for the end of data sending
   if(WSAWaitForMultipleEvents(1,&send_overlapped.hEvent,TRUE,m_socket_client_timeout,FALSE)==WSA_WAIT_TIMEOUT)
     {
      ErrorSet(WSAGetLastError());
      ClientClose();
      return(false);
     }
//--- successful, waited until the end of input/output
   return(true);
  }
//+------------------------------------------------------------------+
//| Set error description                                            |
//+------------------------------------------------------------------+
void CExchangeSocket::ErrorSet(DWORD error_code)
  {
   CMTStrPath error_desc;
//--- copy error description
   error_desc.ErrorMsg(error_code);
//--- set error description
   ErrorSet(error_desc.Str());
  }
//+------------------------------------------------------------------+
//| Set error description                                            |
//+------------------------------------------------------------------+
void CExchangeSocket::ErrorSet(LPCWSTR fmt,...)
  {
//--- check
   if(fmt==NULL)
      return;
//--- data
   va_list arg_ptr;
//--- arguments list
   va_start(arg_ptr,fmt);
//--- format string and set value
   _vsnwprintf_s(m_error_desc,_countof(m_error_desc)-1,_TRUNCATE,fmt,arg_ptr);
//--- end of arguments list
   va_end(arg_ptr);
  };
//+------------------------------------------------------------------+
//| Get error description                                            |
//+------------------------------------------------------------------+
LPCWSTR CExchangeSocket::ErrorGet(void)
  {
//--- return error description
   if(m_error_desc[0]!=L'\0')
      return(m_error_desc);
   else
      return(L"unknown error");
  }
//+------------------------------------------------------------------+
//| Prepare incoming connection                                      |
//+------------------------------------------------------------------+
bool CExchangeSocket::ServerPrepare(LPCWSTR address,const uint32_t port)
  {
   sockaddr_in serv_addr   ={0};
   char       *ptr         =NULL;
   char        addr_str[64]={0};
   ULONG       addr_int    =0;
//--- check
   if(!address || port==0)
      return(false);
//--- create socket
   if((m_socket_server=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
     {
      ExtLogger.Out(MTLogErr,L"failed to create socket (%d)",::WSAGetLastError());
      return(false);
     }
//--- copy address in ANSI
   CMTStr::Copy(addr_str,_countof(addr_str),address);
//--- convert address
   if((addr_int=inet_addr(addr_str))==INADDR_NONE)
      addr_int=INADDR_ANY;
//--- set address and port
   serv_addr.sin_addr.s_addr=addr_int;
   serv_addr.sin_family     =AF_INET;
   serv_addr.sin_port       =htons(port);
//--- bind socket to address
   if(bind(m_socket_server,(sockaddr*)&serv_addr,sizeof(serv_addr))!=0)
     {
      ExtLogger.Out(MTLogErr,L"failed to bind socket on port %u (%d)",port,::WSAGetLastError());
      return(false);
     }
//--- wait for incoming connection
   if(listen(m_socket_server,5)==SOCKET_ERROR)
     {
      ExtLogger.Out(MTLogErr,L"failed to listen socket (%d)",::WSAGetLastError());
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Manage socket operation mode (blocking/non blocking)             |
//+------------------------------------------------------------------+
void CExchangeSocket::ClientSetBlocking(const bool blocking)
  {
   ULONG block_flag;
//--- check
   if(m_socket_client==INVALID_SOCKET)
      return;
//--- determine desired mode
   if(blocking)
      block_flag=0;
   else
      block_flag=1;
//--- switch socket to desired mode
   ioctlsocket(m_socket_client,(long)FIONBIO,&block_flag);
  }
//+------------------------------------------------------------------+
//| Receive incoming client connection                               |
//+------------------------------------------------------------------+
bool CExchangeSocket::ClientAccept(void)
  {
   SOCKET       sock=INVALID_SOCKET;
   SOCKADDR_IN  accept_sin;
   fd_set       fd;
   timeval      tv={DEFAULT_SOCKET_TIMEOUT/1000,0};
   int32_t      len=0;
   char        *ptr=NULL;
//--- check if there is an incoming connection
   FD_ZERO(&fd);
   FD_SET(m_socket_server,&fd);
//--- wait for connection
   select(1,&fd,NULL,NULL,&tv);
//--- check
   if(!FD_ISSET(m_socket_server,&fd))
      return(false);
//--- accept incoming connection
   len=sizeof(accept_sin);
   if((sock=accept(m_socket_server,(sockaddr *)&accept_sin,(int32_t *)&len))==INVALID_SOCKET)
     {
      ExtLogger.Out(MTLogErr,L"failed to accept incoming connection (%d)",WSAGetLastError());
      return(false);
     }
//--- remember incoming connection socket
   m_socket_client=sock;
//--- switch socket to non blocking mode
   ClientSetBlocking(false);
//--- write to log about received connection
   ExtLogger.Out(MTLogOK,L"incoming connection from %S",inet_ntoa(accept_sin.sin_addr));
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Disconnect                                                       |
//+------------------------------------------------------------------+
void CExchangeSocket::Close(SOCKET &sock)
  {
//--- close socket if there is any
   if(sock!=INVALID_SOCKET)
     {
      shutdown(sock,2);
      closesocket(sock);
      sock=INVALID_SOCKET;
     }
  }
//+------------------------------------------------------------------+
