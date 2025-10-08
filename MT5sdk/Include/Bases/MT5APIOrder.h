//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade Order Interface                                            |
//+------------------------------------------------------------------+
class IMTOrder
  {
public:
   //--- order types
   enum EnOrderType
     {
      OP_BUY                   =0,     // buy order
      OP_SELL                  =1,     // sell order
      OP_BUY_LIMIT             =2,     // buy limit order
      OP_SELL_LIMIT            =3,     // sell limit order
      OP_BUY_STOP              =4,     // buy stop order
      OP_SELL_STOP             =5,     // sell stop order
      OP_BUY_STOP_LIMIT        =6,     // buy stop limit order
      OP_SELL_STOP_LIMIT       =7,     // sell stop limit order
      OP_CLOSE_BY              =8,     // close by
      //--- enumeration borders
      OP_FIRST                 =OP_BUY,
      OP_LAST                  =OP_CLOSE_BY
     };
   //--- order filling types
   enum EnOrderFilling
     {
      ORDER_FILL_FOK           =0,     // fill or kill
      ORDER_FILL_IOC           =1,     // immediate or cancel
      ORDER_FILL_RETURN        =2,     // return order in queue
      ORDER_FILL_BOC           =3,     // book or cancel
      //--- enumeration borders
      ORDER_FILL_FIRST         =ORDER_FILL_FOK,
      ORDER_FILL_LAST          =ORDER_FILL_BOC
     };
   //--- order expiration types
   enum EnOrderTime
     {
      ORDER_TIME_GTC           =0,     // good till cancel
      ORDER_TIME_DAY           =1,     // good till day
      ORDER_TIME_SPECIFIED     =2,     // good till specified
      ORDER_TIME_SPECIFIED_DAY =3,     // good till specified day
      //--- enumeration borders
      ORDER_TIME_FIRST         =ORDER_TIME_GTC,
      ORDER_TIME_LAST          =ORDER_TIME_SPECIFIED_DAY
     };
   //--- order state
   enum EnOrderState
     {
      ORDER_STATE_STARTED      =0,     // order started
      ORDER_STATE_PLACED       =1,     // order placed in system
      ORDER_STATE_CANCELED     =2,     // order canceled by client
      ORDER_STATE_PARTIAL      =3,     // order partially filled
      ORDER_STATE_FILLED       =4,     // order filled
      ORDER_STATE_REJECTED     =5,     // order rejected
      ORDER_STATE_EXPIRED      =6,     // order expired
      ORDER_STATE_REQUEST_ADD  =7,     // order requested to add
      ORDER_STATE_REQUEST_MODIFY=8,    // order requested to modify
      ORDER_STATE_REQUEST_CANCEL=9,    // order requested to cancel
      //--- enumeration borders
      ORDER_STATE_FIRST        =ORDER_STATE_STARTED,
      ORDER_STATE_LAST         =ORDER_STATE_REQUEST_CANCEL
     };
   //--- order activation state
   enum EnOrderActivation
     {
      ACTIVATION_NONE          =0,     // none
      ACTIVATION_PENDING       =1,     // pending order activated
      ACTIVATION_STOPLIMIT     =2,     // stop-limit order activated
      ACTIVATION_EXPIRATION    =3,     // order expired
      ACTIVATION_STOPOUT       =4,     // order activate for stop-out
      //--- enumeration borders
      ACTIVATION_FIRST         =ACTIVATION_NONE,
      ACTIVATION_LAST          =ACTIVATION_STOPOUT
     };
   //--- order creation reasons
   enum EnOrderReason
     {
      ORDER_REASON_CLIENT      =0,     // order placed manually
      ORDER_REASON_EXPERT      =1,     // order placed by expert
      ORDER_REASON_DEALER      =2,     // order placed by dealer
      ORDER_REASON_SL          =3,     // order placed due SL
      ORDER_REASON_TP          =4,     // order placed due TP
      ORDER_REASON_SO          =5,     // order placed due Stop-Out
      ORDER_REASON_ROLLOVER    =6,     // order placed due rollover
      ORDER_REASON_EXTERNAL_CLIENT =7, // order placed from the external system by client
      ORDER_REASON_VMARGIN     =8,     // order placed due variation margin
      ORDER_REASON_GATEWAY     =9,     // order placed by gateway
      ORDER_REASON_SIGNAL      =10,    // order placed by signal service
      ORDER_REASON_SETTLEMENT  =11,    // order placed due to settlement
      ORDER_REASON_TRANSFER    =12,    // order placed due to transfer
      ORDER_REASON_SYNC        =13,    // order placed due to synchronization
      ORDER_REASON_EXTERNAL_SERVICE=14,// order placed due to service in external system
      ORDER_REASON_MIGRATION   =15,    // order placed due account migration from MetaTrader 4 or MetaTrader 5
      ORDER_REASON_MOBILE      =16,    // order placed manually by mobile terminal
      ORDER_REASON_WEB         =17,    // order placed manually by web terminal
      ORDER_REASON_SPLIT       =18,    // order placed due to split
      ORDER_REASON_CORPORATE_ACTION=19,// placed due to corporate action
      //--- enumeration borders
      ORDER_REASON_FIRST        =ORDER_REASON_CLIENT,
      ORDER_REASON_LAST         =ORDER_REASON_CORPORATE_ACTION
     };
   //--- order activation flags
   enum EnTradeActivationFlags
     {
      ACTIV_FLAGS_NO_LIMIT     =0x00000001,
      ACTIV_FLAGS_NO_STOP      =0x00000002,
      ACTIV_FLAGS_NO_SLIMIT    =0x00000004,
      ACTIV_FLAGS_NO_SL        =0x00000008,
      ACTIV_FLAGS_NO_TP        =0x00000010,
      ACTIV_FLAGS_NO_SO        =0x00000020,
      ACTIV_FLAGS_NO_EXPIRATION=0x00000040,
      //--- enumeration borders
      ACTIV_FLAGS_NONE         =0x00000000,
      ACTIV_FLAGS_ALL          =ACTIV_FLAGS_NO_LIMIT|ACTIV_FLAGS_NO_STOP|ACTIV_FLAGS_NO_SLIMIT|ACTIV_FLAGS_NO_SL|
      ACTIV_FLAGS_NO_TP|ACTIV_FLAGS_NO_SO|ACTIV_FLAGS_NO_EXPIRATION
     };
   //--- modification flags
   enum EnTradeModifyFlags
     {
      MODIFY_FLAGS_ADMIN       =0x00000001,
      MODIFY_FLAGS_MANAGER     =0x00000002,
      MODIFY_FLAGS_POSITION    =0x00000004,
      MODIFY_FLAGS_RESTORE     =0x00000008,
      MODIFY_FLAGS_API_ADMIN   =0x00000010,
      MODIFY_FLAGS_API_MANAGER =0x00000020,
      MODIFY_FLAGS_API_SERVER  =0x00000040,
      MODIFY_FLAGS_API_GATEWAY =0x00000080,
      //--- enumeration borders
      MODIFY_FLAGS_NONE        =0x00000000,
      MODIFY_FLAGS_ALL         =MODIFY_FLAGS_ADMIN|MODIFY_FLAGS_MANAGER|MODIFY_FLAGS_POSITION|MODIFY_FLAGS_RESTORE|
      MODIFY_FLAGS_API_ADMIN|MODIFY_FLAGS_API_MANAGER|MODIFY_FLAGS_API_SERVER|MODIFY_FLAGS_API_GATEWAY
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTOrder* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   virtual LPCWSTR   Print(MTAPISTR& string) const=0;
   //--- order ticket
   virtual uint64_t    Order(void) const=0;
   //--- order ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t order)=0;
   //--- processed dealer login (0-means auto)
   virtual uint64_t    Dealer(void) const=0;
   virtual MTAPIRES  Dealer(const uint64_t dealer)=0;
   //--- order symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- currency digits
   virtual uint32_t  DigitsCurrency(void) const=0;
   virtual MTAPIRES  DigitsCurrency(const uint32_t digits)=0;
   //--- contract size
   virtual double    ContractSize(void) const=0;
   virtual MTAPIRES  ContractSize(const double contract_size)=0;
   //--- EnOrderState
   virtual uint32_t  State(void) const=0;
   //--- EnOrderReason
   virtual uint32_t  Reason(void) const=0;
   //--- order setup time
   virtual int64_t     TimeSetup(void) const=0;
   virtual MTAPIRES  TimeSetup(const int64_t time)=0;
   //--- order expiration
   virtual int64_t     TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t time)=0;
   //--- order filling/cancel time
   virtual int64_t     TimeDone(void) const=0;
   virtual MTAPIRES  TimeDone(const int64_t time)=0;
   //--- EnOrderType
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- EnOrderFilling
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- EnOrderTime
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //--- order price
   virtual double    PriceOrder(void) const=0;
   virtual MTAPIRES  PriceOrder(const double price)=0;
   //--- order trigger price (stop-limit price)
   virtual double    PriceTrigger(void) const=0;
   virtual MTAPIRES  PriceTrigger(const double price)=0;
   //--- order current price
   virtual double    PriceCurrent(void) const=0;
   virtual MTAPIRES  PriceCurrent(const double price)=0;
   //--- order SL
   virtual double    PriceSL(void) const=0;
   virtual MTAPIRES  PriceSL(const double price)=0;
   //--- order TP
   virtual double    PriceTP(void) const=0;
   virtual MTAPIRES  PriceTP(const double price)=0;
   //--- order initial volume
   virtual uint64_t    VolumeInitial(void) const=0;
   virtual MTAPIRES  VolumeInitial(const uint64_t volume)=0;
   //--- order current volume
   virtual uint64_t    VolumeCurrent(void) const=0;
   virtual MTAPIRES  VolumeCurrent(const uint64_t volume)=0;
   //--- expert id (filled by expert advisor)
   virtual uint64_t    ExpertID(void) const=0;
   virtual MTAPIRES  ExpertID(const uint64_t id)=0;
   //--- position id
   virtual uint64_t    PositionID(void) const=0;
   virtual MTAPIRES  PositionID(const uint64_t id)=0;
   //--- order comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- order activation state, time and price
   virtual uint32_t  ActivationMode(void) const=0;
   virtual int64_t     ActivationTime(void) const=0;
   virtual double    ActivationPrice(void) const=0;
   virtual uint32_t  ActivationFlags(void) const=0;
   //--- order internal data for API usage
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,double& value) const=0;
   virtual MTAPIRES  ApiDataClear(const uint16_t app_id)=0;
   virtual MTAPIRES  ApiDataClearAll(void)=0;
   //--- order setup time in us since 1970.01.01
   virtual int64_t     TimeSetupMsc(void) const=0;
   virtual MTAPIRES  TimeSetupMsc(const int64_t time)=0;
   //--- order setup time in us since 1970.01.01
   virtual int64_t     TimeDoneMsc(void) const=0;
   virtual MTAPIRES  TimeDoneMsc(const int64_t time)=0;
   //--- order activation state, time and price
   virtual MTAPIRES  ActivationMode(const uint32_t mode)=0;
   virtual MTAPIRES  ActivationTime(const int64_t atm)=0;
   virtual MTAPIRES  ActivationPrice(const double price)=0;
   virtual MTAPIRES  ActivationFlags(const uint32_t flags)=0;
   //--- margin conversion rate (from symbol margin currency to deposit currency)
   virtual double    RateMargin(void) const=0;
   virtual MTAPIRES  RateMargin(const double rate)=0;
   //--- user record internal data for API usage
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,double& value) const=0;
   //--- order ticket
   virtual MTAPIRES  OrderSet(const uint64_t order)=0;
   //--- position by id
   virtual uint64_t    PositionByID(void) const=0;
   virtual MTAPIRES  PositionByID(const uint64_t id)=0;
   //--- modification flags
   virtual uint32_t  ModificationFlags(void) const=0;
   //--- EnOrderState
   virtual MTAPIRES  StateSet(const uint32_t state)=0;
   //--- EnOrderReason
   virtual MTAPIRES  ReasonSet(const uint32_t reason)=0;
   //--- order initial volume with extended accuracy
   virtual uint64_t    VolumeInitialExt(void) const=0;
   virtual MTAPIRES  VolumeInitialExt(const uint64_t volume)=0;
   //--- order current volume with extended accuracy
   virtual uint64_t    VolumeCurrentExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentExt(const uint64_t volume)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTOrder(void) {}
  };
//+------------------------------------------------------------------+
//| Order array interface                                            |
//+------------------------------------------------------------------+
class IMTOrderArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTOrderArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTOrder* order)=0;
   virtual MTAPIRES  AddCopy(const IMTOrder* order)=0;
   virtual MTAPIRES  Add(IMTOrderArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTOrderArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTOrder* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTOrder* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTOrder* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTOrder* Next(const uint32_t index) const=0;
   //--- sorting and search
   virtual MTAPIRES  Sort(MTSortFunctionPtr sort_function)=0;
   virtual int32_t   Search(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreatOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchGreater(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLessOrEq(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLess(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchLeft(const void *key,MTSortFunctionPtr sort_function) const=0;
   virtual int32_t   SearchRight(const void *key,MTSortFunctionPtr sort_function) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTOrderArray(void) {}
  };
//+------------------------------------------------------------------+
//| Open orders events notification interface                        |
//+------------------------------------------------------------------+
class IMTOrderSink
  {
public:
   virtual void      OnOrderAdd(const IMTOrder*    /*order*/)       {  }
   virtual void      OnOrderUpdate(const IMTOrder* /*order*/)       {  }
   virtual void      OnOrderDelete(const IMTOrder* /*order*/)       {  }
   virtual void      OnOrderClean(const uint64_t /*login*/)           {  }
   virtual void      OnOrderSync(void)                              {  }
  };
//+------------------------------------------------------------------+
//| History orders                                                   |
//+------------------------------------------------------------------+
class IMTHistorySink
  {
public:
   virtual void      OnHistoryAdd(const IMTOrder*    /*order*/)     {  }
   virtual void      OnHistoryUpdate(const IMTOrder* /*order*/)     {  }
   virtual void      OnHistoryDelete(const IMTOrder* /*order*/)     {  }
   virtual void      OnHistoryClean(const uint64_t /*login*/)         {  }
   virtual void      OnHistorySync(void)                            {  }
  };
//+------------------------------------------------------------------+
