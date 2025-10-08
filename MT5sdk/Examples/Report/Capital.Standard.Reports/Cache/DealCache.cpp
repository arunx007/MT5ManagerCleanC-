//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealCache.h"
#include "..\Tools\ReportInterval.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealCache::CDealCacheContext::CDealCacheContext(IMTReportAPI &api,const CMTStr &name) :
   CReportCacheContext(api,name.Str(),s_cache_version,0),m_keys(api.KeySetCreate())
  {
//--- check status
   if(m_res!=MT_RET_OK)
      return;
//--- check pointers
   if(!m_keys)
      m_res=MT_RET_ERR_MEM;
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealCache::CDealCacheContext::~CDealCacheContext(void)
  {
//--- release interfaces
   if(m_keys)
     {
      m_keys->Release();
      m_keys=nullptr;
     }
  }
//+------------------------------------------------------------------+
//| Deal request dataset fields descriptions                         |
//+------------------------------------------------------------------+
DatasetField CDealCache::s_fields[]=
   //--- id                                      , select , offset
  {{ IMTDatasetField::FIELD_DEAL_LOGIN           , true   , offsetof(DealRecord,login)           },
   { IMTDatasetField::FIELD_DEAL_TIME            , true   , offsetof(DealRecord,time)            },
   { IMTDatasetField::FIELD_DEAL_ACTION          },
   { IMTDatasetField::FIELD_DEAL_ENTRY           },
   { IMTDatasetField::FIELD_DEAL_VOLUME_EXT      , true   , offsetof(DealRecord,volume)          },
   { IMTDatasetField::FIELD_DEAL_CONTRACT_SIZE   , true   , offsetof(DealRecord,contract_size)   },
   { IMTDatasetField::FIELD_DEAL_RATE_PROFIT     , true   , offsetof(DealRecord,rate_profit)     },
   { IMTDatasetField::FIELD_DEAL_PROFIT          , true   , offsetof(DealRecord,profit)          },
   { IMTDatasetField::FIELD_DEAL_REASON          , true   , offsetof(DealRecord,reason)          },
   { IMTDatasetField::FIELD_DEAL_DIGITS_CURRENCY , true   , offsetof(DealRecord,currency_digits) },
  };
//--- deal request actions and entries
uint64_t CDealCache::s_actions[]={ IMTDeal::DEAL_BUY, IMTDeal::DEAL_SELL };
uint64_t CDealCache::s_entries[]={ IMTDeal::ENTRY_OUT,IMTDeal::ENTRY_INOUT };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealCache::CDealCache(CDealCacheContext &context,CUserCache &users) :
   CReportCacheTime(context),m_users(users),m_logins(users.Logins()),m_keys(context.Detach(context.m_keys))
  {
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CDealCache* CDealCache::Create(IMTReportAPI &api,CUserCache &users,CReportParameter &params,CCurrencyConverterCache &currency,MTAPIRES &res)
  {
//--- check user cache empty
   if(!users.Logins().Total())
     {
      res=MT_RET_OK_NONE;
      return(NULL);
     }
//--- calculate logins hash
   const uint64_t login_hash=KeyHash(users.Logins());
//--- make cache name
   CMTStr128 name;
   name.Format(L"Deal_Hour_%.016llx_%x",login_hash,CReportInterval::WeekMonday(api.TimeGeneration()));
//--- create cache context
   CDealCacheContext context(api,name);
   if((res=context.Status())!=MT_RET_OK)
      return(NULL);
//--- initalize currency convertor
   if((res=currency.Initialize(&api,params,context.m_cache,DEAL_KEY_FIELD_CURRENCY))!=MT_RET_OK)
      return(NULL);
//--- create deal cache object
   CDealCache *balances=new(std::nothrow) CDealCache(context,users);
   if(!balances)
      res=MT_RET_ERR_MEM;
//--- return new deal cache object
   return(balances);
  }
//+------------------------------------------------------------------+
//| write deals to deals hours                                       |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::WriteData(const IMTDataset &deals,uint64_t &id_last)
  {
//--- deals total
   if(!deals.RowTotal())
      return(MT_RET_OK);
//--- begin read users
   MTAPIRES res=m_users.ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- write deals hours to cache
   res=WriteDeals(deals);
//--- end read users
   const MTAPIRES res_users=m_users.ReadEnd();
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
   if(res_users!=MT_RET_OK)
      return(res_users);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| write deals to deals hours                                       |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::WriteDeals(const IMTDataset &deals)
  {
//--- deals total
   const uint32_t total=deals.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- iterate deals
   CDealHourWrite hour(m_cache,m_value);
   DealRecord deal={0};
   uint64_t login=0;
   uint32_t user_currency=UINT_MAX,deal_currency=UINT_MAX;
   uint32_t user_country=UINT_MAX,deal_country=UINT_MAX;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read deal
      MTAPIRES res=deals.RowRead(pos,&deal,sizeof(deal));
      if(res!=MT_RET_OK)
         return(res);
      //--- check user change
      if(login!=deal.login)
        {
         //--- read user
         login=deal.login;
         const CUserCache::UserCache *user=nullptr;
         res=m_users.ReadUser(login,user);
         if(res!=MT_RET_OK)
            return(res);
         //--- check user
         if(!user)
            return(MT_RET_ERROR);
         //--- update currency
         if(user_currency!=user->currency)
           {
            user_currency=user->currency;
            //--- read currency from users dictionary
            LPCWSTR currency=nullptr;
            MTAPIRES res=m_users.ReadCurrency(*user,currency);
            if(res!=MT_RET_OK)
               return(res);
            //--- write currency to dictionary
            if((res=m_cache.WriteDictionaryString(DEAL_KEY_FIELD_CURRENCY,currency,deal_currency))!=MT_RET_OK)
               return(res);
           }
         //--- update country
         if(user_country!=user->country)
           {
            user_country=user->country;
            //--- read country from users dictionary
            LPCWSTR country=nullptr;
            MTAPIRES res=m_users.ReadCountry(user_country,country);
            if(res!=MT_RET_OK)
               return(res);
            //--- write country to dictionary
            if((res=m_cache.WriteDictionaryString(DEAL_KEY_FIELD_COUNTRY,country,deal_country))!=MT_RET_OK)
               return(res);
           }
        }
      //--- make deal key
      const uint64_t key=MakeKey(deal,deal_currency,deal_country);
      //--- check last key
      if(hour.Key()!=key)
        {
         //--- write deal hour to cache
         res=hour.Write();
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(res);
         //--- get user hour
         res=hour.Read(key);
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(res);
        }
      //--- add deal hour to cache
      res=hour.AddDeal(deal);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(res);
     }
//--- write hour to cache
   const MTAPIRES res=hour.Write();
   return(res==MT_RET_OK || res==MT_RET_OK_NONE ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Request deals from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::SelectData(IMTDataset &deals,const int64_t from,const int64_t to,const uint64_t id_from)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create request
   IMTDatasetRequest *request=m_api.DatasetRequestCreate();
   if(!request)
      return(MT_RET_ERR_MEM);
//--- fill request
   MTAPIRES res=DealRequest(*request,from,to);
   if(res!=MT_RET_OK)
     {
      request->Release();
      return(res);
     }
//--- request from base
   res=m_api.DealSelect(request,&deals);
//--- release request
   request->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill deals request                                               |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::DealRequest(IMTDatasetRequest &request,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- fill request
   MTAPIRES res=MT_RET_OK;
   CDatasetRequest composer(s_fields,_countof(s_fields),request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_DEAL_LOGIN:   res=composer.FieldAddWhereKeys(m_logins); break;
         case IMTDatasetField::FIELD_DEAL_TIME:    res=composer.FieldAddBetweenInt(from,to); break;
         case IMTDatasetField::FIELD_DEAL_ACTION:  res=composer.FieldAddWhereUIntArray(s_actions,_countof(s_actions)); break;
         case IMTDatasetField::FIELD_DEAL_ENTRY:   res=composer.FieldAddWhereUIntArray(s_entries,_countof(s_entries)); break;
         default:                                  res=composer.FieldAdd(); break;
        }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Make deal key                                                    |
//+------------------------------------------------------------------+
uint64_t CDealCache::MakeKey(const DealRecord &deal,const uint32_t currency,const uint32_t country)
  {
   const DealKey key{ (unsigned)(deal.time/SECONDS_IN_HOUR),currency,country,deal.reason,0 };
   return(*(const uint64_t*)(&key));
  }
//+------------------------------------------------------------------+
//| Calculate keys set hash                                          |
//+------------------------------------------------------------------+
uint64_t CDealCache::KeyHash(const IMTReportCacheKeySet &keys)
  {
//--- fastcache64 used
   const uint64_t m=0x880355f21e6d1965ULL;
   uint64_t hash=0;
//--- iterate all keys
   for(const uint64_t *key=keys.Next(NULL);key;key=keys.Next(key))
     {
      hash^=HashMix(*key);
      hash*=m;
     }
   return(HashMix(hash));
  }
//+------------------------------------------------------------------+
//| Compression function for Merkle-Damgard construction             |
//+------------------------------------------------------------------+
uint64_t CDealCache::HashMix(uint64_t h)
  {
   h^=h>>23;
   h*=0x2127599bf4325c37ULL;
   h^=h>>47;
   return(h);
  }
//+------------------------------------------------------------------+
//| read deal hour from cache                                        |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::CDealHourWrite::Read(const uint64_t key)
  {
//--- store key
   m_key=key;
//--- read from cache
   MTAPIRES res=m_cache.ReadValue(m_key,&m_value);
   if(res!=MT_RET_OK)
     {
      //--- clear value
      if(res==MT_RET_OK_NONE)
         memset(&m_hour,0,sizeof(m_hour));
      //--- return error code
      return(res);
     }
//--- get value size
   const uint32_t size=m_value.Size();
//--- check value size
   if(size!=sizeof(m_hour))
      return(MT_RET_ERROR);
//--- retrive data
   const void *data=m_value.Data();
   if(!data)
      return(MT_RET_ERROR);
//--- copy data
   memcpy(&m_hour,data,sizeof(m_hour));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write deal hour to cache                                         |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::CDealHourWrite::Write(void)
  {
//--- check key
   if(!m_key)
      return(MT_RET_OK_NONE);
//--- write buffer to cache
   const MTAPIRES res=m_cache.WriteValue(m_key,&m_hour,sizeof(m_hour));
//--- clear key
   m_key=0;
   return(res);
  }
//+------------------------------------------------------------------+
//| Add deal to cache writer                                         |
//+------------------------------------------------------------------+
MTAPIRES CDealCache::CDealHourWrite::AddDeal(const DealRecord &deal)
  {
//--- check deal
   if(!deal.volume || fabs(deal.contract_size)<DBL_EPSILON || fabs(deal.rate_profit)<DBL_EPSILON || _isnan(deal.rate_profit) || _isnan(deal.profit))
      return(MT_RET_OK_NONE);
//--- calculate volume
   const double volume=fabs((deal.volume/100000000.0)*deal.contract_size*deal.rate_profit);
//--- add deal to hour
   if(deal.profit<-DBL_EPSILON)
     {
      m_hour.loss_count++;
      m_hour.loss_volume=SMTMath::MoneyAdd(m_hour.loss_volume,volume,deal.currency_digits);
      m_hour.loss=SMTMath::MoneyAdd(m_hour.loss,deal.profit,deal.currency_digits);
     }
   else
     {
      m_hour.profit_count++;
      m_hour.profit_volume=SMTMath::MoneyAdd(m_hour.profit_volume,volume,deal.currency_digits);
      m_hour.profit=SMTMath::MoneyAdd(m_hour.profit,deal.profit,deal.currency_digits);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
