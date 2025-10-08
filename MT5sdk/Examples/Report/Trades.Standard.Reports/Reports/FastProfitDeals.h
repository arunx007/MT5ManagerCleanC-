//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportGroup.h"
#include "..\Tools\DatasetField.h"
#include "..\Tools\BaseSelect.h"
#include "..\Tools\HistorySelect.h"
#include "..\Tools\GroupCache.h"
//+------------------------------------------------------------------+
//| Daily Postions Report                                            |
//+------------------------------------------------------------------+
class CFastProfitDeals : public CReportGroup
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN          =1,
      COLUMN_NAME           =2,
      COLUMN_DEAL           =3,
      COLUMN_ID             =4,
      COLUMN_ORDER          =5,
      COLUMN_POSITION       =6,
      COLUMN_OPEN_TIME      =7,
      COLUMN_TYPE           =8,
      COLUMN_SYMBOL         =9,
      COLUMN_VOLUME         =10,
      COLUMN_OPEN_PRICE     =11,
      COLUMN_SL             =12,
      COLUMN_TP             =13,
      COLUMN_CLOSE_TIME     =14,
      COLUMN_DURATION       =15,
      COLUMN_CLOSE_VOLUME   =16,
      COLUMN_CLOSE_PRICE    =17,
      COLUMN_REASON         =18,
      COLUMN_COMMISSION     =19,
      COLUMN_FEE            =20,
      COLUMN_SWAP           =21,
      COLUMN_PROFIT         =22,
      COLUMN_DAILY_PROFIT   =23,
      COLUMN_CURRENCY       =24,
      COLUMN_PROFIT_CURRENCY=25,
      COLUMN_PROFIT_SPREADS =26,
      COLUMN_COMMENT        =27,
      COLUMN_DIGITS         =28,
      COLUMN_DIGITS_CURRENCY=29,
     };
   //+------------------------------------------------------------------+
   //| deal brief record                                                |
   //+------------------------------------------------------------------+
   #pragma pack(push,1)
   struct DealBriefRecord
     {
      uint64_t          login;               // owner client login
      uint64_t          position;            // position ticket
      double            profit;              // profit
      uint32_t          digits_currency;     // number of digits of deal currency
     };
   //+------------------------------------------------------------------+
   //| deal record                                                      |
   //+------------------------------------------------------------------+
   struct DealRecord
     {
      uint64_t          login;               // owner client login
      uint64_t          deal;                // deal ticket
      wchar_t           id[32];              // external id
      uint64_t          order;               // order
      uint64_t          position_id;         // deal position ticket
      int64_t           time_create;         // deal create time
      uint32_t          action;              // deal action - EnDealAction
      wchar_t           symbol[32];          // deal symbol
      uint64_t          volume;              // deal volume
      double            price_open;          // deal price
      double            sl;                  // deal SL price
      double            tp;                  // deal TP price
      double            market_bid;          // bid market price
      double            market_ask;          // ask market price
      double            commission;          // commission
      double            commission_fee;      // fee
      double            storage;             // storage
      double            profit;              // profit
      uint32_t          reason;              // deal reason - EnDealReason
      wchar_t           comment[32];         // comment
      uint32_t          digits;              // number of digits of deal symbol
      uint32_t          digits_currency;     // number of digits of deal currency
      //--- check type
      bool              IsBuy (void) const      { return(action==IMTDeal::DEAL_BUY); }
      bool              IsSell(void) const      { return(action==IMTDeal::DEAL_SELL); }
      //--- check balance operation
      bool              IsBalance(void) const   { return(IsBalance(action)); }
      static bool       IsBalance(const uint32_t action);
      //--- check service deal
      inline bool       IsService(void) const;
     };
   //+------------------------------------------------------------------+
   //| position record                                                  |
   //+------------------------------------------------------------------+
   struct PositionRecord
     {
      uint64_t          position_id{};       // position ticket
      uint32_t          type{};              // EnPositionAction
      uint64_t          open_volume{};
      int64_t           open_time{};
      double            open_price{};

                        PositionRecord(void)=default;
                        PositionRecord(const uint64_t id) : position_id{id} {}
      //--- init position history by deal
      inline bool       DealInit(const DealRecord &deal);
      //--- add deal to position history
      inline bool       DealAdd(const DealRecord &deal);
     };
   //--- table record
   struct TableRecord : PositionRecord
     {
      uint64_t          deal{};              // deal
      wchar_t           id[32]{};            // external id
      uint64_t          order{};             // order
      uint64_t          login{};             // login
      wchar_t           name[32]{};          // user name
      wchar_t           currency[32]{};      // currency
      wchar_t           symbol[32]{};        // symbol
      double            sl{};
      double            tp{};
      uint64_t          close_volume{};
      int64_t           close_time{};
      int32_t           duration{};
      double            close_price{};
      uint32_t          reason{};
      double            commission{};
      double            commission_fee{};
      double            storage{};
      double            profit{};
      double            daily_profit{};      // total daily profit of trading account
      double            profit_currency{};   // profit in report's currency
      double            profit_spreads{};    // profit in spreads
      wchar_t           comment[32]{};       // comment
      uint32_t          digits{};            // number of digits of position symbol
      uint32_t          digits_currency{};   // number of digits of position currency

                        TableRecord(void)=default;
                        TableRecord(const uint64_t id) : PositionRecord{id} {}
      //--- fill by deal
      inline void       DealFill(const DealRecord &deal);
     };
   //--- user record
   struct UserRecord
     {
      uint64_t          login;               // login
      wchar_t           name[32];            // name
      wchar_t           group[64];           // group
     };
   #pragma pack(pop)
   //--- position table item
   struct PositionItem
     {
      uint64_t          position_id{};       // position id
      PositionRecord   *record{};            // position record pointer

                        PositionItem(const uint64_t id) : position_id{id} {}
     };
   //--- daily profit of trading account
   struct UserProfit
     {
      uint64_t          login{};             // login
      double            daily_profit{};      // total daily profit of trading account
     };
   //--- table record array type
   typedef TMTArray<TableRecord> TableRecordArray;
   //--- hash functor
   struct PositionItemHash
     {
      uint64_t          operator()(const PositionItem &item) const   { return(operator()(item.position_id)); }
      uint64_t          operator()(const uint64_t id) const            { return(id); }
     };
   //--- comparsion functor
   struct PositionItemEqual
     {
      bool              operator()(const PositionItem &l,const PositionItem &r) const  { return(operator()(l,r.position_id)); }
      bool              operator()(const PositionItem &l,const uint64_t id) const        { return(l.position_id==id); }
     };
   //--- position hash table type
   typedef TMTHashTable<PositionItem,PositionItemHash,PositionItemEqual> PositionRecordTable;
   typedef TMTHashAllocator<sizeof(PositionRecord)> PositionRecordAllocator;
   //--- table record array type
   typedef TMTArray<TableRecord> TableRecordArray;
   //--- hash functor
   struct UserProfitHash
     {
      uint64_t          operator()(const UserProfit &item) const     { return(operator()(item.login)); }
      uint64_t          operator()(const uint64_t login) const         { return(login); }
     };
   //--- comparsion functor
   struct UserProfitEqual
     {
      bool              operator()(const UserProfit &l,const UserProfit &r) const   { return(operator()(l,r.login)); }
      bool              operator()(const UserProfit &l,const uint64_t login) const    { return(l.login==login); }
     };
   //--- daily profit of trading account hash table type
   typedef TMTHashTable<UserProfit,UserProfitHash,UserProfitEqual> UserProfitTable;

private:
   int64_t           m_ctm;                  // report time
   int64_t           m_duration;             // maximal deal duration
   double            m_daily_profit_usd;     // minimal total daily profit, USD
   double            m_profit_usd_lot;       // minimal deal profit, USD/lot
   double            m_profit_spreads;       // minimal deal profit, spreads
   const uint32_t    m_currency_digits;      // report currency digits
   //--- API interfaces
   IMTReportCacheKeySet *m_deal_logins;      // deal logins
   IMTReportCacheKeySet *m_position_ids;     // position ids
   //--- select objects
   CUserSelect       m_user_select;          // user select object
   CDealSelect       m_deal_select;          // deal history select object
   //--- records
   TableRecord       m_record;               // current table record
   UserRecord        m_user;                 // current user record
   UserProfitTable   m_user_profits;         // daily profit of trading account hash table
   PositionRecordTable m_positions;          // history positions
   TableRecordArray  m_summaries;            // summaries
   TableRecord      *m_summary;              // current summary record
   PositionRecordAllocator m_allocator;      // positions allocator
   //---
   static const MTReportInfo s_info;                  // report info
   static const ReportColumn s_columns[];             // column descriptions   
   static const DatasetField s_user_fields[];         // user request fields descriptions
   static const uint32_t s_user_request_limit=100000;     // user request limit
   static const DatasetField s_deal_brief_fields[];   // deal brief request fields descriptions
   static const DatasetField s_deal_fields[];         // deal request fields descriptions
   static const uint64_t s_deal_actions[];              // deal request actions
   static const uint32_t s_deal_request_limit=16384;      // deal request limit

public:
   //--- constructor/destructor
                     CFastProfitDeals(void);
   virtual          ~CFastProfitDeals(void);
   //--- report information
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(void) override;
   //--- prepare requests
   MTAPIRES          PrepareDealRequest(const DatasetField *fields,uint32_t fields_total);
   //--- collect deals brief information
   MTAPIRES          CollectDealsBrief(void);
   //--- write report parts
   MTAPIRES          WriteLogins(void);
   MTAPIRES          WritePositions(IMTDataset &dataset,IMTReportCacheKeySet &logins,CGroupCache &groups);
   MTAPIRES          WritePosition(const TableRecord &record);
   //--- read from dataset
   MTAPIRES          ReadUser(const uint64_t login,IMTDataset &dataset);
   //--- calculate total daily profit
   MTAPIRES          CalcDailyProfit(const DealBriefRecord &deal);
   //--- add record to positions
   MTAPIRES          PositionsAdd(const DealRecord &deal,double rate);
   //--- check fast position
   bool              IsFastPosition(const PositionRecord &position,const DealRecord &deal) const;
   //--- check profit deal
   bool              IsDealProfit(const PositionRecord &position,const DealRecord &deal,double rate,double &profit_spreads) const;
   //--- summary
   TableRecord*      SummaryGet(const TableRecord &record);
   void              SummaryAdd(TableRecord &summary,const TableRecord &record);
   MTAPIRES          SummaryWrite(void);
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

