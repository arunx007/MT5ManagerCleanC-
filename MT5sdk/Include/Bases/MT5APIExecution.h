//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade account interface                                          |
//+------------------------------------------------------------------+
class IMTExecution
  {
public:
   //--- execution state
   enum EnTradeExecutions
     {
      TE_ORDER_FIRST               =0,
      //--- order filling
      TE_ORDER_NEW_REQUEST         =0,
      TE_ORDER_NEW                 =1,
      TE_ORDER_FILL                =2,
      TE_ORDER_REJECT              =3,
      //--- order modify
      TE_ORDER_MODIFY_REQUEST      =4,
      TE_ORDER_MODIFY              =5,
      TE_ORDER_MODIFY_REJECT       =6,
      //--- order cancel
      TE_ORDER_CANCEL_REQUEST      =7,
      TE_ORDER_CANCEL              =8,
      TE_ORDER_CANCEL_REJECT       =9,
      //--- order others
      TE_ORDER_CHANGE_ID           =10,
      TE_ORDER_CLOSE_BY            =11,
      TE_ORDER_EXPIRE              =12,
      TE_ORDER_LAST                =TE_ORDER_EXPIRE,
      //--- deal modifications
      TE_DEAL_FIRST                =50,
      TE_DEAL_CANCEL               =50,
      TE_DEAL_CORRECT              =51,
      TE_DEAL_EXTERNAL             =52,
      TE_DEAL_REPO                 =53,
      TE_DEAL_COMMISSION           =54,
      TE_DEAL_LAST                 =TE_DEAL_COMMISSION,
      //--- end of session executions
      TE_EOS_FIRST                 =100,
      TE_EOS_CANCEL_DAILY_ORDERS   =100,
      TE_EOS_VARIATION_MARGIN      =101,
      TE_EOS_RECALC_DEALS          =102,
      TE_EOS_SETTLEMENT            =103,
      TE_EOS_TRANSFER              =104,
      TE_EOS_CANCEL_ALL_ORDERS     =105,
      TE_EOS_ROLLOVER              =106,
      TE_EOS_LAST                  =TE_EOS_ROLLOVER,
      //--- balance operations
      TE_BALANCE_FIRST             =150,
      TE_BALANCE_CHANGE            =150,
      TE_BALANCE_CORRECT           =151,
      TE_BALANCE_UNBLOCK_PROFIT    =152,
      TE_BALANCE_LAST              =TE_BALANCE_UNBLOCK_PROFIT,
      //--- position modifications
      TE_POSITION_FIRST            =200,
      TE_POSITION_CHANGE_ID        =201,
      TE_POSITION_CHANGE_SLTP      =202,
      TE_POSITION_CHANGE_SLTP_REJECT=203,
      TE_POSITION_LAST             =TE_POSITION_CHANGE_SLTP_REJECT,
      //--- enumeration borders
      TE_FIRST                     =TE_ORDER_FIRST,
      TE_LAST                      =TE_POSITION_LAST
     };
   //--- execution flags
   enum EnFlags
     {
      TE_FLAG_NONE                 =0x00000000,  // none
      TE_FLAG_BROADCAST            =0x00000001,  // broadcast execution will be send to all trade servers
      TE_FLAG_MARKUP               =0x00000002,  // gateway settings request to markup prices
      TE_FLAG_SETTLEMENT_COMMISSION=0x00000004,  // charge commissions on setllement - TE_EOS_SETTLEMENT
      //--- enumeration borders
      TE_FLAG_ALL                  =TE_FLAG_BROADCAST|TE_FLAG_MARKUP|TE_FLAG_SETTLEMENT_COMMISSION
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTExecution* exec)=0;
   virtual MTAPIRES  Clear(void)=0;
   virtual LPCWSTR   Print(MTAPISTR& string) const=0;
   //--- id
   virtual uint64_t  ID(void) const=0;
   virtual MTAPIRES  ID(const uint64_t id)=0;
   //--- execution id in external system (exchange, ECN, etc)
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- execution action
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t action)=0;
   //--- datetime
   virtual int64_t   Datetime(void) const=0;
   virtual MTAPIRES  Datetime(const int64_t datetime)=0;
   //--- login
   virtual uint64_t  Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- group
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
   //--- flags
   virtual uint64_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- digits
   virtual uint32_t  Digits(void) const=0;
   //--- comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- MT5 order ticket
   virtual uint64_t  Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- order ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   OrderExternalID(void) const=0;
   virtual MTAPIRES  OrderExternalID(LPCWSTR id)=0;
   //--- order type IMTOrder::EnOrderType
   virtual uint32_t  OrderType(void) const=0;
   virtual MTAPIRES  OrderType(const uint32_t type)=0;
   //--- order volume
   virtual uint64_t  OrderVolume(void) const=0;
   virtual MTAPIRES  OrderVolume(const uint64_t volume)=0;
   //--- order price
   virtual double    OrderPrice(void) const=0;
   virtual MTAPIRES  OrderPrice(const double price)=0;
   //--- order activation flags
   virtual uint32_t  OrderActivationFlags(void) const=0;
   virtual MTAPIRES  OrderActivationFlags(const uint32_t activation)=0;
   //--- deal ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   DealExternalID(void) const=0;
   virtual MTAPIRES  DealExternalID(LPCWSTR id)=0;
   //--- deal action IMTDeal::EnDealAction
   virtual uint32_t  DealAction(void) const=0;
   virtual MTAPIRES  DealAction(const uint32_t action)=0;
   //--- deal volume
   virtual uint64_t  DealVolume(void) const=0;
   virtual MTAPIRES  DealVolume(const uint64_t volume)=0;
   //--- unfilled order volume
   virtual uint64_t  DealVolumeRemaind(void) const=0;
   virtual MTAPIRES  DealVolumeRemaind(const uint64_t volume)=0;
   //--- deal price
   virtual double    DealPrice(void) const=0;
   virtual MTAPIRES  DealPrice(const double price)=0;
   //--- digits set
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- external system trade account (exchange, ECN, etc)
   virtual LPCWSTR   ExternalAccount(void) const=0;
   virtual MTAPIRES  ExternalAccount(LPCWSTR account)=0;
   //--- order trigger price
   virtual double    OrderPriceTrigger(void) const=0;
   virtual MTAPIRES  OrderPriceTrigger(const double price)=0;
   //--- EnOrderTime
   virtual uint32_t  OrderTypeTime(void) const=0;
   virtual MTAPIRES  OrderTypeTime(const uint32_t type)=0;
   //--- order expiration
   virtual int64_t   OrderTimeExpiration(void) const=0;
   virtual MTAPIRES  OrderTimeExpiration(const int64_t time)=0;
   //--- EnOrderFilling
   virtual uint32_t  OrderTypeFill(void) const=0;
   virtual MTAPIRES  OrderTypeFill(const uint32_t type)=0;
   //--- session start datetime
   virtual int64_t   EOSSessionStart(void) const=0;
   virtual MTAPIRES  EOSSessionStart(const int64_t start)=0;
   //--- session end datetime
   virtual int64_t   EOSSessionEnd(void) const=0;
   virtual MTAPIRES  EOSSessionEnd(const int64_t end)=0;
   //--- session settlement price
   virtual double    EOSPriceSettlement(void) const=0;
   virtual MTAPIRES  EOSPriceSettlement(const double price)=0;
   //--- profit conversion price
   virtual double    EOSProfitRateBuy(void) const=0;
   virtual double    EOSProfitRateSell(void) const=0;
   virtual MTAPIRES  EOSProfitRate(const double rate_buy,const double rate_sell)=0;
   //--- tick value
   virtual double    EOSTickValue(void) const=0;
   virtual MTAPIRES  EOSTickValue(const double value)=0;
   //--- order price SL
   virtual double    OrderPriceSL(void) const=0;
   virtual MTAPIRES  OrderPriceSL(const double price)=0;
   //--- order price TP
   virtual double    OrderPriceTP(void) const=0;
   virtual MTAPIRES  OrderPriceTP(const double price)=0;
   //--- execution price on gateway
   virtual double    PriceGateway(void) const=0;
   virtual MTAPIRES  PriceGateway(const double price)=0;
   //--- order activation flags
   virtual uint32_t  OrderActivationMode(void) const=0;
   virtual MTAPIRES  OrderActivationMode(const uint32_t activation)=0;
   //--- deal commission
   virtual double    DealCommission(void) const=0;
   virtual MTAPIRES  DealCommission(const double comm)=0;
   //--- datetime in us since 1970.01.01
   virtual int64_t   DatetimeMsc(void) const=0;
   virtual MTAPIRES  DatetimeMsc(const int64_t datetime)=0;
   //--- symbol field new value
   virtual LPCWSTR   SymbolNew(void) const=0;
   virtual MTAPIRES  SymbolNew(LPCWSTR symbol)=0;
   //--- internal data for API usage
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,double& value) const=0;
   //--- deal storage
   virtual double    DealStorage(void) const=0;
   virtual MTAPIRES  DealStorage(const double storage)=0;
   //--- rollover values
   virtual double    EOSRolloverValueLong(void) const=0;
   virtual double    EOSRolloverValueShort(void) const=0;
   virtual MTAPIRES  EOSRolloverValue(const double value_long,const double value_short)=0;
   //--- deal reason
   virtual uint32_t  DealReason(void) const=0;
   virtual MTAPIRES  DealReason(const uint32_t reason)=0;
   //--- gateway id
   virtual uint64_t  GatewayID(void) const=0;
   virtual MTAPIRES  GatewayID(const uint64_t gateway_id)=0;
   //--- MT5 position ticket
   virtual uint64_t  Position(void) const=0;
   virtual MTAPIRES  Position(const uint64_t position)=0;
   //--- MT5 position ticket
   virtual uint64_t  PositionBy(void) const=0;
   virtual MTAPIRES  PositionBy(const uint64_t position)=0;
   //--- position ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   PositionExternalID(void) const=0;
   virtual MTAPIRES  PositionExternalID(LPCWSTR id)=0;
   //--- position ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   PositionByExternalID(void) const=0;
   virtual MTAPIRES  PositionByExternalID(LPCWSTR id)=0;
   //--- external trade system return code
   virtual int32_t   ExternalRetcode(void) const=0;
   virtual MTAPIRES  ExternalRetcode(const int32_t retcode)=0;
   //--- order volume with extendede accuracy
   virtual uint64_t  OrderVolumeExt(void) const=0;
   virtual MTAPIRES  OrderVolumeExt(const uint64_t volume)=0;
   //--- deal volume with extendede accuracy
   virtual uint64_t  DealVolumeExt(void) const=0;
   virtual MTAPIRES  DealVolumeExt(const uint64_t volume)=0;
   //--- unfilled order volume with extendede accuracy
   virtual uint64_t  DealVolumeRemaindExt(void) const=0;
   virtual MTAPIRES  DealVolumeRemaindExt(const uint64_t volume)=0;
   //--- internal data for API usage
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,double& value) const=0;
   virtual MTAPIRES  ApiDataRawSet(const void *data,const uint32_t datalen)=0;
   virtual LPCVOID   ApiDataRawGet(void) const=0;
   virtual uint32_t  ApiDataRawMax(void) const=0;
   virtual MTAPIRES  ApiDataClear(const uint16_t app_id)=0;
   virtual MTAPIRES  ApiDataClearAll(void)=0;
   //--- position price SL
   virtual double    PositionPriceSL(void) const=0;
   virtual MTAPIRES  PositionPriceSL(const double price)=0;
   //--- position price TP
   virtual double    PositionPriceTP(void) const=0;
   virtual MTAPIRES  PositionPriceTP(const double price)=0;
   //--- confirmed gateway volume with extended accuracy
   virtual uint64_t  VolumeGatewayExt(void) const=0;
   virtual MTAPIRES  VolumeGatewayExt(const uint64_t volume)=0;
   //--- confirmed gateway deal type
   virtual uint32_t  ActionGateway(void) const=0;
   virtual MTAPIRES  ActionGateway(const uint32_t action)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTExecution(void) {}
  };
//+------------------------------------------------------------------+
