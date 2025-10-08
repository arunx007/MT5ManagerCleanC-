//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Daily Detailed Report                                            |
//+------------------------------------------------------------------+
class CTradeModification : public IMTReportContext
  {
private:
   //--- constants
   enum constants
     {
      EXTRA_DIGITS=3
     };

private:
   //---
   static MTReportInfo s_info;             // report information
   //---
   IMTReportAPI     *m_api;                // api interface
   IMTConReport     *m_report;             // report config interface
   IMTConParam      *m_param;              // report param
   IMTPositionArray *m_positions_modified; // position array interface
   IMTPositionArray *m_positions_user;     // position array interface for one user
   IMTOrderArray    *m_orders_modified;    // order array interface
   IMTOrderArray    *m_orders_user;        // order array interface for one user
   IMTDealArray     *m_deals_modified;     // deal array interface
   IMTDealArray     *m_deals_user;         // deal array interface for one user
   IMTOrderArray    *m_history_modified;   // hsitory order array interface 
   IMTOrderArray    *m_history_user;       // hsitory order array interface for one user

public:
   //--- constructor/destructor
                     CTradeModification(void);
   virtual          ~CTradeModification(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);
private:
   //--- initialize / clear
   bool              Initialize(void);
   void              Clear(void);
   MTAPIRES          CollectModifiedPositions(const uint64_t login);
   MTAPIRES          CollectModifiedOrders(const uint64_t login);
   MTAPIRES          CollectModifiedDeals(const uint64_t login);
   MTAPIRES          CollectModifiedHistory(const uint64_t login);
   //--- write html
   MTAPIRES          WriteHtml(void);
   //--- write functions
   bool              WriteHead(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WriteOrders(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteHistoryOrders(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteDeals(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WritePositions(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
  };
//+------------------------------------------------------------------+
