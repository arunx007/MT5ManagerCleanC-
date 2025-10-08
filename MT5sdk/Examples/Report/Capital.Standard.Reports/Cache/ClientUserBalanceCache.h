//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ClientBalanceCache.h"
#include "ClientUserCache.h"
//+------------------------------------------------------------------+
//| Client users balances composite cache receiver class             |
//+------------------------------------------------------------------+
class CClientUserBalanceReceiver : public CClientBalanceTypes
  {
public:
   //--- cache types
   typedef CClientUserCache::ClientUserCache ClientUserCache;

public:
   virtual          ~CClientUserBalanceReceiver(void) {}
   //--- client and user balance read handler overridable
   virtual MTAPIRES  ClientUserBalanceRead(const ClientUserCache &client_user,const ClientCache &client,const CUserBalance &balance,const double *rate);
   //--- client and user balance deal read handler overridable
   virtual MTAPIRES  ClientUserBalanceDealRead(const ClientUserCache &client_user,const ClientCache &client,const DealBalance &balance,const double *rate) { return(MT_RET_ERROR); }
   //--- client inactive read handler overridable
   virtual MTAPIRES  ClientInactiveRead(const ClientCache &client) { return(MT_RET_ERROR); }
   //--- client user inactive read handler overridable
   virtual MTAPIRES  ClientUserInactiveRead(const ClientUserCache &client_user) { return(MT_RET_ERROR); }
  };
//+------------------------------------------------------------------+
//| Clients users balances composite cache class                     |
//+------------------------------------------------------------------+
class CClientUserBalanceCache : public CClientBalanceCacheBase
  {
private:
   CClientUserCache *m_client_users;               // client by user cache
   IMTReportCacheKeySet *m_active_ids;             // active clients ids
   CClientUserBalanceReceiver *m_receiver;         // receiver pointer

public:
   explicit          CClientUserBalanceCache(CCurrencyConverterCache &currency);
   virtual          ~CClientUserBalanceCache(void);
   //--- clear
   void              Clear(void);
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI &api,CReportParameter &params);
   //--- update cache
   MTAPIRES          Update(const bool until_now);
   //--- read client user balance
   MTAPIRES          Read(CClientUserBalanceReceiver *receiver);
   //--- read client inactive
   MTAPIRES          ReadClientInactive(void);
   //--- read client user inactive
   MTAPIRES          ReadClientUserInactive(void);
   //--- begin read clients
   MTAPIRES          ClientsReadBegin(const CReportCache *&cache);
   //--- end read clients
   MTAPIRES          ClientsReadEnd(void);

private:
   //--- user and client filtered out handler
   virtual MTAPIRES  UserClientFilteredOut(const uint64_t client_id) override;
   //--- user and client balance read handler
   virtual MTAPIRES  UserClientBalanceRead(const CUserCache::UserCache &user,const ClientCache &client,const CUserBalance &balance,const double *rate) override;
   //--- client read handler
   MTAPIRES          ClientRead(const uint64_t id,const ClientCache &client);
   //--- client user read handler
   MTAPIRES          ClientUserRead(const uint64_t id,const CClientUserCache::ClientUserCache &client_user);
  };
//+------------------------------------------------------------------+
