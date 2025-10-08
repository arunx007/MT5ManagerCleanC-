//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Online connection interface                                      |
//+------------------------------------------------------------------+
class IMTOnline
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTOnline* online)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- unique session id
   virtual uint64_t    SessionID(void) const=0;
   //--- client login
   virtual uint64_t    Login(void) const=0;
   //--- client group
   virtual LPCWSTR   Group(void) const=0;
   //--- client ip
   virtual LPCWSTR   Address(MTAPISTR& ip) const=0;
   //--- client terminal type from IMTUser::EnUsersConnectionTypes
   virtual uint32_t  Type(void) const=0;
   //--- client terminal build
   virtual uint32_t  Build(void) const=0;
   //--- connection time
   virtual int64_t     Time(void) const=0;
   //--- unique computer id
   virtual LPCWSTR   ComputerID(MTAPISTR& cid) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTOnline(void) {}
  };
//+------------------------------------------------------------------+
//| Account array interface                                          |
//+------------------------------------------------------------------+
class IMTOnlineArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTOnlineArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTOnline* online)=0;
   virtual MTAPIRES  AddCopy(const IMTOnline* online)=0;
   virtual MTAPIRES  Add(IMTOnlineArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTOnlineArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTOnline* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTOnline* online)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTOnline* online)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTOnline* Next(const uint32_t index) const=0;
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
                    ~IMTOnlineArray(void) {}
  };
//+------------------------------------------------------------------+