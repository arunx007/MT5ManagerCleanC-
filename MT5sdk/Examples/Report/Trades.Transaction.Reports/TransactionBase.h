//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Settings of database                                             |
//+------------------------------------------------------------------+
#define TransactionsBaseName          L"TradeTransactions"
#define TransactionsBaseVersion       102
//+------------------------------------------------------------------+
//| Description of a database header                                 |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct DataBaseHeader
  {
   uint32_t          version;
   wchar_t           copyright[64];
   wchar_t           name[32];
   int64_t           timesign;
   char              reserved[236];
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Structure of record                                              |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct TransactionRecord
  {
   //--- trader operations
   enum Actions
     {
      ACTION_MARKET                   =0,        // market operation
      ACTION_MARKET_IN                =0,        // deprecated
      ACTION_MARKET_OUT               =1,        // deprecated
      ACTION_MARKET_INOUT             =2,        // deprecated
      ACTION_POSITION_MODIFY          =3,        // modification of the position
      ACTION_PENDING                  =4,        // pending order
      ACTION_PENDING_MODIFY           =5,        // modification of a pending order
      ACTION_PENDING_CANCEL           =6,        // cancel a pending order
      ACTION_PENDING_EXPIRATION       =7,        // pending order expiration
      ACTION_PENDING_ACTIVATION       =8,        // order activation
      ACTION_SL                       =9,        // stop-loss activation
      ACTION_TP                       =10,       // take-profit activation
      ACTION_ROLLOVER                 =11,       // rollover
      ACTION_DEPOSIT                  =13,       // deposit operation
      ACTION_STOPOUT_ORDER            =14,       // order stop out activation
      ACTION_STOPOUT_POSITION         =15,       // position stop out activation
      ACTION_STOPLIMIT                =16,       // stop limit activation
      ACTION_DAILY_POSITION           =17,       // position at the end of day
      ACTION_DAILY_ORDER              =18,       // order at the end of day
      ACTION_DEALER_MARKET            =19,       // market operation    
      ACTION_DEALER_POS_MODIFY        =20,       // modification of the position
      ACTION_DEALER_PENDING           =21,       // pending order
      ACTION_DEALER_PENDING_MODIFY    =22,       // modification of a pending order
      ACTION_DEALER_PENDING_CANCEL    =23,       // cancel a pending order
      ACTION_DEALER_PENDING_ACTIVATION=24,       // order activation
      ACTION_CLOSE_BY                 =25,       // close by
      ACTION_DEALER_CLOSE_BY          =26,       // dealer close by
      ACTION_VMARGIN                  =27,       // variation margin
     };
   //--- trader operation type
   enum Type
     {
      //--- for deals, orders and position
      TYPE_BUY                        =0,
      TYPE_SELL                       =1,
      //--- for orders only
      TYPE_BUY_LIMIT                  =2,
      TYPE_SELL_LIMIT                 =3,
      TYPE_BUY_STOP                   =4,
      TYPE_SELL_STOP                  =5,
      TYPE_BUY_STOP_LIMIT             =6,
      TYPE_SELL_STOP_LIMIT            =7,
      //--- deposit operations
      TYPE_DEPOSIT                    =8,
      TYPE_CREDIT                     =9,
      TYPE_CHARGE                     =10,
      TYPE_CORRECTION                 =11,
      TYPE_BONUS                      =12,
      TYPE_COMMISSION                 =13,
      TYPE_COMMISSION_DAILY           =14,
      TYPE_COMMISSION_MONTHLY         =15,
      TYPE_AGENT_DAILY                =16,
      TYPE_AGENT_MONTHLY              =17,
      TYPE_INTERESTRATE               =18,
      TYPE_BUY_CANCELED               =19,
      TYPE_SELL_CANCELED              =20,
      TYPE_DIVIDEND                   =21,
      TYPE_DIVIDEND_FRANKED           =22,
      TYPE_TAX                        =23,
      TYPE_AGENT                      =24,
      //--- close by operations
      TYPE_CLOSE_BY                   =25,
      TYPE_SO_COMPENSATION            =26,
      TYPE_SO_COMPENSATION_CREDIT     =27,
     };
   //--- entries to market
   enum Entry
     {
      ENTRY_IN     =0,
      ENTRY_OUT    =1,
      ENTRY_INOUT  =2,
      ENTRY_OUT_BY =3,
      ENTRY_UNKNOWN=UINT_MAX,
     };
   //---
   uint64_t          login;
   uint32_t          leverage;
   wchar_t           ip[32];              // user's ip
   wchar_t           currency[16];        // currency deposit account
   uint64_t          order;               // order number
   uint64_t          deal;                // deal number
   uint32_t          action;
   uint32_t          type;                // type of operation
   wchar_t           symbol[32];
   uint64_t          lots;
   double            amount;
   int64_t           timestamp;
   double            price;               // price of a transaction
   double            bid;
   double            ask;
   double            price_position;
   double            sl;
   double            tp;
   double            margin_rate;
   double            margin_amount;
   double            commission;
   double            swap;
   double            profit;
   wchar_t           profit_currency[32];
   double            profit_rate;
   wchar_t           group_owner[64];
   double            profit_raw;
   double            price_gateway;
   uint32_t          entry;
   double            amount_closed;
   uint32_t          reason;
   uint32_t          retcode;
   uint32_t          digits;
   uint32_t          digits_currency;
   //--- external id
   wchar_t           order_id[32];
   wchar_t           deal_id[32];
   //--- dealer
   uint64_t          dealer;
   //--- position and close by position
   uint64_t          position;
   uint64_t          position_by;
   wchar_t           position_id[32];
   //--- reserved
   int32_t           reserved[51];
   //+------------------------------------------------------------------+
   //| Is empty the structure                                           |
   //+------------------------------------------------------------------+
   bool IsEmpty(void)
     {
      //--- checks
      if(login       || leverage      || order           || deal          || action ||
         type        || lots          || amount          || timestamp     || price  ||
         bid         || ask           || price_position  || sl            || tp     ||
         margin_rate || margin_amount || commission      || swap          || profit ||
         profit_rate || profit_raw    || price_gateway   || amount_closed || reason ||
         retcode     || digits        || digits_currency || dealer        ||
         position    || position_by)
         return(false);
      //--- checks
      if(ip[0] || currency[0] || symbol[0] || profit_currency[0] || group_owner[0] || order_id[0] || deal_id[0] || position_id[0])
         return(false);
      //--- empty
      return(true);
     }
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Data base writer class                                           |
//+------------------------------------------------------------------+
class CTransactionBase
  {
private:
   //--- special type
   typedef TMTArray<const TransactionRecord,128> RecordArray;
   //--- constants
   enum constants
     {
      FILE_STEP_SIZE   =4*MB,
      FILE_DELTA_RESIZE=64*KB,
     };
   //--- server API interface
   IMTServerAPI     *m_api;
   //--- database file
   CMTFile           m_file;
   uint64_t          m_file_size;
   int64_t           m_timestamp;
   CMTSync           m_file_sync;
   //--- cache of records
   RecordArray       m_income;
   CMTSync           m_income_sync;
   HANDLE            m_income_event;
   //--- thread of records processing
   CMTThread         m_thread;
   HANDLE            m_thread_exit;
   HANDLE            m_standby_event;
   //--- 
   CMTStrPath        m_path;

public:
   //--- constructor/destructor
                     CTransactionBase(void);
                    ~CTransactionBase(void);
   //--- initialize
   bool              Initialize(CMTStr& path,IMTServerAPI *api);
   //--- write
   bool              Write(const TransactionRecord& record);
   //--- read
   bool              OpenRead(const int64_t timestamp);
   bool              Next(TransactionRecord& record);
   //--- close
   void              Close(void);
   //--- wait for records processing thread standby
   bool              WaitStandby(const uint32_t timeout);

private:
   bool              OpenWrite(const int64_t timestamp);
   bool              CheckHeader(void);
   bool              WriteHeader(void);
   bool              WriteRecord(const TransactionRecord& record);
   bool              SeekLastRecord(void);
   bool              ConvertBase(const int64_t timestamp);
   void              ConvertBase100(TransactionRecord& record);
   void              ConvertBase101(TransactionRecord& record);
   //--- cache of records
   bool              PushRecord(const TransactionRecord& record);
   void              PopRecords(RecordArray& records);
   //--- thread of records processing
   static uint32_t __stdcall ProcessWrapper(LPVOID param);
   void              Process(void);
   bool              Start(void);
   void              Shutdown(void);
  };
//+------------------------------------------------------------------+
