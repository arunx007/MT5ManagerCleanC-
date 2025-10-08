//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealWeekCache.h"
#include "..\Tools\ReportInterval.h"
//+------------------------------------------------------------------+
//| Deal request dataset fields descriptions                         |
//+------------------------------------------------------------------+
const DatasetField CDealWeekCache::s_fields[]=
   //--- id                                      , select , offset
  {{ IMTDatasetField::FIELD_DEAL_LOGIN           , true   , offsetof(DealRecord,login)           },
   { IMTDatasetField::FIELD_DEAL_TIME            , true   , offsetof(DealRecord,time)            },
   { IMTDatasetField::FIELD_DEAL_ACTION          , true   , offsetof(DealRecord,action)          },
   { IMTDatasetField::FIELD_DEAL_ENTRY           , true   , offsetof(DealRecord,entry)           },
   { IMTDatasetField::FIELD_DEAL_VOLUME_EXT      , true   , offsetof(DealRecord,volume)          },
   { IMTDatasetField::FIELD_DEAL_CONTRACT_SIZE   , true   , offsetof(DealRecord,contract_size)   },
   { IMTDatasetField::FIELD_DEAL_RATE_PROFIT     , true   , offsetof(DealRecord,rate_profit)     },
   { IMTDatasetField::FIELD_DEAL_PROFIT          , true   , offsetof(DealRecord,profit)          },
   { IMTDatasetField::FIELD_DEAL_DIGITS_CURRENCY , true   , offsetof(DealRecord,currency_digits) },
  };
//--- deal request actions and entries
const uint64_t CDealWeekCache::s_actions[]={ IMTDeal::DEAL_BUY, IMTDeal::DEAL_SELL, IMTDeal::DEAL_BALANCE };
const uint64_t CDealWeekCache::s_entries[]={ IMTDeal::ENTRY_IN, IMTDeal::ENTRY_OUT,IMTDeal::ENTRY_INOUT };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealWeekCache::CDealWeekCache(CReportCacheContext &context,CUserGroupCache &users) :
   CReportCacheTime(context),m_users(users)
  {
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CDealWeekCache* CDealWeekCache::Create(IMTReportAPI &api,CUserGroupCache &users,CReportParameter &params,CCurrencyConverterCache &currency,MTAPIRES &res)
  {
//--- make cache name
   CMTStr128 name;
   name.Format(L"Deal_Week_%x",CReportInterval::WeekMonday(api.TimeGeneration()));
//--- create cache context
   CReportCacheContext context(api,name.Str(),s_cache_version,0);
   if((res=context.Status())!=MT_RET_OK)
      return(NULL);
//--- initalize currency convertor
   if((res=currency.Initialize(&api,params,context.m_cache,DEAL_KEY_FIELD_CURRENCY))!=MT_RET_OK)
      return(NULL);
//--- create deal cache object
   CDealWeekCache *cache=new(std::nothrow) CDealWeekCache(context,users);
   if(!cache)
      res=MT_RET_ERR_MEM;
//--- return new deal cache object
   return(cache);
  }
//+------------------------------------------------------------------+
//| update deal week aggregates                                      |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::Update(int64_t from,int64_t to)
  {
//--- checks
   if(from>to || !to)
      return(MT_RET_ERR_PARAMS);
//--- get report generation time
   int64_t now=m_api.TimeGeneration();
   if(!now)
      return(MT_RET_ERR_PARAMS);
//--- report time with week granularity
   from=CReportInterval::WeekBeginMonday(from);
   to  =CReportInterval::WeekBeginMonday(to)+SECONDS_IN_WEEK-1;
   now =CReportInterval::WeekBeginMonday(now)-1;
//--- limit to time to report generation time
   if(to>now)
      to=now;
//--- check time range
   if(from>to)
      return(MT_RET_OK_NONE);
//--- update cache
   return(CReportCacheTime::Update(from,to));
  }
//+------------------------------------------------------------------+
//| write cache                                                      |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::Write(const int64_t from,const int64_t to)
  {
//--- update users groups
   MTAPIRES res=m_users.Update(0,to);
   if(res!=MT_RET_OK)
      return(res);
//--- write cache
   return(CReportCacheTime::Write(from,to));
  }
//+------------------------------------------------------------------+
//| write cache range                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::WriteRange(int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api.DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- begin read users
   MTAPIRES res=m_users.ReadBegin();
   if(res!=MT_RET_OK)
      return(res);
//--- update cache month by month
   for(int64_t end=MonthEnd(from,to);from<to && res==MT_RET_OK;from=end+1,end=MonthEnd(from,to))
     {
      //--- write cache month range
      res=WriteRangeLimit(*data,from,end);
     }
//--- end read users
   const MTAPIRES res_users=m_users.ReadEnd();
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(res);
   return(res_users);
  }
//+------------------------------------------------------------------+
//| write deals to deals hours                                       |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::WriteData(const IMTDataset &deals,uint64_t &id_last)
  {
//--- deals total
   const uint32_t total=deals.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- iterate deals
   CDealWeekWrite week(m_cache,m_value);
   DealRecord deal={0};
   uint64_t login=0;
   const CUserGroupCache::UserGroup *user=nullptr;
   uint32_t user_currency=UINT_MAX,deal_currency=UINT_MAX;
   uint32_t user_group=UINT_MAX,deal_group=UINT_MAX;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read deal
      MTAPIRES res=deals.RowRead(pos,&deal,sizeof(deal));
      if(res!=MT_RET_OK)
         return(res);
      //--- store last deal login
      if(id_last<deal.login)
         id_last=deal.login;
      //--- check user change
      if(login!=deal.login)
        {
         //--- read user
         login=deal.login;
         user=nullptr;
         res=m_users.ReadUser(login,user);
         if(res==MT_RET_OK_NONE)
            continue;
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
         //--- update group
         if(user_group!=user->group)
           {
            user_group=user->group;
            //--- read group from users dictionary
            LPCWSTR group=nullptr;
            MTAPIRES res=m_users.ReadGroup(user_group,group);
            if(res!=MT_RET_OK)
               return(res);
            //--- write group to dictionary
            if((res=m_cache.WriteDictionaryString(DEAL_KEY_FIELD_GROUP,group,deal_group))!=MT_RET_OK)
               return(res);
           }
        }
      else
         if(!user)
            continue;
      //--- make deal key
      const uint64_t key=MakeKey(deal,deal_currency,deal_group);
      //--- check last key
      if(week.Key()!=key)
        {
         //--- write deal week to cache
         res=week.Write();
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(res);
         //--- get user week
         res=week.Read(key);
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(res);
        }
      //--- add deal week to cache
      res=week.AddDeal(deal);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(res);
     }
//--- write week to cache
   const MTAPIRES res=week.Write();
   return(res==MT_RET_OK || res==MT_RET_OK_NONE ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Request deals from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::SelectData(IMTDataset &deals,const int64_t from,const int64_t to,const uint64_t id_from)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create request
   IMTDatasetRequest *request=m_api.DatasetRequestCreate();
   if(!request)
      return(MT_RET_ERR_MEM);
//--- fill request
   MTAPIRES res=DealRequest(*request,from,to,id_from);
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
MTAPIRES CDealWeekCache::DealRequest(IMTDatasetRequest &request,const int64_t from,const int64_t to,const uint64_t id_from)
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
         case IMTDatasetField::FIELD_DEAL_LOGIN:   res=composer.FieldAddBetweenUInt(id_from,UINT64_MAX); break;
         case IMTDatasetField::FIELD_DEAL_TIME:    res=composer.FieldAddBetweenInt(from,to); break;
         case IMTDatasetField::FIELD_DEAL_ACTION:  res=composer.FieldAddWhereUIntArray(s_actions,_countof(s_actions)); break;
         case IMTDatasetField::FIELD_DEAL_ENTRY:   res=composer.FieldAddWhereUIntArray(s_entries,_countof(s_entries)); break;
         default:                                  res=composer.FieldAdd(); break;
        }
//--- limit the number of rows of the resulting dataset
   request.RowLimit(s_request_limit);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Make deal key                                                    |
//+------------------------------------------------------------------+
uint64_t CDealWeekCache::MakeKey(const DealRecord &deal,const uint32_t currency,const uint32_t group)
  {
   const DealKey key{ CReportInterval::WeekMonday(deal.time),currency,group,0 };
   return(*(const uint64_t*)(&key));
  }
//+------------------------------------------------------------------+
//| end of month                                                     |
//+------------------------------------------------------------------+
int64_t CDealWeekCache::MonthEnd(int64_t ctm,const int64_t to)
  {
   ctm=SMTTime::MonthBegin(SMTTime::MonthBegin(ctm)+SECONDS_IN_MONTH+SECONDS_IN_WEEK)-1;
   return(std::min(ctm,to));
  }
//+------------------------------------------------------------------+
//| read deal week from cache                                        |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::CDealWeekWrite::Read(const uint64_t key)
  {
//--- store key
   m_key=key;
//--- read from cache
   MTAPIRES res=m_cache.ReadValue(m_key,&m_value);
   if(res!=MT_RET_OK)
     {
      //--- clear value
      if(res==MT_RET_OK_NONE)
         memset(&m_week,0,sizeof(m_week));
      //--- return error code
      return(res);
     }
//--- get value size
   const uint32_t size=m_value.Size();
//--- check value size
   if(size!=sizeof(m_week))
      return(MT_RET_ERROR);
//--- retrive data
   const void *data=m_value.Data();
   if(!data)
      return(MT_RET_ERROR);
//--- copy data
   memcpy(&m_week,data,sizeof(m_week));
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write deal week to cache                                         |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::CDealWeekWrite::Write(void)
  {
//--- check key
   if(!m_key)
      return(MT_RET_OK_NONE);
//--- write buffer to cache
   const MTAPIRES res=m_cache.WriteValue(m_key,&m_week,sizeof(m_week));
//--- clear key
   m_key=0;
   return(res);
  }
//+------------------------------------------------------------------+
//| Add deal to cache writer                                         |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::CDealWeekWrite::AddDeal(const DealRecord &deal)
  {
//--- balance deal
   if(deal.entry==IMTDeal::ENTRY_IN)
      return(AddBalanceDeal(deal));
//--- check deal
   if(!deal.volume || fabs(deal.contract_size)<DBL_EPSILON || fabs(deal.rate_profit)<DBL_EPSILON || _isnan(deal.rate_profit) || _isnan(deal.profit))
      return(MT_RET_OK_NONE);
//--- calculate volume
   const double volume=fabs((deal.volume/100000000.0)*deal.contract_size*deal.rate_profit);
//--- deal aggregate
   Deal &aggregate=deal.profit>=0.0 ? m_week.profit : m_week.loss;
//--- aggregate deal
   aggregate.count++;
   aggregate.volume=SMTMath::MoneyAdd(aggregate.volume,volume,deal.currency_digits);
   aggregate.value=SMTMath::MoneyAdd(aggregate.value,deal.profit,deal.currency_digits);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add balance deal to cache writer                                 |
//+------------------------------------------------------------------+
MTAPIRES CDealWeekCache::CDealWeekWrite::AddBalanceDeal(const DealRecord &deal)
  {
//--- balance deal
   if(deal.action!=IMTDeal::DEAL_BALANCE)
      return(MT_RET_OK_NONE);
//--- deal balance aggregate, skip epsilon or qnan profits
   Balance *balance=deal.profit>DBL_EPSILON ? &m_week.deposit : (deal.profit<-DBL_EPSILON ? &m_week.withdrawal : nullptr);
   if(!balance)
      return(MT_RET_OK_NONE);
//--- aggregate deal
   balance->count++;
   balance->value=SMTMath::MoneyAdd(balance->value,deal.profit,deal.currency_digits);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
