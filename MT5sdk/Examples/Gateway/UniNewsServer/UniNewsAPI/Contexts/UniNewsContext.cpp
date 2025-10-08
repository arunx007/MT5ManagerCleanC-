//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UniNewsContext.h"
//+------------------------------------------------------------------+
//| Connection ID                                                    |
//+------------------------------------------------------------------+
volatile long CUniNewsContext::m_connect_id=0;
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUniNewsContext::CUniNewsContext(SOCKET sock,const uint32_t ip,CUniNewsSource &data_source) : m_data_source(data_source),m_socket(sock),m_workflag(false),m_next(NULL),m_protocol(*this),m_receive_checktime(0),m_send_checktime(0),m_subscribe_from(0),m_subscribed(false)
  {
//--- increment connection id
   ::InterlockedIncrement(&m_connect_id);
//--- format ip address
   m_ip.Format(L"%u.%u.%u.%u:%u",ip&255,(ip>>8)&255,(ip>>16)&255,(ip>>24)&255,(uint32_t)m_connect_id);
//--- clear subscription languages list
   ZeroMemory(m_subscribe_languages,sizeof(m_subscribe_languages));
   ZeroMemory(m_login,sizeof(m_login));
   ZeroMemory(m_password,sizeof(m_password));
//--- update working time
   UpdateTime(THREAD_TIMEOUT);
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUniNewsContext::~CUniNewsContext(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Initialize datafeed connection context                           |
//+------------------------------------------------------------------+
bool CUniNewsContext::Initialize(LPCWSTR login,LPCWSTR password)
  {
//--- check
   if(!login || !password)
      return(false);
//--- already working
   if(m_workflag)
      Shutdown();
//--- copy login
   CMTStr::Copy(m_login,login);
//--- copy password
   CMTStr::Copy(m_password,password);
//--- initialize buffers
   m_buffer_receive.Reallocate(RECEIVE_BUFFER_SIZE);
//--- clear subscription parameters
   m_subscribed=false;
   m_subscribe_from=0;
   m_subscribe_keywords.Clear();
   ZeroMemory(m_subscribe_languages,sizeof(m_subscribe_languages));
//--- reset data send/receive time
   m_send_checktime=m_receive_checktime=_time64(nullptr);
//--- create thread
   m_workflag=true;
//--- start thread
   if(!m_thread.Start(ProcessThreadWrapper,(void*)this,STACK_SIZE_COMMON))
     {
      ExtLogger.Out(MTLogErr,L"failed to start context thread (%d) for connection %s",::GetLastError(),m_ip.Str());
      m_workflag=false;
      Shutdown();
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CUniNewsContext::Shutdown(void)
  {
//--- send logout message
   SendLogout(CUniNewsMsgLogout::LOGOUT_REASON_SERVER);
//--- reset workflag
   m_workflag=false;
//--- shutdown thread
   m_thread.Shutdown();
//--- close connection
   CloseConnection();
//--- shutdown protocol
   m_protocol.Shutdown();
//--- speed up shutdown
   UpdateTime((int64_t)0);
  }
//+------------------------------------------------------------------+
//| Check context                                                    |
//+------------------------------------------------------------------+
bool CUniNewsContext::Check(const int64_t curtime)
  {
//--- check time of last activity and last data send/receive
   return(curtime<m_close_time &&
          curtime<m_send_checktime+PING_PERIOD_MAX &&
          curtime<m_receive_checktime+PING_PERIOD_MAX);
  }
//+------------------------------------------------------------------+
//| Close connection                                                 |
//+------------------------------------------------------------------+
void CUniNewsContext::CloseConnection(void)
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
//| Send specified message                                           |
//+------------------------------------------------------------------+
bool CUniNewsContext::MessageSend(CUniNewsMsg *msg)
  {
//--- check message
   if(!msg)
      return(false);
//--- get message data
   const char *data     =msg->GetRaw();
   uint32_t    data_size=msg->GetMsgSize();
//--- check message data
   if(!data || data_size==0)
      return(false);
//--- lock buffer
   m_buffer_sync.Lock();
//--- add data to buffer
   if(!m_buffer_send.Add(data,data_size))
      ExtLogger.Out(MTLogErr,L"%s failed to add %u bytes of data to send buffer",m_ip.Str(),data_size);
//--- unlock buffer
   m_buffer_sync.Unlock();
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Login message notification                                       |
//+------------------------------------------------------------------+
bool CUniNewsContext::OnMessageLogin(CUniNewsMsgLogin *msg)
  {
   uint32_t result=CUniNewsMsgLoginResult::RESULT_SUCCESSFUL;
//--- check message
   if(!msg)
      return(false);
//--- check protocol version
   if(msg->GetVersion()!=UNINEWS_PROTOCOL_VERSION)
     {
      //--- write to log
      ExtLogger.Out(MTLogWarn,L"%s unsupported protocol version %u, closing connection",m_ip.Str(),msg->GetVersion());
      //--- set result
      result=CUniNewsMsgLoginResult::RESULT_UNSUPPORTED_PROTOCOL;
     }
   else
     {
      //--- check login
      if(!CheckLogin(msg->GetLogin(),msg->GetPasswordHash()))
        {
         //--- write to log
         ExtLogger.Out(MTLogWarn,L"%s invalid login or password used",m_ip.Str());
         //--- set result
         result=CUniNewsMsgLoginResult::RESULT_INVALID_PASSWORD;
        }
     }
//--- send login result message
   if(!SendLoginResult(result))
      return(false);
//--- write to log
   if(result==CUniNewsMsgLoginResult::RESULT_SUCCESSFUL)
      ExtLogger.Out(MTLogWarn,L"%s authorized with login '%s'",m_ip.Str(),msg->GetLogin());
   else
      m_workflag=false;
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Logout message notification                                      |
//+------------------------------------------------------------------+
bool CUniNewsContext::OnMessageLogout(CUniNewsMsgLogout *msg)
  {
//--- check message
   if(!msg)
      return(false);
//--- write to log
   ExtLogger.Out(MTLogOK,L"%s client closed connection by reason (%u)",m_ip.Str(),msg->GetReason());
//--- stop working thread
   m_workflag=false;
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Subscribtion message notification                                |
//+------------------------------------------------------------------+
bool CUniNewsContext::OnMessageSubscribe(CUniNewsMsgSubscribe *msg)
  {
//--- check message
   if(!msg)
      return(false);
//--- get subscription parameters
   m_subscribe_from=msg->GetTimeFrom();
//--- save subscribtion languages
   if(msg->GetLanguages() && msg->GetLanguagesTotal()>0)
      memcpy(m_subscribe_languages,msg->GetLanguages(),sizeof(uint32_t)*msg->GetLanguagesTotal());
//--- parse keywords   
   if(msg->GetKeywords())
     {
      wchar_t  keyword[64];
      wchar_t *ptr=(LPWSTR)msg->GetKeywords();
      int32_t      pos=0;
      //--- find all keywords splitted by commas
      while((pos=CMTStr::FindChar(ptr,L','))>=0)
        {
         //--- skip empty keywords
         if(pos==0)
           {
            ptr++;
            continue;
           }
         //--- assign keyword
         CMTStr::Copy(keyword,ptr,pos);
         //--- add new keyword
         if(!m_subscribe_keywords.Add(&keyword))
           {
            ExtLogger.Out(MTLogErr,L"%s failed to add subscription keyword to list",m_ip.Str());
            return(false);
           }
         //--- move to next keyword after comma
         ptr+=pos+1;
        }
      //--- add last keyword
      if(*ptr!='\0')
        {
         //--- assign keyword
         CMTStr::Copy(keyword,ptr);
         //--- add new keyword
         if(!m_subscribe_keywords.Add(&keyword))
           {
            ExtLogger.Out(MTLogErr,L"%s failed to add subscription keyword to list",m_ip.Str());
            return(false);
           }

        }
     }
//--- format log record
   CMTStr2048 str;
   str.Format(L"subscribed on news from %I64d, keywords [",m_subscribe_from);
   for(uint32_t i=0;i<m_subscribe_keywords.Total();i++)
     {
      str.Append(m_subscribe_keywords[i]);
      if(i<m_subscribe_keywords.Total()-1)
         str.Append(L",");
     }
   str.Append(L"], languages [");
   bool first=true;
   for(uint32_t i=0;i<_countof(m_subscribe_languages);i++)
      if(m_subscribe_languages[i]>0)
        {
         CMTStr32 buf;
         buf.Format(L"%u",m_subscribe_languages[i]);
         if(!first)
            str.Append(L",");
         str.Append(buf);
         first=false;
        }
   str.Append(L"]");
//--- write to log   
   ExtLogger.Out(MTLogOK,L"%s %s",m_ip.Str(),str.Str());
//--- send news history
   ExtLogger.Out(MTLogOK,L"%s sending news history since %I64u",m_ip.Str(),m_subscribe_from);
   if(m_data_source.SendNewsHistory(*this))
      m_subscribed=true;
   else
     {
      //--- history news sending failed
      ExtLogger.Out(MTLogErr,L"%s failed to send news history",m_ip.Str());
      m_workflag=false;
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Send login result                                                |
//+------------------------------------------------------------------+
bool CUniNewsContext::SendLoginResult(const uint32_t result/*=CUniNewsMsgLoginResult::RESULT_COMMON_ERROR*/)
  {
   CUniNewsMsgLoginResult msg;
   bool                   res=true;
//--- set protocol version
   res=res && msg.SetVersion(UNINEWS_PROTOCOL_VERSION);
//--- set result
   res=res && msg.SetResult(result);
//--- send message
   res=res && MessageSend(&msg);
//--- write error to log
   if(!res)
      ExtLogger.Out(MTLogErr,L"%s failed to send login result message");
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send logout message                                              |
//+------------------------------------------------------------------+
bool CUniNewsContext::SendLogout(const uint32_t reason/*=CUniNewsMsgLogout::LOGOUT_REASON_UNKNOWN*/)
  {
   CUniNewsMsgLogout msg;
   bool              res=true;
//--- set reason
   res=res && msg.SetReason(reason);
//--- send message
   res=res && MessageSend(&msg);
//--- write error to log
   if(!res)
      ExtLogger.Out(MTLogErr,L"%s failed to send logout message");
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send news message                                                |
//+------------------------------------------------------------------+
bool CUniNewsContext::SendNews(CUniNewsMsgNews *news,LPCWSTR keywords,bool history_news/*=false*/)
  {
//--- check
   if(!news || !keywords)
      return(false);
//--- skip nonhistory news if client still not subscribed
   if(!history_news && !m_subscribed)
      return(true);
//--- skip old news
   if(m_subscribe_from>0 && news->GetDatetime()<=m_subscribe_from)
      return(true);
//--- check news language
   uint32_t lang=news->GetLanguage();
   bool skip_news=true;
   for(uint32_t i=0;i<_countof(m_subscribe_languages);i++)
      if(m_subscribe_languages[i]==lang)
        {
         skip_news=false;
         break;
        }
//--- skip news
   if(skip_news)
      return(true);
//--- check keywords
   skip_news=true;
//--- go through all keywords and check
   for(uint32_t i=0;i<m_subscribe_keywords.Total();i++)
      if(CMTStr::CompareNoCase(m_subscribe_keywords[i],L"any")==0 || CMTStr::Find(keywords,m_subscribe_keywords[i])>=0)
        {
         skip_news=false;
         break;
        }
//--- skip news
   if(skip_news && m_subscribe_keywords.Total()>0)
      return(true);
//--- send news message
   if(!MessageSend(news))
     {
      ExtLogger.Out(MTLogErr,L"%s failed to send news message",m_ip.Str());
      return(false);
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Send ping message                                                |
//+------------------------------------------------------------------+
bool CUniNewsContext::SendPing(void)
  {
//--- ping message
   CUniNewsMsgPing msg;
//--- send message
   if(!MessageSend(&msg))
     {
      ExtLogger.Out(MTLogErr,L"%s failed to send ping message",m_ip.Str());
      return(false);
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Check login                                                      |
//+------------------------------------------------------------------+
bool CUniNewsContext::CheckLogin(LPCWSTR login,LPCWSTR password_hash)
  {
//--- check
   if(!login || !password_hash)
      return(false);
//--- check login
   if(CMTStr::Compare(m_login,login)!=0)
      return(false);
//--- check password
   UniNewsPasswordContainer container={0};
//--- copy login and password into container
   CMTStr::Copy(container.login,m_login);
   CMTStr::Copy(container.password,m_password);
//--- copy salt into container
   memcpy_s(container.salt,sizeof(container.salt),CUniNewsProtocol::s_salt,sizeof(CUniNewsProtocol::s_salt));
//--- calculate hash
   char sha_hash[32]={0};
   CSHA256::CalculateHash((PUCHAR)&container,sizeof(container),(PUCHAR)sha_hash,sizeof(sha_hash));
//--- clear source data
   SecureZeroMemory(&container,sizeof(container));
//--- get hex hash representation
   wchar_t hex_byte[4] ={0};
   wchar_t hex_data[_countof(sha_hash)*2+1 ]={0};
   for(uint32_t i=0;i<_countof(sha_hash);i++)
     {
      CMTStr::FormatStr(hex_byte,L"%02x",(uint8_t)sha_hash[i]);
      CMTStr::Append(hex_data,hex_byte);
     }
//--- check hash
   if(CMTStr::Compare(hex_data,password_hash)!=0)
      return(false);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Start thread of connection processing                            |
//+------------------------------------------------------------------+
uint32_t __stdcall CUniNewsContext::ProcessThreadWrapper(LPVOID param)
  {
//--- check
   CUniNewsContext *pThis=reinterpret_cast<CUniNewsContext*>(param);
   if(pThis)
      pThis->ProcessThread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Process messages                                                 |
//+------------------------------------------------------------------+
__declspec(noinline) void CUniNewsContext::ProcessThread(void)
  {
//--- write to log about new connection
   ExtLogger.Out(MTLogOK,L"%s connected",m_ip.Str());
//--- reset data send/receive time
   m_send_checktime=m_receive_checktime=_time64(nullptr);
//--- flags of data processing
   bool data_send=false,data_receive=false;
//--- messages processing cycle
   while(m_workflag && Connected())
     {
      //--- receive data
      if(!Receive(data_receive))
        {
         ExtLogger.Out(MTLogErr,L"%s data receive error [%d]",m_ip.Str(),WSAGetLastError());
         CloseConnection();
         break;
        }
      //--- send data
      if(!Send(data_send))
        {
         ExtLogger.Out(MTLogErr,L"%s send data error [%d]",m_ip.Str(),WSAGetLastError());
         CloseConnection();
         break;
        }
      //--- update working time
      UpdateTime((int64_t)THREAD_TIMEOUT);
      //--- if there was no data, sleep
      if(!data_send && !data_receive)
         Sleep(THREAD_SLEEP);
     }
//--- send remaining data without checking result
   if(Connected())
      Send(data_send);
//--- thread finished
   ExtLogger.Out(MTLogOK,L"%s finished",m_ip.Str());
  }
//+------------------------------------------------------------------+
//| Receive data                                                     |
//+------------------------------------------------------------------+
bool CUniNewsContext::Receive(bool &data_processed)
  {
   int32_t   buffer_received=0;
   DWORD err_code       =0;
   bool  res            =false;
   int32_t   data_available =0;
//--- reset data processed flag
   data_processed=false;
//--- check connection
   if(!Connected())
      return(false);
//--- get available data amount
   data_available=IsReadible();
//--- there are not incoming data
   if(data_available<=0)
      return(true);
//--- check available data size
   if((uint32_t)data_available>m_buffer_receive.Max())
      data_available=m_buffer_receive.Max();
//--- clear buffer
   m_buffer_receive.Clear();
//--- read data
   if(res=Read(m_buffer_receive.Buffer(),data_available,buffer_received,&err_code))
     {
      m_buffer_receive.Len(buffer_received);
      //--- notificate protocol about received data
      if(buffer_received>0)
        {
         //--- set last receive time
         m_receive_checktime=_time64(nullptr);
         //--- set flag if data was processed
         if(res=m_protocol.OnReceive(m_buffer_receive.Buffer(),m_buffer_receive.Len()))
            data_processed=true;
        }
     }
   else
     {
      //--- write to log
      ExtLogger.Out(MTLogErr,L"%s failed to read data from socket (%u)",m_ip.Str(),err_code);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send data                                                        |
//+------------------------------------------------------------------+
bool CUniNewsContext::Send(bool &data_processed)
  {
   DWORD err_code=0;
//--- reset data processed flag
   data_processed=false;
//--- check connection
   if(!Connected())
      return(false);
//--- clear temp buffer
   m_buffer_tmp.Clear();
//--- lock buffer
   m_buffer_sync.Lock();
//--- swap buffers
   m_buffer_send.Swap(m_buffer_tmp);
//--- unlock buffer
   m_buffer_sync.Unlock();
//--- check data amount
   if(m_buffer_tmp.Len()>0)
     {
      //--- write data to client
      if(!Write(m_buffer_tmp.Buffer(),m_buffer_tmp.Len(),&err_code))
        {
         //--- write to log
         ExtLogger.Out(MTLogErr,L"%s failed to send %u bytes of data (%u)",m_ip.Str(),m_buffer_tmp.Len(),err_code);
         return(false);
        }
      //--- update last send time
      m_send_checktime=_time64(nullptr);
     }
   else
     {
      //--- if there are no data, send ping
      if(m_send_checktime+PING_PERIOD<_time64(nullptr))
         SendPing();
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Check readable data                                              |
//+------------------------------------------------------------------+
int32_t CUniNewsContext::IsReadible(void)
  {
   unsigned long size=0;
//--- check available data amount
   if(m_socket!=INVALID_SOCKET)
      if(ioctlsocket(m_socket,FIONREAD,&size)!=0)
        {
         CloseConnection();
         size=0;
        }
//--- return data amount
   return(size);
  }
//+------------------------------------------------------------------+
//| Read socket data                                                 |
//+------------------------------------------------------------------+
bool CUniNewsContext::Read(char *buf,int buf_max,int &buf_readed,DWORD* err_code/*=NULL*/)
  {
//--- reset error code
   if(err_code)
      *err_code=0;
//--- check
   if(!buf || buf_max<1 || m_socket==INVALID_SOCKET)
      return(false);
//--- read data
   buf_readed=recv(m_socket,buf,buf_max,0);
//--- nothing was read
   if(buf_readed==SOCKET_ERROR)
     {
      DWORD last_error;
      //--- reset read data
      buf_readed=0;
      //--- analize error code
      if((last_error=WSAGetLastError())!=WSAEWOULDBLOCK)
        {
         //--- set error code
         if(err_code)
            *err_code=last_error;
         //--- close connection
         CloseConnection();
         return(false);
        }
     }
//--- update working time
   UpdateTime(THREAD_TIMEOUT);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Write socket data                                                |
//+------------------------------------------------------------------+
bool CUniNewsContext::Write(const char *sendbuf,const uint32_t len,DWORD* err_code/*=NULL*/)
  {
   DWORD   bytes=0;
   WSABUF  wb;
//--- reset error code
   if(err_code)
      *err_code=0;
//--- check
   if(!sendbuf || m_socket==INVALID_SOCKET)
      return(false);
//--- prepare data
   wb.buf=(char*)sendbuf;
   wb.len=len;
//--- send data and check amount
   if(WSASend(m_socket,&wb,1,&bytes,0,NULL,NULL)!=0 || bytes!=wb.len)
     {
      //--- set error code
      if(err_code)
         *err_code=WSAGetLastError();
      //--- close connection
      CloseConnection();
      return(false);
     }
//--- update working time
   UpdateTime(THREAD_TIMEOUT);
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
