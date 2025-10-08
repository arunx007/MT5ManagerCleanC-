//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\CurrencyConverterCache.h"
#include "..\Tools\ReportTop.h"
#include "..\Tools\ReportDictionary.h"
#include "UserCache.h"
#include "BalanceCache.h"
#include "ClientCache.h"
#include "UserFilter.h"
#include "ClientFilter.h"
//+------------------------------------------------------------------+
//| Clients balances composite cache types class                     |
//+------------------------------------------------------------------+
class CClientBalanceTypes
  {
public:
   //--- cache types
   typedef CClientCache::ClientCache ClientCache;
   typedef CBalanceCache::CUserBalance CUserBalance;
   typedef CBalanceCache::DealBalance DealBalance;

public:
   virtual          ~CClientBalanceTypes(void) {}
  };
//+------------------------------------------------------------------+
//| Clients balances composite cache base class                      |
//+------------------------------------------------------------------+
class CClientBalanceCacheBase : public CClientBalanceTypes
  {
protected:
   IMTReportAPI     *m_api;                        // report api
   CUserCache       *m_users;                      // user cache
   CBalanceCache    *m_balances;                   // balance cache
   CClientCache     *m_clients;                    // clients cache
   CClientFilter     m_client_filter;              // client filter
   int64_t           m_now;                        // report generation time

private:
   CCurrencyConverterCache &m_currency;            // currency converter reference

public:
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI &api,CReportParameter &params);
   //--- read client dictionary string
   MTAPIRES          ReadDictionaryString(const uint32_t dictionary_id,const uint32_t pos,LPCWSTR &string) const;
   //--- eead top names
   MTAPIRES          ReadTopNames(const uint32_t dictionary_id,const CReportTopBase &top,CReportDictionary &names);

protected:
   explicit          CClientBalanceCacheBase(CCurrencyConverterCache &currency);
   virtual          ~CClientBalanceCacheBase(void);
   //--- clear
   void              Clear(void);
   //--- read client balance
   MTAPIRES          Read(const IMTReportCacheKeySet &logins);

private:
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);
   //--- user balance read handler
   MTAPIRES          UserBalanceRead(const CUserCache::UserCache &user,const CUserBalance &balance);
   //--- user and client filtered out handler overridable
   virtual MTAPIRES  UserClientFilteredOut(const uint64_t client_id) { return(MT_RET_OK); }
   //--- user and client balance read handler overridable
   virtual MTAPIRES  UserClientBalanceRead(const CUserCache::UserCache &user,const ClientCache &client,const CUserBalance &balance,const double *rate)=0;
  };
//+------------------------------------------------------------------+
//| Clients balances composite cache receiver class                  |
//+------------------------------------------------------------------+
class CClientBalanceReceiver : public CClientBalanceTypes
  {
public:
   virtual          ~CClientBalanceReceiver(void) {}
   //--- client balance read handler overridable
   virtual MTAPIRES  ClientBalanceRead(const ClientCache &client,const CUserBalance &balance,const double *rate);
   //--- client balance deal read handler overridable
   virtual MTAPIRES  ClientBalanceDealRead(const ClientCache &client,const DealBalance &balance,const double *rate) { return(MT_RET_ERROR); }
  };
//+------------------------------------------------------------------+
//| Clients balances composite cache class                           |
//+------------------------------------------------------------------+
class CClientBalanceCache : public CClientBalanceCacheBase
  {
private:
   CUserFilterWithClients *m_user_clients;         // user clients filter
   const IMTReportCacheKeySet *m_logins;           // user logins
   CClientBalanceReceiver *m_receiver;             // receiver pointer

public:
   explicit          CClientBalanceCache(CCurrencyConverterCache &currency);
   virtual          ~CClientBalanceCache(void);
   //--- clear
   void              Clear(void);
   //--- update cache
   MTAPIRES          Update(const bool until_now);
   //--- read client balance
   MTAPIRES          Read(CClientBalanceReceiver *receiver);

private:
   //--- user and client balance read handler overridable
   virtual MTAPIRES  UserClientBalanceRead(const CUserCache::UserCache &user,const ClientCache &client,const CUserBalance &balance,const double *rate) override;
  };
//+------------------------------------------------------------------+
