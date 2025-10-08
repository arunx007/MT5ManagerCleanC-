//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeSocket.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeSocket::CExchangeSocket(void)
   : m_socket(INVALID_SOCKET),
     m_socket_timeout(DEFAULT_SOCKET_TIMEOUT),
     m_socket_event(WSA_INVALID_EVENT)
  {
//--- clear strings
   m_error_desc[0]=L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeSocket::~CExchangeSocket(void)
  {
//--- close socket
   CExchangeSocket::Close();
//--- close event, if any
   if(m_socket_event!=WSA_INVALID_EVENT)
     {
      WSACloseEvent(m_socket_event);
      m_socket_event=WSA_INVALID_EVENT;
     }
  }
//+------------------------------------------------------------------+
//| Establish connection                                             |
//+------------------------------------------------------------------+
bool CExchangeSocket::Connect(LPCWSTR address,const uint32_t port)
  {
   sockaddr_in srv   ={0};
   int32_t         result=SOCKET_ERROR;
//--- check
   if(!address || port==0)
      return(false);
//--- close old connection
   Close();
//--- check address
   uint32_t addr            =0;
   char tmp_address[256]={0};
   CMTStr::Copy(tmp_address,_countof(tmp_address),address);
   if((addr=inet_addr(tmp_address))==INADDR_NONE)
     {
      hostent *hp=NULL;
      if((hp=gethostbyname(tmp_address))==NULL)
         return(false);
      //--- get address
      srv.sin_addr.s_addr=*((ULONG*)hp->h_addr);
     }
   else
      srv.sin_addr.s_addr=addr;
//--- type of address and port
   srv.sin_family=AF_INET;
   srv.sin_port  =htons(port);
//--- create socket
   if((m_socket=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
     {
      ErrorSet(::WSAGetLastError());
      return(false);
     }
//--- connect with infinite timeout
   if((result=connect(m_socket,(sockaddr*)&srv,sizeof(srv)))==SOCKET_ERROR)
      ErrorSet(::WSAGetLastError());
//--- close everything in case of error
   if(result==SOCKET_ERROR)
     {
      Close();
      return(false);
     }
//--- switch socket to non blocking mode
   SetBlocking(false);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Close connection                                                 |
//+------------------------------------------------------------------+
void CExchangeSocket::Close(void)
  {
//--- check socket
   if(m_socket!=INVALID_SOCKET)
     {
      shutdown(m_socket,2);
      closesocket(m_socket);
      m_socket=INVALID_SOCKET;
     }
  }
//+------------------------------------------------------------------+
//| Read data from socket                                            |
//+------------------------------------------------------------------+
bool CExchangeSocket::Read(char *buf,const uint32_t buf_max,uint32_t &buf_readed)
  {
//--- check
   if(!buf || buf_max==0 || m_socket==INVALID_SOCKET)
      return(false);
//--- read data
   buf_readed=recv(m_socket,buf,buf_max,0);
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
         Close();
         return(false);
        }
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Send data                                                        |
//+------------------------------------------------------------------+
bool CExchangeSocket::Send(const char *buf,const uint32_t buf_size)
  {
   WSAOVERLAPPED send_overlapped;
   WSABUF        send_buffer;
   DWORD         bytes=0;
//--- check
   if(!buf || buf_size==0 || m_socket==INVALID_SOCKET)
      return(false);
//--- create socket event, if not already
   if(m_socket_event==WSA_INVALID_EVENT)
     {
      //--- create
      if((m_socket_event=WSACreateEvent())==WSA_INVALID_EVENT)
        {
         //--- remember error description
         ErrorSet(WSAGetLastError());
         return(false);
        }
     }
//--- send data
   send_buffer.buf       =(char*)buf;
   send_buffer.len       =buf_size;
   send_overlapped.hEvent=m_socket_event;
//--- send and check that everything has been send
   if(WSASend(m_socket,&send_buffer,1,&bytes,0,&send_overlapped,NULL)==0 && bytes==send_buffer.len)
      return(true);
//--- analyze error code
   DWORD error_code=WSAGetLastError();
   if(error_code!=WSA_IO_PENDING)
     {
      ErrorSet(error_code);
      Close();
      return(false);
     }
//--- wait for the end of data sending
   if(WSAWaitForMultipleEvents(1,&send_overlapped.hEvent,TRUE,m_socket_timeout,FALSE)==WSA_WAIT_TIMEOUT)
     {
      ErrorSet(WSAGetLastError());
      Close();
      return(false);
     }
//--- successful, waited until the end of input/output
   return(true);
  }
//+------------------------------------------------------------------+
//| Manage socket operation mode (blocking/non blocking)             |
//+------------------------------------------------------------------+
void CExchangeSocket::SetBlocking(const bool blocking)
  {
   ULONG block_flag;
//--- check
   if(m_socket==INVALID_SOCKET)
      return;
//--- set desired socket mode
   if(blocking)
      block_flag=0;
   else
      block_flag=1;
//--- switch socket to desired mode
   ioctlsocket(m_socket,(long)FIONBIO,&block_flag);
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
  }
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
