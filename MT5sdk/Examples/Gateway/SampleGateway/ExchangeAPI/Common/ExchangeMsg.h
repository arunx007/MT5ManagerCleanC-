//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Macro for reading/writing message fields                         |
//+------------------------------------------------------------------+
//+------------------------------------------------------------------+
//| Read string value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_STR(tag_name,tag_value)\
   if(!CExchangeMsg::GetTagValueStr(buffer,buffer_size,tag_name,tag_value,_countof(tag_value))) return(false);\
//+------------------------------------------------------------------+
//| Read uint32_t value from specified tag                               |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_UINT(tag_name,tag_value)\
   if(!CExchangeMsg::GetTagValueUINT(buffer,buffer_size,tag_name,tag_value)) return(false);\
//+------------------------------------------------------------------+
//| Read uint64_t value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_UINT64(tag_name,tag_value)\
   if(!CExchangeMsg::GetTagValueUINT64(buffer,buffer_size,tag_name,tag_value)) return(false);\
//+------------------------------------------------------------------+
//| Read int64_t value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_INT64(tag_name,tag_value)\
   if(!CExchangeMsg::GetTagValueINT64(buffer,buffer_size,tag_name,tag_value)) return(false);\
//+------------------------------------------------------------------+
//| Read double value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_DBL(tag_name,tag_value)\
   if(!CExchangeMsg::GetTagValueDbl(buffer,buffer_size,tag_name,tag_value)) return(false);\
//+------------------------------------------------------------------+
//| Read specified string value from specified tag                   |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_NUM_STR(tag_name,tag_value,tag_num)\
   if(!CExchangeMsg::GetTagValueStr(buffer,buffer_size,tag_name,tag_value,_countof(tag_value),tag_num)) return(false);\
//+------------------------------------------------------------------+
//| Read uint32_t value from specified tag                               |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_NUM_UINT(tag_name,tag_value,tag_num)\
   if(!CExchangeMsg::GetTagValueUINT(buffer,buffer_size,tag_name,tag_value,tag_num)) return(false);\
//+------------------------------------------------------------------+
//| Read uint64_t value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_NUM_UINT64(tag_name,tag_value,tag_num)\
   if(!CExchangeMsg::GetTagValueUINT64(buffer,buffer_size,tag_name,tag_value,tag_num)) return(false);\
//+------------------------------------------------------------------+
//| Read int64_t value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_NUM_INT64(tag_name,tag_value,tag_num)\
   if(!CExchangeMsg::GetTagValueINT64(buffer,buffer_size,tag_name,tag_value,tag_num)) return(false);\
//+------------------------------------------------------------------+
//| Read double value from specified tag                             |
//+------------------------------------------------------------------+
#define READ_MSG_TAG_NUM_DBL(tag_name,tag_value,tag_num)\
   if(!CExchangeMsg::GetTagValueDbl(buffer,buffer_size,tag_name,tag_value,tag_num)) return(false);\
//+------------------------------------------------------------------+
//| Write string value for specified tag                             |
//+------------------------------------------------------------------+
#define WRITE_MSG_TAG_STR(tag_name,tag_value)\
   if((len=CExchangeMsg::SetTagFormat(tag_name,buffer_ptr,buffer_size,"%S%c",tag_value,MSG_SEPARATOR_TAG))<1) return(false);\
   buffer_ptr +=len;\
   buffer_size-=len;\
//+------------------------------------------------------------------+
//| Write uint32_t value for specified tag                               |
//+------------------------------------------------------------------+
#define WRITE_MSG_TAG_UINT(tag_name,tag_value)\
   if((len=CExchangeMsg::SetTagFormat(tag_name,buffer_ptr,buffer_size,"%u%c",tag_value,MSG_SEPARATOR_TAG))<1) return(false);\
   buffer_ptr +=len;\
   buffer_size-=len;\
//+------------------------------------------------------------------+
//| Write uint64_t value for specified tag                             |
//+------------------------------------------------------------------+
#define WRITE_MSG_TAG_UINT64(tag_name,tag_value)\
   if((len=CExchangeMsg::SetTagFormat(tag_name,buffer_ptr,buffer_size,"%I64u%c",tag_value,MSG_SEPARATOR_TAG))<1) return(false);\
   buffer_ptr +=len;\
   buffer_size-=len;\
//+------------------------------------------------------------------+
//| Write int64_t value for specified tag                              |
//+------------------------------------------------------------------+
#define WRITE_MSG_TAG_INT64(tag_name,tag_value)\
   if((len=CExchangeMsg::SetTagFormat(tag_name,buffer_ptr,buffer_size,"%I64d%c",tag_value,MSG_SEPARATOR_TAG))<1) return(false);\
   buffer_ptr +=len;\
   buffer_size-=len;\
//+------------------------------------------------------------------+
//| Write double value for specified tag                             |
//+------------------------------------------------------------------+
#define WRITE_MSG_TAG_DBL(tag_name,tag_value)\
   if((len=CExchangeMsg::SetTagFormat(tag_name,buffer_ptr,buffer_size,"%.5lf%c",tag_value,MSG_SEPARATOR_TAG))<1) return(false);\
   buffer_ptr +=len;\
   buffer_size-=len;\
//+------------------------------------------------------------------+
//| Write character of message end                                   |
//+------------------------------------------------------------------+
#define WRITE_MSG_SEPARATOR()\
   if((len=_snprintf_s(buffer_ptr,buffer_size,_TRUNCATE,"%c",MSG_SEPARATOR))<1) return(false);\
   buffer_size-=len;\
//+------------------------------------------------------------------+
//| Class with static methods of processing message data             |
//+------------------------------------------------------------------+
class CExchangeMsg
  {
public:
   //--- check message protocol version
   static bool       CheckVersion(const char *buffer,const uint32_t buffer_size);
   //--- get message type
   static bool       GetType(const char *buffer,const uint32_t buffer_size,uint32_t &msg_type);
   //--- get values of message tags
   static bool       GetTagValueStr(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,LPWSTR tag_value,const uint32_t tag_value_size,uint32_t tag_num=0);
   static bool       GetTagValueUINT(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,uint32_t &tag_value,uint32_t tag_num=0);
   static bool       GetTagValueUINT64(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,uint64_t &tag_value,uint32_t tag_num=0);
   static bool       GetTagValueINT64(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,int64_t &tag_value,uint32_t tag_num=0);
   static bool       GetTagValueDbl(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,double &tag_value,uint32_t tag_num=0);
   //--- write tag value into buffer with formatting
   static int32_t    SetTagFormat(LPCSTR tag_name,char *buffer,int buffer_size,LPCSTR fmt,...);
  };
//+------------------------------------------------------------------+
//| Message header                                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct ExchangeMsgHeader
  {
   uint32_t          version;
   uint32_t          type;
   //--- serialization methods
   static bool       Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgHeader &msg);
   static bool       Write(const ExchangeMsgHeader &msg,char* &buffer_ptr,int &buffer_size,int &len);
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Read message from buffer to structure                            |
//+------------------------------------------------------------------+
inline bool ExchangeMsgHeader::Read(const char *buffer,const uint32_t buffer_size,ExchangeMsgHeader &msg)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- get protocol version
   READ_MSG_TAG_UINT(MSG_TAG_HEADER_VERSION,msg.version)
//--- get type
   READ_MSG_TAG_UINT(MSG_TAG_HEADER_TYPE,msg.type)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Write structure to buffer                                        |
//+------------------------------------------------------------------+
inline bool ExchangeMsgHeader::Write(const ExchangeMsgHeader &msg,char* &buffer_ptr,int &buffer_size,int &len)
  {
//--- check
   if(!buffer_ptr || buffer_size==0)
      return(false);
//--- write protocol version
   WRITE_MSG_TAG_UINT(MSG_TAG_HEADER_VERSION,msg.version)
//--- write type
   WRITE_MSG_TAG_UINT(MSG_TAG_HEADER_TYPE,msg.type)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
