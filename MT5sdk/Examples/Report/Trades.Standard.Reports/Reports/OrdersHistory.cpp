//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "OrdersHistory.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo COrdersHistory::s_info=
{
   100,
   MTReportAPIVersion,
   0,
   L"Orders History",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                    // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },3            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
ReportColumn COrdersHistory::s_columns[]=
  {
   //--- id,            name,         type,                                    width, width_max, offset,                  size,                             digits_column, flags
     { COLUMN_ORDER,    L"Order",     IMTDatasetColumn::TYPE_UINT64,           10, 0,   offsetof(TableRecord,order),      0,                                0,             IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_ID,         L"ID",        IMTDatasetColumn::TYPE_STRING,           10, 0,   offsetof(TableRecord,id),         MtFieldSize(TableRecord,id),      0,             0                              },
   { COLUMN_LOGIN,      L"Login",     IMTDatasetColumn::TYPE_USER_LOGIN,       10, 0,   offsetof(TableRecord,login),      0,                                0,             0,                             },
   { COLUMN_NAME,       L"Name",      IMTDatasetColumn::TYPE_STRING,           20, 0,   offsetof(TableRecord,name),       MtFieldSize(TableRecord,name),    0,             0,                             },
   { COLUMN_SETUP_TIME, L"Setup time",IMTDatasetColumn::TYPE_DATETIME_MSC,     18, 190, offsetof(TableRecord,time_setup), 0,                                0,             0,                             },
   { COLUMN_TYPE,       L"Type",      IMTDatasetColumn::TYPE_ORDER_TYPE,       10, 0,   offsetof(TableRecord,type),       0,                                0,             0,                             },
   { COLUMN_SYMBOL,     L"Symbol",    IMTDatasetColumn::TYPE_STRING,           10, 0,   offsetof(TableRecord,symbol),     MtFieldSize(TableRecord,symbol),  0,             0,                             },
   { COLUMN_VOLUME,     L"Volume",    IMTDatasetColumn::TYPE_VOLUME_ORDER_EXT, 20, 0,   offsetof(TableRecord,volume),     0,                                0,             0,                             },
   { COLUMN_SETUP_PRICE,L"Price",     IMTDatasetColumn::TYPE_PRICE,            10, 0,   offsetof(TableRecord,price_setup),0,                                COLUMN_DIGITS, 0,                             },
   { COLUMN_SL,         L"S/L",       IMTDatasetColumn::TYPE_PRICE,            10, 0,   offsetof(TableRecord,sl),         0,                                COLUMN_DIGITS, 0,                             },
   { COLUMN_TP,         L"T/P",       IMTDatasetColumn::TYPE_PRICE,            10, 0,   offsetof(TableRecord,tp),         0,                                COLUMN_DIGITS, 0,                             },
   { COLUMN_DONE_TIME,  L"Done time", IMTDatasetColumn::TYPE_DATETIME_MSC,     18, 190, offsetof(TableRecord,time_done),  0,                                0,             0,                             },
   { COLUMN_DONE_PRICE, L"Price",     IMTDatasetColumn::TYPE_PRICE,            10, 0,   offsetof(TableRecord,price_done), 0,                                COLUMN_DIGITS, 0,                             },
   { COLUMN_REASON,     L"Reason",    IMTDatasetColumn::TYPE_ORDER_TYPE_REASON,10, 0,   offsetof(TableRecord,reason),     0,                                0,             0,                             },
   { COLUMN_STATE,      L"State",     IMTDatasetColumn::TYPE_ORDER_STATUS,     10, 0,   offsetof(TableRecord,state),      0,                                0,             0,                             },
   { COLUMN_COMMENT,    L"Comment",   IMTDatasetColumn::TYPE_STRING,           20, 0,   offsetof(TableRecord,comment),    MtFieldSize(TableRecord,comment), 0,             0,                             },
   { COLUMN_DIGITS,     L"Digits",    IMTDatasetColumn::TYPE_UINT32,           10, 0,   offsetof(TableRecord,digits),     0,                                0,             IMTDatasetColumn::FLAG_HIDDEN  },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
COrdersHistory::COrdersHistory(void) : m_api(NULL),m_orders(NULL),
                                       m_user(NULL),m_group(NULL)
  {
   ZeroMemory(&m_summary,sizeof(m_summary));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
COrdersHistory::~COrdersHistory(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void COrdersHistory::Clear(void)
  {
//--- group config interface
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- user interface
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- orders interface
   if(m_orders)
     {
      m_orders->Release();
      m_orders=NULL;
     }
//--- summary
   ZeroMemory(&m_summary,sizeof(m_summary));
//--- api
   m_api=NULL;
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void COrdersHistory::Info(MTReportInfo& info)
  {
//--- get info information  
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void COrdersHistory::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES COrdersHistory::Generate(const uint32_t type,IMTReportAPI *api)
  {
   uint64_t  *logins=NULL;
   uint32_t logins_total=0;
   MTAPIRES res;
//--- clear
   Clear();
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(type!=MTReportInfo::TYPE_TABLE)
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- save API pointer
   m_api=api;
//--- prepare table
   if((res=TablePrepare())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- get report logins
   if((res=api->ParamLogins(logins,logins_total))!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- checks logins
   if(logins && logins_total)
     {
      //--- prepare deals array & user & group
      if((m_orders=api->OrderCreateArray())==NULL ||
         (m_user  =api->UserCreate())      ==NULL ||
         (m_group =api->GroupCreate())     ==NULL)
        {
         api->Free(logins);
         Clear();
         return(MT_RET_ERR_MEM);
        }
      //--- write table
      for(uint32_t i=0;i<logins_total;i++)
         if((res=TableWrite(logins[i]))!=MT_RET_OK)
           {
            api->Free(logins);
            Clear();
            return(res);
           }
     }
//--- free logins
   if(logins)
      api->Free(logins);
//--- write table summary
   if((res=TableWriteSummary())!=MT_RET_OK)
     {
      Clear();
      return(res);
     }
//--- clear 
   Clear();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare table                                                    |
//+------------------------------------------------------------------+
MTAPIRES COrdersHistory::TablePrepare(void)
  {
   IMTDatasetColumn *column;
   MTAPIRES         res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create column
   if((column=m_api->TableColumnCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- clear table columns
   m_api->TableColumnClear();
//--- add column description for TableRecord
   for(uint32_t i=0;i<_countof(s_columns);i++)
     {
      //--- clear
      column->Clear();
      //--- fill info
      column->ColumnID(s_columns[i].id);
      column->Name(s_columns[i].name);
      column->Type(s_columns[i].type);
      column->Width(s_columns[i].width);
      column->WidthMax(s_columns[i].width_max);
      column->Offset(s_columns[i].offset);
      column->Size(s_columns[i].size);
      column->DigitsColumn(s_columns[i].digits_column);
      column->Flags(s_columns[i].flags);
      //--- add column
      if((res=m_api->TableColumnAdd(column))!=MT_RET_OK)
        {
         column->Release();
         return(res);
        }
     }
//--- release column
   column->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write table for specified login                                  |
//+------------------------------------------------------------------+
MTAPIRES COrdersHistory::TableWrite(const uint64_t login)
  {
   IMTOrder   *order=NULL;
   uint32_t    order_total,i;
   TableRecord record={0};
   MTAPIRES    retcode;
//--- checks
   if(!m_api || !login || !m_orders || !m_user || !m_group)
      return(MT_RET_ERR_PARAMS);
//--- get orders
   if((retcode=m_api->HistoryGet(login,m_api->ParamFrom(),m_api->ParamTo(),m_orders))!=MT_RET_OK)
      return(retcode);
//--- nothing to do?
   if(!m_orders->Total())
      return(MT_RET_OK);
//--- get user info
   if((retcode=m_api->UserGetLight(login,m_user))!=MT_RET_OK)
      return(retcode);
//--- get user group info
   if((retcode=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(retcode);
//---
   for(i=0,order_total=m_orders->Total();i<order_total;i++)
     {
      //--- get order
      if((order=m_orders->Next(i))==NULL)
         continue;
      //--- clear record
      ZeroMemory(&record,sizeof(record));
      //--- fill record
      record.order          =order->Order();
      CMTStr::Copy(record.id,  order->ExternalID());
      record.login          =order->Login();
      CMTStr::Copy(record.name,    m_user->Name());
      record.time_setup     =order->TimeSetupMsc();
      record.type           =order->Type();
      CMTStr::Copy(record.symbol,  order->Symbol());
      record.volume[0]      =order->VolumeInitialExt();
      record.volume[1]      =order->VolumeCurrentExt();
      record.price_setup    =order->PriceOrder();
      record.sl             =order->PriceSL();
      record.tp             =order->PriceTP();
      record.time_done      =order->TimeDoneMsc();
      record.price_done     =order->PriceCurrent();
      record.reason         =order->Reason();
      record.state          =order->State();
      CMTStr::Copy(record.comment, order->Comment());
      record.digits         =order->Digits();
      //--- write row
      if((retcode=m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK)
         return(retcode);
      //--- update summary
      m_summary.volume[0]+=order->VolumeInitialExt();
      m_summary.volume[1]+=order->VolumeCurrentExt();
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES COrdersHistory::TableWriteSummary(void)
  {
   IMTDatasetSummary *summary=NULL;
   MTAPIRES          res;
//--- checks
   if(!m_api)
      return(MT_RET_ERR_MEM);
//--- no records?
   if(m_api->TableRowTotal()==0)
      return(MT_RET_OK);
//--- create summary
   if((summary=m_api->TableSummaryCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- clear summary
   summary->Clear();
//--- total text
   summary->ColumnID(COLUMN_ORDER);
   summary->MergeColumn(COLUMN_SYMBOL);
   summary->Line(0);
   summary->ValueString(L"Total");
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
   summary->Line(0);
   summary->ValueVolumeExt(m_summary.volume[0],m_summary.volume[1]);
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
