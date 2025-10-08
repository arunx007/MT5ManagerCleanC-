//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Gateway module config                                            |
//+------------------------------------------------------------------+
class IMTConGatewayModule
  {
public:
   //--- changeable fields flags
   enum EnGatewayFieldMask
     {
      GATEWAY_FIELD_SERVER=1,         // server field
      GATEWAY_FIELD_LOGIN =2,         // login field
      GATEWAY_FIELD_PASS  =4,         // password field
      GATEWAY_FIELD_PARAM =8,         // parameters
      //--- enumeration borders
      GATEWAY_FIELD_NONE  =0,
      GATEWAY_FIELD_ALL   =GATEWAY_FIELD_SERVER|GATEWAY_FIELD_LOGIN|GATEWAY_FIELD_PASS|GATEWAY_FIELD_PARAM
     };

public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConGatewayModule* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- default gateway name
   virtual LPCWSTR   Name(void) const=0;
   //--- vendor name
   virtual LPCWSTR   Vendor(void) const=0;
   //--- gateway description
   virtual LPCWSTR   Description(void) const=0;
   //--- gateway file name
   virtual LPCWSTR   Module(void) const=0;
   //--- gateway default server address for trading
   virtual LPCWSTR   TradingServer(void) const=0;
   //--- gateway default login for trading
   virtual LPCWSTR   TradingLogin(void) const=0;
   //--- gateway default password for trading
   virtual LPCWSTR   TradingPassword(void) const=0;
   //--- gateway version
   virtual uint32_t  Version(void) const=0;
   //--- gateway available flags (IMTConGateway::EnGatewayFlags)
   virtual uint32_t  Flags(void) const=0;
   //--- changeable EnFeedersFieldFlags
   virtual uint32_t  Fields(void) const=0;
   //--- default gateway parameters
   virtual uint32_t  ParameterTotal(void) const=0;
   virtual MTAPIRES  ParameterNext(const uint32_t pos,IMTConParam* param) const=0;
   virtual MTAPIRES  ParameterGet(LPCWSTR name,IMTConParam* param) const=0;
   //--- gateway given name
   virtual LPCWSTR   Gateway(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConGatewayModule(void) {}
  };
//+------------------------------------------------------------------+
//| Symbols name and price translation config                        |
//+------------------------------------------------------------------+
class IMTConGatewayTranslate
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConGatewayTranslate* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- symbol name in datafeed
   virtual LPCWSTR   Source(void) const=0;
   virtual MTAPIRES  Source(LPCWSTR source)=0;
   //--- symbol name in MT5
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- bid markup in points
   virtual INT       BidMarkup(void) const=0;
   virtual MTAPIRES  BidMarkup(const INT markup)=0;
   //--- ask markup in points
   virtual INT       AskMarkup(void) const=0;
   virtual MTAPIRES  AskMarkup(const INT markup)=0;
   //--- digits
   virtual uint32_t  Digits(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConGatewayTranslate(void) {}
  };
//+------------------------------------------------------------------+
//| Gateway config interface                                         |
//+------------------------------------------------------------------+
class IMTConGateway
  {
public:
   //--- gateway working flags
   enum EnGatewayFlags
     {
      GATEWAY_FLAG_REMOTE         =0x00000001,       // gateway works as remote service
      GATEWAY_FLAG_IMPORT_SYMBOLS =0x00000002,       // gateway can import symbols to MetaTrader platform
      GATEWAY_FLAG_IGNORE_QUOTES  =0x00000004,       // ignore quotes from gateway
      GATEWAY_FLAG_IMPORT_BALANCES=0x00000008,       // gateway can import client balances to MetaTrader platform
      GATEWAY_FLAG_EXTENDED_LOG   =0x00000010,       // gateway writes extended log
      GATEWAY_FLAG_SUPP_POSITIONS =0x00000020,       // gateway support request of trading positions from external trading system
      GATEWAY_FLAG_PROFILLING     =0x00000040,       // gateway support request of trading positions from external trading system
      GATEWAY_FLAG_TRIAL          =0x00000100,       // gateway in trial mode
      GATEWAY_FLAG_INTERNAL       =0x00000200,       // gateway works as part of history server
      //--- flags borders
      GATEWAY_FLAG_NONE           =0,
      GATEWAY_FLAG_ALL            =GATEWAY_FLAG_REMOTE|GATEWAY_FLAG_IMPORT_SYMBOLS|
      GATEWAY_FLAG_IGNORE_QUOTES| GATEWAY_FLAG_IMPORT_BALANCES| GATEWAY_FLAG_EXTENDED_LOG | GATEWAY_FLAG_SUPP_POSITIONS |
      GATEWAY_FLAG_PROFILLING | GATEWAY_FLAG_TRIAL | GATEWAY_FLAG_INTERNAL
     };
   //--- gateway modes
   enum EnGatewayMode
     {
      GATEWAY_DISABLED  =0,
      GATEWAY_ENABLED   =1,
      //--- enumeration borders
      GATEWAY_FIRST     =GATEWAY_DISABLED,
      GATEWAY_LAST      =GATEWAY_ENABLED
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConGateway* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- gateway name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- gateway trade request routing id (dealer id)
   virtual uint64_t    ID(void) const=0;
   virtual MTAPIRES  ID(const uint64_t id)=0;
   //--- gateway module name
   virtual LPCWSTR   Module(void) const=0;
   virtual MTAPIRES  Module(LPCWSTR name)=0;
   //--- gateway server address for trading
   virtual LPCWSTR   TradingServer(void) const=0;
   virtual MTAPIRES  TradingServer(LPCWSTR server)=0;
   //--- gateway login for trading
   virtual LPCWSTR   TradingLogin(void) const=0;
   virtual MTAPIRES  TradingLogin(LPCWSTR login)=0;
   //--- gateway password for trading
   virtual LPCWSTR   TradingPassword(void) const=0;
   virtual MTAPIRES  TradingPassword(LPCWSTR password)=0;
   //--- gateway server address
   virtual LPCWSTR   GatewayServer(void) const=0;
   virtual MTAPIRES  GatewayServer(LPCWSTR server)=0;
   //--- EnGatewayMode
   virtual uint32_t  Mode(void) const=0;
   virtual MTAPIRES  Mode(const uint32_t mode)=0;
   //--- EnGatewayFlags
   virtual uint32_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint32_t flags)=0;
   //--- obsolete value
   virtual uint32_t  ObsoleteValue(void) const=0;
   virtual MTAPIRES  ObsoleteValue(const uint32_t value)=0;
   //--- gateway reconnect timeout
   virtual uint32_t  TimeoutReconnect(void) const=0;
   virtual MTAPIRES  TimeoutReconnect(const uint32_t timeout)=0;
   //--- gateway sleep timeout
   virtual uint32_t  TimeoutSleep(void) const=0;
   virtual MTAPIRES  TimeoutSleep(const uint32_t timeout)=0;
   //--- gateway attempts before timeout
   virtual uint32_t  TimeoutAttempts(void) const=0;
   virtual MTAPIRES  TimeoutAttempts(const uint32_t attempts)=0;
   //--- gateway additional parameters access
   virtual MTAPIRES  ParameterAdd(IMTConParam* param)=0;
   virtual MTAPIRES  ParameterUpdate(const uint32_t pos,const IMTConParam* param)=0;
   virtual MTAPIRES  ParameterDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ParameterClear(void)=0;
   virtual MTAPIRES  ParameterShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ParameterTotal(void) const=0;
   virtual MTAPIRES  ParameterNext(const uint32_t pos,IMTConParam* param) const=0;
   virtual MTAPIRES  ParameterGet(LPCWSTR name,IMTConParam* param) const=0;
   //--- list of symbols for translating quotes
   virtual MTAPIRES  SymbolAdd(LPCWSTR path)=0;
   virtual MTAPIRES  SymbolUpdate(const uint32_t pos,LPCWSTR path)=0;
   virtual MTAPIRES  SymbolDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SymbolClear(void)=0;
   virtual MTAPIRES  SymbolShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SymbolTotal(void) const=0;
   virtual LPCWSTR   SymbolNext(const uint32_t pos) const=0;
   //--- list of users groups who works through gateway
   virtual MTAPIRES  GroupAdd(LPCWSTR path)=0;
   virtual MTAPIRES  GroupUpdate(const uint32_t pos,LPCWSTR path)=0;
   virtual MTAPIRES  GroupDelete(const uint32_t pos)=0;
   virtual MTAPIRES  GroupClear(void)=0;
   virtual MTAPIRES  GroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  GroupTotal(void) const=0;
   virtual LPCWSTR   GroupNext(const uint32_t pos) const=0;
   //--- list of symbols translations
   virtual MTAPIRES  TranslateAdd(IMTConGatewayTranslate* param)=0;
   virtual MTAPIRES  TranslateUpdate(const uint32_t pos,const IMTConGatewayTranslate* param)=0;
   virtual MTAPIRES  TranslateDelete(const uint32_t pos)=0;
   virtual MTAPIRES  TranslateClear(void)=0;
   virtual MTAPIRES  TranslateShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  TranslateTotal(void) const=0;
   virtual MTAPIRES  TranslateNext(const uint32_t pos,IMTConGatewayTranslate* param) const=0;
   virtual MTAPIRES  TranslateGet(LPCWSTR symbol,IMTConGatewayTranslate* param) const=0;
   //--- gateway server login
   virtual uint64_t    GatewayLogin(void) const=0;
   virtual MTAPIRES  GatewayLogin(uint64_t login)=0;
   //--- gateway server password
   virtual LPCWSTR   GatewayPassword(void) const=0;
   virtual MTAPIRES  GatewayPassword(LPCWSTR password)=0;
   //--- list of symbols translations
   virtual MTAPIRES  TranslateGetSource(LPCWSTR source,IMTConGatewayTranslate* param) const=0;
   //--- gateway given name
   virtual LPCWSTR   Gateway(void) const=0;
   //--- gateway state information
   virtual bool      StateConnected(void) const=0;
   virtual uint32_t  StateReceivedTicks(void) const=0;
   virtual uint32_t  StateReceivedBooks(void) const=0;
   virtual uint32_t  StateTrafficIn(void) const=0;
   virtual uint32_t  StateTrafficOut(void) const=0;
   virtual uint32_t  StateTradesTotal(void) const=0;
   virtual uint32_t  StateTradesAverageTime(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConGateway(void) {}
  };
//+------------------------------------------------------------------+
//| Gateway config events notification interface                     |
//+------------------------------------------------------------------+
class IMTConGatewaySink
  {
public:
   virtual void      OnGatewayAdd(const IMTConGateway*    /*gateway*/) {  }
   virtual void      OnGatewayUpdate(const IMTConGateway* /*gateway*/) {  }
   virtual void      OnGatewayDelete(const IMTConGateway* /*gateway*/) {  }
   virtual void      OnGatewaySync(void)                               {  }
  };
//+------------------------------------------------------------------+

