//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "..\Bases\Trades\TradeDispatcher.h"
//+------------------------------------------------------------------+
//| State of connection to Gateway API                               |
//+------------------------------------------------------------------+
enum EnGatewayAPIStatus
  {
   STATUS_API_STOPPED   =0x00,                     // no connection
   STATUS_API_CONFIGURED=0x01,                     // Gateway settings received
   STATUS_API_STARTED   =0x02,                     // started
   STATUS_API_FIRST     =STATUS_API_STOPPED,
   STATUS_API_LAST      =STATUS_API_STARTED
  };
//+------------------------------------------------------------------+
//| State of connection to external trading system                   |
//+------------------------------------------------------------------+
enum EnConnectStatus
  {
   STATUS_DISCONNECTED  =0x00,                     // no connection
   STATUS_CONNECTING    =0x01,                     // connecting
   STATUS_CONNECTED     =0x02,                     // connected
   STATUS_SYNCHRONIZED  =0x04,                     // gateway synchronized with external trading system
   STATUS_FIRST         =STATUS_DISCONNECTED,
   STATUS_LAST          =STATUS_SYNCHRONIZED
  };
//+------------------------------------------------------------------+
//| Class of gateway                                                 |
//+------------------------------------------------------------------+
class CGateway
  {

private:
   //--- constants
   enum EnConstants
     {
      RECONNECT_SHORT_TIMEOUT    =3,               // reconnect short period, sec
      RECONNECT_LONG_TIMEOUT     =10,              // reconnect big period, sec
      RECONNECT_LONG_COUNT       =5,               // count of attempts to change period on long
     };
   //--- Gateway API
   IMTGatewayAPI    *m_api_gateway;
   //--- gateway settings
   IMTConGateway    *m_api_gateway_config;
   //--- external trading system API
   CExchangeAPI     *m_api_exchange;
   //--- dispatcher of Gateway API trader requests
   CTradeDispatcher  m_trade_dispatcher;
   //--- reconnect time/attempts
   int64_t           m_connect_time;
   uint32_t          m_connect_attempts;
   //--- state
   volatile LONG     m_status_api_gateway;         // Gateway API connection state
   volatile LONG     m_status_api_exchange;        // external trading system API connection state

public:
   //--- constructor/destructor
                     CGateway();
                    ~CGateway(void);
   //--- initialization/shutdown
   bool              Initialize(IMTGatewayAPI *api_gateway);
   void              Shutdown(void);
   //--- check gateway state
   void              Check(void);
   //---
   //--- methods called by Gateway API
   //---
   //--- receive/clear gateway settings gateway settings
   bool              OnGatewayConfig(const IMTConGateway* config);
   void              OnGatewayConfigClear(void);
   //--- notify of gateway start/stop
   void              OnGatewayStart(void);
   void              OnGatewayStop(void);
   //--- dealing
   void              OnGatewayDealerLock(const MTAPIRES retcode,const IMTRequest *request);
   void              OnGatewayDealerAnswer(const MTAPIRES retcode,const IMTConfirm *confirm) const;
   //--- positions request
   MTAPIRES          HookGatewayPositionsRequest();
   //--- account state request
   MTAPIRES          HookGatewayAccountRequest(uint64_t login,LPCWSTR account_id);
   //--- send tick to Gateway API
   bool              GatewayTickSend(MTTick &tick);
   //--- send book to Gateway API
   bool              GatewayBookSend(MTBook &book);
   //--- get/set gateway API state
   long              StatusGateway(const long status) { return(InterlockedExchange(&m_status_api_gateway,status));  }
   long              StatusGateway()              { return(InterlockedExchangeAdd(&m_status_api_gateway,0));    }
   //--- get/set external connection state
   LONG              StatusExchange(void);
   LONG              StatusExchange(const LONG status);
   //---
   //--- methods of notification from external trading system
   //---
   //--- notify of synchronization
   void              OnExchangeSyncComplete(void);
   //--- notify of closed connection
   void              OnExchangeConnectClosed(void);
   //--- notify of added symbol
   bool              OnExchangeSymbolAdd(const ExchangeSymbol &exchange_symbol);
   //--- notify of order transaction in external trading system
   bool              OnExchangeOrderTrans(const ExchangeOrder &exchange_order,const ExchangeSymbol &symbol);
   //--- notify of deal transaction in external trading system
   bool              OnExchangeDealTrans(const ExchangeDeal &exchange_deal,const ExchangeSymbol &symbol);
   //--- notify of account data
   bool              OnExchangeAccountDataReceived(const ExchangeAccountData &account_data);

private:
   //--- connect/disconnect gateway
   bool              Connect(void);
   void              Disconnect(void);
   //--- import symbol data
   bool              ImportSymbol(const ExchangeSymbol &exchange_symbol,IMTConSymbol *gateway_symbol) const;
   //--- update external account for client
   MTAPIRES          UpdateExternalAccount(const IMTRequest *request) const;
   //--- convert orders to Gateway API format
   MTAPIRES          ConvertOrders(const ExchangeOrdersArray &exchange_orders,IMTOrderArray *gateway_orders) const;
   //--- convert positions to Gateway API format
   MTAPIRES          ConvertPositions(const ExchangePositionsArray &exchange_positions,IMTPositionArray *gateway_positions) const;
  };
//+------------------------------------------------------------------+
