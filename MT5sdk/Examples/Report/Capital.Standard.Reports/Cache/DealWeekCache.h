//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheTime.h"
#include "UserGroupCache.h"
#include "..\Tools\DatasetField.h"
//+------------------------------------------------------------------+
//| Deal week aggregates cache class                                 |
//+------------------------------------------------------------------+
class CDealWeekCache : public CReportCacheTime
  {
public:
   //--- Deal key structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct DealKey
     {
      unsigned          week     : 24;
      unsigned          currency : 10;
      unsigned          group    : 20;
      unsigned          reserved : 10;
     };
   //--- Deal aggregate structure
   struct Deal
     {
      uint32_t          count;                  // deals count
      double            volume;                 // volume (in currency)
      double            value;                  // value of profit or loss (in currency)
     };
   //--- Balance aggregate structure
   struct Balance
     {
      uint32_t          count;                  // deals count
      double            value;                  // value (in currency)
     };
   //--- Deal week aggregate structure
   struct DealWeek
     {
      Deal              profit;                 // profit deals
      Deal              loss;                   // loss deals
      Balance           deposit;                // deposits
      Balance           withdrawal;             // withdrawals
     };
   #pragma pack(pop)
   //--- Deal key structure fields ids
   enum EnDealKeyFields
     {
      DEAL_KEY_FIELD_CURRENCY=0,
      DEAL_KEY_FIELD_GROUP   =1,
     };

private:
   //--- Cache version constant
   //--- Update it when changing structure above!
   static const uint32_t s_cache_version=1;
   //--- Deal record
   #pragma pack(push,1)
   struct DealRecord
     {
      uint64_t          login;                  // login
      int64_t           time;                   // time
      uint32_t          action;                 // action
      uint32_t          entry;                  // entry
      uint64_t          volume;                 // volume
      double            contract_size;          // symbol contract size
      double            rate_profit;            // profit conversion rate
      double            profit;                 // profit
      uint32_t          currency_digits;        // group currency digits
     };
   #pragma pack(pop)
   //--- Deal week write class
   class CDealWeekWrite
     {
   private:
      IMTReportCache   &m_cache;                // cache interface
      IMTReportCacheValue &m_value;             // report cache value interface reference
      uint64_t          m_key;                  // deal key
      DealWeek          m_week;                 // deal week

   public:
      CDealWeekWrite(IMTReportCache &cache,IMTReportCacheValue &value) :
                        m_cache(cache),m_value(value),m_key(0),m_week{} {}
      //-- current deal week key
      uint64_t          Key(void) const { return(m_key); }
      //--- read deal week from cache
      MTAPIRES          Read(const uint64_t key);
      //--- write deal week to cache
      MTAPIRES          Write(void);
      //--- add deal to cache writer
      MTAPIRES          AddDeal(const DealRecord &deal);

   private:
      //--- add balance deal to cache writer
      MTAPIRES          AddBalanceDeal(const DealRecord &deal);
     };

private:
   CUserGroupCache  &m_users;                   // users cache
   //--- static data
   static const DatasetField s_fields[];        // deal request dataset fields descriptions
   static const uint64_t s_actions[];             // deal request actions
   static const uint64_t s_entries[];             // deal request entries
   static const uint32_t s_request_limit=16384;     // deals selection request limit

public:
   //--- create class object
   static CDealWeekCache* Create(IMTReportAPI &api,CUserGroupCache &users,CReportParameter &params,CCurrencyConverterCache &currency,MTAPIRES &res);
   //--- update deal week aggregates
   MTAPIRES          Update(int64_t from,int64_t to);
   //--- read currency string
   MTAPIRES          ReadCurrency(const DealKey &key,LPCWSTR &currency) const { return(ReadDictionaryString(DEAL_KEY_FIELD_CURRENCY,key.currency,currency)); }
   //--- get deal week key
   static const DealKey& Key(const uint64_t &key) { return(*(const DealKey*)(&key)); }

private:
                     CDealWeekCache(CReportCacheContext &context,CUserGroupCache &users);
   virtual          ~CDealWeekCache(void)=default;
   //--- write cache
   virtual MTAPIRES  Write(int64_t from,int64_t to) override;
   //--- write cache range
   virtual MTAPIRES  WriteRange(int64_t from,int64_t to) override;
   //--- request deals from base
   virtual MTAPIRES  SelectData(IMTDataset &deals,int64_t from,int64_t to,uint64_t id_from) override;
   //--- fill deals request
   MTAPIRES          DealRequest(IMTDatasetRequest &request,int64_t from,int64_t to,uint64_t id_from);
   //--- write deals to deals aggregates
   virtual MTAPIRES  WriteData(const IMTDataset &deals,uint64_t &id_last) override;
   //--- make deal key
   static uint64_t   MakeKey(const DealRecord &deal,uint32_t currency,uint32_t group);
   //--- end of month
   static int64_t    MonthEnd(int64_t ctm,int64_t to);
   //--- logging
   virtual MTAPIRES  LogSelectError(const MTAPIRES res,const int64_t from,const int64_t to,const uint64_t id_from) override
     {
      return(LogErrorTime(res,L"Deals selection from #%I64u with creation time",from,to,id_from));
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
