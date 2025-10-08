//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UniFeederSocket.h"
#include "..\DataSource\DataSource.h"
//+------------------------------------------------------------------+
//| Forward declarations                                             |
//+------------------------------------------------------------------+
class CDataSource;
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUniFeederSocket::CUniFeederSocket(CDataSource &parent)
   : m_parent(parent),m_socket(INVALID_SOCKET),m_pingtime(0),m_closetime(0),m_data(NULL),
     m_data_max(0),m_data_readed(0),m_data_total(0)
  {
//--- null the string
   m_string[0]=L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUniFeederSocket::~CUniFeederSocket(void)
  {
//--- close the socket
   Close();
//--- release the memory
   if(m_data)
     {
      delete[] m_data;
      m_data=NULL;
     }
//--- null positions
   m_data_max=m_data_readed=m_data_total=0;
  }
//+------------------------------------------------------------------+
//| Checking the connection                                          |
//+------------------------------------------------------------------+
bool CUniFeederSocket::Check(void)
  {
//--- socket not closed yet?
   if(m_socket==INVALID_SOCKET)
      return(false);
//--- check the time
   if(_time64(nullptr)>=m_closetime)
     {
      //--- close the socket
      Close();
      return(false);
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Check connection to the server and connect, if necessary         |
//+------------------------------------------------------------------+
bool CUniFeederSocket::Connect(LPCSTR server,LPCSTR login,LPCSTR password,LPCSTR symbols)
  {
//--- checking
   if(!server || !login || !password)
      return(false);
//--- if already connected, fine, quit
   if(m_socket!=INVALID_SOCKET)
      return(true);
//--- return the connection result
   return(ConnectDDE(server) && Login(login,password,symbols));
  }
//+------------------------------------------------------------------+
//| Closing the socket                                               |
//+------------------------------------------------------------------+
void CUniFeederSocket::Close(void)
  {
//--- if the socket is open, close it
   if(m_socket!=INVALID_SOCKET)
     {
      shutdown(m_socket,2);
      closesocket(m_socket);
      m_socket=INVALID_SOCKET;
     }
  }
//+------------------------------------------------------------------+
//| Receipt of ticks                                                 |
//+------------------------------------------------------------------+
bool CUniFeederSocket::ReadTicks(MTTickArray &ticks)
  {
   MTTick tick={0};
//--- read data from socket
   if(!DataCheck())
      return(false);
//--- process all received strings
   while(DataGetString(m_string,_countof(m_string)-1))
     {
      //--- reset tick
      ZeroMemory(&tick,sizeof(MTTick));
      //--- read one tick and store it in the buffer
      if(ReadTick(m_string,tick))
         if(!ticks.Add(&tick))
            return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Connection to UniDDE server                                      |
//+------------------------------------------------------------------+
bool CUniFeederSocket::ConnectDDE(LPCSTR server)
  {
   struct sockaddr_in srv={0};
   struct hostent    *hp=NULL;
   char              *cp=NULL,ip[128]={0};
   uint32_t           addr=0;
   int32_t                port=0;
//--- checking
   if(!server || !server[0])
      return(false);
//--- unparse the server and port
   strncpy_s(ip,_countof(ip),server,_TRUNCATE);
   if((cp=strstr(ip,":"))!=NULL)
     {
      port=atoi(cp+1);
      *cp=0;
     }
//--- close the old connection
   Close();
//--- refresh the time
   UpdateTime();
//--- create a socket
   if((m_socket=socket(AF_INET,SOCK_STREAM,0))==INVALID_SOCKET)
     {
      ExtLogger.Out(MTLogErr,L"unifeeder socket error [%d]",WSAGetLastError());
      return(false);
     }
//--- get a host
   if((addr=inet_addr(ip))==INADDR_NONE)
     {
      if((hp=gethostbyname(ip))==NULL)
        {
         Close();
         ExtLogger.Out(MTLogErr,L"unifeeder socket unresolved address [%S]",ip);
         return(false);
        }
      srv.sin_addr.s_addr=*((unsigned long*)hp->h_addr);
     }
   else
      srv.sin_addr.s_addr=addr;
//--- fill out parameters
   srv.sin_family=AF_INET;
   srv.sin_port  =htons(port);
//--- connect
   if(connect(m_socket,(struct sockaddr*)&srv,sizeof(srv))!=0)
     {
      Close();
      ExtLogger.Out(MTLogErr,L"unifeeder socket connect to %S failed [%d]",server,WSAGetLastError());
      return(false);
     }
//--- refresh the time
   UpdateTime();
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Authorization                                                    |
//+------------------------------------------------------------------+
bool CUniFeederSocket::Login(LPCSTR login,LPCSTR password,LPCSTR symbols)
  {
   char  tmp[MAX_PATH]={0};
   int32_t   count=0;
   bool  found=false;
//--- checking
   if(!login || !password || !symbols)
      return(false);
//--- form a command
   _snprintf_s(tmp,_countof(tmp)-1,_TRUNCATE,"%s\r\n",login);
//--- send a command
   if(!SendString(tmp))
     {
      ExtLogger.Out(MTLogErr,L"unifeeder socket login failed [%S]",login);
      return(false);
     }
//--- wait for a login line
   for(count=0;count<LOGIN_COUNT_MAX;count++)
     {
      //--- read the next string and check inclusion of the sub-string "Login: "
      if(ReadStringCheck(tmp,sizeof(tmp)-1,"Login: ",found) && found)
         break;
     }
//--- if we couldn't find the string, error
   if(!found)
     {
      ExtLogger.Out(MTLogErr,L"unifeeder socket invalid login [%S]",login);
      return(false);
     }
//--- form a command
   _snprintf_s(tmp,_countof(tmp)-1,_TRUNCATE,"%s\r\n",password);
//--- send a command
   if(!SendString(tmp))
     {
      ExtLogger.Out(MTLogErr,L"unifeeder socket login failed [%S]",login);
      return(false);
     }
//--- wait for a password line
   for(count=0;count<LOGIN_COUNT_MAX;count++)
     {
      //--- read the next string and check inclusion of the sub-string "Password: "
      if(ReadStringCheck(tmp,sizeof(tmp)-1,"Password: ",found) && found)
         break;
     }
//--- if we couldn't find the string, error
   if(!found)
     {
      ExtLogger.Out(MTLogErr,L"unifeeder socket invalid headers [%S]",login);
      return(false);
     }
//--- read the reply
   for(count=0;count<LOGIN_COUNT_MAX;count++)
     {
      //--- read the next string and check inclusion of the sub-string "Password: "
      if(ReadStringCheck(tmp,sizeof(tmp)-1,"Access granted",found))
         break;
     }
//--- if we couldn't find the string, error
   if(!found)
     {
      ExtLogger.Out(MTLogErr,L"unifeeder socket invalid access [%S]",login);
      return(false);
     }
//--- login is confirmed
   m_pingtime=_time64(nullptr);
//--- reset positions
   m_data_total=m_data_readed=0;
//--- send symbols
   if(symbols && symbols[0])
     {
      if(!SendString("> Symbols:") || !SendString(symbols) || !SendString("\r\n"))
        {
         ExtLogger.OutString(MTLogErr,L"unifeeder socket symbols initialization failed");
         return(false);
        }
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+ 
//| Reading a tick from a string                                     | 
//| Format: USDJPY 0 1.0106 1.0099                                   | 
//+------------------------------------------------------------------+
bool CUniFeederSocket::ReadTick(LPSTR str,MTTick &tick)
  {
   char symbols[MAX_PATH]={0},*cp=NULL,*np=NULL;
   int32_t  params=1;
//--- checking
   if(!str)
      return(false);
//--- search for the tick symbol
   if((cp=strstr(str," "))==NULL)
      return(false);
//--- null the string
   *cp=0;
//--- form the tick symbol
   _snwprintf_s(tick.symbol,_countof(tick.symbol)-1,_TRUNCATE,L"%S",str);
//--- move to the next parameter
   np=cp+1;
//--- read the number of parameters
   while(*np!=0)
     {
      if(*np==' ')
         params++;
      np++;
     }
//--- skip time parameters
   if(params>2)
      if(!(cp=strstr(cp+1," ")))
         return(false);
//--- bid price
   tick.bid=atof(cp+1);
//--- the next parameter
   if((cp=strstr(cp+1," "))==NULL)
      return(true);
//--- ask price
   tick.ask=atof(cp+1);
//--- checking obtained data
   if(tick.bid<=0 || tick.ask<=0 || tick.bid>tick.ask)
     {
      ExtLogger.Out(MTLogErr,L"failed to parse tick, invalid bid/ask symbol '%s'",tick.symbol);
      return(false);
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Obtain the size of data in the socket                            |
//+------------------------------------------------------------------+
int32_t CUniFeederSocket::IsReadable(void)
  {
   ULONG size=0;
//--- checking the socket validity
   if(m_socket!=INVALID_SOCKET)
      if(ioctlsocket(m_socket,FIONREAD,&size)!=0)
        {
         Close();
         size=0;
        }
//--- return the size
   return(size);
  }
//+------------------------------------------------------------------+
//| Read data from the socket                                        |
//+------------------------------------------------------------------+
bool CUniFeederSocket::DataUpdate(void)
  {
   int32_t   len=0,res=0;
   char *cp=NULL;
//--- check if there are data in the socket
   if((len=IsReadable())<1)
      return(true);
//--- shift unprocessed data
   if(m_data!=NULL && m_data_readed>0)
     {
      if((m_data_total-m_data_readed)>0)
         memmove(m_data,m_data+m_data_readed,m_data_total-m_data_readed);
      //--- refresh pointers
      m_data_total       -=m_data_readed;
      m_data[m_data_total]=0;
      m_data_readed       =0;
     }
//--- checking the buffer size
   if((m_data_total+len)>m_data_max || !m_data)
     {
      //--- checking the maximal size
      if((m_data_total+len)>READ_BUFFER_MAX)
        {
         ExtLogger.Out(MTLogErr,L"datafeed incoming buffer too large %d bytes",m_data_total+len);
         return(false);
        }
      //--- select data
      if((cp=new char[m_data_total+len+READ_BUFFER_STEP+16])==NULL)
        {
         ExtLogger.Out(MTLogErr,L"no enough memory for %d bytes",m_data_total+len+1024);
         return(false);
        }
      //--- copy old data
      if(m_data!=NULL)
        {
         if(m_data_total>0)
            memcpy(cp,m_data,m_data_total);
         delete[] m_data; m_data=NULL;
        }
      //--- copy the pointer to m_data
      m_data    =cp;
      m_data_max=m_data_total+len+READ_BUFFER_STEP;
     }
//--- read data from the socket
   if((res=recv(m_socket,m_data+m_data_total,len,0))<1)
      return(false);
//--- update incoming traffic
   m_parent.StateTraffic(res,0);
//--- form the result
   m_data_total       +=res;
   m_data[m_data_total]=0;
//--- refresh the time
   UpdateTime();
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse the line obtained from the socket                          |
//+------------------------------------------------------------------+
bool CUniFeederSocket::DataGetString(char *buf,const int32_t maxlen)
  {
   char *start=m_data+m_data_readed,*end=m_data+m_data_total;
   int32_t   len=0;
//--- checking parameters
   if(!buf || maxlen<1 || !m_data || m_data_total<1)
      return(false);
//--- start parsing
   while(start<end && len<maxlen)
     {
      //--- checking till the end of the line
      if(*start=='\n')
        {
         //--- form the end of data
         *buf=0;
         m_data_readed+=len+1;
         //--- everything is ok
         return(true);
        }
      //--- copy data, skip the '\r' symbol
      if(*start!='\r')
         *buf++=*start;
      start++;
      len++;
     }
//--- error, couldn't read data
   return(false);
  }
//+------------------------------------------------------------------+
//| Data check                                                       |
//+------------------------------------------------------------------+
bool CUniFeederSocket::DataCheck(void)
  {
//--- check connection
   if(m_socket==INVALID_SOCKET)
      return(false);
//--- send ping to DDE Connector server if necessary
   if(!SendPing())
     {
      ExtLogger.OutString(MTLogErr,L"unifeeder socket ping failed");
      return(false);
     }
//--- get all data from the socket to buffer m_data
   if(!DataUpdate())
     {
      ExtLogger.OutString(MTLogErr,L"unifeeder socket connection lost");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Read data from the socket and compare to the preset substring    |
//+------------------------------------------------------------------+
bool CUniFeederSocket::ReadStringCheck(char *buf,const int32_t maxlen,LPCSTR str,bool &found)
  {
   bool res=false;
//--- checking
   if(!buf || !str)
      return(false);
//--- reset the flag that we've found the line
   found=false;
//--- reading data
   if(res=ReadString(buf,maxlen))
      found=strstr(buf,str)!=NULL;
//--- return the result
   return(res);
  }
//+------------------------------------------------------------------+
//| Read the string from the socket                                  |
//+------------------------------------------------------------------+
bool CUniFeederSocket::ReadString(char *buf,const int32_t maxlen)
  {
   int32_t count=0,len=0;
   char* ptr=buf;
//--- checking
   if(!buf || m_socket==INVALID_SOCKET)
      return(false);
//--- read data
   while(len<maxlen)
     {
      if(recv(m_socket,buf,1,0)!=1)
        {
         //--- checking errors of the socket
         if(WSAGetLastError()!=WSAEWOULDBLOCK || count>10)
           {
            Close();
            return(false);
           }
         //--- read data
         count++;
         Sleep(50);
         continue;
        }
      //--- reached the end of the string?
      if(*buf==13)
         continue;
      if(*buf==10)
         break;
      len++;
      buf++;
     }
//--- null the end of the string
   *buf=0;
//--- update incoming traffic
   m_parent.StateTraffic(len,0);
//--- raise the time
   UpdateTime();
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Sending ping to DDE Connector                                    |
//+------------------------------------------------------------------+
bool CUniFeederSocket::SendPing(void)
  {
   uint64_t ctm=_time64(nullptr);
//--- every minute send a ping signal to DDE Connector
   if((ctm-m_pingtime)>60)
     {
      //--- send the ping
      if(SendString("> Ping\r\n")==false)
        {
         ExtLogger.OutString(MTLogErr,L"datafeed ping failed");
         return(false);
        }
      //--- save the time of the last ping
      m_pingtime=ctm;
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Send the string                                                  |
//+------------------------------------------------------------------+
bool CUniFeederSocket::SendString(LPCSTR buf)
  {
   int32_t count=0,res,len,size;
//--- checking
   if(buf==NULL || m_socket==INVALID_SOCKET) return(false);
//--- send data
   len=size=(int)strlen(buf);
   while(len>0)
     {
      if((res=send(m_socket,buf,len,0))<1)
        {
         //--- checking for the socket error
         if(WSAGetLastError()!=WSAEWOULDBLOCK || count>10) { Close(); return(false); }
         //--- write the rest of data
         count++; Sleep(50); continue;
        }
      //--- change positions
      buf+=res; len-=res;
     }
//--- raise the time
   UpdateTime();
//--- update outgoing traffic
   m_parent.StateTraffic(0,size);
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+