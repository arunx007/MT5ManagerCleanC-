//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\CurrencyConverterCache.h"
#include "UserCache.h"
#include "ClientUserCache.h"
#include "BalanceCache.h"
#include "ClientCache.h"
#include "UserFilter.h"
#include "ClientFilter.h"
//+------------------------------------------------------------------+
//| Clients FTD composite cache receiver interface class             |
//+------------------------------------------------------------------+
class IClientFtdReceiver
  {
public:
   virtual          ~IClientFtdReceiver(void) {}
   //--- client FTD read handler overridable
   virtual MTAPIRES  ClientFtdRead(const CClientCache::ClientCache &client,const int64_t time,const double value)=0;
  };
//+------------------------------------------------------------------+
//| Clients FTD coposite cache class                                 |
//+------------------------------------------------------------------+
class CClientFtdCache
  {
private:
   //--- client ftd structure
   struct ClientFtd
     {
      uint64_t          id;                        // client id
      uint32_t          day;                       // first deposit day
      double            value;                     // first deposit value
     };
   //--- client ftd array type
   typedef TMTArray<ClientFtd> ClientFtdArray;

private:
   IMTReportAPI     *m_api;                        // report api
   CUserCache       *m_users;                      // user cache
   CClientUserCache *m_client_users;               // client by user cache
   CBalanceCache    *m_balances;                   // balance cache
   CClientCache     *m_clients;                    // clients cache
   IMTReportCacheKeySet *m_ids;                    // ftd clients ids
   CClientFilter     m_client_filter;              // client filter
   uint32_t          m_from;                       // day from
   uint32_t          m_to;                         // day to
   int64_t           m_clients_from;               // first client registration time
   CCurrencyConverterCache &m_currency;            // currency converter reference
   ClientFtdArray    m_ftds;                       // client ftd array
   IClientFtdReceiver *m_receiver;                 // receiver pointer

public:
   explicit          CClientFtdCache(CCurrencyConverterCache &currency);
   virtual          ~CClientFtdCache(void);
   //--- clear
   void              Clear(void);
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI &api,CReportParameter &params);
   //--- update cache
   MTAPIRES          Update(const int64_t from,const int64_t to);
   //--- read client ftd
   MTAPIRES          Read(IClientFtdReceiver *receiver);
   //--- begin read clients
   MTAPIRES          ClientsReadBegin(const CReportCache *&cache);
   //--- end read clients
   MTAPIRES          ClientsReadEnd(void);

private:
   //--- read balance clients
   MTAPIRES          ReadBalanceClients(const int64_t from,const int64_t to);
   //--- read first client registration time
   MTAPIRES          ReadClientFrom(const int64_t from,const int64_t to);
   //--- user read handler
   MTAPIRES          UserRead(const uint64_t login,const CUserCache::UserCache &user);
   //--- user balance read handler
   MTAPIRES          UserBalanceRead(const uint64_t login,const CUserCache::UserCache &user,const CBalanceCache::CUserBalance &balance);
   //--- client user read handler
   MTAPIRES          ClientUserRead(const uint64_t id,const CClientUserCache::ClientUserCache &client);
   //--- calculate ftd
   MTAPIRES          CalculateFtd(void);
   //--- initialize ftd
   MTAPIRES          InitializeFtd(void);
   //--- read balance clients ftd
   MTAPIRES          ReadBalanceClientsFtd(void);
   //--- user ftd read handler
   MTAPIRES          UserFtdRead(const uint64_t login,const CUserCache::UserCache &user);
   //--- user balance read handler
   MTAPIRES          UserBalanceFtdRead(const CUserCache::UserCache &user,const CBalanceCache::CUserBalance &balance,ClientFtd &ftd);
   //--- client read handler
   MTAPIRES          ClientRead(const uint64_t id,const CClientCache::ClientCache &client);
   //--- search static method
   static int32_t    SearchFtdClient(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
