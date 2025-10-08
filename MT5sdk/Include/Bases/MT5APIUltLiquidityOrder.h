//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Ultency Liquidity Order                                          |
//+------------------------------------------------------------------+
class IMTUltLiquidityOrder
  {
public:
   //--- order types
   enum EnLiquidityProviders
     {
      PROVIDER_BBOOK          =0,               // B-Book
      PROVIDER_AGGREGATED     =0xFFFFFFFF,      // Aggregation
      //--- A-Book providers borders
      PROVIDER_ABOOK_FROM     =PROVIDER_BBOOK+1,
      PROVIDER_ABOOK_TO       =PROVIDER_AGGREGATED-1,
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUltLiquidityOrder* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint64_t  Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t id)=0;
   //--- order ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- ME instance id
   virtual uint64_t  InstanceID(void) const=0;
   virtual MTAPIRES  InstanceID(const uint64_t id)=0;
   //--- matching order ticket
   virtual uint64_t  OrderMatching(void) const=0;
   virtual MTAPIRES  OrderMatching(const uint64_t order)=0;
   //--- liquidity provider
   virtual uint32_t  LiquidityProvider(void) const=0;
   virtual MTAPIRES  LiquidityProvider(const uint32_t lp)=0;
   //--- liquidity provider config id
   virtual uint64_t  LiquidityProviderID(void) const=0;
   virtual MTAPIRES  LiquidityProviderID(const uint64_t id)=0;
   //--- source trade account login
   virtual uint64_t  SourceLogin(void) const=0;
   virtual MTAPIRES  SourceLogin(const uint64_t login)=0;
   //--- source client id
   virtual uint64_t  SourceClientID(void) const=0;
   virtual MTAPIRES  SourceClientID(const uint64_t login)=0;
   //--- IMTOrder::EnOrderType
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- IMTOrder::EnOrderFilling
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- IMTOrder::EnOrderState
   virtual uint32_t  State(void) const=0;
   virtual MTAPIRES  State(const uint32_t type)=0;
   //--- order symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- order initial size
   virtual uint64_t  SizeInitial(void) const=0;
   virtual MTAPIRES  SizeInitial(const uint64_t size)=0;
   //--- order executed size
   virtual uint64_t  SizeExecuted(void) const=0;
   virtual MTAPIRES  SizeExecuted(const uint64_t size)=0;
   //--- order initial volume
   virtual uint64_t  VolumeInitial(void) const=0;
   virtual MTAPIRES  VolumeInitial(const uint64_t volume)=0;
   //--- order executed volumme
   virtual uint64_t  VolumeExecuted(void) const=0;
   virtual MTAPIRES  VolumeExecuted(const uint64_t volume)=0;
   //--- order executed volumme
   virtual double    ContractSize(void) const=0;
   virtual MTAPIRES  ContractSize(const double size)=0;
   //--- order price
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- expected execution price
   virtual double    PriceExpected(void) const=0;
   virtual MTAPIRES  PriceExpected(const double price)=0;
   //--- order setup time
   virtual int64_t   TimeSetup(void) const=0;
   virtual MTAPIRES  TimeSetup(const int64_t time)=0;
   //--- order expiration
   virtual int64_t   TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t time)=0;
   //--- order filling/cancel time
   virtual int64_t   TimeDone(void) const=0;
   virtual MTAPIRES  TimeDone(const int64_t time)=0;
   //--- execution cost
   virtual double    CostExecuted(void) const=0;
   virtual MTAPIRES  CostExecuted(const double cost)=0;
   //--- markup
   virtual int32_t   Markup(void) const=0;
   virtual MTAPIRES  Markup(const int32_t markup)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- processing time for order placement
   virtual int64_t   PerformanceTimePlace(void) const=0;
   virtual MTAPIRES  PerformanceTimePlace(const int64_t ptm)=0;
   //--- processing time for order filling
   virtual int64_t   PerformanceTimeFill(void) const=0;
   virtual MTAPIRES  PerformanceTimeFill(const int64_t ptm)=0;

   //--- explicit destructor is prohibited
protected:
                    ~IMTUltLiquidityOrder(void) {}
  };
//+------------------------------------------------------------------+
//| Order array interface                                            |
//+------------------------------------------------------------------+
class IMTUltLiquidityOrderArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUltLiquidityOrderArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTUltLiquidityOrder* order)=0;
   virtual MTAPIRES  AddCopy(const IMTUltLiquidityOrder* order)=0;
   virtual MTAPIRES  Add(IMTUltLiquidityOrderArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTUltLiquidityOrderArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTUltLiquidityOrder* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTUltLiquidityOrder* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTUltLiquidityOrder* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTUltLiquidityOrder* Next(const uint32_t index) const=0;
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
                    ~IMTUltLiquidityOrderArray(void) {}
  };
//+------------------------------------------------------------------+
