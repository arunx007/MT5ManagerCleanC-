//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Gateway API version                                              |
//+------------------------------------------------------------------+
#define MTGatewayAPIVersion  5200
#define MTGatewayAPIDate     L"1 Aug 2025"

#include <wincrypt.h>
#include <winsock2.h>
#include <cstdint>
#include <intrin.h>
#include "MT5APIConstants.h"
#include "MT5APITools.h"
#include "MT5APILogger.h"
#include "MT5APIPublicKey.h"
#include "Config\MT5APIConfigFeeder.h"
#include "Config\MT5APIConfigGateway.h"
#include "Config\MT5APIConfigCommon.h"
#include "Config\MT5APIConfigSymbol.h"
#include "Config\MT5APIConfigGroup.h"
#include "Config\MT5APIConfigNetwork.h"
#include "Config\MT5APIConfigTime.h"
#include "Config\MT5APIConfigSpread.h"
#include "Bases\MT5APIAccount.h"
#include "Bases\MT5APIBook.h"
#include "Bases\MT5APIConfirm.h"
#include "Bases\MT5APIExecution.h"
#include "Bases\MT5APIMail.h"
#include "Bases\MT5APIOrder.h"
#include "Bases\MT5APIPosition.h"
#include "Bases\MT5APIRequest.h"
#include "Bases\MT5APITick.h"
#include "Bases\MT5APIUser.h"
#include "Bases\MT5APIChart.h"

//-- include necessary library
#pragma comment(lib,"advapi32.lib")

//--- PVS Studio helper
//V_FORMATTED_IO_FUNC, class:IMTGatewayAPI, function:LoggerOut, format_arg:2, ellipsis_arg:3, enable_on_virtual

//+------------------------------------------------------------------+
//| Gateway description info                                         |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTGatewayInfo
  {
   //--- gateway modes flags
   enum EnModes
     {
      GATEWAY_MODE_QUOTES     =1,                           // receive quotes
      GATEWAY_MODE_NEWS       =2,                           // receive news
      GATEWAY_MODE_POSITIONS  =4,                           // gateway can process requests of trade positions
      //--- enumeration borders
      GATEWAY_MODE_NONE       =0,
      GATEWAY_MODE_ALL        =GATEWAY_MODE_QUOTES|GATEWAY_MODE_NEWS|GATEWAY_MODE_POSITIONS
     };
   //--- necessary fields flags
   enum EnFields
     {
      GATEWAY_FIELD_SERVER    =1,                           // server field
      GATEWAY_FIELD_LOGIN     =2,                           // login field
      GATEWAY_FIELD_PASSWORD  =4,                           // password field
      GATEWAY_FIELD_PARAMS    =8,                           // parameters
      //--- enumeration borders
      GATEWAY_FIELD_NONE      =0,
      GATEWAY_FIELD_ALL       =GATEWAY_FIELD_SERVER|GATEWAY_FIELD_LOGIN|GATEWAY_FIELD_PASSWORD|GATEWAY_FIELD_PARAMS
     };
   //--- data
   uint32_t          version;                               // gateway version
   uint32_t          version_api;                           // gateway API version
   wchar_t           name_default[128];                     // gateway default name
   wchar_t           copyright[128];                        // copyright
   wchar_t           server_default[128];                   // default dataserver address:port
   wchar_t           login_default[64];                     // gateway default login
   wchar_t           password_default[64];                  // gateway default password
   wchar_t           parameters_default[512];               // additional parameters
   uint32_t          mode;                                  // EnModes
   uint32_t          fields;                                // EnFields
   wchar_t           description[512];                      // description
   wchar_t           module_id[16];                         // gateway module id
   wchar_t           build_date[16];                        // datafeed\gateway build date
   wchar_t           build_api_date[16];                    // Gateway API build date
   uint32_t          reserved[32];                          // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| News structure                                                   |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTNews
  {
   //--- constants
   enum constants
     {
      MAX_NEWS_BODY_LEN=15*1024*1024                        // max. body len
     };
   //--- news topic flags
   enum EnNewsFlags
     {
      FLAG_PRIORITY    =1,                                  // priority flag
      FLAG_CALENDAR    =2,                                  // calendar item flag
      FLAG_MIME        =4,                                  // MIME content flag
      FLAG_ALLOW_DEMO  =8                                   // allow news body for 'topic only' accounts
     };
   uint32_t          language;                              // news language (WinAPI LANGID)
   wchar_t           subject[256];                          // news subject
   wchar_t           category[256];                         // news category
   uint32_t          flags;                                 // EnNewsFlags
   wchar_t          *body;                                  // body
   uint32_t          body_len;                              // body length
   uint32_t          languages_list[32];                    // list of languages available for news
   int64_t           datetime;                              // datetime
   uint32_t          reserved[30];                          // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Economic event structure                                         |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTEconomicEvent
  {
   //--- news topic flags
   enum EnEconomicEventPriority
     {
      PRIORITY_LOW     =0,                                  // low priority of event
      PRIORITY_NORMAL  =1,                                  // normal priority of event
      PRIORITY_HIGH    =2                                   // high priority of event
     };
   int64_t           eventtime;                             // event time
   wchar_t           name[128];                             // event name
   wchar_t           currency[32];                          // event currency
   uint32_t          priority;                              // event priority
   wchar_t           period[128];                           // event period
   wchar_t           val_previous[32];                      // event previous value
   wchar_t           val_forecast[32];                      // event forecast value
   wchar_t           val_actual[32];                        // event actual value
   uint32_t          language;                              // news language (WinAPI LANGID)
   uint32_t          languages_list[32];                    // list of languages available for news
   uint32_t          reserved[31];                          // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Gateway events notification interface                            |
//+------------------------------------------------------------------+
class IMTGatewaySink
  {
public:
   //--- connection type
   enum EnConnectType
     {
      CONNECT_TYPE_TRADE   =1,   // Trade server
      CONNECT_TYPE_HISTORY =2,   // History server
      CONNECT_TYPE_ECN     =3,   // ECN
      CONNECT_TYPE_BACKUP  =4,   // Backup server
     };

public:
   //--- MetaTrader Server's connection events and hooks
   virtual MTAPIRES  HookServerConnect(LPCWSTR         /*address*/,
                                       const uint32_t  /*type*/,
                                       const uint64_t  /*login*/)       { return(MT_RET_OK);               }
   virtual void      OnServerDisconnect(LPCWSTR        /*address*/,
                                        const uint32_t /*type*/,
                                        const uint64_t /*login*/)       {  }
   //--- gateway events
   virtual void      OnGatewayConfig(const uint64_t /*login*/,const IMTConGateway* /*config*/) {  }
   virtual void      OnGatewayConfig(const uint64_t /*login*/,const IMTConFeeder*  /*config*/) {  }
   virtual void      OnGatewayStart(void)                               {  }
   virtual void      OnGatewayStop(void)                                {  }
   virtual void      OnGatewayShutdown(const uint64_t /*login*/)        {  }
   //--- asynchronous methods notifications
   //--- DealerLock result notification
   virtual void      OnDealerLock(const MTAPIRES     /*retcode*/,
                                  const IMTRequest*  /*request*/,
                                  const IMTUser*     /*user*/,
                                  const IMTAccount*  /*account*/,
                                  const IMTOrder*    /*order*/,
                                  const IMTPosition* /*position*/)      {  }
   //--- DealerAnswer result notification
   virtual void      OnDealerAnswer(const MTAPIRES      /*retcode*/,
                                    const IMTConfirm*   /*confirm*/)    {  }
   virtual void      OnDealerAnswer(const uint64_t      /*login*/,
                                    const MTAPIRES      /*retcode*/,
                                    const IMTExecution* /*execution*/)  {  }
   //--- external trading system positions control hooks
   virtual MTAPIRES  HookGatewayPositionsRequest(void)                  { return(MT_RET_ERR_NOTIMPLEMENT); }
   virtual MTAPIRES  HookGatewayPositionsCheck(void)                    { return(MT_RET_ERR_NOTIMPLEMENT); }
   virtual MTAPIRES  HookGatewayOrdersRequest(void)                     { return(MT_RET_ERR_NOTIMPLEMENT); }
   virtual MTAPIRES  HookGatewayAccountRequest(uint64_t /*login*/,
                                               LPCWSTR  /*account_id*/) { return(MT_RET_ERR_NOTIMPLEMENT); }
   virtual void      OnGatewayAccountAnswer(const MTAPIRES           /*retcode*/,
                                            const int64_t            /*request_id*/,
                                            const IMTUser*           /*user*/,
                                            const IMTAccount*        /*account*/,
                                            const IMTOrderArray*     /*orders*/,
                                            const IMTPositionArray*  /*positions*/) {  }
   virtual void      OnGatewayAccountSet(const MTAPIRES           /*retcode*/,
                                         const int64_t            /*request_id*/,
                                         const IMTUser*           /*user*/,
                                         const IMTAccount*        /*account*/,
                                         const IMTOrderArray*     /*orders*/,
                                         const IMTPositionArray*  /*positions*/) {  }
   //--- quotes symbols changes
   virtual void      OnServerSymbolAdd(LPCWSTR /*symbol*/)     {  }
   virtual void      OnServerSymbolDelete(LPCWSTR /*symbol*/)  {  }
   //--- MetaTrader Server's synchronization event
   virtual void      OnServerSynchronized(LPCWSTR        /*address*/,
                                          const uint32_t /*type*/,
                                          const uint64_t /*id*/) {  }
  };
//+------------------------------------------------------------------+
//| Gateway API interface                                            |
//+------------------------------------------------------------------+
class IMTGatewayAPI
  {
public:
   //--- dealer flags
   enum EnDealerRequestFlags
     {
      DEALER_FLAG_NONE                 =0x00000000,                    // zero flags
      DEALER_FLAG_AUTOLOCK             =0x00000001,                    // automatically lock new requests from request queue
      DEALER_FLAG_USER                 =0x00000002,                    // get IMTUser data on each request
      DEALER_FLAG_ACCOUNT              =0x00000004,                    // get IMTAccount data on each request
      DEALER_FLAG_ORDER                =0x00000008,                    // get IMTOrder data on each request
      DEALER_FLAG_POSITION             =0x00000010,                    // get IMTPosition data on each request
      DEALER_FLAG_EXTERNAL_ACC         =0x00000020,                    // fill ExternalAccount field on each request
      DEALER_FLAG_MARKUP_TRADES        =0x00000040                     // allow modify trading prices according to markup settings
     };
   //--- interface release
   virtual void      Release(void)=0;
   //--- gateway license check
   virtual MTAPIRES  LicenseCheck(MTLicenseCheck &check)=0;
   //--- gateway server start-stop
   virtual MTAPIRES  Start(IMTGatewaySink *sink,LPCWSTR address=nullptr)=0;
   virtual MTAPIRES  Stop(void)=0;
   //--- gateway external connection state
   virtual MTAPIRES  StateConnect(const uint32_t state)=0;
   virtual MTAPIRES  StateTraffic(const uint32_t received_bytes,const uint32_t sent_bytes)=0;
   virtual MTAPIRES  StateReserved1(void)=0;
   virtual MTAPIRES  StateReserved2(void)=0;
   virtual MTAPIRES  StateReserved3(void)=0;
   virtual MTAPIRES  StateReserved4(void)=0;
   //--- client connection management
   virtual MTAPIRES  ClientAdd(const uint64_t login,LPCWSTR password)=0;
   virtual MTAPIRES  ClientAllowIP(LPCWSTR address)=0;
   virtual MTAPIRES  ClientReserved1(void)=0;
   virtual MTAPIRES  ClientReserved2(void)=0;
   virtual MTAPIRES  ClientReserved3(void)=0;
   virtual MTAPIRES  ClientReserved4(void)=0;
   //--- gateway server data access
   virtual MTAPIRES  ServerIP(MTAPISTR &ip)=0;
   virtual uint32_t  ServerPort(void)=0;
   virtual uint32_t  ServerConnections(void)=0;
   virtual MTAPIRES  ServerReserved1(void)=0;
   virtual MTAPIRES  ServerReserved2(void)=0;
   virtual MTAPIRES  ServerReserved3(void)=0;
   virtual MTAPIRES  ServerReserved4(void)=0;
   //--- log functions
   virtual MTAPIRES  LoggerOut(const uint32_t code,LPCWSTR format,...)=0;
   virtual void      LoggerFlush(void)=0;
   virtual LPCWSTR   LoggerPath(MTAPISTR& ip)=0;
   virtual MTAPIRES  LoggerOutString(const uint32_t code,LPCWSTR string)=0;
   virtual MTAPIRES  LoggerReserved3(void)=0;
   virtual MTAPIRES  LoggerReserved4(void)=0;
   //--- send data functions
   virtual MTAPIRES  SendTickStats(MTTickStat *stats,const uint32_t stats_total)=0;
   virtual MTAPIRES  SendTicks(MTTick *ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  SendBookDiffs(MTBookDiff *bookdiffs,const uint32_t bookdiffs_total)=0;
   virtual MTAPIRES  SendNews(MTNews *news,const uint32_t news_total)=0;
   virtual MTAPIRES  SendEconomicEvents(MTEconomicEvent *events,const uint32_t events_total)=0;
   virtual MTAPIRES  SendBooks(MTBook *books,const uint32_t books_total)=0;
   virtual MTAPIRES  SendReserved1(void)=0;
   virtual MTAPIRES  SendReserved2(void)=0;
   //--- feeder configuration
   virtual IMTConFeeder* FeederCreate(void)=0;
   virtual IMTConParam* FeederParamCreate(void)=0;
   virtual IMTConFeederTranslate* FeederTranslateCreate(void)=0;
   //--- gateway configuration
   virtual IMTConGateway* GatewayCreate(void)=0;
   virtual IMTConParam* GatewayParamCreate(void)=0;
   virtual IMTConGatewayTranslate* GatewayTranslateCreate(void)=0;
   //--- clients group configuration
   virtual IMTConGroup* GroupCreate(void)=0;
   virtual IMTConGroupSymbol* GroupSymbolCreate(void)=0;
   virtual IMTConCommission* GroupCommissionCreate(void)=0;
   virtual IMTConCommTier* GroupTierCreate(void)=0;
   virtual MTAPIRES  GroupSubscribe(IMTConGroupSink *sink)=0;
   virtual MTAPIRES  GroupUnsubscribe(IMTConGroupSink *sink)=0;
   virtual uint32_t  GroupTotal(void)=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConGroup *group)=0;
   virtual MTAPIRES  GroupGet(LPCWSTR name,IMTConGroup *group)=0;
   virtual MTAPIRES  GroupReserved1(void)=0;
   virtual MTAPIRES  GroupReserved2(void)=0;
   virtual MTAPIRES  GroupReserved3(void)=0;
   virtual MTAPIRES  GroupReserved4(void)=0;
   //--- trade server symbol configuration access
   virtual IMTConSymbol* SymbolCreate(void)=0;
   virtual IMTConSymbolSession* SymbolSessionCreate(void)=0;
   virtual MTAPIRES  SymbolSubscribe(IMTConSymbolSink *sink)=0;
   virtual MTAPIRES  SymbolUnsubscribe(IMTConSymbolSink *sink)=0;
   virtual MTAPIRES  SymbolAddPreliminary(IMTConSymbol *symbol)=0;
   virtual uint32_t  SymbolTotal(void)=0;
   virtual MTAPIRES  SymbolNext(const uint32_t pos,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,LPCWSTR name_group,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolUpdate(IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolDelete(LPCWSTR name)=0;
   virtual MTAPIRES  SymbolReserved1(void)=0;
   //--- time config
   virtual IMTConTime* TimeCreate(void)=0;
   virtual MTAPIRES  TimeSubscribe(IMTConTimeSink *sink)=0;
   virtual MTAPIRES  TimeUnsubscribe(IMTConTimeSink *sink)=0;
   virtual int64_t   TimeCurrent(void)=0;
   virtual MTAPIRES  TimeGet(IMTConTime *common)=0;
   virtual MTAPIRES  TimeReserved1(void)=0;
   virtual MTAPIRES  TimeReserved2(void)=0;
   virtual MTAPIRES  TimeReserved3(void)=0;
   virtual MTAPIRES  TimeReserved4(void)=0;
   //--- requests
   virtual IMTRequest* RequestCreate(void)=0;
   virtual IMTRequestArray* RequestArrayCreate(void)=0;
   virtual MTAPIRES  RequestSubscribe(IMTRequestSink *sink)=0;
   virtual MTAPIRES  RequestUnsubscribe(IMTRequestSink *sink)=0;
   virtual uint32_t  RequestTotal(void)=0;
   virtual MTAPIRES  RequestNext(const uint32_t pos,IMTRequest *request,IMTUser *user,IMTAccount *account,IMTOrder *order,IMTPosition *position)=0;
   virtual MTAPIRES  RequestGet(const uint32_t id,IMTRequest *request,IMTUser *user,IMTAccount *account,IMTOrder *order,IMTPosition *position)=0;
   virtual MTAPIRES  RequestGetAll(IMTRequestArray *requests)=0;
   virtual MTAPIRES  RequestReserved1(void)=0;
   virtual MTAPIRES  RequestReserved2(void)=0;
   virtual MTAPIRES  RequestReserved3(void)=0;
   virtual MTAPIRES  RequestReserved4(void)=0;
   //--- dealing
   virtual IMTConfirm* DealerConfirmCreate(void)=0;
   virtual IMTExecution* DealerExecutionCreate(void)=0;
   virtual MTAPIRES  DealerStart(const uint32_t flags)=0;
   virtual MTAPIRES  DealerStop(void)=0;
   virtual MTAPIRES  DealerGetAsync(void)=0;
   virtual MTAPIRES  DealerLockAsync(const uint32_t id)=0;
   virtual MTAPIRES  DealerAnswerAsync(IMTConfirm *confirm)=0;
   virtual MTAPIRES  DealerExecuteAsync(IMTExecution *execution)=0;
   virtual MTAPIRES  DealerReserved1(void)=0;
   virtual MTAPIRES  DealerReserved2(void)=0;
   virtual MTAPIRES  DealerReserved3(void)=0;
   virtual MTAPIRES  DealerReserved4(void)=0;
   //--- internal email
   virtual IMTMail*  MailCreate(void)=0;
   virtual MTAPIRES  MailSend(IMTMail *mail)=0;
   virtual MTAPIRES  MailReserved1(void)=0;
   virtual MTAPIRES  MailReserved2(void)=0;
   virtual MTAPIRES  MailReserved3(void)=0;
   virtual MTAPIRES  MailReserved4(void)=0;
   //--- clients database
   virtual IMTUser*  UserCreate(void)=0;
   virtual IMTAccount* UserCreateAccount(void)=0;
   //--- open orders database
   virtual IMTOrder* OrderCreate(void)=0;
   //--- trade positions database
   virtual IMTPosition* PositionCreate(void)=0;
   //--- custom settings control
   virtual MTAPIRES  SettingsAdd(const IMTConParam *param)=0;
   virtual MTAPIRES  SettingsUpdate(const uint32_t pos,const IMTConParam *param)=0;
   virtual MTAPIRES  SettingsUpdate(const IMTConParam *param)=0;
   virtual MTAPIRES  SettingsDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SettingsDelete(LPCWSTR name)=0;
   virtual MTAPIRES  SettingsClear(void)=0;
   virtual uint32_t  SettingsTotal(void)=0;
   virtual MTAPIRES  SettingsNext(const uint32_t pos,IMTConParam *param)=0;
   virtual MTAPIRES  SettingsGet(LPCWSTR name,IMTConParam *param)=0;
   virtual MTAPIRES  SettingsReserved1(void)=0;
   virtual MTAPIRES  SettingsReserved2(void)=0;
   virtual MTAPIRES  SettingsReserved3(void)=0;
   virtual MTAPIRES  SettingsReserved4(void)=0;
   //--- external trading system positions control
   virtual IMTConParamArray* GatewayParamArrayCreate(void)=0;
   virtual IMTPositionArray* GatewayPositionArrayCreate(void)=0;
   virtual MTAPIRES  GatewayPositionsAnswer(const MTAPIRES result,const int64_t positions_time,const IMTPositionArray *positions)=0;
   virtual MTAPIRES  GatewayPositionsCheck(const MTAPIRES result,const int64_t positions_time,const IMTPositionArray *positions)=0;
   virtual MTAPIRES  GatewayPositionsReserved1(void)=0;
   virtual MTAPIRES  GatewayPositionsReserved2(void)=0;
   virtual MTAPIRES  GatewayPositionsReserved3(void)=0;
   virtual MTAPIRES  GatewayPositionsReserved4(void)=0;
   //--- chart data
   virtual MTAPIRES  ChartRequest(LPCWSTR symbol,const int64_t from,const int64_t to,MTChartBar *&bars,uint32_t &bars_total)=0;
   virtual MTAPIRES  ChartDelete(LPCWSTR symbol,const int64_t *bars_dates,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartUpdate(LPCWSTR symbol,const MTChartBar *bars,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartReplace(LPCWSTR symbol,const int64_t from,const int64_t to,const MTChartBar *bars,const uint32_t bars_total)=0;
   virtual MTAPIRES  ChartReserved1(void)=0;
   virtual MTAPIRES  ChartReserved2(void)=0;
   virtual MTAPIRES  ChartReserved3(void)=0;
   virtual MTAPIRES  ChartReserved4(void)=0;
   //--- memory management
   virtual void*     Allocate(const uint32_t bytes)=0;
   virtual void      Free(void *ptr)=0;
   //--- spreads configuration
   virtual IMTConSpread* SpreadCreate(void)=0;
   virtual IMTConSpreadLeg* SpreadLegCreate(void)=0;
   virtual MTAPIRES  SpreadSubscribe(IMTConSpreadSink *sink)=0;
   virtual MTAPIRES  SpreadUnsubscribe(IMTConSpreadSink *sink)=0;
   virtual MTAPIRES  SpreadAdd(IMTConSpread *spread)=0;
   virtual MTAPIRES  SpreadDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SpreadShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SpreadTotal(void)=0;
   virtual MTAPIRES  SpreadNext(const uint32_t pos,IMTConSpread *spread)=0;
   virtual MTAPIRES  SpreadGet(const uint32_t id,IMTConSpread *spread)=0;
   virtual MTAPIRES  SpreadReserved1(void)=0;
   virtual MTAPIRES  SpreadReserved2(void)=0;
   virtual MTAPIRES  SpreadReserved3(void)=0;
   virtual MTAPIRES  SpreadReserved4(void)=0;
   //--- external trading system pending orders control
   virtual IMTOrderArray* GatewayOrderArrayCreate(void)=0;
   virtual MTAPIRES  GatewayOrdersAnswer(const MTAPIRES result,const int64_t orders_time,const IMTOrderArray *orders)=0;
   virtual MTAPIRES  GatewayOrdersReserved1(void)=0;
   virtual MTAPIRES  GatewayOrdersReserved2(void)=0;
   virtual MTAPIRES  GatewayOrdersReserved3(void)=0;
   virtual MTAPIRES  GatewayOrdersReserved4(void)=0;
   //--- external trading system trading accounts control
   virtual MTAPIRES  GatewayAccountAnswer(const MTAPIRES result,
                                          const int64_t accounts_time,
                                          const IMTUser *user,
                                          const IMTAccount *account,
                                          const IMTOrderArray *orders,
                                          const IMTPositionArray *positions)=0;
   virtual MTAPIRES  GatewayAccountRequest(const int64_t request_id,
                                           const IMTUser *user)=0;
   virtual MTAPIRES  GatewayAccountSet(const int64_t request_id,
                                       const IMTUser *user,
                                       const IMTAccount *account,
                                       const IMTOrderArray *orders,
                                       const IMTPositionArray *positions)=0;
   virtual MTAPIRES  GatewayAccountsReserved1(void)=0;
   virtual MTAPIRES  GatewayAccountsReserved2(void)=0;
   //--- platform server config
   virtual IMTConServer* NetServerCreate(void)=0;
   virtual IMTConServerRange* NetServerRangeCreate(void)=0;
   virtual MTAPIRES  NetServerSubscribe(IMTConServerSink* sink)=0;
   virtual MTAPIRES  NetServerUnsubscribe(IMTConServerSink* sink)=0;
   virtual uint32_t  NetServerTotal(void)=0;
   virtual MTAPIRES  NetServerNext(const uint32_t pos,IMTConServer* config)=0;
   virtual MTAPIRES  NetServerGet(const uint64_t id,IMTConServer* config)=0;
   virtual MTAPIRES  NetServerReserved1(void)=0;
   virtual MTAPIRES  NetServerReserved2(void)=0;
   virtual MTAPIRES  NetServerReserved3(void)=0;
   virtual MTAPIRES  NetServerReserved4(void)=0;
   //--- clients database
   virtual MTAPIRES  UserSubscribe(IMTUserSink* sink)=0;
   virtual MTAPIRES  UserUnsubscribe(IMTUserSink* sink)=0;
   virtual uint32_t  UserTotal(void)=0;
   virtual MTAPIRES  UserGet(const uint64_t login,IMTUser* user)=0;
   virtual MTAPIRES  UserGroup(const uint64_t login,MTAPISTR& group)=0;
   virtual MTAPIRES  UserLogins(uint64_t*& logins,uint32_t& logins_total)=0;
   virtual MTAPIRES  UserGetByAccount(LPCWSTR account,IMTUser* user)=0;
   virtual MTAPIRES  UserReserved1(void)=0;
   virtual MTAPIRES  UserReserved2(void)=0;
   virtual MTAPIRES  UserReserved3(void)=0;
   virtual MTAPIRES  UserReserved4(void)=0;
   virtual MTAPIRES  UserReserved5(void)=0;
   virtual MTAPIRES  UserReserved6(void)=0;
   virtual MTAPIRES  UserReserved7(void)=0;
   //--- gateway symbol configuration access
   virtual MTAPIRES  GatewaySymbolAdd(IMTConSymbol *symbol)=0;
   virtual uint32_t  GatewaySymbolTotal(void)=0;
   virtual MTAPIRES  GatewaySymbolNext(const uint32_t pos,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  GatewaySymbolGet(LPCWSTR name,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  GatewaySymbolDelete(LPCWSTR name)=0;
   virtual MTAPIRES  GatewaySymbolClear(void)=0;
   virtual MTAPIRES  GatewaySymbolReserver1(void)=0;
   virtual MTAPIRES  GatewaySymbolReserver2(void)=0;
   virtual MTAPIRES  GatewaySymbolReserver3(void)=0;
   virtual MTAPIRES  GatewaySymbolReserver4(void)=0;
   //--- ticks history
   virtual MTAPIRES  TickHistoryRequest(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickRate*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickHistoryRequestRaw(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickRate*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickHistoryAdd(LPCWSTR symbol,const MTTickRate* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickHistoryReplace(LPCWSTR symbol,const int64_t from_msc,const int64_t to_msc,const MTTickRate* ticks,const uint32_t ticks_total)=0;
   virtual MTAPIRES  TickHistoryReserver1(void)=0;
   virtual MTAPIRES  TickHistoryReserver2(void)=0;
   virtual MTAPIRES  TickHistoryReserver3(void)=0;
   virtual MTAPIRES  TickHistoryReserver4(void)=0;
   //--- common config
   virtual IMTConCommon* CommonCreate(void)=0;
   virtual MTAPIRES  CommonSubscribe(IMTConCommonSink* sink)=0;
   virtual MTAPIRES  CommonUnsubscribe(IMTConCommonSink* sink)=0;
   virtual MTAPIRES  CommonGet(IMTConCommon* common)=0;
   virtual MTAPIRES  CommonReserved1(void)=0;
   virtual MTAPIRES  CommonReserved2(void)=0;
   virtual MTAPIRES  CommonReserved3(void)=0;
   virtual MTAPIRES  CommonReserved4(void)=0;
   //--- explicit destructor is prohibited
protected:
   ~IMTGatewayAPI(void)
     {
     }
  };

//+------------------------------------------------------------------+
//| Entry point functions declarations                               |
//+------------------------------------------------------------------+
typedef /*__declspec(dllexport)*/ MTAPIRES(*MTGatewayVersion_t)          (uint32_t &version);
typedef /*__declspec(dllexport)*/ MTAPIRES(*MTGatewayCreate_t)           (MTGatewayInfo &info,IMTGatewayAPI **gateway);
typedef /*__declspec(dllexport)*/ MTAPIRES(*MTGatewayCreateLocal_t)      (MTGatewayInfo &info,IMTGatewayAPI **gateway,int32_t argc,wchar_t** argv);
typedef /*__declspec(dllexport)*/ MTAPIRES(*MTGatewayExceptionHandler_t) (LPCWSTR crash_path,LPCWSTR app_name,int32_t app_version,int32_t app_build,LPCWSTR app_build_date,int32_t app_build_revision,LPCWSTR mailbox);

//+------------------------------------------------------------------+
//| Entry point functions names                                      |
//+------------------------------------------------------------------+
static const LPCSTR s_MTGatewayVersion             ="MTGatewayVersion";
static const LPCSTR s_MTGatewayCreate              ="MTGatewayCreate";
static const LPCSTR s_MTGatewayCreateLocal         ="MTGatewayCreateLocal";
static const LPCSTR s_MTGatewayExceptionHandler    ="MTGatewayExceptionHandler";

//+------------------------------------------------------------------+
//| Gateway factory                                                  |
//+------------------------------------------------------------------+
class CMTGatewayAPIFactory
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
   MTGatewayVersion_t m_mtversion=nullptr;
   MTGatewayCreate_t m_mtcreate=nullptr;
   MTGatewayCreateLocal_t m_mtcreate_local=nullptr;
   MTGatewayExceptionHandler_t m_mtexceptions=nullptr;
   int32_t           m_avx_version=-1;

public:
                     CMTGatewayAPIFactory(void);
   //--- initialize & shutdown
   MTAPIRES          Initialize(void);
   MTAPIRES          Shutdown(void);

   MTAPIRES          ExceptionHandler(LPCWSTR crash_path,LPCWSTR app_name/*Client Desktop*/,int32_t app_version/*500*/,int32_t app_build/*3092*/,LPCWSTR app_build_date/*26 Oct 2021*/,int32_t app_build_revision/*12345*/,LPCWSTR mailbox/*crashes@company.com*/);

   MTAPIRES          Create(MTGatewayInfo &info,IMTGatewayAPI **gateway,int32_t argc=0,wchar_t** argv=nullptr) const;   // create instance
   static MTAPIRES   LicenseCheck(IMTGatewayAPI *gateway,LPCWSTR name);                                                 // check license signature
   //--- version info
   MTAPIRES          Version(uint32_t &version) const;                                                                  // version info
   void              VersionCPUID(int32_t (&cpu_info)[4],int32_t id) const;
   int32_t           VersionAVX(void);

private:
   bool              FindLibrary(wchar_t *path,const size_t path_maxlen);
  };

//+------------------------------------------------------------------+
//| Library constructor                                              |
//+------------------------------------------------------------------+
inline CMTGatewayAPIFactory::CMTGatewayAPIFactory(void)
  {
  }
//+------------------------------------------------------------------+
//| Library initialization                                           |
//+------------------------------------------------------------------+
inline MTAPIRES CMTGatewayAPIFactory::Initialize(void)
  {
   wchar_t path[MAX_PATH]={};
//--- find the Gateway API DLL
   if(!FindLibrary(path,_countof(path)-1))
      return(MT_RET_ERR_NOTFOUND);
//--- try to load Gateway API DLL from current folder
   if((m_hmodule=::LoadLibraryW(path))==NULL)
      return(MT_RET_ERR_NOTFOUND);
//--- find entry point addresses
   m_mtversion     =reinterpret_cast<MTGatewayVersion_t>(::GetProcAddress(m_hmodule,s_MTGatewayVersion));
   m_mtcreate      =reinterpret_cast<MTGatewayCreate_t>(::GetProcAddress(m_hmodule,s_MTGatewayCreate));
   m_mtcreate_local=reinterpret_cast<MTGatewayCreateLocal_t>(::GetProcAddress(m_hmodule,s_MTGatewayCreateLocal));
   m_mtexceptions  =reinterpret_cast<MTGatewayExceptionHandler_t>(::GetProcAddress(m_hmodule,s_MTGatewayExceptionHandler));
//--- check results
   if(!m_mtversion || !m_mtcreate || !m_mtcreate_local)
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
inline MTAPIRES CMTGatewayAPIFactory::Shutdown(void)
  {
//--- check & free
   if(m_hmodule)
     {
      ::FreeLibrary((HMODULE)m_hmodule);
      m_hmodule=NULL;
     }
//--- zero members
   m_mtversion     =NULL;
   m_mtcreate      =nullptr;
   m_mtcreate_local=nullptr;
   m_mtexceptions  =nullptr;
//--- ok
   return(MT_RET_OK);
  }

//+------------------------------------------------------------------+
//| Library initialization                                           |
//+------------------------------------------------------------------+
inline MTAPIRES CMTGatewayAPIFactory::ExceptionHandler(LPCWSTR crash_path,LPCWSTR app_name/*Client Desktop*/,int32_t app_version/*500*/,int32_t app_build/*3092*/,LPCWSTR app_build_date/*26 Oct 2021*/,int32_t app_build_revision/*12345*/,LPCWSTR mailbox/*crashes@company.com*/)
  {
//--- check members & parameters
   if(!m_hmodule || !m_mtexceptions)
      return(MT_RET_ERROR);
//--- setup exception handlers
   return m_mtexceptions(crash_path,app_name,app_version,app_build,app_build_date,app_build_revision,mailbox);
  }

//+------------------------------------------------------------------+
//| Gateway interface creation                                       |
//+------------------------------------------------------------------+
inline MTAPIRES CMTGatewayAPIFactory::Create(MTGatewayInfo& info,IMTGatewayAPI **gateway,int32_t argc/*=0*/,wchar_t** argv/*=NULL*/) const
  {
   MTAPIRES res=MT_RET_ERROR;
//--- check members & parameters
   if(!m_hmodule || !m_mtcreate || !m_mtcreate_local || !gateway)
      return(MT_RET_ERROR);
//--- call common or local-mode creation
   if(!argc || !argv)
      res=m_mtcreate(info,gateway);
   else
      res=m_mtcreate_local(info,gateway,argc,argv);
//--- result
   return(res);
  }

//+------------------------------------------------------------------+
//| Gateway license sign check                                       |
//+------------------------------------------------------------------+
inline MTAPIRES CMTGatewayAPIFactory::LicenseCheck(IMTGatewayAPI *gateway,LPCWSTR name)
  {
//--- checking
   if(!gateway || !name || name[0]==L'\0')
      return(MT_RET_ERROR);
//--- fill license information, copy name
   MTLicenseCheck license={};

   wcsncpy_s(license.name,_countof(license.name),name,_TRUNCATE);
   CMTStr::Terminate(license.name);
//--- generate sequence of random numbers
   license.random_size=sizeof(license.random);

   srand(GetTickCount64() & 0x7fffffff);
   for(license.random_size=0;license.random_size<sizeof(license.random);license.random_size++)
      license.random[license.random_size]=char(rand()&0xFF);
//--- check license
   MTLicenseCheck license_check;
   MTAPIRES       res;

   memcpy(&license_check,&license,sizeof(license));
   res=gateway->LicenseCheck(license_check);
   if(res!=MT_RET_OK)
      return(res);

   if(license_check.retcode!=MT_RET_OK)
      return(license_check.retcode);
//--- check license sign, initialize provider
   HCRYPTPROV prov=NULL;
   HCRYPTHASH hash=NULL;
   HCRYPTKEY  key =NULL;

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
//| Find the library                                                 |
//+------------------------------------------------------------------+
inline bool CMTGatewayAPIFactory::FindLibrary(wchar_t *path,const size_t path_maxlen)
  {
   LPCWSTR library_default=L"MT5APIGateway64.dll";
   LPCWSTR library=library_default;
//--- detect architecture
#ifdef _M_ARM64
   library=L"MT5APIGateway64arm.dll";
#else
   const int32_t avx=VersionAVX();

   if(avx>1)
      library=L"MT5APIGateway64avx2.dll";
   else
      if(avx>0)
         library=L"MT5APIGateway64avx.dll";
#endif
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
//| Version                                                          |
//+------------------------------------------------------------------+
inline MTAPIRES CMTGatewayAPIFactory::Version(uint32_t &version) const
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
inline void CMTGatewayAPIFactory::VersionCPUID(int32_t (&cpu_info)[4],int32_t id) const
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
inline int32_t CMTGatewayAPIFactory::VersionAVX(void)
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
