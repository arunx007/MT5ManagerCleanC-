//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Ultency Matching Order                                           |
//+------------------------------------------------------------------+
class IMTUltMatchingOrder
  {
public:
   //--- order state
   enum EnMatchingState
     {
      STATE_NONE        =0,
      STATE_PROCESSING  =1,
      STATE_CANCELED    =2,
      STATE_EXPIRED     =3,
      STATE_DONE        =4,
      STATE_DONE_PARTIAL=5,
      //--- enumeration borders
      STATE_FIRST       =STATE_NONE,
      STATE_LAST        =STATE_DONE_PARTIAL
     };
   //--- matching flags
   enum EnMatchingFlags
     {
      MCH_FLAG_MERGE_DEALS=0x00000001,
      //--- enumeration borders
      MCH_FLAG_NONE       =0x00000000,
      MCH_FLAG_ALL        =MCH_FLAG_MERGE_DEALS
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUltMatchingOrder* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint64_t  Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t id)=0;
   //--- ME instance id
   virtual uint64_t  InstanceID(void) const=0;
   virtual MTAPIRES  InstanceID(const uint64_t id)=0;
   //--- source trade order
   virtual uint64_t  SourceOrder(void) const=0;
   virtual MTAPIRES  SourceOrder(const uint64_t order)=0;
   //--- source trade account login
   virtual uint64_t  SourceLogin(void) const=0;
   virtual MTAPIRES  SourceLogin(const uint64_t login)=0;
   //--- source client id
   virtual uint64_t  SourceClientID(void) const=0;
   virtual MTAPIRES  SourceClientID(const uint64_t id)=0;
   //--- source server id
   virtual uint64_t  SourceServer(void) const=0;
   virtual MTAPIRES  SourceServer(const uint64_t server)=0;
   //--- source order group
   virtual LPCWSTR   SourceGroup(void) const=0;
   virtual MTAPIRES  SourceGroup(LPCWSTR group)=0;
   //--- source order IMTRequest::EnTradeActions
   virtual uint32_t  SourceAction(void) const=0;
   virtual MTAPIRES  SourceAction(const uint32_t type)=0;
   //--- source order IMTOrder::EnOrderType
   virtual uint32_t  SourceType(void) const=0;
   virtual MTAPIRES  SourceType(const uint32_t type)=0;
   //--- source order IMTOrder::EnOrderFilling
   virtual uint32_t  SourceTypeFill(void) const=0;
   virtual MTAPIRES  SourceTypeFill(const uint32_t type)=0;
   //--- source order EnOrderTime
   virtual uint32_t  SourceTypeTime(void) const=0;
   virtual MTAPIRES  SourceTypeTime(const uint32_t type)=0;
   //--- source order expiration
   virtual int64_t   SourceTimeExpiration(void) const=0;
   virtual MTAPIRES  SourceTimeExpiration(const int64_t time)=0;
   //--- source volume symbol
   virtual LPCWSTR   SourceSymbol(void) const=0;
   virtual MTAPIRES  SourceSymbol(LPCWSTR symbol)=0;
   //--- source size
   virtual uint64_t  SourceSize(void) const=0;
   virtual MTAPIRES  SourceSize(const uint64_t size)=0;
   //--- source volume
   virtual uint64_t  SourceVolume(void) const=0;
   virtual MTAPIRES  SourceVolume(const uint64_t volume)=0;
   //--- source contract size
   virtual double    SourceContractSize(void) const=0;
   virtual MTAPIRES  SourceContractSize(const double size)=0;
   //--- source price
   virtual double    SourcePrice(void) const=0;
   virtual MTAPIRES  SourcePrice(const double price)=0;
   //--- source digits
   virtual uint32_t  SourceDigits(void) const=0;
   virtual MTAPIRES  SourceDigits(const uint32_t digits)=0;
   //--- IMTUltMatchingOrder::EnMatchingState
   virtual uint32_t  State(void) const=0;
   virtual MTAPIRES  State(const uint32_t type)=0;
   //--- IMTOrder::EnOrderType
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- IMTOrder::EnOrderFilling
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- order EnOrderTime
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //--- order setup time
   virtual int64_t   TimeSetup(void) const=0;
   virtual MTAPIRES  TimeSetup(const int64_t time)=0;
   //--- order expiration
   virtual int64_t   TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t time)=0;
   //--- order filling/cancel time
   virtual int64_t   TimeDone(void) const=0;
   virtual MTAPIRES  TimeDone(const int64_t time)=0;
   //--- order symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- order price
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- markup
   virtual int32_t   Markup(void) const=0;
   virtual MTAPIRES  Markup(const int32_t markup)=0;
   //--- coverage ratio
   virtual double    Coverage(void) const=0;
   virtual MTAPIRES  Coverage(const double coverage)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- flags EnMatchingFlags
   virtual uint32_t  MatchingFlags(void) const=0;
   virtual MTAPIRES  MatchingFlags(const uint32_t digits)=0;
   //--- abook volume
   virtual uint64_t  ABookSize(void) const=0;
   virtual MTAPIRES  ABookSize(const uint64_t size)=0;
   //--- bbook volume
   virtual uint64_t  BBookSize(void) const=0;
   virtual MTAPIRES  BBookSize(const uint64_t size)=0;
   //--- abook volume taken
   virtual uint64_t  ABookSizeTaken(void) const=0;
   virtual MTAPIRES  ABookSizeTaken(const uint64_t size)=0;
   //--- abook volume executed
   virtual uint64_t  ABookSizeExecuted(void) const=0;
   virtual MTAPIRES  ABookSizeExecuted(const uint64_t size)=0;
   //--- abook cost executed
   virtual double    ABookCostExecuted(void) const=0;
   virtual MTAPIRES  ABookCostExecuted(const double cost)=0;
   //--- bbook volume executed
   virtual uint64_t  BBookSizeExecuted(void) const=0;
   virtual MTAPIRES  BBookSizeExecuted(const uint64_t size)=0;
   //--- bbook cost executed
   virtual double    BBookCostExecuted(void) const=0;
   virtual MTAPIRES  BBookCostExecuted(const double cost)=0;
   //--- bbook volume executed
   virtual uint64_t  SourceSizeExecuted(void) const=0;
   virtual MTAPIRES  SourceSizeExecuted(const uint64_t size)=0;
   //--- bbook volume executed
   virtual uint64_t  SourceVolumeExecuted(void) const=0;
   virtual MTAPIRES  SourceVolumeExecuted(const uint64_t volume)=0;
   //--- bbook cost executed
   virtual double    SourceCostExecuted(void) const=0;
   virtual MTAPIRES  SourceCostExecuted(const double cost)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTUltMatchingOrder(void) {}
  };
//+------------------------------------------------------------------+
//| Order array interface                                            |
//+------------------------------------------------------------------+
class IMTUltMatchingOrderArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUltMatchingOrderArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTUltMatchingOrder* order)=0;
   virtual MTAPIRES  AddCopy(const IMTUltMatchingOrder* order)=0;
   virtual MTAPIRES  Add(IMTUltMatchingOrderArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTUltMatchingOrderArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTUltMatchingOrder* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTUltMatchingOrder* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTUltMatchingOrder* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTUltMatchingOrder* Next(const uint32_t index) const=0;
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
                    ~IMTUltMatchingOrderArray(void) {}
  };
//+------------------------------------------------------------------+
