//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "MT5APIAccount.h"
#include "MT5APIPosition.h"
//+------------------------------------------------------------------+
//| Trade deal interface                                             |
//+------------------------------------------------------------------+
class IMTDeal
  {
public:
   //--- deal types
   enum EnDealAction
     {
      DEAL_BUY                 =0,     // buy
      DEAL_SELL                =1,     // sell
      DEAL_BALANCE             =2,     // deposit operation
      DEAL_CREDIT              =3,     // credit operation
      DEAL_CHARGE              =4,     // additional charges
      DEAL_CORRECTION          =5,     // correction deals
      DEAL_BONUS               =6,     // bonus
      DEAL_COMMISSION          =7,     // commission
      DEAL_COMMISSION_DAILY    =8,     // daily commission
      DEAL_COMMISSION_MONTHLY  =9,     // monthly commission
      DEAL_AGENT_DAILY         =10,    // daily agent commission
      DEAL_AGENT_MONTHLY       =11,    // monthly agent commission
      DEAL_INTERESTRATE        =12,    // interest rate charges
      DEAL_BUY_CANCELED        =13,    // canceled buy deal
      DEAL_SELL_CANCELED       =14,    // canceled sell deal
      DEAL_DIVIDEND            =15,    // dividend
      DEAL_DIVIDEND_FRANKED    =16,    // franked dividend
      DEAL_TAX                 =17,    // taxes
      DEAL_AGENT               =18,    // instant agent commission
      DEAL_SO_COMPENSATION     =19,    // negative balance compensation after stop-out
      DEAL_SO_COMPENSATION_CREDIT=20,  // credit compensation after stop-out
      //--- enumeration borders
      DEAL_FIRST               =DEAL_BUY,
      DEAL_LAST                =DEAL_SO_COMPENSATION_CREDIT
     };
   //--- deal entry direction
   enum EnDealEntry
     {
      ENTRY_IN                 =0,     // in market
      ENTRY_OUT                =1,     // out of market
      ENTRY_INOUT              =2,     // reverse
      ENTRY_OUT_BY             =3,     // closed by  hedged position
      //--- enumeration borders
      ENTRY_FIRST              =ENTRY_IN,
      ENTRY_LAST               =ENTRY_OUT_BY
     };
   //--- deal creation reasons
   enum EnDealReason
     {
      DEAL_REASON_CLIENT       =0,     // deal placed manually
      DEAL_REASON_EXPERT       =1,     // deal placed by expert
      DEAL_REASON_DEALER       =2,     // deal placed by dealer
      DEAL_REASON_SL           =3,     // deal placed due SL
      DEAL_REASON_TP           =4,     // deal placed due TP
      DEAL_REASON_SO           =5,     // deal placed due Stop-Out
      DEAL_REASON_ROLLOVER     =6,     // deal placed due rollover
      DEAL_REASON_EXTERNAL_CLIENT=7,   // deal placed from the external system by client
      DEAL_REASON_VMARGIN      =8,     // deal placed due variation margin
      DEAL_REASON_GATEWAY      =9,     // deal placed by gateway
      DEAL_REASON_SIGNAL       =10,    // deal placed by signal service
      DEAL_REASON_SETTLEMENT   =11,    // deal placed due to settlement
      DEAL_REASON_TRANSFER     =12,    // deal placed due position transfer
      DEAL_REASON_SYNC         =13,    // deal placed due position synchronization
      DEAL_REASON_EXTERNAL_SERVICE=14, // deal placed from the external system due service issues
      DEAL_REASON_MIGRATION    =15,    // deal placed due migration
      DEAL_REASON_MOBILE       =16,    // deal placed manually by mobile terminal
      DEAL_REASON_WEB          =17,    // deal placed manually by web terminal
      DEAL_REASON_SPLIT        =18,    // deal placed due split
      DEAL_REASON_CORPORATE_ACTION=19, // deal placed due corporate action
      //--- enumeration borders
      DEAL_REASON_FIRST        =DEAL_REASON_CLIENT,
      DEAL_REASON_LAST         =DEAL_REASON_CORPORATE_ACTION
     };
   //--- modification flags
   enum EnTradeModifyFlags
     {
      MODIFY_FLAGS_ADMIN       =0x00000001,
      MODIFY_FLAGS_MANAGER     =0x00000002,
      MODIFY_FLAGS_POSITION    =0x00000004,
      MODIFY_FLAGS_RESTORE     =0x00000008,
      MODIFY_FLAGS_API_ADMIN   =0x00000010,
      MODIFY_FLAGS_API_MANAGER =0x00000020,
      MODIFY_FLAGS_API_SERVER  =0x00000040,
      MODIFY_FLAGS_API_GATEWAY =0x00000080,
      //--- enumeration borders
      MODIFY_FLAGS_NONE         =0x00000000,
      MODIFY_FLAGS_ALL          =MODIFY_FLAGS_ADMIN|MODIFY_FLAGS_MANAGER|MODIFY_FLAGS_POSITION|MODIFY_FLAGS_RESTORE|
      MODIFY_FLAGS_API_ADMIN|MODIFY_FLAGS_API_MANAGER|MODIFY_FLAGS_API_SERVER|MODIFY_FLAGS_API_GATEWAY
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTDeal* deal)=0;
   virtual MTAPIRES  Clear(void)=0;
   virtual LPCWSTR   Print(MTAPISTR& string) const=0;
   //--- deal ticket
   virtual uint64_t  Deal(void) const=0;
   //--- deal ticket in external system (exchange, ECN, etc)
   virtual LPCWSTR   ExternalID(void) const=0;
   virtual MTAPIRES  ExternalID(LPCWSTR id)=0;
   //--- client login
   virtual uint64_t  Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- processed dealer login (0-means auto)
   virtual uint64_t  Dealer(void) const=0;
   virtual MTAPIRES  Dealer(const uint64_t dealer)=0;
   //--- deal order ticket
   virtual uint64_t  Order(void) const=0;
   virtual MTAPIRES  Order(const uint64_t order)=0;
   //--- EnDealAction
   virtual uint32_t  Action(void) const=0;
   virtual MTAPIRES  Action(const uint32_t action)=0;
   //--- EnEntryFlags
   virtual uint32_t  Entry(void) const=0;
   virtual MTAPIRES  Entry(const uint32_t entry)=0;
   //--- price digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- currency digits
   virtual uint32_t  DigitsCurrency(void) const=0;
   virtual MTAPIRES  DigitsCurrency(const uint32_t digits)=0;
   //--- symbol contract size
   virtual double    ContractSize(void) const=0;
   virtual MTAPIRES  ContractSize(const double contract_size)=0;
   //--- deal creation datetime in seconds
   virtual int64_t   Time(void) const=0;
   virtual MTAPIRES  Time(const int64_t time)=0;
   //--- deal symbol
   virtual LPCWSTR   Symbol(void) const=0;
   virtual MTAPIRES  Symbol(LPCWSTR symbol)=0;
   //--- deal price
   virtual double    Price(void) const=0;
   virtual MTAPIRES  Price(const double price)=0;
   //--- deal volume
   virtual uint64_t  Volume(void) const=0;
   virtual MTAPIRES  Volume(const uint64_t volume)=0;
   //--- deal profit
   virtual double    Profit(void) const=0;
   virtual MTAPIRES  Profit(const double profit)=0;
   //--- deal collected swaps
   virtual double    Storage(void) const=0;
   virtual MTAPIRES  Storage(const double storage)=0;
   //--- deal commission
   virtual double    Commission(void) const=0;
   virtual MTAPIRES  Commission(const double comm)=0;
   //--- obsolete value
   virtual double    ObsoleteValue(void) const=0;
   virtual MTAPIRES  ObsoleteValue(const double agent)=0;
   //--- profit conversion rate (from symbol profit currency to deposit currency)
   virtual double    RateProfit(void) const=0;
   virtual MTAPIRES  RateProfit(const double rate)=0;
   //--- margin conversion rate (from symbol margin currency to deposit currency)
   virtual double    RateMargin(void) const=0;
   virtual MTAPIRES  RateMargin(const double rate)=0;
   //--- expert id (filled by expert advisor)
   virtual uint64_t    ExpertID(void) const=0;
   virtual MTAPIRES  ExpertID(const uint64_t id)=0;
   //--- position id
   virtual uint64_t    PositionID(void) const=0;
   virtual MTAPIRES  PositionID(const uint64_t id)=0;
   //--- deal comment
   virtual LPCWSTR   Comment(void) const=0;
   virtual MTAPIRES  Comment(LPCWSTR comment)=0;
   //--- deal internal data for API usage
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataSet(const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataGet(const uint16_t app_id,const uint8_t id,double& value) const=0;
   virtual MTAPIRES  ApiDataClear(const uint16_t app_id)=0;
   virtual MTAPIRES  ApiDataClearAll(void)=0;
   //--- deal profit in symbol's profit currency
   virtual double    ProfitRaw(void) const=0;
   virtual MTAPIRES  ProfitRaw(const double profit)=0;
   //--- closed position  price
   virtual double    PricePosition(void) const=0;
   virtual MTAPIRES  PricePosition(const double price)=0;
   //--- closed volume
   virtual uint64_t    VolumeClosed(void) const=0;
   virtual MTAPIRES  VolumeClosed(const uint64_t volume)=0;
   //--- tick value
   virtual double    TickValue(void) const=0;
   virtual MTAPIRES  TickValue(const double value)=0;
   //--- tick size
   virtual double    TickSize(void) const=0;
   virtual MTAPIRES  TickSize(const double size)=0;
   //--- flags
   virtual uint64_t    Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- deal creation datetime in us since 1970.01.01
   virtual int64_t     TimeMsc(void) const=0;
   virtual MTAPIRES  TimeMsc(const int64_t time)=0;
   //--- EnDealReason
   virtual uint32_t  Reason(void) const=0;
   //--- source gateway name
   virtual LPCWSTR   Gateway(void) const=0;
   //--- deal price on gateway
   virtual double    PriceGateway(void) const=0;
   //--- user record internal data for API usage
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const int64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const uint64_t value)=0;
   virtual MTAPIRES  ApiDataUpdate(const uint32_t pos,const uint16_t app_id,const uint8_t id,const double value)=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,int64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,uint64_t& value) const=0;
   virtual MTAPIRES  ApiDataNext(const uint32_t pos,uint16_t& app_id,uint8_t& id,double& value) const=0;
   //--- deal ticket
   virtual MTAPIRES  DealSet(const uint64_t deal)=0;
   //--- modification flags
   virtual uint32_t  ModificationFlags(void) const=0;
   //--- EnOrderReason
   virtual MTAPIRES  ReasonSet(const uint32_t reason)=0;
   //--- source gateway name
   virtual MTAPIRES  GatewaySet(LPCWSTR gateway)=0;
   //--- deal price on gateway
   virtual MTAPIRES  PriceGatewaySet(double price_gateway)=0;
   //--- order SL
   virtual double    PriceSL(void) const=0;
   virtual MTAPIRES  PriceSL(const double price)=0;
   //--- order TP
   virtual double    PriceTP(void) const=0;
   virtual MTAPIRES  PriceTP(const double price)=0;
   //--- deal volume with extended accuracy
   virtual uint64_t    VolumeExt(void) const=0;
   virtual MTAPIRES  VolumeExt(const uint64_t volume)=0;
   //--- closed volume with extended accuracy
   virtual uint64_t    VolumeClosedExt(void) const=0;
   virtual MTAPIRES  VolumeClosedExt(const uint64_t volume)=0;
   //--- fee
   virtual double    Fee(void) const=0;
   virtual MTAPIRES  Fee(const double fee)=0;
   //--- value
   virtual double    Value(void) const=0;
   virtual MTAPIRES  Value(const double value)=0;
   //--- market prices at time of the deal
   virtual double    MarketBid(void) const=0;
   virtual double    MarketAsk(void) const=0;
   virtual double    MarketLast(void) const=0;
   //--- market prices at time of the deal
   virtual MTAPIRES  MarketBidSet(const double price)=0;
   virtual MTAPIRES  MarketAskSet(const double price)=0;
   virtual MTAPIRES  MarketLastSet(const double price)=0;
   //--- confirmed gateway volume with extended accuracy
   virtual uint64_t  VolumeGatewayExt(void) const=0;
   virtual MTAPIRES  VolumeGatewayExt(const uint64_t volume)=0;
   //--- confirmed gateway deal type
   virtual uint32_t  ActionGateway(void) const=0;
   virtual MTAPIRES  ActionGateway(const uint32_t action)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTDeal(void) {}
  };
//+------------------------------------------------------------------+
//| Deal array interface                                             |
//+------------------------------------------------------------------+
class IMTDealArray
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTDealArray* array)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- add
   virtual MTAPIRES  Add(IMTDeal* deal)=0;
   virtual MTAPIRES  AddCopy(const IMTDeal* deal)=0;
   virtual MTAPIRES  Add(IMTDealArray* array)=0;
   virtual MTAPIRES  AddCopy(const IMTDealArray* array)=0;
   //--- delete & detach
   virtual MTAPIRES  Delete(const uint32_t pos)=0;
   virtual IMTDeal*  Detach(const uint32_t pos)=0;
   //--- update
   virtual MTAPIRES  Update(const uint32_t pos,IMTDeal* deal)=0;
   virtual MTAPIRES  UpdateCopy(const uint32_t pos,const IMTDeal* deal)=0;
   virtual MTAPIRES  Shift(const uint32_t pos,const int32_t shift)=0;
   //--- data access
   virtual uint32_t  Total(void) const=0;
   virtual IMTDeal*  Next(const uint32_t index) const=0;
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
                    ~IMTDealArray(void) {}
  };
//+------------------------------------------------------------------+
//| Deals events notification interface                              |
//+------------------------------------------------------------------+
class IMTDealSink
  {
public:
   virtual void      OnDealAdd(const IMTDeal*    /*deal*/) {  }
   virtual void      OnDealUpdate(const IMTDeal* /*deal*/) {  }
   virtual void      OnDealDelete(const IMTDeal* /*deal*/) {  }
   virtual void      OnDealClean(const uint64_t /*login*/)   {  }
   virtual void      OnDealSync(void)                      {  }
   virtual void      OnDealPerform(const IMTDeal* /*deal*/,IMTAccount* /*account*/,IMTPosition* /*position*/) {  }
   virtual void      OnDealPerformCloseBy(const IMTDeal* /*deal*/,const IMTDeal* /*deal_by*/,IMTAccount* /*account*/,IMTPosition* /*position*/) {  }
  };
//+------------------------------------------------------------------+
