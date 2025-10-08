//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportCacheSelectTime.h"
#include "..\Tools\DatasetField.h"
//+------------------------------------------------------------------+
//| Deposit cache class                                              |
//+------------------------------------------------------------------+
class CDepositCache : public CReportCacheSelectTime
  {
public:
   //--- User deposit structure
   //--- Update version constant below when change this structure!
   #pragma pack(push,1)
   struct UserDeposit
     {
      double            amount;                 // deposit amount
      uint32_t          count;                  // deposit count
      int64_t           first_time;             // registration time
      double            first;                  // FTD
      uint32_t          currency_digits;        // currency digits
      //--- initialize
      void              Init(const double value,const int64_t ctm,const uint32_t _currency_digits);
      //--- deposit addition
      void              DepositAdd(const double value,const int64_t ctm);
     };
   #pragma pack(pop)

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
      double            profit;                 // profit
      uint32_t          currency_digits;        // group currency digits
     };
   #pragma pack(pop)

private:
   //--- static data
   static DatasetField s_fields[];              // deal request dataset fields descriptions

public:
   //--- create class object
   static CDepositCache* Create(IMTReportAPI &api,MTAPIRES &res);
   //--- read user deposit
   MTAPIRES          ReadDeposit(const uint64_t login,const UserDeposit *&deposit) const { return(CReportCache::ReadData(login,(const void*&)deposit,sizeof(*deposit))); }

private:
   explicit          CDepositCache(CReportCacheContext &context);
   virtual          ~CDepositCache(void);
   //--- write deals to deposits
   virtual MTAPIRES  WriteData(const IMTDataset &deals) override;
   //--- request deals from base
   virtual MTAPIRES  SelectData(IMTDataset &deals,const IMTReportCacheKeySet &keys,const int64_t from,const int64_t to) override;
   //--- fill deals request
   MTAPIRES          DealRequest(IMTDatasetRequest &request,const IMTReportCacheKeySet &logins,const int64_t from,const int64_t to);
  };
//+------------------------------------------------------------------+
