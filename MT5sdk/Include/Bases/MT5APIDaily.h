//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Trade account interface                                          |
//+------------------------------------------------------------------+
class IMTDaily
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTDaily* exec)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- date and time
   virtual int64_t     Datetime(void) const=0;
   virtual MTAPIRES  Datetime(const int64_t datetime)=0;
   //--- previous generation datetime
   virtual int64_t     DatetimePrev(void) const=0;
   virtual MTAPIRES  DatetimePrev(const int64_t datetime)=0;
   //--- login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- group
   virtual LPCWSTR   Group(void) const=0;
   virtual MTAPIRES  Group(LPCWSTR group)=0;
   //--- currency
   virtual LPCWSTR   Currency(void) const=0;
   virtual MTAPIRES  Currency(LPCWSTR curr)=0;
   virtual uint32_t  CurrencyDigits(void) const=0;
   //--- company
   virtual LPCWSTR   Company(void) const=0;
   virtual MTAPIRES  Company(LPCWSTR company)=0;
   //--- e-mail
   virtual LPCWSTR   EMail(void) const=0;
   virtual MTAPIRES  EMail(LPCWSTR mail)=0;
   //--- balance
   virtual double    Balance(void) const=0;
   virtual MTAPIRES  Balance(const double balance)=0;
   //--- credit
   virtual double    Credit(void) const=0;
   virtual MTAPIRES  Credit(const double credit)=0;
   //--- interest rate
   virtual double    InterestRate(void) const=0;
   virtual MTAPIRES  InterestRate(const double credit)=0;
   //--- commission daily
   virtual double    CommissionDaily(void) const=0;
   virtual MTAPIRES  CommissionDaily(const double comm)=0;
   //--- commission monthly
   virtual double    CommissionMonthly(void) const=0;
   virtual MTAPIRES  CommissionMonthly(const double comm)=0;
   //--- commission daily
   virtual double    AgentDaily(void) const=0;
   virtual MTAPIRES  AgentDaily(const double agent)=0;
   //--- commission monthly
   virtual double    AgentMonthly(void) const=0;
   virtual MTAPIRES  AgentMonthly(const double agent)=0;
   //--- last day balance
   virtual double    BalancePrevDay(void) const=0;
   virtual MTAPIRES  BalancePrevDay(const double balance)=0;
   //--- last month balance
   virtual double    BalancePrevMonth(void) const=0;
   virtual MTAPIRES  BalancePrevMonth(const double balance)=0;
   //--- last day equity
   virtual double    EquityPrevDay(void) const=0;
   virtual MTAPIRES  EquityPrevDay(const double balance)=0;
   //--- last month equity
   virtual double    EquityPrevMonth(void) const=0;
   virtual MTAPIRES  EquityPrevMonth(const double balance)=0;
   //---
   //--- margin
   //---
   virtual double    Margin(void) const=0;
   virtual MTAPIRES  Margin(const double margin)=0;
   //--- free margin 
   virtual double    MarginFree(void) const=0;
   virtual MTAPIRES  MarginFree(const double margin_free)=0;
   //--- margin level
   virtual double    MarginLevel(void) const=0;
   virtual MTAPIRES  MarginLevel(const double margin_level)=0;
   //--- margin leverage
   virtual uint32_t  MarginLeverage(void) const=0;
   virtual MTAPIRES  MarginLeverage(const uint32_t leverage)=0;
   //---
   //--- floating profit
   //---
   virtual double    Profit(void) const=0;
   virtual MTAPIRES  Profit(const double profit)=0;
   //--- storage
   virtual double    ProfitStorage(void) const=0;
   virtual MTAPIRES  ProfitStorage(const double storage)=0;
   //--- obsolete value
   virtual double    ObsoleteValue(void) const=0;
   virtual MTAPIRES  ObsoleteValue(const double value)=0;
   //--- equity
   virtual double    ProfitEquity(void) const=0;
   virtual MTAPIRES  ProfitEquity(const double equity)=0;
   //---
   //--- daily fixed profit details
   //---
   virtual double    DailyProfit(void) const=0;
   virtual MTAPIRES  DailyProfit(const double profit)=0;
   //---
   virtual double    DailyBalance(void) const=0;
   virtual MTAPIRES  DailyBalance(const double balance)=0;
   //---
   virtual double    DailyCredit(void) const=0;
   virtual MTAPIRES  DailyCredit(const double comm)=0;
   //---
   virtual double    DailyCharge(void) const=0;
   virtual MTAPIRES  DailyCharge(const double charge)=0;
   //---
   virtual double    DailyCorrection(void) const=0;
   virtual MTAPIRES  DailyCorrection(const double correction)=0;
   //---
   virtual double    DailyBonus(void) const=0;
   virtual MTAPIRES  DailyBonus(const double bonus)=0;
   //---
   virtual double    DailyStorage(void) const=0;
   virtual MTAPIRES  DailyStorage(const double storage)=0;
   //---
   virtual double    DailyCommInstant(void) const=0;
   virtual MTAPIRES  DailyCommInstant(const double comm)=0;
   //---
   virtual double    DailyCommRound(void) const=0;
   virtual MTAPIRES  DailyCommRound(const double comm)=0;
   //---
   virtual double    DailyAgent(void) const=0;
   virtual MTAPIRES  DailyAgent(const double comm)=0;
   //---
   virtual double    DailyInterest(void) const=0;
   virtual MTAPIRES  DailyInterest(const double interest)=0;
   //--- list of open positions
   virtual MTAPIRES  PositionAdd(IMTPosition* position)=0;
   virtual MTAPIRES  PositionUpdate(const uint32_t pos,const IMTPosition* position)=0;
   virtual MTAPIRES  PositionDelete(const uint32_t pos)=0;
   virtual MTAPIRES  PositionClear(void)=0;
   virtual MTAPIRES  PositionShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  PositionTotal(void) const=0;
   virtual MTAPIRES  PositionNext(const uint32_t pos,IMTPosition* position) const=0;
   virtual MTAPIRES  PositionGet(LPCWSTR symbol,IMTPosition* position) const=0;
   //--- list of open orders
   virtual MTAPIRES  OrderAdd(IMTOrder* order)=0;
   virtual MTAPIRES  OrderUpdate(const uint32_t pos,const IMTOrder* order)=0;
   virtual MTAPIRES  OrderDelete(const uint32_t pos)=0;
   virtual MTAPIRES  OrderClear(void)=0;
   virtual MTAPIRES  OrderShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  OrderTotal(void) const=0;
   virtual MTAPIRES  OrderNext(const uint32_t pos,IMTOrder* order) const=0;
   virtual MTAPIRES  OrderGet(const uint64_t ticket,IMTOrder* order) const=0;
   //--- assets
   virtual double    ProfitAssets(void) const=0;
   virtual MTAPIRES  ProfitAssets(const double assets)=0;
   //--- liabilities
   virtual double    ProfitLiabilities(void) const=0;
   virtual MTAPIRES  ProfitLiabilities(const double liabilities)=0;
   //--- 
   virtual double    DailyDividend(void) const=0;
   virtual MTAPIRES  DailyDividend(const double dividend)=0;
   //--- 
   virtual double    DailyTaxes(void) const=0;
   virtual MTAPIRES  DailyTaxes(const double taxes)=0;
   //--- 
   virtual double    DailySOCompensation(void) const=0;
   virtual MTAPIRES  DailySOCompensation(const double compensation)=0;
   //--- 
   virtual double    DailyCommFee(void) const=0;
   virtual MTAPIRES  DailyCommFee(const double fee)=0;
   //--- 
   virtual double    DailySOCompensationCredit(void) const=0;
   virtual MTAPIRES  DailySOCompensationCredit(const double compensation)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTDaily(void) {}
  };
//+------------------------------------------------------------------+
//| Deal array interface                                             |
//+------------------------------------------------------------------+
class IMTDailyArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTDailyArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTDaily* daily)=0;
   virtual MTAPIRES  AddCopy(const IMTDaily* daily)=0;
   virtual MTAPIRES  Add(IMTDailyArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTDailyArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTDaily* Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTDaily* daily)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTDaily* daily)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTDaily* Next(const uint32_t index) const=0;
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
                    ~IMTDailyArray(void) {}
  };
//+------------------------------------------------------------------+
//| Deals events notification interface                              |
//+------------------------------------------------------------------+
class IMTDailySink
  {
public:
   virtual void      OnDailyAdd(const IMTDaily*    /*daily*/)  {  }
   virtual void      OnDailyUpdate(const IMTDaily* /*daily*/)  {  }
   virtual void      OnDailyDelete(const IMTDaily* /*daily*/)  {  }
   virtual void      OnDailyClean(const uint64_t /*login*/)      {  }
   virtual void      OnDailySync(void)                         {  }
  };
//+------------------------------------------------------------------+
