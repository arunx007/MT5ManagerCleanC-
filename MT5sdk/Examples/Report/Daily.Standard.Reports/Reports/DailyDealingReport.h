//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//---
#include "..\Tools\LogFileReader.h"
//---
#define DEFAULT_PROFIT_TRESHOLD    L"10.0"   // in default currency
#define DEFAULT_TICK_PIPS_TRESHOLD L"10"     // in pips
#define DEFAULT_MAX_LINE           L"50"
//--- 
#define AUTOMANAGER_LOGIN          0
#define AUTOMANAGER_NAME           L"Auto Execution"
//---
#define UNKNOWN_NAME               L"Unknown"
//+------------------------------------------------------------------+
//| Daily Dealing Report                                             |
//+------------------------------------------------------------------+
class CDailyDealingReport : public IMTReportContext
  {
private:
   //--- constants
   enum constants
     {
      MAX_MANAGERS=10,
     };
   //--- manager
   struct Manager
     {
      uint64_t          login;                  // manager
      wchar_t           name[128];              // name
      uint64_t          connections_count;      // connections counter
      uint64_t          requests_count;         // requests counter
      uint64_t          rejects_count;          // rejects counter
      uint64_t          requotes_count;         // requotes counter
      double            profit_currency;        // managers`s profit in currency
      int64_t           profit_pips;            // managers`s profit in pips
     };
   //--- suspect action of dealer
   struct SuspectAction
     {
      uint64_t          login;                  // manager
      double            profit_currency;        // managers`s profit in currency
      int64_t           profit_pips;            // managers`s profit in pips
      wchar_t           details[1024];          // log message
     };
   //--- ticks from dealer
   struct DealerTick
     {
      uint64_t          login;                  // manager
      wchar_t           symbol[32];             // symbol
      uint64_t          bid;                    // bid deviation
      uint64_t          ask;                    // ask deviation
     };
   //--- arrays
   typedef TMTArray<Manager,64> ManagersArray;
   typedef TMTArray<SuspectAction,256> SuspectActionsArray;
   typedef TMTArray<DealerTick,256> TicksArray;
   //---
   IMTReportAPI*     m_api;                     // api interface
   //--- 
   SuspectActionsArray m_suspect_actions;       // suspect operations
   ManagersArray     m_managers;                // managers
   TicksArray        m_ticks;                   // ticks
   //--- param configs
   wchar_t           m_currency[32];            // currency for converting
   wchar_t           m_groups[128];             // groups
   uint64_t          m_pips;                    // threshold pips from dealer`s ticks
   double            m_profit;                  // threshold dealer`s profit
   uint64_t          m_max_line;                // max line for dealer action table
   //--- helper configs
   IMTUser*          m_user;                    // user
   IMTConSymbol*     m_symbol;                  // symbol
   IMTConGroup*      m_group;                   // group
   IMTConGateway*    m_gateway;                 // gateway
   uint32_t          m_digits;                  // currency digits
   //--- charts
   IMTReportChart*   m_managers_chart;          // managers chart
   //---
   CLogFileReader    m_file_reader;             // log file reader
   //--- static data
   static MTReportInfo s_info;                  // report information
   static ReportColumn s_columns_manager[];     // column descriptions total
   static ReportColumn s_columns_action[];      // column description suspect action
   static ReportColumn s_columns_tick[];        // column description dealer tick

public:
   //--- constructor/destructor
                     CDailyDealingReport(void);
   virtual          ~CDailyDealingReport(void);
   //--- get information about report
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- create/release interfaces
   void              Clear(void);
   bool              CreateInterfaces(void);
   //--- get parameters
   MTAPIRES          GetParameters(void);
   //--- processing
   MTAPIRES          LoadInfo(void);
   bool              ParseUserManager(uint64_t& user,uint64_t& manager,const CMTStr& message,bool request=false);
   bool              ParseUser(uint64_t& user,const CMTStr& message);
   bool              ParseActivateOrder(const CMTStr& message,double& profit,double& pips,CMTStr& symbol);
   bool              ParseManager(uint64_t& manager,const CMTStr& message);
   bool              ParseTick(const CMTStr& message,CMTStr& symbol,double& bid,double& ask,int pos);
   bool              ParseDeletedPosition(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos);
   bool              ParseModifyPosition(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos);
   bool              ParseMessage(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos,bool confirm=true);
   bool              ParseMessageAutomate(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos);
   //--- get manager name
   void              GetManagerName(uint64_t login,CMTStr& name);
   //--- get file name
   bool              GetFileName(CMTStr& name,const int64_t day);
   //--- prepare charts
   MTAPIRES          PrepareManagersChart(void);
   //--- write html
   MTAPIRES          WriteAll(void);
   bool              WriteManagers(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteSuspectActions(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteSuspectTicks(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   //--- generate dashboard report
   MTAPIRES          GenerateDashboard(void);
   //--- generate dashboard report
   MTAPIRES          GenerateDashboardManager(void);
   //--- generate dashboard suspect action of dealer
   MTAPIRES          GenerateDashboardAction(void);
   //--- generate dashboard ticks from dealer
   MTAPIRES          GenerateDashboardTick(void);
   //--- add managers chart
   MTAPIRES          AddManagersChart(IMTDataset *data,uint32_t column,uint32_t left);
   //--- sorting
   static int32_t    SortManagerByLogin(const void *left,const void *right);
   static int32_t    SortManagerByRequests(const void *left,const void *right);
   static int32_t    SearchManagerByLogin(const void *left,const void *right);
   static int32_t    SortActionByProfit(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

