//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheTime.h"
#include "UserCache.h"
#include "..\Tools\DatasetField.h"
//+------------------------------------------------------------------+
//| Deal cache class                                                 |
//+------------------------------------------------------------------+
class CDealCache : public CReportCacheTime
  {
public:
   //--- Deal key structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct DealKey
     {
      unsigned          hour     : 24;
      unsigned          currency : 10;
      unsigned          country  : 12;
      unsigned          reason   :  6;
      unsigned          reserved : 12;
     };
   //--- Deal hour structure
   struct DealHour
     {
      uint32_t          profit_count;           // profit deals count
      uint32_t          loss_count;             // loss deals count
      double            profit_volume;          // profit volume (in currency)
      double            loss_volume;            // loss volume (in currency)
      double            profit;                 // profit
      double            loss;                   // loss
     };
   #pragma pack(pop)
   //--- Deal key structure fields ids
   enum EnDealKeyFields
     {
      DEAL_KEY_FIELD_CURRENCY=0,
      DEAL_KEY_FIELD_COUNTRY =1,
      DEAL_KEY_FIELD_REASON  =2,
     };

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=2;
   //--- Deal record
   #pragma pack(push,1)
   struct DealRecord
     {
      uint64_t          login;                  // login
      int64_t           time;                   // time
      uint64_t          volume;                 // volume
      double            contract_size;          // symbol contract size
      double            rate_profit;            // profit conversion rate
      double            profit;                 // profit
      uint32_t          reason;                 // reason
      uint32_t          currency_digits;        // group currency digits
     };
   #pragma pack(pop)
   //--- Deal hour write class
   class CDealHourWrite
     {
   private:
      IMTReportCache   &m_cache;                // cache interface
      IMTReportCacheValue &m_value;             // report cache value interface reference
      uint64_t          m_key;                  // deal key
      DealHour          m_hour;                 // deal hour

   public:
      CDealHourWrite(IMTReportCache &cache,IMTReportCacheValue &value) :
                        m_cache(cache),m_value(value),m_key(0),m_hour{0} {}
      //-- current deal hour key
      uint64_t          Key(void) const { return(m_key); }
      //--- read deal hour from cache
      MTAPIRES          Read(const uint64_t key);
      //--- write deal hour to cache
      MTAPIRES          Write(void);
      //--- add deal to cache writer
      MTAPIRES          AddDeal(const DealRecord &deal);
     };

private:
   //--- cache context class
   class CDealCacheContext : public CReportCacheContext
     {
   public:
      IMTReportCacheKeySet *m_keys;             // keys set

   public:
                        CDealCacheContext(IMTReportAPI &api,const CMTStr &name);
      virtual          ~CDealCacheContext(void);
     };

private:
   CUserCache       &m_users;                   // users cache
   const IMTReportCacheKeySet &m_logins;        // logins set
   IMTReportCacheKeySet &m_keys;                // keys set
   //--- static data
   static DatasetField s_fields[];              // deal request dataset fields descriptions
   static uint64_t   s_actions[];               // deal request actions
   static uint64_t   s_entries[];               // deal request entries

public:
   //--- create class object
   static CDealCache* Create(IMTReportAPI &api,CUserCache &users,CReportParameter &params,CCurrencyConverterCache &currency,MTAPIRES &res);
   //--- read currency string
   MTAPIRES          ReadCurrency(const DealKey &key,LPCWSTR &currency) const { return(ReadDictionaryString(DEAL_KEY_FIELD_CURRENCY,key.currency,currency)); }
   //--- get deal hour key
   static const DealKey& Key(const uint64_t &key) { return(*(const DealKey*)(&key)); }
   //--- get deal hour key field value
   static uint32_t   KeyField(const DealKey &key,const EnDealKeyFields field) { return(field==DEAL_KEY_FIELD_COUNTRY ? key.country : key.reason); }

private:
                     CDealCache(CDealCacheContext &context,CUserCache &users);
   virtual          ~CDealCache(void)=default;
   //--- request deals from base
   virtual MTAPIRES  SelectData(IMTDataset &deals,int64_t from,int64_t to,uint64_t id_from) override;
   //--- write deals to balances
   virtual MTAPIRES  WriteData(const IMTDataset &deals,uint64_t &id_last) override;
   //--- fill deals request
   MTAPIRES          DealRequest(IMTDatasetRequest &request,const int64_t from,const int64_t to);
   //--- write deals to deals hours
   MTAPIRES          WriteDeals(const IMTDataset &deals);
   //--- make deal key
   static uint64_t   MakeKey(const DealRecord &deal,const uint32_t currency,const uint32_t country);
   //--- calculate keys set hash
   static uint64_t   KeyHash(const IMTReportCacheKeySet &keys);
   //--- compression function for Merkle-Damgard construction
   static uint64_t   HashMix(uint64_t h);
   //--- logging
   virtual MTAPIRES  LogSelectError(const MTAPIRES res,const int64_t from,const int64_t to,const uint64_t id_from) override
     {
      return(LogErrorTime(res,L"Deals selection with creation time",from,to));
     }
   virtual MTAPIRES  LogWriteError(const MTAPIRES res,const int64_t from,const int64_t to) override
     {
      return(LogErrorTime(res,L"Deals aggregation with creation time",from,to));
     }
   virtual MTAPIRES  LogWriteOk(const int64_t from,const int64_t to,const uint64_t total,const uint64_t time_select,const uint64_t time_write) override
     {
      return(LogOkTime(L"Selected %I64u deals in %I64u ms, aggregated in %I64u ms, creation time",from,to,total,time_select,time_write));
     }
  };
//+------------------------------------------------------------------+
