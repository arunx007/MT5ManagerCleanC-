//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| ECN Matching Interface                                           |
//+------------------------------------------------------------------+
class IMTECNMatching
  {
public:
   //--- matching order states
   enum ENCMatchingState
     {
      ORDER_STATE_NONE        =0,         // initial state
      ORDER_STATE_TAKEN       =1,         // order taken for filling
      ORDER_STATE_PARTIAL     =2,         // order partially filled and returned to Depth Of Market
      ORDER_STATE_CANCELED    =3,         // order canceled
      ORDER_STATE_EXPIRED     =4,         // order expired
      ORDER_STATE_DONE        =5,         // order filled
      ORDER_STATE_DONE_PARTIAL=6,         // order partially filled and canceled
      //--- enumeration borders
      ORDER_STATE_FIRST       =ORDER_STATE_NONE,
      ORDER_STATE_LAST        =ORDER_STATE_DONE_PARTIAL
     };
   //--- order flags
   enum EnECNMatchingOrderFlags
     {
      ORDER_FLAGS_NONE        =0x00000000, // none
      ORDER_FLAGS_MISSING     =0x00000001, // no client orders of trade server or trade server disconnected from ECN
      //--- enumeration borders
      ORDER_FLAGS_ALL         =ORDER_FLAGS_MISSING
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNMatching* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- MT5 client order
   virtual uint64_t    Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- MT5 client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- MT5 trade server id
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- client group name
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
   //--- order state ENCMatchingOrderState
   virtual uint32_t  State(void) const=0;
   virtual MTAPIRES  State(const uint32_t state)=0;
   //--- order flags EnECNMatchingOrderFlags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- order setup in ECN time in us since 1970.01.01
   virtual int64_t     TimeSetupMsc(void) const=0;
   virtual MTAPIRES  TimeSetupMsc(const int64_t time)=0;
   //--- order expiration
   virtual int64_t     TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t time)=0;
   //--- the name of the financial symbol for which the order in the ECN was placed
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- the name of the financial symbol for which the client order was placed
   virtual LPCWSTR   SymbolClient(void) const=0;
   virtual MTAPIRES  SymbolClient(LPCWSTR symbol)=0;
   //--- EnOrderType
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- EnOrderFilling
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- EnOrderTime
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //--- the price of the order which was created in the ECN to execute the client order
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- the price specified in the original client order
   virtual double    PriceClient(void) const=0;
   virtual MTAPIRES  PriceClient(const double price)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- client order price digits
   virtual uint32_t  DigitsClient(void) const=0;
   virtual MTAPIRES  DigitsClient(const uint32_t digits)=0;
   //--- the volume of the order which was created in the ECN to execute the client order: 
   //--- the initial operation volume requested by the client in the order and the current filled volume in the ECN.
   //--- with extended accuracy
   virtual uint64_t    VolumeInitialExt(void) const=0;
   virtual MTAPIRES  VolumeInitialExt(const uint64_t volume)=0;
   //--- the volume of the order which was created in the ECN to execute the client order: 
   //--- order current volume
   //--- with extended accuracy
   virtual uint64_t    VolumeCurrentExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentExt(const uint64_t volume)=0;
   //--- the volume of the order which was created by the client: source volume and currently executed volume
   //--- the initial operation volume
   //--- with extended accuracy
   virtual uint64_t    VolumeInitialClientExt(void) const=0;
   virtual MTAPIRES  VolumeInitialClientExt(const uint64_t volume)=0;
   //--- the volume of the order which was created by the client: source volume and currently executed volume
   //--- order current volume
   //--- with extended accuracy
   virtual uint64_t    VolumeCurrentClientExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentClientExt(const uint64_t volume)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTECNMatching(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Matching Order Array Interface                               |
//+------------------------------------------------------------------+
class IMTECNMatchingArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNMatchingArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTECNMatching* order)=0;
   virtual MTAPIRES  AddCopy(const IMTECNMatching* order)=0;
   virtual MTAPIRES  Add(IMTECNMatchingArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTECNMatchingArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTECNMatching* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTECNMatching* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTECNMatching* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTECNMatching* Next(const uint32_t index) const=0;
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
                    ~IMTECNMatchingArray(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Filling Order Interface                                      |
//+------------------------------------------------------------------+
class IMTECNFilling
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNFilling* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- MT5 client order login (initial order login)
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- MT5 client order (initial order)
   virtual uint64_t    Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- MT5 trade server id
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- time of order creation in the ECN in us since 1970.01.01
   virtual int64_t     TimeSetupMsc(void) const=0;
   virtual MTAPIRES  TimeSetupMsc(const int64_t time)=0;
   //--- order ID in the external system. The ID is filled by the gateway through which the order is sent for execution
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- order state EnOrderState
   virtual uint32_t  State(void) const=0;
   virtual MTAPIRES  State(const uint32_t state)=0;
   //--- order symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- EnOrderType - the type of the order which was created for sending to the external system: buy, sell, buy limit or sell limit
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- EnOrderFilling - fill policy applicable to the order. It is determined by the original client order
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- expiration mode of the filling order. It is determined by the original client order
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //--- the volume of filling orders: the original volume created by the client
   //--- with extended accuracy
   virtual uint64_t    VolumeInitialExt(void) const=0;
   virtual MTAPIRES  VolumeInitialExt(const uint64_t volume)=0;
   //---  the volume of filling orders: current filled volume
   //--- with extended accuracy
   virtual uint64_t    VolumeCurrentExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentExt(const uint64_t volume)=0;
   //--- the price of the order
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- allowable deviation for order execution. It is specified in filling settings.
   virtual uint32_t  Deviation(void) const=0;
   virtual MTAPIRES  Deviation(const uint32_t deviation)=0;
   //--- the gateway which is used to forward the order to the external system
   virtual uint64_t    Provider(void) const=0;
   virtual MTAPIRES  Provider(const uint64_t provider_id)=0;
   //--- order comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTECNFilling(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Matching Order Array Interface                               |
//+------------------------------------------------------------------+
class IMTECNFillingArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNFillingArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTECNFilling* order)=0;
   virtual MTAPIRES  AddCopy(const IMTECNFilling* order)=0;
   virtual MTAPIRES  Add(IMTECNFillingArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTECNFillingArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTECNFilling* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTECNFilling* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTECNFilling* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTECNFilling* Next(const uint32_t index) const=0;
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
                    ~IMTECNFillingArray(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Filling Order Interface                                      |
//+------------------------------------------------------------------+
class IMTECNProvider
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNProvider* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- record id
   virtual uint64_t    ID(void) const=0;
   //--- provider id
   virtual uint64_t    ProviderID(void) const=0;
   //--- provider name
   virtual LPCWSTR   Name(void) const=0;
   //--- provider module name
   virtual LPCWSTR   Module(void) const=0;
   //--- provider version
   virtual uint32_t  Version(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTECNProvider(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Matching Order Array Interface                               |
//+------------------------------------------------------------------+
class IMTECNProviderArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNProviderArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTECNProvider* provider)=0;
   virtual MTAPIRES  AddCopy(const IMTECNProvider* provider)=0;
   virtual MTAPIRES  Add(IMTECNProviderArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTECNProviderArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTECNProvider* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTECNProvider* provider)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTECNProvider* provider)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTECNProvider* Next(const uint32_t index) const=0;
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
                    ~IMTECNProviderArray(void) {}
  };
//+------------------------------------------------------------------+
//| ECN History Matching Order Interface                             |
//+------------------------------------------------------------------+
class IMTECNHistoryMatching
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNHistoryMatching* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- MT5 client order
   virtual uint64_t    Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- MT5 client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- MT5 trade server id
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- order state IMTECNMatching::ENCMatchingOrderState
   virtual uint32_t  State(void) const=0;
   virtual MTAPIRES  State(const uint32_t state)=0;
   //--- time of order placing by a client in us since 1970.01.01
   virtual int64_t     TimeSetupMsc(void) const=0;
   virtual MTAPIRES  TimeSetupMsc(const int64_t time)=0;
   //--- order execution time in the ECN
   virtual int64_t     TimeDoneMsc(void) const=0;
   virtual MTAPIRES  TimeDoneMsc(const int64_t time)=0;
   //--- order expiration
   virtual int64_t     TimeExpiration(void) const=0;
   virtual MTAPIRES  TimeExpiration(const int64_t time)=0;
   //--- the name of the financial symbol for which the order in the ECN was placed
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- the name of the financial symbol for which the client order was placed
   virtual LPCWSTR   SymbolClient(void) const=0;
   virtual MTAPIRES  SymbolClient(LPCWSTR symbol)=0;
   //--- EnOrderType -  the type of the order which was placed in the ECN: buy, sell, buy limit or sell limit
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- EnOrderType -  the type of the order which was placed by the client: buy, sell, buy limit or sell limit
   virtual uint32_t  TypeClient(void) const=0;
   virtual MTAPIRES  TypeClient(const uint32_t type)=0;
   //--- EnOrderFilling - the fill policy of the order, which was created in the ECN
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- EnOrderFilling - the fill policy of the order, specified in the original client order
   virtual uint32_t  TypeFillClient(void) const=0;
   virtual MTAPIRES  TypeFillClient(const uint32_t type)=0;
   //--- EnOrderTime - expiration mode of the order which was created in the ECN
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //--- EnOrderTime - expiration mode of the order, which is specified in the original client order
   virtual uint32_t  TypeTimeClient(void) const=0;
   virtual MTAPIRES  TypeTimeClient(const uint32_t type)=0;
   //--- the price of the order which was created in the ECN to execute the client order
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- the price specified in the original client order
   virtual double    PriceClient(void) const=0;
   virtual MTAPIRES  PriceClient(const double price)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- client order price digits
   virtual uint32_t  DigitsClient(void) const=0;
   virtual MTAPIRES  DigitsClient(const uint32_t digits)=0;
   //--- the volume of the order which was created in the ECN to execute the client order: 
   //--- the initial operation volume requested by the client in the order and the current filled volume in the ECN.
   //--- with extended accuracy
   virtual uint64_t    VolumeInitialExt(void) const=0;
   virtual MTAPIRES  VolumeInitialExt(const uint64_t volume)=0;
   //--- the volume of the order which was created in the ECN to execute the client order: 
   //--- order current volume
   //--- with extended accuracy
   virtual uint64_t    VolumeCurrentExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentExt(const uint64_t volume)=0;
   //--- the volume of the order which was created by the client: source volume and currently executed volume
   //--- the initial operation volume
   //--- with extended accuracy
   virtual uint64_t    VolumeInitialClientExt(void) const=0;
   virtual MTAPIRES  VolumeInitialClientExt(const uint64_t volume)=0;
   //--- the volume of the order which was created by the client: source volume and currently executed volume
   //--- order current volume
   //--- with extended accuracy
   virtual uint64_t    VolumeCurrentClientExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentClientExt(const uint64_t volume)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTECNHistoryMatching(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Matching History Order Array Interface                       |
//+------------------------------------------------------------------+
class IMTECNHistoryMatchingArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNHistoryMatchingArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTECNHistoryMatching* order)=0;
   virtual MTAPIRES  AddCopy(const IMTECNHistoryMatching* order)=0;
   virtual MTAPIRES  Add(IMTECNHistoryMatchingArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTECNHistoryMatchingArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTECNHistoryMatching* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTECNHistoryMatching* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTECNHistoryMatching* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTECNHistoryMatching* Next(const uint32_t index) const=0;
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
                    ~IMTECNHistoryMatchingArray(void) {}
  };
//+------------------------------------------------------------------+
//| ECN History Filling Order Interface                              |
//+------------------------------------------------------------------+
class IMTECNHistoryFilling
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNHistoryFilling* order)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- MT5 client order
   virtual uint64_t    Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- the ticket of an opposite order which is being used to fill the current order. 
   //--- it is filled in only if the order is matched within the cluster, with the counter order of another client in MetaTrader 5.
   virtual uint64_t    OrderMatching(void) const=0;
   virtual MTAPIRES  OrderMatching(const uint64_t order)=0;
   //--- the internal ticket of the execution order sent to liquidity provider
   virtual uint64_t    OrderGateway(void) const=0;
   virtual MTAPIRES  OrderGateway(const uint64_t order)=0;
   //--- MT5 client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- the login of the client, who placed an opposite order which is being used to fill the current order. 
   //--- it is filled in only if the order is matched within the cluster, with the counter order of another client in MetaTrader 5.
   virtual uint64_t    LoginMatching(void) const=0;
   virtual MTAPIRES  LoginMatching(const uint64_t login)=0;
   //--- MT5 trade server id
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- time of order placing by a client in us since 1970.01.01
   virtual int64_t     TimeSetupMsc(void) const=0;
   virtual MTAPIRES  TimeSetupMsc(const int64_t time)=0;
   //--- order execution time in the ECN
   virtual int64_t     TimeDoneMsc(void) const=0;
   virtual MTAPIRES  TimeDoneMsc(const int64_t time)=0;
   //--- order ID in the external system. the ID is filled by the gateway through which the order is sent for execution
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- order symbol in ECN
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- EnOrderType - the type of the order which was created for sending to the external system: buy, sell, buy limit or sell limit
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- EnOrderFilling - fill policy applicable to the order. It is determined by the original client order
   virtual uint32_t  TypeFill(void) const=0;
   virtual MTAPIRES  TypeFill(const uint32_t type)=0;
   //--- expiration mode of the filling order. It is determined by the original client order
   virtual uint32_t  TypeTime(void) const=0;
   virtual MTAPIRES  TypeTime(const uint32_t type)=0;
   //---  the volume of filling orders: the original volume created by the client
   //--- with extended accuracy
   virtual uint64_t    VolumeInitialExt(void) const=0;
   virtual MTAPIRES  VolumeInitialExt(const uint64_t volume)=0;
   //---  the volume of filling orders: current filled volume
   //--- with extended accuracy
   virtual uint64_t    VolumeCurrentExt(void) const=0;
   virtual MTAPIRES  VolumeCurrentExt(const uint64_t volume)=0;
   //--- the price of the order which was created in the ECN to execute the client order
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- allowable deviation for order execution. It is specified in filling settings.
   virtual uint32_t  Deviation(void) const=0;
   virtual MTAPIRES  Deviation(const uint32_t deviation)=0;
   //--- the gateway which is used to forward the order to the external system
   virtual uint64_t    Provider(void) const=0;
   virtual MTAPIRES  Provider(const uint64_t provider_id)=0;
   //--- order comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTECNHistoryFilling(void) {}
  };
//+------------------------------------------------------------------+
//| ECN Filling History Order Array Interface                        |
//+------------------------------------------------------------------+
class IMTECNHistoryFillingArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNHistoryFillingArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTECNHistoryFilling* order)=0;
   virtual MTAPIRES  AddCopy(const IMTECNHistoryFilling* order)=0;
   virtual MTAPIRES  Add(IMTECNHistoryFillingArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTECNHistoryFillingArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTECNHistoryFilling* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTECNHistoryFilling* order)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTECNHistoryFilling* order)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTECNHistoryFilling* Next(const uint32_t index) const=0;
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
                    ~IMTECNHistoryFillingArray(void) {}
  };
//+------------------------------------------------------------------+
//| ECN History Deal Interface                                       |
//+------------------------------------------------------------------+
class IMTECNHistoryDeal
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNHistoryDeal* deal)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- MT5 client order
   virtual uint64_t    Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- the internal ticket of the execution order sent to liquidity provider
   virtual uint64_t    OrderGateway(void) const=0;
   virtual MTAPIRES  OrderGateway(const uint64_t order)=0;
   //---  the internal ticket of the deal; it is only used in ECN for internal purposes.
   virtual uint64_t    DealGateway(void) const=0;
   virtual MTAPIRES  DealGateway(const uint64_t deal)=0;
   //--- MT5 client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- MT5 trade server id
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t server)=0;
   //--- deal ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- deal creation datetime in us since 1970.01.01
   virtual int64_t     TimeMsc(void) const=0;
   virtual MTAPIRES  TimeMsc(const int64_t time)=0;
   //--- symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- EnDealAction
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t action)=0;
   //--- deal volume
   //--- with extended accuracy
   virtual uint64_t    VolumeExt(void) const=0;
   virtual MTAPIRES  VolumeExt(const uint64_t volume)=0;
   //--- the price at which the deal was executed at the platform side, taking into account price translation settings
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- the price at which the deal was actually executed at the external system side.
   virtual double    PriceGateway(void) const=0;
   virtual MTAPIRES  PriceGateway(const double price)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- gateway price digits
   virtual uint32_t  DigitsGateway(void) const=0;
   virtual MTAPIRES  DigitsGateway(const uint32_t digits)=0;
   //--- commission charged by the external system for the deal. The value is filled by gateways.
   virtual double    Commission(void) const=0;
   virtual MTAPIRES  Commission(const double price)=0;
   //--- the gateway which is used to forward the order to the external system
   virtual uint64_t    Provider(void) const=0;
   virtual MTAPIRES  Provider(const uint64_t provider_id)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTECNHistoryDeal(void) {}
  };
//+------------------------------------------------------------------+
//| ECN ECN History Deal Array Interface                             |
//+------------------------------------------------------------------+
class IMTECNHistoryDealArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTECNHistoryDealArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTECNHistoryDeal* deal)=0;
   virtual MTAPIRES  AddCopy(const IMTECNHistoryDeal* deal)=0;
   virtual MTAPIRES  Add(IMTECNHistoryDealArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTECNHistoryDealArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTECNHistoryDeal* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTECNHistoryDeal* deal)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTECNHistoryDeal* deal)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTECNHistoryDeal* Next(const uint32_t index) const=0;
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
                    ~IMTECNHistoryDealArray(void) {}
  };
//+------------------------------------------------------------------+
