//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Spread leg config                                                |
//+------------------------------------------------------------------+
class IMTConSpreadLeg
  {
public:
   //--- leg modes
   enum EnLegMode
     {
      LEG_MODE_SYMBOL    =0, // symbol specified by Symbol
      LEG_MODE_FUTURES   =1, // symbol specified by basis in Symbol + expiration range TimeFrom-TimeTo
      //--- enumeration borders
      LEG_MODE_FIRST     =LEG_MODE_SYMBOL,
      LEG_MODE_LAST      =LEG_MODE_FUTURES
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConSpreadLeg* leg)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- mode
   virtual uint32_t  Mode(void) const=0;
   virtual MTAPIRES  Mode(const uint32_t mode)=0;
   //--- symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- time from
   virtual int64_t     TimeFrom(void) const=0;
   virtual MTAPIRES  TimeFrom(const int64_t from)=0;
   //--- time to
   virtual int64_t     TimeTo(void) const=0;
   virtual MTAPIRES  TimeTo(const int64_t to)=0;
   //--- ratio
   virtual uint64_t    Ratio(void) const=0;
   virtual MTAPIRES  Ratio(const uint64_t ratio)=0;
   //--- ratio double
   virtual double    RatioDbl(void) const=0;
   virtual MTAPIRES  RatioDbl(const double ratio)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConSpreadLeg(void) {}
  };
//+------------------------------------------------------------------+
//| Spread config                                                    |
//+------------------------------------------------------------------+
class IMTConSpread
  {
public:
   //--- spread types
   enum EnSpreadMarginType
     {
      MARGIN_TYPE_VALUE    =0,
      MARGIN_TYPE_MAXIMAL  =1,
      MARGIN_TYPE_CME_INTER=2,
      MARGIN_TYPE_CME_INTRA=3,
      //--- enumeration borders
      MARGIN_TYPE_FIRST    =MARGIN_TYPE_VALUE,
      MARGIN_TYPE_LAST     =MARGIN_TYPE_CME_INTRA
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConSpread* spread)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint32_t  ID(void) const=0;
   //--- margin type
   virtual uint32_t  MarginType(void) const=0;
   virtual MTAPIRES  MarginType(const uint32_t type)=0;
   //--- initial margin
   virtual double    MarginInitial(void) const=0;
   virtual MTAPIRES  MarginInitial(const double margin)=0;
   //--- maintenance margin
   virtual double    MarginMaintenance(void) const=0;
   virtual MTAPIRES  MarginMaintenance(const double margin)=0;
   //--- leg A
   virtual MTAPIRES  ALegAdd(IMTConSpreadLeg* leg)=0;
   virtual MTAPIRES  ALegUpdate(const uint32_t pos,const IMTConSpreadLeg* leg)=0;
   virtual MTAPIRES  ALegDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ALegClear(void)=0;
   virtual MTAPIRES  ALegShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ALegTotal(void) const=0;
   virtual MTAPIRES  ALegNext(const uint32_t pos,IMTConSpreadLeg* leg) const=0;
   //--- leg B
   virtual MTAPIRES  BLegAdd(IMTConSpreadLeg* leg)=0;
   virtual MTAPIRES  BLegUpdate(const uint32_t pos,const IMTConSpreadLeg* leg)=0;
   virtual MTAPIRES  BLegDelete(const uint32_t pos)=0;
   virtual MTAPIRES  BLegClear(void)=0;
   virtual MTAPIRES  BLegShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  BLegTotal(void) const=0;
   virtual MTAPIRES  BLegNext(const uint32_t pos,IMTConSpreadLeg* leg) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConSpread(void) {}
  };
//+------------------------------------------------------------------+
//| Spread config events notification interface                      |
//+------------------------------------------------------------------+
class IMTConSpreadSink
  {
public:
   virtual void      OnSpreadAdd(const IMTConSpread*    /*config*/) {  }
   virtual void      OnSpreadUpdate(const IMTConSpread* /*config*/) {  }
   virtual void      OnSpreadDelete(const IMTConSpread* /*config*/) {  }
   virtual void      OnSpreadSync(void)                             {  }
  };
//+------------------------------------------------------------------+
