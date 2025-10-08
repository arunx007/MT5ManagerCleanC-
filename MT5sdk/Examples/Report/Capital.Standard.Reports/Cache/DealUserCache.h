//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Traders.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "DealBaseCache.h"
//+------------------------------------------------------------------+
//| Client deals statistics cache class                              |
//+------------------------------------------------------------------+
class CDealUserCache : public CDealBaseCache
  {
public:
   //--- Deals statistics structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   //--- Deal price difference aggregate structure
   struct DealPrice
     {
      uint32_t          count;                  // deal count
      double            price_diff;             // sum of deals prices differences in percents
     };
   //--- Deal aggregate structure
   struct Deal
     {
      DealPrice         ordered;                // deals prices differences ordered
      DealPrice         actual;                 // deals prices differences actual
     };
   //--- Deals statistics aggregate structure
   struct DealStat
     {
      Deal              loss;                   // loss deals
      Deal              profit;                 // profit deals
     };
   #pragma pack(pop)

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=1;
   //--- Deal record
   #pragma pack(push,1)
   struct DealRecord
     {
      uint64_t          login;                  // login
      uint32_t          action;                 // action
      uint32_t          entry;                  // entry
      double            price;                  // price
      double            price_position;         // price position
      double            sl;                     // SL
      double            tp;                     // TP
     };
   #pragma pack(pop)
   //--- Deal statistics write class
   class CDealStatWrite
     {
   private:
      IMTReportCache   &m_cache;                // cache interface
      uint64_t          m_login;                // user login
      DealStat          m_stat;                 // deal statistics

   public:
                        CDealStatWrite(IMTReportCache &cache) : m_cache(cache),m_login(0),m_stat{} {}
      //--- current deal statistics user login
      uint64_t          Login(void) const    { return(m_login); }
      //--- initialize deal statistics writer
      void              Init(const uint64_t login,const DealStat *stat);
      //--- write deal statistics to cache
      MTAPIRES          Write(void);
      //--- add deal to cache writer
      void              AddDeal(const DealRecord &deal);

   private:
      //--- add price difference to deal statistics aggregate
      static void       AddPriceDiffCheck(DealPrice &aggregate,double price,double base,bool buy);
      static void       AddPriceDiff(DealPrice &aggregate,double diff,bool buy);
     };

private:
   //--- static data
   static const DatasetField s_fields[];        // deal request dataset fields descriptions
   static const uint64_t s_actions[];             // deal request actions
   static const uint64_t s_entries[];             // deal request entries
   static const uint32_t s_request_limit=16384;     // deal request limit

public:
   //--- create class object
   static CDealUserCache* Create(IMTReportAPI &api,const int64_t from,const int64_t to,MTAPIRES &res);
   //--- read user deals statistics
   MTAPIRES          ReadStat(const uint64_t login,const DealStat *&stat) const { return(CReportCache::ReadData(login,(const void*&)stat,sizeof(*stat))); }

private:
   explicit          CDealUserCache(CReportCacheKeysTimeContext &context) : CDealBaseCache(context) {}
   virtual          ~CDealUserCache(void)=default;
   //--- prepare deals request
   virtual MTAPIRES  PrepareRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &keys) override;
   //--- fill deals request
   virtual MTAPIRES  DealRequest(IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from) override;
   //--- write deals to deals aggregates
   virtual MTAPIRES  WriteData(const IMTDataset &deals,uint64_t &id_last) override;
   //--- calculate time hash
   static uint64_t   TimeHash(const int64_t from,const int64_t to);
   //--- compression function for Merkle-Damgard construction
   static uint64_t   HashMix(uint64_t h);
  };
//+------------------------------------------------------------------+
