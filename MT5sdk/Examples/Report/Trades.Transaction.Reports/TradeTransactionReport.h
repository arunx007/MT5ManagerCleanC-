//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "TransactionBase.h"
//+------------------------------------------------------------------+
//| Trade Transaction Report                                         |
//+------------------------------------------------------------------+
class CTradeTransactionReport : public IMTReportContext
  {
private:
   //--- constants
   enum EnConstants
     {
      RATE_DIGITS            =8,   // digits for rate
     };
   //--- column ids
   enum columns
     {
      COLUMN_LOGIN           =1,
      COLUMN_LEVERAGE        =2,
      COLUMN_DEALER          =3,
      COLUMN_IP              =4,
      COLUMN_CURRENCY        =5,
      COLUMN_ORDER           =6,
      COLUMN_ORDER_ID        =7,
      COLUMN_DEAL            =8,
      COLUMN_DEAL_ID         =9,
      COLUMN_POSITION        =10,
      COLUMN_POSITION_ID     =11,
      COLUMN_ACTION          =12,
      COLUMN_TYPE            =13,
      COLUMN_ENTRY           =14,
      COLUMN_SYMBOL          =15,
      COLUMN_POSITION_BY     =16,
      COLUMN_LOTS            =17,
      COLUMN_AMOUNT          =18,
      COLUMN_TIME            =19,
      COLUMN_PRICE           =20,
      COLUMN_BID             =21,
      COLUMN_ASK             =22,
      COLUMN_POSITION_PRICE  =23,
      COLUMN_STOPLOSS        =24,
      COLUMN_TAKEPROFIT      =25,
      COLUMN_MARGIN_RATE     =26,
      COLUMN_MARGIN_AMOUNT   =27,
      COLUMN_COMMISSION      =28,
      COLUMN_SWAP            =29,
      COLUMN_PROFIT          =30,
      COLUMN_PROFIT_CURRENCY =31,
      COLUMN_PROFIT_RATE     =32,
      COLUMN_PROFIT_RAW      =33,
      COLUMN_AMOUNT_CLOSED   =34,
      COLUMN_GATEWAY_PRICE   =35,
      COLUMN_REASON          =36,
      COLUMN_RETCODE         =37,
      COLUMN_GROUP_OWNER     =38,
      COLUMN_DIGITS          =39,
      COLUMN_CURRENCY_DIGITS =40
     };
   //--- table record
   #pragma pack(push,1)
   struct TableRecord
     {
      uint64_t          login;
      uint32_t          leverage;
      wchar_t           ip[32];
      uint64_t          dealer;
      wchar_t           currency[16];
      uint64_t          order;
      wchar_t           order_id[32];
      uint64_t          deal;
      wchar_t           deal_id[32];
      uint64_t          position;
      wchar_t           position_id[32];
      wchar_t           action[32];
      wchar_t           type[32];
      wchar_t           entry[8];
      uint64_t          position_by;
      wchar_t           symbol[32];
      uint64_t          lots;
      double            amount;
      int64_t           timestamp;
      double            price;
      wchar_t           bid[16];
      wchar_t           ask[16];
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
      double            profit_raw;
      double            amount_closed;
      double            price_gateway;
      wchar_t           reason[32];
      wchar_t           retcode[32];
      wchar_t           group_owner[64];
      uint32_t          digits;
      uint32_t          digits_currency;
      //+------------------------------------------------------------------+
      //| Copy information from transaction record                         |
      //+------------------------------------------------------------------+
      void Set(TransactionRecord& record)
        {
         dealer         =record.dealer;
         login          =record.login;
         leverage       =record.leverage;
         order          =record.order;
         deal           =record.deal;
         position       =record.position;
         position_by    =record.position_by;
         lots           =record.lots;
         amount         =record.amount;
         timestamp      =record.timestamp;
         price          =record.price;
         price_position =record.price_position;
         sl             =record.sl;
         tp             =record.tp;
         margin_rate    =record.margin_rate;
         margin_amount  =record.margin_amount;
         commission     =record.commission;
         swap           =record.swap;
         profit         =record.profit;
         profit_rate    =record.profit_rate;
         profit_raw     =record.profit_raw;
         amount_closed  =record.amount_closed;
         price_gateway  =record.price_gateway;
         digits         =record.digits;
         digits_currency=record.digits_currency;
         //---
         CMTStr::Copy(ip             ,record.ip);
         CMTStr::Copy(order_id       ,record.order_id);
         CMTStr::Copy(deal_id        ,record.deal_id);
         CMTStr::Copy(position_id    ,record.position_id);
         CMTStr::Copy(currency       ,record.currency);
         CMTStr::Copy(symbol         ,record.symbol);
         CMTStr::Copy(profit_currency,record.profit_currency);
         CMTStr::Copy(group_owner    ,record.group_owner);
         //---
         CMTStr::Copy(action,PrintAction(record.action));
         CMTStr::Copy(type  ,PrintType(record.type));
         CMTStr::Copy(entry ,PrintEntry(record.entry));
         CMTStr::Copy(reason,PrintReason(record.reason));
         //---
         CMTStr::Copy(retcode,SMTFormat::FormatError(record.retcode));
         //--- bid
         double normalized=SMTMath::PriceNormalize(record.bid,record.digits);
         if(normalized==0)
            bid[0]=L'\0';
         else
            CMTStr::FormatStr(bid,_countof(bid),L"%.*f",record.digits,normalized);
         //--- ask
         normalized=SMTMath::PriceNormalize(record.ask,record.digits);
         if(normalized==0)
            ask[0]=L'\0';
         else
            CMTStr::FormatStr(ask,_countof(ask),L"%.*f",record.digits,normalized);
        }
     };
   #pragma pack(pop)
   //---
   typedef TMTArray<uint64_t> LoginsArray;
private:
   //--- table record
   static MTReportInfo s_info;                  // report information     
   static ReportColumn s_columns[];             // column descriptions   
   //--- api
   IMTReportAPI*     m_api;                     // api interface
   CTransactionBase  m_base;                    // base of transactions
   LoginsArray       m_logins;                  // array of logins
   //--- configs
   CMTStr32          m_symbol_mask;             // symbol mask
   bool              m_show_owner;              // show owner flag
   bool              m_show_daily;              // show daily transactions
   bool              m_show_reason;             // show reason
   bool              m_show_retcode;            // show retcode
   bool              m_show_rejected;           // show rejected transactions

public:
   //--- constructor/destructor
                     CTradeTransactionReport(void);
   virtual          ~CTradeTransactionReport(void);
   //--- get information about report
   static void       Info(MTReportInfo& info) { info=s_info; }
   //--- release plug-in
   virtual void      Release(void)            { delete this; }
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);
   //---
   static LPCWSTR    PrintAction(const uint32_t action);
   static LPCWSTR    PrintType(const uint32_t type);
   static LPCWSTR    PrintEntry(const uint32_t entry);
   static LPCWSTR    PrintReason(const uint32_t reason);

private:
   void              Clear(void);
   //--- table management
   MTAPIRES          TablePrepare(void);
   MTAPIRES          TableWrite(void);
   //--- search functions
   static int32_t    SortLogins(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
