//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyOrderReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CDailyOrderReport::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Daily Orders",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   0,
   MTReportInfo::TYPE_TABLE,
   L"Daily",
                     // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, DEFAULT_GROUPS },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM },
     },2,            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
const ReportColumn CDailyOrderReport::s_columns[]=
   //--- id,              name,            type,                                     width, width_max, offset,                     size,                          digits_column,          flags
  {{ COLUMN_ORDER        ,L"Order",        IMTDatasetColumn::TYPE_UINT64,            10,   0, offsetof(TableRecord,order),         0,                             0,            IMTDatasetColumn::FLAG_PRIMARY|IMTDatasetColumn::FLAG_LEFT },
   { COLUMN_ID           ,L"ID",           IMTDatasetColumn::TYPE_STRING,            10,   0, offsetof(TableRecord,id),            MtFieldSize(TableRecord,id) },
   { COLUMN_LOGIN        ,L"Login",        IMTDatasetColumn::TYPE_USER_LOGIN,        10,   0, offsetof(TableRecord,login),         0,                             0,            IMTDatasetColumn::FLAG_SORT_DEFAULT },
   { COLUMN_NAME         ,L"Name",         IMTDatasetColumn::TYPE_STRING,            20,   0, offsetof(TableRecord,name),          MtFieldSize(TableRecord,name) },
   { COLUMN_TIME_REPORT  ,L"Report Time",  IMTDatasetColumn::TYPE_DATETIME,          18, 190, offsetof(TableRecord,datetime)       },
   { COLUMN_TIME_SETUP   ,L"Setup Time",   IMTDatasetColumn::TYPE_DATETIME_MSC,      18, 190, offsetof(TableRecord,time_setup)     },
   { COLUMN_TYPE         ,L"Type",         IMTDatasetColumn::TYPE_ORDER_TYPE,        10,   0, offsetof(TableRecord,type)           },
   { COLUMN_SYMBOL       ,L"Symbol",       IMTDatasetColumn::TYPE_STRING,            10,   0, offsetof(TableRecord,symbol),        MtFieldSize(TableRecord,symbol) },
   { COLUMN_VOLUME       ,L"Volume",       IMTDatasetColumn::TYPE_VOLUME_ORDER_EXT,  12,   0, offsetof(TableRecord,volume_initial) },
   { COLUMN_PRICE_ORDER  ,L"Price",        IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_order),   0,                             COLUMN_DIGITS },
   { COLUMN_PRICE_SL     ,L"S / L",        IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_sl),      0,                             COLUMN_DIGITS },
   { COLUMN_PRICE_TP     ,L"T / P",        IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_tp),      0,                             COLUMN_DIGITS },
   { COLUMN_PRICE_CURRENT,L"Current Price",IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_current), 0,                             COLUMN_DIGITS },
   { COLUMN_TYPE_REASON  ,L"Reason",       IMTDatasetColumn::TYPE_ORDER_TYPE_REASON, 10,   0, offsetof(TableRecord,type_reason)    },
   { COLUMN_STATE        ,L"State",        IMTDatasetColumn::TYPE_ORDER_STATUS,      10,   0, offsetof(TableRecord,state)          },
   { COLUMN_COMMENT      ,L"Comment",      IMTDatasetColumn::TYPE_STRING,            20,   0, offsetof(TableRecord,comment),       MtFieldSize(TableRecord,comment) },
   { COLUMN_DIGITS       ,L"Digits",       IMTDatasetColumn::TYPE_UINT32,             0,   0, offsetof(TableRecord,digits),        0,                             0,            IMTDatasetColumn::FLAG_HIDDEN }
  };
//--- User request fields descriptions
const DatasetField CDailyOrderReport::s_daily_fields[]=
   //--- id                                  , select, offset                      , size
  {{ IMTDatasetField::FIELD_DAILY_DATE_TIME  , true, offsetof(UserRecord,datetime)},
   { IMTDatasetField::FIELD_DAILY_LOGIN      , true, offsetof(UserRecord,login)   },
   { IMTDatasetField::FIELD_DAILY_NAME       , true, offsetof(UserRecord,name)    , MtFieldSize(UserRecord,name)},
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyOrderReport::CDailyOrderReport(void) :
   CReportBase(s_info),
   m_ctm(0),m_report_logins(nullptr),m_daily(nullptr),m_daily_order(nullptr),m_record{},m_user(m_record),m_order(m_record),m_summary{}
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyOrderReport::~CDailyOrderReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDailyOrderReport::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDailyOrderReport::Clear(void)
  {
//--- reset interval
   m_ctm=0;
//--- interfaces
   if(m_report_logins)
     {
      m_report_logins->Release();
      m_report_logins=nullptr;
     }
   if(m_daily)
     {
      m_daily->Release();
      m_daily=nullptr;
     }
   if(m_daily_order)
     {
      m_daily_order->Release();
      m_daily_order=nullptr;
     }
   m_daily_select.Clear();
//--- clear records
   ZeroMemory(&m_record,sizeof(m_record));
//--- clear summaries
   ZeroMemory(&m_summary,sizeof(m_summary));
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- report interval
   m_ctm=SMTTime::DayBegin(m_api->ParamFrom());
   if(m_ctm<0)
      return(MT_RET_ERR_PARAMS);
//--- daily report
   if(!(m_report_logins=m_api->KeySetCreate()) ||
      !(m_daily=m_api->DailyCreate()) ||
      !(m_daily_order=m_api->OrderCreate()))
      return(MT_RET_ERR_MEM);
//--- prepare daily request
   MTAPIRES res=m_daily_select.Prepare(m_api,s_daily_fields,_countof(s_daily_fields),s_daily_request_limit);
   if(res!=MT_RET_OK)
      return(res);
//--- prepare table
   return(ReportColumn::ReportTablePrepare(*m_api,s_columns,_countof(s_columns)));
  }
//+------------------------------------------------------------------+
//| Report writing                                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::Write(void)
  {
//--- checks
   if(!m_api || !m_report_logins)
      return(MT_RET_ERR_PARAMS);
//--- get report request logins
   MTAPIRES res=m_api->KeySetParamLogins(m_report_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- write users by logins
   if((res=WriteLogins())!=MT_RET_OK)
      return(res);
//--- write summary
   if((res=SummaryWrite())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write users by logins                                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::WriteLogins(void)
  {
//--- checks
   if(!m_api || !m_report_logins)
      return(MT_RET_ERR_PARAMS);
//--- users total
   const uint32_t total=m_report_logins->Total();
   if(!total)
      return(MT_RET_OK);
//--- select users
   MTAPIRES res=m_daily_select.Select(m_ctm,m_ctm+SECONDS_IN_DAY-1,m_report_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- select users part by part
   for(uint64_t login=0;IMTDataset *users=m_daily_select.Next(res,login);login++)
     {
      //--- write users orders
      res=WriteUsersOrders(*users,login);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.Error(res,L"Accounts processing"));
      //--- clear dataset rows
      if((res=users->RowClear())!=MT_RET_OK)
         return(res);
     }
   return(res);
  }
//+------------------------------------------------------------------+
//| Write users orders                                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::WriteUsersOrders(IMTDataset &users,uint64_t &login_last)
  {
//--- iterate users
   for(uint32_t i=0,total=users.RowTotal();i<total;i++)
     {
      //--- read user
      MTAPIRES res=users.RowRead(i,&m_user,sizeof(m_user));
      if(res!=MT_RET_OK)
         return(res);
      //--- store last user login
      if(login_last<m_user.login)
         login_last=m_user.login;
      //--- write user orders
      res=WriteOrders();
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.Error(res,L"Orders processing with time"));
     }
//--- result
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write orders                                                     |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::WriteOrders(void)
  {
//--- checks
   if(!m_api || !m_daily || !m_daily_order)
      return(MT_RET_ERR_PARAMS);
//--- get daily report
   MTAPIRES res=m_api->DailyGet(m_record.login,m_record.datetime,m_daily);
   if(res!=MT_RET_OK)
      return(res);
//--- orders
   const uint32_t total=m_daily->OrderTotal();
   if(!total)
      return(MT_RET_OK);
//--- iterate daily orders
   for(uint32_t i=0;i<total;i++)
     {
      //--- daily order
      if((res=m_daily->OrderNext(i,m_daily_order))!=MT_RET_OK)
         return(res);
      //--- write order
      if((res=WriteOrder())!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write order                                                      |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::WriteOrder(void)
  {
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- fill order
   MTAPIRES res=FillOrder();
   if(res!=MT_RET_OK)
      return(res);
//--- write record
   if((res=m_api->TableRowWrite(&m_record,sizeof(m_record)))!=MT_RET_OK)
      return(res);
//--- add order to summary
   SummaryAdd();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill order                                                       |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::FillOrder(void)
  {
   if(!m_daily_order)
      return(MT_RET_ERR_PARAMS);
//--- fill order
   m_order.order         =m_daily_order->Order();
   CMTStr::Copy(m_order.id,m_daily_order->ExternalID());
   m_order.time_setup    =m_daily_order->TimeSetupMsc();
   m_order.type          =m_daily_order->Type();
   CMTStr::Copy(m_order.symbol,m_daily_order->Symbol());
   m_order.volume_initial=m_daily_order->VolumeInitialExt();
   m_order.volume_current=m_daily_order->VolumeCurrentExt();
   m_order.price_order   =m_daily_order->PriceOrder();
   m_order.price_sl      =m_daily_order->PriceSL();
   m_order.price_tp      =m_daily_order->PriceTP();
   m_order.price_current =m_daily_order->PriceCurrent();
   m_order.type_reason   =m_daily_order->Reason();
   m_order.state         =m_daily_order->State();
   CMTStr::Copy(m_order.comment,m_daily_order->Comment());
   m_order.digits        =m_daily_order->Digits();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add order to summary                                             |
//+------------------------------------------------------------------+
void CDailyOrderReport::SummaryAdd(void)
  {
//--- add order to summary
   m_summary.volume_initial+=m_order.volume_initial;
   m_summary.volume_current+=m_order.volume_current;
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDailyOrderReport::SummaryWrite(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_MEM);
//--- no records?
   if(!m_api->TableRowTotal())
      return(MT_RET_OK);
//--- create summary
   IMTDatasetSummary *summary=m_api->TableSummaryCreate();
   if(!summary)
      return(MT_RET_ERR_MEM);
//--- clear summary
   summary->Clear();
//--- total text
   summary->ColumnID(COLUMN_ORDER);
   summary->MergeColumn(COLUMN_SYMBOL);
   summary->Line(0);
   summary->ValueString(L"Total");
//--- add summary
   MTAPIRES res=m_api->TableSummaryAdd(summary);
   if(res!=MT_RET_OK)
     {
      summary->Release();
      return(res);
     }
//--- clear summary
   summary->Clear();
//--- total amount
   summary->ColumnID(COLUMN_VOLUME);
   summary->Line(0);
   summary->ValueVolumeExt(m_summary.volume_initial,m_summary.volume_current);
//--- add summary
   if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
     {
      summary->Release();
      return(res);
     }
//--- release summary
   summary->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
