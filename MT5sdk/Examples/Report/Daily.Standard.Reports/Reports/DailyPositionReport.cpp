//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyPositionReport.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CDailyPositionReport::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Daily Positions",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   0,
   MTReportInfo::TYPE_TABLE,
   L"Daily",
                     // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, DEFAULT_GROUPS },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM },
     },2             // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
const ReportColumn CDailyPositionReport::s_columns[]=
   //--- id,                name,              type,                                     width, width_max, offset,                      size,                          digits_column,          flags
  {{ COLUMN_POSITION       ,L"Position",       IMTDatasetColumn::TYPE_UINT64,            10,   0, offsetof(TableRecord,position),       0,                             0,            IMTDatasetColumn::FLAG_PRIMARY|IMTDatasetColumn::FLAG_LEFT },
   { COLUMN_ID             ,L"ID",             IMTDatasetColumn::TYPE_STRING,            10,   0, offsetof(TableRecord,id),             MtFieldSize(TableRecord,id) },
   { COLUMN_LOGIN          ,L"Login",          IMTDatasetColumn::TYPE_USER_LOGIN,        10,   0, offsetof(TableRecord,login),          0,                             0,            IMTDatasetColumn::FLAG_SORT_DEFAULT },
   { COLUMN_NAME           ,L"Name",           IMTDatasetColumn::TYPE_STRING,            20,   0, offsetof(TableRecord,name),           MtFieldSize(TableRecord,name), },
   { COLUMN_TIME_REPORT    ,L"Report Time",    IMTDatasetColumn::TYPE_DATETIME,          18, 190, offsetof(TableRecord,datetime)        },
   { COLUMN_TIME_CREATE    ,L"Creation Time",  IMTDatasetColumn::TYPE_DATETIME_MSC,      18, 190, offsetof(TableRecord,time_create)     },
   { COLUMN_ACTION         ,L"Action",         IMTDatasetColumn::TYPE_DEAL_ACTION,       10,   0, offsetof(TableRecord,action)          },
   { COLUMN_SYMBOL         ,L"Symbol",         IMTDatasetColumn::TYPE_STRING,            10,   0, offsetof(TableRecord,symbol),         MtFieldSize(TableRecord,symbol) },
   { COLUMN_VOLUME         ,L"Volume",         IMTDatasetColumn::TYPE_VOLUME_EXT,        10,   0, offsetof(TableRecord,volume)          },
   { COLUMN_PRICE_OPEN     ,L"Open Price",     IMTDatasetColumn::TYPE_PRICE_POSITION,    10,   0, offsetof(TableRecord,price_open),     0,                             COLUMN_DIGITS },
   { COLUMN_PRICE_SL       ,L"S / L",          IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_sl),       0,                             COLUMN_DIGITS },
   { COLUMN_PRICE_TP       ,L"T / P",          IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_tp),       0,                             COLUMN_DIGITS },
   { COLUMN_PRICE_CURRENT  ,L"Current Price",  IMTDatasetColumn::TYPE_PRICE,             10,   0, offsetof(TableRecord,price_current),  0,                             COLUMN_DIGITS },
   { COLUMN_STORAGE        ,L"Swap",           IMTDatasetColumn::TYPE_MONEY,             10,   0, offsetof(TableRecord,storage),        0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_PROFIT         ,L"Profit",         IMTDatasetColumn::TYPE_MONEY,             10,   0, offsetof(TableRecord,profit),         0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_CURRENCY,       L"Currency",       IMTDatasetColumn::TYPE_STRING,             9,  70, offsetof(TableRecord,currency),       MtFieldSize(TableRecord,currency) },
   { COLUMN_REASON         ,L"Reason",         IMTDatasetColumn::TYPE_ORDER_TYPE_REASON, 10,   0, offsetof(TableRecord,reason)          },
   { COLUMN_COMMENT        ,L"Comment",        IMTDatasetColumn::TYPE_STRING,            20,   0, offsetof(TableRecord,comment),        MtFieldSize(TableRecord,comment) },
   { COLUMN_DIGITS         ,L"Digits",         IMTDatasetColumn::TYPE_UINT32,             0,   0, offsetof(TableRecord,digits),         0,                             0,            IMTDatasetColumn::FLAG_HIDDEN },
   { COLUMN_DIGITS_CURRENCY,L"Currency Digits",IMTDatasetColumn::TYPE_UINT32,             0,   0, offsetof(TableRecord,currency_digits),0,                             0,            IMTDatasetColumn::FLAG_HIDDEN }
  };
//--- User request fields descriptions
const DatasetField CDailyPositionReport::s_daily_fields[]=
   //--- id                                      , select, offset                     , size
  {{ IMTDatasetField::FIELD_DAILY_DATE_TIME      , true, offsetof(UserRecord,datetime)},
   { IMTDatasetField::FIELD_DAILY_LOGIN          , true, offsetof(UserRecord,login)   },
   { IMTDatasetField::FIELD_DAILY_NAME           , true, offsetof(UserRecord,name)    , MtFieldSize(UserRecord,name)},
   { IMTDatasetField::FIELD_DAILY_CURRENCY       , true, offsetof(UserRecord,currency), MtFieldSize(UserRecord,currency)},
   { IMTDatasetField::FIELD_DAILY_DIGITS_CURRENCY, true, offsetof(UserRecord,currency_digits) },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyPositionReport::CDailyPositionReport(void) :
   CReportBase(s_info),
   m_ctm(0),m_report_logins(nullptr),m_daily(nullptr),m_daily_position(nullptr),m_record{},m_user(m_record),m_position(m_record),m_summary(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyPositionReport::~CDailyPositionReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDailyPositionReport::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDailyPositionReport::Clear(void)
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
   if(m_daily_position)
     {
      m_daily_position->Release();
      m_daily_position=nullptr;
     }
   m_daily_select.Clear();
//--- clear records
   ZeroMemory(&m_record,sizeof(m_record));
//--- clear summaries
   m_summaries.Clear();
   m_summary=nullptr;
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::Prepare(void)
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
      !(m_daily_position=m_api->PositionCreate()))
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
MTAPIRES CDailyPositionReport::Write(void)
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
MTAPIRES CDailyPositionReport::WriteLogins(void)
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
      //--- write users positions
      res=WriteUsersPositions(*users,login);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.Error(res,L"Accounts processing"));
      //--- clear dataset rows
      if((res=users->RowClear())!=MT_RET_OK)
         return(res);
     }
   return(res);
  }
//+------------------------------------------------------------------+
//| Write users positions                                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::WriteUsersPositions(IMTDataset &users,uint64_t &login_last)
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
      //--- write user positions
      res=WritePositions();
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.Error(res,L"Positions processing with time"));
     }
//--- result
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write positions                                                  |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::WritePositions(void)
  {
//--- checks
   if(!m_api || !m_daily || !m_daily_position)
      return(MT_RET_ERR_PARAMS);
//--- get daily report
   MTAPIRES res=m_api->DailyGet(m_record.login,m_record.datetime,m_daily);
   if(res!=MT_RET_OK)
      return(res);
//--- positions
   const uint32_t total=m_daily->PositionTotal();
   if(!total)
      return(MT_RET_OK);
//--- initialize summary for currency
   if((res=SummaryInit())!=MT_RET_OK)
      return(res);
//--- iterate daily positions
   for(uint32_t i=0;i<total;i++)
     {
      //--- daily position
      if((res=m_daily->PositionNext(i,m_daily_position))!=MT_RET_OK)
         return(res);
      //--- write position
      if((res=WritePosition())!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write position                                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::WritePosition(void)
  {
   if(!m_api || !m_summary)
      return(MT_RET_ERR_PARAMS);
//--- fill position
   MTAPIRES res=FillPosition();
   if(res!=MT_RET_OK)
      return(res);
//--- write record
   if((res=m_api->TableRowWrite(&m_record,sizeof(m_record)))!=MT_RET_OK)
      return(res);
//--- add position to summary
   SummaryAdd(*m_summary);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Fill position                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::FillPosition(void)
  {
   if(!m_daily_position)
      return(MT_RET_ERR_PARAMS);
//--- fill position
   m_position.position     =m_daily_position->Position();
   CMTStr::Copy(m_position.id,m_daily_position->ExternalID());
   m_position.time_create  =m_daily_position->TimeCreateMsc();
   m_position.action       =m_daily_position->Action();
   CMTStr::Copy(m_position.symbol,m_daily_position->Symbol());
   m_position.volume       =m_daily_position->VolumeExt();
   m_position.price_open   =m_daily_position->PriceOpen();
   m_position.price_sl     =m_daily_position->PriceSL();
   m_position.price_tp     =m_daily_position->PriceTP();
   m_position.price_current=m_daily_position->PriceCurrent();
   m_position.storage      =m_daily_position->Storage();
   m_position.profit       =m_daily_position->Profit();
   m_position.reason       =m_daily_position->Reason();
   CMTStr::Copy(m_position.comment,m_daily_position->Comment());
   m_position.digits       =m_daily_position->Digits();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Initialize summary                                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::SummaryInit(void)
  {
//---- check summary
   if(m_summary && !CMTStr::Compare(m_summary->currency,m_record.currency))
      return(MT_RET_OK);
//--- prepare summary
   TableRecord summary{};
   CMTStr::Copy(summary.currency,m_record.currency);
//--- search summary
   m_summary=m_summaries.Search(&summary,SortSummary);
   if(!m_summary)
     {
      //--- insert new summary
      summary.currency_digits=m_record.currency_digits;
      m_summary=m_summaries.Insert(&summary,SortSummary);
      if(!m_summary)
         return(MT_RET_ERR_MEM);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Add deal to summary                                              |
//+------------------------------------------------------------------+
void CDailyPositionReport::SummaryAdd(TableRecord &summary)
  {
//--- update summary
   summary.storage=SMTMath::MoneyAdd(summary.storage,m_position.storage,summary.currency_digits);
   summary.profit =SMTMath::MoneyAdd(summary.profit, m_position.profit, summary.currency_digits);
   summary.volume+=m_position.volume;
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDailyPositionReport::SummaryWrite(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create summary
   IMTDatasetSummary *summary=m_api->TableSummaryCreate();
   if(!summary)
      return(MT_RET_ERR_MEM);
//--- iterate all summaries
   MTAPIRES res;
   uint32_t line=0;
   for(const TableRecord *record=m_summaries.First();record;record=m_summaries.Next(record))
     {
      //--- check for not empty
      if(!record->volume || !*record->currency)
         continue;
      //--- clear summary
      summary->Clear();
      //--- total text
      summary->ColumnID(COLUMN_POSITION);
      summary->MergeColumn(COLUMN_SYMBOL);
      summary->Line(line);
      summary->ValueString(line==0 ? L"Total" : L"");
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- total amount
      summary->ColumnID(COLUMN_STORAGE);
      summary->Line(line);
      summary->ValueMoney(record->storage);
      summary->Digits(record->currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- total amount
      summary->ColumnID(COLUMN_PROFIT);
      summary->Line(line);
      summary->ValueMoney(record->profit);
      summary->Digits(record->currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- total amount
      summary->ColumnID(COLUMN_VOLUME);
      summary->Line(line);
      summary->ValueVolumeExt(record->volume);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- currency
      summary->ColumnID(COLUMN_CURRENCY);
      summary->Line(line);
      summary->ValueString(record->currency);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      line++;
     }
//--- release summary
   summary->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Sort summary                                                     |
//+------------------------------------------------------------------+
int32_t CDailyPositionReport::SortSummary(const void *left,const void *right)
  {
   const TableRecord *lft=(const TableRecord*)left;
   const TableRecord *rgh=(const TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
