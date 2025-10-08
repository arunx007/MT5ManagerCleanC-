//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "CurrencyConverterCache.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CCurrencyConverterCache::CCurrencyConverterCache(void) :
   m_cache(nullptr),m_dictionary_id(0),m_currency_pos(UINT_MAX)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CCurrencyConverterCache::~CCurrencyConverterCache(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CCurrencyConverterCache::Initialize(IMTReportAPI *api,CReportParameter &params,const IMTReportCache *cache,const uint32_t dictionary_id,LPCWSTR param_name,LPCWSTR currency_default)
  {
//--- clear
   Clear();
//--- checks
   if(!api || !cache || !param_name || !*param_name || !currency_default || !*currency_default)
      return(MT_RET_ERR_PARAMS);
//--- base call
   MTAPIRES res=CCurrencyConverter::Initialize(api,params,param_name,currency_default);
   if(res!=MT_RET_OK)
      return(res);
//--- store cache and dictionary id
   m_cache=cache;
   m_dictionary_id=dictionary_id;
   m_currency_pos=UINT_MAX;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CCurrencyConverterCache::Clear(void)
  {
//--- base call
   CCurrencyConverter::Clear();
//--- clear parameters
   m_cache=nullptr;
   m_dictionary_id=0;
   m_currency_pos=UINT_MAX;
   m_rates.Clear();
  }
//+------------------------------------------------------------------+
//| Check currency for conversion needed                             |
//+------------------------------------------------------------------+
bool CCurrencyConverterCache::NeedConversion(const uint32_t currency)
  {
//--- check currency dictionary position
   if(m_currency_pos==UINT_MAX && m_cache)
     {
      //--- get currency dictionary position
      if(m_cache->ReadDictionaryPos(m_dictionary_id,m_currency.Str(),m_currency_pos)!=MT_RET_OK)
         m_currency_pos=UINT_MAX;
     }
//--- compare currency
   return(m_currency_pos!=currency);
  }
//+------------------------------------------------------------------+
//| Currency conversion rate                                         |
//+------------------------------------------------------------------+
MTAPIRES CCurrencyConverterCache::CurrencyRate(double &rate,const uint32_t currency)
  {
//--- check cache pointer
   if(!m_cache)
      return(MT_RET_ERR_PARAMS);
//--- check conversion needed
   if(!NeedConversion(currency))
     {
      rate=1.0;
      return(MT_RET_OK);
     }
//--- allocate rate cache
   const uint32_t total=m_rates.Total();
   if(currency>=total)
     {
      //--- reserve memory
      const uint32_t size=currency+1;
      if(size>m_rates.Max())
         if(!m_rates.Reserve(std::max(size,m_rates.Max()*2)))
            return(MT_RET_ERR_MEM);
      //--- resize array
      if(!m_rates.Resize(size))
         return(MT_RET_ERR_MEM);
      //--- initialize allocated range
      for(uint32_t i=total;i<size;i++)
         m_rates[i]=NAN;
     }
//--- get rate
   rate=m_rates[currency];
//--- check rate
   if(_isnan(rate))
     {
      //--- get currency name from dictionary
      LPCWSTR currency_name=nullptr;
      MTAPIRES res=m_cache->ReadDictionaryString(m_dictionary_id,currency,currency_name);
      if(res!=MT_RET_OK)
         return(res);
      //--- check currency name
      if(!currency_name)
         return(MT_RET_ERROR);
      //--- check currency name not empty      
      if(*currency_name)
        {
         //--- currency conversion rate
         if((res=CCurrencyConverter::CurrencyRate(rate,currency_name))!=MT_RET_OK)
            return(res);
        }
      else
         rate=0.0;
      //--- store conversion rate
      m_rates[currency]=rate;
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
