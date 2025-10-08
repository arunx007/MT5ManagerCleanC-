//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
//---
#include "stdafx.h"
#include "ExchangeProtocol.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeProtocol::CExchangeProtocol(CExchangeContextBase &context)
   : m_context(context),
     m_buffer(NULL),
     m_buffer_datasize(0),
     m_buffer_maxsize(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeProtocol::~CExchangeProtocol(void)
  {
//--- delete buffer
   if(m_buffer)
     {
      delete[] m_buffer;
      m_buffer=NULL;
     }
//---
   m_buffer_maxsize =0;
   m_buffer_datasize=0;
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CExchangeProtocol::Shutdown(void)
  {
//--- clear buffer
   BufferClear();
  }
//+------------------------------------------------------------------+
//| Notify of data receipt                                           |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnReceive(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- add data to buffer
   if(!BufferAdd(buffer,buffer_size))
     {
      ExtLogger.Out(MTLogErr,L"failed to add data to incoming buffer (buffer size=%u Kb)",buffer_size/1024);
      return(false);
     }
//--- parse message buffer, logging inside
   return(BufferParse());
  }
//+------------------------------------------------------------------+
//| Get message                                                      |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessage(const char *buffer,const uint32_t buffer_size)
  {
   bool res=false;
//--- check
   if(!buffer || buffer_size==0 || buffer_size>MSG_MAX_SIZE)
      return(false);
//--- check message protocol version
   if(!CExchangeMsg::CheckVersion(buffer,buffer_size))
      return(false);
//--- get message type
   uint32_t msg_type=MSG_TYPE_UNKNOWN;
   if(!CExchangeMsg::GetType(buffer,buffer_size,msg_type))
     {
      //--- write to log
      ExtLogger.OutString(MTLogErr,L"failed to get message type");
      return(false);
     }
//--- analyze message type
   switch(msg_type)
     {
      case MSG_TYPE_LOGIN:
        {
         //--- login message
         res=OnMessageLogin(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_LOGOUT:
        {
         //--- logout message
         res=OnMessageLogout(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_SYMBOL:
        {
         //--- message with symbol settings
         res=OnMessageSymbol(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_TICK:
        {
         //--- tick message
         res=OnMessageTick(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_ORDER:
        {
         //--- order message
         res=OnMessageOrder(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_HEARTBEAT:
        {
         //--- HeartBeat message
         res=OnMessageHeartBeat(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_DEAL:
        {
         //--- deal message
         res=OnMessageDeal(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_BOOK:
        {
         //--- book message
         res=OnMessageBook(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_ACCOUNT_DATA_REQUEST:
        {
         //--- book message
         res=OnMessageAccountDataRequest(buffer,buffer_size);
         break;
        }
      case MSG_TYPE_ACCOUNT_DATA:
        {
         //--- book message
         res=OnMessageAccountData(buffer,buffer_size);
         break;
        }
      default:
        {
         //--- unknown type of message
         ExtLogger.Out(MTLogErr,L"unknown message type (%u)",msg_type);
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process login message                                            |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageLogin(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgLogin msg={0};
   if(!ExchangeMsgLogin::Read(buffer,buffer_size,msg))
      return(false);
//--- read message from buffer to message structure
   return(m_context.OnMessageLogin(msg));
  }
//+------------------------------------------------------------------+
//| Process logout message                                           |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageLogout(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgLogout msg={0};
   if(!ExchangeMsgLogout::Read(buffer,buffer_size,msg))
      return(false);
//--- read message from buffer to message structure
   return(m_context.OnMessageLogout(msg));
  }
//+------------------------------------------------------------------+
//| Process message with symbol settings                             |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageSymbol(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgSymbol msg={0};
   if(!ExchangeMsgSymbol::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageSymbol(msg));
  }
//+------------------------------------------------------------------+
//| Process message with tick                                        |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageTick(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgTick msg={0};
   if(!ExchangeMsgTick::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageTick(msg));
  }
//+------------------------------------------------------------------+
//| Process message with book                                        |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageBook(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgBook msg={0};
   if(!ExchangeMsgBook::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageBook(msg));
  }
//+------------------------------------------------------------------+
//| Process message with order                                       |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageOrder(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgOrder msg={0};
   if(!ExchangeMsgOrder::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageOrder(msg));
  }
//+------------------------------------------------------------------+
//| Process message with account data request                        |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageAccountDataRequest(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgAccountDataRequest msg={0};
   if(!ExchangeMsgAccountDataRequest::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageAccountDataRequest(msg));
  }
//+------------------------------------------------------------------+
//| Process message with orders list                                 |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageAccountData(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgAccountData msg={0};
   if(!ExchangeMsgAccountData::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageAccountData(msg));
  }
//+------------------------------------------------------------------+
//| Process message with deal                                        |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageDeal(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgDeal msg={0};
   if(!ExchangeMsgDeal::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageDeal(msg));
  }
//+------------------------------------------------------------------+
//| Process HeartBeat message                                        |
//+------------------------------------------------------------------+
bool CExchangeProtocol::OnMessageHeartBeat(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- read message from buffer to message structure
   ExchangeMsgHeartBeat msg={0};
   if(!ExchangeMsgHeartBeat::Read(buffer,buffer_size,msg))
      return(false);
//--- notify context about message
   return(m_context.OnMessageHeartBeat(msg));
  }
//+------------------------------------------------------------------+
//| Add data to buffer                                               |
//+------------------------------------------------------------------+
bool CExchangeProtocol::BufferAdd(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0 || m_buffer_datasize+buffer_size>BUFFER_MAX_SIZE)
      return(false);
//--- check available size in buffer
   if(!BufferAllocate(buffer_size))
      return(false);
//--- copy buffer
   memcpy_s(m_buffer+m_buffer_datasize,m_buffer_maxsize-m_buffer_datasize,buffer,buffer_size);
//--- update data size
   m_buffer_datasize+=buffer_size;
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse buffer to get messages                                     |
//+------------------------------------------------------------------+
bool CExchangeProtocol::BufferParse(void)
  {
   bool res=true;
//--- check
   if(!m_buffer || m_buffer_datasize<1)
      return(false);
//--- beginning of message
   char *msg_crnt=m_buffer;
//--- skip possible line breaks in the beginning of message
   while((*msg_crnt=='\r' || *msg_crnt=='\n') && msg_crnt<m_buffer+m_buffer_datasize)
      msg_crnt++;
//--- remember beginning of message
   char *msg_beg=msg_crnt;
   char *msg_end=NULL;
//--- find end of message 
   while(msg_crnt<m_buffer+m_buffer_datasize)
     {
      //--- compare current character with character of message end
      if(*msg_crnt==MSG_SEPARATOR)
        {
         //--- calculate end of message
         msg_end=msg_crnt;
         //--- notify protocol of incoming message
         if(!OnMessage(msg_beg,(uint32_t)(msg_end-msg_beg)))
           {
            ExtLogger.OutString(MTLogErr,L"failed to process incoming message");
            res=false;
            break;
           }
         //--- go to the beginning of next message
         msg_crnt=msg_end;
         //--- skip possible line breaks in the end of message
         while((*msg_crnt=='\r' || *msg_crnt=='\n') && msg_crnt<m_buffer+m_buffer_datasize)
            msg_crnt++;
         //--- remember pointer to the beginning of message
         msg_beg=msg_crnt;
        }
      else
         msg_crnt++;
     }
//--- if successful
   if(res)
     {
      //--- skip possible line breaks in the beginning of message
      while((*msg_beg=='\r' || *msg_beg=='\n') && msg_beg<m_buffer+m_buffer_datasize)
         msg_beg++;
      //--- calculate amount of remaining data in buffer
      m_buffer_datasize=(uint32_t)(m_buffer+m_buffer_datasize-msg_beg);
      //--- shift buffer, move unprocessed data to the beginning of buffer
      memmove_s(m_buffer,m_buffer_maxsize,msg_beg,m_buffer_datasize);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Buffer reallocation                                              |
//+------------------------------------------------------------------+
bool CExchangeProtocol::BufferAllocate(const uint32_t buffer_size)
  {
//--- if there is no buffer or it is not big enough, then create/recreate buffer
   if(!m_buffer || (m_buffer_datasize+buffer_size>m_buffer_maxsize))
     {
      //--- new buffer
      char *new_buffer     =NULL;
      uint32_t  new_buffer_size=0;
      //--- calculate size of new buffer
      new_buffer_size=((m_buffer_datasize+buffer_size)/BUFFER_REALLOC_STEP+1)*BUFFER_REALLOC_STEP;
      //--- create buffer
      if((new_buffer=new(std::nothrow) char[new_buffer_size])==NULL)
         return(false);
      //--- if there is an old buffer, copy it to the new one and then delete
      if(m_buffer)
        {
         //--- copy
         memcpy_s(new_buffer,new_buffer_size,m_buffer,m_buffer_datasize);
         //--- delete old buffer
         delete[] m_buffer;
        }
      //--- set new buffer
      m_buffer        =new_buffer;
      m_buffer_maxsize=new_buffer_size;
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Clear buffer                                                     |
//+------------------------------------------------------------------+
void CExchangeProtocol::BufferClear(void)
  {
//--- clear buffer
   if(m_buffer!=NULL && m_buffer_maxsize>0)
     {
      ZeroMemory(m_buffer,m_buffer_maxsize);
      m_buffer_datasize=0;
     }
  }
//+------------------------------------------------------------------+
