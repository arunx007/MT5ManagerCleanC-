//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
//---
#include "stdafx.h"
#include "UniNewsProtocol.h"
//+------------------------------------------------------------------+
//| Salt for hash calculations                                       |
//+------------------------------------------------------------------+
const BYTE CUniNewsProtocol::s_salt[]=
  {
   0x14,0xf0,0x2e,0x0c,0x48,0x2b,0xa1,0xe4,0xb6,0x77,0xf5,0x87,0x1b,0x5e,0x0c,0x54,
   0xea,0x4f,0xcb,0x61,0xc2,0x0a,0x86,0xd2,0x9e,0xc9,0xbd,0xf1,0xcb,0x3d,0x56,0x09
   };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUniNewsProtocol::CUniNewsProtocol(CUniNewsContextBase &context)
   : m_context(context),
     m_buffer(NULL),
     m_buffer_total(0),
     m_buffer_max(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUniNewsProtocol::~CUniNewsProtocol(void)
  {
//--- free buffer
   if(m_buffer)
      delete[] m_buffer;
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CUniNewsProtocol::Shutdown(void)
  {
//--- reset unprocessed data size
   m_buffer_total=0;
  }
//+------------------------------------------------------------------+
//| Notification of getting new data                                 |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnReceive(const char *buffer,const uint32_t buffer_size)
  {
   uint32_t processed=0;
//--- check buffer
   if(!buffer || buffer_size==0)
      return(false);
//--- if there is no unprocessed data, parse incoming data immediately
   if(m_buffer_total==0)
     {
      //--- parse data
      if(!BufferParse(buffer,buffer_size,processed))
         return(false);
      //--- add unprocessed data into buffer
      if(processed<buffer_size && !BufferAdd(buffer+processed,buffer_size-processed,0))
         return(false);
     }
   else
     {
      //--- if there is unprocessed data, add incoming data into buffer
      if(!BufferAdd(buffer,buffer_size,m_buffer_total))
         return(false);
      //--- parse buffer
      if(!BufferParse(m_buffer,m_buffer_total,processed))
         return(false);
      //--- add unprocessed data into buffer
      if(processed<m_buffer_total)
        {
         if(!BufferAdd(m_buffer+processed,m_buffer_total-processed,0))
            return(false);
        }
      else
         m_buffer_total=0;
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Notification of an incoming message                              |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessage(const TagHeader *message_tag)
  {
   bool res=true;
//--- check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- analize message type
   switch(message_tag->type)
     {
      case TAG_MSG_PING:
         res=OnMessagePing(message_tag);
         break;
      case TAG_MSG_LOGIN:
         res=OnMessageLogin(message_tag);
         break;
      case TAG_MSG_LOGIN_RESULT:
         res=OnMessageLoginResult(message_tag);
         break;
      case TAG_MSG_LOGOUT:
         res=OnMessageLogout(message_tag);
         break;
      case TAG_MSG_SUBSCRIBE:
         res=OnMessageSubscribe(message_tag);
         break;
      case TAG_MSG_NEWS:
         res=OnMessageNews(message_tag);
         break;
      default:
         ExtLogger.Out(MTLogOK,L"Unknown message received, message type: %u, message size: %u",message_tag->type,message_tag->size);
         break;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Notification of login message                                    |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessageLogin(const TagHeader *message_tag)
  {
//--- check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- login message
   CUniNewsMsgLogin msg;
//--- set data to message
   if(!msg.SetRaw((char*)message_tag,message_tag->size))
      return(false);
//--- notify context
   return(m_context.OnMessageLogin(&msg));
  }
//+------------------------------------------------------------------+
//| Notification of login result message                             |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessageLoginResult(const TagHeader *message_tag)
  {
//---check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- login result message
   CUniNewsMsgLoginResult msg;
//--- set data to message
   if(!msg.SetRaw((char*)message_tag,message_tag->size))
      return(false);
//--- notify context
   return(m_context.OnMessageLoginResult(&msg));
  }
//+------------------------------------------------------------------+
//| Notification of logout message                                   |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessageLogout(const TagHeader *message_tag)
  {
//---check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- logout message
   CUniNewsMsgLogout msg;
//--- set data to message
   if(!msg.SetRaw((char*)message_tag,message_tag->size))
      return(false);
//--- notify context
   return(m_context.OnMessageLogout(&msg));
  }
//+------------------------------------------------------------------+
//| Notification of subscription message                             |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessageSubscribe(const TagHeader *message_tag)
  {
//---check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- subscription message
   CUniNewsMsgSubscribe msg;
//--- set data to message
   if(!msg.SetRaw((char*)message_tag,message_tag->size))
      return(false);
//--- notify context
   return(m_context.OnMessageSubscribe(&msg));
  }
//+------------------------------------------------------------------+
//| Notification of news message                                     |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessageNews(const TagHeader *message_tag)
  {
//---check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- news message
   CUniNewsMsgNews msg;
//--- set data to message
   if(!msg.SetRaw((char*)message_tag,message_tag->size))
      return(false);
//--- notify context
   return(m_context.OnMessageNews(&msg));
  }
//+------------------------------------------------------------------+
//| Notification of ping message                                     |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::OnMessagePing(const TagHeader *message_tag)
  {
//---check
   if(!message_tag || message_tag->size<sizeof(TagHeader))
      return(false);
//--- ping message
   CUniNewsMsgPing msg;
//--- set data to message
   if(!msg.SetRaw((char*)message_tag,message_tag->size))
      return(false);
//--- notify context
   return(m_context.OnMessagePing(&msg));
  }
//+------------------------------------------------------------------+
//| Parse incoming data                                              |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::BufferParse(const char *buffer,const uint32_t buffer_size,uint32_t &size_processed)
  {
   TagHeader *msg;
   uint32_t   offset;
//--- reset processed data amount
   size_processed=0;
//---check
   if(!buffer)
      return(false);
//--- go through all messages
   for(offset=0,msg=NULL;offset+sizeof(TagHeader)<buffer_size;offset+=msg->size)
     {
      //--- get message tag pointer
      msg=(TagHeader*)(buffer+offset);
      //--- check message size
      if(msg->size>CUniNewsMsg::MAX_MSG_SIZE)
         return(false);
      //--- exit if tag exceeds buffer borders
      if(size_processed+msg->size>buffer_size)
         break;
      //--- increase processed data amount
      size_processed+=msg->size;
      //--- notify about new message
      if(!OnMessage(msg))
         return(false);
     }
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
//| Add unprocessed data at specifiedbuffer position                 |
//+------------------------------------------------------------------+
bool CUniNewsProtocol::BufferAdd(const char *buffer,const uint32_t buffer_size,uint32_t pos)
  {
//---check
   if(!buffer)
      return(false);
//--- check position
   if(pos>m_buffer_total)
      pos=m_buffer_total;
//--- check buffer size
   if(pos+buffer_size>MAX_BUFFER_SIZE)
      return(false);
//--- if buffer too small for new data, reallocate it
   if(!m_buffer || pos+buffer_size>m_buffer_max)
     {
      char *buffer_new=NULL;
      //--- allocate buffer with news size
      if((buffer_new=new(std::nothrow) char[pos+buffer_size+BUFFER_STEP])==NULL)
         return(false);
      //--- copy old data
      if(m_buffer && pos>0)
         memcpy(buffer_new,m_buffer,pos);
      //--- free old buffer
      if(m_buffer)
         delete[] m_buffer;
      //--- use new buffer
      m_buffer=buffer_new;
      //--- save new buffer size
      m_buffer_max=pos+buffer_size+BUFFER_STEP;
     }
//--- check buffer
   if(!m_buffer)
      return(false);
//--- copy new data
   memcpy(m_buffer+pos,buffer,buffer_size);
//--- update data size
   m_buffer_total=pos+buffer_size;
//--- successed
   return(true);
  }
//+------------------------------------------------------------------+
