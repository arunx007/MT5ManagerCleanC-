//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Server  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Manager report config interface                                  |
//+------------------------------------------------------------------+
class IMTConManagerReport
  {
public:
   //--- permissions flags
   enum EnPermissionsFlags
     {
      PERMISSION_NONE  =0x00000000,        // none
      PERMISSION_VIEW  =0x00000001,        // permission to view
      PERMISSION_EXPORT=0x00000002,        // permission to export
      //--- enumeration borders
      PERMISSION_ALL   =PERMISSION_VIEW|PERMISSION_EXPORT
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConManagerReport* report)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- report name or mask
   virtual LPCWSTR   Report(void) const=0;
   virtual MTAPIRES  Report(LPCWSTR Report)=0;
   //--- permissions flags
   virtual uint64_t    Permissions(void) const=0;
   virtual MTAPIRES  Permissions(const uint64_t permissions)=0;
   //--- history limit in days
   virtual uint32_t  LimitDays(void) const=0;
   virtual MTAPIRES  LimitDays(const uint32_t limit)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConManagerReport(void) {}
  };
//+------------------------------------------------------------------+
//| Manager access config interface                                  |
//+------------------------------------------------------------------+
class IMTConManagerAccess
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConManagerAccess* access)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- ip address range from
   virtual LPCWSTR   From(void) const=0;
   virtual MTAPIRES  From(LPCWSTR name)=0;
   //--- ip address range to
   virtual LPCWSTR   To(void) const=0;
   virtual MTAPIRES  To(LPCWSTR value)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConManagerAccess(void) {}
  };
//+------------------------------------------------------------------+
//| Manager config interface                                         |
//+------------------------------------------------------------------+
class IMTConManager
  {
public:
   enum EnManagerRights
     {
      RIGHT_ADMIN              =0,       // permission to connect using MetaTrader 5 Administrator
      RIGHT_MANAGER            =1,       // permission to connect using MetaTrader 5 Manager
      //---
      RIGHT_CFG_SERVERS        =10,      // permission to configure network
      RIGHT_CFG_ACCESS         =11,      // permission to configure IP access list
      RIGHT_CFG_TIME           =12,      // permission to configure operation time
      RIGHT_CFG_HOLIDAYS       =13,      // permission to configure holidays
      RIGHT_CFG_HST_SYNC       =14,      // permission to configure history charts synchronization
      RIGHT_CFG_SYMBOLS        =15,      // permission to configure symbols
      RIGHT_CFG_GROUPS         =16,      // permission to configure groups
      RIGHT_CFG_MANAGERS       =17,      // permission to configure managers' permissions
      RIGHT_CFG_DATAFEEDS      =18,      // permission to configure datafeeds
      RIGHT_CFG_REQUESTS       =19,      // permission to configure request routing
      RIGHT_SRV_JOURNALS       =20,      // permission to access server logs
      RIGHT_SRV_REPORTS        =21,      // permission to receive automatic server reports
      RIGHT_CHARTS             =22,      // permission to edit charts
      RIGHT_EMAIL              =23,      // permission to send emails
      RIGHT_ACCOUNTANT         =24,      // permission of accountant (charge/discharge)
      RIGHT_ACC_READ           =25,      // permission to access accounts
      RIGHT_ACC_DETAILS_NAME   =26,      // permission to access account name
      RIGHT_ACC_MANAGER        =27,      // permission to edit accounts
      RIGHT_ACC_ONLINE         =28,      // permission to view currently connected clients
      RIGHT_TRADES_READ        =29,      // permission to access orders and positions
      RIGHT_TRADES_MANAGER     =30,      // permission to edit trade records
      RIGHT_QUOTES             =31,      // permission to throw in quotes
      RIGHT_RISK_MANAGER       =32,      // permission of risk manager
      RIGHT_REPORTS            =33,      // permission to receive reports
      RIGHT_NEWS               =34,      // permission to send news
      RIGHT_CFG_GATEWAYS       =35,      // permission to configure gateways
      RIGHT_CFG_PLUGINS        =36,      // permission to configure plugins
      RIGHT_TRADES_DEALER      =37,      // permission of dealer
      RIGHT_CFG_REPORTS        =38,      // permission to configure reports
      RIGHT_EXPORT             =39,      // permission to export data
      RIGHT_SYMBOL_DETAILS     =40,      // permission to modify spread and execution mode
      RIGHT_TECHSUPPORT        =41,      // permission to access technical support page
      RIGHT_TRADES_SUPERVISOR  =42,      // permission of supervisor
      RIGHT_QUOTES_RAW         =43,      // permission to see raw quotes without spread difference
      RIGHT_MARKET             =44,      // permission to access applications market
      RIGHT_GRP_DETAILS_MARGIN =45,      // permission to edit groups margin
      RIGHT_NOTIFICATIONS      =46,      // permission to send notifications
      RIGHT_ACC_DELETE         =47,      // permission to delete accounts
      RIGHT_TRADES_DELETE      =48,      // permission to delete trades
      RIGHT_CONFIRM_ACTIONS    =49,      // permission to confirm actions by confirmation dialog
      RIGHT_CFG_ECN            =50,      // permission to configure ECN
      RIGHT_GRP_DETAILS_COMMISSION=51,   // permission to edit groups commission
      RIGHT_SUBSCRIPTIONS_VIEW =52,      // permission to view subscriptions
      RIGHT_SUBSCRIPTIONS_EDIT =53,      // permission to edit subscriptions
      RIGHT_CFG_FUNDS          =54,      // permission to configure funds
      RIGHT_CFG_MAILS          =55,      // permission to configure mail servers
      RIGHT_CFG_MESSENGERS     =56,      // permission to configure messengers
      RIGHT_CFG_KYC            =57,      // permission to configure KYC
      RIGHT_CFG_AUTOMATIONS    =58,      // permission to configure automation
      RIGHT_CFG_ALLOCATIONS    =59,      // permission to configure accounts allocation
      RIGHT_CFG_VPS            =60,      // permission to configure VPS
      RIGHT_CFG_PAYMENTS       =61,      // permission to configure payments
      RIGHT_ADMIN_COMPUTER     =62,      // permission to manage cluster computer
      RIGHT_CFG_WEB_SERVICES   =63,      // permission to configure web services
      RIGHT_FINTEZA_ACCESS     =64,      // permission to access Finteza
      RIGHT_FINTEZA_WEBSITES   =65,      // permission to access Finteza websites
      RIGHT_FINTEZA_CAMPAIGNS  =66,      // permission to access Finteza campaigns
      RIGHT_FINTEZA_REPORTS    =67,      // permission to access Finteza reports
      RIGHT_ACC_TECHNICAL      =70,      // permission to technical accounts
      RIGHT_ACC_TECHNICAL_MODIFY=71,     // permission to modify "technical account" option
      RIGHT_PAYMENTS_PROCESS   =72,      // permission to process payments
      RIGHT_CFG_CORPORATES     =73,      // permission to configure corporate links
      RIGHT_CFG_LEVERAGES      =74,      // permission to configure floating leverages
      RIGHT_CFG_STREAMING      =75,      // permission to configure data streaming
      RIGHT_ACC_DETAILS_LOCATION    =76, // permission to access account location
      RIGHT_ACC_DETAILS_ADDRESS     =77, // permission to access account address
      RIGHT_ACC_DETAILS_ID          =78, // permission to access account ID
      RIGHT_ACC_DETAILS_EMAIL       =79, // permission to access account email
      RIGHT_ACC_DETAILS_PHONE       =80, // permission to access account phone
      RIGHT_ACC_DETAILS_GENERAL     =81, // permission to access account general personal data
      RIGHT_CLIENTS_DETAILS_NAME    =82, // permission to access client name
      RIGHT_CLIENTS_DETAILS_LOCATION=83, // permission to access client location
      RIGHT_CLIENTS_DETAILS_ADDRESS =84, // permission to access client address
      RIGHT_CLIENTS_DETAILS_ID      =85, // permission to access client ID
      RIGHT_CLIENTS_DETAILS_EMAIL   =86, // permission to access client email
      RIGHT_CLIENTS_DETAILS_PHONE   =87, // permission to access client phone
      RIGHT_CLIENTS_DETAILS_GENERAL =88, // permission to access client general personal data
      RIGHT_CLIENTS_ACCESS     =96,      // permission to access clients
      RIGHT_CLIENTS_CREATE     =97,      // permission to create clients
      RIGHT_CLIENTS_EDIT       =98,      // permission to edit clients
      RIGHT_CLIENTS_DELETE     =99,      // permission to delete clients
      RIGHT_DOCUMENTS_ACCESS   =100,     // permission to access documents
      RIGHT_DOCUMENTS_CREATE   =101,     // permission to create documents
      RIGHT_DOCUMENTS_EDIT     =102,     // permission to edits documents
      RIGHT_DOCUMENTS_DELETE   =103,     // permission to delete documents
      RIGHT_DOCUMENTS_FILES_ADD=104,     // permission to add files for documents
      RIGHT_DOCUMENTS_FILES_DELETE=105,  // permission to delete files for documents
      RIGHT_COMMENTS_ACCESS    =106,     // permission to access comments
      RIGHT_COMMENTS_CREATE    =107,     // permission to create comments
      RIGHT_COMMENTS_DELETE    =108,     // permission to delete comments
      RIGHT_CLIENTS_KYC        =109,     // permission to initate KYC check
      RIGHT_PAYMENTS_ACCESS    =110,     // permission to access payments
      RIGHT_PAYMENTS_EDIT      =111,     // permission to edit payments
      RIGHT_PAYMENTS_DELETE    =112,     // permission to delete payments
      RIGHT_ULTENCY_ACCESS     =113,     // permission to access Ultency
      RIGHT_ULTENCY_EDIT       =114,     // permission to edit Ultency config
      RIGHT_ULTENCY_DELETE     =115,     // permission to delete Ultency config
      RIGHT_ULTENCY_SERVICEDESK=116,     // permission to Ultency ServiceDesk
      //--- enumeration borders
      RIGHT_FIRST              =RIGHT_ADMIN,
      RIGHT_LAST               =128,
     };
   //--- right flags
   enum EnManagerRightFlags
     {
      RIGHT_FLAGS_DENIED       =0,  // right denied
      RIGHT_FLAGS_GRANTED      =1,  // right granted
      //--- enumeration borders
      RIGHT_FLAGS_NONE         =0,
      RIGHT_FLAGS_ALL          =RIGHT_FLAGS_GRANTED
     };
   //--- data access limitation
   enum EnManagerLimit
     {
      MANAGER_LIMIT_ALL        =0,  // unlimited
      MANAGER_LIMIT_MONTHS_1   =1,  // 1 month
      MANAGER_LIMIT_MONTHS_3   =2,  // 3 months
      MANAGER_LIMIT_MONTHS_6   =3,  // 6 months
      MANAGER_LIMIT_YEAR_1     =4,  // 1 year
      MANAGER_LIMIT_YEAR_2     =5,  // 2 years
      MANAGER_LIMIT_YEAR_3     =6,  // 3 years
      //--- enumeration borders
      MANAGER_LIMIT_FIRST      =MANAGER_LIMIT_ALL,
      MANAGER_LIMIT_LAST       =MANAGER_LIMIT_YEAR_3
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConManager* manager)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- login
   virtual uint64_t    Login(void) const=0;
   virtual MTAPIRES  Login(const uint64_t login)=0;
   //--- internal email mailbox name
   virtual LPCWSTR   Mailbox(void) const=0;
   virtual MTAPIRES  Mailbox(LPCWSTR mailbox)=0;
   //--- trade server id
   virtual uint64_t    Server(void) const=0;
   //--- logs access limit EnManagerLimit
   virtual uint32_t  LimitLogs(void) const=0;
   virtual MTAPIRES  LimitLogs(const uint32_t limit)=0;
   //--- reports access limit EnManagerLimit
   virtual uint32_t  LimitReports(void) const=0;
   virtual MTAPIRES  LimitReports(const uint32_t limit)=0;
   //--- rights
   virtual uint32_t  Right(const uint32_t right) const=0;
   virtual MTAPIRES  Right(const uint32_t right,const uint32_t flags)=0;
   //--- allowed groups list
   virtual MTAPIRES  GroupAdd(LPCWSTR path)=0;
   virtual MTAPIRES  GroupUpdate(const uint32_t pos,LPCWSTR path)=0;
   virtual MTAPIRES  GroupShift(const uint32_t pos,const int32_t shift)=0;
   virtual MTAPIRES  GroupDelete(const uint32_t pos)=0;
   virtual uint32_t  GroupTotal(void) const=0;
   virtual LPCWSTR   GroupNext(const uint32_t pos) const=0;
   //--- allowed addressed list
   virtual MTAPIRES  AccessAdd(IMTConManagerAccess* access)=0;
   virtual MTAPIRES  AccessUpdate(const uint32_t pos,const IMTConManagerAccess* access)=0;
   virtual MTAPIRES  AccessDelete(const uint32_t pos)=0;
   virtual MTAPIRES  AccessShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  AccessTotal(void) const=0;
   virtual MTAPIRES  AccessNext(const uint32_t pos,IMTConManagerAccess* access) const=0;
   //--- allowed reports list
   virtual MTAPIRES  ReportAdd(IMTConManagerReport* report)=0;
   virtual MTAPIRES  ReportUpdate(const uint32_t pos,const IMTConManagerReport* report)=0;
   virtual MTAPIRES  ReportDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ReportShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ReportTotal(void) const=0;
   virtual MTAPIRES  ReportNext(const uint32_t pos,IMTConManagerReport* report) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConManager(void) {}
  };
//+------------------------------------------------------------------+
//| Managers config events notification interface                    |
//+------------------------------------------------------------------+
class IMTConManagerSink
  {
public:
   virtual void      OnManagerAdd(const IMTConManager*    /*config*/) {  }
   virtual void      OnManagerUpdate(const IMTConManager* /*config*/) {  }
   virtual void      OnManagerDelete(const IMTConManager* /*config*/) {  }
   virtual void      OnManagerSync(void)                              {  }
   //--- config modification hooks (only for Server API)
   virtual MTAPIRES  HookManagerAdd(const uint64_t /*login*/,IMTConManager* /*new_cfg*/)                                 { return(MT_RET_OK); }
   virtual MTAPIRES  HookManagerUpdate(const uint64_t /*login*/,const IMTConManager* /*cfg*/,IMTConManager* /*new_cfg*/) { return(MT_RET_OK); }
   virtual MTAPIRES  HookManagerDelete(const uint64_t /*login*/,const IMTConManager* /*cfg*/)                            { return(MT_RET_OK); }
  };
//+------------------------------------------------------------------+
