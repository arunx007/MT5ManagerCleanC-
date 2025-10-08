//+------------------------------------------------------------------+
//|                                                MetaTrader 5 API  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Commission tier config                                           |
//+------------------------------------------------------------------+
class IMTConLeverageTier
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConLeverageTier* cfg)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- tier range from
   virtual double    RangeFrom(void) const=0;
   virtual MTAPIRES  RangeFrom(const double value)=0;
   //--- tier range to
   virtual double    RangeTo(void) const=0;
   virtual MTAPIRES  RangeTo(const double value)=0;
   //--- rate for initial orders
   virtual double    MarginRateInitial(void) const=0;
   virtual MTAPIRES  MarginRateInitial(const double value)=0;
   //--- rate for position maintenance
   virtual double    MarginRateMaintenance(void) const=0;
   virtual MTAPIRES  MarginRateMaintenance(const double value)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConLeverageTier(void) {}
  };
//+------------------------------------------------------------------+
//| Floating Leverage rule configuration interface                   |
//+------------------------------------------------------------------+
class IMTConLeverageRule
  {
public:
   //--- range modes
   enum EnRangeMode
     {
      RANGE_VOLUME           =0,  // range in volume
      RANGE_VOLUME_PER_SYMBOL=1,  // range in volume per symbol
      RANGE_VALUE            =2,  // range in value
      RANGE_VALUE_PER_SYMBOL =3,  // range in value per symbol
      //--- enumeration borders
      RANGE_FIRST          =RANGE_VOLUME,
      RANGE_LAST           =RANGE_VALUE_PER_SYMBOL
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConLeverageRule* cfg)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- commission name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- description
   virtual LPCWSTR   Description(void) const=0;
   virtual MTAPIRES  Description(LPCWSTR descr)=0;
   //--- symbols path
   virtual LPCWSTR   Path(void) const=0;
   virtual MTAPIRES  Path(LPCWSTR path)=0;
   //--- EnRangeMode
   virtual uint32_t  RangeMode(void) const=0;
   virtual MTAPIRES  RangeMode(const uint32_t mode)=0;
   //---- value calculation currency
   virtual LPCWSTR   RangeValueCurrency(void) const=0;
   virtual MTAPIRES  RangeValueCurrency(LPCWSTR currency)=0;
   //--- commission tiers
   virtual MTAPIRES  TierAdd(IMTConLeverageTier* tier)=0;
   virtual MTAPIRES  TierUpdate(const uint32_t pos,const IMTConLeverageTier* tier)=0;
   virtual MTAPIRES  TierDelete(const uint32_t pos)=0;
   virtual MTAPIRES  TierClear(void)=0;
   virtual MTAPIRES  TierShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  TierTotal(void) const=0;
   virtual MTAPIRES  TierNext(const uint32_t pos,IMTConLeverageTier* tier) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConLeverageRule(void) {}
  };
//+------------------------------------------------------------------+
//| Floating Leverage configuration interface                        |
//+------------------------------------------------------------------+
class IMTConLeverage
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConLeverage* cfg)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- rules
   virtual MTAPIRES  RuleAdd(IMTConLeverageRule* cfg)=0;
   virtual MTAPIRES  RuleUpdate(const uint32_t pos,const IMTConLeverageRule* cfg)=0;
   virtual MTAPIRES  RuleDelete(const uint32_t pos)=0;
   virtual MTAPIRES  RuleClear(void)=0;
   virtual MTAPIRES  RuleShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  RuleTotal(void) const=0;
   virtual MTAPIRES  RuleNext(const uint32_t pos,IMTConLeverageRule* cfg) const=0;
   virtual MTAPIRES  RuleGet(LPCWSTR name,IMTConLeverageRule* cfg) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConLeverage(void) {}
  };
//+------------------------------------------------------------------+
//| Group configuration array interface                              |
//+------------------------------------------------------------------+
class IMTConLeverageArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConLeverageArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTConLeverage* record)=0;
   virtual MTAPIRES  AddCopy(const IMTConLeverage* record)=0;
   virtual MTAPIRES  Add(IMTConLeverageArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTConLeverageArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTConLeverage* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTConLeverage* record)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTConLeverage* record)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTConLeverage*  Next(const uint32_t index) const=0;
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
                    ~IMTConLeverageArray(void) {}
  };
//+------------------------------------------------------------------+
//| Leverage config events notification interface                    |
//+------------------------------------------------------------------+
class IMTConLeverageSink
  {
public:
   virtual void      OnLeverageAdd(const IMTConLeverage*    /*config*/) {  }
   virtual void      OnLeverageUpdate(const IMTConLeverage* /*config*/) {  }
   virtual void      OnLeverageDelete(const IMTConLeverage* /*config*/) {  }
   virtual void      OnLeverageSync(void)                            {  }
   //--- config modification hooks (only for Server API)
   virtual MTAPIRES  HookLeverageAdd(const uint64_t /*login*/,IMTConLeverage* /*new_cfg*/)                                  { return(MT_RET_OK); }
   virtual MTAPIRES  HookLeverageUpdate(const uint64_t /*login*/,const IMTConLeverage* /*cfg*/,IMTConLeverage* /*new_cfg*/) { return(MT_RET_OK); }
   virtual MTAPIRES  HookLeverageDelete(const uint64_t /*login*/,const IMTConLeverage* /*cfg*/)                             { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+ 
