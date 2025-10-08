//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DealsHistory.h"
#include "..\Tools\ReportTimer.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CDealsHistory::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Deals History",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NO_GROUPS_LOGINS,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                    // params
     {{ MTReportParam::TYPE_GROUPS,  MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,    MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,    MTAPI_PARAM_TO           },
     },3            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
const ReportColumn CDealsHistory::s_columns[]=
   //--- id,                name,             type,                                    width, width_max, offset,                       size,                              digits_column,          flags
  {{ COLUMN_DEAL,           L"Deal",          IMTDatasetColumn::TYPE_UINT64,           10, 0,   offsetof(TableRecord,deal),            0,                                 0,                      IMTDatasetColumn::FLAG_PRIMARY|IMTDatasetColumn::FLAG_LEFT },
   { COLUMN_ID,             L"ID",            IMTDatasetColumn::TYPE_STRING,           10, 0,   offsetof(TableRecord,id),              MtFieldSize(TableRecord,id),       0,                      0                                   },
   { COLUMN_ORDER,          L"Order",         IMTDatasetColumn::TYPE_UINT64,           10, 0,   offsetof(TableRecord,order),           0,                                 0,                      0                                   },
   { COLUMN_POSITION,       L"Position",      IMTDatasetColumn::TYPE_UINT64,           10, 0,   offsetof(TableRecord,position),        0,                                 0,                      0                                   },
   { COLUMN_LOGIN,          L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN,       10, 0,   offsetof(TableRecord,login),           0,                                 0,                      IMTDatasetColumn::FLAG_SORT_DEFAULT },
   { COLUMN_NAME,           L"Name",          IMTDatasetColumn::TYPE_STRING,           20, 0,   offsetof(TableRecord,name),            MtFieldSize(TableRecord,name),     0,                      0                                   },
   { COLUMN_TIME,           L"Time",          IMTDatasetColumn::TYPE_DATETIME_MSC,     18, 190, offsetof(TableRecord,time_create),     0,                                 0,                      0                                   },
   { COLUMN_TYPE,           L"Type",          IMTDatasetColumn::TYPE_DEAL_ACTION,      10, 100, offsetof(TableRecord,action),          0,                                 0,                      0                                   },
   { COLUMN_ENTRY,          L"Entry",         IMTDatasetColumn::TYPE_DEAL_ENTRY ,      10, 0,   offsetof(TableRecord,entry),           0,                                 0,                      0                                   },
   { COLUMN_SYMBOL,         L"Symbol",        IMTDatasetColumn::TYPE_STRING,           10, 0,   offsetof(TableRecord,symbol),          MtFieldSize(TableRecord,symbol),   0,                      0                                   },
   { COLUMN_VOLUME,         L"Volume",        IMTDatasetColumn::TYPE_VOLUME_EXT,       8,  0,   offsetof(TableRecord,volume),          0,                                 0,                      0                                   },
   { COLUMN_PRICE,          L"Price",         IMTDatasetColumn::TYPE_PRICE,            8,  0,   offsetof(TableRecord,price),           0,                                 COLUMN_DIGITS,          0                                   },
   { COLUMN_SL,             L"S / L",         IMTDatasetColumn::TYPE_PRICE,            8,  0,   offsetof(TableRecord,sl),              0,                                 COLUMN_DIGITS,          0                                   },
   { COLUMN_TP,             L"T / P",         IMTDatasetColumn::TYPE_PRICE,            8,  0,   offsetof(TableRecord,tp),              0,                                 COLUMN_DIGITS,          0                                   },
   { COLUMN_REASON,         L"Reason",        IMTDatasetColumn::TYPE_ORDER_TYPE_REASON,10, 0,   offsetof(TableRecord,reason),          0,                                 0,                      0                                   },
   { COLUMN_COMMISSION,     L"Commission",    IMTDatasetColumn::TYPE_MONEY,            8,  0,   offsetof(TableRecord,commission),      0,                                 COLUMN_CURRENCY_DIGITS, 0                                   },
   { COLUMN_FEE,            L"Fee",           IMTDatasetColumn::TYPE_MONEY,            8,  0,   offsetof(TableRecord,fee),             0,                                 COLUMN_CURRENCY_DIGITS, 0                                   },
   { COLUMN_STORAGE,        L"Swap",          IMTDatasetColumn::TYPE_MONEY,            8,  0,   offsetof(TableRecord,storage),         0,                                 COLUMN_CURRENCY_DIGITS, 0                                   },
   { COLUMN_PROFIT,         L"Profit",        IMTDatasetColumn::TYPE_MONEY,            10, 0,   offsetof(TableRecord,profit),          0,                                 COLUMN_CURRENCY_DIGITS, 0                                   },
   { COLUMN_DEALER,         L"Dealer",        IMTDatasetColumn::TYPE_USER_LOGIN,       5,  0,   offsetof(TableRecord,dealer),          0,                                 0,                      0                                   },
   { COLUMN_CURRENCY,       L"Currency",      IMTDatasetColumn::TYPE_STRING,           9,  70,  offsetof(TableRecord,currency),        MtFieldSize(TableRecord,currency), 0,                      0                                   },
   { COLUMN_COMMENT,        L"Comment",       IMTDatasetColumn::TYPE_STRING,           20, 0,   offsetof(TableRecord,comment),         MtFieldSize(TableRecord,comment),  0,                      0                                   },
   { COLUMN_DIGITS,         L"Digits",        IMTDatasetColumn::TYPE_UINT32,           0,  0,   offsetof(TableRecord,digits),          0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN       },
   { COLUMN_CURRENCY_DIGITS,L"CurrencyDigits",IMTDatasetColumn::TYPE_UINT32,           0,  0,   offsetof(TableRecord,currency_digits), 0,                                 0,                      IMTDatasetColumn::FLAG_HIDDEN       }
  };
//--- User request fields descriptions
const DatasetField CDealsHistory::s_user_fields[]=
   //--- id                                   , select, offset                        , size
  {{ IMTDatasetField::FIELD_USER_LOGIN        , true, offsetof(UserRecord,login)      },
   { IMTDatasetField::FIELD_USER_NAME         , true, offsetof(UserRecord,name)       , MtFieldSize(UserRecord,name)},
   { IMTDatasetField::FIELD_USER_GROUP        , true, offsetof(UserRecord,group)      , MtFieldSize(UserRecord,group)},
  };
//--- Deal request fields descriptions
const DatasetField CDealsHistory::s_deal_fields[]=
   //--- id                                   , select , offset
  {{ IMTDatasetField::FIELD_DEAL_DEAL         , true, offsetof(DealRecord,deal)       },
   { IMTDatasetField::FIELD_DEAL_EXTERNAL_ID  , true, offsetof(DealRecord,id)         , MtFieldSize(DealRecord,id) },
   { IMTDatasetField::FIELD_DEAL_ORDER        , true, offsetof(DealRecord,order)      },
   { IMTDatasetField::FIELD_DEAL_POSITION_ID  , true, offsetof(DealRecord,position)   },
   { IMTDatasetField::FIELD_DEAL_LOGIN        , true, offsetof(DealRecord,login)      },
   { IMTDatasetField::FIELD_DEAL_TIME         },
   { IMTDatasetField::FIELD_DEAL_TIME_MSC     , true, offsetof(DealRecord,time_create)},
   { IMTDatasetField::FIELD_DEAL_ACTION       , true, offsetof(DealRecord,action)     },
   { IMTDatasetField::FIELD_DEAL_ENTRY        , true, offsetof(DealRecord,entry)      },
   { IMTDatasetField::FIELD_DEAL_SYMBOL       , true, offsetof(DealRecord,symbol)     , MtFieldSize(DealRecord,symbol) },
   { IMTDatasetField::FIELD_DEAL_VOLUME_EXT   , true, offsetof(DealRecord,volume)     },
   { IMTDatasetField::FIELD_DEAL_PRICE        , true, offsetof(DealRecord,price)      },
   { IMTDatasetField::FIELD_DEAL_PRICE_SL     , true, offsetof(DealRecord,sl)         },
   { IMTDatasetField::FIELD_DEAL_PRICE_TP     , true, offsetof(DealRecord,tp)         },
   { IMTDatasetField::FIELD_DEAL_REASON       , true, offsetof(DealRecord,reason)     },
   { IMTDatasetField::FIELD_DEAL_COMMISSION   , true, offsetof(DealRecord,commission) },
   { IMTDatasetField::FIELD_DEAL_FEE          , true, offsetof(DealRecord,fee)        },
   { IMTDatasetField::FIELD_DEAL_STORAGE      , true, offsetof(DealRecord,storage)    },
   { IMTDatasetField::FIELD_DEAL_PROFIT       , true, offsetof(DealRecord,profit)     },
   { IMTDatasetField::FIELD_DEAL_DEALER       , true, offsetof(DealRecord,dealer)     },
   { IMTDatasetField::FIELD_DEAL_COMMENT      , true, offsetof(DealRecord,comment)    , MtFieldSize(DealRecord,comment) },
   { IMTDatasetField::FIELD_DEAL_DIGITS       , true, offsetof(DealRecord,digits)     },
  };
//--- deal request actions
const uint64_t CDealsHistory::s_deal_actions[]={ IMTDeal::DEAL_BUY, IMTDeal::DEAL_SELL };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDealsHistory::CDealsHistory(void) :
   CTradeReport(s_info),
   m_group(nullptr),m_user_request(nullptr),m_user_request_login(nullptr),m_user_request_group(nullptr),m_user_dataset(nullptr),
   m_user_logins(nullptr),m_deal_logins(nullptr),m_report_logins(nullptr),
   m_user_total(0),m_user_time_select(0),
   m_record{},m_deal(m_record),m_user{},m_user_valid(false),m_summary(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDealsHistory::~CDealsHistory(void)
  {
   CDealsHistory::Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDealsHistory::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDealsHistory::Clear(void)
  {
//--- group config interface
   if(m_group)
     {
      m_group->Release();
      m_group=nullptr;
     }
//--- user request interface
   if(m_user_request)
     {
      m_user_request->Release();
      m_user_request=nullptr;
     }
//--- user request login field interface
   if(m_user_request_login)
     {
      m_user_request_login->Release();
      m_user_request_login=nullptr;
     }
//--- user request group field interface
   if(m_user_request_group)
     {
      m_user_request_group->Release();
      m_user_request_group=nullptr;
     }
//--- user dataset interface
   m_user_dataset=nullptr;
//--- user logins interface
   if(m_user_logins)
     {
      m_user_logins->Release();
      m_user_logins=nullptr;
     }
//--- deal logins interface
   if(m_deal_logins)
     {
      m_deal_logins->Release();
      m_deal_logins=nullptr;
     }
//--- report logins interface
   if(m_report_logins)
     {
      m_report_logins->Release();
      m_report_logins=nullptr;
     }
//--- deal select object
   m_deal_select.Clear();
//--- clear counts and times
   m_user_total=0;
   m_user_time_select=0;
//--- clear records
   ZeroMemory(&m_record,sizeof(m_record));
   ZeroMemory(&m_user,sizeof(m_user));
   m_user_valid=false;
   m_summaries.Clear();
   m_summary=nullptr;
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- create interfaces
   if(!(m_group=m_api->GroupCreate()) ||
      !(m_user_request=m_api->DatasetRequestCreate()) ||
      !(m_user_dataset=m_api->DatasetAppend()) ||
      !(m_user_logins=m_api->KeySetCreate()) ||
      !(m_deal_logins=m_api->KeySetCreate()) ||
      !(m_report_logins=m_api->KeySetCreate()))
      return(MT_RET_ERR_MEM);
//--- prepare deal request
   MTAPIRES res=PrepareDealRequest();
   if(res!=MT_RET_OK)
      return(res);
//--- prepare table
   return(ReportColumn::ReportTablePrepare(*m_api,s_columns,_countof(s_columns)));
  }
//+------------------------------------------------------------------+
//| Prepare user request                                             |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::PrepareUserRequest(void)
  {
//--- checks
   if(!m_api || !m_user_request)
      return(MT_RET_ERR_PARAMS);
//--- fill request
   MTAPIRES res=MT_RET_OK;
   CDatasetRequest composer(s_user_fields,_countof(s_user_fields),*m_user_request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_USER_LOGIN:
            m_user_request_login=composer.FieldAddReference(res);
            break;
         case IMTDatasetField::FIELD_USER_GROUP:
            m_user_request_group=composer.FieldAddReference(res);
            break;
         default:
            res=composer.FieldAdd();
            break;
        }
   if(res!=MT_RET_OK)
      return(res);
//--- limit the number of rows of the resulting dataset
   return(m_user_request->RowLimit(s_user_request_limit));
  }
//+------------------------------------------------------------------+
//| Prepare deal request                                             |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::PrepareDealRequest(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- prepare deal select object
   MTAPIRES res=m_deal_select.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(MT_RET_ERROR);
//--- request
   IMTDatasetRequest *request=m_deal_select.Request();
   if(!request)
      return(MT_RET_ERROR);
//--- fill request
   CDatasetRequest composer(s_deal_fields,_countof(s_deal_fields),*request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_DEAL_LOGIN:
            res=m_deal_select.FieldLogin(composer.FieldAddReference(res));
            break;
         case IMTDatasetField::FIELD_DEAL_TIME:
            res=m_deal_select.FieldTime(composer.FieldAddReference(res));
            break;
         case IMTDatasetField::FIELD_DEAL_ACTION:
            res=composer.FieldAddWhereUIntArray(s_deal_actions,_countof(s_deal_actions));
            break;
         default:
            res=composer.FieldAdd();
            break;
        }
   if(res!=MT_RET_OK)
      return(res);
//--- limit the number of rows of the resulting dataset
   return(request->RowLimit(s_deal_request_limit));
  }
//+------------------------------------------------------------------+
//| Report writing                                                   |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::Write(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- request parameter
   const LPCWSTR filter=m_api->ParamGroups();
   if(!filter)
      return(MT_RET_ERR_PARAMS);
//--- write groups
   MTAPIRES res=WriteGroups(filter);
   if(res!=MT_RET_OK)
      return(res);
//--- log counts and times
   if((res=m_log.Ok(L"Selected %I64u accounts in %I64u ms",m_user_total,m_user_time_select))!=MT_RET_OK)
      return(res);
//--- write summary
   if((res=SummaryWrite())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write report groups                                              |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteGroups(LPCWSTR groups)
  {
//--- checks
   if(!groups || !m_api || !m_group || !m_report_logins)
      return(MT_RET_ERR_PARAMS);
//--- check for groups filter
   const bool filter=CMTStr::Compare(groups,L"*");
//--- prepare users request
   MTAPIRES res=PrepareUserRequest();
   if(res!=MT_RET_OK)
      return(res);
//--- write all users
   if(!filter)
      return(WriteUsersLogins(nullptr));
//--- get report request logins
   if((res=m_api->KeySetParamLogins(m_report_logins))!=MT_RET_OK)
      return(res);
//--- delete empty logins set
   if(!m_report_logins->Total())
     {
      m_report_logins->Release();
      m_report_logins=nullptr;
     }
//--- iterate all groups
   for(uint32_t pos=0,total=m_api->GroupTotal();pos<total;pos++)
     {
      //--- get group
      if((res=m_api->GroupNext(pos,m_group))!=MT_RET_OK)
         return(res);
      //--- write match group
      if(CMTStr::CheckGroupMask(groups,m_group->Group()))
         if((res=WriteGroup())!=MT_RET_OK)
            return(res);
     }
//--- write users by logins
   return(WriteLogins());
  }
//+------------------------------------------------------------------+
//| Write users by logins                                            |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteLogins(void)
  {
//--- checks
   if(!m_api || !m_group || !m_user_request || !m_user_request_login || !m_user_request_group || !m_user_dataset)
      return(MT_RET_ERR_PARAMS);
//--- check logins not empty
   if(!m_report_logins)
      return(MT_RET_OK);
//--- logins total
   const uint32_t total=m_report_logins->Total();
   if(!total)
      return(MT_RET_OK);
//--- clear uers record
   ZeroMemory(&m_user,sizeof(m_user));
//--- clear user request group field conditions
   MTAPIRES res=DatasetField::FieldClearConditions(*m_user_request_group,true);
   if(res!=MT_RET_OK)
      return(res);
//--- clear dataset
   if((res=m_user_dataset->Clear())!=MT_RET_OK)
      return(res);
//--- write users by reports logins
   return(WriteUsersLogins(m_report_logins));
  }
//+------------------------------------------------------------------+
//| Write users group                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteGroup(void)
  {
//--- checks
   if(!m_api || !m_group || !m_user_request || !m_user_request_login || !m_user_request_group || !m_user_dataset)
      return(MT_RET_ERR_PARAMS);
//--- fill record
   CMTStr::Copy(m_record.currency,m_group->Currency());
   m_record.currency_digits=m_group->CurrencyDigits();
//--- initialize summary for currency
   MTAPIRES res=SummaryInit();
   if(res!=MT_RET_OK)
      return(res);
//--- clear user request group field conditions
   if((res=DatasetField::FieldClearConditions(*m_user_request_group,false))!=MT_RET_OK)
      return(res);
//--- update user request group field condition
   if((res=m_user_request_group->WhereAddString(m_group->Group()))!=MT_RET_OK)
      return(res);
//--- limit the number of rows of the resulting dataset
   if((res=m_user_request->RowLimit(s_user_request_limit))!=MT_RET_OK)
      return(res);
//--- read users part by part
   bool partial=true;
   for(uint64_t login=0;partial;login++)
     {
      //--- update request login range
      if((res=m_user_request_login->BetweenUInt(login,UINT64_MAX))!=MT_RET_OK)
         return(res);
      //--- select users
      const CReportTimer timer_select;
      res=m_api->UserSelect(m_user_request,m_user_dataset);
      m_user_time_select+=timer_select.Elapsed();
      m_user_total+=m_user_dataset->RowTotal();
      partial=res==MT_RET_ERR_PARTIAL;
      if(!partial && res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.Error(res,L"Accounts selection by \'%s\' group from #%I64u",m_group->Group(),login));
      //--- write users
      res=WriteUsers(login);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.Error(res,L"Accounts of \'%s\' group processing",m_group->Group()));
      //--- clear dataset rows
      if((res=m_user_dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write users                                                      |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteUsers(uint64_t &login_last)
  {
//--- checks
   if(!m_api || !m_user_dataset || !m_user_logins)
      return(MT_RET_ERR_PARAMS);
//--- user dataset interation
   if(!m_user_dataset->RowTotal())
      return(MT_RET_OK);
//--- read user logins from dataset
   MTAPIRES res=ReadLogins(*m_user_logins,*m_user_dataset,login_last);
   if(res!=MT_RET_OK)
      return(res);
//--- remove logins from report set
   if(m_report_logins)
      if((res=m_report_logins->RemoveSet(m_user_logins))!=MT_RET_OK)
         return(res);
//--- write users deals
   return(WriteUsersDeals());
  }
//+------------------------------------------------------------------+
//| Write users by logins                                            |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteUsersLogins(IMTReportCacheKeySet *logins)
  {
//--- checks
   if(!m_api || !m_deal_logins)
      return(MT_RET_ERR_PARAMS);
//--- select deals by time
   MTAPIRES res=m_deal_select.Select(m_api->ParamFrom(),m_api->ParamTo(),logins);
   if(res!=MT_RET_OK)
      return(res);
//--- select deals part by part
   CGroupCache groups(*m_api);
   for(uint64_t login=0;IMTDataset *dataset=m_deal_select.Next(res,login);login++)
     {
      //--- read logins from dataset
      if((res=ReadLogins(*m_deal_logins,*dataset,login))!=MT_RET_OK)
         return(res);
      //--- write deals by logins
      res=WriteDealsLogins(*dataset,*m_deal_logins,groups);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.ErrorTime(res,L"Deals processing with time",m_api->ParamFrom(),m_api->ParamTo()));
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Write users deals                                                |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteUsersDeals(void)
  {
//--- checks
   if(!m_api || !m_user_logins)
      return(MT_RET_ERR_PARAMS);
//--- select deals by logins
   MTAPIRES res=m_deal_select.Select(m_api->ParamFrom(),m_api->ParamTo(),m_user_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- select deals part by part
   for(uint64_t login=0;IMTDataset *dataset=m_deal_select.Next(res,login);login++)
     {
      //--- write deals
      res=WriteDeals(*dataset,login);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.ErrorTime(res,L"Deals processing with time",m_api->ParamFrom(),m_api->ParamTo()));
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Write deals                                                      |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteDeals(IMTDataset &dataset,uint64_t &login_last)
  {
//--- checks
   if(!m_api || !m_group || !m_summary)
      return(MT_RET_ERR_PARAMS);
//--- deals total
   const uint32_t total=dataset.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- iterate deals
   MTAPIRES res=MT_RET_OK;
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read deal
      if((res=dataset.RowRead(pos,&m_deal,sizeof(m_deal)))!=MT_RET_OK)
         return(res);
      //--- store last user login
      if(login_last<m_deal.login)
         login_last=m_deal.login;
      //--- compare logins
      if(m_deal.login!=m_user.login)
        {
         //--- read user from dataset
         if((res=ReadUser(false))!=MT_RET_OK)
            return(res);
        }
      if(!m_user_valid)
         continue;
      //--- write record
      if((res=m_api->TableRowWrite(&m_record,sizeof(m_record)))!=MT_RET_OK)
         return(res);
      //--- add deal to summary
      SummaryAdd(*m_summary);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write deals by logins                                            |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::WriteDealsLogins(IMTDataset &dataset,IMTReportCacheKeySet &logins,CGroupCache &groups)
  {
//--- checks
   if(!m_api || !m_user_request || !m_user_request_login || !m_user_logins || !m_user_dataset)
      return(MT_RET_ERR_PARAMS);
//--- deals total
   const uint32_t total=dataset.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- update user request login set
   MTAPIRES res=m_user_request_login->WhereUIntSet(logins.Array(),logins.Total());
   if(res!=MT_RET_OK)
      return(res);
//--- reset rows limit of the resulting dataset
   if((res=m_user_request->RowLimit(UINT_MAX))!=MT_RET_OK)
      return(res);
//--- select users
   const CReportTimer timer_select;
   res=m_api->UserSelect(m_user_request,m_user_dataset);
   m_user_time_select+=timer_select.Elapsed();
   const uint32_t user_total=m_user_dataset->RowTotal();
   m_user_total+=user_total;
   if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
      return(m_log.Error(res,L"Accounts selection by %u logins",logins.Total()));
//--- read user logins from dataset
   uint64_t login_last=0;
   if((res=ReadLogins(*m_user_logins,*m_user_dataset,login_last))!=MT_RET_OK)
      return(res);
   if(!m_user_logins->Total())
      return(MT_RET_OK);
//--- iterate deals
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read deal
      if((res=dataset.RowRead(pos,&m_deal,sizeof(m_deal)))!=MT_RET_OK)
         return(res);
      //--- compare logins
      if(m_deal.login!=m_user.login)
        {
         //--- read user from dataset
         if((res=ReadUser(true))!=MT_RET_OK)
            return(res);
         if(!m_user_valid)
            continue;
         //--- check group change
         if(!groups.SameGroup(m_user.group))
           {
            //--- update group currency
            res=groups.GroupCurrency(m_user.group,m_record.currency,m_record.currency_digits);
            if(res!=MT_RET_OK)
               return(res);
            //--- initialize summary for currency
            if((res=SummaryInit())!=MT_RET_OK)
               return(res);
           }
        }
      else
         if(!m_user_valid)
            continue;
      //--- check valid currency
      if(!*m_record.currency)
         continue;
      //--- write record
      if((res=m_api->TableRowWrite(&m_record,sizeof(m_record)))!=MT_RET_OK)
         return(res);
      //--- add deal to summary
      if(m_summary)
         SummaryAdd(*m_summary);
     }
//--- clear user dataset
   return(m_user_dataset->RowClear());
  }
//+------------------------------------------------------------------+
//| Read logins from dataset                                         |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::ReadLogins(IMTReportCacheKeySet &logins,IMTDataset &dataset,uint64_t &login_last)
  {
//--- clear logins
   MTAPIRES res=logins.Clear();
   if(res!=MT_RET_OK)
      return(res);
//--- reserve logins
   const uint32_t total=dataset.RowTotal();
   if(!total)
      return(MT_RET_OK);
   if((res=logins.Reserve(total))!=MT_RET_OK)
      return(res);
//--- iterate dataset
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read login
      uint64_t login=0;
      if((res=dataset.RowRead(pos,&login,sizeof(uint64_t)))!=MT_RET_OK)
         return(res);
      //--- store last login
      if(login_last<login)
         login_last=login;
      //--- add login to set
      if((res=logins.Insert(login))!=MT_RET_OK)
         return(res);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Read user from dataset                                           |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::ReadUser(const bool group)
  {
//--- check pointers
   if(!m_user_logins || !m_user_dataset)
      return(MT_RET_ERR_PARAMS);
//--- search login
   const uint64_t *login=m_user_logins->Search(m_record.login);
   if(!login)
     {
      //--- user not found
      m_user.login=m_record.login;
      m_record.name[0]=0;
      m_user_valid=false;
      return(MT_RET_OK);
     }
//--- pointer to keys sorted array
   const uint64_t *begin=m_user_logins->Array();
   if(!begin || login<begin)
      return(MT_RET_ERROR);
//--- dataset row
   const uint32_t row=uint32_t(login-begin);
   const uint32_t size=group ? sizeof(m_user) : offsetof(UserRecord,group);
   MTAPIRES res=m_user_dataset->RowRead(row,&m_user,size);
   if(res!=MT_RET_OK)
      return(res);
//--- compare logins
   if(m_record.login!=m_user.login)
      return(MT_RET_ERROR);
//--- copy user name
   CMTStr::Copy(m_record.name,m_user.name);
   m_user_valid=true;
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Initialize summary for currency                                  |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::SummaryInit(void)
  {
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
void CDealsHistory::SummaryAdd(TableRecord &summary)
  {
//--- update summary
   summary.commission=SMTMath::MoneyAdd(summary.commission,m_deal.commission,summary.currency_digits);
   summary.fee       =SMTMath::MoneyAdd(summary.fee,       m_deal.fee,       summary.currency_digits);
   summary.storage   =SMTMath::MoneyAdd(summary.storage,   m_deal.storage,   summary.currency_digits);
   summary.profit    =SMTMath::MoneyAdd(summary.profit,    m_deal.profit,    summary.currency_digits);
   summary.volume   +=m_deal.volume;
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDealsHistory::SummaryWrite(void)
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
      summary->ColumnID(COLUMN_DEAL);
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
      summary->ColumnID(COLUMN_COMMISSION);
      summary->Line(line);
      summary->ValueMoney(record->commission);
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
      summary->ColumnID(COLUMN_FEE);
      summary->Line(line);
      summary->ValueMoney(record->fee);
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
int32_t CDealsHistory::SortSummary(const void *left,const void *right)
  {
   const TableRecord *lft=(const TableRecord*)left;
   const TableRecord *rgh=(const TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
