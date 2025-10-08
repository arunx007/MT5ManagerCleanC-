//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Exposure interface                                               |
//+------------------------------------------------------------------+
class IMTExposure
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTExposure* exposure)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- exposure currency or symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual uint32_t  Digits(void) const=0;
   //--- volumes
   virtual double    VolumeClients(void) const=0;
   virtual double    VolumeCoverage(void) const=0;
   //--- conversation rate from symbol volume 
   //--- to net volume in exposure currency
   virtual double    PriceRate(void) const=0;
   //--- net volume in exposure currency
   virtual double    VolumeNet(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTExposure(void) {}
  };
//+------------------------------------------------------------------+
//| Exposure array interface                                         |
//+------------------------------------------------------------------+
class IMTExposureArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTExposureArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTExposure* exposure)=0;
   virtual MTAPIRES  AddCopy(const IMTExposure* exposure)=0;
   virtual MTAPIRES  Add(IMTExposureArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTExposureArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTExposure* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTExposure* exposure)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTExposure* exposure)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTExposure* Next(const uint32_t index) const=0;
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
                    ~IMTExposureArray(void) {}
  };
//+------------------------------------------------------------------+
//| Exposure events notification interface                           |
//+------------------------------------------------------------------+
class IMTExposureSink
  {
public:
   virtual void      OnExposureUpdate(const IMTExposure* /*exposure*/) { }
  };
//+------------------------------------------------------------------+
  