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
class CPositionsHistory : public CReportGroup
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN          =1,
      COLUMN_NAME           =2,
      COLUMN_OPEN_TIME      =3,
      COLUMN_POSITION       =4,
      COLUMN_TYPE           =5,
      COLUMN_VOLUME         =6,
      COLUMN_SYMBOL         =7,
      COLUMN_OPEN_PRICE     =8,
      COLUMN_SL             =9,
      COLUMN_TP             =10,
      COLUMN_CLOSE_TIME     =11,
      COLUMN_CLOSE_PRICE    =12,
      COLUMN_REASON         =13,
      COLUMN_COMMISSION     =14,
      COLUMN_FEE            =15,
      COLUMN_SWAP           =16,
      COLUMN_PROFIT         =17,
      COLUMN_CURRENCY       =18,
      COLUMN_COMMENT        =19,
      COLUMN_CLOSE_VOLUME   =20,
      COLUMN_DIGITS         =21,
      COLUMN_DIGITS_CURRENCY=22,
     };
   //+------------------------------------------------------------------+
   //| deal position record                                             |
   //+------------------------------------------------------------------+
   #pragma pack(push,1)
   struct DealPositionRecord
     {
      uint64_t          login;               // owner client login
      uint64_t          position;            // position ticket
     };
   //+------------------------------------------------------------------+
   //| deal record                                                      |
   //+------------------------------------------------------------------+
   struct DealRecord
     {
      uint64_t          login;               // owner client login
      uint64_t          deal;                // deal ticket
      uint64_t          position_id;         // deal position ticket
      int64_t           time_create;         // deal create time
      uint32_t          entry;               // deal entry - EnDealEntry
      uint32_t          action;              // deal action - EnDealAction
      wchar_t           symbol[32];          // deal symbol
      uint64_t          volume;              // deal volume
      double            price_open;          // deal price
      double            sl;                  // deal SL price
      double            tp;                  // deal TP price
      double            commission;          // commission
      double            commission_fee;      // fee
      double            storage;             // storage
      double            profit;              // profit
      uint32_t          reason;              // deal reason - EnDealReason
      wchar_t           comment[32];         // comment
      uint32_t          digits;              // number of digits of deal symbol
      uint32_t          digits_currency;     // number of digits of deal currency
      //--- check in/out
      bool              IsOut(void) const      { return(entry==IMTDeal::ENTRY_OUT); }
      bool              IsInOut(void) const    { return(entry==IMTDeal::ENTRY_INOUT); }
      //--- check type
      bool              IsBuy (void) const     { return(action==IMTDeal::DEAL_BUY); }
      bool              IsSell(void) const     { return(action==IMTDeal::DEAL_SELL); }
      //--- check balance operation
      bool              IsBalance(void) const { return(IsBalance(action)); }
      static bool       IsBalance(const uint32_t action);
      //--- check service deal
      inline bool       IsService(void) const;
     };
   //+------------------------------------------------------------------+
   //| order record                                                     |
   //+------------------------------------------------------------------+
   struct OrderRecord
     {
      uint64_t          order;               // order ticket
      int64_t           time_setup;          // time of order reception from a client into the system
     };
   //+------------------------------------------------------------------+
   //| position record                                                  |
   //+------------------------------------------------------------------+
   struct PositionRecord
     {
      uint64_t          position_id{};       // position ticket
      wchar_t           symbol[32]{};        // position symbol
      uint32_t          type{};              // EnPositionAction
      uint32_t          open_reason{};
      uint64_t          close_volume{};
      uint64_t          open_volume{};
      int64_t           open_time{};
      double            open_price{};
      double            sl{};
      double            tp{};
      int64_t           close_time{};
      double            close_price{};
      double            commission{};
      double            commission_fee{};
      double            storage{};
      double            profit{};
      wchar_t           comment[32]{};       // comment
      uint32_t          digits{};            // number of digits of position symbol
      uint32_t          digits_currency{};   // number of digits of position currency

                        PositionRecord(void)=default;
                        PositionRecord(const uint64_t id) : position_id{id} {}
      //--- check balance operation
      bool              IsBalance(void) const { return(DealRecord::IsBalance(type)); }
      //--- init position history by deal
      inline bool       DealInit(const DealRecord &deal);
      //--- add deal to position history
      inline void       DealAdd(const DealRecord &deal);
     };
   //--- table record
   struct TableRecord : PositionRecord
     {
      uint64_t          login{};             // login
      wchar_t           name[32]{};          // user name
      wchar_t           currency[32]{};      // currency

                        TableRecord(void)=default;
                        TableRecord(const uint64_t id) : PositionRecord{id} {}
     };
   //--- user record
   struct UserRecord
     {
      uint64_t          login;               // login
      wchar_t           name[32];            // name
      wchar_t           group[64];           // group
     };
   //--- table item
   struct TableItem
     {
      uint64_t          position_id{};       // position id
      TableRecord      *record{};            // table record pointer

                        TableItem(const uint64_t id) : position_id{id} {}
     };
   #pragma pack(pop)
   //--- table record array type
   typedef TMTArray<TableRecord> TableRecordArray;
   //--- positions writer class
   struct            CPositionWriter;
   //--- hash functor
   struct TableItemHash
     {
      uint64_t          operator()(const TableItem &item) const   { return(operator()(item.position_id)); }
      uint64_t          operator()(const uint64_t id) const         { return(id); }
     };
   //--- comparsion functor
   struct TableItemEqual
     {
      bool              operator()(const TableItem &l,const TableItem &r) const  { return(operator()(l,r.position_id)); }
      bool              operator()(const TableItem &l,const uint64_t id) const     { return(l.position_id==id); }
     };
   //--- hash table type
   typedef TMTHashTable<TableItem,TableItemHash,TableItemEqual> TableRecords;
   typedef TMTHashAllocator<sizeof(TableRecord)> TableRecordAllocator;

private:
   int64_t           m_from;                 // report time interval begin
   int64_t           m_to;                   // report time interval end
   int64_t           m_from_orders;          // earliest order time
   //--- API interfaces
   IMTReportCacheKeySet *m_deal_logins;      // deal logins
   IMTReportCacheKeySet *m_position_ids;     // position ids
   //--- select objects
   CUserSelect       m_user_select;          // user select object
   CDealSelect       m_deal_select;          // deal history select object
   CHistorySelect    m_order_select;         // order history select object
   //--- records
   TableRecord       m_record;               // current table record
   UserRecord        m_user;                 // current user record
   TableRecords      m_positions;            // history positions
   TableRecordArray  m_summaries;            // summaries
   TableRecordAllocator m_allocator;         // positions allocator
   //---
   static const MTReportInfo s_info;                  // report info
   static const ReportColumn s_columns[];             // column descriptions   
   static const DatasetField s_user_fields[];         // user request fields descriptions
   static const uint32_t s_user_request_limit=100000;     // user request limit
   static const DatasetField s_order_time_fields[];   // order time request fields descriptions
   static const uint32_t s_order_request_limit=16384;     // order request limit
   static const DatasetField s_deal_position_fields[];// deal position request fields descriptions
   static const DatasetField s_deal_fields[];         // deal request fields descriptions
   static const uint32_t s_deal_request_limit=16384;      // deal request limit

public:
   //--- constructor/destructor
                     CPositionsHistory(void);
   virtual          ~CPositionsHistory(void);
   //--- report information
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(void) override;
   //--- prepare requests
   MTAPIRES          PrepareDealRequest(const DatasetField *fields,uint32_t fields_total);
   MTAPIRES          PrepareOrderRequest(void);
   //--- collect deals logins and positions ids sets
   MTAPIRES          CollectLoginsAndPositionIds(void);
   //--- find earliest order time
   MTAPIRES          FindEarliestOrderTime(void);
   //--- write report parts
   MTAPIRES          WriteLogins(void);
   MTAPIRES          WritePositions(IMTDataset &dataset,IMTReportCacheKeySet &logins,CGroupCache &groups,CPositionWriter &writer);
   //--- read from dataset
   MTAPIRES          ReadUser(const uint64_t login,IMTDataset &dataset);
   //--- add record to positions
   MTAPIRES          PositionsAdd(const DealRecord &deal,CPositionWriter &writer);
   //--- initialize table record
   bool              TableRecordInit(TableRecord &record,const DealRecord &deal,const bool balance) const;
   //--- summary
   void              SummaryAdd(TableRecord &summary,const PositionRecord &position);
   MTAPIRES          SummaryWrite(void);
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

