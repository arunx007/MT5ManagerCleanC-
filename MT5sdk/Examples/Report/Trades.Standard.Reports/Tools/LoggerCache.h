//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Logger request result class                                      |
//+------------------------------------------------------------------+
class CLoggerResult
  {
private:
   int64_t           m_to;                // request end time
   int64_t           m_to_result;         // request result end time
   MTLogRecord      *m_logs;              // first log record
   uint32_t          m_logs_total;        // log record count

public:
   //--- timeout log record
   static const wchar_t s_timeout_record[];

public:
   //--- constructor/destructor
                     CLoggerResult(void);
                    ~CLoggerResult(void);
   //--- clear contents
   void              Clear(IMTReportAPI &api);
   //--- logger request
   MTAPIRES          Request(IMTReportAPI &api,const int64_t from,const int64_t to,const CMTStr64 &request);
   //--- request result end time
   int64_t           ResultTo(void) const          { return(m_to_result); }
   //--- check for completenes
   bool              Complete(void) const          { return(m_to==m_to_result); }
   //--- log record by index
   const MTLogRecord* Record(const uint32_t pos) const { return(m_logs && pos<m_logs_total ? m_logs+pos : nullptr); }
  };
//+------------------------------------------------------------------+
//| Logger request result class array type                           |
//+------------------------------------------------------------------+
typedef TMTArray<CLoggerResult> CLoggerResultArray;
//+------------------------------------------------------------------+
//| Logger cache class                                               |
//+------------------------------------------------------------------+
class CLoggerCache
  {
private:
   IMTReportAPI     *m_api;               // api interface
   CMTStr64          m_request;           // logger request string
   int64_t           m_ctm_from;          // request begin time
   CLoggerResultArray m_logs;             // request results

public:
   //--- constructor/destructor
   explicit          CLoggerCache(IMTReportAPI *api);
                    ~CLoggerCache(void);
   //--- clear contents
   void              Clear(void);
   //--- add id to request
   bool              Add(const uint64_t id,const int64_t ctm);
   bool              AddPair(const uint64_t id1,const int64_t ctm1,const uint64_t id2,const int64_t ctm2);
   //--- logger request
   MTAPIRES          Request(void);
   //--- write log records to html
   MTAPIRES          HtmlWriteLog(const uint64_t id1,const uint64_t id2=0) const;

private:
   //--- clear results
   void              ClearResult(void);
   //--- add request string
   bool              AddRequest(const CMTStr &request);
   //--- update request time
   void              UpdateTime(const int64_t ctm);
  };
//+------------------------------------------------------------------+
//| Logger cache holder class                                        |
//+------------------------------------------------------------------+
class CLoggerCacheHolder
  {
private:
   IMTReportAPI     *m_api;               // api interface
   CLoggerCache      m_cache;             // logger cache
   uint32_t          m_pos_beg;           // cached item index begin
   uint32_t          m_pos_end;           // cached item index end

public:
   //--- constructor/destructor
   explicit          CLoggerCacheHolder(IMTReportAPI *api);
                    ~CLoggerCacheHolder(void);
   //--- cache array items
   const IMTPosition* CachePositions(const IMTPositionArray &positions,const uint32_t pos,MTAPIRES &res);
   const IMTOrder*   CacheOrders(const IMTOrderArray &orders,const uint32_t pos,MTAPIRES &res);
   const IMTDeal*    CacheDeals(const IMTDealArray &deals,const uint32_t pos,MTAPIRES &res);
   //--- write item log to html
   MTAPIRES          HtmlWriteLogPosition(const IMTPosition &position);
   MTAPIRES          HtmlWriteLogOrder(const IMTOrder &order);
   MTAPIRES          HtmlWriteLogDeal(const IMTDeal &deal);

private:
   //--- check cache for item index
   bool              CheckCache(const uint32_t pos);
   //--- add item to request
   bool              AddPosition(const IMTPosition &position);
   bool              AddOrder(const IMTOrder &order);
   bool              AddDeal(const IMTDeal &deal);
  };
//+------------------------------------------------------------------+
