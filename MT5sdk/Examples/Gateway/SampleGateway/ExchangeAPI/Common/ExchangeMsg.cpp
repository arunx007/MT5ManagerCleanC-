//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeMsg.h"
#include "ExchangeProtocolData.h"
//+------------------------------------------------------------------+
//| Check message protocol version                                   |
//+------------------------------------------------------------------+
bool CExchangeMsg::CheckVersion(const char *buffer,const uint32_t buffer_size)
  {
   uint32_t version=0;
   bool res    =true;
//--- get version
   READ_MSG_TAG_UINT(MSG_TAG_HEADER_VERSION,version)
//--- check version
   if(version!=EXCHANGE_PROTOCOL_VERSION)
     {
      ExtLogger.OutString(MTLogErr,L"invalid message protocol version");
      res=false;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get message type                                                 |
//+------------------------------------------------------------------+
bool CExchangeMsg::GetType(const char *buffer,const uint32_t buffer_size,uint32_t &msg_type)
  {
//--- get type
   READ_MSG_TAG_UINT(MSG_TAG_HEADER_TYPE,msg_type)
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Get value of message tag                                         |
//+------------------------------------------------------------------+
bool CExchangeMsg::GetTagValueStr(const char *buffer,const uint32_t buffer_size,LPCSTR msg_tag_name,LPWSTR msg_tag_value,const uint32_t msg_tag_value_size,uint32_t tag_num/*=0*/)
  {
   char tag_name[64]   ={0};     // tag name buffer
   char tag_value[512] ={0};     // tag value buffer
   uint32_t tag_name_count =0;       // size of tag name buffer
   uint32_t tag_value_count=0;       // size of tag value buffer
   bool tag_found      =false;   // tag found
   bool res            =false;   // result
//--- check
   if(!buffer || buffer_size==0 || !msg_tag_name || *msg_tag_name=='\0')
      return(false);
//--- search tag by name
   for(uint32_t i=0;i<buffer_size;i++)
     {
      //--- found tag separator
      if(buffer[i]==MSG_SEPARATOR_TAG)
        {
         //--- terminate tag name and tag value buffers
         tag_name[tag_name_count]  ='\0';
         tag_value[tag_value_count]='\0';
         //--- check tag name
         if(strcmp(tag_name,msg_tag_name)==0)
           {
            //--- skip first fields
            if(tag_num)
               tag_num--;
            else
              {
               //--- found, return result
               CMTStr::Copy(msg_tag_value,msg_tag_value_size,tag_value);
               res=true;
               break;
              }
           }
         //--- reset 'tag found' flag
         tag_found=false;
         //--- reset tag name and tag value buffers
         tag_name_count =0;
         tag_value_count=0;
         //--- next iteration
         continue;
        }
      //--- unless next tag is not found, copy tag name or tag value to buffers
      if(!tag_found)
        {
         //--- tag name, check size
         if(tag_name_count>_countof(tag_name)-1)
            return(false);
         //--- copy symbol to tag name buffer
         tag_name[tag_name_count++]=buffer[i];
         //--- found tag value separator
         if(buffer[i]==MSG_SEPARATOR_TAGVALUE)
           {
            //--- set 'tag found' flag
            tag_found=true;
           }
        }
      else
        {
         //--- tag value, check size
         if(tag_value_count>_countof(tag_value)-1)
            return(false);
         //--- copy symbol to tag value buffer
         tag_value[tag_value_count++]=buffer[i];
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get integer value of message tag                                 |
//+------------------------------------------------------------------+
bool CExchangeMsg::GetTagValueUINT(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,uint32_t &tag_value,uint32_t tag_num/*=0*/)
  {
   wchar_t str[64]={};
   bool    res    =false;
//--- check
   if(!buffer || buffer_size==0 || !tag_name)
      return(res);
//--- get tag data
   if(res=GetTagValueStr(buffer,buffer_size,tag_name,str,_countof(str),tag_num))
      tag_value=_wtoi(str);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get 64 bit integer value of message tag                          |
//+------------------------------------------------------------------+
bool CExchangeMsg::GetTagValueUINT64(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,uint64_t &tag_value,uint32_t tag_num/*=0*/)
  {
   wchar_t str[64]={};
   bool    res    =false;
//--- check
   if(!buffer || buffer_size==0 || !tag_name)
      return(res);
//--- get tag data
   if(res=GetTagValueStr(buffer,buffer_size,tag_name,str,_countof(str),tag_num))
      tag_value=_wtoi64(str);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get 64 bit integer value of message tag                          |
//+------------------------------------------------------------------+
bool CExchangeMsg::GetTagValueINT64(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,int64_t &tag_value,uint32_t tag_num/*=0*/)
  {
   wchar_t str[64]={};
   bool    res    =false;
//--- check
   if(!buffer || buffer_size==0 || !tag_name)
      return(res);
//--- get tag data
   if(res=GetTagValueStr(buffer,buffer_size,tag_name,str,_countof(str),tag_num))
      tag_value=_wtoi64(str);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get floating point value of message tag                          |
//+------------------------------------------------------------------+
bool CExchangeMsg::GetTagValueDbl(const char *buffer,const uint32_t buffer_size,LPCSTR tag_name,double &tag_value,uint32_t tag_num/*=0*/)
  {
   wchar_t str[64]={};
   bool    res    =false;
//--- check
   if(!buffer || buffer_size==0 || !tag_name)
      return(res);
//--- get tag data
   if(res=GetTagValueStr(buffer,buffer_size,tag_name,str,_countof(str),tag_num))
      tag_value=_wtof(str);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Write tag into buffer with formatting                            |
//+------------------------------------------------------------------+
int32_t CExchangeMsg::SetTagFormat(LPCSTR tag_name,char *buffer,int buffer_size,LPCSTR fmt,...)
  {
   va_list arg_ptr;
   int32_t     len=0,len_value;
//--- check
   if(!tag_name || !buffer || buffer_size==0 || !fmt)
      return(-1);
//--- add tag
   if((len=_snprintf_s(buffer,buffer_size,_TRUNCATE,"%s",tag_name))<1)
      return(-1);
//--- check available size in buffer
   if(buffer_size<=len)
      return(-1);
//--- consider written data
   buffer     +=len;
   buffer_size-=len;
//--- arguments list
   va_start(arg_ptr,fmt);
//--- add tag value
   if((len_value=_vsnprintf_s(buffer,buffer_size,_TRUNCATE,fmt,arg_ptr))>0)
      len+=len_value;
   else
      len=-1;
//--- end of arguments list
   va_end(arg_ptr);
//--- return result
   return(len);
  }
//+------------------------------------------------------------------+
