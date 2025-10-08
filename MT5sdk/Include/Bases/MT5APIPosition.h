//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade position interface                                         |
//+------------------------------------------------------------------+
class IMTPosition
  {
public:
   //--- position types
   enum EnPositionAction
     {
      POSITION_BUY             =0,     // buy
      POSITION_SELL            =1,     // sell
      //--- enumeration borders
      POSITION_FIRST           =POSITION_BUY,
      POSITION_LAST            =POSITION_SELL
     };
   //--- activation modes
   enum EnActivation
     {
      ACTIVATION_NONE          =0,     // none
      ACTIVATION_SL            =1,     // SL activated
      ACTIVATION_TP            =2,     // TP activated
      ACTIVATION_STOPOUT       =3,     // Stop-Out activated
      //--- enumeration borders
      ACTIVATION_FIRST  =ACTIVATION_NONE,
      ACTIVATION_LAST   =ACTIVATION_STOPOUT,
     };
   //--- position activation flags
   enum EnTradeActivationFlags
     {
      ACTIV_FLAGS_NO_LIMIT     =0x00000001,
      ACTIV_FLAGS_NO_STOP      =0x00000002,
      ACTIV_FLAGS_NO_SLIMIT    =0x00000004,
      ACTIV_FLAGS_NO_SL        =0x00000008,
      ACTIV_FLAGS_NO_TP        =0x00000010,
      ACTIV_FLAGS_NO_SO        =0x00000020,
      ACTIV_FLAGS_NO_EXPIRATION=0x00000040,
      //--- 
      ACTIV_FLAGS_NONE         =0x00000000,
      ACTIV_FLAGS_ALL          =ACTIV_FLAGS_NO_LIMIT|ACTIV_FLAGS_NO_STOP|ACTIV_FLAGS_NO_SLIMIT|ACTIV_FLAGS_NO_SL|
      ACTIV_FLAGS_NO_TP|ACTIV_FLAGS_NO_SO|ACTIV_FLAGS_NO_EXPIRATION
     };
   //--- position creation reasons
   enum EnPositionReason
     {
      POSITION_REASON_CLIENT   =0,           // position placed manually
      POSITION_REASON_EXPERT   =1,           // position placed by expert
      POSITION_REASON_DEALER   =2,           // position placed by dealer
      POSITION_REASON_SL       =3,           // position placed due SL
      POSITION_REASON_TP       =4,           // position placed due TP
      POSITION_REASON_SO       =5,           // position placed due Stop-Out
      POSITION_REASON_ROLLOVER =6,           // position placed due rollover
      POSITION_REASON_EXTERNAL_CLIENT=7,     // position placed from the external system by client
      POSITION_REASON_VMARGIN  =8,           // position placed due variation margin
      POSITION_REASON_GATEWAY  =9,           // position placed by gateway
      POSITION_REASON_SIGNAL   =10,          // position placed by signal service
      POSITION_REASON_SETTLEMENT=11,         // position placed due to settlement
      POSITION_REASON_TRANSFER =12,          // position placed due position transfer
      POSITION_REASON_SYNC     =13,          // position placed due position synchronization
      POSITION_REASON_EXTERNAL_SERVICE=14,   // position placed from the external system due service issues
      POSITION_REASON_MIGRATION=15,          // position placed due migration
      POSITION_REASON_MOBILE   =16,          // position placed by mobile terminal
      POSITION_REASON_WEB      =17,          // position placed by web terminal
      POSITION_REASON_SPLIT    =18,          // position placed due split
      POSITION_REASON_CORPORATE_ACTION=19,   // position placed due corporate action
      //--- enumeration borders
      POSITION_REASON_FIRST    =POSITION_REASON_CLIENT,
      POSITION_REASON_LAST     =POSITION_REASON_CORPORATE_ACTION
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
      MODIFY_FLAGS_NONE         =0x00000000,
      MODIFY_FLAGS_ALL          =MODIFY_FLAGS_ADMIN|MODIFY_FLAGS_MANAGER|MODIFY_FLAGS_POSITION|MODIFY_FLAGS_RESTORE|
      MODIFY_FLAGS_API_ADMIN|MODIFY_FLAGS_API_MANAGER|MODIFY_FLAGS_API_SERVER|MODIFY_FLAGS_API_GATEWAY
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTPosition* position)=0;
   virtual MTAPIRES  Clear(void)=0;
   virtual LPCWSTR   Print(MTAPISTR& string) const=0;
   //--- owner client login
   virtual uint64_t  Login(void) const=0;
   //--- position symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- EnPositionAction
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t action)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- currency digits
   virtual uint32_t  DigitsCurrency(void) const=0;
   virtual MTAPIRES  DigitsCurrency(const uint32_t digits)=0;
   //--- symbol contract size
   virtual double    ContractSize(void) const=0;
   virtual MTAPIRES  ContractSize(const double contract_size)=0;
   //--- position create time
   virtual int64_t   TimeCreate(void) const=0;
   virtual MTAPIRES  TimeCreate(const int64_t time)=0;
   //--- position last update time
   virtual int64_t   TimeUpdate(void) const=0;
   virtual MTAPIRES  TimeUpdate(const int64_t time)=0;
   //--- position weighted average open price
   virtual double    PriceOpen(void) const=0;
   virtual MTAPIRES  PriceOpen(const double price)=0;
   //--- position current price
   virtual double    PriceCurrent(void) const=0;
   virtual MTAPIRES  PriceCurrent(const double price)=0;
   //--- position SL price
   virtual double    PriceSL(void) const=0;
   virtual MTAPIRES  PriceSL(const double price)=0;
   //--- position TP price
   virtual double    PriceTP(void) const=0;
   virtual MTAPIRES  PriceTP(const double price)=0;
   //--- position volume
   virtual uint64_t  Volume(void) const=0;
   virtual MTAPIRES  Volume(const uint64_t volume)=0;
   //--- position floating profit
   virtual double    Profit(void) const=0;
   virtual MTAPIRES  Profit(const double profit)=0;
   //--- position accumulated swaps
   virtual double    Storage(void) const=0;
   virtual MTAPIRES  Storage(const double storage)=0;
   //--- obsolete value
   virtual double    ObsoleteValue(void) const=0;
   virtual MTAPIRES  ObsoleteValue(const double value)=0;
   //--- profit conversion rate (from symbol profit currency to deposit currency)
   virtual double    RateProfit(void) const=0;
   virtual MTAPIRES  RateProfit(const double rate)=0;
   //--- margin conversion rate (from symbol margin currency to deposit currency)
   virtual double    RateMargin(void) const=0;
   virtual MTAPIRES  RateMargin(const double rate)=0;
   //--- expert id (filled by expert advisor)
   virtual uint64_t  ExpertID(void) const=0;
   virtual MTAPIRES  ExpertID(const uint64_t id)=0;
   //--- expert position id
   virtual uint64_t  ExpertPositionID(void) const=0;
   virtual MTAPIRES  ExpertPositionID(const uint64_t id)=0;
   //--- comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- order activation state, time and price
   virtual uint32_t  ActivationMode(void) const=0;
   virtual int64_t   ActivationTime(void) const=0;
   virtual double    ActivationPrice(void) const=0;
   virtual uint32_t  ActivationFlags(void) const=0;
   //--- position internal data for API usage
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,double& value) const=0;
   virtual MTAPIRES  ApiDataClear(const uint16_t app_id)=0;
   virtual MTAPIRES  ApiDataClearAll(void)=0;
   //--- position create time in us since 1970.01.01
   virtual int64_t   TimeCreateMsc(void) const=0;
   virtual MTAPIRES  TimeCreateMsc(const int64_t time)=0;
   //--- position last update time in us since 1970.01.01
   virtual int64_t   TimeUpdateMsc(void) const=0;
   virtual MTAPIRES  TimeUpdateMsc(const int64_t time)=0;
   //--- order activation state, time and price
   virtual MTAPIRES  ActivationMode(const uint32_t mode)=0;
   virtual MTAPIRES  ActivationTime(const int64_t atm)=0;
   virtual MTAPIRES  ActivationPrice(const double price)=0;
   virtual MTAPIRES  ActivationFlags(const uint32_t flags)=0;
   //--- processed dealer login (0-means auto) (first position deal dealer)
   virtual uint64_t  Dealer(void) const=0;
   virtual MTAPIRES  Dealer(const uint64_t dealer)=0;
   //--- user record internal data for API usage
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,double& value) const=0;
   //--- owner client login
   virtual MTAPIRES  LoginSet(const uint64_t login)=0;
   //--- position ticket
   virtual uint64_t  Position(void) const=0;
   //--- position ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- modification flags
   virtual uint32_t  ModificationFlags(void) const=0;
   //--- position reason - EnPositionReason
   virtual uint32_t  Reason(void) const=0;
   //--- position volume
   virtual uint64_t  VolumeExt(void) const=0;
   virtual MTAPIRES  VolumeExt(const uint64_t volume)=0;
   //--- EnPositionReason
   virtual MTAPIRES  ReasonSet(const uint32_t reason)=0;
   //--- execution price on gateway
   virtual double    PriceGateway(void) const=0;
   virtual MTAPIRES  PriceGateway(const double price)=0;
   //--- confirmed gateway volume with extended accuracy
   virtual uint64_t  VolumeGatewayExt(void) const=0;
   virtual MTAPIRES  VolumeGatewayExt(const uint64_t volume)=0;
   //--- confirmed gateway deal type
   virtual uint32_t  ActionGateway(void) const=0;
   virtual MTAPIRES  ActionGateway(const uint32_t action)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTPosition(void) {}
  };
//+------------------------------------------------------------------+
//| Position array interface                                         |
//+------------------------------------------------------------------+
class IMTPositionArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTPositionArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTPosition* position)=0;
   virtual MTAPIRES  AddCopy(const IMTPosition* position)=0;
   virtual MTAPIRES  Add(IMTPositionArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTPositionArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTPosition* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTPosition* position)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTPosition* position)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTPosition* Next(const uint32_t index) const=0;
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
                    ~IMTPositionArray(void) {}
  };
//+------------------------------------------------------------------+
//| Position events notification interface                           |
//+------------------------------------------------------------------+
class IMTPositionSink
  {
public:
   virtual void      OnPositionAdd(const IMTPosition* /*position*/)    {  }
   virtual void      OnPositionUpdate(const IMTPosition* /*position*/) {  }
   virtual void      OnPositionDelete(const IMTPosition* /*position*/) {  }
   virtual void      OnPositionClean(const uint64_t /*login*/)           {  }
   virtual void      OnPositionSync(void)                              {  }
  };
//+------------------------------------------------------------------+
