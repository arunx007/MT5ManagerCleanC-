//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Fund trade account                                               |
//+------------------------------------------------------------------+
class IMTConFundAccount
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConFundAccount* account)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   //--- balance
   virtual double    Balance(void) const=0;
   //--- equity
   virtual double    Equity(void) const=0;
   //--- currency
   virtual LPCWSTR   Currency(void) const=0;
   //--- currency digits
   virtual uint32_t  CurrencyDigits(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConFundAccount(void) {}
  };
//+------------------------------------------------------------------+
//| Fund investor                                                    |
//+------------------------------------------------------------------+
class IMTConFundInvestor
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConFundInvestor* investor)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   //--- shares
   virtual uint64_t    SharesVolume(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConFundInvestor(void) {}
  };
//+------------------------------------------------------------------+
//| Fund config                                                      |
//+------------------------------------------------------------------+
class IMTConFund
  {
public:
   //--- flags
   enum EnFlags
     {
      FLAG_NONE               =0x00000000,
      FLAG_ENABLED            =0x00000001,
      //---
      FLAG_ALL                =FLAG_ENABLED
     };
   //--- type
   enum EnType
     {
      TYPE_OPEN_END           =0,
      TYPE_CLOSED_END         =1,
      //---
      TYPE_FIRST              =TYPE_OPEN_END,
      TYPE_LAST               =TYPE_CLOSED_END,
     };
   //--- recalculation
   enum EnRecalculation
     {
      RECALCULATION_MINUTELY=0,
      RECALCULATION_HOURLY  =1,
      RECALCULATION_DAILY   =2,
      RECALCULATION_MANUAL  =3,
      //---
      RECALCULATION_FIRST   =RECALCULATION_MINUTELY,
      RECALCULATION_LAST    =RECALCULATION_MANUAL,
     };
   //--- fees mode
   enum EnFeeMode
     {
      FEE_MODE_AUTOMATIC    =0,
      FEE_MODE_REPORT       =1,
      //---
      FEE_MODE_FIRST        =FEE_MODE_AUTOMATIC,
      FEE_MODE_LAST         =FEE_MODE_REPORT,
     };
   //--- management fee charge
   enum EnFeePeriod
     {
      FEE_PERIOD_DAILY      =0,
      FEE_PERIOD_MONTHLY    =1,
      FEE_PERIOD_QUARTERLY  =2,
      FEE_PERIOD_ANNUAL     =3,
      //---
      FEE_PERIOD_FIRST      =FEE_PERIOD_DAILY,
      FEE_PERIOD_LAST       =FEE_PERIOD_ANNUAL
     };
   //--- assets mode for fee calculation 
   enum EnFeeAssests
     {
      FEE_ASSETS_END         =0,
      FEE_ASSETS_BEGIN       =1,
      FEE_ASSETS_AVERAGE     =2,
      //---
      FEE_ASSETS_FIRST       =FEE_ASSETS_END,
      FEE_ASSETS_LAST        =FEE_ASSETS_AVERAGE,
     };
   //--- success fee calculation modes
   enum EnFeeSuccessCalc
     {
      FEE_SUCCESS_CALC_HURDLE_HWM_SOFT=0,
      FEE_SUCCESS_CALC_HURDLE_HWM_HARD=1,
      //---
      FEE_SUCCESS_CALC_FIRST     =FEE_SUCCESS_CALC_HURDLE_HWM_SOFT,
      FEE_SUCCESS_CALC_LAST      =FEE_SUCCESS_CALC_HURDLE_HWM_HARD
     };
   //--- success fee modes
   enum EnFeeSuccessModes
     {
      FEE_SUCCESS_MODE_BEFORE_MF =0,
      FEE_SUCCESS_MODE_AFTER_MF  =1,
      //---
      FEE_SUCCESS_MODE_FIRST     =FEE_SUCCESS_MODE_BEFORE_MF,
      FEE_SUCCESS_MODE_LAST      =FEE_SUCCESS_MODE_AFTER_MF
     };
   //--- High Water Mark calaculation modes
   enum EnFeeSuccessHWMType
     {
      FEE_SUCCESS_HWM_TYPE_FULL  =0,
      FEE_SUCCESS_HWM_TYPE_QUATER=1,
      FEE_SUCCESS_HWM_TYPE_YEAR  =2,
      //---
      FEE_SUCCESS_HWM_TYPE_FIRST =FEE_SUCCESS_HWM_TYPE_FULL,
      FEE_SUCCESS_HWM_TYPE_LAST  =FEE_SUCCESS_HWM_TYPE_YEAR
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConFund* fund)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- Symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- Symbol Perfomance
   virtual LPCWSTR   SymbolPerfomance(void) const=0;
   virtual MTAPIRES  SymbolPerfomance(LPCWSTR symbol)=0;
   //--- Symbol Assets
   virtual LPCWSTR   SymbolAssets(void) const=0;
   virtual MTAPIRES  SymbolAssets(LPCWSTR symbol)=0;
   //--- Server
   virtual uint64_t    Server(void) const=0;
   virtual MTAPIRES  Server(const uint64_t manager)=0;
   //--- Manager
   virtual uint64_t    Manager(void) const=0;
   virtual MTAPIRES  Manager(const uint64_t manager)=0;
   //--- Flags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- Type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- Recalculation
   virtual uint32_t  Recalculation(void) const=0;
   virtual MTAPIRES  Recalculation(const uint32_t recalculation)=0;
   //--- start period
   virtual int64_t     StartDate(void) const=0;
   virtual MTAPIRES  StartDate(const int64_t date)=0;
   //--- end period
   virtual int64_t     EndDate(void) const=0;
   virtual MTAPIRES  EndDate(const int64_t date)=0;
   //--- currency
   virtual LPCWSTR   Currency(void) const=0;
   virtual MTAPIRES  Currency(LPCWSTR currency)=0;
   //--- Max capital
   virtual double    MaxCapital(void) const=0;
   virtual MTAPIRES  MaxCapital(const double max_capital)=0;
   //--- Max investors
   virtual uint32_t  MaxInvestors(void) const=0;
   virtual MTAPIRES  MaxInvestors(const uint32_t max_investors)=0;
   //--- Fee mode
   virtual uint32_t  FeeMode(void) const=0;
   virtual MTAPIRES  FeeMode(const uint32_t mode)=0;
   //--- Fee period
   virtual uint32_t  FeePeriod(void) const=0;
   virtual MTAPIRES  FeePeriod(const uint32_t period)=0;
   //--- Fee account
   virtual uint64_t    FeeAccount(void) const=0;
   virtual MTAPIRES  FeeAccount(const uint64_t fee_account)=0;
   //--- Management fee Type
   virtual uint32_t  FeeManagementType(void) const=0;
   virtual MTAPIRES  FeeManagementType(const uint32_t fee)=0;
   //--- Management fee value
   virtual double    FeeManagementValue(void) const=0;
   virtual MTAPIRES  FeeManagementValue(const double fee)=0;
   //--- Management assets mode
   virtual uint32_t  FeeManagementAssets(void) const=0;
   virtual MTAPIRES  FeeManagementAssets(const uint32_t mode)=0;
   //--- Success fee calculation mode
   virtual uint32_t  FeeSuccessCalc(void) const=0;
   virtual MTAPIRES  FeeSuccessCalc(const uint32_t mode)=0;
   //--- Success fee apply mode
   virtual uint32_t  FeeSuccessMode(void) const=0;
   virtual MTAPIRES  FeeSuccessMode(const uint32_t mode)=0;
   //--- Success fee apply mode
   virtual double    FeeSuccessValue(void) const=0;
   virtual MTAPIRES  FeeSuccessValue(const double value)=0;
   //--- Success fee High Water Mark calculation mode
   virtual uint32_t  FeeSuccessHWM(void) const=0;
   virtual MTAPIRES  FeeSuccessHWM(const uint32_t mode)=0;
   //--- Success fee hurdle rate
   virtual double    FeeSuccessHurdleRate(void) const=0;
   virtual MTAPIRES  FeeSuccessHurdleRate(const double rate)=0;
   //--- accounts
   virtual MTAPIRES  AccountAdd(IMTConFundAccount* account)=0;
   virtual MTAPIRES  AccountUpdate(const uint32_t pos,const IMTConFundAccount* account)=0;
   virtual MTAPIRES  AccountDelete(const uint32_t pos)=0;
   virtual MTAPIRES  AccountClear(void)=0;
   virtual MTAPIRES  AccountShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  AccountTotal(void) const=0;
   virtual MTAPIRES  AccountNext(const uint32_t pos,IMTConFundAccount* account) const=0;
   //--- investors
   virtual MTAPIRES  InvestorAdd(IMTConFundInvestor* investor)=0;
   virtual MTAPIRES  InvestorUpdate(const uint32_t pos,const IMTConFundInvestor* investor)=0;
   virtual MTAPIRES  InvestorDelete(const uint32_t pos)=0;
   virtual MTAPIRES  InvestorClear(void)=0;
   virtual MTAPIRES  InvestorShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  InvestorTotal(void) const=0;
   virtual MTAPIRES  InvestorNext(const uint32_t pos,IMTConFundInvestor* investor) const=0;
   //--- fund state
   virtual double    StateCurrentCaptital(void) const=0;
   virtual uint32_t  StateCurrentInvestors(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConFund(void) {}
  };
//+------------------------------------------------------------------+
//| Mail config events notification interface                        |
//+------------------------------------------------------------------+
class IMTConFundSink
  {
public:
   virtual void      OnFundAdd(const IMTConFund*    /*config*/) {  }
   virtual void      OnFundUpdate(const IMTConFund* /*config*/) {  }
   virtual void      OnFundDelete(const IMTConFund* /*config*/) {  }
   virtual void      OnFundSync(void)                           {  }
  };
//+------------------------------------------------------------------+
