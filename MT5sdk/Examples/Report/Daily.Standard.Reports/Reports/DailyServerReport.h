//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "..\Tools\LogFileReader.h"
//+------------------------------------------------------------------+
//| Daily Server Report                                              |
//+------------------------------------------------------------------+
class CDailyServerReport : public IMTReportContext
  {
private:
   //--- terminal types
   enum EnTerminals
     {
      TERMINAL_ADMIN          =0,
      TERMINAL_MANAGER        =1,
      TERMINAL_CLIENT         =2,
      TERMINAL_IPHONE         =3,
      TERMINAL_ANDROID        =4,
      TERMINAL_MANAGER_API    =5,
      TERMINAL_MANAGER_WEB_API=6,
      //---
      TERMINAL_FIRST          =TERMINAL_ADMIN,
      TERMINAL_LAST           =TERMINAL_MANAGER_WEB_API,
     };
   //--- manager connection
   struct ManagerConnection
     {
      wchar_t           login[4096];   // all managers have connected
      wchar_t           ip[32];        // connection ip
     };
   //--- daily logs
   struct DailyLog
     {
      int64_t           date;          // day of log
      uint32_t          total;         // number of total messages
      uint32_t          critical;      // number of critical only messages
     };
   //--- server message
   struct ServerMessage
     {
      uint64_t          number;         // number of message
      wchar_t           message[1024];  // current message
     };
   //--- build
   struct BuildInfo
     {
      uint32_t          build;          // number of build
      //--- connection counters
      uint32_t          client_number;  // number of clients have connected from terminal
      uint32_t          iphone_number;  // number of clients have connected from iphone
      uint32_t          android_number; // number of clients have connected from android
     };
   //--- user connection
   struct UserConnection
     {
      uint64_t          login;          // user
      uint32_t          last_build;     // connected user` last build 
      uint32_t          last_type;      // last type of terminal of connected user
      uint32_t          total_messages; // total messages of user
     };
   //--- arrays
   typedef TMTArray<ServerMessage,64> MessagesArray;
   typedef TMTArray<DailyLog> DailyLogsArray;
   typedef TMTArray<UserConnection,2048> UsersArray;
   typedef TMTArray<ManagerConnection,64> ManagersArray;
   typedef TMTArray<BuildInfo> BuildsArray;
   //--- dataset records
#pragma pack(push,1)
   //--- daily logs record
   struct DailyLogRecord
     {
      int64_t           date;
      uint32_t          total;
      uint32_t          critical;
     };
   //--- build record
   struct BuildRecord
     {
      uint32_t          build;
      uint32_t          client;
      uint32_t          iphone;
      uint32_t          android;
     };
   //--- user connection record
   struct UserConnectionRecord
     {
      wchar_t           name[128];
      uint32_t          messages;
     };
   //--- user record
   struct UserRecord
     {
      uint64_t          login;
      wchar_t           name[128];
      wchar_t           group[64];
      wchar_t           ip[32];
      wchar_t           country[32];
      wchar_t           email[64];
      uint32_t          messages;
     };
#pragma pack(pop)
   //--- constants
   enum constants
     {
      DAILY_LOGS_PERIOD    =SECONDS_IN_WEEK,  // period for graph
      MAX_SYMBOLS_COMPARING=10,
      MAX_TOP_FLOODERS     =3,
      MAX_TOP_FLOODERS_DASH=10,
     };

private:
   IMTReportAPI*     m_api;                   // api interface
   //--- message processing
   IMTReportChart*   m_message_chart;         // total/critical chart
   DailyLogsArray    m_daily_logs;            // daily logs array
   MessagesArray     m_critical;              // critical messages array
   IMTUser*          m_user;                  // user interface
   IMTConManager*    m_manager;               // manager config
   //--- managers
   ManagersArray     m_managers;              // managers array
   //--- flooders
   uint32_t          m_total;                 // total messages of user
   IMTReportChart*   m_flooders_chart;        // flooder chart
   //--- builds
   UsersArray        m_users;                 // connected users
   IMTReportChart*   m_builds_chart;          // client build chart
   BuildsArray       m_builds;                // builds array
   //---
   CLogFileReader    m_file_reader;           // log file reader
   uint64_t          m_stop_check_counter;    // stop check counter
   //--- static data
   static MTReportInfo s_info;                // report information
   static ReportColumn s_columns_log[];       // column description daily log
   static ReportColumn s_columns_build[];     // column description build
   static ReportColumn s_columns_connection[];// column description user connection
   static ReportColumn s_columns_user[];      // column description user
   static ReportColumn s_columns_message[];   // column description message
   static ReportColumn s_columns_manager[];   // column description manager

public:
   //--- constructor/destructor
                     CDailyServerReport(void);
   virtual          ~CDailyServerReport(void);
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
   //--- check manager rights
   MTAPIRES          CheckRights(void);
   //--- prepare charts
   MTAPIRES          PrepareHistoryChart(void);
   MTAPIRES          PrepareFloodersChart(void);
   MTAPIRES          PrepareBuildsChart(void);
   //--- messages processing
   bool              GetFileName(CMTStr& name,const int64_t day);
   MTAPIRES          LoadInfo(void);
   MTAPIRES          LoadMessages(void);
   MTAPIRES          FillDailyLogs(const int64_t day,DailyLog& daily_logs);
   //--- builds/flooders
   bool              PrepareBuildsFlooders(void);
   //--- login messages
   MTAPIRES          ProcessLoginMessage(const CMTStr& message,const uint64_t& login);
   bool              MessageHasLogin(const CMTStr& message,uint64_t& login);
   //--- write html
   MTAPIRES          WriteAll(void);
   bool              WriteMessages(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   bool              WriteManagers(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter);
   //--- client type
   uint32_t          GetClientType(const LPCWSTR type);
   void              IncrementType(const uint32_t type,BuildInfo& build);
   LPCWSTR           ConnectionStr(const uint32_t type,const bool short_name=false);
   //--- generate dashboard report
   MTAPIRES          GenerateDashboard(void);
   //--- generate dashboard chart history
   MTAPIRES          AddChartHistory(void);
   //--- generate dashboard chart flood
   MTAPIRES          AddChartFlood(void);
   //--- generate dashboard chart build
   MTAPIRES          AddChartBuild(void);
   //--- generate dashboard table message
   MTAPIRES          AddTableMessage(void);
   //--- generate dashboard table manager
   MTAPIRES          AddTableManager(void);
   //--- add chart
   MTAPIRES          AddDashboardChart(IMTDataset *data,LPCWSTR title,bool bar,bool acc);
   //--- check for stopped state
   MTAPIRES          IsStopped(void);
   //--- sorting
   static int32_t    SortByAlphabetical(const void *left,const void *right);
   static int32_t    SearchByAlphabetical(const void *left,const void *right);
   static int32_t    SortByNumber(const void *left,const void *right);
   static int32_t    SortFlooderByCount(const void *left,const void *right);
   static int32_t    SortManagerByIp(const void *left,const void *right);
   static int32_t    SearchManagerByIp(const void *left,const void *right);
   static int32_t    SearchBuild(const void *left,const void *right);
   static int32_t    SortBuild(const void *left,const void *right);
   static int32_t    SortUser(const void *left,const void *right);
   static int32_t    SearchUser(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

