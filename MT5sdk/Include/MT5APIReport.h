//+------------------------------------------------------------------+
//|                                         MetaTrader 5 API Report  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <cstdint>
#include "MT5APIConstants.h"
#include "MT5APILogger.h"
#include "MT5APITools.h"
#include "Config\MT5APIConfigReport.h"
#include "Config\MT5APIConfigCommon.h"
#include "Config\MT5APIConfigTime.h"
#include "Config\MT5APIConfigHoliday.h"
#include "Config\MT5APIConfigSymbol.h"
#include "Config\MT5APIConfigGroup.h"
#include "Config\MT5APIConfigManager.h"
#include "Config\MT5APIConfigNetwork.h"
#include "Config\MT5APIConfigPlugin.h"
#include "Config\MT5APIConfigFeeder.h"
#include "Config\MT5APIConfigGateway.h"
#include "Config\MT5APIConfigFund.h"
#include "Config\MT5APIConfigSubscription.h"
#include "Config\MT5APIConfigParam.h"
#include "Config\MT5APIConfigLeverage.h"
#include "Config\MT5APIConfigUltLiquidity.h"
#include "Config\MT5APIConfigUltSymbol.h"
#include "Config\MT5APIConfigUltTranslate.h"
#include "Bases\MT5APIUser.h"
#include "Bases\MT5APIAccount.h"
#include "Bases\MT5APIDeal.h"
#include "Bases\MT5APIOrder.h"
#include "Bases\MT5APIPosition.h"
#include "Bases\MT5APIDaily.h"
#include "Bases\MT5APITick.h"
#include "Bases\MT5APIChart.h"
#include "Bases\MT5APIDataset.h"
#include "Bases\MT5APIClient.h"
#include "Bases\MT5APISubscription.h"
#include "Bases\MT5APIGeo.h"
#include "Bases\MT5APIUltDeal.h"
#include "Bases\MT5APIUltLiquidityOrder.h"
#include "Bases\MT5APIUltMatchingOrder.h"
//+------------------------------------------------------------------+
//| Report API version                                               |
//+------------------------------------------------------------------+
#define MTReportAPIVersion             5200
#define MTReportAPIDate                L"1 Aug 2025"

//--- PVS Studio helper
//V_FORMATTED_IO_FUNC, class:IMTReportDashboardHtml, function:Write,     format_arg:1, ellipsis_arg:2, propagate_on_virtual
//V_FORMATTED_IO_FUNC, class:IMTReportAPI,           function:LoggerOut, format_arg:2, ellipsis_arg:3, propagate_on_virtual
//V_FORMATTED_IO_FUNC, class:IMTReportAPI,           function:HtmlWrite, format_arg:1, ellipsis_arg:2, propagate_on_virtual

//+------------------------------------------------------------------+
//| Standard report parameter names                                  |
//+------------------------------------------------------------------+
#define MTAPI_PARAM_GROUPS              L"Groups"         // Groups list
#define MTAPI_PARAM_SYMBOLS             L"Symbols"        // Symbols list
#define MTAPI_PARAM_FROM                L"From"           // From
#define MTAPI_PARAM_TO                  L"To"             // To
#define MTAPI_PARAM_LEAD_SOURCE         L"Lead Source"    // Lead Source
#define MTAPI_PARAM_LEAD_CAMPAIGN       L"Lead Campaign"  // Lead Campaign
#define MTAPI_PARAM_IE_VERSION          L"IE_Version"     // IE version
#define MTAPI_PARAM_SCR_WIDTH           L"Screen_Width"   // Screen width
#define MTAPI_PARAM_SCR_HEIGHT          L"Screen_Height"  // Screen height
//+------------------------------------------------------------------+
//| Standard report colors of accounts                               |
//+------------------------------------------------------------------+
#define MTAPI_COLOR_ACOUNTS_REAL        0xCC4748          // Color of real accounts
#define MTAPI_COLOR_ACOUNTS_PRELIMINARY 0x84B761          // Color of preliminary accounts
#define MTAPI_COLOR_ACOUNTS_DEMO        0xB7B83F          // Color of demo accounts
#define MTAPI_COLOR_ACOUNTS_CONTEST     0x67B7DC          // Color of contest accounts
#define MTAPI_COLOR_ACOUNTS_COVERAGE    0xF9A825          // Color of coverage accounts
#define MTAPI_COLOR_ACOUNTS_MANAGER     0xB9783F          // Color of manager accounts
#define MTAPI_COLOR_ACOUNTS_OTHER       0x607D8B          // Color of other accounts
//+------------------------------------------------------------------+
//| Standard report colors of profit and loss                        |
//+------------------------------------------------------------------+
#define MTAPI_COLOR_PROFIT              0x84B761          // Color of profit
#define MTAPI_COLOR_LOSS                0xCC4748          // Color of loss
#define MTAPI_COLOR_NET_PROFIT          0x67B7DC          // Color of net profit
//+------------------------------------------------------------------+
//| Standard report colors of terminal types                         |
//+------------------------------------------------------------------+
#define MTAPI_COLOR_TERMINAL_CLIENT     0x67B7DC          // Color of Client terminal
#define MTAPI_COLOR_TERMINAL_IPHONE     0x84B761          // Color of IPhone terminal
#define MTAPI_COLOR_TERMINAL_ANDROID    0xFDD400          // Color of Android terminal
//+------------------------------------------------------------------+
//| Common colors to draw charts and graphs                          |
//+------------------------------------------------------------------+
static uint32_t ExtChartColors[]= {
   0x2196F3,
   0x2F4074,
   0xFDD400,
   0xCD82AD,
   0xCC4748,
   0x967BDC,
   0xB7B83F,
   0x67B7DC,
   0x84B761,
   0x607D8B,
   0xF9A825
   };
//+------------------------------------------------------------------+
//| Structures                                                       |
//+------------------------------------------------------------------+
//+------------------------------------------------------------------+
//| Report parameter structure                                       |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTReportParam
  {
   //--- parameter types
   enum ParamType
     {
      TYPE_STRING    =0,                              // string
      TYPE_INT       =1,                              // integer
      TYPE_FLOAT     =2,                              // floating
      TYPE_TIME      =3,                              // time only
      TYPE_DATE      =4,                              // date only
      TYPE_DATETIME  =5,                              // date & time
      TYPE_GROUPS    =6,                              // groups list
      TYPE_SYMBOLS   =7,                              // symbols list
      TYPE_BOOL      =8,                              // boolean value
      //--- enumeration borders
      TYPE_FIRST     =TYPE_STRING,
      TYPE_LAST      =TYPE_BOOL
     };
   //---
   uint32_t          type;                            // type
   wchar_t           name[64];                        // name
   wchar_t           defvalue[256];                   // default value
   uint32_t          reserved[16];                    // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Report about structure                                           |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTReportInfo
  {
   //--- snapshot mode flags
   enum EnSnapshots
     {
      SNAPSHOT_NONE          =0x0,                    // without snapshots
      SNAPSHOT_USERS         =0x1,                    // users database snapshot for request
      SNAPSHOT_USERS_FULL    =0x2,                    // full users database snapshot
      SNAPSHOT_ACCOUNTS      =0x4,                    // trade account states snapshot
      SNAPSHOT_ACCOUNTS_FULL =0x8,                    // trade account states snapshot for request
      SNAPSHOT_ORDERS        =0x10,                   // orders database snapshot
      SNAPSHOT_ORDERS_FULL   =0x20,                   // orders database snapshot for request
      SNAPSHOT_POSITIONS     =0x40,                   // positions database snapshot
      SNAPSHOT_POSITIONS_FULL=0x80,                   // positions database snapshot for request
      SNAPSHOT_NO_GROUPS_LOGINS=0x100,                // not include logins from groups request parameter
      //--- enumeration borders
      SNAPSHOT_ALL           =SNAPSHOT_USERS|SNAPSHOT_ACCOUNTS|SNAPSHOT_ORDERS|SNAPSHOT_POSITIONS,
      SNAPSHOT_ALL_FULL      =SNAPSHOT_USERS_FULL|SNAPSHOT_ACCOUNTS_FULL|SNAPSHOT_ORDERS_FULL|SNAPSHOT_POSITIONS_FULL,
     };
   //--- types reports
   enum EnTypes
     {
      TYPE_NONE              =0x0,                    // no support
      TYPE_HTML              =0x1,                    // hypertext
      TYPE_TABLE             =0x2,                    // binary table
      TYPE_DASHBOARD         =0x4,                    // dashboard
      //--- enumeration borders
      TYPE_ALL               =TYPE_HTML|TYPE_TABLE|TYPE_DASHBOARD
     };
   //--- minimal IE version
   enum EnIEVersion
     {
      IE_VERSION_ANY         =0x0000,                 // any IE version
      IE_VERSION_9           =0x0900,                 // IE 9 required
      //--- enumeration borders
      IE_VERSION_FIRST       =IE_VERSION_ANY,
      IE_VERSION_LAST        =IE_VERSION_9,
     };
   //--- information
   uint32_t          version;                         // report version
   uint32_t          version_api;                     // report API version
   uint32_t          version_ie;                      // minimal IE version
   wchar_t           name[64];                        // report name
   wchar_t           copyright[128];                  // copyright
   wchar_t           description[256];                // description
   uint32_t          snapshots;                       // snapshot modes
   uint32_t          types;                           // report types
   wchar_t           category[64];                    // report category
   MTReportParam     params[64];                      // request parameters
   uint32_t          params_total;                    // request parameters total
   MTReportParam     config[64];                      // config parameters
   uint32_t          config_total;                    // config parameters total
   uint32_t          reserved[64];                    // reserved
  };
#pragma pack(pop)
//+------------------------------------------------------------------+
//| Server description structure                                     |
//+------------------------------------------------------------------+
#pragma pack(push,1)
struct MTReportServerInfo
  {
   wchar_t           platform_name[64];               // platform name
   wchar_t           platform_owner[128];             // platform owner
   uint32_t          server_version;                  // server version
   uint32_t          server_build;                    // server build
   uint32_t          server_type;                     // server type
   uint64_t          server_id;                       // server id
   uint32_t          reserved[32];                    // reserved
  };
#pragma pack(pop)

//+------------------------------------------------------------------+
//| Report chart series interface                                    |
//+------------------------------------------------------------------+
class IMTReportSeries
  {
public:
   //--- series type
   enum EnSeriesType
     {
      TYPE_TITLE              =0,                     // title
      //---
      TYPE_LINE               =100,                   // line
      TYPE_HISTOGRAM          =101,                   // histogram
      TYPE_BAR                =102,                   // bar
      TYPE_AREA               =103,                   // area
      TYPE_PIECE              =104,                   // piece for pie
      //--- enumeration borders
      TYPE_FIRST              =TYPE_TITLE,
      TYPE_LAST               =TYPE_PIECE
     };
   //--- flags
   enum EnSeriesFlags
     {
      FLAG_NONE               =0x0000000,
      FLAG_SHOW_VALUES        =0x0000001,                     // show series values on chart
      //--- enumeration borders
      FLAG_ALL                =FLAG_SHOW_VALUES
     };

public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTReportSeries *series)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- title
   virtual LPCWSTR   Title(void) const=0;
   virtual MTAPIRES  Title(LPCWSTR title)=0;
   //--- type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- flags IMTReportSeries::EnSeriesFlags
   virtual uint64_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- color
   virtual uint32_t  Color(void) const=0;
   virtual MTAPIRES  Color(const uint32_t color)=0;
   //--- tooltip format
   virtual LPCWSTR   Tooltip(void) const=0;
   virtual MTAPIRES  Tooltip(LPCWSTR tooltip)=0;
   //--- values management
   virtual MTAPIRES  ValueClear(void)=0;
   virtual uint32_t  ValueTotal(void) const=0;
   virtual MTAPIRES  ValueAdd(LPCWSTR value)=0;
   virtual MTAPIRES  ValueAddInt(const int64_t value)=0;
   virtual MTAPIRES  ValueAddDouble(const double value)=0;
   virtual MTAPIRES  ValueUpdate(const uint32_t pos,LPCWSTR value)=0;
   virtual MTAPIRES  ValueUpdateInt(const uint32_t pos,const int64_t value)=0;
   virtual MTAPIRES  ValueUpdateDouble(const uint32_t pos,const double value)=0;
   virtual LPCWSTR   ValueNext(const uint32_t pos) const=0;
   virtual MTAPIRES  ValueNextInt(const uint32_t pos,int64_t& value) const=0;
   virtual MTAPIRES  ValueNextDouble(const uint32_t pos,double& value) const=0;
   virtual MTAPIRES  ValueShift(const uint32_t pos,const int32_t shift)=0;
   virtual MTAPIRES  ValueDelete(const uint32_t pos)=0;
   virtual MTAPIRES  ValueDescription(const uint32_t pos,LPCWSTR descr)=0;
   virtual LPCWSTR   ValueDescription(const uint32_t pos) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportSeries(void) {}
  };
//+------------------------------------------------------------------+
//| Report chart interface                                           |
//+------------------------------------------------------------------+
class IMTReportChart
  {
public:
   //--- chart type
   enum EnChartType
     {
      TYPE_GRAPH              =0,                     // classical chart
      TYPE_GRAPH_ACCUMULATION =1,                     // classical chart with accumulation
      TYPE_GRAPH_NORMALIZED   =2,                     // classical chart with normalization
      TYPE_GRAPH_STACKED      =3,                     // classical chart with stack
      //---
      TYPE_BAR                =100,                   // line bar chart
      TYPE_BAR_ACCUMULATION   =101,                   // line bar chart with accumulation
      TYPE_BAR_NORMALIZED     =102,                   // line bar chart with normalization
      TYPE_BAR_STACKED        =103,                   // line bar chart with stack
      //---
      TYPE_PIE                =200,                   // pie chart
      //--- enumeration borders
      TYPE_FIRST              =TYPE_GRAPH,
      TYPE_LAST               =TYPE_PIE
     };
   //--- flags
   enum EnChartFlags
     {
      FLAG_NONE               =0x00000000,
      FLAG_ACCUMULATED_VALUES =0x00000001,             // show accumulated values
      FLAG_SHOW_TABLE         =0x00000002,             // show value table
      //--- enumeration borders
      FLAG_ALL                =FLAG_ACCUMULATED_VALUES|FLAG_SHOW_TABLE
     };
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTReportChart *chart)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- title
   virtual LPCWSTR   Title(void) const=0;
   virtual MTAPIRES  Title(LPCWSTR title)=0;
   //--- type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- digits
   virtual uint32_t  Digits(void) const=0;
   virtual MTAPIRES  Digits(const uint32_t digits)=0;
   //--- flags
   virtual uint64_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- bar height
   virtual uint32_t  BarHeight(void) const=0;
   virtual MTAPIRES  BarHeight(const uint32_t height)=0;
   //--- piece tooltip format
   virtual LPCWSTR   PieceTooltip(void)=0;
   virtual MTAPIRES  PieceTooltip(LPCWSTR tooltip)=0;
   //--- piece description format
   virtual LPCWSTR   PieceDescription(void)=0;
   virtual MTAPIRES  PieceDescription(LPCWSTR description)=0;
   //--- series management
   virtual MTAPIRES  SeriesClear(void)=0;
   virtual MTAPIRES  SeriesAdd(IMTReportSeries *series)=0;
   virtual MTAPIRES  SeriesAddCopy(const IMTReportSeries *series)=0;
   virtual MTAPIRES  SeriesDelete(const uint32_t pos)=0;
   virtual IMTReportSeries* SeriesDetach(const uint32_t pos)=0;
   virtual MTAPIRES  SeriesUpdate(const uint32_t pos,IMTReportSeries* series)=0;
   virtual MTAPIRES  SeriesShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  SeriesTotal(void) const=0;
   virtual IMTReportSeries* SeriesNext(const uint32_t pos)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportChart(void) {}
  };
//+------------------------------------------------------------------+
//| Report HTML interface                                            |
//+------------------------------------------------------------------+
class IMTReportDashboardHtml
  {
public:
   //--- common methods
   virtual MTAPIRES  Assign(const IMTReportDashboardHtml *html)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- HTML management
   virtual MTAPIRES  Write(LPCWSTR format,...)=0;
   virtual MTAPIRES  WriteSafe(LPCWSTR html,const uint32_t flags)=0;
   virtual MTAPIRES  WriteChart(const IMTReportChart *chart)=0;
   virtual MTAPIRES  WriteString(LPCWSTR html)=0;
   virtual MTAPIRES  WriteReserved2(void)=0;
   //--- html template processing
   virtual MTAPIRES  TplLoad(LPCWSTR templstr)=0;
   virtual MTAPIRES  TplLoadFile(LPCWSTR templname)=0;
   virtual MTAPIRES  TplLoadResource(const uint32_t resid,LPCWSTR restype)=0;
   virtual MTAPIRES  TplNext(MTAPISTR& tag,uint32_t *counter)=0;
   virtual MTAPIRES  TplProcess(const uint32_t flags)=0;
   virtual MTAPIRES  TplReserved1(void)=0;
   virtual MTAPIRES  TplReserved2(void)=0;
  };
//+------------------------------------------------------------------+
//| Report widget interface                                          |
//+------------------------------------------------------------------+
class IMTReportDashboardWidget
  {
public:
   //--- chart type
   enum EnWidgetType
     {
      //--- charts
      WIDGET_TYPE_CHART_BAR    =0,                    // bar chart
      WIDGET_TYPE_CHART_LINE   =1,                    // line chart
      WIDGET_TYPE_CHART_AREA   =2,                    // area chart
      WIDGET_TYPE_CHART_PIE    =3,                    // pie chart
      WIDGET_TYPE_CHART_SPLINE =4,                    // spline chart
      WIDGET_TYPE_CHART_AREA_SPLINE=5,                // area spline chart
      WIDGET_TYPE_CHART_GEO    =6,                    // geo chart
      //--- 
      WIDGET_TYPE_VALUE        =100,                  // single value
      WIDGET_TYPE_TABLE        =101,                  // table
      WIDGET_TYPE_HTML         =102,                  // HTML
      //--- enumeration borders
      WIDGET_TYPE_FIRST        =WIDGET_TYPE_CHART_BAR,
      WIDGET_TYPE_LAST         =WIDGET_TYPE_HTML
     };
   //--- chart stack type
   enum EnChartStackType
     {
      CHART_STACK_NONE         =0,                    // no stack chart
      CHART_STACK_SIMPLE       =1,                    // simple stack chart
      CHART_STACK_ACCUMULATION =2,                    // accumulation stack chart
      CHART_STACK_NORMALIZED   =3,                    // normalized stack chart
      //--- enumeration borders
      CHART_STACK_FIRST        =CHART_STACK_NONE,
      CHART_STACK_LAST         =CHART_STACK_NORMALIZED
     };
   //--- chart value axis type
   enum EnChartValueAxis
     {
      CHART_VALUE_AXIS_ABSOLUTE=0,                    // absolute value axis
      CHART_VALUE_AXIS_RELATIVE=1,                    // relative value axis
      //--- enumeration borders
      CHART_VALUE_AXIS_FIRST   =CHART_VALUE_AXIS_ABSOLUTE,
      CHART_VALUE_AXIS_LAST    =CHART_VALUE_AXIS_RELATIVE
     };
   //--- flags
   enum EnWidgetFlags
     {
      WIDGET_FLAG_NONE         =0x00000000,           // no flags
      WIDGET_FLAG_AUTO_WIDTH   =0x00000001,           // auto width
      WIDGET_FLAG_AUTO_HEIGHT  =0x00000002,           // auto height
      WIDGET_FLAG_AUTO_TOP     =0x00000004,           // auto top
      WIDGET_FLAG_AUTO_LEFT    =0x00000008,           // auto left
      WIDGET_FLAG_HIDE_ZEROES  =0x00000100,           // hide zero values from chart
      //--- enumeration borders
      WIDGET_FLAG_DEFAULT      =WIDGET_FLAG_AUTO_WIDTH|WIDGET_FLAG_AUTO_HEIGHT|WIDGET_FLAG_AUTO_TOP|WIDGET_FLAG_AUTO_LEFT,
      WIDGET_FLAG_ALL          =WIDGET_FLAG_AUTO_WIDTH|WIDGET_FLAG_AUTO_HEIGHT|WIDGET_FLAG_AUTO_TOP|WIDGET_FLAG_AUTO_LEFT
     };
   //--- geo chart widget colors
   enum EnChartGeoColors
     {
      CHART_GEO_COLOR_BLUE     =0,                    // blue (default)
      CHART_GEO_COLOR_ORANGE   =1,                    // orange
      CHART_GEO_COLOR_RED      =2,                    // red
      CHART_GEO_COLOR_PURPLE   =3,                    // purple
      CHART_GEO_COLOR_DARK_BLUE=4,                    // dark blue
      //--- enumeration borders
      CHART_GEO_COLOR_FIRST    =CHART_GEO_COLOR_BLUE,
      CHART_GEO_COLOR_LAST     =CHART_GEO_COLOR_DARK_BLUE
     };
   //--- common methods
   virtual MTAPIRES  Assign(const IMTReportDashboardWidget *widget)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- title
   virtual LPCWSTR   Title(void) const=0;
   virtual MTAPIRES  Title(LPCWSTR title)=0;
   //--- type
   virtual uint32_t  Type(void) const=0;
   virtual MTAPIRES  Type(const uint32_t type)=0;
   //--- chart stack type
   virtual uint32_t  ChartStackType(void) const=0;
   virtual MTAPIRES  ChartStackType(const uint32_t stack_type)=0;
   //--- flags
   virtual uint64_t  Flags(void) const=0;
   virtual MTAPIRES  Flags(const uint64_t flags)=0;
   //--- widget width
   virtual uint32_t  Width(void) const=0;
   virtual MTAPIRES  Width(const uint32_t width)=0;
   //--- widget height
   virtual uint32_t  Height(void) const=0;
   virtual MTAPIRES  Height(const uint32_t height)=0;
   //--- widget left edge position
   virtual uint32_t  Left(void) const=0;
   virtual MTAPIRES  Left(const uint32_t left)=0;
   //--- widget top edge position
   virtual uint32_t  Top(void) const=0;
   virtual MTAPIRES  Top(const uint32_t top)=0;
   //--- description
   virtual LPCWSTR   Description(void) const=0;
   virtual MTAPIRES  Description(LPCWSTR description)=0;
   //--- chart value axis
   virtual uint32_t  ChartValueAxis(void) const=0;
   virtual MTAPIRES  ChartValueAxis(const uint32_t value_axis)=0;
   //--- HTML
   virtual IMTReportDashboardHtml* Html(void)=0;
   virtual MTAPIRES  Html(IMTReportDashboardHtml *html)=0;
   //--- dataset
   virtual IMTDataset* Data(void)=0;
   virtual MTAPIRES  Data(IMTDataset *data_set)=0;
   //--- title column
   virtual uint32_t  DataColumnTitle(void) const=0;
   virtual MTAPIRES  DataColumnTitle(const uint32_t column_id)=0;
   //--- data set column management
   virtual MTAPIRES  DataColumnClear(void)=0;
   virtual MTAPIRES  DataColumnAdd(const uint32_t column_id)=0;
   virtual MTAPIRES  DataColumnDelete(const uint32_t pos)=0;
   virtual MTAPIRES  DataColumnShift(const uint32_t pos,const int32_t shift)=0;
   virtual uint32_t  DataColumnTotal(void) const=0;
   virtual const uint32_t* DataColumnNext(const uint32_t pos) const=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportDashboardWidget(void) {}
  };
//+------------------------------------------------------------------+
//| Report cache key set interface                                   |
//+------------------------------------------------------------------+
class IMTReportCacheKeySet
  {
public:
   //--- common methods
   virtual void      Release(void)=0;
   virtual MTAPIRES  Assign(const IMTReportCacheKeySet *keyset)=0;
   virtual MTAPIRES  Clear(void)=0;
   //--- swap with keyset
   virtual MTAPIRES  Swap(IMTReportCacheKeySet *keyset)=0;
   //--- total keys in set
   virtual uint32_t  Total(void) const=0;
   //--- pointer to keys sorted array
   virtual const uint64_t* Array(void) const=0;
   //--- iterate sorted keys in array
   virtual const uint64_t* Next(const uint64_t *key) const=0;
   //--- search key
   virtual const uint64_t* Search(const uint64_t key) const=0;
   //--- check all keys from passed set present
   virtual MTAPIRES  ContainsSet(const IMTReportCacheKeySet *keyset) const=0;
   //--- reserve memory for keys
   virtual MTAPIRES  Reserve(const uint32_t total)=0;
   //--- insert keys
   virtual MTAPIRES  Insert(const uint64_t key)=0;
   virtual MTAPIRES  InsertArray(const uint64_t *keys,const uint32_t total)=0;
   virtual MTAPIRES  InsertSet(const IMTReportCacheKeySet *keyset)=0;
   //--- remove keys
   virtual MTAPIRES  Remove(const uint64_t key)=0;
   virtual MTAPIRES  RemoveArray(const uint64_t *keys,const uint32_t total)=0;
   virtual MTAPIRES  RemoveSet(const IMTReportCacheKeySet *keyset)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportCacheKeySet(void) {}
  };
//+------------------------------------------------------------------+
//| Report cache value interface                                     |
//| Contains valid key and data until next calls:                    |
//| IMTReportCache::Release()                                        |
//| IMTReportCache::ReadEnd()                                        |
//| IMTReportCache::WriteEnd()                                       |
//+------------------------------------------------------------------+
class IMTReportCacheValue
  {
public:
   //--- release interface
   virtual void      Release(void)=0;
   //--- key
   virtual uint64_t  Key(void) const=0;
   //--- data
   virtual const void* Data(void) const=0;
   virtual uint32_t  Size(void) const=0;
   //--- iterate to next value
   virtual MTAPIRES  Next(void)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportCacheValue(void) {}
  };
//+------------------------------------------------------------------+
//| Report cache interface                                           |
//+------------------------------------------------------------------+
class IMTReportCache
  {
public:
   //--- release interface
   virtual void      Release(void)=0;
   //--- name
   virtual LPCWSTR   Name(void) const=0;
   //--- last generation time
   virtual int64_t   TimeLastWrite(void) const=0;
   //--- create value object
   virtual IMTReportCacheValue* ValueCreate(void) const=0;
   //--- read begin/end
   virtual MTAPIRES  ReadBegin(void)=0;
   virtual MTAPIRES  ReadEnd(void)=0;
   //--- read parameters
   virtual int64_t   ReadParamFrom(void) const=0;
   virtual int64_t   ReadParamTo(void) const=0;
   virtual LPCWSTR   ReadParamString(void) const=0;
   virtual const void* ReadParamData(uint32_t &size) const=0;
   virtual MTAPIRES  ReadParamKeySet(IMTReportCacheKeySet *keys) const=0;
   //--- read value by key
   virtual MTAPIRES  ReadValue(const uint64_t key,IMTReportCacheValue *value) const=0;
   //--- read multiple values by key set
   virtual MTAPIRES  ReadValues(const IMTReportCacheKeySet *keys,IMTReportCacheValue *value) const=0;
   //--- get keys from set that missing in cache
   virtual MTAPIRES  ReadMissingKeys(const IMTReportCacheKeySet *keys,IMTReportCacheKeySet *missing_keys) const=0;
   //--- get string position in dictionary
   virtual MTAPIRES  ReadDictionaryPos(const uint32_t dictionary_id,LPCWSTR string,uint32_t &pos) const=0;
   //--- get string from dictionary by position
   virtual MTAPIRES  ReadDictionaryString(const uint32_t dictionary_id,const uint32_t pos,LPCWSTR &string) const=0;
   //--- write begin/end
   virtual MTAPIRES  WriteBegin(void)=0;
   virtual MTAPIRES  WriteEnd(bool apply)=0;
   //--- write parameters
   virtual MTAPIRES  WriteParamFrom(const int64_t from)=0;
   virtual MTAPIRES  WriteParamTo(const int64_t to)=0;
   virtual MTAPIRES  WriteParamString(LPCWSTR param)=0;
   virtual MTAPIRES  WriteParamData(const void *data,const uint32_t size)=0;
   virtual MTAPIRES  WriteParamKeySet(const IMTReportCacheKeySet *keys,const bool merge)=0;
   //--- write value by key
   virtual MTAPIRES  WriteValue(const uint64_t key,const void *value,const uint32_t size)=0;
   //--- write empty value for keys from set that missing in cache
   virtual MTAPIRES  WriteMissingKeys(const IMTReportCacheKeySet *keys)=0;
   //--- write new string to dictionary or get existing string position
   virtual MTAPIRES  WriteDictionaryString(const uint32_t dictionary_id,LPCWSTR string,uint32_t &pos)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportCache(void) {}
  };
//+------------------------------------------------------------------+
//| Report API interface                                             |
//+------------------------------------------------------------------+
class IMTReportAPI
  {
public:
   //--- HtmlWriteSafe flags
   enum EnHtmlSafeFlags
     {
      HTML_SAFE_NONE          =0x0,    // no flags
      HTML_SAFE_USENOBSP      =0x1,    // replace blank to &nbsp;
     };
   //--- HtmlTplProcess flags
   enum EnTplProcessFlags
     {
      TPL_PROCESS_NONE        =0x0,    // no flags
     };
   //--- Dashboard flags
   enum EnDashboardFlags
     {
      DASHBOARD_FLAG_NONE     =0x0,    // no flags
     };

public:
   //--- server description
   virtual MTAPIRES  About(MTReportServerInfo& info)=0;
   //--- license check
   virtual MTAPIRES  LicenseCheck(LPCWSTR license_name)=0;
   //--- memory management
   virtual void*     Allocate(const uint32_t size)=0;
   virtual void      Free(void* ptr)=0;
   //--- clear result
   virtual MTAPIRES  Clear(void)=0;
   virtual MTAPIRES  FunctionReserved1(void)=0;
   virtual MTAPIRES  FunctionReserved2(void)=0;
   virtual MTAPIRES  FunctionReserved3(void)=0;
   virtual MTAPIRES  FunctionReserved4(void)=0;
   virtual MTAPIRES  FunctionReserved5(void)=0;
   virtual MTAPIRES  FunctionReserved6(void)=0;
   //--- common flags functions
   virtual MTAPIRES  IsStopped(void)=0;
   virtual MTAPIRES  IsReserved1(void)=0;
   virtual MTAPIRES  IsReserved2(void)=0;
   virtual MTAPIRES  IsReserved3(void)=0;
   virtual MTAPIRES  IsReserved4(void)=0;
   virtual MTAPIRES  IsReserved5(void)=0;
   virtual MTAPIRES  IsReserved6(void)=0;
   //--- server log functions
   virtual MTAPIRES  LoggerOut(const uint32_t code,LPCWSTR msg,...)=0;
   virtual MTAPIRES  LoggerRequest(const uint32_t mode,const uint32_t type,const int64_t from,const int64_t to,LPCWSTR filter,MTLogRecord*& records,uint32_t& records_total)=0;
   virtual void      LoggerFlush(void)=0;
   virtual MTAPIRES  LoggerOutString(const uint32_t code,LPCWSTR string)=0;
   virtual MTAPIRES  LoggerReserved2(void)=0;
   virtual MTAPIRES  LoggerReserved3(void)=0;
   virtual MTAPIRES  LoggerReserved4(void)=0;
   //--- report parameters
   virtual IMTConParam* ParamCreate(void)=0;
   virtual uint32_t  ParamTotal(void)=0;
   virtual MTAPIRES  ParamGet(LPCWSTR name,IMTConParam *param)=0;
   virtual MTAPIRES  ParamNext(const uint32_t pos,IMTConParam *param)=0;
   virtual MTAPIRES  ParamReserved1(void)=0;
   virtual MTAPIRES  ParamReserved2(void)=0;
   virtual MTAPIRES  ParamReserved3(void)=0;
   virtual MTAPIRES  ParamReserved4(void)=0;
   //--- report parameters helpers
   virtual MTAPIRES  ParamLogins(uint64_t*& logins,uint32_t& total)=0;
   virtual int64_t   ParamFrom(void)=0;
   virtual int64_t   ParamTo(void)=0;
   virtual LPCWSTR   ParamGroups(void)=0;
   virtual LPCWSTR   ParamSymbols(void)=0;
   virtual uint32_t  ParamIEVersion(void)=0;
   virtual uint32_t  ParamScreenWidth(void)=0;
   virtual uint32_t  ParamScreenHeight(void)=0;
   virtual MTAPIRES  ParamHelperReserved1(void)=0;
   virtual MTAPIRES  ParamHelperReserved2(void)=0;
   virtual MTAPIRES  ParamHelperReserved3(void)=0;
   virtual MTAPIRES  ParamHelperReserved4(void)=0;
   //--- HTML management
   virtual MTAPIRES  HtmlWrite(LPCWSTR format,...)=0;
   virtual MTAPIRES  HtmlWriteSafe(LPCWSTR html,const uint32_t flags)=0;
   virtual MTAPIRES  HtmlWriteString(LPCWSTR html)=0;
   virtual MTAPIRES  HtmlWriteReserved2(void)=0;
   virtual MTAPIRES  HtmlTplLoad(LPCWSTR templstr)=0;
   virtual MTAPIRES  HtmlTplLoadFile(LPCWSTR templname)=0;
   virtual MTAPIRES  HtmlTplLoadResource(const uint32_t resid,LPCWSTR restype)=0;
   virtual MTAPIRES  HtmlTplNext(MTAPISTR& tag,uint32_t *counter)=0;
   virtual MTAPIRES  HtmlTplProcess(const uint32_t flags)=0;
   virtual MTAPIRES  HtmlTplReserved1(void)=0;
   virtual MTAPIRES  HtmlTplReserved2(void)=0;
   virtual MTAPIRES  HtmlReserved1(void)=0;
   virtual MTAPIRES  HtmlReserved2(void)=0;
   virtual MTAPIRES  HtmlReserved3(void)=0;
   virtual MTAPIRES  HtmlReserved4(void)=0;
   //--- table column management
   virtual IMTDatasetColumn* TableColumnCreate(void)=0;
   virtual MTAPIRES  TableColumnClear(void)=0;
   virtual MTAPIRES  TableColumnAdd(const IMTDatasetColumn *column)=0;
   virtual MTAPIRES  TableColumnDelete(const uint32_t pos)=0;
   virtual uint32_t  TableColumnTotal(void)=0;
   virtual uint32_t  TableColumnSize(void)=0;
   virtual MTAPIRES  TableColumnNext(const uint32_t pos,IMTDatasetColumn *column)=0;
   virtual MTAPIRES  TableColumnReserved1(void)=0;
   virtual MTAPIRES  TableColumnReserved2(void)=0;
   //--- table row management
   virtual MTAPIRES  TableRowClear(void)=0;
   virtual MTAPIRES  TableRowWrite(const void *data,const uint32_t size)=0;
   virtual uint32_t  TableRowTotal(void)=0;
   virtual MTAPIRES  TableRowReserved1(void)=0;
   virtual MTAPIRES  TableRowReserved2(void)=0;
   //--- table summary management
   virtual IMTDatasetSummary* TableSummaryCreate(void)=0;
   virtual MTAPIRES  TableSummaryClear(void)=0;
   virtual MTAPIRES  TableSummaryAdd(const IMTDatasetSummary *summary)=0;
   virtual MTAPIRES  TableSummaryDelete(const uint32_t pos)=0;
   virtual MTAPIRES  TableSummaryNext(const uint32_t pos,IMTDatasetSummary *summary)=0;
   virtual uint32_t  TableSummaryTotal(void)=0;
   virtual MTAPIRES  TableSummaryReserved1(void)=0;
   virtual MTAPIRES  TableSummaryReserved2(void)=0;
   virtual MTAPIRES  TableSummaryReserved3(void)=0;
   //--- table reserved functions
   virtual MTAPIRES  TableReserved1(void)=0;
   virtual MTAPIRES  TableReserved2(void)=0;
   virtual MTAPIRES  TableReserved3(void)=0;
   virtual MTAPIRES  TableReserved4(void)=0;
   virtual MTAPIRES  TableReserved5(void)=0;
   virtual MTAPIRES  TableReserved6(void)=0;
   //--- chart management
   virtual IMTReportChart* ChartCreate(void)=0;
   virtual IMTReportSeries* ChartCreateSeries(void)=0;
   virtual MTAPIRES  ChartWriteHtml(const IMTReportChart *chart)=0;
   virtual MTAPIRES  ChartReserved1(void)=0;
   virtual MTAPIRES  ChartReserved2(void)=0;
   virtual MTAPIRES  ChartReserved3(void)=0;
   virtual MTAPIRES  ChartReserved4(void)=0;
   //--- report configuration
   virtual IMTConReport* ReportCreate(void)=0;
   virtual MTAPIRES  ReportCurrent(IMTConReport *report)=0;
   virtual MTAPIRES  ReportReserved1(void)=0;
   virtual MTAPIRES  ReportReserved2(void)=0;
   virtual MTAPIRES  ReportReserved3(void)=0;
   virtual MTAPIRES  ReportReserved4(void)=0;
   //--- common configuration 
   virtual IMTConCommon* CommonCreate(void)=0;
   virtual MTAPIRES  CommonGet(IMTConCommon* common)=0;
   virtual IMTConAccountAllocation* CommonCreateAllocation(void)=0;
   virtual IMTConAccountAgreement* CommonCreateAgreement(void)=0;
   virtual MTAPIRES  CommonReserved3(void)=0;
   virtual MTAPIRES  CommonReserved4(void)=0;
   //--- network configuration
   virtual IMTConServer* NetServerCreate(void)=0;
   virtual IMTConServerRange* NetServerRangeCreate(void)=0;
   virtual uint32_t  NetServerTotal(void)=0;
   virtual MTAPIRES  NetServerNext(const uint32_t pos,IMTConServer *server)=0;
   virtual MTAPIRES  NetServerGet(const uint64_t id,IMTConServer *server)=0;
   virtual MTAPIRES  NetServerReserved1(void)=0;
   virtual MTAPIRES  NetServerReserved2(void)=0;
   virtual MTAPIRES  NetServerReserved3(void)=0;
   virtual MTAPIRES  NetServerReserved4(void)=0;
   //--- time configuration
   virtual IMTConTime* TimeCreate(void)=0;
   virtual int64_t   TimeCurrent(void)=0;
   virtual int64_t   TimeGeneration(void)=0;
   virtual MTAPIRES  TimeGet(IMTConTime *time)=0;
   virtual MTAPIRES  TimeReserved1(void)=0;
   virtual MTAPIRES  TimeReserved2(void)=0;
   virtual MTAPIRES  TimeReserved3(void)=0;
   virtual MTAPIRES  TimeReserved4(void)=0;
   //--- holiday configuration
   virtual IMTConHoliday* HolidayCreate()=0;
   virtual uint32_t  HolidayTotal(void)=0;
   virtual MTAPIRES  HolidayNext(const uint32_t pos,IMTConHoliday *holiday)=0;
   virtual MTAPIRES  HolidayReserved1(void)=0;
   virtual MTAPIRES  HolidayReserved2(void)=0;
   virtual MTAPIRES  HolidayReserved3(void)=0;
   virtual MTAPIRES  HolidayReserved4(void)=0;
   //--- clients group configuration
   virtual IMTConGroup* GroupCreate(void)=0;
   virtual IMTConGroupSymbol* GroupSymbolCreate(void)=0;
   virtual IMTConCommission* GroupCommissionCreate(void)=0;
   virtual IMTConCommTier* GroupTierCreate(void)=0;
   virtual uint32_t  GroupTotal(void)=0;
   virtual MTAPIRES  GroupNext(const uint32_t pos,IMTConGroup *group)=0;
   virtual MTAPIRES  GroupGet(LPCWSTR name,IMTConGroup *group)=0;
   virtual MTAPIRES  GroupGetLight(LPCWSTR name,IMTConGroup *group)=0;
   virtual MTAPIRES  GroupReserved1(void)=0;
   virtual MTAPIRES  GroupReserved2(void)=0;
   virtual MTAPIRES  GroupReserved3(void)=0;
   virtual MTAPIRES  GroupReserved4(void)=0;
   //--- symbols configuration
   virtual IMTConSymbol* SymbolCreate(void)=0;
   virtual IMTConSymbolSession* SymbolSessionCreate(void)=0;
   virtual uint32_t  SymbolTotal(void)=0;
   virtual MTAPIRES  SymbolNext(const uint32_t pos,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolGetLight(LPCWSTR name,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolGet(LPCWSTR name,const IMTConGroup *group,IMTConSymbol *symbol)=0;
   virtual MTAPIRES  SymbolExist(const IMTConSymbol* symbol,const IMTConGroup* group)=0;
   virtual MTAPIRES  SymbolReserved1(void)=0;
   virtual MTAPIRES  SymbolReserved2(void)=0;
   virtual MTAPIRES  SymbolReserved3(void)=0;
   virtual MTAPIRES  SymbolReserved4(void)=0;
   //--- managers configuration
   virtual IMTConManager* ManagerCreate(void)=0;
   virtual IMTConManagerAccess* ManagerAccessCreate(void)=0;
   virtual MTAPIRES  ManagerCurrent(IMTConManager *manager)=0;
   virtual uint32_t  ManagerTotal(void)=0;
   virtual MTAPIRES  ManagerNext(const uint32_t pos,IMTConManager *manager)=0;
   virtual MTAPIRES  ManagerGet(const uint64_t login,IMTConManager *manager)=0;
   virtual IMTConManagerReport* ManagerReportCreate(void)=0;
   virtual MTAPIRES  ManagerReserved2(void)=0;
   virtual MTAPIRES  ManagerReserved3(void)=0;
   virtual MTAPIRES  ManagerReserved4(void)=0;
   //--- clients database
   virtual IMTUser*  UserCreate(void)=0;
   virtual IMTAccount* UserCreateAccount(void)=0;
   virtual MTAPIRES  UserGet(const uint64_t login,IMTUser *user)=0;
   virtual MTAPIRES  UserGetLight(const uint64_t login,IMTUser *user)=0;
   virtual MTAPIRES  UserLogins(LPCWSTR group,uint64_t*& logins,uint32_t &total)=0;
   virtual MTAPIRES  UserAccountGet(const uint64_t login,IMTAccount *account)=0;
   virtual MTAPIRES  UserSelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  UserAccountSelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  UserReserved3(void)=0;
   virtual MTAPIRES  UserReserved4(void)=0;
   //--- trade orders database
   virtual IMTOrder* OrderCreate(void)=0;
   virtual IMTOrderArray* OrderCreateArray(void)=0;
   virtual MTAPIRES  OrderGet(const uint64_t ticket,IMTOrder *order)=0;
   virtual MTAPIRES  OrderGet(const uint64_t login,IMTOrderArray *orders)=0;
   virtual MTAPIRES  OrderSelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  OrderReserved2(void)=0;
   virtual MTAPIRES  OrderReserved3(void)=0;
   virtual MTAPIRES  OrderReserved4(void)=0;
   //--- trade history orders databases
   virtual MTAPIRES  HistoryGet(const uint64_t ticket,IMTOrder *order)=0;
   virtual MTAPIRES  HistoryGet(const uint64_t login,const int64_t from,const int64_t to,IMTOrderArray *orders)=0;
   virtual MTAPIRES  HistorySelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  HistoryReserved2(void)=0;
   virtual MTAPIRES  HistoryReserved3(void)=0;
   virtual MTAPIRES  HistoryReserved4(void)=0;
   //--- trade deals database
   virtual IMTDeal*  DealCreate(void)=0;
   virtual IMTDealArray* DealCreateArray(void)=0;
   virtual MTAPIRES  DealGet(const uint64_t ticket,IMTDeal *deal)=0;
   virtual MTAPIRES  DealGet(const uint64_t login,const int64_t from,const int64_t to,IMTDealArray *deals)=0;
   virtual MTAPIRES  DealSelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  DealReserved2(void)=0;
   virtual MTAPIRES  DealReserved3(void)=0;
   virtual MTAPIRES  DealReserved4(void)=0;
   //--- trade positions database
   virtual IMTPosition* PositionCreate(void)=0;
   virtual IMTPositionArray* PositionCreateArray(void)=0;
   virtual MTAPIRES  PositionGet(const uint64_t login,LPCWSTR symbol,IMTPosition *position)=0;
   virtual MTAPIRES  PositionGet(const uint64_t login,IMTPositionArray *positions)=0;
   virtual MTAPIRES  PositionGetByTicket(const uint64_t ticket,IMTPosition *position)=0;
   virtual MTAPIRES  PositionSelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  PositionReserved2(void)=0;
   virtual MTAPIRES  PositionReserved3(void)=0;
   //--- trade daily database
   virtual IMTDaily* DailyCreate(void)=0;
   virtual IMTDailyArray* DailyCreateArray(void)=0;
   virtual MTAPIRES  DailyGet(const uint64_t login,const int64_t datetime,IMTDaily *daily)=0;
   virtual MTAPIRES  DailyGetLight(const uint64_t login,const int64_t datetime,IMTDaily *daily)=0;
   virtual MTAPIRES  DailyGet(const uint64_t login,const int64_t from,const int64_t to,IMTDailyArray *daily)=0;
   virtual MTAPIRES  DailyGetLight(const uint64_t login,const int64_t from,const int64_t to,IMTDailyArray *daily)=0;
   virtual MTAPIRES  DailySelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual MTAPIRES  DailyReserved2(void)=0;
   virtual MTAPIRES  DailyReserved3(void)=0;
   virtual MTAPIRES  DailyReserved4(void)=0;
   //--- trade methods
   virtual MTAPIRES  TradeProfit(LPCWSTR group,LPCWSTR symbol,const uint32_t type,const uint64_t volume,const double price_open,const double price_close,double& profit,double& profit_rate)=0;
   virtual MTAPIRES  TradeRateBuy(LPCWSTR base,LPCWSTR currency,double& rate,LPCWSTR group=NULL,LPCWSTR symbol=NULL,const double price=0)=0;
   virtual MTAPIRES  TradeRateSell(LPCWSTR base,LPCWSTR currency,double& rate,LPCWSTR group=NULL,LPCWSTR symbol=NULL,const double price=0)=0;
   virtual MTAPIRES  TradeProfitExt(LPCWSTR group,LPCWSTR symbol,const uint32_t type,const uint64_t volume,const double price_open,const double price_close,double& profit,double& profit_rate)=0;
   virtual MTAPIRES  TradeReserved2(void)=0;
   virtual MTAPIRES  TradeReserved3(void)=0;
   virtual MTAPIRES  TradeReserved4(void)=0;
   //--- plugins configuration
   virtual IMTConPlugin* PluginCreate(void)=0;
   virtual IMTConPluginModule* PluginModuleCreate(void)=0;
   virtual IMTConParam* PluginParamCreate(void)=0;
   virtual uint32_t  PluginTotal(void)=0;
   virtual MTAPIRES  PluginNext(const uint32_t pos,IMTConPlugin *plugin)=0;
   virtual MTAPIRES  PluginGet(const uint64_t server,LPCWSTR name,IMTConPlugin *plugin)=0;
   virtual uint32_t  PluginModuleTotal(void)=0;
   virtual MTAPIRES  PluginModuleNext(const uint32_t pos,IMTConPluginModule *module)=0;
   virtual MTAPIRES  PluginModuleGet(const uint64_t server,LPCWSTR name,IMTConPluginModule *module)=0;
   virtual MTAPIRES  PluginReserved1(void)=0;
   virtual MTAPIRES  PluginReserved2(void)=0;
   virtual MTAPIRES  PluginReserved3(void)=0;
   virtual MTAPIRES  PluginReserved4(void)=0;
   //--- datafeeds configuration
   virtual IMTConFeeder* FeederCreate(void)=0;
   virtual IMTConFeederModule* FeederModuleCreate(void)=0;
   virtual IMTConParam* FeederParamCreate(void)=0;
   virtual IMTConFeederTranslate* FeederTranslateCreate(void)=0;
   virtual uint32_t  FeederTotal(void)=0;
   virtual MTAPIRES  FeederNext(const uint32_t pos,IMTConFeeder *feeder)=0;
   virtual MTAPIRES  FeederGet(LPCWSTR name,IMTConFeeder *feeder)=0;
   virtual uint32_t  FeederModuleTotal(void)=0;
   virtual MTAPIRES  FeederModuleNext(const uint32_t pos,IMTConFeederModule *module)=0;
   virtual MTAPIRES  FeederModuleGet(LPCWSTR name,IMTConFeederModule *module)=0;
   virtual MTAPIRES  FeederReserved1(void)=0;
   virtual MTAPIRES  FeederReserved2(void)=0;
   virtual MTAPIRES  FeederReserved3(void)=0;
   virtual MTAPIRES  FeederReserved4(void)=0;
   //--- gateways configuration
   virtual IMTConGateway* GatewayCreate(void)=0;
   virtual IMTConGatewayModule* GatewayModuleCreate(void)=0;
   virtual IMTConParam* GatewayParamCreate(void)=0;
   virtual IMTConGatewayTranslate* GatewayTranslateCreate(void)=0;
   virtual uint32_t  GatewayTotal(void)=0;
   virtual MTAPIRES  GatewayNext(const uint32_t pos,IMTConGateway *gateway)=0;
   virtual MTAPIRES  GatewayGet(LPCWSTR name,IMTConGateway *gateway)=0;
   virtual uint32_t  GatewayModuleTotal(void)=0;
   virtual MTAPIRES  GatewayModuleNext(const uint32_t pos,IMTConGatewayModule *module)=0;
   virtual MTAPIRES  GatewayModuleGet(LPCWSTR name,IMTConGatewayModule *module)=0;
   virtual MTAPIRES  GatewayReserved1(void)=0;
   virtual MTAPIRES  GatewayReserved2(void)=0;
   virtual MTAPIRES  GatewayReserved3(void)=0;
   virtual MTAPIRES  GatewayReserved4(void)=0;
   //--- ticks
   virtual MTAPIRES  TickHistoryGet(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickHistoryGetRaw(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickLast(LPCWSTR symbol,MTTickShort& tick)=0;
   virtual MTAPIRES  TickLast(const IMTConSymbol* symbol,MTTickShort& tick)=0;
   virtual MTAPIRES  TickStat(LPCWSTR symbol,MTTickStat& stat)=0;
   virtual MTAPIRES  TickReserved4(void)=0;
   //--- chart methods
   virtual MTAPIRES  ChartHistoryGet(LPCWSTR symbol,const int64_t from,const int64_t to,MTChartBar*& bars,uint32_t& bars_total)=0;
   virtual MTAPIRES  ChartHistoryReserved1(void)=0;
   virtual MTAPIRES  ChartHistoryReserved2(void)=0;
   virtual MTAPIRES  ChartHistoryReserved3(void)=0;
   virtual MTAPIRES  ChartHistoryReserved4(void)=0;
   //--- dashboard width
   virtual uint32_t  DashboardWidth(void)=0;
   virtual MTAPIRES  DashboardWidth(const uint32_t width)=0;
   //--- dashboard height
   virtual uint32_t  DashboardHeight(void)=0;
   virtual MTAPIRES  DashboardHeight(const uint32_t height)=0;
   //--- dashboard title
   virtual LPCWSTR   DashboardTitle(void)=0;
   virtual MTAPIRES  DashboardTitle(LPCWSTR title)=0;
   //--- dashboard flags
   virtual uint64_t  DashboardFlags(void)=0;
   virtual MTAPIRES  DashboardFlags(const uint64_t flags)=0;
   //--- reserved dashboard properties
   virtual MTAPIRES  DashboardReserved1(void)=0;
   virtual MTAPIRES  DashboardReserved2(void)=0;
   virtual MTAPIRES  DashboardReserved3(void)=0;
   virtual MTAPIRES  DashboardReserved4(void)=0;
   //--- dataset management
   virtual IMTDataset* DatasetAppend(void)=0;
   virtual MTAPIRES  DatasetClear(void)=0;
   virtual MTAPIRES  DatasetDelete(const uint32_t pos)=0;
   virtual uint32_t  DatasetTotal(void) const=0;
   virtual IMTDataset* DatasetNext(const uint32_t pos)=0;
   virtual IMTDatasetRequest* DatasetRequestCreate(void)=0;
   virtual MTAPIRES  DatasetReserved2(void)=0;
   virtual MTAPIRES  DatasetReserved3(void)=0;
   virtual MTAPIRES  DatasetReserved4(void)=0;
   //--- dashboard HTML management
   virtual IMTReportDashboardHtml* DashboardHtmlAppend(void)=0;
   virtual MTAPIRES  DashboardHtmlClear(void)=0;
   virtual MTAPIRES  DashboardHtmlDelete(const uint32_t pos)=0;
   virtual uint32_t  DashboardHtmlTotal(void) const=0;
   virtual IMTReportDashboardHtml* DashboardHtmlNext(const uint32_t pos)=0;
   virtual MTAPIRES  DashboardHtmlReserved1(void)=0;
   virtual MTAPIRES  DashboardHtmlReserved2(void)=0;
   virtual MTAPIRES  DashboardHtmlReserved3(void)=0;
   virtual MTAPIRES  DashboardHtmlReserved4(void)=0;
   //--- dashboard widget management
   virtual IMTReportDashboardWidget* DashboardWidgetAppend(void)=0;
   virtual MTAPIRES  DashboardWidgetClear(void)=0;
   virtual MTAPIRES  DashboardWidgetDelete(const uint32_t pos)=0;
   virtual uint32_t  DashboardWidgetTotal(void) const=0;
   virtual IMTReportDashboardWidget* DashboardWidgetNext(const uint32_t pos)=0;
   virtual MTAPIRES  DashboardWidgetReserved1(void)=0;
   virtual MTAPIRES  DashboardWidgetReserved2(void)=0;
   virtual MTAPIRES  DashboardWidgetReserved3(void)=0;
   virtual MTAPIRES  DashboardWidgetReserved4(void)=0;
   //--- key set management
   virtual IMTReportCacheKeySet* KeySetCreate(void) const=0;
   virtual MTAPIRES  KeySetParamLogins(IMTReportCacheKeySet *keyset) const=0;
   virtual MTAPIRES  KeySetReserved1(void)=0;
   virtual MTAPIRES  KeySetReserved2(void)=0;
   virtual MTAPIRES  KeySetReserved3(void)=0;
   virtual MTAPIRES  KeySetReserved4(void)=0;
   //--- report cache management
   virtual IMTReportCache* ReportCacheCreate(void) const=0;
   virtual MTAPIRES  ReportCacheGet(LPCWSTR name,const uint32_t version,IMTReportCache *report_cache)=0;
   virtual MTAPIRES  ReportCacheGetTemporary(LPCWSTR name,const uint32_t version,const uint64_t key_time_to_live,IMTReportCache *report_cache)=0;
   virtual MTAPIRES  ReportCacheReserved1(void)=0;
   virtual MTAPIRES  ReportCacheReserved2(void)=0;
   virtual MTAPIRES  ReportCacheReserved3(void)=0;
   virtual MTAPIRES  ReportCacheReserved4(void)=0;
   //--- clients database
   virtual MTAPIRES  ClientSelect(const IMTDatasetRequest *request,IMTDataset *dataset)=0;
   virtual IMTClient *ClientCreate(void)=0;
   virtual IMTClientArray *ClientCreateArray(void)=0;
   virtual MTAPIRES  ClientGet(const uint64_t client_id,IMTClient *client)=0;
   virtual MTAPIRES  ClientGetHistory(const uint64_t client_id,const uint64_t author,const int64_t from,const int64_t to,IMTClientArray *history)=0;
   virtual MTAPIRES  ClientIdsAll(uint64_t*& ids,uint32_t& ids_total)=0;
   virtual MTAPIRES  ClientIdsByGroup(const LPCWSTR groups,uint64_t*& ids,uint32_t& ids_total)=0;
   virtual MTAPIRES  ClientUserLogins(const uint64_t client_id,uint64_t*& logins,uint32_t& logins_total)=0;
   virtual MTAPIRES  ClientReserved8(void)=0;
   virtual MTAPIRES  ClientReserved9(void)=0;
   //--- funds configuration
   virtual IMTConFund* FundCreate(void)=0;
   virtual IMTConFundAccount* FundAccountCreate(void)=0;
   virtual IMTConFundInvestor* FundInvestorCreate(void)=0;
   virtual uint32_t  FundTotal(void)=0;
   virtual MTAPIRES  FundNext(const uint32_t pos,IMTConFund *fund)=0;
   virtual MTAPIRES  FundGet(LPCWSTR name,IMTConFund *fund)=0;
   virtual MTAPIRES  FundReserved1(void)=0;
   virtual MTAPIRES  FundReserved2(void)=0;
   virtual MTAPIRES  FundReserved3(void)=0;
   virtual MTAPIRES  FundReserved4(void)=0;
   //--- ticks
   virtual MTAPIRES  TickGet(LPCWSTR symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickGet(const IMTConSymbol* symbol,const int64_t from,const int64_t to,MTTickShort*& ticks,uint32_t& ticks_total)=0;
   virtual MTAPIRES  TickReserved5(void)=0;
   virtual MTAPIRES  TickReserved6(void)=0;
   virtual MTAPIRES  TickReserved7(void)=0;
   virtual MTAPIRES  TickReserved8(void)=0;
   //--- subscription configuration
   virtual IMTConSubscription* SubscriptionCfgCreate()=0;
   virtual IMTConSubscriptionSymbol* SubscriptionCfgSymbolCreate()=0;
   virtual IMTConSubscriptionNews* SubscriptionCfgNewsCreate()=0;
   virtual uint32_t  SubscriptionCfgTotal(void)=0;
   virtual MTAPIRES  SubscriptionCfgNext(const uint32_t pos,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgGet(LPCWSTR name,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgGetByID(const uint64_t id,IMTConSubscription* config)=0;
   virtual MTAPIRES  SubscriptionCfgReserved1(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved2(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved3(void)=0;
   virtual MTAPIRES  SubscriptionCfgReserved4(void)=0;
   //--- subscription records
   virtual IMTSubscription* SubscriptionCreate()=0;
   virtual IMTSubscriptionArray* SubscriptionCreateArray(void)=0;
   virtual bool      SubscriptionExist(const uint64_t login,const uint64_t subscription)=0;
   virtual MTAPIRES  SubscriptionGet(const uint64_t login,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionGetBySubscription(const uint64_t login,const uint64_t subscription,IMTSubscription* record)=0;
   virtual MTAPIRES  SubscriptionGetByID(const uint64_t id,IMTSubscription* record)=0;
   virtual MTAPIRES  SubscriptionGetByLogins(const uint64_t *logins,uint32_t logins_total,IMTSubscriptionArray* records)=0;
   virtual MTAPIRES  SubscriptionReserved1(void)=0;
   virtual MTAPIRES  SubscriptionReserved2(void)=0;
   virtual MTAPIRES  SubscriptionReserved3(void)=0;
   virtual MTAPIRES  SubscriptionReserved4(void)=0;
   //--- subscription history records
   virtual IMTSubscriptionHistory* SubscriptionHistoryCreate()=0;
   virtual IMTSubscriptionHistoryArray* SubscriptionHistoryCreateArray(void)=0;
   virtual MTAPIRES  SubscriptionHistoryGet(const int64_t from,const int64_t to,const uint64_t login,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryGetByID(const uint64_t id,IMTSubscriptionHistory* record)=0;
   virtual MTAPIRES  SubscriptionHistoryGetByLogins(const int64_t from,const int64_t to,const uint64_t *logins,uint32_t logins_total,IMTSubscriptionHistoryArray* records)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved1(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved2(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved3(void)=0;
   virtual MTAPIRES  SubscriptionHistoryReserved4(void)=0;
   //--- floating leverage configuration
   virtual IMTConLeverage* LeverageCreate(void)=0;
   virtual IMTConLeverageRule* LeverageRuleCreate(void)=0;
   virtual IMTConLeverageTier* LeverageTierCreate(void)=0;
   virtual uint32_t  LeverageTotal(void)=0;
   virtual MTAPIRES  LeverageNext(const uint32_t pos,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageGet(LPCWSTR name,IMTConLeverage* cfg)=0;
   virtual MTAPIRES  LeverageReserved1(void)=0;
   virtual MTAPIRES  LeverageReserved2(void)=0;
   virtual MTAPIRES  LeverageReserved3(void)=0;
   virtual MTAPIRES  LeverageReserved4(void)=0;
   //--- geodata
   virtual IMTGeo*   GeoCreate(void)=0;
   virtual MTAPIRES  GeoResolve(LPCWSTR address,const uint32_t flags,IMTGeo* record)=0;
   virtual MTAPIRES  GeoResolveBatch(LPCWSTR* addresses,const uint32_t addresses_total,const uint32_t flags,IMTGeo** records,MTAPIRES* results)=0;
   virtual MTAPIRES  GeoResolveReserved1(void)=0;
   virtual MTAPIRES  GeoResolveReserved2(void)=0;
   virtual MTAPIRES  GeoResolveReserved3(void)=0;
   virtual MTAPIRES  GeoResolveReserved4(void)=0;
   //--- ultency liquidity provider configuration
   virtual IMTConUltLiquidity* UltLiquidityCreate(void)=0;
   virtual uint32_t  UltLiquidityTotal(void)=0;
   virtual MTAPIRES  UltLiquidityNext(const uint32_t pos,IMTConUltLiquidity *config)=0;
   virtual MTAPIRES  UltLiquidityGet(const uint64_t server,const uint32_t id,IMTConUltLiquidity* cfg)=0;
   virtual MTAPIRES  UltLiquidityReserved1(void)=0;
   virtual MTAPIRES  UltLiquidityReserved2(void)=0;
   virtual MTAPIRES  UltLiquidityReserved3(void)=0;
   virtual MTAPIRES  UltLiquidityReserved4(void)=0;
   //--- ultency liquidity provider history configuration
   virtual IMTConUltLiquidityHistory* UltLiqHistoryCreate(void)=0;
   virtual uint32_t  UltLiqHistoryTotal(void)=0;
   virtual MTAPIRES  UltLiqHistoryNext(const uint32_t pos,IMTConUltLiquidityHistory *config)=0;
   virtual MTAPIRES  UltLiqHistoryGet(const uint64_t server,const uint32_t id,IMTConUltLiquidityHistory* cfg)=0;
   virtual MTAPIRES  UltLiqHistoryReserved1(void)=0;
   virtual MTAPIRES  UltLiqHistoryReserved2(void)=0;
   virtual MTAPIRES  UltLiqHistoryReserved3(void)=0;
   virtual MTAPIRES  UltLiqHistoryReserved4(void)=0;
   //--- ultency symbol
   virtual IMTConUltSymbol* UltSymbolCreate(void)=0;
   virtual IMTConUltSymbolLP* UltSymbolLPCreate(void)=0;
   virtual IMTConUltSymbolBand* UltSymbolBandCreate(void)=0;
   virtual uint32_t  UltSymbolTotal(void)=0;
   virtual MTAPIRES  UltSymbolNext(const uint32_t pos,IMTConUltSymbol *config)=0;
   virtual MTAPIRES  UltSymbolGet(const uint64_t server,LPCWSTR name,IMTConUltSymbol* cfg)=0;
   virtual MTAPIRES  UltSymbolReserved1(void)=0;
   virtual MTAPIRES  UltSymbolReserved2(void)=0;
   virtual MTAPIRES  UltSymbolReserved3(void)=0;
   virtual MTAPIRES  UltSymbolReserved4(void)=0;
   //--- ultency translate configuration
   virtual IMTConUltTranslate* UltTranslateCreate(void)=0;
   virtual uint32_t  UltTranslateTotal(void)=0;
   virtual MTAPIRES  UltTranslateNext(const uint32_t pos,IMTConUltTranslate *config)=0;
   virtual MTAPIRES  UltTranslateReserved1(void)=0;
   virtual MTAPIRES  UltTranslateReserved2(void)=0;
   virtual MTAPIRES  UltTranslateReserved3(void)=0;
   virtual MTAPIRES  UltTranslateReserved4(void)=0;
   //--- Ultency deals database
   virtual IMTUltDeal* UltDealCreate(void)=0;
   virtual IMTUltDealArray* UltDealCreateArray(void)=0;
   virtual MTAPIRES  UltDealRequest(const uint64_t ultency_server,
                                    const uint64_t *logins,const uint32_t logins_total,
                                    const uint64_t *tickets,const uint32_t tickets_total,
                                    const uint64_t *tickets_liquidity,const uint32_t tickets_liquidity_total,
                                    const uint64_t *tickets_matching,const uint32_t tickets_matching_total,
                                    const int64_t from,const int64_t to,
                                    LPCWSTR symbol,
                                    IMTUltDealArray* deals)=0;
   virtual MTAPIRES  UltDealReserved1(void)=0;
   virtual MTAPIRES  UltDealReserved2(void)=0;
   virtual MTAPIRES  UltDealReserved3(void)=0;
   virtual MTAPIRES  UltDealReserved4(void)=0;
   //--- Ultency Liquidity Orders database
   virtual IMTUltLiquidityOrder* UltLiquidityOrderCreate(void)=0;
   virtual IMTUltLiquidityOrderArray* UltLiquidityOrderCreateArray(void)=0;
   virtual MTAPIRES  UltLiquidityOrderRequest(const uint64_t ultency_server,
                                              const uint64_t *logins,const uint32_t logins_total,
                                              const uint64_t *tickets,const uint32_t tickets_total,
                                              const uint64_t *tickets_matching,const uint32_t tickets_matching_total,
                                              LPCWSTR symbol,
                                              IMTUltLiquidityOrderArray* orders)=0;
   virtual MTAPIRES  UltLiquidityOrderReserved1(void)=0;
   virtual MTAPIRES  UltLiquidityOrderReserved2(void)=0;
   virtual MTAPIRES  UltLiquidityOrderReserved3(void)=0;
   virtual MTAPIRES  UltLiquidityOrderReserved4(void)=0;
   //--- Ultency Liquidity Orders History
   virtual MTAPIRES  UltLiquidityHistoryRequest(const uint64_t ultency_server,
                                                const uint64_t *logins,const uint32_t logins_total,
                                                const uint64_t *tickets,const uint32_t tickets_total,
                                                const uint64_t *tickets_matching,const uint32_t tickets_matching_total,
                                                const int64_t from,const int64_t to,
                                                LPCWSTR symbol,
                                                IMTUltLiquidityOrderArray* orders)=0;
   virtual MTAPIRES  UltLiquidityHistoryReserved1(void)=0;
   virtual MTAPIRES  UltLiquidityHistoryReserved2(void)=0;
   virtual MTAPIRES  UltLiquidityHistoryReserved3(void)=0;
   virtual MTAPIRES  UltLiquidityHistoryReserved4(void)=0;
   //--- Ultency Matching Orders database
   virtual IMTUltMatchingOrder* UltMatchingOrderCreate(void)=0;
   virtual IMTUltMatchingOrderArray* UltMatchingOrderCreateArray(void)=0;
   virtual MTAPIRES  UltMatchingOrderRequest(const uint64_t ultency_server,
                                             const uint64_t *logins,const uint32_t logins_total,
                                             const uint64_t *tickets,const uint32_t tickets_total,
                                             const uint64_t *tickets_source,const uint32_t tickets_source_total,
                                             LPCWSTR symbol,
                                             IMTUltMatchingOrderArray* orders)=0;
   virtual MTAPIRES  UltMatchingOrderReserved1(void)=0;
   virtual MTAPIRES  UltMatchingOrderReserved2(void)=0;
   virtual MTAPIRES  UltMatchingOrderReserved3(void)=0;
   virtual MTAPIRES  UltMatchingOrderReserved4(void)=0;
   //--- Ultency Matching Orders History
   virtual MTAPIRES  UltMatchingHistoryRequest(const uint64_t ultency_server,
                                               const uint64_t *logins,const uint32_t logins_total,
                                               const uint64_t *tickets,const uint32_t tickets_total,
                                               const uint64_t *tickets_source,const uint32_t tickets_source_total,
                                               const int64_t from,const int64_t to,
                                               LPCWSTR symbol,
                                               IMTUltMatchingOrderArray* orders)=0;
   virtual MTAPIRES  UltMatchingHistoryReserved1(void)=0;
   virtual MTAPIRES  UltMatchingHistoryReserved2(void)=0;
   virtual MTAPIRES  UltMatchingHistoryReserved3(void)=0;
   virtual MTAPIRES  UltMatchingHistoryReserved4(void)=0;
  };
//+------------------------------------------------------------------+
//| Report context interface                                         |
//+------------------------------------------------------------------+
class IMTReportContext
  {
public:
   virtual void      Release(void)=0;
   //--- report generation method
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api)=0;
   //--- explicit destructor is prohibited
protected:
                    ~IMTReportContext(void) {}
  };
//+------------------------------------------------------------------+
//| Report DLL entry points                                          |
//+------------------------------------------------------------------+
MTAPIENTRY MTAPIRES  MTReportAbout(const uint32_t index,MTReportInfo& info);
MTAPIENTRY MTAPIRES  MTReportCreate(const uint32_t index,const uint32_t apiversion,IMTReportContext **context);
//+------------------------------------------------------------------+
