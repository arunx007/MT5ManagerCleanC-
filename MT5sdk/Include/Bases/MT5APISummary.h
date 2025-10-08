//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade summary interface                                          |
//+------------------------------------------------------------------+
class IMTSummary
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTSummary* summary)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- summary symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual uint32_t  Digits(void) const=0;
   //--- total positions
   virtual uint32_t  PositionClients(void) const=0;
   virtual uint32_t  PositionCoverage(void) const=0;
   //--- volumes
   virtual uint64_t    VolumeBuyClients(void) const=0;
   virtual uint64_t    VolumeBuyCoverage(void) const=0;
   virtual uint64_t    VolumeSellClients(void) const=0;
   virtual uint64_t    VolumeSellCoverage(void) const=0;
   virtual double    VolumeNet(void) const=0;
   //--- average prices
   virtual double    PriceBuyClients(void) const=0;
   virtual double    PriceBuyCoverage(void) const=0;
   virtual double    PriceSellClients(void) const=0;
   virtual double    PriceSellCoverage(void) const=0;
   //--- profit
   virtual double    ProfitClients(void) const=0;
   virtual double    ProfitCoverage(void) const=0;
   virtual double    ProfitFullClients(void) const=0;
   virtual double    ProfitFullCoverage(void) const=0;
   virtual double    ProfitUncovered(void) const=0;
   virtual double    ProfitUncoveredFull(void) const=0;
   //--- volumes with extended accuracy
   virtual uint64_t    VolumeBuyClientsExt(void) const=0;
   virtual uint64_t    VolumeBuyCoverageExt(void) const=0;
   virtual uint64_t    VolumeSellClientsExt(void) const=0;
   virtual uint64_t    VolumeSellCoverageExt(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTSummary(void) {}
  };
//+------------------------------------------------------------------+
//| Trade summary array interface                                    |
//+------------------------------------------------------------------+
class IMTSummaryArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTSummaryArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTSummary* summary)=0;
   virtual MTAPIRES  AddCopy(const IMTSummary* summary)=0;
   virtual MTAPIRES  Add(IMTSummaryArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTSummaryArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTSummary* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTSummary* summary)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTSummary* summary)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTSummary* Next(const uint32_t index) const=0;
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
                    ~IMTSummaryArray(void) {}
  };
//+------------------------------------------------------------------+
//| Trade summary events notification interface                      |
//+------------------------------------------------------------------+
class IMTSummarySink
  {
public:
   virtual void      OnSummaryUpdate(const IMTSummary* /*summary*/) { }
  };
//+------------------------------------------------------------------+
  