//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\ReportBase.h"
#include "..\Tools\DatasetField.h"
#include "..\Tools\HistorySelect.h"
//+------------------------------------------------------------------+
//| Daily Postions Report                                            |
//+------------------------------------------------------------------+
class CDailyPositionReport : public CReportBase
  {
private:
   //--- column ids
   enum columns
     {
      COLUMN_POSITION       =1,
      COLUMN_ID             =2,
      COLUMN_LOGIN          =3,
      COLUMN_NAME           =4,
      COLUMN_TIME_REPORT    =5,
      COLUMN_TIME_CREATE    =6,
      COLUMN_ACTION         =7,
      COLUMN_SYMBOL         =8,
      COLUMN_VOLUME         =9,
      COLUMN_PRICE_OPEN     =10,
      COLUMN_PRICE_SL       =11,
      COLUMN_PRICE_TP       =12,
      COLUMN_PRICE_CURRENT  =13,
      COLUMN_STORAGE        =14,
      COLUMN_PROFIT         =15,
      COLUMN_CURRENCY       =16,
      COLUMN_REASON         =17,
      COLUMN_COMMENT        =18,
      COLUMN_DIGITS         =19,
      COLUMN_DIGITS_CURRENCY=20
     };
   //--- user record
   #pragma pack(push,1)
   struct UserRecord
     {
      int64_t           datetime;            // data and time
      uint64_t          login;               // login
      wchar_t           name[128];           // name
      wchar_t           currency[32];        // currency
      uint32_t          currency_digits;     // number of digits of currency
     };
   //--- position record
   struct PositionRecord
     {
      uint64_t          position;            // position ticket
      wchar_t           id[32];              // position ticket in external system (exchange, ECN, etc)
      int64_t           time_create;         // position create time
      uint32_t          action;              // EnPositionAction
      wchar_t           symbol[32];          // position symbol
      uint64_t          volume;              // position volume
      double            price_open;          // position open price
      double            price_sl;            // position SL price
      double            price_tp;            // position TP price
      double            price_current;       // position current price
      double            storage;             // position accumulated swaps
      double            profit;              // position floating profit
      uint32_t          reason;              // position reason - EnPositionReason
      wchar_t           comment[32];         // comment
      uint32_t          digits;              // number of digits of position symbol
     };
   //--- table record
   struct TableRecord : UserRecord,PositionRecord
     {
     };
   #pragma pack(pop)
   //--- table record array type
   typedef TMTArray<TableRecord> TableRecordArray;

private:
   int64_t           m_ctm;                  // report time
   IMTReportCacheKeySet *m_report_logins;    // report request logins
   IMTDaily         *m_daily;                // daily report interface
   IMTPosition      *m_daily_position;       // position interface
   CDailySelect      m_daily_select;         // daily select object
   //--- records
   TableRecord       m_record;               // current table record
   UserRecord       &m_user;                 // reference to current user record
   PositionRecord   &m_position;             // reference to current position record
   TableRecordArray  m_summaries;            // summaries
   TableRecord      *m_summary;              // current summary record
   //---
   static const MTReportInfo s_info;               // report info
   static const ReportColumn s_columns[];          // column descriptions   
   static const DatasetField s_daily_fields[];     // daily request fields descriptions
   static const uint32_t s_daily_request_limit=100000; // daily request limit

public:
   //--- constructor/destructor
                     CDailyPositionReport(void);
   virtual          ~CDailyPositionReport(void);
   //--- report information
   static void       Info(MTReportInfo& info);

private:
   //--- base overrides
   virtual void      Clear(void) override;
   virtual MTAPIRES  Prepare(void) override;
   virtual MTAPIRES  Write(void) override;
   //--- write report parts
   MTAPIRES          WriteLogins(void);
   MTAPIRES          WriteUsersPositions(IMTDataset &users,uint64_t &login_last);
   MTAPIRES          WritePositions(void);
   MTAPIRES          WritePosition(void);
   MTAPIRES          FillPosition(void);
   //--- summary
   MTAPIRES          SummaryInit(void);
   void              SummaryAdd(TableRecord &summary);
   MTAPIRES          SummaryWrite(void);
   static int32_t    SortSummary(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

