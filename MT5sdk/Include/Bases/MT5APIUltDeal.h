//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Ultency deal interface                                           |
//+------------------------------------------------------------------+
class IMTUltDeal
  {
public:
   //--- deal types
   enum EnUltDealAction
     {
      DEAL_BUY                 =0,     // buy
      DEAL_SELL                =1,     // sell
      //--- enumeration borders
      DEAL_FIRST               =DEAL_BUY,
      DEAL_LAST                =DEAL_SELL
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUltDeal* deal)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- deal ticket
   virtual uint64_t  Deal(void) const=0;
   virtual MTAPIRES  Deal(const uint64_t id)=0;
   //--- deal ticket in external system
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- ME instance id
   virtual uint64_t  InstanceID(void) const=0;
   virtual MTAPIRES  InstanceID(const uint64_t id)=0;
   //--- liquidity order ticket
   virtual uint64_t  OrderLiquidity(void) const=0;
   virtual MTAPIRES  OrderLiquidity(const uint64_t order)=0;
   //--- matching order ticket
   virtual uint64_t  OrderMatching(void) const=0;
   virtual MTAPIRES  OrderMatching(const uint64_t order)=0;
   //--- liquidity config
   virtual uint32_t  LiquidityProvider(void) const=0;
   virtual MTAPIRES  LiquidityProvider(const uint32_t lp)=0;
   //--- liquidity 
   virtual uint64_t  LiquidityProviderID(void) const=0;
   virtual MTAPIRES  LiquidityProviderID(const uint64_t id)=0;
   //--- source trade account login
   virtual uint64_t  SourceLogin(void) const=0;
   virtual MTAPIRES  SourceLogin(const uint64_t login)=0;
   //--- source client id
   virtual uint64_t  SourceClientID(void) const=0;
   virtual MTAPIRES  SourceClientID(const uint64_t login)=0;
   //--- EnDealAction
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t action)=0;
   //--- deal symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- deal volume
   virtual uint64_t  Size(void) const=0;
   virtual MTAPIRES  Size(const uint64_t volume)=0;
   //--- deal price
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- markup
   virtual int32_t   Markup(void) const=0;
   virtual MTAPIRES  Markup(const int32_t markup)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- contract size
   virtual double    ContractSize(void) const=0;
   virtual MTAPIRES  ContractSize(const double size)=0;
   //--- deal creation datetime in us
   virtual int64_t   Time(void) const=0;
   virtual MTAPIRES  Time(const int64_t time)=0;
   //--- deal symbol
   virtual LPCWSTR   LPSymbol(void) const=0;
   virtual MTAPIRES  LPSymbol(LPCWSTR symbol)=0;
   //--- deal size
   virtual uint64_t  LPSize(void) const=0;
   virtual MTAPIRES  LPSize(const uint64_t size)=0;
   //--- deal volume
   virtual uint64_t  LPVolume(void) const=0;
   virtual MTAPIRES  LPVolume(const uint64_t volume)=0;
   //--- deal price
   virtual double    LPPrice(void) const=0;
   virtual MTAPIRES  LPPrice(const double price)=0;
   //--- markup
   virtual int32_t   LPMarkup(void) const=0;
   virtual MTAPIRES  LPMarkup(const int32_t markup)=0;
   //--- price digits
   virtual uint32_t  LPDigits(void) const=0;
   virtual MTAPIRES  LPDigits(const uint32_t digits)=0;
   //--- contract size
   virtual double    LPContractSize(void) const=0;
   virtual MTAPIRES  LPContractSize(const double size)=0;
   //--- deal creation datetime in us
   virtual double    LPCommission(void) const=0;
   virtual MTAPIRES  LPCommission(const double commission)=0;
   //--- processing time in lp
   virtual int64_t   PerformanceTimeLP(void) const=0;
   virtual MTAPIRES  PerformanceTimeLP(const int64_t ptm)=0;
   //--- processing time in ultency
   virtual int64_t   PerformanceTimeUltency(void) const=0;
   virtual MTAPIRES  PerformanceTimeUltency(const int64_t ptm)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTUltDeal(void) {}
  };
//+------------------------------------------------------------------+
//| Deal array interface                                             |
//+------------------------------------------------------------------+
class IMTUltDealArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTUltDealArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTUltDeal* deal)=0;
   virtual MTAPIRES  AddCopy(const IMTUltDeal* deal)=0;
   virtual MTAPIRES  Add(IMTUltDealArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTUltDealArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTUltDeal* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTUltDeal* deal)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTUltDeal* deal)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTUltDeal*  Next(const uint32_t index) const=0;
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
                    ~IMTUltDealArray(void) {}
  };
//+------------------------------------------------------------------+