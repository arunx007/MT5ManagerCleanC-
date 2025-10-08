//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "UserFilter.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserFilter::CUserFilter(IMTReportAPI &api,const CUserCache &cache,const int64_t from,const int64_t to) :
   m_from(from),m_to(to),m_keys(nullptr),m_api(api),m_cache(cache),m_param_lead_source(nullptr),m_param_lead_campaign(nullptr),m_lead_source(-1),m_lead_campaign(-1)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUserFilter::~CUserFilter(void)
  {
//--- release interfaces
   if(m_param_lead_source)
     {
      m_param_lead_source->Release();
      m_param_lead_source=nullptr;
     }
   if(m_param_lead_campaign)
     {
      m_param_lead_campaign->Release();
      m_param_lead_campaign=nullptr;
     }
   if(m_keys)
     {
      m_keys->Release();
      m_keys=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CUserFilter::Initialize(void)
  {
//--- check keys interface created
   if(m_keys)
      return(MT_RET_OK);
//--- create interfaces
   if(!m_param_lead_source)
      m_param_lead_source=m_api.ParamCreate();
   if(!m_param_lead_campaign)
      m_param_lead_campaign=m_api.ParamCreate();
   m_keys=m_api.KeySetCreate();
//--- check interfaces
   if(!m_param_lead_source || !m_param_lead_campaign || !m_keys)
      return(MT_RET_ERR_MEM);
//--- get lead source parameter
   uint32_t pos=0;
   if(LPCWSTR lead_source=ParamGetString(MTAPI_PARAM_LEAD_SOURCE,m_param_lead_source,L"*"))
      if(m_cache.ReadLeadSourcePos(lead_source,pos)==MT_RET_OK)
         m_lead_source=pos;
//--- get lead campaign parameter
   if(LPCWSTR lead_campaign=ParamGetString(MTAPI_PARAM_LEAD_CAMPAIGN,m_param_lead_campaign,L"*"))
      if(m_cache.ReadLeadCampaignPos(lead_campaign,pos)==MT_RET_OK)
         m_lead_campaign=pos;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| get report parameter string value                                |
//+------------------------------------------------------------------+
LPCWSTR CUserFilter::ParamGetString(LPCWSTR name,IMTConParam *param,LPCWSTR def) const
  {
//--- check arguments
   if(!name || !param)
      return(nullptr);
//--- get lead source parameter
   if(m_api.ParamGet(name,param)!=MT_RET_OK)
      return(nullptr);
//--- get paramter string value
   LPCWSTR value=param->ValueString();
   if(!value)
      return(nullptr);
//--- check default value
   if(def && !CMTStr::Compare(value,def))
      return(nullptr);
//--- return value
   return(value);
  }
//+------------------------------------------------------------------+
//| swap logins key set                                              |
//+------------------------------------------------------------------+
MTAPIRES CUserFilter::LoginsSwap(IMTReportCacheKeySet &logins)
  {
//--- check keyset
   if(!m_keys)
      return(MT_RET_OK_NONE);
//--- swap logins
   return(logins.Swap(m_keys));
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CUserFilter::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- initialization
   const MTAPIRES res=Initialize();
   if(res!=MT_RET_OK)
      return(res);
//--- match user with filter
   if(!UserMatch(user))
      return(MT_RET_OK);
//--- check key set
   if(!m_keys)
      return(MT_RET_ERROR);
//--- insert login to set
   return(m_keys->Insert(login));
  }
//+------------------------------------------------------------------+
//| match user with filter                                           |
//+------------------------------------------------------------------+
bool CUserFilter::UserMatch(const CUserCache::UserCache &user) const
  {
//--- registration time filter
   if(user.registration<m_from || user.registration>m_to)
      return(false);
//--- lead source filter
   if(m_lead_source>=0 && m_lead_source!=user.lead_source)
      return(false);
//--- lead campaign filter
   if(m_lead_campaign>=0 && m_lead_campaign!=user.lead_campaign)
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserFilterWithClients::CUserFilterWithClients(IMTReportAPI &api,const CUserCache &cache,const int64_t from,const int64_t to) :
    CUserFilter(api,cache,from,to),m_clients(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CUserFilterWithClients::~CUserFilterWithClients(void)
  {
//--- release interfaces
   if(m_clients)
     {
      m_clients->Release();
      m_clients=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CUserFilterWithClients::Initialize(void)
  {
//--- check keys interface created
   if(m_clients)
      return(MT_RET_OK);
//--- base call
   MTAPIRES res=CUserFilter::Initialize();
   if(res!=MT_RET_OK)
      return(res);
//--- create interfaces
   m_clients=m_api.KeySetCreate();
//--- check interfaces
   if(!m_keys)
      return(MT_RET_ERR_MEM);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CUserFilterWithClients::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- initialization
   MTAPIRES res=Initialize();
   if(res!=MT_RET_OK)
      return(res);
//--- check client id
   if(!user.client_id)
      return(MT_RET_OK);
//--- registration time filter
   if(user.registration<m_from || user.registration>m_to)
      return(MT_RET_OK);
//--- check key set
   if(!m_keys)
      return(MT_RET_ERROR);
//--- insert login to set
   if((res=m_keys->Insert(login))!=MT_RET_OK)
      return(res);
//--- check key set
   if(!m_clients)
      return(MT_RET_ERROR);
//--- insert client id to set
   return(m_clients->Insert(user.client_id));
  }
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CUserFilterByClients::CUserFilterByClients(const int64_t from,const IMTReportCacheKeySet &clients,IMTReportCacheKeySet &logins) :
   m_from(from),m_clients(clients),m_logins(logins)
  {
  }
//+------------------------------------------------------------------+
//| user read handler                                                |
//+------------------------------------------------------------------+
MTAPIRES CUserFilterByClients::UserRead(const uint64_t login,const CUserCache::UserCache &user)
  {
//--- check client id
   if(!user.client_id)
      return(MT_RET_OK);
//--- registration time filter
   if(user.registration<m_from)
      return(MT_RET_OK);
//--- search client id
   if(!m_clients.Search(user.client_id))
      return(MT_RET_OK);
//--- insert user login to set
   return(m_logins.Insert(login));
  }
//+------------------------------------------------------------------+
