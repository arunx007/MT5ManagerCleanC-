//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Active subscription interface                                    |
//+------------------------------------------------------------------+
class IMTSubscription
  {
public:
   //---- subscription status
   enum EnStatus
     {
      STATUS_ACTIVE            =0,          // subscription is active
      STATUS_SUSPENDED         =1,          // suspended due "no money"
      STATUS_CANCELED          =2,          // canceled by client
      //---
      STATUS_FIRST             =STATUS_ACTIVE,
      STATUS_LAST              =STATUS_CANCELED
     };
   //---- subscription flags
   enum EnFlags
     {
      FLAG_FREE_PERIOD         =0x01,        // subscription is activated with free period
      //---
      FLAG_NONE                =0x00,
      FLAG_ALL                 =FLAG_FREE_PERIOD
     };

   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTSubscription* obj)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint64_t    ID(void) const=0;
   //--- client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- subscription id
   virtual uint64_t    Subscription(void) const=0;
   virtual MTAPIRES  Subscription(const uint64_t subscription_id)=0;
   //--- EnStatus
   virtual uint32_t  Status(void) const=0;
   virtual MTAPIRES  Status(const uint32_t status)=0;
   //--- subscribe time
   virtual int64_t     TimeSubscribe(void) const=0;
   virtual MTAPIRES  TimeSubscribe(const int64_t time)=0;
   //--- last renewal time
   virtual int64_t     TimeRenewal(void) const=0;
   virtual MTAPIRES  TimeRenewal(const int64_t time)=0;
   //--- expire time
   virtual int64_t     TimeExpire(void) const=0;
   virtual MTAPIRES  TimeExpire(const int64_t time)=0;
   //--- flags
   virtual uint32_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint32_t flags)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTSubscription(void) {}
  };
//+------------------------------------------------------------------+
//| Active subscriptions array interface                             |
//+------------------------------------------------------------------+
class IMTSubscriptionArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTSubscriptionArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTSubscription* record)=0;
   virtual MTAPIRES  AddCopy(const IMTSubscription* record)=0;
   virtual MTAPIRES  Add(IMTSubscriptionArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTSubscriptionArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTSubscription* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTSubscription* record)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTSubscription* record)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTSubscription*  Next(const uint32_t index) const=0;
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
                    ~IMTSubscriptionArray(void) {}
  };
//+------------------------------------------------------------------+
//| Subscription history action interface                            |
//+------------------------------------------------------------------+
class IMTSubscriptionHistory
  {
public:
   //---- actions
   enum EnAction
     {
      ACTION_SUBSCRIBE         =0,          // subscription
      ACTION_RENEWAL           =1,          // renewal
      ACTION_SUSPEND           =2,          // suspend
      ACTION_CANCEL            =3,          // cancel
      ACTION_DELETED           =4,          // delete
      //--- enumeration borders
      ACTION_FIRST             =ACTION_SUBSCRIBE,
      ACTION_LAST              =ACTION_DELETED
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTSubscriptionHistory* obj)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- id
   virtual uint64_t    ID(void) const=0;
   //--- client login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- subscription config id
   virtual uint64_t    Subscription(void) const=0;
   virtual MTAPIRES  Subscription(const uint64_t subscription_id)=0;
   //--- subscription record id
   virtual uint64_t    Record(void) const=0;
   virtual MTAPIRES  Record(const uint64_t record_id)=0;
   //--- EnStatus
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t status)=0;
   //--- subscribe time
   virtual int64_t     Time(void) const=0;
   virtual MTAPIRES  Time(const int64_t time)=0;
   //--- ammount
   virtual double    Amount(void) const=0;
   virtual MTAPIRES  Amount(const double ammount)=0;
   //--- digits
   virtual uint32_t  AmountDigits(void) const=0;
   virtual MTAPIRES  AmountDigits(const uint32_t digits)=0;
   //--- ammount deal
   virtual uint64_t    AmountDeal(void) const=0;
   virtual MTAPIRES  AmountDeal(const uint64_t deal)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTSubscriptionHistory(void) {}
  };
//+------------------------------------------------------------------+
//| Subscription history action array interface                      |
//+------------------------------------------------------------------+
class IMTSubscriptionHistoryArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTSubscriptionHistoryArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  AddCopy(const IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  Add(IMTSubscriptionHistoryArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTSubscriptionHistoryArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTSubscriptionHistory* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTSubscriptionHistory*  Next(const uint32_t index) const=0;
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
                    ~IMTSubscriptionHistoryArray(void) {}
  };
//+------------------------------------------------------------------+
//| Subscription events notification interface                       |
//+------------------------------------------------------------------+
class IMTSubscriptionSink
  {
public:
   //--- events
   virtual void      OnSubscriptionAdd(const IMTSubscription*    /*subscription*/) {  }
   virtual void      OnSubscriptionUpdate(const IMTSubscription* /*subscription*/) {  }
   virtual void      OnSubscriptionDelete(const IMTSubscription* /*subscription*/) {  }
   virtual void      OnSubscriptionJoin(const uint64_t /*manager*/,const uint64_t /*login*/,const IMTConSubscription* /*config*/,IMTSubscription* /*record*/,IMTSubscriptionHistory* /*history*/) { }
   virtual void      OnSubscriptionCancel(const uint64_t /*manager*/,const uint64_t /*login*/,const IMTConSubscription* /*config*/,IMTSubscription* /*record*/,IMTSubscriptionHistory* /*history*/) { }
  };
//+------------------------------------------------------------------+
//| Subscription history events notification interface               |
//+------------------------------------------------------------------+
class IMTSubscriptionHistorySink
  {
public:
   //--- events
   virtual void      OnSubscriptionHistoryAdd(const IMTSubscriptionHistory*    /*subscription*/) {  }
   virtual void      OnSubscriptionHistoryUpdate(const IMTSubscriptionHistory* /*subscription*/) {  }
   virtual void      OnSubscriptionHistoryDelete(const IMTSubscriptionHistory* /*subscription*/) {  }
  };
//+------------------------------------------------------------------+
