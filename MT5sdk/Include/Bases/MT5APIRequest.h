//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade request interface                                          |
//+------------------------------------------------------------------+
class IMTRequest
  {
public:
   //--- trade action types
   enum EnTradeActions
     {
      //--- clients actions
      TA_CLIENT_FIRST       =0,
      TA_PRICE              =0,
      TA_REQUEST            =1,
      TA_INSTANT            =2,
      TA_MARKET             =3,
      TA_EXCHANGE           =4,
      TA_PENDING            =5,
      TA_SLTP               =6,
      TA_MODIFY             =7,
      TA_REMOVE             =8,
      TA_TRANSFER           =9,
      TA_CLOSE_BY           =10,
      TA_CLIENT_LAST        =TA_CLOSE_BY,
      //--- server actions
      TA_SERVER_FIRST       =100,
      TA_ACTIVATE           =100,
      TA_ACTIVATE_SL        =101,
      TA_ACTIVATE_TP        =102,
      TA_ACTIVATE_STOPLIMIT =103,
      TA_STOPOUT_ORDER      =104,
      TA_STOPOUT_POSITION   =105,
      TA_EXPIRATION         =106,
      TA_SERVER_LAST        =TA_EXPIRATION,
      //--- dealer actions
      TA_DEALER_FIRST       =200,
      TA_DEALER_POS_EXECUTE =200,
      TA_DEALER_ORD_PENDING =201,
      TA_DEALER_POS_MODIFY  =202,
      TA_DEALER_ORD_MODIFY  =203,
      TA_DEALER_ORD_REMOVE  =204,
      TA_DEALER_ORD_ACTIVATE=205,
      TA_DEALER_BALANCE     =206,
      TA_DEALER_ORD_SLIMIT  =207,
      TA_DEALER_CLOSE_BY    =208,
      TA_DEALER_LAST        =TA_DEALER_CLOSE_BY,
      //--- enumeration borders
      TA_FIRST              =TA_PRICE,
      TA_LAST               =TA_DEALER_CLOSE_BY,
      TA_END                =255
     };
   //--- trade action flags
   enum EnTradeActionFlags
     {
      TA_FLAG_CLOSE            =0x00000001, // position close request
      TA_FLAG_MARKET           =0x00000002, // market prices request
      TA_FLAG_CHANGED_PRICE    =0x00000004, // order price or position open price will be changed
      TA_FLAG_CHANGED_TRIGGER  =0x00000008, // order trigger price will be changed
      TA_FLAG_CHANGED_SL       =0x00000010, // order or position SL will be changed
      TA_FLAG_CHANGED_TP       =0x00000020, // order or position TP will be changed
      TA_FLAG_CHANGED_EXP_TYPE =0x00000040, // order expiration type will be changed
      TA_FLAG_CHANGED_EXP_TIME =0x00000080, // order expiration datetime will be changed
      TA_FLAG_EXPERT           =0x00000100, // request from expert
      TA_FLAG_SIGNAL           =0x00000200, // request from signal
      TA_FLAG_SKIP_MARGIN_CHECK=0x00000400, // skip margin check (only for dealers)
      //--- enumeration borders
      TA_FLAG_NONE            =0x00000000,
      TA_FLAG_ALL             =TA_FLAG_CLOSE      | TA_FLAG_MARKET     | TA_FLAG_CHANGED_PRICE | TA_FLAG_CHANGED_TRIGGER |
      TA_FLAG_CHANGED_SL | TA_FLAG_CHANGED_TP | TA_FLAG_CHANGED_EXP_TYPE | TA_FLAG_CHANGED_EXP_TIME | TA_FLAG_EXPERT | TA_FLAG_SIGNAL |
      TA_FLAG_SKIP_MARGIN_CHECK
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTRequest* request)=0;
   virtual MTAPIRES  Clear(void)=0;
   virtual LPCWSTR   Print(MTAPISTR& string) const=0;
   //--- request id
   virtual uint32_t  ID(void) const=0;
   //--- client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- client group
   virtual LPCWSTR   Group(void) const=0;
   //--- symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   //--- EnTradeActions
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t action)=0;
   //--- order expiration
   virtual int64_t     TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t time)=0;
   //--- order type IMTOrder::EnOrderType
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- IMTOrder::EnOrderFilling
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- IMTOrder::EnOrderTime
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //--- additional flags EnTradeActionFlags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- volume
   virtual uint64_t    Volume(void) const=0;
   virtual MTAPIRES  Volume(const uint64_t volume)=0;
   //--- order ticket
   virtual uint64_t    Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- order ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   OrderExternalID(void) const=0;
   virtual MTAPIRES  OrderExternalID(LPCWSTR id)=0;
   //--- order price
   virtual double    PriceOrder(void) const=0;
   virtual MTAPIRES  PriceOrder(const double price)=0;
   //--- order trigger price
   virtual double    PriceTrigger(void) const=0;
   virtual MTAPIRES  PriceTrigger(const double price)=0;
   //--- order SL price
   virtual double    PriceSL(void) const=0;
   virtual MTAPIRES  PriceSL(const double price)=0;
   //--- order TP price
   virtual double    PriceTP(void) const=0;
   virtual MTAPIRES  PriceTP(const double price)=0;
   //--- price deviation details
   virtual uint64_t    PriceDeviation(void) const=0;
   virtual MTAPIRES  PriceDeviation(const uint64_t deviation)=0;
   virtual double    PriceDeviationTop(void) const=0;
   virtual double    PriceDeviationBottom(void) const=0;
   //--- comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //---
   //--- request result information
   //---
   //--- request state retcode
   virtual MTAPIRES  ResultRetcode(void) const=0;
   //--- request dealer
   virtual uint64_t    ResultDealer(void) const=0;
   //--- request result deal
   virtual uint64_t    ResultDeal(void) const=0;
   //--- request result order
   virtual uint64_t    ResultOrder(void) const=0;
   //--- request result confirmed volume
   virtual uint64_t    ResultVolume(void) const=0;
   //--- request result confirmed price
   virtual double    ResultPrice(void) const=0;
   //--- request result dealer prices
   virtual double    ResultDealerBid(void) const=0;
   virtual double    ResultDealerAsk(void) const=0;
   virtual double    ResultDealerLast(void) const=0;
   //--- request result market prices
   virtual double    ResultMarketBid(void) const=0;
   virtual double    ResultMarketAsk(void) const=0;
   virtual double    ResultMarketLast(void) const=0;
   //--- request result comment
   virtual LPCWSTR   ResultComment(void) const=0;
   //--- external system trade account (exchange, ECN, etc)
   virtual LPCWSTR   ExternalAccount(void) const=0;
   virtual MTAPIRES  ExternalAccount(LPCWSTR account)=0;
   //--- client side request id
   virtual uint32_t  IDClient(void) const=0;
   //--- source request ip
   virtual LPCWSTR   IP(void) const=0;
   virtual MTAPIRES  IP(LPCWSTR ip)=0;
   //--- source dealer login (for dealer transaction)
   virtual uint64_t    SourceLogin(void) const=0;
   virtual MTAPIRES  SourceLogin(const uint64_t login)=0;
   //--- position ticket
   virtual uint64_t    Position(void) const=0;
   virtual MTAPIRES  Position(const uint64_t position)=0;
   //--- position ticket for close-by
   virtual uint64_t    PositionBy(void) const=0;
   virtual MTAPIRES  PositionBy(const uint64_t position)=0;
   //--- position ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   PositionExternalID(void) const=0;
   virtual MTAPIRES  PositionExternalID(LPCWSTR id)=0;
   //--- position ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   PositionByExternalID(void) const=0;
   virtual MTAPIRES  PositionByExternalID(LPCWSTR id)=0;
   //--- volume with extended accuracy
   virtual uint64_t    VolumeExt(void) const=0;
   virtual MTAPIRES  VolumeExt(const uint64_t volume)=0;
   //--- request result confirmed volume with extended accuracy
   virtual uint64_t    ResultVolumeExt(void) const=0;
   //--- price digits
   virtual MTAPIRES  DigitsSet(const uint32_t digits)=0;
   //--- request internal data for API usage
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,double& value) const=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,double& value) const=0;
   virtual LPVOID    ApiDataRaw(void) const=0;
   virtual uint32_t  ApiDataRawMax(void) const=0;
   virtual MTAPIRES  ApiDataClear(const uint16_t app_id)=0;
   virtual MTAPIRES  ApiDataClearAll(void)=0;
   //--- current order volume 
   virtual uint64_t    VolumeCurrent(void) const=0;
   virtual MTAPIRES  VolumeCurrent(const uint64_t volume)=0;
   //--- current order volume 
   virtual uint64_t    VolumeCurrentExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentExt(const uint64_t volume)=0;
   //--- trade symbol before symbols mapping
   virtual LPCWSTR   SymbolOriginal(void) const=0;
   virtual MTAPIRES  SymbolOriginal(LPCWSTR symbol)=0;
   //--- spread difference
   virtual INT       SpreadDiff(void) const=0;
   virtual MTAPIRES  SpreadDiff(const INT diff)=0;
   //--- spread difference balance
   virtual INT       SpreadDiffBalance(void) const=0;
   virtual MTAPIRES  SpreadDiffBalance(const INT spread)=0;
   //--- EnOrderReason
   virtual uint32_t  Reason(void) const=0;
   virtual MTAPIRES  Reason(const uint32_t reason)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTRequest(void) {}
  };
//+------------------------------------------------------------------+
//| Request array interface                                          |
//+------------------------------------------------------------------+
class IMTRequestArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTRequestArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTRequest* request)=0;
   virtual MTAPIRES  AddCopy(const IMTRequest* request)=0;
   virtual MTAPIRES  Add(IMTRequestArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTRequestArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTRequest* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTRequest* request)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTRequest* request)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t    Total(void) const=0;
   virtual IMTRequest* Next(const uint32_t index) const=0;
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
                    ~IMTRequestArray(void) {}
  };
//+------------------------------------------------------------------+
//| Request events notification interface                            |
//+------------------------------------------------------------------+
class IMTRequestSink
  {
public:
   virtual void      OnRequestAdd(const IMTRequest*    /*request*/) {  }
   virtual void      OnRequestUpdate(const IMTRequest* /*request*/) {  }
   virtual void      OnRequestDelete(const IMTRequest* /*request*/) {  }
   virtual void      OnRequestSync(void)                            {  }
  };
//+------------------------------------------------------------------+
