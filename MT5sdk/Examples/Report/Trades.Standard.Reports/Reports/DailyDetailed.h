//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Daily Detailed Report                                            |
//+------------------------------------------------------------------+
class CDailyDetailed : public IMTReportContext
  {
private:
   //--- constants
   enum constants
     {
      EXTRA_DIGITS=3
     };
   //--- errors type
   enum errors
     {
      DAILY_NOT_FOUND,
      BAD_REQUEST
     };
private:
   //---
   static MTReportInfo s_info;      // report information
   //---
   IMTReportAPI     *m_api;         // api interface
   IMTOrderArray    *m_orders;      // order array interface
   IMTDaily         *m_daily;       // daily interface
   IMTDealArray     *m_deals;       // deal array interface
   IMTConGroup      *m_group;       // group config interface
   IMTConServer     *m_server;     // server
   IMTConReport     *m_report;      // report config interface
   IMTUser          *m_user;        // user interface
   IMTPosition      *m_position;    // position interface
   IMTOrder         *m_order;       // order interface

public:
                     CDailyDetailed(void);
   virtual          ~CDailyDetailed(void);
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
   //--- load info for report
   bool              LoadInfo(MTAPIRES& res);
   //--- write html
   MTAPIRES          WriteHtml(void);
   MTAPIRES          WriteErrorHtml(int32_t type);
   //--- write functions
   bool              WriteClosedOrder(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteClosedDeals(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteOpenPosition(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteOrder(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteHead(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WriteDealsSummary(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WritePositionsSummary(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WriteOrdersDealsSummary(MTAPISTR& tag,MTAPIRES& retcode);
   bool              WriteTotalSummary(MTAPISTR& tag,MTAPIRES& retcode);
  };
//+------------------------------------------------------------------+
