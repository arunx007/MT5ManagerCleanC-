//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Logins, orders and deals range                                   |
//+------------------------------------------------------------------+
class IMTConServerRange
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServerRange* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- from
   virtual uint64_t  From(void) const=0;
   virtual MTAPIRES  From(const uint64_t from)=0;
   //--- to
   virtual uint64_t  To(void) const=0;
   virtual MTAPIRES  To(const uint64_t to)=0;
   //--- used range
   virtual uint64_t  UsedFrom(void) const=0;
   virtual uint64_t  UsedTo(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConServerRange(void) {}
  };
//+------------------------------------------------------------------+
//| Network addresses range                                          |
//+------------------------------------------------------------------+
class IMTConAddressRange
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConAddressRange* range)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- ip address range from
   virtual LPCWSTR   From(void) const=0;
   virtual MTAPIRES  From(LPCWSTR name)=0;
   //--- ip address range to
   virtual LPCWSTR   To(void) const=0;
   virtual MTAPIRES  To(LPCWSTR value)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConAddressRange(void) {}
  };
//+------------------------------------------------------------------+
//| Server cluster server connection state                           |
//+------------------------------------------------------------------+
class IMTConClusterState
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConClusterState* state)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- server id
   virtual uint64_t  Id(void) const=0;
   //--- connection state
   virtual bool      Connected(void) const=0;
   virtual LPCWSTR   ConnectedAddress(void) const=0;
   virtual int64_t   ConnectedTime(void) const=0;
   //--- connection stats
   virtual int64_t   StatsDay(void) const=0;
   virtual uint32_t  StatsPing(void) const=0;
   virtual uint32_t  StatsPingMin(void) const=0;
   virtual uint32_t  StatsPingMax(void) const=0;
   virtual uint32_t  StatsSpeed(void) const=0;
   virtual uint32_t  StatsSpeedMin(void) const=0;
   virtual uint32_t  StatsSpeedMax(void) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConClusterState(void) {}
  };
//+------------------------------------------------------------------+
//| Server cluster server connection state                           |
//+------------------------------------------------------------------+
class IMTConBackupFolder
  {
public:
   //--- folder flags
   enum EnBackupFlags
     {
      FOLDER_FLAG_NONE      =0x00000000,
      FOLDER_FLAG_SUBFOLDERS=0x00000001,
      //--- enumeration borders
      FOLDER_FLAG_ALL       =FOLDER_FLAG_SUBFOLDERS
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConBackupFolder* state)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- folder
   virtual LPCWSTR   Folder(void) const=0;
   virtual MTAPIRES  Folder(LPCWSTR folder)=0;
   //--- masks
   virtual LPCWSTR   Masks(void) const=0;
   virtual MTAPIRES  Masks(LPCWSTR masks)=0;
   //--- filter
   virtual LPCWSTR   Filter(void) const=0;
   virtual MTAPIRES  Filter(LPCWSTR filter)=0;
   //--- flags
   virtual uint32_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint32_t flags)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConBackupFolder(void) {}
  };
//+------------------------------------------------------------------+
//| Trade and main platform server configuration                     |
//+------------------------------------------------------------------+
class IMTConServerTrade
  {
public:
   //--- demo account allocation mode
   enum EnDemoMode
     {
      DEMO_DISABLED         =0, // demo account allocation disabled 
      DEMO_PROLONG          =1, // prolong demo acounts after reconnect
      DEMO_FIXED            =2, // demo account with fixed period
      //--- enumeration borders
      DEMO_FIRST            =DEMO_DISABLED,
      DEMO_LAST             =DEMO_FIXED
     };
   //--- enumeration borders
   enum EnOvernightMode
     {
      OVERNIGHT_END_DAY     =0, // overnight performed at the end of the trading day
      OVERNIGHT_START_DAY   =1, // overnight performed at the begin of the trading day
      //--- enumeration borders
      OVERNIGHT_FIRST       =OVERNIGHT_END_DAY,
      OVERNIGHT_LAST        =OVERNIGHT_START_DAY
     };
   //--- overmonth mode 
   enum EnOvermonthMode
     {
      OVERMONTH_LAST_DAY    =0, // overmonth performed at the last month day
      OVERMONTH_FIRST_DAY   =1, // overmonth performed at the first month day
      //--- enumeration borders
      OVERMONTH_FIRST       =OVERMONTH_LAST_DAY,
      OVERMONTH_LAST        =OVERMONTH_FIRST_DAY
     };
   //--- overnigh days enumeration
   enum EnOvernightDays
     {
      OVERNIGHT_DAYS_SUN         =0x00000001,
      OVERNIGHT_DAYS_MON         =0x00000002,
      OVERNIGHT_DAYS_TUE         =0x00000004,
      OVERNIGHT_DAYS_WED         =0x00000008,
      OVERNIGHT_DAYS_THU         =0x00000010,
      OVERNIGHT_DAYS_FRI         =0x00000020,
      OVERNIGHT_DAYS_SAT         =0x00000040,
      //--- rollovers schedule
      OVERNIGHT_DAYS_ROLLOVER_SUN=0x00000080,
      OVERNIGHT_DAYS_ROLLOVER_MON=0x00000100,
      OVERNIGHT_DAYS_ROLLOVER_TUE=0x00000200,
      OVERNIGHT_DAYS_ROLLOVER_WED=0x00000400,
      OVERNIGHT_DAYS_ROLLOVER_THU=0x00000800,
      OVERNIGHT_DAYS_ROLLOVER_FRI=0x00001000,
      OVERNIGHT_DAYS_ROLLOVER_SAT=0x00002000,
      //--- enumeration borders
      OVERNIGHT_DAYS_NONE        =0x00000000,
      OVERNIGHT_DAYS_DEFAULT =OVERNIGHT_DAYS_MON | OVERNIGHT_DAYS_TUE | OVERNIGHT_DAYS_WED | OVERNIGHT_DAYS_THU | OVERNIGHT_DAYS_FRI |
      OVERNIGHT_DAYS_ROLLOVER_MON | OVERNIGHT_DAYS_ROLLOVER_TUE | OVERNIGHT_DAYS_ROLLOVER_WED | OVERNIGHT_DAYS_ROLLOVER_THU | OVERNIGHT_DAYS_ROLLOVER_FRI,
      OVERNIGHT_DAYS_ALL     =OVERNIGHT_DAYS_SUN | OVERNIGHT_DAYS_MON | OVERNIGHT_DAYS_TUE | OVERNIGHT_DAYS_WED | OVERNIGHT_DAYS_THU | OVERNIGHT_DAYS_FRI | OVERNIGHT_DAYS_SAT |
      OVERNIGHT_DAYS_ROLLOVER_SUN | OVERNIGHT_DAYS_ROLLOVER_MON | OVERNIGHT_DAYS_ROLLOVER_TUE | OVERNIGHT_DAYS_ROLLOVER_WED | OVERNIGHT_DAYS_ROLLOVER_THU |
      OVERNIGHT_DAYS_ROLLOVER_FRI | OVERNIGHT_DAYS_ROLLOVER_SAT
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServerTrade* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- demo accounts mode EnDemoMode
   virtual uint32_t  DemoMode(void) const=0;
   virtual MTAPIRES  DemoMode(const uint32_t mode)=0;
   //--- demo accounts period
   virtual uint32_t  DemoPeriod(void) const=0;
   virtual MTAPIRES  DemoPeriod(const uint32_t period)=0;
   //--- overnight mode
   virtual uint32_t  OvernightMode(void) const=0;
   virtual MTAPIRES  OvernightMode(const uint32_t mode)=0;
   //--- overnight time (in minutes)
   virtual uint32_t  OvernightTime(void) const=0;
   virtual MTAPIRES  OvernightTime(const uint32_t time)=0;
   virtual int64_t   OvernightTimeLast(void) const=0;
   virtual int64_t   OvernightTimePrev(void) const=0;
   //--- overnight mode
   virtual uint32_t  OvermonthMode(void) const=0;
   virtual MTAPIRES  OvermonthMode(const uint32_t mode)=0;
   //--- overmonth time (in minutes)
   virtual int64_t   OvermonthTimeLast(void) const=0;
   virtual int64_t   OvermonthTimePrev(void) const=0;
   //--- client logins ranges for this trade server
   virtual MTAPIRES  LoginsRangeAdd(IMTConServerRange* range)=0;
   virtual MTAPIRES  LoginsRangeUpdate(const uint32_t pos,const IMTConServerRange* range)=0;
   virtual MTAPIRES  LoginsRangeDelete(const uint32_t pos)=0;
   virtual MTAPIRES  LoginsRangeClear(void)=0;
   virtual MTAPIRES  LoginsRangeShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  LoginsRangeTotal(void) const=0;
   virtual MTAPIRES  LoginsRangeNext(const uint32_t pos,IMTConServerRange* range) const=0;
   //--- orders tickets ranges for this trade server
   virtual MTAPIRES  OrdersRangeAdd(IMTConServerRange* range)=0;
   virtual MTAPIRES  OrdersRangeUpdate(const uint32_t pos,const IMTConServerRange* range)=0;
   virtual MTAPIRES  OrdersRangeDelete(const uint32_t pos)=0;
   virtual MTAPIRES  OrdersRangeClear(void)=0;
   virtual MTAPIRES  OrdersRangeShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  OrdersRangeTotal(void) const=0;
   virtual MTAPIRES  OrdersRangeNext(const uint32_t pos,IMTConServerRange* range) const=0;
   //--- deals tickets ranges for this trade server
   virtual MTAPIRES  DealsRangeAdd(IMTConServerRange* range)=0;
   virtual MTAPIRES  DealsRangeUpdate(const uint32_t pos,const IMTConServerRange* range)=0;
   virtual MTAPIRES  DealsRangeDelete(const uint32_t pos)=0;
   virtual MTAPIRES  DealsRangeClear(void)=0;
   virtual MTAPIRES  DealsRangeShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  DealsRangeTotal(void) const=0;
   virtual MTAPIRES  DealsRangeNext(const uint32_t pos,IMTConServerRange* range) const=0;
   //--- trade totals
   virtual uint32_t  TotalUsers(void) const=0;
   virtual uint32_t  TotalUsersReal(void) const=0;
   virtual uint32_t  TotalDeals(void) const=0;
   virtual uint32_t  TotalOrders(void) const=0;
   virtual uint32_t  TotalOrdersHistory(void) const=0;
   virtual uint32_t  TotalPositions(void) const=0;
   //--- overnight days EnOvernightDays
   virtual uint32_t  OvernightDays(void) const=0;
   virtual MTAPIRES  OvernightDays(const uint32_t days)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConServerTrade(void) {}
  };
//+------------------------------------------------------------------+
//| History server configuration                                     |
//+------------------------------------------------------------------+
class IMTConServerHistory
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServerHistory* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- datafeeds switch timeout
   virtual uint32_t  DatafeedsTimeout(void) const=0;
   virtual MTAPIRES  DatafeedsTimeout(const uint32_t timeout)=0;
   //--- max news buffer
   virtual uint32_t  NewsMax(void) const=0;
   virtual MTAPIRES  NewsMax(const uint32_t news_max)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConServerHistory(void) {}
  };
//+------------------------------------------------------------------+
//| Backup server configuration                                      |
//+------------------------------------------------------------------+
class IMTConServerBackup
  {
public:
   //--- backup flags
   enum EnBackupFlags
     {
      FLAG_ENABLE_BACKUPS =0x00000001,
      FLAG_ENABLE_TICKS   =0x00000002,
      FLAG_ENABLE_FAILOVER=0x00000004,
      FLAG_ENABLE_LOGS    =0x00000008
     };
   //--- backup period
   enum EnBackupPeriod
     {
      BACKUP_DISABLED   =0,
      BACKUP_15MINUTES  =1,
      BACKUP_30MINUTES  =2,
      BACKUP_1HOUR      =3,
      BACKUP_4HOURS     =4,
      BACKUP_1DAY       =5,
      //--- enumeration borders
      BACKUP_FIRST      =BACKUP_DISABLED,
      BACKUP_LAST       =BACKUP_1DAY
     };
   //--- backup copy time to live
   enum EnBackupTTL
     {
      BACKUP_TTL_1DAY   =1,
      BACKUP_TTL_3DAYS  =2,
      BACKUP_TTL_1WEEK  =3,
      BACKUP_TTL_1MONTH =4,
      BACKUP_TTL_3MONTHS=5,
      BACKUP_TTL_6MONTHS=6,
      //--- enumeration borders
      BACKUP_TTL_FIRST  =BACKUP_TTL_1DAY,
      BACKUP_TTL_LAST   =BACKUP_TTL_6MONTHS
     };
   //--- SQL export modes
   enum EnSQLExportMode
     {
      SQL_MODE_NONE      =0,
      SQL_MODE_MSSQL     =1,
      SQL_MODE_OBSOLETE  =2,
      SQL_MODE_MYSQL     =3,
      SQL_MODE_ORACLE    =4,
      SQL_MODE_POSTGRESQL=5,
      //--- enumeration borders
      SQL_MODE_FIRST    =SQL_MODE_NONE,
      SQL_MODE_LAST     =SQL_MODE_POSTGRESQL,
     };
   //--- SQL export flags
   enum EnSQLExportFlags
     {
      SQL_FLAG_NONE      =0x00000000,
      SQL_FLAG_PARTITIONS=0x00000001, // split by years
      SQL_FLAG_SKIP_DEMO =0x00000002, // skip demo accounts
      //--- enumeration borders
      SQL_FLAG_ALL       =SQL_FLAG_PARTITIONS|SQL_FLAG_SKIP_DEMO
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServerBackup* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- master server
   virtual uint64_t  MasterServer(void) const=0;
   virtual MTAPIRES  MasterServer(const uint64_t id)=0;
   //--- backup path
   virtual LPCWSTR   BackupPath(void) const=0;
   virtual MTAPIRES  BackupPath(LPCWSTR path)=0;
   //--- full backup time (minutes)
   virtual uint32_t  BackupFullTime(void) const=0;
   virtual MTAPIRES  BackupFullTime(const uint32_t time)=0;
   //--- backup period - EnBackupPeriod
   virtual uint32_t  BackupPeriod(void) const=0;
   virtual MTAPIRES  BackupPeriod(const uint32_t period)=0;
   //--- backup copy TTL - EnBackupTTL
   virtual uint32_t  BackupTTL(void) const=0;
   virtual MTAPIRES  BackupTTL(const uint32_t period)=0;
   //--- backup flags - EnBackupFlags
   virtual uint64_t  BackupFlags(void) const=0;
   virtual MTAPIRES  BackupFlags(const uint64_t flags)=0;
   //--- sql export mode
   virtual uint32_t  SQLExportMode(void) const=0;
   virtual MTAPIRES  SQLExportMode(const uint32_t mode)=0;
   //--- sql export flags
   virtual uint64_t  SQLExportFlags(void) const=0;
   virtual MTAPIRES  SQLExportFlags(const uint64_t flags)=0;
   //--- sql export open trades refresh period in minutes
   virtual uint32_t  SQLExportPeriod(void) const=0;
   virtual MTAPIRES  SQLExportPeriod(const uint32_t period)=0;
   //--- sql export server
   virtual LPCWSTR   SQLExportServer(void) const=0;
   virtual MTAPIRES  SQLExportServer(LPCWSTR server)=0;
   //--- sql export login
   virtual LPCWSTR   SQLExportLogin(void) const=0;
   virtual MTAPIRES  SQLExportLogin(LPCWSTR login)=0;
   //--- sql export password
   virtual LPCWSTR   SQLExportPassword(void) const=0;
   virtual MTAPIRES  SQLExportPassword(LPCWSTR password)=0;
   //--- sql export folder
   virtual LPCWSTR   SQLExportFolder(void) const=0;
   virtual MTAPIRES  SQLExportFolder(LPCWSTR folder)=0;
   //--- last synchronization with master server
   virtual int64_t   BackupLastSync(void) const=0;
   //--- deals tickets ranges for this trade server
   virtual MTAPIRES  FoldersAdd(IMTConBackupFolder* folder)=0;
   virtual MTAPIRES  FoldersUpdate(const uint32_t pos,const IMTConBackupFolder* folder)=0;
   virtual MTAPIRES  FoldersDelete(const uint32_t pos)=0;
   virtual MTAPIRES  FoldersClear(void)=0;
   virtual MTAPIRES  FoldersShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  FoldersTotal(void) const=0;
   virtual MTAPIRES  FoldersNext(const uint32_t pos,IMTConBackupFolder* folder) const=0;
   //--- last synchronization with SQL
   virtual int64_t   SQLExportLastSync(void) const=0;
   //---  last server startup, last full & archive backup
   virtual int64_t   BackupLastStartup(void) const=0;
   virtual int64_t   BackupLastFull(void) const=0;
   virtual int64_t   BackupLastArchive(void) const=0;
   //--- explicit destructor is prohibited

protected:
                    ~IMTConServerBackup(void) {}
  };
//+------------------------------------------------------------------+
//| Access server configuration                                      |
//+------------------------------------------------------------------+
class IMTConServerAccess
  {
public:
   //--- access mask
   enum EnAccessMask
     {
      ACCESS_ALLOW_CLIENT     =0x00000001,
      ACCESS_ALLOW_MANAGER    =0x00000002,
      ACCESS_ALLOW_ADMIN      =0x00000004,
      ACCESS_ALLOW_RESERVED   =0x00000008,
      ACCESS_ALLOW_MANAGER_API=0x00000010,
      ACCESS_ALLOW_WEB_API    =0x00000020,
      ACCESS_ALLOW_VPS        =0x00000040,
      ACCESS_ALLOW_ULTENCY    =0x00000080,
      //--- enumeration borders
      ACCESS_ALLOW_NONE       =0x00000000,
      ACCESS_ALLOW_ALL        =ACCESS_ALLOW_CLIENT|ACCESS_ALLOW_MANAGER|ACCESS_ALLOW_ADMIN|ACCESS_ALLOW_RESERVED|ACCESS_ALLOW_MANAGER_API|ACCESS_ALLOW_WEB_API|ACCESS_ALLOW_VPS|ACCESS_ALLOW_ULTENCY
     };
   //--- access flags
   enum EnAccessFlags
     {
      ACCESS_FLAGS_INVISIBLE  =0x00000001,
      ACCESS_FLAGS_HIDELOGIN  =0x00000002,
      //--- enumeration borders
      ACCESS_FLAGS_NONE       =0,
      ACCESS_FLAGS_ALL        =ACCESS_FLAGS_INVISIBLE|ACCESS_FLAGS_HIDELOGIN
     };
   //--- EnServerPriority
   enum EnServerPriority
     {
      PRIORITY_HIGHEST        =0,
      PRIORITY_LOWEST         =15,
      PRIORITY_IDLE           =255,
      //---
      PRIORITY_FIRST          =PRIORITY_HIGHEST,
      PRIORITY_LAST           =PRIORITY_IDLE
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServerAccess* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- priority
   virtual uint32_t  Priority(void) const=0;
   virtual MTAPIRES  Priority(const uint32_t priority)=0;
   virtual uint32_t  PriorityCurrent(void) const=0;
   //--- access flags EnAccessMask
   virtual uint32_t  AccessMask(void) const=0;
   virtual MTAPIRES  AccessMask(const uint32_t mask)=0;
   //--- max news buffer
   virtual uint32_t  NewsMax(void) const=0;
   virtual MTAPIRES  NewsMax(const uint32_t news_max)=0;
   //--- antiflood
   virtual uint32_t  AntifloodEnabled(void) const=0;
   virtual MTAPIRES  AntifloodEnabled(const uint32_t enabled)=0;
   //--- antiflood connects criterion
   virtual uint32_t  AntifloodConnects(void) const=0;
   virtual MTAPIRES  AntifloodConnects(const uint32_t connects)=0;
   //--- antiflood connects criterion
   virtual uint32_t  AntifloodErrors(void) const=0;
   virtual MTAPIRES  AntifloodErrors(const uint32_t errors)=0;
   //--- list of public addresses (address:port) available for terminals
   virtual MTAPIRES  PointsAdd(LPCWSTR path)=0;
   virtual MTAPIRES  PointsUpdate(const uint32_t pos,LPCWSTR address)=0;
   virtual MTAPIRES  PointsDelete(const uint32_t pos)=0;
   virtual MTAPIRES  PointsClear(void)=0;
   virtual MTAPIRES  PointsShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  PointsTotal(void) const=0;
   virtual LPCWSTR   PointsNext(const uint32_t pos) const=0;
   //--- list of internal addresses (address:port) for server listening
   virtual MTAPIRES  BindingsAdd(LPCWSTR path)=0;
   virtual MTAPIRES  BindingsUpdate(const uint32_t pos,LPCWSTR address)=0;
   virtual MTAPIRES  BindingsDelete(const uint32_t pos)=0;
   virtual MTAPIRES  BindingsClear(void)=0;
   virtual MTAPIRES  BindingsShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  BindingsTotal(void) const=0;
   virtual LPCWSTR   BindingsNext(const uint32_t pos) const=0;
   //--- list of trade attended trade servers
   virtual MTAPIRES  ServersAdd(const uint64_t server_id)=0;
   virtual MTAPIRES  ServersUpdate(const uint32_t pos,uint64_t server_id)=0;
   virtual MTAPIRES  ServersDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ServersClear(void)=0;
   virtual MTAPIRES  ServersShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ServersTotal(void) const=0;
   virtual uint64_t  ServersNext(const uint32_t pos) const=0;
   //--- access flags EnAccessFlags
   virtual uint32_t  AccessFlags(void) const=0;
   virtual MTAPIRES  AccessFlags(const uint32_t flags)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTConServerAccess(void) {}
  };
//+------------------------------------------------------------------+
//| Ant DDoS server configuration                                     |
//+------------------------------------------------------------------+
class IMTConServerAntiDDoS
  {
public:
   //--- access mask
   enum EnAccessMask
     {
      ACCESS_ALLOW_CLIENT     =1,
      ACCESS_ALLOW_MANAGER    =2,
      ACCESS_ALLOW_ADMIN      =4,
      ACCESS_ALLOW_CLIENT_API =8,
      ACCESS_ALLOW_MANAGER_API=16,
      ACCESS_ALLOW_WEB_API    =32,
      //--- enumeration borders
      ACCESS_ALLOW_NONE       =0,
      ACCESS_ALLOW_ALL        =ACCESS_ALLOW_CLIENT|ACCESS_ALLOW_MANAGER|ACCESS_ALLOW_ADMIN|ACCESS_ALLOW_CLIENT_API|ACCESS_ALLOW_MANAGER_API|ACCESS_ALLOW_WEB_API
     };
   //--- EnServerPriority
   enum EnServerPriority
     {
      PRIORITY_HIGHEST        =0,
      PRIORITY_LOWEST         =15,
      PRIORITY_IDLE           =255,
      //---
      PRIORITY_FIRST          =PRIORITY_HIGHEST,
      PRIORITY_LAST           =PRIORITY_IDLE
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServerAntiDDoS* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- priority
   virtual uint32_t  Priority(void) const=0;
   virtual MTAPIRES  Priority(const uint32_t priority)=0;
   //--- access flags EnAccessMask
   virtual uint32_t  AccessMask(void) const=0;
   virtual MTAPIRES  AccessMask(const uint32_t mask)=0;
   //--- list of public addresses (address:port) available for terminals
   virtual MTAPIRES  PointsAdd(LPCWSTR path)=0;
   virtual MTAPIRES  PointsUpdate(const uint32_t pos,LPCWSTR address)=0;
   virtual MTAPIRES  PointsDelete(const uint32_t pos)=0;
   virtual MTAPIRES  PointsClear(void)=0;
   virtual MTAPIRES  PointsShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  PointsTotal(void) const=0;
   virtual LPCWSTR   PointsNext(const uint32_t pos) const=0;
   //--- list of trade attended trade servers
   virtual MTAPIRES  ServersAdd(const uint64_t server_id)=0;
   virtual MTAPIRES  ServersUpdate(const uint32_t pos,uint64_t server_id)=0;
   virtual MTAPIRES  ServersDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ServersClear(void)=0;
   virtual MTAPIRES  ServersShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  ServersTotal(void) const=0;
   virtual uint64_t  ServersNext(const uint32_t pos) const=0;
   //--- list of source addresses
   virtual MTAPIRES  SourcesAdd(IMTConAddressRange* range)=0;
   virtual MTAPIRES  SourcesUpdate(const uint32_t pos,const IMTConAddressRange* range)=0;
   virtual MTAPIRES  SourcesDelete(const uint32_t pos)=0;
   virtual MTAPIRES  SourcesShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SourcesTotal(void) const=0;
   virtual MTAPIRES  SourcesNext(const uint32_t pos,IMTConAddressRange* access) const=0;
protected:
                    ~IMTConServerAntiDDoS(void) {}
  };
//+------------------------------------------------------------------+
//| Common platform server configuration                             |
//+------------------------------------------------------------------+
class IMTConServer
  {
public:
   //--- server types
   enum EnServerTypes
     {
      NET_MAIN_TRADE_SERVER   =0,
      NET_TRADE_SERVER        =1,
      NET_HISTORY_SERVER      =2,
      NET_ACCESS_SERVER       =3,
      NET_BACKUP_SERVER       =4,
      NET_OBSOLETE_SERVER_1   =5,
      NET_OBSOLETE_SERVER_2   =6,
      NET_ANTIDDOS_SERVER     =7,
      NET_MATCHING_SERVER     =8,
      //--- enumeration borders
      NET_SERVER_FIRST        =NET_MAIN_TRADE_SERVER,
      NET_SERVER_LAST         =NET_MATCHING_SERVER
     };
   //--- failover mode
   enum EnFailoverModes
     {
      FAILOVER_MODE_DISABLED  =0,  // disabled
      FAILOVER_MODE_BY_MOST   =1,  // server is unaccessible for most witness servers
      FAILOVER_MODE_BY_ALL    =2,  // server is unaccessible for all witness servers
      //--- enumeration borders
      FAILOVER_MODE_FIRST     =FAILOVER_MODE_DISABLED,
      FAILOVER_MODE_LAST      =FAILOVER_MODE_BY_ALL
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTConServer* param)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- server type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- server name
   virtual LPCWSTR   Name(void) const=0;
   virtual MTAPIRES  Name(LPCWSTR name)=0;
   //--- address 
   virtual LPCWSTR   Address(void) const=0;
   virtual MTAPIRES  Address(LPCWSTR name)=0;
   //--- server id
   virtual uint64_t  Id(void) const=0;
   virtual MTAPIRES  Id(const uint64_t id)=0;
   //--- password
   virtual MTAPIRES  Password(LPCWSTR password)=0;
   virtual MTAPIRES  PasswordCheck(LPCWSTR password) const=0;
   //--- service time in minutes
   virtual uint32_t  ServiceTime(void) const=0;
   virtual MTAPIRES  ServiceTime(const uint32_t stime)=0;
   //--- list of network adapters
   virtual LPCWSTR   AdaptersCurrent(void) const=0;
   virtual MTAPIRES  AdaptersCurrent(LPCWSTR current)=0;
   virtual uint32_t  AdaptersTotal(void) const=0;
   virtual LPCWSTR   AdaptersNext(const uint32_t pos) const=0;
   //--- list of available ips
   virtual uint32_t  AddressTotal(void) const=0;
   virtual uint32_t  AddressNext(const uint32_t pos) const=0;
   //--- server info
   virtual uint32_t  Version(void) const=0;
   virtual uint32_t  Build(void) const=0;
   virtual LPCWSTR   BuildDate(void) const=0;
   virtual int64_t     LastBootTime(void) const=0;
   virtual bool      Connected(void) const=0;
   virtual LPCWSTR   OS(void) const=0;
   //--- cpu info
   virtual LPCWSTR   CPU(void) const=0;
   virtual uint32_t  CPUTotal(void) const=0;
   virtual uint32_t  CPUUsageMax(void) const=0;
   virtual uint32_t  CPUUsageCritical(void) const=0;
   //--- memory info (in MB)
   virtual uint32_t  MemoryTotal(void) const=0;
   virtual uint32_t  MemoryFree(void) const=0;
   virtual uint32_t  MemoryFreeMin(void) const=0;
   virtual uint32_t  MemoryFreeCritical(void) const=0;
   //--- hdd info (in MB)
   virtual uint32_t  HDDTotal(void) const=0;
   virtual uint32_t  HDDFree(void) const=0;
   virtual uint32_t  HDDFreeCritical(void) const=0;
   virtual uint32_t  HDDFragments(void) const=0;
   virtual uint32_t  HDDFragmentsCritical(void) const=0;
   virtual uint32_t  HDDSpeedRead(void) const=0;
   virtual uint32_t  HDDSpeedReadCritical(void) const=0;
   virtual uint32_t  HDDSpeedWrite(void) const=0;
   virtual uint32_t  HDDSpeedWriteCritical(void) const=0;
   //--- connections info
   virtual uint32_t  ConnectsMax(void) const=0;
   virtual uint32_t  ConnectsCritical(void) const=0;
   //--- network info (Kbyte/s)
   virtual uint32_t  NetworkMax(void) const=0;
   virtual uint32_t  NetworkCritical(void) const=0;
   //--- specific server interfaces
   virtual IMTConServerTrade* TradeServer(void)=0;
   virtual IMTConServerHistory* HistoryServer(void)=0;
   virtual IMTConServerAccess* AccessServer(void)=0;
   virtual IMTConServerBackup* BackupServer(void)=0;
   virtual IMTConServerAntiDDoS* AntiDDoSServer(void)=0;
   virtual void*     ReservedServer1(void)=0;
   virtual void*     ReservedServer2(void)=0;
   virtual void*     ReservedServer3(void)=0;
   virtual void*     ReservedServer4(void)=0;
   //--- list of external addresses (address:port) for connections to server
   virtual MTAPIRES  PointsAdd(LPCWSTR path)=0;
   virtual MTAPIRES  PointsUpdate(const uint32_t pos,LPCWSTR address)=0;
   virtual MTAPIRES  PointsDelete(const uint32_t pos)=0;
   virtual MTAPIRES  PointsClear(void)=0;
   virtual MTAPIRES  PointsShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  PointsTotal(void) const=0;
   virtual LPCWSTR   PointsNext(const uint32_t pos) const=0;
   //--- list of internal addresses (address:port) for server listening
   virtual MTAPIRES  BindingsAdd(LPCWSTR path)=0;
   virtual MTAPIRES  BindingsUpdate(const uint32_t pos,LPCWSTR address)=0;
   virtual MTAPIRES  BindingsDelete(const uint32_t pos)=0;
   virtual MTAPIRES  BindingsClear(void)=0;
   virtual MTAPIRES  BindingsShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  BindingsTotal(void) const=0;
   virtual LPCWSTR   BindingsNext(const uint32_t pos) const=0;
   //--- failover mode
   virtual uint32_t  FailoverMode(void) const=0;
   virtual MTAPIRES  FailoverMode(const uint32_t mode)=0;
   //--- failover timeout
   virtual uint32_t  FailoverTimeout(void) const=0;
   virtual MTAPIRES  FailoverTimeout(const uint32_t timeout)=0;
   //--- cluster servers state
   virtual uint32_t  ClusterStateTotal(void) const=0;
   virtual MTAPIRES  ClusterStateNext(const uint32_t pos,IMTConClusterState* state) const=0;
   virtual MTAPIRES  ClusterStateGet(const uint64_t id,IMTConClusterState* state) const=0;
   //--- address
   virtual LPCWSTR   AddressIPv6(void) const=0;
   virtual MTAPIRES  AddressIPv6(LPCWSTR name)=0;
   //--- list of available ips
   virtual uint32_t  AddressIPv6Total(void) const=0;
   virtual MTAPIRES  AddressIPv6Next(const uint32_t pos,MTAPISTR& address) const=0;

protected:
                    ~IMTConServer(void) {}
  };
//+------------------------------------------------------------------+
//| Time config events notification interface                        |
//+------------------------------------------------------------------+
class IMTConServerSink
  {
public:
   virtual void      OnConServerAdd(const IMTConServer*    /*server*/) {  }
   virtual void      OnConServerUpdate(const IMTConServer* /*server*/) {  }
   virtual void      OnConServerDelete(const IMTConServer* /*server*/) {  }
   virtual void      OnConServerSync(void)                             {  }
  };
//+------------------------------------------------------------------+
