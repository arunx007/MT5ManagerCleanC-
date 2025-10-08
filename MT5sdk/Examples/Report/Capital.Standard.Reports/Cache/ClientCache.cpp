//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ClientCache.h"
#include "..\Tools\ReportColumn.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| request dataset fields descriptions                              |
//+------------------------------------------------------------------+
DatasetField CClientCache::s_fields[]=
  {
   //--- id                                           , select , offset                                 ,size
     { IMTDatasetField::FIELD_CLIENT_ID                 , true   , offsetof(ClientRecord,client_id)     },
   { IMTDatasetField::FIELD_CLIENT_PERSON_NAME        , true   , offsetof(ClientRecord,first_name)    , MtFieldSize(ClientRecord,first_name)    },
   { IMTDatasetField::FIELD_CLIENT_PERSON_MIDDLE_NAME , true   , offsetof(ClientRecord,middle_name)   , MtFieldSize(ClientRecord,middle_name)   },
   { IMTDatasetField::FIELD_CLIENT_PERSON_LAST_NAME   , true   , offsetof(ClientRecord,second_name)   , MtFieldSize(ClientRecord,second_name)   },
   { IMTDatasetField::FIELD_CLIENT_ADDRESS_COUNTRY    , true   , offsetof(ClientRecord,country)       , MtFieldSize(ClientRecord,country)       },
   { IMTDatasetField::FIELD_CLIENT_LEAD_SOURCE        , true   , offsetof(ClientRecord,lead_source)   , MtFieldSize(ClientRecord,lead_source)   },
   { IMTDatasetField::FIELD_CLIENT_LEAD_CAMPAIGN      , true   , offsetof(ClientRecord,lead_campaign) , MtFieldSize(ClientRecord,lead_campaign) },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientCache::CClientCache(CReportCacheContext &context) :
   CReportCacheSelect(context)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientCache::~CClientCache(void)
  {
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CClientCache* CClientCache::Create(IMTReportAPI &api,MTAPIRES &res)
  {
//--- create cache context
   CReportCacheContext context(api,L"Client_Cache",s_cache_version);
   if((res=context.Status())!=MT_RET_OK)
      return(nullptr);
//--- create client cache object
   CClientCache *clients=new(std::nothrow) CClientCache(context);
   if(!clients)
      res=MT_RET_ERR_MEM;
//--- return new client cache object
   return(clients);
  }
//+------------------------------------------------------------------+
//| write clients to cache                                           |
//+------------------------------------------------------------------+
MTAPIRES CClientCache::WriteData(const IMTDataset &clients)
  {
//--- deals total
   const uint32_t total=clients.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- save current time
   const CReportTimer timer;
//--- iterate clients
   ClientRecord client={0};
   MTAPIRES res=MT_RET_OK;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read client
      if((res=clients.RowRead(pos,&client,sizeof(client)))!=MT_RET_OK)
         break;
      //--- write client cache
      if((res=WriteClient(client))!=MT_RET_OK)
         break;
     }
//--- check resul
   if(res!=MT_RET_OK)
     {
      m_api.LoggerOut(ErrorLogType(res),L"Client caching failed, error: %s",SMTFormat::FormatError(res));
      return(res);
     }
//--- write statistics to log
   return(m_api.LoggerOut(MTLogOK,L"Cached %u clients in %I64u ms",total,timer.Elapsed()));
  }
//+------------------------------------------------------------------+
//| Request clients from base                                        |
//+------------------------------------------------------------------+
MTAPIRES CClientCache::SelectData(IMTDataset &clients,const IMTReportCacheKeySet &keys)
  {
//--- create request
   IMTDatasetRequest *request=m_api.DatasetRequestCreate();
   if(!request)
      return(MT_RET_ERR_MEM);
//--- fill request
   MTAPIRES res=ClientRequest(*request,keys);
   if(res!=MT_RET_OK)
     {
      request->Release();
      return(res);
     }
//--- save current time
   const CReportTimer timer;
//--- request from base
   if((res=m_api.ClientSelect(request,&clients))!=MT_RET_OK)
      m_api.LoggerOut(MTLogAtt,L"Client selection failed, error: %s",SMTFormat::FormatError(res));
   else
      res=m_api.LoggerOut(MTLogOK,L"Selected %u clients in %I64u ms",clients.RowTotal(),timer.Elapsed());
//--- release request
   request->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill clients request                                             |
//+------------------------------------------------------------------+
MTAPIRES CClientCache::ClientRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &ids)
  {
//--- fill request
   MTAPIRES res=MT_RET_OK;
   CDatasetRequest composer(s_fields,_countof(s_fields),request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_CLIENT_ID:             res=composer.FieldAddWhereKeys(ids); break;
         default:                                           res=composer.FieldAdd(); break;
        }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Write client to cache                                            |
//+------------------------------------------------------------------+
MTAPIRES CClientCache::WriteClient(const ClientRecord &client)
  {
//--- fill client cache strings
   ClientCache cache;
   MTAPIRES res=MT_RET_OK;
   CMTStr256 str;
   if((res=m_cache.WriteDictionaryString(DICTIONARY_ID_NAME         ,FormatFullName(str,client)  ,cache.name))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(DICTIONARY_ID_COUNTRY      ,client.country              ,cache.country))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(DICTIONARY_ID_LEAD_SOURCE  ,client.lead_source          ,cache.lead_source))!=MT_RET_OK)
      return(res);
   if((res=m_cache.WriteDictionaryString(DICTIONARY_ID_LEAD_CAMPAIGN,client.lead_campaign        ,cache.lead_campaign))!=MT_RET_OK)
      return(res);
//--- write user cache value
   return(m_cache.WriteValue(client.client_id,&cache,sizeof(cache)));
  }
//+------------------------------------------------------------------+
//| Client full name formatting                                      |
//+------------------------------------------------------------------+
LPCWSTR CClientCache::FormatFullName(CMTStr& str,const wchar_t (&name)[32],const wchar_t (&last_name)[32],const wchar_t (&middle_name)[32])
  {
//--- есть все?
   if(name[0] && last_name[0] && middle_name[0])
     {
      str.Format(L"%s %s %s",last_name,name,middle_name);
      return(str.Str());
     }
//--- есть имя-фамилия?
   if(name[0] && last_name[0])
     {
      str.Format(L"%s %s",last_name,name);
      return(str.Str());
     }
//--- есть имя-отчество?
   if(name[0] && middle_name[0])
     {
      str.Format(L"%s %s",name,middle_name);
      return(str.Str());
     }
//--- есть фамилия-отчество?
   if(last_name[0] && middle_name[0])
     {
      str.Format(L"%s %s",last_name,middle_name);
      return(str.Str());
     }
//--- check name exists
   if(name[0])
      return(name);
//--- check last name exists
   if(last_name[0])
      return(last_name);
//--- return middle name
   return(middle_name);
  }
//+------------------------------------------------------------------+
