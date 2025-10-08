//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Manager |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once

//+------------------------------------------------------------------+
//| Manager API version                                              |
//+------------------------------------------------------------------+
#define MTManagerAPIVersion  5200
#define MTManagerAPIDate     L"1 Aug 2025"

#include <wincrypt.h>
#include <cstdint>
#include <intrin.h>
#include "MT5APIConstants.h"
#include "MT5APILogger.h"
#include "MT5APIPublicKey.h"
#include "MT5APITools.h"
#include "Config\MT5APIConfigCommon.h"
#include "Config\MT5APIConfigFeeder.h"
#include "Config\MT5APIConfigFirewall.h"
#include "Config\MT5APIConfigGateway.h"
#include "Config\MT5APIConfigGroup.h"
#include "Config\MT5APIConfigHistory.h"
#include "Config\MT5APIConfigHoliday.h"
#include "Config\MT5APIConfigManager.h"
#include "Config\MT5APIConfigNetwork.h"
#include "Config\MT5APIConfigPlugin.h"
#include "Config\MT5APIConfigReport.h"
#include "Config\MT5APIConfigRoute.h"
#include "Config\MT5APIConfigSpread.h"
#include "Config\MT5APIConfigSymbol.h"
#include "Config\MT5APIConfigTime.h"
#include "Config\MT5APIConfigEmail.h"
#include "Config\MT5APIConfigMessenger.h"
#include "Config\MT5APIConfigAutomation.h"
#include "Config\MT5APIConfigSubscription.h"
#include "Config\MT5APIConfigVPS.h"
#include "Config\MT5APIConfigKYC.h"
#include "Config\MT5APIConfigLeverage.h"
#include "Bases\MT5APIAccount.h"
#include "Bases\MT5APIBook.h"
#include "Bases\MT5APIByteStream.h"
#include "Bases\MT5APICertificate.h"
#include "Bases\MT5APIChart.h"
#include "Bases\MT5APIConfirm.h"
#include "Bases\MT5APIDaily.h"
#include "Bases\MT5APIDeal.h"
#include "Bases\MT5APIExposure.h"
#include "Bases\MT5APIMail.h"
#include "Bases\MT5APINews.h"
#include "Bases\MT5APIOrder.h"
#include "Bases\MT5APIPosition.h"
#include "Bases\MT5APIRequest.h"
#include "Bases\MT5APISummary.h"
#include "Bases\MT5APITick.h"
#include "Bases\MT5APIUser.h"
#include "Bases\MT5APIOnline.h"
#include "Bases\MT5APIECN.h"
#include "Bases\MT5APIClient.h"
#include "Bases\MT5APIDocument.h"
#include "Bases\MT5APIAttachment.h"
#include "Bases\MT5APIComment.h"
#include "Bases\MT5APISubscription.h"
#include "Bases\MT5APIGeo.h"
//+------------------------------------------------------------------+
//| Include necessary library                                        |
//+------------------------------------------------------------------+
#pragma comment(lib,"advapi32.lib")
//+------------------------------------------------------------------+
//| Proxy description                                                |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTProxyInfo
  {
   //--- types of proxy servers
   enum
     {
      PROXY_SOCKS4   =0,                     // SOCKS4
      PROXY_SOCKS5   =1,                     // SOCKS5
      PROXY_HTTP     =2,                     // HTTP (including NTLM)
      PROXY_FIRST    =PROXY_SOCKS4,          // first type
      PROXY_LAST     =PROXY_HTTP             // last type
     };
   //--- proxy description
   int32_t           enable;                 // proxy enabled or disabled
   int32_t           type;                   // type of proxy server
   wchar_t           address[64];            // IP:port of proxy server
   wchar_t           auth[64];               // login:password
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Manager API events notification interface                        |
//+------------------------------------------------------------------+
class IMTManagerSink
  {
public:
   virtual void      OnDisconnect(void) {  }
   virtual void      OnTradeAccountSet(const MTAPIRES           /*retcode*/,
                                       const int64_t              /*request_id*/,
                                       const IMTUser*           /*user*/,
                                       const IMTAccount*        /*account*/,
                                       const IMTOrderArray*     /*orders*/,
                                       const IMTPositionArray*  /*positions*/) {  }
   virtual void      OnConnect(void) {  }
  };
//+------------------------------------------------------------------+
//| Dealer events notification interface                             |
//+------------------------------------------------------------------+
class IMTDealerSink
  {
public:
   virtual void      OnDealerResult(const IMTConfirm* /*result*/)  {  }
   virtual void      OnDealerAnswer(const IMTRequest* /*request*/) {  }
  };
//+------------------------------------------------------------------+
//| Manager API interface                                            |
//+------------------------------------------------------------------+
class IMTManagerAPI
  {
public:
   //--- pumping modes
   enum EnPumpModes
     {
      PUMP_MODE_USERS         =0x00000001,   // pump users
      PUMP_MODE_ACTIVITY      =0x00000002,   // pump users online activity
      PUMP_MODE_MAIL          =0x00000004,   // pump mails
      PUMP_MODE_ORDERS        =0x00000008,   // pump orders
      PUMP_MODE_NEWS          =0x00000020,   // pump news
      PUMP_MODE_POSITIONS     =0x00000080,   // pump positions
      PUMP_MODE_GROUPS        =0x00000100,   // pump group configurations
      PUMP_MODE_SYMBOLS       =0x00000200,   // pump symbol configurations
      PUMP_MODE_HOLIDAYS      =0x00000800,   // pump holiday configurations
      PUMP_MODE_TIME          =0x00001000,   // pump time configuration
      PUMP_MODE_GATEWAYS      =0x00002000,   // pump gateways configurations
      PUMP_MODE_REQUESTS      =0x00004000,   // pump requests (for supervisor)
      PUMP_MODE_PLUGINS       =0x00008000,   // pump plugins configurations
      PUMP_MODE_CLIENTS       =0x00010000,   // pump clients
      PUMP_MODE_SUBSCRIPTIONS =0x00020000,   // pump subscriptions
      //--- enumeration ranges
      PUMP_MODE_FULL          =0xffffffff    // full pumping
     };
   //--- external account sync modes
   enum EnExternalSyncModes
     {
      EXTERNAL_SYNC_ALL      =0,
      EXTERNAL_SYNC_BALANCE  =1,
      EXTERNAL_SYNC_POSITIONS=2,
      EXTERNAL_SYNC_ORDERS   =3,
      EXTERNAL_SYNC_LAST     =EXTERNAL_SYNC_ORDERS
     };
   //--- interface release
   virtual void      Release(void)=0;
   //--- Manager license check
   virtual MTAPIRES  LicenseCheck(MTLicenseCheck& check)=0;
   //--- memory management
   virtual void*     Allocate(const uint32_t bytes)=0;
   virtual void      Free(void* ptr)=0;
   //--- proxy
   virtual void      ProxySet(const MTProxyInfo &proxy)=0;
   //--- connect/disconnect
   virtual MTAPIRES  Connect(LPCWSTR server,uint64_t login,LPCWSTR password,LPCWSTR password_cert,uint64_t pump_mode,uint32_t timeout=INFINITE)=0;
   virtual void      Disconnect(void)=0;
   virtual MTAPIRES  Subscribe(IMTManagerSink* sink)=0;
   virtual MTAPIRES  Unsubscribe(IMTManagerSink* sink)=0;
   //--- API journal
   virtual MTAPIRES  LoggerOut(const uint32_t code,LPCWSTR format,...)=0;
   virtual void      LoggerFlush(void)=0;
   virtual MTAPIRES  LoggerServerRequest(const uint32_t mode,const uint32_t type,const int64_t from,const int64_t to,LPCWSTR filter,
                                         MTLogRecord*& records,uint32_t& records_total)=0;
   virtual MTAPIRES  LoggerOutString(const uint32_t code,LPCWSTR string)=0;
   virtual MTAPIRES  LoggerReserved2(void)=0;
   virtual MTAPIRES  LoggerReserved3(void)=0;
   virtual MTAPIRES  LoggerReserved4(void)=0;
   //--- time configuration
   virtual IMTConTime* TimeCreate(void)=0;
   virtual MTAPIRES  TimeSubscribe(IMTConTimeSink* sink)=0;
   virtual MTAPIRES  TimeUnsubscribe(IMTConTimeSink* sink)=0;
   virtual MTAPIRES  TimeGet(IMTConTime* config)=0;
   virtual int64_t   TimeServer(void)=0;
   virtual MTAPIRES  TimeServerRequest(int64_t& time_msc)=0;
   virtual MTAPIRES  TimeReserved2(void)=0;
   virtual MTAPIRES  TimeReserved3(void)=0;
   virtual MTAPIRES  TimeReserved4(void)=0;
   //--- holidays configuration
   virtual IMTConHoliday* HolidayCreate()=0;
   virtual MTAPIRES  HolidaySubscribe(IMTConHolidaySink* sink)=0;
   virtual MTAPIRES  HolidayUnsubscribe(IMTConHolidaySink* sink)=0;
   virtual uint32_t  HolidayTotal(void)=0;
   virtual MTAPIRES  HolidayNext(const uint32_t pos,IMTConHoliday* config)=0;
   //--- manager configuration
   virtual IMTConManagerReport* ManagerReportCreate(void)=0;
   virtual IMTConManager* ManagerCreate(void)=0;
   virtual IMTConManagerAccess* ManagerAccessCreate(void)=0;
   virtual MTAPIRES  ManagerCurrent(IMTConManager* manager)=0;
   //--- clients group configuration
   virtual IMTConGroup* GroupCreate(void)=0;
   virtual IMTConGroupSymbol* GroupSymbolCreate(void)=0;
   virtual IMTConCommission* GroupCommissionCreate(void)=0;
   virtual IMTConCommTier* GroupTierCreate(void)=0;
   virtual MTAPIRES  GroupSubscribe(IMTConGroupSink* sink)=0;
   virtual MTAPIRES  GroupUnsubscribe(IMTConGroupSink* sink)=0;
   virtual uint32_t  GroupTotal(void)=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConGroup* group)=0;
   virtual MTAPIRES  GroupGet(LPCWSTR name,IMTConGroup* group)=0;
   virtual MTAPIRES  GroupRequest(LPCWSTR name,IMTConGroup* group)=0;
   virtual MTAPIRES  GroupUpdate(IMTConGroup* group)=0;
   virtual MTAPIRES  GroupUpdateBatch(IMTConGroup** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual IMTConGroupArray* GroupCreateArray(void)=0;
   virtual MTAPIRES  GroupRequestArray(LPCWSTR mask,IMTConGroupArray* groups)=0;
   //--- symbols configuration
   virtual IMTConSymbol* SymbolCreate(void)=0;
   virtual IMTConSymbolSession* SymbolSessionCreate(void)=0;
   virtual MTAPIRES  SymbolSubscribe(IMTConSymbolSink* sink)=0;
   virtual MTAPIRES  SymbolUnsubscribe(IMTConSymbolSink* sink)=0;
   virtual uint32_t  SymbolTotal(void)=0;
   virtual MTAPIRES  SymbolNext(const uint32_t pos,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,LPCWSTR group,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolRequest(LPCWSTR name,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolRequest(LPCWSTR name,LPCWSTR group,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolExist(const IMTConSymbol* symbol,const IMTConGroup* group)=0;
   virtual MTAPIRES  SymbolUpdate(IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolUpdateBatch(IMTConSymbol** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual IMTConSymbolArray* SymbolCreateArray(void)=0;
   virtual MTAPIRES  SymbolRequestArray(LPCWSTR mask,LPCWSTR group,IMTConSymbolArray* symbols)=0;
   //--- selected symbols base
   virtual MTAPIRES  SelectedAdd(LPCWSTR symbol)=0;
   virtual MTAPIRES  SelectedAddAll(void)=0;
   virtual MTAPIRES  SelectedDelete(LPCWSTR symbol)=0;
   virtual MTAPIRES  SelectedDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SelectedDeleteAll(void)=0;
   virtual MTAPIRES  SelectedShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SelectedTotal(void)=0;
   virtual MTAPIRES  SelectedNext(const uint32_t pos,MTAPISTR &symbol)=0;
   virtual MTAPIRES  SelectedAddBatch(LPWSTR* symbols,uint32_t symbols_total)=0;
   virtual MTAPIRES  SelectedDeleteBatch(LPWSTR* symbols,uint32_t symbols_total)=0;
   virtual MTAPIRES  SelectedReserved1(void)=0;
   virtual MTAPIRES  SelectedReserved2(void)=0;
   //--- clients database
   virtual IMTUser*  UserCreate(void)=0;
   virtual IMTAccount* UserCreateAccount(void)=0;
   virtual MTAPIRES  UserSubscribe(IMTUserSink* sink)=0;
   virtual MTAPIRES  UserUnsubscribe(IMTUserSink* sink)=0;
   virtual MTAPIRES  UserAdd(IMTUser* user,LPCWSTR master_pass,LPCWSTR investor_pass)=0;
   virtual MTAPIRES  UserDelete(const uint64_t login)=0;
   virtual MTAPIRES  UserUpdate(IMTUser* user)=0;
   virtual uint32_t  UserTotal(void)=0;
   virtual MTAPIRES  UserGet(const uint64_t login,IMTUser* user)=0;
   virtual MTAPIRES  UserRequest(const uint64_t login,IMTUser *user)=0;
   virtual MTAPIRES  UserGroup(const uint64_t login,MTAPISTR& group)=0;
   virtual MTAPIRES  UserLogins(LPCWSTR group,uint64_t*& logins,uint32_t& logins_total)=0;
   virtual MTAPIRES  UserPasswordCheck(const uint32_t type,const uint64_t login,LPCWSTR password)=0;
   virtual MTAPIRES  UserPasswordChange(const uint32_t type,const uint64_t login,LPCWSTR password)=0;
   virtual MTAPIRES  UserCertDelete(const uint64_t login)=0;
   virtual MTAPIRES  UserCertConfirm(const uint64_t login)=0;
   virtual MTAPIRES  UserExternalSync(const uint64_t login,const uint64_t gateway_id,LPCWSTR account_id,uint32_t sync_mode)=0;
   virtual MTAPIRES  UserAccountGet(const uint64_t login,IMTAccount* account)=0;
   virtual MTAPIRES  UserAccountRequest(const uint64_t login,IMTAccount* account)=0;
   virtual IMTCertificate *UserCertCreate(void)=0;
   virtual MTAPIRES  UserCertUpdate(const uint64_t login,const IMTCertificate *certificate)=0;
   virtual MTAPIRES  UserCertGet(const uint64_t login,IMTCertificate *certificate)=0;
   virtual MTAPIRES  UserBalanceCheck(const uint64_t login,const uint32_t fixflag,double& balance_user,double& balance_history,double& credit_user,double& credit_history)=0;
   //--- deals database
   virtual IMTDeal*  DealCreate(void)=0;
   virtual IMTDealArray* DealCreateArray(void)=0;
   virtual MTAPIRES  DealRequest(const uint64_t ticket,IMTDeal* deal)=0;
   virtual MTAPIRES  DealRequest(const uint64_t login,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealSubscribe(IMTDealSink* sink)=0;
   virtual MTAPIRES  DealUnsubscribe(IMTDealSink* sink)=0;
   virtual MTAPIRES  DealDelete(const uint64_t ticket)=0;
   virtual MTAPIRES  DealUpdate(IMTDeal* deal)=0;
   //--- trade positions database
   virtual IMTPosition* PositionCreate(void)=0;
   virtual IMTPositionArray* PositionCreateArray(void)=0;
   virtual MTAPIRES  PositionSubscribe(IMTPositionSink* sink)=0;
   virtual MTAPIRES  PositionUnsubscribe(IMTPositionSink* sink)=0;
   virtual MTAPIRES  PositionGet(const uint64_t login,LPCWSTR symbol,IMTPosition* position)=0;
   virtual MTAPIRES  PositionGet(const uint64_t login,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequest(const uint64_t login,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionGetByTicket(const uint64_t ticket,IMTPosition* position)=0;
   virtual MTAPIRES  PositionDelete(IMTPosition* position)=0;
   virtual MTAPIRES  PositionUpdate(IMTPosition* position)=0;
   virtual MTAPIRES  PositionGetByGroup(LPCWSTR mask,IMTPositionArray* positions)=0;
   //--- trade orders database
   virtual IMTOrder* OrderCreate(void)=0;
   virtual IMTOrderArray* OrderCreateArray(void)=0;
   virtual MTAPIRES  OrderSubscribe(IMTOrderSink* sink)=0;
   virtual MTAPIRES  OrderUnsubscribe(IMTOrderSink* sink)=0;
   virtual MTAPIRES  OrderGet(const uint64_t ticket,IMTOrder* order)=0;
   virtual MTAPIRES  OrderGetOpen(const uint64_t login,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequest(const uint64_t ticket,IMTOrder* order)=0;
   virtual MTAPIRES  OrderRequestOpen(const uint64_t login,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequest(const uint64_t login,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderDelete(const uint64_t ticket)=0;
   virtual MTAPIRES  OrderUpdate(IMTOrder *order)=0;
   virtual MTAPIRES  OrderGetByGroup(LPCWSTR mask,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderGetByLogins(const uint64_t *logins,const uint32_t logins_total,IMTOrderArray* orders)=0;
   //--- ticks data
   virtual MTAPIRES  TickSubscribe(IMTTickSink* sink)=0;
   virtual MTAPIRES  TickUnsubscribe(IMTTickSink* sink)=0;
   virtual MTAPIRES  TickAdd(LPCWSTR symbol,MTTick& tick)=0;
   virtual MTAPIRES  TickAddStat(MTTick& tick,MTTickStat& stat)=0;
   virtual MTAPIRES  TickLast(LPCWSTR symbol,MTTickShort& tick)=0;
   virtual MTAPIRES  TickLast(uint32_t& id,MTTick*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickStat(LPCWSTR symbol,MTTickStat& stat)=0;
   virtual MTAPIRES  TickHistoryRequest(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickLast(LPCWSTR symbol,LPCWSTR group,MTTickShort& tick)=0;
   virtual MTAPIRES  TickLastRaw(LPCWSTR symbol,MTTickShort& tick)=0;
   virtual MTAPIRES  TickAddBatch(MTTick* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickReserved2(void)=0;
   //--- books
   virtual MTAPIRES  BookSubscribe(LPCWSTR symbol,IMTBookSink* sink)=0;
   virtual MTAPIRES  BookUnsubscribe(LPCWSTR symbol,IMTBookSink* sink)=0;
   virtual MTAPIRES  BookGet(LPCWSTR symbol,MTBook& book)=0;
   virtual MTAPIRES  BookSubscribeBatch(LPWSTR* symbols,uint32_t symbols_total,IMTBookSink* sink)=0;
   virtual MTAPIRES  BookUnsubscribeBatch(LPWSTR* symbols,uint32_t symbols_total,IMTBookSink* sink)=0;
   virtual MTAPIRES  BookReserved3(void)=0;
   virtual MTAPIRES  BookReserved4(void)=0;
   //--- internal email
   virtual IMTMail*  MailCreate(void)=0;
   virtual MTAPIRES  MailSubscribe(IMTMailSink* sink)=0;
   virtual MTAPIRES  MailUnsubscribe(IMTMailSink* sink)=0;
   virtual uint32_t  MailTotal(void)=0;
   virtual MTAPIRES  MailNext(const uint32_t pos,IMTMail* mail)=0;
   virtual MTAPIRES  MailDelete(const uint32_t pos)=0;
   virtual MTAPIRES  MailDeleteId(const uint64_t id)=0;
   virtual MTAPIRES  MailSend(IMTMail* mail)=0;
   virtual MTAPIRES  MailBodyRequest(const uint64_t id,IMTMail *mail)=0;
   virtual MTAPIRES  MailReserved2(void)=0;
   virtual MTAPIRES  MailReserved3(void)=0;
   virtual MTAPIRES  MailReserved4(void)=0;
   //--- internal news
   virtual IMTNews*  NewsCreate(void)=0;
   virtual MTAPIRES  NewsSubscribe(IMTNewsSink* sink)=0;
   virtual MTAPIRES  NewsUnsubscribe(IMTNewsSink* sink)=0;
   virtual uint32_t  NewsTotal(void)=0;
   virtual MTAPIRES  NewsNext(const uint32_t pos,IMTNews* news)=0;
   virtual MTAPIRES  NewsBodyRequest(const uint64_t id,IMTNews *news)=0;
   virtual MTAPIRES  NewsSend(IMTNews* news)=0;
   virtual MTAPIRES  NewsReserved1(void)=0;
   virtual MTAPIRES  NewsReserved2(void)=0;
   virtual MTAPIRES  NewsReserved3(void)=0;
   virtual MTAPIRES  NewsReserved4(void)=0;
   //--- requests
   virtual IMTRequest* RequestCreate(void)=0;
   virtual IMTRequestArray* RequestCreateArray(void)=0;
   virtual MTAPIRES  RequestSubscribe(IMTRequestSink* sink)=0;
   virtual MTAPIRES  RequestUnsubscribe(IMTRequestSink* sink)=0;
   virtual uint32_t  RequestTotal(void)=0;
   virtual MTAPIRES  RequestNext(const uint32_t pos,IMTRequest* request)=0;
   virtual MTAPIRES  RequestGet(const uint32_t id,IMTRequest* request)=0;
   virtual MTAPIRES  RequestGetAll(IMTRequestArray* requests)=0;
   virtual MTAPIRES  RequestReserved1(void)=0;
   virtual MTAPIRES  RequestReserved2(void)=0;
   virtual MTAPIRES  RequestReserved3(void)=0;
   virtual MTAPIRES  RequestReserved4(void)=0;
   //--- dealing
   virtual IMTConfirm* DealerConfirmCreate(void)=0;
   virtual MTAPIRES  DealerUnsubscribe(IMTDealerSink* sink)=0;
   virtual MTAPIRES  DealerStart(void)=0;
   virtual MTAPIRES  DealerStop(void)=0;
   virtual MTAPIRES  DealerGet(IMTRequest* request)=0;
   virtual MTAPIRES  DealerLock(const uint32_t id,IMTRequest* request)=0;
   virtual MTAPIRES  DealerAnswer(IMTConfirm* confirm)=0;
   virtual MTAPIRES  DealerSend(IMTRequest* request,IMTDealerSink* sink,uint32_t& id)=0;
   virtual MTAPIRES  DealerBalance(const uint64_t login,const double value,const uint32_t type,LPCWSTR comment,uint64_t& deal_id)=0;
   virtual MTAPIRES  DealerBalanceRaw(const uint64_t login,const double value,const uint32_t type,LPCWSTR comment,uint64_t& deal_id)=0;
   virtual MTAPIRES  DealerReserved1(void)=0;
   virtual MTAPIRES  DealerReserved2(void)=0;
   virtual MTAPIRES  DealerReserved3(void)=0;
   //--- custom commands
   virtual MTAPIRES  CustomCommand(LPCVOID indata,const uint32_t indata_len,LPVOID& outdata,uint32_t& outdata_len)=0;
   virtual MTAPIRES  CustomCommand(IMTByteStream* indata,IMTByteStream* outdata)=0;
   virtual IMTByteStream* CustomCreateStream(void)=0;
   virtual MTAPIRES  CustomReserved3(void)=0;
   virtual MTAPIRES  CustomReserved4(void)=0;
   //--- trade summary
   virtual IMTSummary* SummaryCreate(void)=0;
   virtual IMTSummaryArray* SummaryCreateArray(void)=0;
   virtual MTAPIRES  SummarySubscribe(IMTSummarySink* sink)=0;
   virtual MTAPIRES  SummaryUnsubscribe(IMTSummarySink* sink)=0;
   virtual LPCWSTR   SummaryCurrency(void)=0;
   virtual MTAPIRES  SummaryCurrency(LPCWSTR currency)=0;
   virtual uint32_t  SummaryTotal(void)=0;
   virtual MTAPIRES  SummaryNext(const uint32_t pos,IMTSummary* summary)=0;
   virtual MTAPIRES  SummaryGet(LPCWSTR symbol,IMTSummary* summary)=0;
   virtual MTAPIRES  SummaryGetAll(IMTSummaryArray* summary)=0;
   //--- exposure
   virtual IMTExposure* ExposureCreate(void)=0;
   virtual IMTExposureArray* ExposureCreateArray(void)=0;
   virtual MTAPIRES  ExposureSubscribe(IMTExposureSink* sink)=0;
   virtual MTAPIRES  ExposureUnsubscribe(IMTExposureSink* sink)=0;
   virtual LPCWSTR   ExposureCurrency(void)=0;
   virtual MTAPIRES  ExposureCurrency(LPCWSTR currency)=0;
   virtual uint32_t  ExposureTotal(void)=0;
   virtual MTAPIRES  ExposureNext(const uint32_t pos,IMTExposure* exposure)=0;
   virtual MTAPIRES  ExposureGet(LPCWSTR symbol,IMTExposure* exposure)=0;
   virtual MTAPIRES  ExposureGetAll(IMTExposureArray* exposure)=0;
   //--- external accounts
   virtual MTAPIRES  UserExternalGet(const uint64_t gateway_id,LPCWSTR account,IMTUser* user)=0;
   virtual MTAPIRES  UserExternalGet(LPCWSTR account,IMTUser* user)=0;
   virtual MTAPIRES  UserExternalRequest(const uint64_t gateway_id,LPCWSTR account,IMTUser* user)=0;
   virtual MTAPIRES  UserExternalRequest(LPCWSTR account,IMTUser* user)=0;
   //--- clients and trade accounts request
   virtual IMTUserArray* UserCreateArray(void)=0;
   virtual IMTAccountArray* UserCreateAccountArray(void)=0;
   virtual MTAPIRES  UserRequestArray(LPCWSTR group,IMTUserArray* users)=0;
   virtual MTAPIRES  UserAccountRequestArray(LPCWSTR group,IMTAccountArray *accounts)=0;
   //--- self password management
   virtual MTAPIRES  PasswordChange(const uint32_t type,LPCWSTR password)=0;
   virtual MTAPIRES  PasswordReserved1(void)=0;
   virtual MTAPIRES  PasswordReserved2(void)=0;
   //--- daily reports database
   virtual IMTDaily* DailyCreate(void)=0;
   virtual IMTDailyArray* DailyCreateArray(void)=0;
   virtual MTAPIRES  DailyRequest(const uint64_t login,const int64_t from,const int64_t to,IMTDailyArray* daily)=0;
   virtual MTAPIRES  DailyRequestLight(const uint64_t login,const int64_t from,const int64_t to,IMTDailyArray* daily)=0;
   virtual MTAPIRES  DailyRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTDailyArray* daily)=0;
   virtual MTAPIRES  DailyRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTDailyArray* daily)=0;
   virtual MTAPIRES  DailyRequestLightByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTDailyArray* daily)=0;
   virtual MTAPIRES  DailyRequestLightByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTDailyArray* daily)=0;
   //--- plug-ins configuration
   virtual IMTConPlugin* PluginCreate(void)=0;
   virtual IMTConPluginModule* PluginModuleCreate(void)=0;
   virtual IMTConParam* PluginParamCreate(void)=0;
   virtual MTAPIRES  PluginUpdate(IMTConPlugin* plugin)=0;
   virtual uint32_t  PluginTotal(void)=0;
   virtual MTAPIRES  PluginNext(const uint32_t pos,IMTConPlugin* plugin)=0;
   virtual MTAPIRES  PluginGet(LPCWSTR name,IMTConPlugin* plugin)=0;
   virtual MTAPIRES  PluginReserved1(void)=0;
   virtual MTAPIRES  PluginReserved2(void)=0;
   virtual MTAPIRES  PluginReserved3(void)=0;
   virtual MTAPIRES  PluginReserved4(void)=0;
   //--- spreads configuration
   virtual IMTConSpread* SpreadCreate(void)=0;
   virtual IMTConSpreadLeg* SpreadLegCreate(void)=0;
   virtual MTAPIRES  SpreadSubscribe(IMTConSpreadSink* sink)=0;
   virtual MTAPIRES  SpreadUnsubscribe(IMTConSpreadSink* sink)=0;
   virtual uint32_t  SpreadTotal(void)=0;
   virtual MTAPIRES  SpreadNext(const uint32_t pos,IMTConSpread* spread)=0;
   virtual MTAPIRES  SpreadReserved1(void)=0;
   virtual MTAPIRES  SpreadReserved2(void)=0;
   virtual MTAPIRES  SpreadReserved3(void)=0;
   virtual MTAPIRES  SpreadReserved4(void)=0;
   //--- trade methods
   virtual MTAPIRES  TradeProfit(LPCWSTR group,LPCWSTR symbol,const uint32_t type,const uint64_t volume,const double price_open,const double price_close,double& profit,double& profit_rate)=0;
   virtual MTAPIRES  TradeRateBuy(LPCWSTR base,LPCWSTR currency,double& rate,LPCWSTR group=NULL,LPCWSTR symbol=NULL,const double price=0)=0;
   virtual MTAPIRES  TradeRateSell(LPCWSTR base,LPCWSTR currency,double& rate,LPCWSTR group=NULL,LPCWSTR symbol=NULL,const double price=0)=0;
   virtual MTAPIRES  TradeMarginCheck(const uint64_t login,LPCWSTR symbol,const uint32_t type,const uint64_t volume,const double price,IMTAccount* account_new=NULL,IMTAccount* account_current=NULL)=0;
   virtual MTAPIRES  TradeMarginCheck(const IMTOrder* order,IMTAccount* account_new=NULL,IMTAccount* account_current=NULL)=0;
   virtual MTAPIRES  TradeProfitExt(LPCWSTR group,LPCWSTR symbol,const uint32_t type,const uint64_t volume,const double price_open,const double price_close,double& profit,double& profit_rate)=0;
   virtual MTAPIRES  TradeMarginCheckExt(const uint64_t login,LPCWSTR symbol,const uint32_t type,const uint64_t volume,const double price,IMTAccount* account_new=NULL,IMTAccount* account_current=NULL)=0;
   virtual MTAPIRES  TradeReserved3(void)=0;
   virtual MTAPIRES  TradeReserved4(void)=0;
   //--- network connection
   virtual MTAPIRES  NetworkRescan(const uint32_t flags,const uint32_t timeout)=0;
   virtual uint64_t  NetworkBytesSent(void)=0;
   virtual uint64_t  NetworkBytesRead(void)=0;
   virtual MTAPIRES  NetworkServer(MTAPISTR& server)=0;
   virtual MTAPIRES  NetworkAddress(MTAPISTR& address)=0;
   virtual MTAPIRES  NetworkReserved1(void)=0;
   virtual MTAPIRES  NetworkReserved2(void)=0;
   virtual MTAPIRES  NetworkReserved3(void)=0;
   virtual MTAPIRES  NetworkReserved4(void)=0;
   //--- online connection data
   virtual IMTOnline* OnlineCreate(void)=0;
   virtual IMTOnlineArray* OnlineCreateArray(void)=0;
   virtual uint32_t  OnlineTotal(void)=0;
   virtual MTAPIRES  OnlineNext(const uint32_t pos,IMTOnline* online)=0;
   virtual MTAPIRES  OnlineGet(const uint64_t login,IMTOnlineArray* online)=0;
   virtual MTAPIRES  OnlineDisconnect(IMTOnline* online)=0;
   virtual MTAPIRES  OnlineDisconnectBatch(IMTOnlineArray* online,MTAPIRES* results)=0;
   virtual MTAPIRES  OnlineDisconnectBatchArray(IMTOnline** online,const uint32_t online_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OnlineReserved4(void)=0;
   //--- trading accounts control
   virtual MTAPIRES  TradeAccountSet(const int64_t request_id,
                                     const IMTUser *user,
                                     const IMTAccount *account,
                                     const IMTOrderArray *orders,
                                     const IMTPositionArray *positions)=0;
   virtual MTAPIRES  TradeAccountReserved1(void)=0;
   virtual MTAPIRES  TradeAccountReserved2(void)=0;
   virtual MTAPIRES  TradeAccountReserved3(void)=0;
   virtual MTAPIRES  TradeAccountReserved4(void)=0;
   //--- notifications
   virtual MTAPIRES  NotificationsSend(LPWSTR metaquotes_ids,LPCWSTR message)=0;
   virtual MTAPIRES  NotificationsSend(const uint64_t* logins,const uint32_t logins_total,LPCWSTR message)=0;
   virtual MTAPIRES  NotificationsReserved1(void)=0;
   virtual MTAPIRES  NotificationsReserved2(void)=0;
   virtual MTAPIRES  NotificationsReserved3(void)=0;
   virtual MTAPIRES  NotificationsReserved4(void)=0;
   //--- settings
   virtual MTAPIRES  SettingGet(LPCWSTR section,LPCWSTR key,LPVOID& outdata,uint32_t& outdata_len)=0;
   virtual MTAPIRES  SettingSet(LPCWSTR section,LPCWSTR key,const LPVOID indata,const uint32_t indata_len)=0;
   virtual MTAPIRES  SettingDelete(LPCWSTR section,LPCWSTR key)=0;
   virtual MTAPIRES  SettingReserved1(void)=0;
   virtual MTAPIRES  SettingReserved2(void)=0;
   virtual MTAPIRES  SettingReserved3(void)=0;
   virtual MTAPIRES  SettingReserved4(void)=0;
   //--- chart data
   virtual MTAPIRES  ChartRequest(LPCWSTR symbol,const int64_t from,const int64_t to,MTChartBar*& bars,uint32_t& bars_total)=0;
   virtual MTAPIRES  ChartDelete(LPCWSTR symbol,const int64_t* bars_dates,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartUpdate(LPCWSTR symbol,const MTChartBar* bars,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartReplace(LPCWSTR symbol,const int64_t from,const int64_t to,const MTChartBar* bars,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartSplit(LPCWSTR symbol,const uint32_t new_shares,const uint32_t old_shares,const uint32_t rounding_rule,const int64_t datetime_from,const int64_t datetime_to)=0;
   virtual MTAPIRES  ChartReserved2(void)=0;
   virtual MTAPIRES  ChartReserved3(void)=0;
   //--- ticks data
   virtual MTAPIRES  TickHistoryRequestRaw(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickHistoryAdd(LPCWSTR symbol,const MTTickShort* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickHistoryReplace(LPCWSTR symbol,const int64_t from_msc,const int64_t to_msc,const MTTickShort* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickHistoryReserved1(void)=0;
   virtual MTAPIRES  TickHistoryReserved2(void)=0;
   virtual MTAPIRES  TickHistoryReserved3(void)=0;
   virtual MTAPIRES  TickHistoryReserved4(void)=0;
   //--- deals database
   virtual MTAPIRES  DealRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealUpdateBatch(IMTDealArray* deals,MTAPIRES* results)=0;
   virtual MTAPIRES  DealUpdateBatchArray(IMTDeal** deals,const uint32_t deals_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DealDeleteBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DealRequestPage(const uint64_t login,const int64_t from,const int64_t to,const uint32_t offset,const uint32_t total,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealAdd(IMTDeal* deal)=0;
   virtual MTAPIRES  DealAddBatch(IMTDealArray* deals,MTAPIRES* results)=0;
   virtual MTAPIRES  DealAddBatchArray(IMTDeal** deals,const uint32_t deals_total,MTAPIRES* results)=0;
   //--- positions database
   virtual MTAPIRES  PositionGetByLogins(const uint64_t *logins,const uint32_t logins_total,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionGetByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionDeleteByTicket(const uint64_t ticket)=0;
   virtual MTAPIRES  PositionRequestByGroup(LPCWSTR group,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionUpdateBatch(IMTPositionArray* positions,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionUpdateBatchArray(IMTPosition** positions,const uint32_t positions_total,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionDeleteBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionSplit(const uint64_t* tickets,const uint32_t tickets_total,const double *adjustments,const uint32_t new_shares,const uint32_t old_shares,const uint32_t round_rule_price,const uint32_t round_rule_volume,const uint32_t flags,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionGetBySymbol(LPCWSTR groups,LPCWSTR symbol,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,IMTPositionArray* positions)=0;
   //--- orders database
   virtual MTAPIRES  OrderGetByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequestByGroup(LPCWSTR group,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderUpdateBatch(IMTOrderArray* orders,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderUpdateBatchArray(IMTOrder** orders,const uint32_t orders_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderDeleteBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderGetBySymbol(LPCWSTR groups,LPCWSTR symbol,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderAdd(IMTOrder* order)=0;
   virtual MTAPIRES  OrderAddBatch(IMTOrderArray* orders,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderAddBatchArray(IMTOrder** orders,const uint32_t orders_total,MTAPIRES* results)=0;
   //--- orders history database
   virtual MTAPIRES  HistoryRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestPage(const uint64_t login,const int64_t from,const int64_t to,const uint32_t offset,const uint32_t total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryReserved4(void)=0;
   //--- email servers configuration
   virtual MTAPIRES  EmailSend(LPCWSTR account,LPCWSTR to,LPCWSTR to_name,LPCWSTR subject,LPCWSTR body)=0;
   virtual MTAPIRES  EmailReserved1(void)=0;
   virtual MTAPIRES  EmailReserved2(void)=0;
   virtual MTAPIRES  EmailReserved3(void)=0;
   virtual MTAPIRES  EmailReserved4(void)=0;
   //--- messengers configuration
   virtual MTAPIRES  MessengerSend(LPCWSTR destination,LPCWSTR group,LPCWSTR sender,LPCWSTR text)=0;
   virtual MTAPIRES  MessengerVerifyPhone(LPCWSTR phone_number)=0;
   virtual MTAPIRES  MessengerReserved2(void)=0;
   virtual MTAPIRES  MessengerReserved3(void)=0;
   virtual MTAPIRES  MessengerReserved4(void)=0;
   //--- ECN
   virtual IMTECNMatching* ECNCreateMatching(void)=0;
   virtual IMTECNMatchingArray* ECNCreateMatchingArray(void)=0;
   virtual IMTECNFilling* ECNCreateFilling(void)=0;
   virtual IMTECNFillingArray* ECNCreateFillingArray(void)=0;
   virtual IMTECNProvider* ECNCreateProvider(void)=0;
   virtual IMTECNProviderArray* ECNCreateProviderArray(void)=0;
   virtual IMTECNHistoryMatching* ECNCreateHistoryMatching(void)=0;
   virtual IMTECNHistoryMatchingArray* ECNCreateHistoryMatchingArray(void)=0;
   virtual IMTECNHistoryFilling* ECNCreateHistoryFilling(void)=0;
   virtual IMTECNHistoryFillingArray* ECNCreateHistoryFillingArray(void)=0;
   virtual IMTECNHistoryDeal* ECNCreateHistoryDeal(void)=0;
   virtual IMTECNHistoryDealArray* ECNCreateHistoryDealArray(void)=0;
   virtual MTAPIRES  ECNRequestByGroup(LPCWSTR mask,IMTECNMatchingArray* matching,IMTECNFillingArray* filling,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTECNMatchingArray* matching,IMTECNFillingArray* filling,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTECNMatchingArray* matching,IMTECNFillingArray* filling,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestHistoryByGroup(LPCWSTR mask,const int64_t from,const int64_t to,
                                              IMTECNHistoryMatchingArray* matching,IMTECNHistoryFillingArray* filling,IMTECNHistoryDealArray* deals,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestHistoryByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,
                                               IMTECNHistoryMatchingArray* matching,IMTECNHistoryFillingArray* filling,IMTECNHistoryDealArray* deals,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestHistoryByTickets(const uint64_t *tickets,const uint32_t tickets_total,const int64_t from,const int64_t to,
                                                IMTECNHistoryMatchingArray* matching,IMTECNHistoryFillingArray* filling,IMTECNHistoryDealArray* deals,IMTECNProviderArray* providers)=0;
   //--- client management
   virtual IMTClient *ClientCreate(void)=0;
   virtual IMTClientArray *ClientCreateArray(void)=0;
   virtual MTAPIRES  ClientAdd(IMTClient *client)=0;
   virtual MTAPIRES  ClientAddBatch(IMTClientArray* clients,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientAddBatchArray(IMTClient **clients,const uint32_t clients_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUpdate(IMTClient *client)=0;
   virtual MTAPIRES  ClientUpdateBatch(IMTClientArray* clients,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUpdateBatchArray(IMTClient **clients,const uint32_t clients_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientDelete(const uint64_t client_id)=0;
   virtual MTAPIRES  ClientDeleteBatch(const uint64_t *client_ids,const uint32_t client_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientRequest(const uint64_t client_id,IMTClient *client)=0;
   virtual MTAPIRES  ClientRequestByGroup(LPCWSTR groups,IMTClientArray *clients)=0;
   virtual MTAPIRES  ClientRequestHistory(const uint64_t client_id,const uint64_t author,const int64_t from,const int64_t to,IMTClientArray *history)=0;
   virtual MTAPIRES  ClientUserAdd(const uint64_t client_id,const uint64_t login)=0;
   virtual MTAPIRES  ClientUserAddBatch(const uint64_t client_id,const uint64_t *logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUserDelete(const uint64_t client_id,const uint64_t login)=0;
   virtual MTAPIRES  ClientUserDeleteBatch(const uint64_t client_id,const uint64_t *logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUserRequest(const uint64_t client_id,uint64_t*& logins,uint32_t& logins_total)=0;
   virtual MTAPIRES  ClientReserved1(void)=0;
   virtual MTAPIRES  ClientReserved2(void)=0;
   virtual MTAPIRES  ClientReserved3(void)=0;
   virtual MTAPIRES  ClientReserved4(void)=0;
   //--- document management
   virtual IMTDocument *DocumentCreate(void)=0;
   virtual IMTDocumentArray *DocumentCreateArray(void)=0;
   virtual MTAPIRES  DocumentAdd(IMTDocument *document)=0;
   virtual MTAPIRES  DocumentAddBatch(IMTDocumentArray *documents,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentAddBatchArray(IMTDocument** documents,const uint32_t documents_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentUpdate(IMTDocument *document)=0;
   virtual MTAPIRES  DocumentUpdateBatch(IMTDocumentArray *documents,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentUpdateBatchArray(IMTDocument** documents,const uint32_t documents_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentDelete(const uint64_t document_id)=0;
   virtual MTAPIRES  DocumentDeleteBatch(const uint64_t *document_ids,const uint32_t document_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentRequest(const uint64_t document_id,IMTDocument *document)=0;
   virtual MTAPIRES  DocumentRequestByClient(const uint64_t client_id,IMTDocumentArray *documents)=0;
   virtual MTAPIRES  DocumentRequestHistory(const uint64_t document_id,const uint64_t author,const int64_t from,const int64_t to,IMTDocumentArray *history)=0;
   virtual MTAPIRES  DocumentReserved1(void)=0;
   virtual MTAPIRES  DocumentReserved2(void)=0;
   virtual MTAPIRES  DocumentReserved3(void)=0;
   virtual MTAPIRES  DocumentReserved4(void)=0;
   //--- comment management
   virtual IMTComment *CommentCreate(void)=0;
   virtual IMTCommentArray *CommentCreateArray(void)=0;
   virtual MTAPIRES  CommentAdd(IMTComment *comment)=0;
   virtual MTAPIRES  CommentAddBatch(IMTCommentArray *comments,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentAddBatchArray(IMTComment **comments,const uint32_t comments_total,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentUpdate(IMTComment *comment)=0;
   virtual MTAPIRES  CommentUpdateBatch(IMTCommentArray *comments,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentUpdateBatchArray(IMTComment **comments,const uint32_t comments_total,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentDelete(const uint64_t comment_id)=0;
   virtual MTAPIRES  CommentDeleteBatch(const uint64_t *comment_ids,const uint32_t comment_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentRequest(const uint64_t comment_id,IMTComment *comment)=0;
   virtual MTAPIRES  CommentRequestByClient(const uint64_t client_id,const uint32_t position,const uint32_t total,IMTCommentArray *comments)=0;
   virtual MTAPIRES  CommentRequestByDocument(const uint64_t document_id,const uint32_t position,const uint32_t total,IMTCommentArray *comments)=0;
   virtual MTAPIRES  CommentReserved1(void)=0;
   virtual MTAPIRES  CommentReserved2(void)=0;
   virtual MTAPIRES  CommentReserved3(void)=0;
   virtual MTAPIRES  CommentReserved4(void)=0;
   //--- attachment management
   virtual IMTAttachment* AttachmentCreate(void)=0;
   virtual IMTAttachmentArray* AttachmentCreateArray(void)=0;
   virtual MTAPIRES  AttachmentAdd(IMTAttachment *attachment)=0;
   virtual MTAPIRES  AttachmentAddBatch(IMTAttachmentArray* attachments,MTAPIRES* results)=0;
   virtual MTAPIRES  AttachmentAddBatchArray(IMTAttachment **attachments,const uint32_t attachments_total,MTAPIRES* results)=0;
   virtual MTAPIRES  AttachmentRequest(const uint64_t *attachment_ids,const uint32_t attachment_ids_total,IMTAttachmentArray *attachments)=0;
   virtual MTAPIRES  AttachmentReserved1(void)=0;
   virtual MTAPIRES  AttachmentReserved2(void)=0;
   virtual MTAPIRES  AttachmentReserved3(void)=0;
   virtual MTAPIRES  AttachmentReserved4(void)=0;
   //--- trade accounts sinks
   virtual MTAPIRES  UserAccountSubscribe(IMTAccountSink* sink)=0;
   virtual MTAPIRES  UserAccountUnsubscribe(IMTAccountSink* sink)=0;
   virtual MTAPIRES  UserRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTUserArray* users)=0;
   virtual MTAPIRES  UserGetByGroup(LPCWSTR mask,IMTUserArray* users)=0;
   virtual MTAPIRES  UserGetByLogins(const uint64_t *logins,const uint32_t logins_total,IMTUserArray* users)=0;
   virtual MTAPIRES  UserUpdateBatch(IMTUserArray* users,MTAPIRES* results)=0;
   virtual MTAPIRES  UserUpdateBatchArray(IMTUser** users,const uint32_t users_total,MTAPIRES* results)=0;
   //--- subscription configuration
   virtual IMTConSubscription* SubscriptionCfgCreate()=0;
   virtual IMTConSubscriptionSymbol* SubscriptionCfgSymbolCreate()=0;
   virtual IMTConSubscriptionNews* SubscriptionCfgNewsCreate()=0;
   virtual MTAPIRES  SubscriptionCfgSubscribe(IMTConSubscriptionSink* sink)=0;
   virtual MTAPIRES  SubscriptionCfgUnsubscribe(IMTConSubscriptionSink* sink)=0;
   virtual uint32_t  SubscriptionCfgTotal(void)=0;
   virtual MTAPIRES  SubscriptionCfgNext(const uint32_t pos,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgGet(LPCWSTR name,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgGetByID(const uint64_t id,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgRequest(LPCWSTR name,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgRequestByID(const uint64_t id,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgReserved1(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved2(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved3(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved4(void)=0;
   //--- subscription records
   virtual IMTSubscription* SubscriptionCreate()=0;
   virtual IMTSubscriptionArray* SubscriptionCreateArray(void)=0;
   virtual MTAPIRES  SubscriptionJoin(const uint64_t login,const uint64_t subscription,IMTSubscription* record,IMTSubscriptionHistory* history)=0;
   virtual MTAPIRES  SubscriptionJoinBatch(const uint64_t* logins,const uint64_t* subscriptions,const uint32_t total,MTAPIRES* results,IMTSubscriptionArray* records,IMTSubscriptionHistoryArray* history)=0;
   virtual MTAPIRES  SubscriptionCancel(const uint64_t login,const uint64_t subscription,IMTSubscription* record,IMTSubscriptionHistory* history)=0;
   virtual MTAPIRES  SubscriptionCancelBatch(const uint64_t* logins,const uint64_t* subscriptions,const uint32_t total,MTAPIRES* results,IMTSubscriptionArray* records,IMTSubscriptionHistoryArray* history)=0;
   virtual MTAPIRES  SubscriptionUpdate(IMTSubscription* subscription)=0;
   virtual MTAPIRES  SubscriptionUpdateBatch(IMTSubscriptionArray* records,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionUpdateBatchArray(IMTSubscription** records,const uint32_t records_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionDelete(const uint64_t id)=0;
   virtual MTAPIRES  SubscriptionDeleteBatch(const uint64_t* ids,const uint32_t ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionRequest(const uint64_t login,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionRequestByID(const uint64_t id,IMTSubscription* record)=0;
   virtual MTAPIRES  SubscriptionRequestByGroup(LPCWSTR group,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionRequestByIDs(const uint64_t *ids,const uint32_t ids_total,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionReserved1(void)=0;
   virtual MTAPIRES  SubscriptionReserved2(void)=0;
   virtual MTAPIRES  SubscriptionReserved3(void)=0;
   virtual MTAPIRES  SubscriptionReserved4(void)=0;
   //--- subscription history records
   virtual IMTSubscriptionHistory* SubscriptionHistoryCreate()=0;
   virtual IMTSubscriptionHistoryArray* SubscriptionHistoryCreateArray(void)=0;
   virtual MTAPIRES  SubscriptionHistoryUpdate(IMTSubscriptionHistory* subscription)=0;
   virtual MTAPIRES  SubscriptionHistoryUpdateBatch(IMTSubscriptionHistoryArray* records,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionHistoryUpdateBatchArray(IMTSubscriptionHistory** records,const uint32_t records_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionHistoryDelete(const uint64_t id)=0;
   virtual MTAPIRES  SubscriptionHistoryDeleteBatch(const uint64_t* ids,const uint32_t ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionHistoryRequest(const uint64_t login,const int64_t from,const int64_t to,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByID(const uint64_t id,IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByIDs(const uint64_t *ids,const uint32_t ids_total,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved1(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved2(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved3(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved4(void)=0;
   //--- deals database
   virtual MTAPIRES  DealPerform(IMTDeal* deal)=0;
   virtual MTAPIRES  DealPerformBatch(IMTDealArray* deals,MTAPIRES* results)=0;
   virtual MTAPIRES  DealPerformBatchArray(IMTDeal** deals,const uint32_t deals_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DealRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealReserved3(void)=0;
   virtual MTAPIRES  DealReserved4(void)=0;
   //--- users database
   virtual MTAPIRES  UserDeleteBatch(const uint64_t* logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  UserAccountRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTAccountArray *accounts)=0;
   virtual MTAPIRES  UserBalanceCheckBatch(const uint64_t* logins,const uint32_t logins_total,const uint32_t fixflag,MTAPIRES* results,double* balance_user,double* balance_history,double* credit_user,double* credit_history)=0;
   virtual MTAPIRES  UserAccountGetByGroup(LPCWSTR mask,IMTAccountArray* accounts)=0;
   virtual MTAPIRES  UserAccountGetByLogins(const uint64_t *logins,const uint32_t logins_total,IMTAccountArray* accounts)=0;
   virtual MTAPIRES  UserReserved5(void)=0;
   virtual MTAPIRES  UserReserved6(void)=0;
   virtual MTAPIRES  UserReserved7(void)=0;
   //--- trade orders database
   virtual MTAPIRES  OrderRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderCancel(const uint64_t ticket)=0;
   virtual MTAPIRES  OrderCancelBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderReserved3(void)=0;
   virtual MTAPIRES  OrderReserved4(void)=0;
   //--- clients group configuration
   virtual IMTConLeverage* LeverageCreate(void)=0;
   virtual IMTConLeverageArray* LeverageCreateArray(void)=0;
   virtual IMTConLeverageRule* LeverageRuleCreate(void)=0;
   virtual IMTConLeverageTier* LeverageTierCreate(void)=0;
   virtual MTAPIRES  LeverageSubscribe(IMTConLeverageSink* sink)=0;
   virtual MTAPIRES  LeverageUnsubscribe(IMTConLeverageSink* sink)=0;
   virtual uint32_t  LeverageTotal(void)=0;
   virtual MTAPIRES  LeverageNext(const uint32_t pos,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageGet(LPCWSTR name,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageRequest(LPCWSTR name,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageRequestArray(LPCWSTR groups_mask,IMTConLeverageArray* leverages)=0;
   virtual MTAPIRES  LeverageReserved1(void)=0;
   virtual MTAPIRES  LeverageReserved2(void)=0;
   virtual MTAPIRES  LeverageReserved3(void)=0;
   virtual MTAPIRES  LeverageReserved4(void)=0;
   //--- geodata
   virtual IMTGeo*   GeoCreate(void)=0;
   virtual MTAPIRES  GeoResolve(LPCWSTR address,const uint32_t flags,IMTGeo* record)=0;
   virtual MTAPIRES  GeoResolveBatch(LPCWSTR* addresses,uint32_t addresses_total,const uint32_t flags,IMTGeo** records,MTAPIRES* results)=0;
   virtual MTAPIRES  GeoResolveReserved1(void)=0;
   virtual MTAPIRES  GeoResolveReserved2(void)=0;
   virtual MTAPIRES  GeoResolveReserved3(void)=0;
   virtual MTAPIRES  GeoResolveReserved4(void)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTManagerAPI(void) {}
  };
//+------------------------------------------------------------------+
//| Administrator API interface                                      |
//+------------------------------------------------------------------+
class IMTAdminAPI
  {
public:
   //--- pumping modes
   enum EnPumpModes
     {
      PUMP_MODE_MAIL          =0x00000004,   // pump mails
      PUMP_MODE_NEWS          =0x00000020,   // pump news
      //--- enumeration ranges
      PUMP_MODE_FULL          =0xffffffff    // full pumping
     };
   //--- external sync modes
   enum EnExternalSyncModes
     {
      EXTERNAL_SYNC_ALL      =0,
      EXTERNAL_SYNC_BALANCE  =1,
      EXTERNAL_SYNC_POSITIONS=2,
      EXTERNAL_SYNC_ORDERS   =3,
      EXTERNAL_SYNC_LAST     =EXTERNAL_SYNC_ORDERS
     };
   //--- interface release
   virtual void      Release(void)=0;
   //--- Manager license check
   virtual MTAPIRES  LicenseCheck(MTLicenseCheck& check)=0;
   //--- memory management
   virtual void*     Allocate(const uint32_t bytes)=0;
   virtual void      Free(void* ptr)=0;
   //--- proxy
   virtual void      ProxySet(const MTProxyInfo &proxy)=0;
   //--- connect/disconnect
   virtual MTAPIRES  Connect(LPCWSTR server,uint64_t login,LPCWSTR password,LPCWSTR password_cert,uint64_t pump_mode,uint32_t timeout=INFINITE)=0;
   virtual void      Disconnect(void)=0;
   virtual MTAPIRES  Subscribe(IMTManagerSink* sink)=0;
   virtual MTAPIRES  Unsubscribe(IMTManagerSink* sink)=0;
   //--- server management
   virtual MTAPIRES  ServerActivate(void)=0;
   virtual MTAPIRES  ServerLiveUpdate(void)=0;
   //--- API journal
   virtual MTAPIRES  LoggerOut(const uint32_t code,LPCWSTR format,...)=0;
   virtual void      LoggerFlush(void)=0;
   virtual MTAPIRES  LoggerServerRequest(const uint64_t server_id,const uint32_t mode,const uint32_t type,
                                         const int64_t from,const int64_t to,LPCWSTR filter,
                                         MTLogRecord*& records,uint32_t& records_total)=0;
   virtual MTAPIRES  LoggerGatewayRequest(const uint32_t gateway_pos,const int64_t from,const int64_t to,LPCWSTR filter,
                                          MTLogRecord*& records,uint32_t& records_total)=0;
   virtual MTAPIRES  LoggerFeederRequest(const uint32_t feeder_pos,const int64_t from,const int64_t to,LPCWSTR filter,
                                         MTLogRecord*& records,uint32_t& records_total)=0;
   virtual MTAPIRES  LoggerOutString(const uint32_t code,LPCWSTR string)=0;
   virtual MTAPIRES  LoggerReserved2(void)=0;
   virtual MTAPIRES  LoggerReserved3(void)=0;
   virtual MTAPIRES  LoggerReserved4(void)=0;
   //--- common configuration
   virtual IMTConCommon* CommonCreate(void)=0;
   virtual MTAPIRES  CommonSubscribe(IMTConCommonSink* sink)=0;
   virtual MTAPIRES  CommonUnsubscribe(IMTConCommonSink* sink)=0;
   virtual MTAPIRES  CommonGet(IMTConCommon* common)=0;
   virtual MTAPIRES  CommonSet(IMTConCommon* common)=0;
   virtual IMTConAccountAllocation* CommonCreateAllocation(void)=0;
   virtual IMTConAccountAgreement* CommonCreateAgreement(void)=0;
   virtual MTAPIRES  CommonReserved3(void)=0;
   virtual MTAPIRES  CommonReserved4(void)=0;
   //--- platform server configuration
   virtual IMTConServer* NetServerCreate(void)=0;
   virtual IMTConServerRange* NetServerRangeCreate(void)=0;
   virtual MTAPIRES  NetServerSubscribe(IMTConServerSink* sink)=0;
   virtual MTAPIRES  NetServerUnsubscribe(IMTConServerSink* sink)=0;
   virtual MTAPIRES  NetServerRestart(const uint64_t id)=0;
   virtual MTAPIRES  NetServerUpdate(IMTConServer* config)=0;
   virtual MTAPIRES  NetServerDelete(const uint32_t pos)=0;
   virtual MTAPIRES  NetServerShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  NetServerTotal(void)=0;
   virtual MTAPIRES  NetServerNext(const uint32_t pos,IMTConServer* config)=0;
   virtual MTAPIRES  NetServerGet(const uint64_t id,IMTConServer* config)=0;
   virtual MTAPIRES  NetServerUpdateBatch(IMTConServer** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  NetServerDeleteBatch(IMTConServer** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual IMTConAddressRange* NetServerAddressRangeCreate(void)=0;
   virtual IMTConClusterState* NetServerClusterStateCreate(void)=0;
   //--- server firewall configuration
   virtual IMTConFirewall* FirewallCreate()=0;
   virtual MTAPIRES  FirewallSubscribe(IMTConFirewallSink* sink)=0;
   virtual MTAPIRES  FirewallUnsubscribe(IMTConFirewallSink* sink)=0;
   virtual MTAPIRES  FirewallUpdate(IMTConFirewall* config)=0;
   virtual MTAPIRES  FirewallDelete(const uint32_t pos)=0;
   virtual MTAPIRES  FirewallShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  FirewallTotal(void)=0;
   virtual MTAPIRES  FirewallNext(const uint32_t pos,IMTConFirewall* config)=0;
   virtual MTAPIRES  FirewallUpdateBatch(IMTConFirewall** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  FirewallDeleteBatch(IMTConFirewall** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  FirewallReserved3(void)=0;
   virtual MTAPIRES  FirewallReserved4(void)=0;
   //--- time configuration
   virtual IMTConTime* TimeCreate(void)=0;
   virtual MTAPIRES  TimeSubscribe(IMTConTimeSink* sink)=0;
   virtual MTAPIRES  TimeUnsubscribe(IMTConTimeSink* sink)=0;
   virtual MTAPIRES  TimeGet(IMTConTime* config)=0;
   virtual MTAPIRES  TimeSet(IMTConTime* config)=0;
   virtual int64_t   TimeServer(void)=0;
   virtual MTAPIRES  TimeServerRequest(int64_t& time_msc)=0;
   virtual MTAPIRES  TimeReserved2(void)=0;
   virtual MTAPIRES  TimeReserved3(void)=0;
   virtual MTAPIRES  TimeReserved4(void)=0;
   //--- holidays configuration
   virtual IMTConHoliday* HolidayCreate()=0;
   virtual MTAPIRES  HolidaySubscribe(IMTConHolidaySink* sink)=0;
   virtual MTAPIRES  HolidayUnsubscribe(IMTConHolidaySink* sink)=0;
   virtual MTAPIRES  HolidayUpdate(IMTConHoliday* config)=0;
   virtual MTAPIRES  HolidayDelete(const uint32_t pos)=0;
   virtual MTAPIRES  HolidayShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  HolidayTotal(void)=0;
   virtual MTAPIRES  HolidayNext(const uint32_t pos,IMTConHoliday* config)=0;
   virtual MTAPIRES  HolidayUpdateBatch(IMTConHoliday** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  HolidayDeleteBatch(IMTConHoliday** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  HolidayReserved3(void)=0;
   virtual MTAPIRES  HolidayReserved4(void)=0;
   //--- clients group configuration
   virtual IMTConGroup* GroupCreate(void)=0;
   virtual IMTConGroupSymbol* GroupSymbolCreate(void)=0;
   virtual IMTConCommission* GroupCommissionCreate(void)=0;
   virtual IMTConCommTier* GroupTierCreate(void)=0;
   virtual MTAPIRES  GroupSubscribe(IMTConGroupSink* sink)=0;
   virtual MTAPIRES  GroupUnsubscribe(IMTConGroupSink* sink)=0;
   virtual MTAPIRES  GroupUpdate(IMTConGroup* group)=0;
   virtual MTAPIRES  GroupDelete(LPCWSTR name)=0;
   virtual MTAPIRES  GroupDelete(const uint32_t pos)=0;
   virtual MTAPIRES  GroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  GroupTotal(void)=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConGroup* group)=0;
   virtual MTAPIRES  GroupGet(LPCWSTR name,IMTConGroup* group)=0;
   virtual MTAPIRES  GroupUpdateBatch(IMTConGroup** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  GroupDeleteBatch(IMTConGroup** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  GroupReserved3(void)=0;
   virtual MTAPIRES  GroupReserved4(void)=0;
   //--- managers configuration
   virtual IMTConManager* ManagerCreate(void)=0;
   virtual IMTConManagerAccess* ManagerAccessCreate(void)=0;
   virtual MTAPIRES  ManagerSubscribe(IMTConManagerSink* sink)=0;
   virtual MTAPIRES  ManagerUnsubscribe(IMTConManagerSink* sink)=0;
   virtual MTAPIRES  ManagerUpdate(IMTConManager* manager)=0;
   virtual MTAPIRES  ManagerDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ManagerShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ManagerTotal(void)=0;
   virtual MTAPIRES  ManagerNext(const uint32_t pos,IMTConManager* manager)=0;
   virtual MTAPIRES  ManagerGet(const uint64_t login,IMTConManager* manager)=0;
   virtual MTAPIRES  ManagerUpdateBatch(IMTConManager** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ManagerDeleteBatch(IMTConManager** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ManagerCurrent(IMTConManager* manager)=0;
   virtual IMTConManagerReport* ManagerReportCreate(void)=0;
   //--- routing configuration
   virtual IMTConRoute* RouteCreate(void)=0;
   virtual IMTConCondition* RouteConditionCreate(void)=0;
   virtual IMTConRouteDealer* RouteDealerCreate(void)=0;
   virtual MTAPIRES  RouteSubscribe(IMTConRouteSink* sink)=0;
   virtual MTAPIRES  RouteUnsubscribe(IMTConRouteSink* sink)=0;
   virtual MTAPIRES  RouteUpdate(IMTConRoute* route)=0;
   virtual MTAPIRES  RouteDelete(LPCWSTR name)=0;
   virtual MTAPIRES  RouteDelete(const uint32_t pos)=0;
   virtual MTAPIRES  RouteShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  RouteTotal(void)=0;
   virtual MTAPIRES  RouteNext(const uint32_t pos,IMTConRoute* route)=0;
   virtual MTAPIRES  RouteGet(LPCWSTR name,IMTConRoute* route)=0;
   virtual MTAPIRES  RouteUpdateBatch(IMTConRoute** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  RouteDeleteBatch(IMTConRoute** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  RouteReserved3(void)=0;
   virtual MTAPIRES  RouteReserved4(void)=0;
   //--- gateways configuration
   virtual IMTConGateway* GatewayCreate(void)=0;
   virtual IMTConGatewayModule* GatewayModuleCreate(void)=0;
   virtual IMTConParam* GatewayParamCreate(void)=0;
   virtual IMTConGatewayTranslate* GatewayTranslateCreate(void)=0;
   virtual MTAPIRES  GatewaySubscribe(IMTConGatewaySink* sink)=0;
   virtual MTAPIRES  GatewayUnsubscribe(IMTConGatewaySink* sink)=0;
   virtual MTAPIRES  GatewayRestart(void)=0;
   virtual MTAPIRES  GatewayUpdate(IMTConGateway* gateway)=0;
   virtual MTAPIRES  GatewayDelete(LPCWSTR name)=0;
   virtual MTAPIRES  GatewayDelete(const uint32_t pos)=0;
   virtual MTAPIRES  GatewayShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  GatewayTotal(void)=0;
   virtual MTAPIRES  GatewayNext(const uint32_t pos,IMTConGateway* gateway)=0;
   virtual MTAPIRES  GatewayGet(LPCWSTR name,IMTConGateway* gateway)=0;
   virtual uint32_t  GatewayModuleTotal(void)=0;
   virtual MTAPIRES  GatewayModuleNext(const uint32_t pos,IMTConGatewayModule* module)=0;
   virtual MTAPIRES  GatewayModuleGet(LPCWSTR name,IMTConGatewayModule* module)=0;
   virtual MTAPIRES  GatewayPositionRequest(uint64_t gateway_id,IMTPositionArray *positions,int64_t &positions_time)=0;
   virtual MTAPIRES  GatewayUpdateBatch(IMTConGateway** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  GatewayDeleteBatch(IMTConGateway** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  GatewayReserved3(void)=0;
   //--- plug-ins configuration
   virtual IMTConPlugin* PluginCreate(void)=0;
   virtual IMTConPluginModule* PluginModuleCreate(void)=0;
   virtual IMTConParam* PluginParamCreate(void)=0;
   virtual MTAPIRES  PluginSubscribe(IMTConPluginSink* sink)=0;
   virtual MTAPIRES  PluginUnsubscribe(IMTConPluginSink* sink)=0;
   virtual MTAPIRES  PluginUpdate(IMTConPlugin* plugin)=0;
   virtual MTAPIRES  PluginDelete(const uint64_t server_id,LPCWSTR name)=0;
   virtual MTAPIRES  PluginDelete(const uint32_t pos)=0;
   virtual MTAPIRES  PluginShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  PluginTotal(void)=0;
   virtual MTAPIRES  PluginNext(const uint32_t pos,IMTConPlugin* plugin)=0;
   virtual MTAPIRES  PluginGet(const uint64_t server_id,LPCWSTR name,IMTConPlugin* plugin)=0;
   virtual uint32_t  PluginModuleTotal(void)=0;
   virtual MTAPIRES  PluginModuleNext(const uint32_t pos,IMTConPluginModule* module)=0;
   virtual MTAPIRES  PluginModuleGet(const uint64_t server_id,LPCWSTR name,IMTConPluginModule* module)=0;
   virtual MTAPIRES  PluginUpdateBatch(IMTConPlugin** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  PluginDeleteBatch(IMTConPlugin** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  PluginReserved3(void)=0;
   virtual MTAPIRES  PluginReserved4(void)=0;
   //--- data feeds configuration
   virtual IMTConFeeder* FeederCreate(void)=0;
   virtual IMTConFeederModule* FeederModuleCreate(void)=0;
   virtual IMTConParam* FeederParamCreate(void)=0;
   virtual IMTConFeederTranslate* FeederTranslateCreate(void)=0;
   virtual MTAPIRES  FeederSubscribe(IMTConFeederSink* sink)=0;
   virtual MTAPIRES  FeederUnsubscribe(IMTConFeederSink* sink)=0;
   virtual MTAPIRES  FeederRestart(void)=0;
   virtual MTAPIRES  FeederUpdate(IMTConFeeder* feeder)=0;
   virtual MTAPIRES  FeederDelete(LPCWSTR name)=0;
   virtual MTAPIRES  FeederDelete(const uint32_t pos)=0;
   virtual MTAPIRES  FeederShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  FeederTotal(void)=0;
   virtual MTAPIRES  FeederNext(const uint32_t pos,IMTConFeeder* feeder)=0;
   virtual MTAPIRES  FeederGet(LPCWSTR name,IMTConFeeder* feeder)=0;
   virtual uint32_t  FeederModuleTotal(void)=0;
   virtual MTAPIRES  FeederModuleNext(const uint32_t pos,IMTConFeederModule* module)=0;
   virtual MTAPIRES  FeederModuleGet(LPCWSTR name,IMTConFeederModule* module)=0;
   virtual MTAPIRES  FeederUpdateBatch(IMTConFeeder** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  FeederDeleteBatch(IMTConFeeder** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  FeederReserved3(void)=0;
   virtual MTAPIRES  FeederReserved4(void)=0;
   //--- report configuration
   virtual IMTConReport* ReportCreate(void)=0;
   virtual IMTConReportModule* ReportModuleCreate(void)=0;
   virtual IMTConParam* ReportParamCreate(void)=0;
   virtual MTAPIRES  ReportSubscribe(IMTConReportSink* sink)=0;
   virtual MTAPIRES  ReportUnsubscribe(IMTConReportSink* sink)=0;
   virtual MTAPIRES  ReportUpdate(IMTConReport* report)=0;
   virtual MTAPIRES  ReportDelete(const uint64_t server_id,LPCWSTR name)=0;
   virtual MTAPIRES  ReportDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ReportShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ReportTotal(void)=0;
   virtual MTAPIRES  ReportNext(const uint32_t pos,IMTConReport* report)=0;
   virtual MTAPIRES  ReportGet(const uint64_t server_id,LPCWSTR name,IMTConReport* report)=0;
   virtual uint32_t  ReportModuleTotal(void)=0;
   virtual MTAPIRES  ReportModuleNext(const uint32_t pos,IMTConReportModule* module)=0;
   virtual MTAPIRES  ReportModuleGet(const uint64_t server_id,LPCWSTR name,IMTConReportModule* module)=0;
   virtual MTAPIRES  ReportUpdateBatch(IMTConReport** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ReportDeleteBatch(IMTConReport** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ReportReserved3(void)=0;
   virtual MTAPIRES  ReportReserved4(void)=0;
   //--- symbols configuration
   virtual IMTConSymbol* SymbolCreate(void)=0;
   virtual IMTConSymbolSession* SymbolSessionCreate(void)=0;
   virtual MTAPIRES  SymbolSubscribe(IMTConSymbolSink* sink)=0;
   virtual MTAPIRES  SymbolUnsubscribe(IMTConSymbolSink* sink)=0;
   virtual MTAPIRES  SymbolUpdate(IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolDelete(LPCWSTR name)=0;
   virtual MTAPIRES  SymbolDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SymbolShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SymbolTotal(void)=0;
   virtual MTAPIRES  SymbolNext(const uint32_t pos,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,IMTConSymbol* symbol)=0;
   virtual MTAPIRES  SymbolExist(const IMTConSymbol* symbol,const IMTConGroup* group)=0;
   virtual MTAPIRES  SymbolUpdateBatch(IMTConSymbol** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SymbolDeleteBatch(IMTConSymbol** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SymbolReserved3(void)=0;
   virtual MTAPIRES  SymbolReserved4(void)=0;
   //--- history synchronization configuration
   virtual IMTConHistorySync* HistorySyncCreate(void)=0;
   virtual MTAPIRES  HistorySyncSubscribe(IMTConHistorySyncSink* sink)=0;
   virtual MTAPIRES  HistorySyncUnsubscribe(IMTConHistorySyncSink* sink)=0;
   virtual MTAPIRES  HistorySyncStart(void)=0;
   virtual MTAPIRES  HistorySyncUpdate(IMTConHistorySync* config)=0;
   virtual MTAPIRES  HistorySyncDelete(const uint32_t pos)=0;
   virtual MTAPIRES  HistorySyncShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  HistorySyncTotal(void)=0;
   virtual MTAPIRES  HistorySyncNext(const uint32_t pos,IMTConHistorySync* config)=0;
   virtual MTAPIRES  HistorySyncUpdateBatch(IMTConHistorySync** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  HistorySyncDeleteBatch(IMTConHistorySync** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  HistorySyncReserved3(void)=0;
   virtual MTAPIRES  HistorySyncReserved4(void)=0;
   //--- users database
   virtual IMTUser*  UserCreate(void)=0;
   virtual IMTUserArray* UserCreateArray(void)=0;
   virtual MTAPIRES  UserAdd(IMTUser* user,LPCWSTR master_pass,LPCWSTR investor_pass)=0;
   virtual MTAPIRES  UserDelete(const uint64_t login)=0;
   virtual MTAPIRES  UserUpdate(IMTUser* user)=0;
   virtual MTAPIRES  UserRequest(const uint64_t login,IMTUser *user)=0;
   virtual MTAPIRES  UserPasswordCheck(const uint32_t type,const uint64_t login,LPCWSTR password)=0;
   virtual MTAPIRES  UserPasswordChange(const uint32_t type,const uint64_t login,LPCWSTR password)=0;
   virtual MTAPIRES  UserCertDelete(const uint64_t login)=0;
   virtual MTAPIRES  UserCertConfirm(const uint64_t login)=0;
   virtual MTAPIRES  UserExternalSync(const uint64_t login,const uint64_t gateway_id,LPCWSTR account_id,uint32_t sync_mode)=0;
   virtual MTAPIRES  UserArchive(const uint64_t login)=0;
   virtual MTAPIRES  UserArchiveRequest(const uint64_t login,IMTUser* user)=0;
   virtual MTAPIRES  UserBackupRequest(const int64_t backup,const uint64_t login,IMTUser* user)=0;
   virtual MTAPIRES  UserBackupList(const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  UserRestore(IMTUser* user)=0;
   virtual IMTCertificate *UserCertCreate(void)=0;
   virtual MTAPIRES  UserCertUpdate(const uint64_t login,const IMTCertificate *certificate)=0;
   virtual MTAPIRES  UserCertGet(const uint64_t login,IMTCertificate *certificate)=0;
   virtual MTAPIRES  UserBalanceCheck(const uint64_t login,const uint32_t fixflag,double& balance_user,double& balance_history,double& credit_user,double& credit_history)=0;
   //--- deals database
   virtual IMTDeal*  DealCreate(void)=0;
   virtual IMTDealArray* DealCreateArray(void)=0;
   virtual MTAPIRES  DealRequest(const uint64_t ticket,IMTDeal* deal)=0;
   virtual MTAPIRES  DealRequest(const uint64_t login,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealDelete(const uint64_t ticket)=0;
   virtual MTAPIRES  DealUpdate(IMTDeal* deal)=0;
   virtual MTAPIRES  DealBackupList(const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  DealBackupRequest(const int64_t backup,const uint64_t ticket,IMTDeal* deal)=0;
   virtual MTAPIRES  DealBackupRequest(const int64_t backup,const uint64_t login,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealBackupRestore(IMTDeal* deal)=0;
   virtual MTAPIRES  DealBackupList(const uint64_t server,const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  DealBackupRequest(const uint64_t server,const int64_t backup,const uint64_t ticket,IMTDeal* deal)=0;
   virtual MTAPIRES  DealBackupRequest(const uint64_t server,const int64_t backup,const uint64_t login,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   //--- trade positions database
   virtual IMTPosition* PositionCreate(void)=0;
   virtual IMTPositionArray* PositionCreateArray(void)=0;
   virtual MTAPIRES  PositionRequest(const uint64_t login,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionDelete(IMTPosition* position)=0;
   virtual MTAPIRES  PositionUpdate(IMTPosition* position)=0;
   virtual MTAPIRES  PositionBackupList(const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  PositionBackupRequest(const int64_t backup,const uint64_t login,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionBackupRestore(IMTPosition* position)=0;
   virtual MTAPIRES  PositionBackupList(const uint64_t server,const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  PositionBackupRequest(const uint64_t server,const int64_t backup,const uint64_t login,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionCheck(const uint64_t login,IMTPositionArray* current,IMTPositionArray* invalid,IMTPositionArray* missed,IMTPositionArray* nonexist)=0;
   virtual MTAPIRES  PositionFix(const uint64_t login,IMTPositionArray* current)=0;
   //--- trade orders database
   virtual IMTOrder* OrderCreate(void)=0;
   virtual IMTOrderArray* OrderCreateArray(void)=0;
   virtual MTAPIRES  OrderRequest(const uint64_t ticket,IMTOrder* order)=0;
   virtual MTAPIRES  OrderRequestOpen(const uint64_t login,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequest(const uint64_t login,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderDelete(const uint64_t ticket)=0;
   virtual MTAPIRES  OrderUpdate(IMTOrder *order)=0;
   virtual MTAPIRES  OrderBackupList(const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  OrderBackupRequest(const int64_t backup,const uint64_t ticket,IMTOrder* order)=0;
   virtual MTAPIRES  OrderBackupRequestOpen(const int64_t backup,const uint64_t login,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderBackupRequestHistory(const int64_t backup,const uint64_t login,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderBackupRestore(IMTOrder* order)=0;
   virtual MTAPIRES  OrderBackupList(const uint64_t server,const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  OrderBackupRequest(const uint64_t server,const int64_t backup,const uint64_t ticket,IMTOrder* order)=0;
   virtual MTAPIRES  OrderBackupRequestOpen(const uint64_t server,const int64_t backup,const uint64_t login,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderBackupRequestHistory(const uint64_t server,const int64_t backup,const uint64_t login,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   //--- ticks data
   virtual MTAPIRES  TickRequest(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickRequestRaw(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickAdd(LPCWSTR symbol,const MTTickShort* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickReplace(LPCWSTR symbol,const int64_t from_msc,const int64_t to_msc,const MTTickShort* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickReserved3(void)=0;
   virtual MTAPIRES  TickReserved4(void)=0;
   //--- chart data
   virtual MTAPIRES  ChartRequest(LPCWSTR symbol,const int64_t from,const int64_t to,MTChartBar*& bars,uint32_t& bars_total)=0;
   virtual MTAPIRES  ChartDelete(LPCWSTR symbol,const int64_t* bars_dates,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartUpdate(LPCWSTR symbol,const MTChartBar* bars,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartReplace(LPCWSTR symbol,const int64_t from,const int64_t to,const MTChartBar* bars,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartSplit(LPCWSTR symbol,const uint32_t new_shares,const uint32_t old_shares,const uint32_t rounding_rule,const int64_t datetime_from,const int64_t datetime_to)=0;
   virtual MTAPIRES  ChartReserved2(void)=0;
   virtual MTAPIRES  ChartReserved3(void)=0;
   //--- internal email
   virtual IMTMail*  MailCreate(void)=0;
   virtual MTAPIRES  MailSubscribe(IMTMailSink* sink)=0;
   virtual MTAPIRES  MailUnsubscribe(IMTMailSink* sink)=0;
   virtual uint32_t  MailTotal(void)=0;
   virtual MTAPIRES  MailNext(const uint32_t pos,IMTMail* mail)=0;
   virtual MTAPIRES  MailDelete(const uint32_t pos)=0;
   virtual MTAPIRES  MailDeleteId(const uint64_t id)=0;
   virtual MTAPIRES  MailSend(IMTMail* mail)=0;
   virtual MTAPIRES  MailBodyRequest(const uint64_t id,IMTMail *mail)=0;
   virtual MTAPIRES  MailReserved2(void)=0;
   virtual MTAPIRES  MailReserved3(void)=0;
   virtual MTAPIRES  MailReserved4(void)=0;
   //--- internal news
   virtual IMTNews*  NewsCreate(void)=0;
   virtual MTAPIRES  NewsSubscribe(IMTNewsSink* sink)=0;
   virtual MTAPIRES  NewsUnsubscribe(IMTNewsSink* sink)=0;
   virtual MTAPIRES  NewsSend(IMTNews* news)=0;
   virtual uint32_t  NewsTotal(void)=0;
   virtual MTAPIRES  NewsNext(const uint32_t pos,IMTNews* news)=0;
   virtual MTAPIRES  NewsBodyRequest(const uint64_t id,IMTNews *news)=0;
   virtual MTAPIRES  NewsReserved4(void)=0;
   //--- external accounts
   virtual MTAPIRES  UserExternalRequest(const uint64_t gateway_id,LPCWSTR account,IMTUser* user)=0;
   virtual MTAPIRES  UserExternalRequest(LPCWSTR account,IMTUser* user)=0;
   virtual MTAPIRES  UserBackupRequest(const uint64_t server,const int64_t backup,const uint64_t login,IMTUser* user)=0;
   virtual MTAPIRES  UserBackupList(const uint64_t server,const int64_t from,const int64_t to,int64_t*& backups,uint32_t& backups_total)=0;
   virtual MTAPIRES  UserLogins(LPCWSTR group,uint64_t*& logins,uint32_t& logins_total)=0;
   virtual MTAPIRES  UserRequestArray(LPCWSTR group,IMTUserArray* users)=0;
   //--- self password management
   virtual MTAPIRES  PasswordChange(const uint32_t type,LPCWSTR password)=0;
   virtual MTAPIRES  PasswordReserved1(void)=0;
   virtual MTAPIRES  PasswordReserved2(void)=0;
   //--- spreads configuration
   virtual IMTConSpread* SpreadCreate(void)=0;
   virtual IMTConSpreadLeg* SpreadLegCreate(void)=0;
   virtual MTAPIRES  SpreadSubscribe(IMTConSpreadSink* sink)=0;
   virtual MTAPIRES  SpreadUnsubscribe(IMTConSpreadSink* sink)=0;
   virtual MTAPIRES  SpreadUpdate(IMTConSpread* spread)=0;
   virtual MTAPIRES  SpreadDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SpreadShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SpreadTotal(void)=0;
   virtual MTAPIRES  SpreadNext(const uint32_t pos,IMTConSpread* spread)=0;
   virtual MTAPIRES  SpreadUpdateBatch(IMTConSpread** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SpreadDeleteBatch(IMTConSpread** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SpreadReserved3(void)=0;
   virtual MTAPIRES  SpreadReserved4(void)=0;
   //--- network connection
   virtual MTAPIRES  NetworkRescan(const uint32_t flags,const uint32_t timeout)=0;
   virtual uint64_t  NetworkBytesSent(void)=0;
   virtual uint64_t  NetworkBytesRead(void)=0;
   virtual MTAPIRES  NetworkServer(MTAPISTR& server)=0;
   virtual MTAPIRES  NetworkAddress(MTAPISTR& address)=0;
   virtual MTAPIRES  NetworkReserved1(void)=0;
   virtual MTAPIRES  NetworkReserved2(void)=0;
   virtual MTAPIRES  NetworkReserved3(void)=0;
   virtual MTAPIRES  NetworkReserved4(void)=0;
   //--- trade orders database
   virtual MTAPIRES  OrderReopen(const uint64_t ticket)=0;
   virtual MTAPIRES  OrderRequestByGroup(LPCWSTR group,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderUpdateBatch(IMTOrderArray* orders,MTAPIRES* results)=0;
   //--- notifications
   virtual MTAPIRES  NotificationsSend(LPWSTR metaquotes_ids,LPCWSTR message)=0;
   virtual MTAPIRES  NotificationsSend(const uint64_t* logins,const uint32_t logins_total,LPCWSTR message)=0;
   virtual MTAPIRES  NotificationsReserved1(void)=0;
   virtual MTAPIRES  NotificationsReserved2(void)=0;
   virtual MTAPIRES  NotificationsReserved3(void)=0;
   virtual MTAPIRES  NotificationsReserved4(void)=0;
   //--- settings
   virtual MTAPIRES  SettingGet(LPCWSTR section,LPCWSTR key,LPVOID& outdata,uint32_t& outdata_len)=0;
   virtual MTAPIRES  SettingSet(LPCWSTR section,LPCWSTR key,const LPVOID indata,const uint32_t indata_len)=0;
   virtual MTAPIRES  SettingDelete(LPCWSTR section,LPCWSTR key)=0;
   virtual MTAPIRES  SettingReserved1(void)=0;
   virtual MTAPIRES  SettingReserved2(void)=0;
   virtual MTAPIRES  SettingReserved3(void)=0;
   virtual MTAPIRES  SettingReserved4(void)=0;
   //--- email servers configuration
   virtual IMTConEmail* EmailCreate(void)=0;
   virtual MTAPIRES  EmailSubscribe(IMTConEmailSink* sink)=0;
   virtual MTAPIRES  EmailUnsubscribe(IMTConEmailSink* sink)=0;
   virtual MTAPIRES  EmailUpdate(IMTConEmail* email)=0;
   virtual MTAPIRES  EmailDelete(LPCWSTR name)=0;
   virtual MTAPIRES  EmailDelete(const uint32_t pos)=0;
   virtual MTAPIRES  EmailShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  EmailTotal(void)=0;
   virtual MTAPIRES  EmailNext(const uint32_t pos,IMTConEmail* email)=0;
   virtual MTAPIRES  EmailGet(LPCWSTR name,IMTConEmail* email)=0;
   virtual MTAPIRES  EmailUpdateBatch(IMTConEmail** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  EmailDeleteBatch(IMTConEmail** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  EmailSend(LPCWSTR account,LPCWSTR to,LPCWSTR to_name,LPCWSTR subject,LPCWSTR body)=0;
   virtual MTAPIRES  EmailReserved2(void)=0;
   virtual MTAPIRES  EmailReserved3(void)=0;
   virtual MTAPIRES  EmailReserved4(void)=0;
   //--- messengers configuration
   virtual IMTConMessenger* MessengerCreate(void)=0;
   virtual MTAPIRES  MessengerSubscribe(IMTConMessengerSink* sink)=0;
   virtual MTAPIRES  MessengerUnsubscribe(IMTConMessengerSink* sink)=0;
   virtual MTAPIRES  MessengerUpdate(IMTConMessenger* messenger)=0;
   virtual MTAPIRES  MessengerDelete(LPCWSTR name)=0;
   virtual MTAPIRES  MessengerDelete(const uint32_t pos)=0;
   virtual MTAPIRES  MessengerShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  MessengerTotal(void)=0;
   virtual MTAPIRES  MessengerNext(const uint32_t pos,IMTConMessenger* messenger)=0;
   virtual MTAPIRES  MessengerGet(LPCWSTR name,IMTConMessenger* messenger)=0;
   virtual MTAPIRES  MessengerUpdateBatch(IMTConMessenger** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  MessengerDeleteBatch(IMTConMessenger** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  MessengerSend(LPCWSTR destination,LPCWSTR group,LPCWSTR sender,LPCWSTR text)=0;
   virtual MTAPIRES  MessengerVerifyPhone(LPCWSTR phone_number)=0;
   virtual IMTConMessengerCountry* MessengerCountryCreate()=0;
   virtual IMTConMessengerGroup* MessengerGroupCreate()=0;
   //--- deals database
   virtual MTAPIRES  DealRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealUpdateBatch(IMTDealArray* deals,MTAPIRES* results)=0;
   virtual MTAPIRES  DealUpdateBatchArray(IMTDeal** deals,const uint32_t deals_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DealDeleteBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DealRequestPage(const uint64_t login,const int64_t from,const int64_t to,const uint32_t offset,const uint32_t total,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealAdd(IMTDeal* deal)=0;
   virtual MTAPIRES  DealAddBatch(IMTDealArray* deals,MTAPIRES* results)=0;
   virtual MTAPIRES  DealAddBatchArray(IMTDeal** deals,const uint32_t deals_total,MTAPIRES* results)=0;
   //--- positions database
   virtual MTAPIRES  PositionRequestByGroup(LPCWSTR group,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionUpdateBatch(IMTPositionArray* positions,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionUpdateBatchArray(IMTPosition** positions,const uint32_t positions_total,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionDeleteBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionDeleteByTicket(const uint64_t ticket)=0;
   virtual MTAPIRES  PositionSplit(const uint64_t* tickets,const uint32_t tickets_total,const double *adjustments,const uint32_t new_shares,const uint32_t old_shares,const uint32_t round_rule_price,const uint32_t round_rule_volume,const uint32_t flags,MTAPIRES* results)=0;
   virtual MTAPIRES  PositionRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,IMTPositionArray* positions)=0;
   virtual MTAPIRES  PositionReserved4(void)=0;
   //--- open orders database
   virtual MTAPIRES  OrderUpdateBatchArray(IMTOrder** orders,const uint32_t orders_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderDeleteBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderAdd(IMTOrder* order)=0;
   virtual MTAPIRES  OrderAddBatch(IMTOrderArray* orders,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderAddBatchArray(IMTOrder** orders,const uint32_t orders_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,IMTOrderArray* orders)=0;
   //--- orders history database
   virtual MTAPIRES  HistoryRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestPage(const uint64_t login,const int64_t from,const int64_t to,const uint32_t offset,const uint32_t total,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,const int64_t from,const int64_t to,IMTOrderArray* orders)=0;
   virtual MTAPIRES  HistoryReserved4(void)=0;
   //--- TLS Certificates
   virtual MTAPIRES  TLSCertificateUpdate(const void* pfx_certificate,const uint32_t pfx_certificate_size,LPCWSTR password)=0;
   virtual MTAPIRES  TLSCertificateDelete(const uint32_t pos)=0;
   virtual MTAPIRES  TLSCertificateShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  TLSCertificateTotal(void)=0;
   virtual MTAPIRES  TLSCertificateNext(const uint32_t pos,MTAPISTR& name,MTAPISTR& thumbprint)=0;
   virtual MTAPIRES  TLSCertificateReserved1(void)=0;
   virtual MTAPIRES  TLSCertificateReserved2(void)=0;
   virtual MTAPIRES  TLSCertificateReserved3(void)=0;
   virtual MTAPIRES  TLSCertificateReserved4(void)=0;
   //--- ECN
   virtual IMTECNMatching* ECNCreateMatching(void)=0;
   virtual IMTECNMatchingArray* ECNCreateMatchingArray(void)=0;
   virtual IMTECNFilling* ECNCreateFilling(void)=0;
   virtual IMTECNFillingArray* ECNCreateFillingArray(void)=0;
   virtual IMTECNProvider* ECNCreateProvider(void)=0;
   virtual IMTECNProviderArray* ECNCreateProviderArray(void)=0;
   virtual IMTECNHistoryMatching* ECNCreateHistoryMatching(void)=0;
   virtual IMTECNHistoryMatchingArray* ECNCreateHistoryMatchingArray(void)=0;
   virtual IMTECNHistoryFilling* ECNCreateHistoryFilling(void)=0;
   virtual IMTECNHistoryFillingArray* ECNCreateHistoryFillingArray(void)=0;
   virtual IMTECNHistoryDeal* ECNCreateHistoryDeal(void)=0;
   virtual IMTECNHistoryDealArray* ECNCreateHistoryDealArray(void)=0;
   virtual MTAPIRES  ECNRequestByGroup(LPCWSTR mask,IMTECNMatchingArray* matching,IMTECNFillingArray* filling,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTECNMatchingArray* matching,IMTECNFillingArray* filling,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestByTickets(const uint64_t *tickets,const uint32_t tickets_total,IMTECNMatchingArray* matching,IMTECNFillingArray* filling,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestHistoryByGroup(LPCWSTR mask,const int64_t from,const int64_t to,
                                              IMTECNHistoryMatchingArray* matching,IMTECNHistoryFillingArray* filling,IMTECNHistoryDealArray* deals,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestHistoryByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,
                                               IMTECNHistoryMatchingArray* matching,IMTECNHistoryFillingArray* filling,IMTECNHistoryDealArray* deals,IMTECNProviderArray* providers)=0;
   virtual MTAPIRES  ECNRequestHistoryByTickets(const uint64_t *tickets,const uint32_t tickets_total,const int64_t from,const int64_t to,
                                                IMTECNHistoryMatchingArray* matching,IMTECNHistoryFillingArray* filling,IMTECNHistoryDealArray* deals,IMTECNProviderArray* providers)=0;
   //--- client management
   virtual IMTClient *ClientCreate(void)=0;
   virtual IMTClientArray *ClientCreateArray(void)=0;
   virtual MTAPIRES  ClientAdd(IMTClient *client)=0;
   virtual MTAPIRES  ClientAddBatch(IMTClientArray* clients,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientAddBatchArray(IMTClient **clients,const uint32_t clients_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUpdate(IMTClient *client)=0;
   virtual MTAPIRES  ClientUpdateBatch(IMTClientArray* clients,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUpdateBatchArray(IMTClient **clients,const uint32_t clients_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientDelete(const uint64_t client_id)=0;
   virtual MTAPIRES  ClientDeleteBatch(const uint64_t *client_ids,const uint32_t client_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientRequest(const uint64_t client_id,IMTClient *client)=0;
   virtual MTAPIRES  ClientRequestByGroup(LPCWSTR groups,IMTClientArray *clients)=0;
   virtual MTAPIRES  ClientRequestHistory(const uint64_t client_id,const uint64_t author,const int64_t from,const int64_t to,IMTClientArray *history)=0;
   virtual MTAPIRES  ClientUserAdd(const uint64_t client_id,const uint64_t login)=0;
   virtual MTAPIRES  ClientUserAddBatch(const uint64_t client_id,const uint64_t *logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUserDelete(const uint64_t client_id,const uint64_t login)=0;
   virtual MTAPIRES  ClientUserDeleteBatch(const uint64_t client_id,const uint64_t *logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  ClientUserRequest(const uint64_t client_id,uint64_t*& logins,uint32_t& logins_total)=0;
   virtual MTAPIRES  ClientReserved1(void)=0;
   virtual MTAPIRES  ClientReserved2(void)=0;
   virtual MTAPIRES  ClientReserved3(void)=0;
   virtual MTAPIRES  ClientReserved4(void)=0;
   //--- document management
   virtual IMTDocument *DocumentCreate(void)=0;
   virtual IMTDocumentArray *DocumentCreateArray(void)=0;
   virtual MTAPIRES  DocumentAdd(IMTDocument *document)=0;
   virtual MTAPIRES  DocumentAddBatch(IMTDocumentArray *documents,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentAddBatchArray(IMTDocument** documents,const uint32_t documents_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentUpdate(IMTDocument *document)=0;
   virtual MTAPIRES  DocumentUpdateBatch(IMTDocumentArray *documents,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentUpdateBatchArray(IMTDocument** documents,const uint32_t documents_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentDelete(const uint64_t document_id)=0;
   virtual MTAPIRES  DocumentDeleteBatch(const uint64_t *document_ids,const uint32_t document_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DocumentRequest(const uint64_t document_id,IMTDocument *document)=0;
   virtual MTAPIRES  DocumentRequestByClient(const uint64_t client_id,IMTDocumentArray *documents)=0;
   virtual MTAPIRES  DocumentRequestHistory(const uint64_t document_id,const uint64_t author,const int64_t from,const int64_t to,IMTDocumentArray *history)=0;
   virtual MTAPIRES  DocumentReserved1(void)=0;
   virtual MTAPIRES  DocumentReserved2(void)=0;
   virtual MTAPIRES  DocumentReserved3(void)=0;
   virtual MTAPIRES  DocumentReserved4(void)=0;
   //--- comment management
   virtual IMTComment *CommentCreate(void)=0;
   virtual IMTCommentArray *CommentCreateArray(void)=0;
   virtual MTAPIRES  CommentAdd(IMTComment *comment)=0;
   virtual MTAPIRES  CommentAddBatch(IMTCommentArray *comments,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentAddBatchArray(IMTComment **comments,const uint32_t comments_total,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentUpdate(IMTComment *comment)=0;
   virtual MTAPIRES  CommentUpdateBatch(IMTCommentArray *comments,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentUpdateBatchArray(IMTComment **comments,const uint32_t comments_total,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentDelete(const uint64_t comment_id)=0;
   virtual MTAPIRES  CommentDeleteBatch(const uint64_t *comment_ids,const uint32_t comment_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  CommentRequest(const uint64_t comment_id,IMTComment *comment)=0;
   virtual MTAPIRES  CommentRequestByClient(const uint64_t client_id,const uint32_t position,const uint32_t total,IMTCommentArray *comments)=0;
   virtual MTAPIRES  CommentRequestByDocument(const uint64_t document_id,const uint32_t position,const uint32_t total,IMTCommentArray *comments)=0;
   virtual MTAPIRES  CommentReserved1(void)=0;
   virtual MTAPIRES  CommentReserved2(void)=0;
   virtual MTAPIRES  CommentReserved3(void)=0;
   virtual MTAPIRES  CommentReserved4(void)=0;
   //--- attachment management
   virtual IMTAttachment* AttachmentCreate(void)=0;
   virtual IMTAttachmentArray* AttachmentCreateArray(void)=0;
   virtual MTAPIRES  AttachmentAdd(IMTAttachment *attachment)=0;
   virtual MTAPIRES  AttachmentAddBatch(IMTAttachmentArray* attachments,MTAPIRES* results)=0;
   virtual MTAPIRES  AttachmentAddBatchArray(IMTAttachment **attachments,const uint32_t attachments_total,MTAPIRES* results)=0;
   virtual MTAPIRES  AttachmentRequest(const uint64_t *attachment_ids,const uint32_t attachment_ids_total,IMTAttachmentArray *attachments)=0;
   virtual MTAPIRES  AttachmentReserved1(void)=0;
   virtual MTAPIRES  AttachmentReserved2(void)=0;
   virtual MTAPIRES  AttachmentReserved3(void)=0;
   virtual MTAPIRES  AttachmentReserved4(void)=0;
   //--- users management
   virtual MTAPIRES  UserRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTUserArray* users)=0;
   virtual MTAPIRES  UserBackupRequestArray(const uint64_t server,const int64_t backup,LPCWSTR groups,IMTUserArray* users)=0;
   virtual MTAPIRES  UserBackupRequestByLogins(const uint64_t server,const int64_t backup,const uint64_t *logins,const uint32_t logins_total,IMTUserArray* users)=0;
   virtual MTAPIRES  UserArchiveRequestArray(LPCWSTR groups,IMTUserArray* users)=0;
   virtual MTAPIRES  UserArchiveRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTUserArray* users)=0;
   virtual MTAPIRES  UserUpdateBatch(IMTUserArray* users,MTAPIRES* results)=0;
   virtual MTAPIRES  UserUpdateBatchArray(IMTUser** users,const uint32_t users_total,MTAPIRES* results)=0;
   //--- automation configuration
   virtual IMTConAutomation* AutomationCreate()=0;
   virtual IMTConAutoCondition* AutomationConditionCreate()=0;
   virtual IMTConAutoAction* AutomationActionCreate()=0;
   virtual IMTConAutoParam* AutomationParamCreate()=0;
   virtual MTAPIRES  AutomationSubscribe(IMTConAutomationSink* sink)=0;
   virtual MTAPIRES  AutomationUnsubscribe(IMTConAutomationSink* sink)=0;
   virtual MTAPIRES  AutomationUpdate(IMTConAutomation* config)=0;
   virtual MTAPIRES  AutomationDelete(LPCWSTR name)=0;
   virtual MTAPIRES  AutomationDelete(const uint32_t pos)=0;
   virtual MTAPIRES  AutomationShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  AutomationTotal(void)=0;
   virtual MTAPIRES  AutomationNext(const uint32_t pos,IMTConAutomation* config)=0;
   virtual MTAPIRES  AutomationGet(LPCWSTR name,IMTConAutomation* config)=0;
   virtual MTAPIRES  AutomationUpdateBatch(IMTConAutomation** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  AutomationDeleteBatch(IMTConAutomation** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  AutomationReserved1(void)=0;
   virtual MTAPIRES  AutomationReserved2(void)=0;
   virtual MTAPIRES  AutomationReserved3(void)=0;
   virtual MTAPIRES  AutomationReserved4(void)=0;
   //--- subscription configuration
   virtual IMTConSubscription* SubscriptionCfgCreate()=0;
   virtual IMTConSubscriptionSymbol* SubscriptionCfgSymbolCreate()=0;
   virtual IMTConSubscriptionNews* SubscriptionCfgNewsCreate()=0;
   virtual MTAPIRES  SubscriptionCfgSubscribe(IMTConSubscriptionSink* sink)=0;
   virtual MTAPIRES  SubscriptionCfgUnsubscribe(IMTConSubscriptionSink* sink)=0;
   virtual MTAPIRES  SubscriptionCfgUpdate(IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgDelete(LPCWSTR name)=0;
   virtual MTAPIRES  SubscriptionCfgDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SubscriptionCfgDeleteByID(const uint64_t id)=0;
   virtual MTAPIRES  SubscriptionCfgShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SubscriptionCfgTotal(void)=0;
   virtual MTAPIRES  SubscriptionCfgNext(const uint32_t pos,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgGet(LPCWSTR name,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgGetByID(const uint64_t id,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgUpdateBatch(IMTConSubscription** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionCfgDeleteBatch(IMTConSubscription** configs,const uint32_t configs_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionCfgReserved1(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved2(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved3(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved4(void)=0;
   //--- subscription records
   virtual IMTSubscription* SubscriptionCreate()=0;
   virtual IMTSubscriptionArray* SubscriptionCreateArray(void)=0;
   virtual MTAPIRES  SubscriptionJoin(const uint64_t login,const uint64_t subscription,IMTSubscription* record,IMTSubscriptionHistory* history)=0;
   virtual MTAPIRES  SubscriptionJoinBatch(const uint64_t* logins,const uint64_t* subscriptions,const uint32_t total,MTAPIRES* results,IMTSubscriptionArray* records,IMTSubscriptionHistoryArray* history)=0;
   virtual MTAPIRES  SubscriptionCancel(const uint64_t login,const uint64_t subscription,IMTSubscription* record,IMTSubscriptionHistory* history)=0;
   virtual MTAPIRES  SubscriptionCancelBatch(const uint64_t* logins,const uint64_t* subscriptions,const uint32_t total,MTAPIRES* results,IMTSubscriptionArray* records,IMTSubscriptionHistoryArray* history)=0;
   virtual MTAPIRES  SubscriptionUpdate(IMTSubscription* subscription)=0;
   virtual MTAPIRES  SubscriptionUpdateBatch(IMTSubscriptionArray* records,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionUpdateBatchArray(IMTSubscription** records,const uint32_t records_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionDelete(const uint64_t id)=0;
   virtual MTAPIRES  SubscriptionDeleteBatch(const uint64_t* ids,const uint32_t ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionRequest(const uint64_t login,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionRequestByID(const uint64_t id,IMTSubscription* record)=0;
   virtual MTAPIRES  SubscriptionRequestByGroup(LPCWSTR group,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionRequestByLogins(const uint64_t *logins,const uint32_t logins_total,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionRequestByIDs(const uint64_t *ids,const uint32_t ids_total,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionReserved1(void)=0;
   virtual MTAPIRES  SubscriptionReserved2(void)=0;
   virtual MTAPIRES  SubscriptionReserved3(void)=0;
   virtual MTAPIRES  SubscriptionReserved4(void)=0;
   //--- subscription history records
   virtual IMTSubscriptionHistory* SubscriptionHistoryCreate()=0;
   virtual IMTSubscriptionHistoryArray* SubscriptionHistoryCreateArray(void)=0;
   virtual MTAPIRES  SubscriptionHistoryUpdate(IMTSubscriptionHistory* subscription)=0;
   virtual MTAPIRES  SubscriptionHistoryUpdateBatch(IMTSubscriptionHistoryArray* records,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionHistoryUpdateBatchArray(IMTSubscriptionHistory** records,const uint32_t records_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionHistoryDelete(const uint64_t id)=0;
   virtual MTAPIRES  SubscriptionHistoryDeleteBatch(const uint64_t* ids,const uint32_t ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  SubscriptionHistoryRequest(const uint64_t login,const int64_t from,const int64_t to,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByID(const uint64_t id,IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByGroup(LPCWSTR group,const int64_t from,const int64_t to,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByLogins(const uint64_t *logins,const uint32_t logins_total,const int64_t from,const int64_t to,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryRequestByIDs(const uint64_t *ids,const uint32_t ids_total,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved1(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved2(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved3(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved4(void)=0;
   //--- VPS configuration
   virtual IMTConVPS* VPSCreate(void)=0;
   virtual IMTConVPSGroup* VPSCreateGroup(void)=0;
   virtual MTAPIRES  VPSSubscribe(IMTConVPSSink* sink)=0;
   virtual MTAPIRES  VPSUnsubscribe(IMTConVPSSink* sink)=0;
   virtual MTAPIRES  VPSGet(IMTConVPS* config)=0;
   virtual MTAPIRES  VPSSet(IMTConVPS* config)=0;
   virtual IMTConVPSRule* VPSCreateRule(void)=0;
   virtual IMTConVPSCondition* VPSCreateCondition(void)=0;
   virtual MTAPIRES  VPSReserved3(void)=0;
   virtual MTAPIRES  VPSReserved4(void)=0;
   //--- KYC configuration
   virtual IMTConKYC* KYCCreate(void)=0;
   virtual IMTConKYCCountry* KYCCountryCreate()=0;
   virtual IMTConKYCGroup* KYCGroupCreate()=0;
   virtual MTAPIRES  KYCSubscribe(IMTConKYCSink* sink)=0;
   virtual MTAPIRES  KYCUnsubscribe(IMTConKYCSink* sink)=0;
   virtual MTAPIRES  KYCUpdate(IMTConKYC* cfg)=0;
   virtual MTAPIRES  KYCDelete(LPCWSTR name)=0;
   virtual MTAPIRES  KYCDelete(const uint32_t pos)=0;
   virtual MTAPIRES  KYCShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  KYCTotal(void)=0;
   virtual MTAPIRES  KYCNext(const uint32_t pos,IMTConKYC* cfg)=0;
   virtual MTAPIRES  KYCGet(LPCWSTR name,IMTConKYC* cfg)=0;
   virtual MTAPIRES  KYCUpdateBatch(IMTConKYC** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  KYCDeleteBatch(IMTConKYC** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  KYCStart(const uint64_t* client_ids,const uint32_t clients_ids_total,MTAPIRES* results)=0;
   virtual MTAPIRES  KYCReserved1(void)=0;
   virtual MTAPIRES  KYCReserved2(void)=0;
   virtual MTAPIRES  KYCReserved3(void)=0;
   virtual MTAPIRES  KYCReserved4(void)=0;
   //--- deals database
   virtual MTAPIRES  DealPerform(IMTDeal* deal)=0;
   virtual MTAPIRES  DealPerformBatch(IMTDealArray* deals,MTAPIRES* results)=0;
   virtual MTAPIRES  DealPerformBatchArray(IMTDeal** deals,const uint32_t deals_total,MTAPIRES* results)=0;
   virtual MTAPIRES  DealRequestByLoginsSymbol(const uint64_t *logins,const uint32_t logins_total,LPCWSTR symbol,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,const int64_t from,const int64_t to,IMTDealArray* deals)=0;
   virtual MTAPIRES  DealReserved3(void)=0;
   virtual MTAPIRES  DealReserved4(void)=0;
   //--- platform server configuration
   virtual IMTConBackupFolder* NetServerBackupFolderCreate(void)=0;
   virtual MTAPIRES  NetServerReserved1(void)=0;
   virtual MTAPIRES  NetServerReserved2(void)=0;
   virtual MTAPIRES  NetServerReserved3(void)=0;
   virtual MTAPIRES  NetServerReserved4(void)=0;
   //--- users database
   virtual MTAPIRES  UserDeleteBatch(const uint64_t* logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  UserArchiveBatch(const uint64_t* logins,const uint32_t logins_total,MTAPIRES* results)=0;
   virtual MTAPIRES  UserRestoreBatch(IMTUserArray* users,MTAPIRES* results)=0;
   virtual MTAPIRES  UserRestoreBatchArray(IMTUser** users,const uint32_t users_total,MTAPIRES* results)=0;
   virtual MTAPIRES  UserBalanceCheckBatch(const uint64_t* logins,const uint32_t logins_total,const uint32_t fixflag,MTAPIRES* results,double* balance_user,double* balance_history,double* credit_user,double* credit_history)=0;
   virtual MTAPIRES  UserReserved2(void)=0;
   virtual MTAPIRES  UserReserved3(void)=0;
   virtual MTAPIRES  UserReserved4(void)=0;
   //--- symbols group configuration
   virtual MTAPIRES  SymbolGroupDelete(LPCWSTR name)=0;
   virtual MTAPIRES  SymbolGroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SymbolGroupTotal(void)=0;
   virtual MTAPIRES  SymbolGroupNext(const uint32_t pos,MTAPISTR &name)=0;
   virtual MTAPIRES  SymbolGroupExist(LPCWSTR name)=0;
   virtual MTAPIRES  SymbolGroupAdd(LPCWSTR name)=0;
   virtual MTAPIRES  SymbolGroupReserved2(void)=0;
   virtual MTAPIRES  SymbolGroupReserved3(void)=0;
   virtual MTAPIRES  SymbolGroupReserved4(void)=0;
   //--- trade orders database
   virtual MTAPIRES  OrderRequestByGroupSymbol(LPCWSTR group,LPCWSTR symbol,IMTOrderArray* orders)=0;
   virtual MTAPIRES  OrderCancel(const uint64_t ticket)=0;
   virtual MTAPIRES  OrderCancelBatch(const uint64_t* tickets,const uint32_t tickets_total,MTAPIRES* results)=0;
   virtual MTAPIRES  OrderReserved3(void)=0;
   virtual MTAPIRES  OrderReserved4(void)=0;
   //--- floating leverage configuration
   virtual IMTConLeverage* LeverageCreate(void)=0;
   virtual IMTConLeverageRule* LeverageRuleCreate(void)=0;
   virtual IMTConLeverageTier* LeverageTierCreate(void)=0;
   virtual MTAPIRES  LeverageSubscribe(IMTConLeverageSink* sink)=0;
   virtual MTAPIRES  LeverageUnsubscribe(IMTConLeverageSink* sink)=0;
   virtual MTAPIRES  LeverageUpdate(IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageDelete(LPCWSTR name)=0;
   virtual MTAPIRES  LeverageDelete(const uint32_t pos)=0;
   virtual MTAPIRES  LeverageShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  LeverageTotal(void)=0;
   virtual MTAPIRES  LeverageNext(const uint32_t pos,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageGet(LPCWSTR name,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageUpdateBatch(IMTConLeverage** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  LeverageDeleteBatch(IMTConLeverage** configs,const uint32_t config_total,MTAPIRES* results)=0;
   virtual MTAPIRES  LeverageReserved1(void)=0;
   virtual MTAPIRES  LeverageReserved2(void)=0;
   virtual MTAPIRES  LeverageReserved3(void)=0;
   virtual MTAPIRES  LeverageReserved4(void)=0;
   //--- custom commands
   virtual IMTByteStream* CustomCreateStream(void)=0;
   virtual MTAPIRES  CustomCommand(LPCVOID indata,const uint32_t indata_len,LPVOID& outdata,uint32_t& outdata_len)=0;
   virtual MTAPIRES  CustomCommand(IMTByteStream* indata,IMTByteStream* outdata)=0;
   virtual MTAPIRES  CustomReserved1(void)=0;
   virtual MTAPIRES  CustomReserved2(void)=0;
   virtual MTAPIRES  CustomReserved3(void)=0;
   virtual MTAPIRES  CustomReserved4(void)=0;
   //--- geodata
   virtual IMTGeo*   GeoCreate(void)=0;
   virtual MTAPIRES  GeoResolve(LPCWSTR address,const uint32_t flags,IMTGeo* record)=0;
   virtual MTAPIRES  GeoResolveBatch(LPCWSTR* addresses,const uint32_t addresses_total,const uint32_t flags,IMTGeo** records,MTAPIRES* results)=0;
   virtual MTAPIRES  GeoResolveReserved1(void)=0;
   virtual MTAPIRES  GeoResolveReserved2(void)=0;
   virtual MTAPIRES  GeoResolveReserved3(void)=0;
   virtual MTAPIRES  GeoResolveReserved4(void)=0;

   //--- explicit destructor is prohibited
protected:
                    ~IMTAdminAPI(void) {}
  };
//+------------------------------------------------------------------+
//| Entry point functions declarations                               |
//+------------------------------------------------------------------+
typedef MTAPIRES(*MTManagerVersion_t)   (uint32_t &version);
typedef MTAPIRES(*MTManagerCreate_t)    (uint32_t api_version,IMTManagerAPI **manager);
typedef MTAPIRES(*MTAdminCreate_t)      (uint32_t api_version,IMTAdminAPI **admin);
typedef MTAPIRES(*MTManagerCreateExt_t) (uint32_t api_version,LPCWSTR datapath,IMTManagerAPI **manager);
typedef MTAPIRES(*MTAdminCreateExt_t)   (uint32_t api_version,LPCWSTR datapath,IMTAdminAPI **admin);
//+------------------------------------------------------------------+
//| Entry point functions names                                      |
//+------------------------------------------------------------------+
static const LPCSTR s_MTManagerVersion   ="MTManagerVersion";
static const LPCSTR s_MTManagerCreate    ="MTManagerCreate";
static const LPCSTR s_MTManagerCreateExt ="MTManagerCreateExt";
static const LPCSTR s_MTAdminCreate      ="MTAdminCreate";
static const LPCSTR s_MTAdminCreateExt   ="MTAdminCreateExt";
//+------------------------------------------------------------------+
//| Manager API factory                                              |
//+------------------------------------------------------------------+
class CMTManagerAPIFactory
  {
private:
   enum EnCPUID_Reg
     {
      REG_EAX,
      REG_EBX,
      REG_ECX,
      REG_EDX,
      REGS_TOTAL
     };

   enum EnCPUID
     {
      CPUID_SSE1_BIT                  =1<<25,                  // in data[3]   ID=1
      CPUID_SSE2_BIT                  =1<<26,                  // in data[3]

      CPUID_SSE3_BIT                  =1<< 0,                  // in data[2 (ECX)]
      CPUID_SSSE3_BIT                 =1<< 9,                  // in data[2]
      CPUID_FMA3_BIT                  =1<<12,                  // in data[2]
      CPUID_CMPXCHG16B_BIT            =1<<13,                  // in data[2]
      CPUID_SSE41_BIT                 =1<<19,                  // in data[2]
      CPUID_SSE42_BIT                 =1<<20,                  // in data[2]
      CPUID_POPCNT_BIT                =1<<23,                  // in data[2]
      CPUID_AES_BIT                   =1<<25,                  // in data[2]
      CPUID_XSAVE_BIT                 =1<<26,                  // in data[2]
      CPUID_OSXSAVE_BIT               =1<<27,                  // in data[2]
      CPUID_F16C_BIT                  =1<<29,                  // in data[2]
      CPUID_AVX_BIT                   =1<<25 | 1<<26 | 1<<27 | 1<<28,  // in data[2] included CPUID_AES_BIT,CPUID_XSAVE_BIT & CPUID_OSXSAVE_BIT
      CPUID_AVX2_EXTENSION            =1<<12 | 1<<23,                  // in data[2] extended check of CPUID_FMA3_BIT, CPUID_POPCNT_BIT
      CPUID_RDRAND_BIT                =1<<30,                  // in data[2]

      CPUID_FSGSBASE_BIT              =1<<0,                   // in data[1]   ID=7
      CPUID_BMI1_BIT                  =1<<3,                   // in data[1]
      CPUID_AVX2_BIT                  =1<<5,                   // in data[1]
      CPUID_BMI2_BIT                  =1<<8,                   // in data[1]
      CPUID_ERMS_BIT                  =1<<9,                   // in data[1]
      CPUID_INVPCID_BIT               =1<<10,                  // in data[1]
      CPUID_AVX512F_BIT               =1<<16,                  // in data[1]   AVX-512 Foundation
      CPUID_AVX512DQ_BIT              =1<<17,                  // in data[1]   AVX-512 Doubleword and Quadword
      CPUID_RDSEED_BIT                =1<<18,                  // in data[1]
      CPUID_ADX_BIT                   =1<<19,                  // in data[1]
      CPUID_AVX512IFMA_BIT            =1<<21,                  // in data[1]   AVX-512 Integer Fused Multiply-Add
      CPUID_AVX512PF_BIT              =1<<26,                  // in data[1]   AVX-512 Prefetch
      CPUID_AVX512ER_BIT              =1<<27,                  // in data[1]   AVX-512 Exponential and Reciprocal
      CPUID_AVX512CD_BIT              =1<<28,                  // in data[1]   AVX-512 Conflict Detection
      CPUID_SHA_BIT                   =1<<29,                  // in data[1]
      CPUID_AVX512BW_BIT              =1<<30,                  // in data[1]   AVX-512 Byte and Word
      CPUID_AVX512VL_BIT              =1<<31,                  // in data[1]   AVX-512 Vector Length

      CPUID_AVX512VBMI_BIT            =1<<1,                   // in data[2]   AVX-512 Vector Byte Manipulation
      CPUID_AVX512VBMI2_BIT           =1<<6,                   // in data[2]   AVX-512 Vector Bit Manipulation Instructions 2
      CPUID_AVX512VNNI_BIT            =1<<11,                  // in data[2]   AVX-512 Vector Neural Network Instructions
      CPUID_AVX512BITALG_BIT          =1<<12,                  // in data[2]   AVX-512 BITALG
      CPUID_AVX512POPCNTDQ_BIT        =1<<14,                  // in data[2]   AVX-512 Vector Population Count D/Q

      CPUID_AVX5124VNNIW_BIT          =1<<2,                   // in data[3]   AVX-512 4-register Vector Neural Network Instructions Word variable precision
      CPUID_AVX5124FMAPS_BIT          =1<<3,                   // in data[3]   AVX-512 4-register Fused Multiply Accumulation Packed Single precision
     };

   HMODULE           m_hmodule=NULL;
   MTManagerVersion_t m_mtversion=nullptr;
   MTManagerCreate_t m_mtmanager=nullptr;
   MTAdminCreate_t   m_mtadmin=nullptr;
   MTManagerCreateExt_t m_mtmanager_ext=nullptr;
   MTAdminCreateExt_t m_mtadmin_ext=nullptr;
   int32_t           m_avx_version=-1;

public:
                     CMTManagerAPIFactory(void);
                    ~CMTManagerAPIFactory(void);
   //--- initialize & shutdown
   MTAPIRES          Initialize(LPCWSTR dll_path=NULL);
   MTAPIRES          Shutdown(void);
   //--- manager creation
   MTAPIRES          CreateManager(uint32_t version,IMTManagerAPI **manager);
   MTAPIRES          CreateAdmin(uint32_t version,IMTAdminAPI **admin);
   //--- manager creation with path
   MTAPIRES          CreateManager(uint32_t version,LPCWSTR datapath,IMTManagerAPI **manager);
   MTAPIRES          CreateAdmin(uint32_t version,LPCWSTR datapath,IMTAdminAPI **admin);
   //--- license check
   static MTAPIRES   LicenseCheckManager(IMTManagerAPI *manager,LPCWSTR name);
   static MTAPIRES   LicenseCheckAdmin(IMTAdminAPI *admin,LPCWSTR name);
   //--- version info
   MTAPIRES          Version(uint32_t &version) const;
   void              VersionCPUID(int32_t (&cpu_info)[4],int32_t id) const;
   int32_t           VersionAVX(void);

private:
   bool              FindLibrary(LPCWSTR dll_path,wchar_t *path,const size_t path_maxlen);
  };
//+------------------------------------------------------------------+
//| Library constructor                                              |
//+------------------------------------------------------------------+
inline CMTManagerAPIFactory::CMTManagerAPIFactory(void)
  {
  }
//+------------------------------------------------------------------+
//| Library destructor                                               |
//+------------------------------------------------------------------+
inline CMTManagerAPIFactory::~CMTManagerAPIFactory(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Library initialization                                           |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::Initialize(LPCWSTR dll_path/*=NULL*/)
  {
   wchar_t  path[MAX_PATH]={};
//--- find the Manager API DLL
   if(!FindLibrary(dll_path,path,_countof(path)-1))
      return(MT_RET_ERR_NOTFOUND);
//--- load Manager API DLL
   if((m_hmodule=::LoadLibraryW(path))==NULL)
      return(MT_RET_ERR_NOTFOUND);
//--- find entry point addresses
   m_mtversion=reinterpret_cast<MTManagerVersion_t>(::GetProcAddress(m_hmodule,s_MTManagerVersion));
//--- find for manager
   if((m_mtmanager_ext=reinterpret_cast<MTManagerCreateExt_t>(::GetProcAddress(m_hmodule,s_MTManagerCreateExt)))==NULL)
      m_mtmanager=reinterpret_cast<MTManagerCreate_t>(::GetProcAddress(m_hmodule,s_MTManagerCreate));
//--- find for admin
   if((m_mtadmin_ext=reinterpret_cast<MTAdminCreateExt_t>(::GetProcAddress(m_hmodule,s_MTAdminCreateExt)))==NULL)
      m_mtadmin=reinterpret_cast<MTAdminCreate_t>(::GetProcAddress(m_hmodule,s_MTAdminCreate));
//--- check results
   if(!m_mtversion || !m_mtmanager_ext || !m_mtadmin_ext)
     {
      Shutdown();
      //--- not found
      return(MT_RET_ERR_NOTFOUND);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Library shutdown                                                 |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::Shutdown(void)
  {
//--- check & free
   if(m_hmodule)
     {
      ::FreeLibrary((HMODULE)m_hmodule);
      m_hmodule=NULL;
     }
//--- zero members
   m_mtversion    =NULL;
   m_mtmanager    =NULL;
   m_mtadmin      =NULL;
   m_mtmanager_ext=NULL;
   m_mtadmin_ext  =NULL;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Find the library                                                 |
//+------------------------------------------------------------------+
inline bool CMTManagerAPIFactory::FindLibrary(LPCWSTR dll_path,wchar_t *path,const size_t path_maxlen)
  {
   LPCWSTR library_default=L"MT5APIManager64.dll";
   LPCWSTR library=library_default;
//--- detect architecture
#ifdef _M_ARM64
   library=L"MT5APIManager64arm.dll";
#else
   const int32_t avx=VersionAVX();

   if(avx>1)
      library=L"MT5APIManager64avx2.dll";
   else
      if(avx>0)
         library=L"MT5APIManager64avx.dll";
#endif
//--- save paths
   if(dll_path)
     {
      //--- check for native architecture
      _snwprintf_s(path,path_maxlen,_TRUNCATE,L"%s\\%s",dll_path,library);
      if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES)
         return(true);
      //--- check for default version
      if(_wcsicmp(library,library_default)!=0)
        {
         _snwprintf_s(path,path_maxlen,_TRUNCATE,L"%s\\%s",dll_path,library_default);
         if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES)
            return(true);
        }
     }
//--- find local folder
   wchar_t  folder[MAX_PATH]={};
   wchar_t *ptr;

   ::GetModuleFileNameW(NULL,folder,_countof(folder)-1);
//--- check for 3 levels
   for(int32_t i=0;i<3;i++)
     {
      //--- remove level
      if((ptr=wcsrchr(folder,L'\\'))==NULL)
         break;

      *ptr=L'\0';
      //--- check the file
      _snwprintf_s(path,path_maxlen,_TRUNCATE,L"%s\\%s",folder,library);
      if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES)
         return(true);
      //--- check /libs folder
      _snwprintf_s(path,path_maxlen,_TRUNCATE,L"%s\\libs\\%s",folder,library);
      if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES)
         return(true);
      //--- check for default version
      if(_wcsicmp(library,library_default)!=0)
        {
         _snwprintf_s(path,path_maxlen,_TRUNCATE,L"%s\\%s",folder,library_default);
         if(GetFileAttributesW(path)!=INVALID_FILE_ATTRIBUTES)
            return(true);
        }
     }
//--- try to find library in PATH
   wcsncpy_s(path,path_maxlen,library,_TRUNCATE);
   return(true);
  }
//+------------------------------------------------------------------+
//| Manager interface creation                                       |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::CreateManager(uint32_t version,IMTManagerAPI **manager)
  {
//--- check parameters
   if(!manager)
      return(MT_RET_ERR_PARAMS);
//--- check members & parameters
   if(!m_hmodule || (!m_mtmanager && !m_mtmanager_ext))
      return(MT_RET_ERROR);
//--- create manager
   return(m_mtmanager_ext ? m_mtmanager_ext(version,NULL,manager) : m_mtmanager(version,manager));
  }
//+------------------------------------------------------------------+
//| Administrator interface creation                                 |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::CreateAdmin(uint32_t version,IMTAdminAPI **admin)
  {
//--- check parameters
   if(!admin)
      return(MT_RET_ERR_PARAMS);
//--- check members & parameters
   if(!m_hmodule || (!m_mtadmin && !m_mtadmin_ext))
      return(MT_RET_ERROR);
//--- create manager
   return(m_mtadmin_ext ? m_mtadmin_ext(version,NULL,admin) : m_mtadmin(version,admin));
  }
//+------------------------------------------------------------------+
//| Manager interface creation                                       |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::CreateManager(uint32_t version,LPCWSTR datapath,IMTManagerAPI **manager)
  {
//--- check parameters
   if(!manager)
      return(MT_RET_ERR_PARAMS);
//--- check members & parameters
   if(!m_hmodule || !m_mtmanager_ext)
      return(MT_RET_ERROR);
//--- create manager
   return(m_mtmanager_ext(version,datapath,manager));
  }
//+------------------------------------------------------------------+
//| Administrator interface creation                                 |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::CreateAdmin(uint32_t version,LPCWSTR datapath,IMTAdminAPI **admin)
  {
//--- check parameters
   if(!admin)
      return(MT_RET_ERR_PARAMS);
//--- check members & parameters
   if(!m_hmodule || !m_mtadmin_ext)
      return(MT_RET_ERROR);
//--- create manager
   return(m_mtadmin_ext(version,datapath,admin));
  }
//+------------------------------------------------------------------+
//| Manager API license sign check                                   |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::LicenseCheckManager(IMTManagerAPI *manager,LPCWSTR name)
  {
   MTLicenseCheck license={},license_check={};
   HCRYPTPROV     prov=NULL;
   HCRYPTHASH     hash=NULL;
   HCRYPTKEY      key =NULL;
   MTAPIRES       res=MT_RET_ERROR;
//--- checking
   if(!manager || !name || name[0]==L'\0')
      return(MT_RET_ERROR);
//--- fill license information, copy name
   wcscpy_s(license.name,_countof(license.name),name);
   CMTStr::Terminate(license.name);
//--- initialize random generator
   srand(GetTickCount64() & 0x7fffffff);
//--- generate sequence of random numbers
   for(license.random_size=0;license.random_size<sizeof(license.random);license.random_size++)
      license.random[license.random_size]=char(rand()&0xFF);
//--- make copy
   memcpy(&license_check,&license,sizeof(license));
//--- check license
   manager->LicenseCheck(license_check);
//--- check retcode
   if(license_check.retcode!=MT_RET_OK)
      return(license_check.retcode);
//--- check license sign, initialize provider
   if(CryptAcquireContextW(&prov,NULL,MS_DEF_PROV_W,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)
      return(MT_RET_ERROR);
//--- import public key
   if(CryptImportKey(prov,(const BYTE*)MT5APIPublicKey,sizeof(MT5APIPublicKey),0,0,&key)!=FALSE)
     {
      //--- initialize hash object
      if(CryptCreateHash(prov,CALG_MD5,0,0,&hash)!=FALSE)
        {
         //--- set hash from original data
         if(CryptHashData(hash,(BYTE*)&license,(DWORD)offsetof(MTLicenseCheck,retcode),0)!=FALSE)
           {
            //--- verify hash sign, using public key
            if(CryptVerifySignature(hash,(BYTE*)license_check.sign,(DWORD)license_check.sign_size,key,NULL,0)!=FALSE)
               res=license_check.retcode;
            else
               res=MT_RET_ERR_DATA;
           }
        }
     }
//--- destroy hash
   if(hash)
      CryptDestroyHash(hash);
//--- destroy key
   if(key)
      CryptDestroyKey(key);
//--- release provider
   if(prov)
      CryptReleaseContext(prov,0);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Manager API license sign check                                   |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::LicenseCheckAdmin(IMTAdminAPI *admin,LPCWSTR name)
  {
   MTLicenseCheck license={},license_check={};
   HCRYPTPROV     prov=NULL;
   HCRYPTHASH     hash=NULL;
   HCRYPTKEY      key =NULL;
   MTAPIRES       res=MT_RET_ERROR;
//--- checking
   if(!admin || !name || name[0]==L'\0')
      return(MT_RET_ERROR);
//--- fill license information, copy name
   wcscpy_s(license.name,_countof(license.name),name);
   CMTStr::Terminate(license.name);
//--- initialize random generator
   srand(GetTickCount64() & 0x7fffffff);
//--- generate sequence of random numbers
   for(license.random_size=0;license.random_size<sizeof(license.random);license.random_size++)
      license.random[license.random_size]=char(rand()&0xFF);
//--- make copy
   memcpy(&license_check,&license,sizeof(license));
//--- check license
   admin->LicenseCheck(license_check);
//--- check retcode
   if(license_check.retcode!=MT_RET_OK)
      return(license_check.retcode);
//--- check license sign, initialize provider
   if(CryptAcquireContextW(&prov,NULL,MS_DEF_PROV_W,PROV_RSA_FULL,CRYPT_VERIFYCONTEXT)==FALSE)
      return(MT_RET_ERROR);
//--- import public key
   if(CryptImportKey(prov,(const BYTE*)MT5APIPublicKey,sizeof(MT5APIPublicKey),0,0,&key)!=FALSE)
     {
      //--- initialize hash object
      if(CryptCreateHash(prov,CALG_MD5,0,0,&hash)!=FALSE)
        {
         //--- set hash from original data
         if(CryptHashData(hash,(BYTE*)&license,(DWORD)offsetof(MTLicenseCheck,retcode),0)!=FALSE)
           {
            //--- verify hash sign received from server, using public key
            if(CryptVerifySignature(hash,(BYTE*)license_check.sign,(DWORD)license_check.sign_size,key,NULL,0)!=FALSE)
               res=license_check.retcode;
            else
               res=MT_RET_ERR_DATA;
           }
        }
     }
//--- destroy hash
   if(hash)
      CryptDestroyHash(hash);
//--- destroy key
   if(key)
      CryptDestroyKey(key);
//--- release provider
   if(prov)
      CryptReleaseContext(prov,0);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Version                                                          |
//+------------------------------------------------------------------+
inline MTAPIRES CMTManagerAPIFactory::Version(uint32_t &version) const
  {
//--- check members & parameters
   if(!m_hmodule || !m_mtversion)
      return(MT_RET_ERROR);
//--- result
   return(m_mtversion(version));
  }
//+------------------------------------------------------------------+
//| __cpuid version                                                  |
//+------------------------------------------------------------------+
inline void CMTManagerAPIFactory::VersionCPUID(int32_t (&cpu_info)[4],int32_t id) const
  {
#if defined(_M_ARM64) || defined(__CLR_VER)
   memset(&cpu_info,0,sizeof(int)*4);
#else
   #ifdef __clang__
//--- restore rbx, there is error in clang - rbx will be poisoned after cpuid
   __asm("  xchgq  %%rbx,%q1\n"
         "  xor %%ecx,%%ecx\n"
         "  cpuid\n"
         "  xchgq  %%rbx,%q1"
         : "=a"(cpu_info[0]), "=r" (cpu_info[1]), "=c"(cpu_info[2]), "=d"(cpu_info[3])
         : "0"(id));
   #else
   __cpuid(cpu_info,id);
   #endif
#endif
  }
//+------------------------------------------------------------------+
//| CPU AVX version                                                  |
//+------------------------------------------------------------------+
inline int32_t CMTManagerAPIFactory::VersionAVX(void)
  {
//--- no AVX for ARM64 or C#
#if defined(_M_ARM64) || defined(__CLR_VER)
   return(0);
#else
   if(m_avx_version<0)
     {
      int32_t version=0;
      int32_t data1[4]={};       // feature set for ID=1
      int32_t data7[4]={};       // feature set for ID=7
      //--- feature set: 1
      VersionCPUID(data1,1);
      if((data1[REG_ECX] & CPUID_AVX_BIT)==CPUID_AVX_BIT)  // CPUID_AVX_BIT, CPUID_AES_BIT, CPUID_XSAVE_BIT, CPUID_OSXSAVE_BIT
         if((_xgetbv(0) & 6)==6)
           {
            version=1;
            //--- feature set: 7
            VersionCPUID(data7,7);
            if(data7[REG_EBX] & CPUID_AVX2_BIT)            // standard AVX2
              {
               //--- check for extended flags
               if((data1[REG_ECX] & CPUID_AVX2_EXTENSION)==CPUID_AVX2_EXTENSION)  // CPUID_FMA3_BIT, CPUID_POPCNT_BIT
                 {
                  version=2;
                  //--- check for AVX512
                  if(data7[REG_EBX] & CPUID_AVX512F_BIT)
                     version=512;
                 }
              }
           }
      //--- save
      m_avx_version=version;
     }
//--- вернем версию
   return(m_avx_version);
#endif
  }
//+------------------------------------------------------------------+
