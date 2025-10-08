//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Name-value parameter interface                                   |
//+------------------------------------------------------------------+
class IMTConParam
  {
public:
   //--- parameter types
   enum EnParamType
     {
      TYPE_STRING    =0,   // string
      TYPE_INT       =1,   // integer
      TYPE_FLOAT     =2,   // floating
      TYPE_TIME      =3 ,  // time only
      TYPE_DATE      =4 ,  // date only
      TYPE_DATETIME  =5 ,  // date & time
      TYPE_GROUPS    =6,   // groups list
      TYPE_SYMBOLS   =7,   // symbols list
      TYPE_BOOL      =8,   // bool
      TYPE_COLOR     =9,   // color
      //--- enumeration borders
      TYPE_FIRST     =TYPE_STRING,
      TYPE_LAST      =TYPE_COLOR
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConParam* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- parameter name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- parameter type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- parameter value (raw string)
   virtual LPCWSTR   Value(void) const=0;
   virtual MTAPIRES  Value(LPCWSTR value)=0;
   //--- parameter value string
   virtual LPCWSTR   ValueString(void) const=0;
   virtual MTAPIRES  ValueString(LPCWSTR value)=0;
   //--- parameter value integer
   virtual int64_t     ValueInt(void) const=0;
   virtual MTAPIRES  ValueInt(const int64_t value)=0;
   //--- parameter value float
   virtual double    ValueFloat(void) const=0;
   virtual MTAPIRES  ValueFloat(const double value)=0;
   //--- parameter value time
   virtual int64_t     ValueTime(void) const=0;
   virtual MTAPIRES  ValueTime(const int64_t value)=0;
   //--- parameter value datetime
   virtual int64_t     ValueDatetime(void) const=0;
   virtual MTAPIRES  ValueDatetime(const int64_t value)=0;
   //--- parameter value groups
   virtual LPCWSTR   ValueGroups(void) const=0;
   virtual MTAPIRES  ValueGroups(LPCWSTR value)=0;
   //--- parameter value symbols
   virtual LPCWSTR   ValueSymbols(void) const=0;
   virtual MTAPIRES  ValueSymbols(LPCWSTR value)=0;
   //--- parameter value bool
   virtual bool      ValueBool(void) const=0;
   virtual MTAPIRES  ValueBool(const bool value)=0;
   //--- value color
   virtual COLORREF  ValueColor(void) const=0;
   virtual MTAPIRES  ValueColor(const COLORREF value)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConParam(void) {}
  };
//+------------------------------------------------------------------+
//| Name-value parameter array interface                             |
//+------------------------------------------------------------------+
class IMTConParamArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConParamArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTConParam* param)=0;
   virtual MTAPIRES  AddCopy(const IMTConParam* param)=0;
   virtual MTAPIRES  Add(IMTConParamArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTConParamArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTConParam* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTConParam* param)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTConParam* param)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTConParam* Next(const uint32_t index) const=0;
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
                    ~IMTConParamArray(void) {}
  };
//+------------------------------------------------------------------+
