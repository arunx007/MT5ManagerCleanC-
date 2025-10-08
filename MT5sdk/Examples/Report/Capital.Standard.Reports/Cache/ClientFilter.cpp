//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ClientFilter.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CClientFilter::CClientFilter(void) :
   m_initialized(false),m_lead_source(-1),m_lead_campaign(-1)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CClientFilter::~CClientFilter(void)
  {
  }
//+------------------------------------------------------------------+
//| initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CClientFilter::Initialize(IMTReportAPI &api,const CClientCache &cache)
  {
//--- check initialized
   if(m_initialized)
      return(MT_RET_OK);
//--- create parameter
   IMTConParam *param=api.ParamCreate();
   if(!param)
      return(MT_RET_ERR_MEM);
//--- get lead source parameter
   uint32_t pos=0;
   if(LPCWSTR lead_source=ParamGetString(api,MTAPI_PARAM_LEAD_SOURCE,param,L"*"))
      if(cache.ReadLeadSourcePos(lead_source,pos)==MT_RET_OK)
         m_lead_source=pos;
//--- get lead campaign parameter
   if(LPCWSTR lead_campaign=ParamGetString(api,MTAPI_PARAM_LEAD_CAMPAIGN,param,L"*"))
      if(cache.ReadLeadCampaignPos(lead_campaign,pos)==MT_RET_OK)
         m_lead_campaign=pos;
//--- release parameter and set initialization flag
   param->Release();
   m_initialized=true;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| clear                                                            |
//+------------------------------------------------------------------+
void CClientFilter::Clear(void)
  {
//--- reset fields
   m_initialized=false;
   m_lead_source=-1;
   m_lead_campaign=-1;
  }
//+------------------------------------------------------------------+
//| get report parameter string value                                |
//+------------------------------------------------------------------+
LPCWSTR CClientFilter::ParamGetString(IMTReportAPI &api,LPCWSTR name,IMTConParam *param,LPCWSTR def)
  {
//--- check arguments
   if(!name || !param)
      return(nullptr);
//--- get lead source parameter
   if(api.ParamGet(name,param)!=MT_RET_OK)
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
//| match client with filter                                         |
//+------------------------------------------------------------------+
bool CClientFilter::ClientMatch(const CClientCache::ClientCache &client) const
  {
//--- lead source filter
   if(m_lead_source>=0 && m_lead_source!=client.lead_source)
      return(false);
//--- lead campaign filter
   if(m_lead_campaign>=0 && m_lead_campaign!=client.lead_campaign)
      return(false);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
