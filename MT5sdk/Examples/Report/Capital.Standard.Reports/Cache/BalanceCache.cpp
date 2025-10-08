//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "BalanceCache.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Deal request dataset fields descriptions                         |
//+------------------------------------------------------------------+
DatasetField CBalanceCache::s_fields[]=
   //--- id                                      , select , offset
  {{ IMTDatasetField::FIELD_DEAL_LOGIN           , true   , offsetof(DealRecord,login)           },
   { IMTDatasetField::FIELD_DEAL_TIME            , true   , offsetof(DealRecord,time)            },
   { IMTDatasetField::FIELD_DEAL_ACTION          },
   { IMTDatasetField::FIELD_DEAL_ENTRY           },
   { IMTDatasetField::FIELD_DEAL_PROFIT          , true   , offsetof(DealRecord,profit)          },
   { IMTDatasetField::FIELD_DEAL_DIGITS_CURRENCY , true   , offsetof(DealRecord,currency_digits) },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CBalanceCache::CBalanceCache(CReportCacheContext &context) :
   CReportCacheSelectTime(context)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CBalanceCache::~CBalanceCache(void)
  {
  }
//+------------------------------------------------------------------+
//| create class object                                              |
//+------------------------------------------------------------------+
CBalanceCache* CBalanceCache::Create(IMTReportAPI &api,MTAPIRES &res)
  {
//--- create cache context
   CReportCacheContext context(api,L"Balance_Cache",s_cache_version);
   if((res=context.Status())!=MT_RET_OK)
      return(nullptr);
//--- create balance cache object
   CBalanceCache *balances=new(std::nothrow) CBalanceCache(context);
   if(!balances)
      res=MT_RET_ERR_MEM;
//--- return new balance cache object
   return(balances);
  }
//+------------------------------------------------------------------+
//| write deals to balances                                          |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::WriteData(const IMTDataset &deals)
  {
//--- deals total
   const uint32_t total=deals.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- save current time
   const CReportTimer timer;
//--- iterate deals
   CUserBalanceWrite balance;
   DealRecord deal={0};
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read deal
      MTAPIRES res=deals.RowRead(pos,&deal,sizeof(deal));
      if(res!=MT_RET_OK)
         return(LogError(res,L"read deal %u of %u",pos,total));
      //--- check last login
      if(balance.Login()!=deal.login)
        {
         //--- write balance to cache
         res=balance.WriteBalance(m_cache);
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(LogError(res,L"write balance %u of %u for %I64u to cache",pos,total,deal.login));
         //--- get user balance
         res=ReadBalance(deal.login,balance);
         if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
            return(LogError(res,L"get %I64u account balance %u of %u",deal.login,pos,total));
        }
      //--- add balance with deal to cache
      res=balance.AddBalanceDeal(deal);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(LogError(res,L"add balance %u of %u for %I64u to cache",pos,total,deal.login));
     }
//--- write balance to cache
   const MTAPIRES res=balance.WriteBalance(m_cache);
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(LogError(res,L"write %u balances to cache",total));
//--- write statistics to log
   return(m_api.LoggerOut(MTLogOK,L"Cached %u accounts balances in %I64u ms",total,timer.Elapsed()));
  }
//+------------------------------------------------------------------+
//| Request deals from base                                          |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::SelectData(IMTDataset &deals,const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to)
  {
//--- checks
   if(from>to)
      return(MT_RET_ERR_PARAMS);
//--- create request
   IMTDatasetRequest *request=m_api.DatasetRequestCreate();
   if(!request)
      return(MT_RET_ERR_MEM);
//--- fill request
   MTAPIRES res=DealRequest(*request,keys,from,to);
   if(res!=MT_RET_OK)
     {
      request->Release();
      return(LogErrorTime(res,L"fill request for %u accounts",from,to,keys.Total()));
     }
//--- save current time
   const CReportTimer timer;
//--- request from base
   if((res=m_api.DealSelect(request,&deals))!=MT_RET_OK)
      LogErrorTime(res,L"Deals selection for %u accounts",from,to,keys.Total());
   else
      res=m_api.LoggerOut(MTLogOK,L"Selected %u deals for %u accounts in %I64u ms",deals.RowTotal(),keys.Total(),timer.Elapsed());
//--- release request
   request->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Fill deals request                                               |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::DealRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &logins,const int64_t from,const int64_t to)
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
         case IMTDatasetField::FIELD_DEAL_LOGIN:   res=composer.FieldAddWhereKeys(logins); break;
         case IMTDatasetField::FIELD_DEAL_TIME:    res=composer.FieldAddBetweenInt(from,to); break;
         case IMTDatasetField::FIELD_DEAL_ACTION:  res=composer.FieldAddWhereUInt(IMTDeal::DEAL_BALANCE); break;
         case IMTDatasetField::FIELD_DEAL_ENTRY:   res=composer.FieldAddWhereUInt(IMTDeal::ENTRY_IN); break;
         default:                                  res=composer.FieldAdd(); break;
        }
//--- return result
   return(LogError(res,L"fill deals request"));
  }
//+------------------------------------------------------------------+
//| read balance                                                     |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::ReadBalance(const uint64_t login,CUserBalance &balance) const
  {
//--- read from cache
   MTAPIRES res=m_cache.ReadValue(login,&m_value);
   if(res!=MT_RET_OK)
     {
      //--- clear value
      if(res==MT_RET_OK_NONE)
         balance.Clear();
      //--- return error code
      return(LogError(res,L"read from cache"));
     }
//--- set value
   return(LogError(balance.Value(m_value),L"set value"));
  }
//+------------------------------------------------------------------+
//| clear value                                                      |
//+------------------------------------------------------------------+
void CBalanceCache::CUserBalance::Clear(void)
  {
//--- clear data and size
   m_data=nullptr;
   m_size=0;
  }
//+------------------------------------------------------------------+
//| set value                                                        |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::CUserBalance::Value(IMTReportCacheValue &value)
  {
//--- clear data
   m_data=nullptr;
//--- get data size
   m_size=value.Size();
   if(!m_size)
      return(MT_RET_OK);
//--- get data
   m_data=(const char*)value.Data();
   if(!m_data)
      return(MT_RET_ERROR);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| User balance access                                              |
//+------------------------------------------------------------------+
const CBalanceCache::UserBalance* CBalanceCache::CUserBalance::User(void) const
  {
//--- check mempack size
   if(m_size<sizeof(UserBalance))
      return(nullptr);
//--- return user balance
   return((const UserBalance*)m_data);
  }
//+------------------------------------------------------------------+
//| Deal balance access                                              |
//+------------------------------------------------------------------+
const CBalanceCache::DealBalance* CBalanceCache::CUserBalance::Deal(const uint32_t pos) const
  {
//--- deal balance offset
   const uint32_t offset=sizeof(UserBalance)+pos*sizeof(DealBalance);
//--- check mempack size
   if(m_size<offset+sizeof(DealBalance) || !m_data)
      return(nullptr);
//--- check deal count
   const CBalanceCache::UserBalance *user=(const UserBalance*)m_data;
   if(pos>=user->aggregate_count)
      return(nullptr);
//--- return deal balance
   return((const DealBalance*)(m_data+offset));
  }
//+------------------------------------------------------------------+
//| Write balance to cache                                           |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::CUserBalanceWrite::WriteBalance(IMTReportCache &cache)
  {
//--- write buffer to cache
   MTAPIRES res=MT_RET_OK_NONE;
   if(m_login && m_buffer.Len()>=sizeof(UserBalance))
     {
      //--- calculate buffer size with deals count upper power of 2
      uint32_t size=m_buffer.Len()-sizeof(UserBalance);
      size/=sizeof(DealBalance);
      size=UpperPowerOfTwo(size)*sizeof(DealBalance)+sizeof(UserBalance);
      if(size>m_buffer.Len())
        {
         //--- ceil size
         if(size>s_buffer_max)
            size=s_buffer_max;
         //--- pad buffer with empty deals
         if(!m_buffer.Reallocate(size))
            return(MT_RET_ERR_MEM);
         memset(m_buffer.Buffer()+m_buffer.Len(),0,size-m_buffer.Len());
         m_buffer.Len(size);
        }
      //--- write buffer
      res=cache.WriteValue(m_login,m_buffer.Buffer(),m_buffer.Len());
     }
//--- clear buffer and login
   m_buffer.Clear();
   m_login=0;
   return(res);
  }
//+------------------------------------------------------------------+
//| Add balance with deal to cache writer                            |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::CUserBalanceWrite::AddBalanceDeal(const DealRecord &deal)
  {
//--- check epsilon or qnan value
   if(!(deal.profit<-DBL_EPSILON || deal.profit>DBL_EPSILON))
      return(MT_RET_OK_NONE);
//--- deal aggregate info
   DealInfo info;
   info.day=(unsigned)(deal.time/SECONDS_IN_DAY);
   info.deposit=deal.profit>0.0;
//--- aggregate balance deal
   return(AggreateDeal(info,deal));
  }
//+------------------------------------------------------------------+
//| aggregate balance deal                                           |
//+------------------------------------------------------------------+
MTAPIRES CBalanceCache::CUserBalanceWrite::AggreateDeal(const DealInfo &info,const DealRecord &deal)
  {
//--- check buffer empty
   UserBalance *user=nullptr;
   if(!m_buffer.Len())
     {
      //--- check data empty
      if(!m_data || !m_size)
        {
         //--- allocate buffer
         if(!AllocateDeal(sizeof(UserBalance)))
            return(MT_RET_ERR_MEM);
         //--- user balance
         user=(UserBalance *)m_buffer.Buffer();
         if(!user)
            return(MT_RET_ERROR);
         //--- initialize user balance
         user->currency_digits=deal.currency_digits;
         user->aggregate_count=0;
         //--- set length
         m_buffer.Len(sizeof(UserBalance));
        }
      else
        {
         //--- clear buffer
         m_buffer.Clear();
         //--- allocate buffer
         if(!AllocateDeal(m_size))
            return(MT_RET_ERR_MEM);
         //--- data size
         uint32_t size=sizeof(UserBalance)+((const UserBalance *)m_data)->aggregate_count*sizeof(DealBalance);
         if(size>m_size)
            size=m_size;
         //--- add data
         if(!m_buffer.Add(m_data,size))
            return(MT_RET_ERR_MEM);
         m_buffer.Len(m_size);
        }
      //--- store login
      m_login=deal.login;
     }
   else
     {
      //--- reallocate buffer for deal
      if(!AllocateDeal(m_buffer.Len()))
         return(MT_RET_ERR_MEM);
     }
//--- user balance
   if(!user)
     {
      user=(UserBalance *)m_buffer.Buffer();
      if(!user)
         return(MT_RET_ERROR);
     }
//--- check aggreagates limit
   if(user->aggregate_count>=s_aggregate_max)
      return(MT_RET_TRADE_LIMIT_REACHED);
//--- deal balance
   DealBalance *balances=(DealBalance *)(m_buffer.Buffer()+sizeof(UserBalance));
   if(!user->aggregate_count)
     {
      //--- init first aggregate by balance deal
      AggreateInit(*balances,info,deal);
     }
   else
     {
      //--- check last aggregate key
      DealBalance *balance=balances+user->aggregate_count-1;
      const uint32_t key=*((const uint32_t*)&info);
      if(balance->key<key)
        {
         //--- append new aggregate
         balance++;
         AggreateInit(*balance,info,deal);
        }
      else
         if(balance->key==key)
           {
            //--- add to last aggregate
            AggreateAdd(*balance,deal);
            return(MT_RET_OK);
           }
         else
           {
            //--- search aggregate
            balance=(DealBalance *)SMTSearch::Search(&info,balances,user->aggregate_count,sizeof(DealBalance),SearchDealBalance);
            if(balance)
              {
               AggreateAdd(*balance,deal);
               return(MT_RET_OK);
              }
            //--- init new aggregate by balance deal
            DealBalance balances_new;
            AggreateInit(balances_new,info,deal);
            //--- insert new aggregate by balance deal
            if(!SMTSearch::Insert(balances,&balances_new,user->aggregate_count,sizeof(DealBalance),SortDealBalance))
               return(MT_RET_ERROR);
           }
     }
//--- increment user deal count
   user->aggregate_count++;
//--- increase buffer size
   m_buffer.Len(sizeof(UserBalance)+user->aggregate_count*sizeof(DealBalance));
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| init aggregate by balance deal                                   |
//+------------------------------------------------------------------+
void CBalanceCache::CUserBalanceWrite::AggreateInit(DealBalance &balance,const DealInfo &info,const DealRecord &deal)
  {
   balance.info=info;
   balance.count=1;
   balance.value=deal.profit;
  }
//+------------------------------------------------------------------+
//| add balance deal to aggregate                                    |
//+------------------------------------------------------------------+
void CBalanceCache::CUserBalanceWrite::AggreateAdd(DealBalance &balance,const DealRecord &deal)
  {
   balance.count++;
   balance.value=SMTMath::MoneyAdd(balance.value,deal.profit,deal.currency_digits);
  }
//+------------------------------------------------------------------+
//| Reallocate cache writer buffer to fit at least one deal          |
//+------------------------------------------------------------------+
bool CBalanceCache::CUserBalanceWrite::AllocateDeal(uint32_t size)
  {
//--- allocate double amount of memory
   size+=sizeof(DealBalance);
   size*=2;
//--- check allocated memory
   if(size<=m_buffer.Max())
      return(true);
//--- allocate memory with double grow
   return(m_buffer.Reallocate(std::max(size,m_buffer.Max()*2)));
  }
//+------------------------------------------------------------------+
//| search deal balance static method                                |
//+------------------------------------------------------------------+
int32_t CBalanceCache::CUserBalanceWrite::SearchDealBalance(const void *left,const void *right)
  {
//--- types conversion
   const uint32_t         lft=*(const uint32_t*)left;
   const DealBalance *rgh=(const DealBalance*)right;
//--- compare keys
   if(lft<rgh->key)
      return(-1);
   if(lft>rgh->key)
      return(1);
   return(0);
  }
//+------------------------------------------------------------------+
//| sort deal balance static method                                  |
//+------------------------------------------------------------------+
int32_t CBalanceCache::CUserBalanceWrite::SortDealBalance(const void *left,const void *right)
  {
//--- types conversion
   const DealBalance *lft=(const DealBalance*)left;
   const DealBalance *rgh=(const DealBalance*)right;
//--- compare keys
   if(lft->key<rgh->key)
      return(-1);
   if(lft->key>rgh->key)
      return(1);
   return(0);
  }
//+------------------------------------------------------------------+
//| Calculate upper power of 2                                       |
//+------------------------------------------------------------------+
uint32_t CBalanceCache::CUserBalanceWrite::UpperPowerOfTwo(uint32_t v)
  {
   v--;
   v|=v>>1;
   v|=v>>2;
   v|=v>>4;
   v|=v>>8;
   v|=v>>16;
   v++;
   return(v);
  }
//+------------------------------------------------------------------+
