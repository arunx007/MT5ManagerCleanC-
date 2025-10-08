//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PositionsHistory.h"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CPositionsHistory::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Positions History",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   0,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                     // params
     {{ MTReportParam::TYPE_GROUPS,  MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,    MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,    MTAPI_PARAM_TO           },
     },3,            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
const ReportColumn CPositionsHistory::s_columns[]=
  //--- id,                name,               type,                                      width, width_max, offset,                      size,                          digits_column,          flags
  {{ COLUMN_LOGIN          ,L"Login",          IMTDatasetColumn::TYPE_USER_LOGIN,         10,   0, offsetof(TableRecord,login),          0,                             0,            IMTDatasetColumn::FLAG_LEFT|IMTDatasetColumn::FLAG_SORT_DEFAULT },
   { COLUMN_NAME           ,L"Name",           IMTDatasetColumn::TYPE_STRING,             20,   0, offsetof(TableRecord,name),           MtFieldSize(TableRecord,name)  },
   { COLUMN_OPEN_TIME      ,L"Time",           IMTDatasetColumn::TYPE_DATETIME_MSC,       18, 190, offsetof(TableRecord,open_time)       },
   { COLUMN_POSITION       ,L"Ticket",         IMTDatasetColumn::TYPE_UINT64,             10,   0, offsetof(TableRecord,position_id),    0,                             0,            IMTDatasetColumn::FLAG_PRIMARY },
   { COLUMN_TYPE           ,L"Type",           IMTDatasetColumn::TYPE_DEAL_ACTION,        10,   0, offsetof(TableRecord,type)            },
   { COLUMN_VOLUME         ,L"Volume",         IMTDatasetColumn::TYPE_VOLUME_POSITION_EXT,10,   0, offsetof(TableRecord,close_volume)    },
   { COLUMN_SYMBOL         ,L"Symbol",         IMTDatasetColumn::TYPE_STRING,             10,   0, offsetof(TableRecord,symbol),         MtFieldSize(TableRecord,symbol) },
   { COLUMN_OPEN_PRICE     ,L"Price",          IMTDatasetColumn::TYPE_PRICE_POSITION,     10,   0, offsetof(TableRecord,open_price),     0,                             COLUMN_DIGITS },
   { COLUMN_SL             ,L"S / L",          IMTDatasetColumn::TYPE_PRICE,              10,   0, offsetof(TableRecord,sl),             0,                             COLUMN_DIGITS },
   { COLUMN_TP             ,L"T / P",          IMTDatasetColumn::TYPE_PRICE,              10,   0, offsetof(TableRecord,tp),             0,                             COLUMN_DIGITS },
   { COLUMN_CLOSE_TIME     ,L"Close Time",     IMTDatasetColumn::TYPE_DATETIME_MSC,       18, 190, offsetof(TableRecord,close_time)      },
   { COLUMN_CLOSE_PRICE    ,L"Close Price",    IMTDatasetColumn::TYPE_PRICE,              10,   0, offsetof(TableRecord,close_price),    0,                             COLUMN_DIGITS },
   { COLUMN_REASON         ,L"Reason",         IMTDatasetColumn::TYPE_ORDER_TYPE_REASON,  10,   0, offsetof(TableRecord,open_reason)     },
   { COLUMN_COMMISSION     ,L"Commission",     IMTDatasetColumn::TYPE_MONEY,              10,   0, offsetof(TableRecord,commission),     0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_FEE            ,L"Fee",            IMTDatasetColumn::TYPE_MONEY,              10,   0, offsetof(TableRecord,commission_fee), 0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_SWAP           ,L"Swap",           IMTDatasetColumn::TYPE_MONEY,              10,   0, offsetof(TableRecord,storage),        0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_PROFIT         ,L"Profit",         IMTDatasetColumn::TYPE_MONEY,              10,   0, offsetof(TableRecord,profit),         0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_CURRENCY       ,L"Currency",       IMTDatasetColumn::TYPE_STRING,              9,  70, offsetof(TableRecord,currency),       MtFieldSize(TableRecord,currency) },
   { COLUMN_COMMENT        ,L"Comment",        IMTDatasetColumn::TYPE_STRING,             20,   0, offsetof(TableRecord,comment),        MtFieldSize(TableRecord,comment) },
   { COLUMN_DIGITS         ,L"Digits",         IMTDatasetColumn::TYPE_UINT32,              0,   0, offsetof(TableRecord,digits),         0,                             0,            IMTDatasetColumn::FLAG_HIDDEN },
   { COLUMN_DIGITS_CURRENCY,L"Currency Digits",IMTDatasetColumn::TYPE_UINT32,              0,   0, offsetof(TableRecord,digits_currency),0,                             0,            IMTDatasetColumn::FLAG_HIDDEN }
  };
//--- User request fields descriptions
const DatasetField CPositionsHistory::s_user_fields[]=
   //--- id                                     , select, offset                              , size
  {{ IMTDatasetField::FIELD_USER_LOGIN          , true, offsetof(UserRecord,login)            },
   { IMTDatasetField::FIELD_USER_NAME           , true, offsetof(UserRecord,name)             , MtFieldSize(UserRecord,name)},
   { IMTDatasetField::FIELD_USER_GROUP          , true, offsetof(UserRecord,group)            , MtFieldSize(UserRecord,group)},
  };
//--- Order time request fields descriptions
const DatasetField CPositionsHistory::s_order_time_fields[]=
   //--- id                                     , select, offset
  {{ IMTDatasetField::FIELD_ORDER_ORDER         , true, offsetof(OrderRecord,order)           },
   { IMTDatasetField::FIELD_ORDER_TIME_SETUP    , true, offsetof(OrderRecord,time_setup)      },
   { IMTDatasetField::FIELD_ORDER_TIME_DONE    },
  };
//--- Deal position request fields descriptions
const DatasetField CPositionsHistory::s_deal_position_fields[]=
   //--- id                                     , select, offset
  {{ IMTDatasetField::FIELD_DEAL_LOGIN          , true, offsetof(DealPositionRecord,login)    },
   { IMTDatasetField::FIELD_DEAL_POSITION_ID    , true, offsetof(DealPositionRecord,position) },
   { IMTDatasetField::FIELD_DEAL_TIME          },
  };
//--- Deal request fields descriptions
const DatasetField CPositionsHistory::s_deal_fields[]=
   //--- id                                     , select, offset                              , size
  {{ IMTDatasetField::FIELD_DEAL_LOGIN          , true, offsetof(DealRecord,login)            },
   { IMTDatasetField::FIELD_DEAL_DEAL           , true, offsetof(DealRecord,deal)             },
   { IMTDatasetField::FIELD_DEAL_POSITION_ID    , true, offsetof(DealRecord,position_id)      },
   { IMTDatasetField::FIELD_DEAL_TIME          },
   { IMTDatasetField::FIELD_DEAL_TIME_MSC       , true, offsetof(DealRecord,time_create)      },
   { IMTDatasetField::FIELD_DEAL_ENTRY          , true, offsetof(DealRecord,entry)            },
   { IMTDatasetField::FIELD_DEAL_ACTION         , true, offsetof(DealRecord,action)           },
   { IMTDatasetField::FIELD_DEAL_SYMBOL         , true, offsetof(DealRecord,symbol)           , MtFieldSize(DealRecord,symbol) },
   { IMTDatasetField::FIELD_DEAL_VOLUME_EXT     , true, offsetof(DealRecord,volume)           },
   { IMTDatasetField::FIELD_DEAL_PRICE          , true, offsetof(DealRecord,price_open)       },
   { IMTDatasetField::FIELD_DEAL_PRICE_SL       , true, offsetof(DealRecord,sl)               },
   { IMTDatasetField::FIELD_DEAL_PRICE_TP       , true, offsetof(DealRecord,tp)               },
   { IMTDatasetField::FIELD_DEAL_COMMISSION     , true, offsetof(DealRecord,commission)       },
   { IMTDatasetField::FIELD_DEAL_FEE            , true, offsetof(DealRecord,commission_fee)   },
   { IMTDatasetField::FIELD_DEAL_STORAGE        , true, offsetof(DealRecord,storage)          },
   { IMTDatasetField::FIELD_DEAL_PROFIT         , true, offsetof(DealRecord,profit)           },
   { IMTDatasetField::FIELD_DEAL_REASON         , true, offsetof(DealRecord,reason)           },
   { IMTDatasetField::FIELD_DEAL_COMMENT        , true, offsetof(DealRecord,comment)          , MtFieldSize(DealRecord,comment) },
   { IMTDatasetField::FIELD_DEAL_DIGITS         , true, offsetof(DealRecord,digits)           },
   { IMTDatasetField::FIELD_DEAL_DIGITS_CURRENCY, true, offsetof(DealRecord,digits_currency)  },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPositionsHistory::CPositionsHistory(void) :
   CReportGroup(s_info),
   m_from(0),m_to(0),m_from_orders(0),m_deal_logins(nullptr),m_position_ids(nullptr),m_record{},m_user{}
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPositionsHistory::~CPositionsHistory(void)
  {
   CPositionsHistory::Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CPositionsHistory::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CPositionsHistory::Clear(void)
  {
//--- base call
   CReportGroup::Clear();
//--- reset interval
   m_from=m_to=m_from_orders=0;
//--- deals logins interface
   if(m_deal_logins)
     {
      m_deal_logins->Release();
      m_deal_logins=nullptr;
     }
//--- orders interface
   if(m_position_ids)
     {
      m_position_ids->Release();
      m_position_ids=nullptr;
     }
//--- select objects
   m_user_select.Clear();
   m_deal_select.Clear();
   m_order_select.Clear();
//--- clear records
   m_record={};
   ZeroMemory(&m_user,sizeof(m_user));
   m_positions.Shutdown();
   m_summaries.Clear();
   m_allocator.Shutdown();
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- base call
   MTAPIRES res=CReportGroup::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- report interval
   m_from=m_api->ParamFrom();
   m_to  =m_api->ParamTo();
//--- check positions history interval
   if(m_from>m_to)
      return(MT_RET_ERR_PARAMS);
//--- create interfaces
   if(!(m_deal_logins=m_api->KeySetCreate()))
      return(MT_RET_ERR_MEM);
   if(!(m_position_ids=m_api->KeySetCreate()))
      return(MT_RET_ERR_MEM);
//--- prepare user request
   if((res=m_user_select.Prepare(m_api,s_user_fields,_countof(s_user_fields),s_user_request_limit))!=MT_RET_OK)
      return(res);
//--- prepare deal history position request
   if((res=PrepareDealRequest(s_deal_position_fields,_countof(s_deal_position_fields)))!=MT_RET_OK)
      return(res);
//--- prepare order history request
   if((res=PrepareOrderRequest())!=MT_RET_OK)
      return(res);
//--- prepare table
   return(ReportColumn::ReportTablePrepare(*m_api,s_columns,_countof(s_columns)));
  }
//+------------------------------------------------------------------+
//| Prepare deal request                                             |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::PrepareDealRequest(const DatasetField *fields,uint32_t fields_total)
  {
//--- checks
   if(!m_api || !fields || !fields_total)
      return(MT_RET_ERR_PARAMS);
//--- initialize deal history select object
   MTAPIRES res=m_deal_select.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(MT_RET_ERROR);
//--- request
   IMTDatasetRequest *request=m_deal_select.Request();
   if(!request)
      return(MT_RET_ERROR);
//--- fill request
   CDatasetRequest composer(fields,fields_total,*request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_DEAL_LOGIN:
            res=m_deal_select.FieldLogin(composer.FieldAddReference(res));
            break;
         case IMTDatasetField::FIELD_DEAL_TIME:
            res=m_deal_select.FieldTime(composer.FieldAddReference(res));
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
//| Prepare order history request                                    |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::PrepareOrderRequest(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- initialize order history select object
   MTAPIRES res=m_order_select.Initialize(m_api);
   if(res!=MT_RET_OK)
      return(MT_RET_ERROR);
//--- request
   IMTDatasetRequest *request=m_order_select.Request();
   if(!request)
      return(MT_RET_ERROR);
//--- fill request
   CDatasetRequest composer(s_order_time_fields,_countof(s_order_time_fields),*request);
   while(const uint32_t id=composer.Next(res))
      switch(id)
        {
         case IMTDatasetField::FIELD_ORDER_ORDER:
            res=m_order_select.FieldLogin(composer.FieldAddReference(res));
            break;
         case IMTDatasetField::FIELD_ORDER_TIME_DONE:
            res=m_order_select.FieldTime(composer.FieldAddReference(res));
            break;
         default:
            res=composer.FieldAdd();
            break;
        }
   if(res!=MT_RET_OK)
      return(res);
//--- limit the number of rows of the resulting dataset
   return(request->RowLimit(s_order_request_limit));
  }
//+------------------------------------------------------------------+
//| Report writing                                                   |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::Write(void)
  {
//--- checks
   if(!m_api || !m_report_logins || !m_deal_logins || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- collect deals logins and positions ids sets
   MTAPIRES res=CollectLoginsAndPositionIds();
   if(res!=MT_RET_OK)
      return(res);
//--- find earliest order time
   if((res=FindEarliestOrderTime())!=MT_RET_OK)
      return(res);
//--- prepare deal history request
   m_deal_select.Clear();
   if((res=PrepareDealRequest(s_deal_fields,_countof(s_deal_fields)))!=MT_RET_OK)
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
//| Collect deals logins and positions ids sets                      |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::CollectLoginsAndPositionIds(void)
  {
//--- checks
   if(!m_api || !m_report_logins || !m_deal_logins || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- select deals history by time
   MTAPIRES res=m_deal_select.Select(m_from,m_to,m_report_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- select deals part by part
   for(uint64_t login=0;IMTDataset *dataset=m_deal_select.Next(res,login);login++)
     {
      //--- reserve logins and positions
      const uint32_t total=dataset->RowTotal();
      if(!total)
         continue;
      if((res=m_deal_logins->Reserve(m_deal_logins->Total()+total))!=MT_RET_OK)
         return(res);
      if((res=m_position_ids->Reserve(m_position_ids->Total()+total))!=MT_RET_OK)
         return(res);
      //--- iterate dataset
      DealPositionRecord record{};
      for(uint32_t pos=0;pos<total;pos++)
        {
         //--- read record
         if((res=dataset->RowRead(pos,&record,sizeof(record)))!=MT_RET_OK)
            return(res);
         //--- store last login
         if(login<record.login)
            login=record.login;
         //--- add login to set
         if((res=m_deal_logins->Insert(record.login))!=MT_RET_OK)
            return(res);
         //--- add position to set and table
         if(record.position)
           {
            //--- add position to set
            if((res=m_position_ids->Insert(record.position))!=MT_RET_OK)
               return(res);
            //--- add deal to table
            if(!m_positions.Find(record.position))
               if(!m_positions.Insert(record.position))
                  return(MT_RET_ERR_MEM);
           }
        }
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Find earliest order time                                         |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::FindEarliestOrderTime(void)
  {
//--- checks
   if(!m_api || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- initialize time
   m_from_orders=m_from;
//--- check positions ids
   if(!m_position_ids->Total())
      return(MT_RET_OK);
//--- select orders history by time
   MTAPIRES res=MT_RET_OK;
   IMTDataset *first_order=m_order_select.SelectFirst(res,m_position_ids);
   if(!first_order)
      return(res);
//--- check empty
   if(!first_order->RowTotal())
      return(MT_RET_OK);
//--- read order record
   OrderRecord record{};
   if((res=first_order->RowRead(0,&record,sizeof(record)))!=MT_RET_OK)
      return(res);
   first_order->Clear();
//--- select orders history by time
   if((res=m_order_select.Select(SMTTime::MonthBegin(record.time_setup),m_api->TimeCurrent(),m_position_ids))!=MT_RET_OK)
      return(res);
//--- select orders part by part
   for(uint64_t order=0;IMTDataset *dataset=m_order_select.Next(res,order);order++)
     {
      //--- dataset size
      const uint32_t total=dataset->RowTotal();
      if(!total)
         continue;
      //--- iterate dataset
      for(uint32_t pos=0;pos<total;pos++)
        {
         //--- read order record
         if((res=dataset->RowRead(pos,&record,sizeof(record)))!=MT_RET_OK)
            return(res);
         //--- store last order id
         if(order<record.order)
            order=record.order;
         //--- update time
         if(m_from_orders>record.time_setup)
            m_from_orders=record.time_setup;
        }
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| write positions functor                                          |
//+------------------------------------------------------------------+
struct CPositionsHistory::CPositionWriter
  {
   CPositionsHistory &report;          // report reference
   MTAPIRES          res{MT_RET_OK};   // result reference
   TableRecord      *m_summary{};      // current summary record
   //--- write table item
   bool              operator()(const TableItem &item) { return(!item.record || operator()(*item.record)); }
   //--- write position
   bool              operator()(const TableRecord &record)
     {
      //--- skip open position
      if(!record.close_volume)
         return(true);
      //--- write record
      if((res=report.m_api->TableRowWrite(&record,sizeof(record)))!=MT_RET_OK)
         return(false);
      //--- summary for currency
      TableRecord *summary=Summary(record);
      if(!summary)
         return(false);
      //--- add position to summary
      report.SummaryAdd(*m_summary,record);
      return(true);
     }
   //--- summary for currency
   TableRecord*      Summary(const TableRecord &record)
     {
      //---- check summary
      if(m_summary && !CMTStr::Compare(m_summary->currency,record.currency))
         return(m_summary);
      //--- prepare summary
      TableRecord summary{};
      CMTStr::Copy(summary.currency,record.currency);
      //--- search summary
      m_summary=report.m_summaries.Search(&summary,SortSummary);
      if(!m_summary)
        {
         //--- insert new summary
         summary.digits_currency=record.digits_currency;
         m_summary=report.m_summaries.Insert(&summary,SortSummary);
         if(!m_summary)
            res=MT_RET_ERR_MEM;
        }
      return(m_summary);
     }
  };
//+------------------------------------------------------------------+
//| Write users by logins                                            |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::WriteLogins(void)
  {
//--- checks
   if(!m_api || !m_deal_logins || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- check deals logins
   if(!m_deal_logins->Total())
      return(MT_RET_OK);
//--- select deals history by time
   MTAPIRES res=m_deal_select.Select(m_from_orders,m_to,m_deal_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- write positions
   CPositionWriter writer{*this};
//--- select deals part by part
   CGroupCache groups(*m_api);
   for(uint64_t login=0;IMTDataset *dataset=m_deal_select.Next(res,login);login++)
     {
      //--- read logins from dataset
      if((res=ReadLogins(*m_position_ids,*dataset,login))!=MT_RET_OK)
         return(res);
      //--- write positions by logins
      res=WritePositions(*dataset,*m_position_ids,groups,writer);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.ErrorTime(res,L"Deals processing with time",m_from,m_to));
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
   if(res!=MT_RET_OK)
      return(res);
//--- write positions
   m_positions.Iterate(std::move(writer));
   return(writer.res);
  }
//+------------------------------------------------------------------+
//| Write positions by logins                                        |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::WritePositions(IMTDataset &dataset,IMTReportCacheKeySet &logins,CGroupCache &groups,CPositionWriter &writer)
  {
//--- checks
   if(!m_api || !m_user_logins)
      return(MT_RET_ERR_PARAMS);
//--- deals total
   const uint32_t total=dataset.RowTotal();
   if(!total)
      return(MT_RET_OK);
//--- select users
   MTAPIRES res=MT_RET_OK;
   IMTDataset *users=m_user_select.SelectUnlimited(res,&logins);
   if(!users)
      return(res);
//--- read user logins from dataset
   uint64_t login_last=0;
   if((res=ReadLogins(*m_user_logins,*users,login_last))!=MT_RET_OK)
      return(res);
   if(!m_user_logins->Total())
      return(MT_RET_OK);
//--- iterate deals
   DealRecord deal{};
   for(uint32_t pos=0;pos<total;pos++)
     {
      //--- read deal
      if((res=dataset.RowRead(pos,&deal,sizeof(deal)))!=MT_RET_OK)
         return(res);
      //--- compare logins
      if(deal.login!=m_user.login)
        {
         //--- read user from dataset
         if((res=ReadUser(deal.login,*users))!=MT_RET_OK)
            return(res);
         if(!m_user_valid)
            continue;
         //--- check group change
         if(!groups.SameGroup(m_user.group))
           {
            //--- update group currency
            if((res=groups.GroupCurrency(m_user.group,m_record.currency,m_record.digits_currency))!=MT_RET_OK)
               return(res);
           }
        }
      else
         if(!m_user_valid)
            continue;
      //--- add deal record to positions
      if((res=PositionsAdd(deal,writer))!=MT_RET_OK)
         return(res);
     }
//--- clear user dataset
   return(users->RowClear());
  }
//+------------------------------------------------------------------+
//| Read user from dataset                                           |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::ReadUser(const uint64_t login,IMTDataset &dataset)
  {
//--- read user
   MTAPIRES res=CReportGroup::ReadUser(dataset,login,&m_user,sizeof(m_user));
   if(res!=MT_RET_OK)
      return(res);
//--- update user name
   if(m_user_valid)
     {
      m_record.login=login;
      CMTStr::Copy(m_record.name,m_user.name);
     }
   else
     {
      m_user.login=login;
      m_record.name[0]=0;
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| add record to positions                                          |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::PositionsAdd(const DealRecord &deal,CPositionWriter &writer)
  {
//--- фильтрация балансовых сделок
   if(deal.IsBalance())
      return(MT_RET_OK);
//--- id
   const uint64_t id=deal.position_id;
//--- find position item
   TableItem *item=m_positions.Find(id);
   if(!item)
      return(MT_RET_OK);
//--- check record exists
   if(item->record)
     {
      //--- add deal to record
      item->record->DealAdd(deal);
      //--- check position close
      if(!item->record->open_volume)
        {
         //--- save record
         if(!writer(*item->record))
            return(writer.res);
         //--- remove item
         m_positions.Remove(id);
         m_allocator.Free(item->record);
        }
     }
   else
     {
      //--- allocate new record
      item->record=(TableRecord*)m_allocator.Allocate(sizeof(TableRecord));
      if(!item->record)
         return(MT_RET_ERR_MEM);
      //--- initialize new record
      new(item->record) TableRecord{id};
      if(!TableRecordInit(*item->record,deal,false))
        {
         //--- remove item
         m_positions.Remove(id);
         m_allocator.Free(item->record);
        }
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| initialize table record                                          |
//+------------------------------------------------------------------+
bool CPositionsHistory::TableRecordInit(TableRecord &record,const DealRecord &deal,const bool balance) const
  {
//--- initialize new position
   if(!record.DealInit(deal))
      return(false);
//--- add user
   record.login=m_record.login;
   CMTStr::Copy(record.name,m_record.name);
   CMTStr::Copy(record.currency,m_record.currency);
//--- add deal
   if(!balance)
      record.DealAdd(deal);
   return(true);
  }
//+------------------------------------------------------------------+
//| Add deal to summary                                              |
//+------------------------------------------------------------------+
void CPositionsHistory::SummaryAdd(TableRecord &summary,const PositionRecord &position)
  {
//--- update summary
   summary.profit        =SMTMath::MoneyAdd(summary.profit        ,position.profit        ,summary.digits_currency);
   if(position.IsBalance())
      return;
   summary.commission    =SMTMath::MoneyAdd(summary.commission    ,position.commission    ,summary.digits_currency);
   summary.commission_fee=SMTMath::MoneyAdd(summary.commission_fee,position.commission_fee,summary.digits_currency);
   summary.storage       =SMTMath::MoneyAdd(summary.storage       ,position.storage       ,summary.digits_currency);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CPositionsHistory::SummaryWrite(void)
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
      if(!*record->currency)
         continue;
      //--- clear summary
      summary->Clear();
      //--- total text
      summary->ColumnID(COLUMN_LOGIN);
      summary->MergeColumn(COLUMN_REASON);
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
      summary->Digits(record->digits_currency);
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
      summary->ValueMoney(record->commission_fee);
      summary->Digits(record->digits_currency);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- clear summary
      summary->Clear();
      //--- total amount
      summary->ColumnID(COLUMN_SWAP);
      summary->Line(line);
      summary->ValueMoney(record->storage);
      summary->Digits(record->digits_currency);
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
      summary->Digits(record->digits_currency);
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
int32_t CPositionsHistory::SortSummary(const void *left,const void *right)
  {
   const TableRecord *lft=(const TableRecord*)left;
   const TableRecord *rgh=(const TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
//| check balance operation                                          |
//+------------------------------------------------------------------+
inline bool CPositionsHistory::DealRecord::IsBalance(const uint32_t deal_action)
  {
   return(deal_action==IMTDeal::DEAL_BALANCE      || deal_action==IMTDeal::DEAL_CREDIT           || deal_action==IMTDeal::DEAL_CHARGE             || deal_action==IMTDeal::DEAL_CORRECTION  || deal_action==IMTDeal::DEAL_BONUS         ||
          deal_action==IMTDeal::DEAL_COMMISSION   || deal_action==IMTDeal::DEAL_COMMISSION_DAILY || deal_action==IMTDeal::DEAL_COMMISSION_MONTHLY || deal_action==IMTDeal::DEAL_AGENT_DAILY || deal_action==IMTDeal::DEAL_AGENT_MONTHLY ||
          deal_action==IMTDeal::DEAL_INTERESTRATE || deal_action==IMTDeal::DEAL_DIVIDEND         || deal_action==IMTDeal::DEAL_DIVIDEND_FRANKED   || deal_action==IMTDeal::DEAL_TAX         || deal_action==IMTDeal::DEAL_AGENT         ||
          deal_action==IMTDeal::DEAL_SO_COMPENSATION || deal_action==IMTDeal::DEAL_SO_COMPENSATION_CREDIT);
  }
//+------------------------------------------------------------------+
//| check service deal                                               |
//+------------------------------------------------------------------+
inline bool CPositionsHistory::DealRecord::IsService(void) const
  {
   return(reason==IMTDeal::DEAL_REASON_ROLLOVER   || reason==IMTDeal::DEAL_REASON_VMARGIN  ||
          reason==IMTDeal::DEAL_REASON_SETTLEMENT || reason==IMTDeal::DEAL_REASON_TRANSFER ||
          reason==IMTDeal::DEAL_REASON_SYNC       || reason==IMTDeal::DEAL_REASON_EXTERNAL_SERVICE || reason==IMTDeal::DEAL_REASON_SPLIT);
  }
//+------------------------------------------------------------------+
//| Initialize position history by deal                              |
//+------------------------------------------------------------------+
inline bool CPositionsHistory::PositionRecord::DealInit(const DealRecord &deal)
  {
//--- check balance operation
   if(deal.IsBalance())
     {
      position_id   =deal.deal;
      type          =deal.action;
      profit        =deal.profit;
      open_reason   =deal.reason;
      open_volume   =0;
      open_time     =deal.time_create;
      open_price    =0;
      sl            =0;
      tp            =0;
      close_volume  =0;
      close_time    =0;
      close_price   =0;
      CMTStr::Copy(comment,deal.comment);
      //--- additional data
      digits         =deal.digits;
      digits_currency=deal.digits_currency;
      return(true);
     }
//--- check position id
   if(!deal.position_id)
      return(false);
//--- store position id
   position_id=deal.position_id;
   return(true);
  }
//+------------------------------------------------------------------+
//| Add deal to position history                                     |
//+------------------------------------------------------------------+
inline void CPositionsHistory::PositionRecord::DealAdd(const DealRecord &deal)
  {
//--- check balance operation
   if(deal.IsBalance())
     {
      position_id   =deal.deal;
      type          =deal.action;
      profit        =deal.profit;
      open_reason   =deal.reason;
      open_volume   =0;
      open_time     =deal.time_create;
      open_price    =0;
      sl            =0;
      tp            =0;
      close_volume  =0;
      close_time    =0;
      close_price   =0;
      CMTStr::Copy(comment,deal.comment);
      //--- additional data
      digits         =deal.digits;
      digits_currency=deal.digits_currency;
      return;
     }
//--- check position id
   if(!deal.position_id)
      return;
//--- check first deal
   if(!open_volume)
     {
      CMTStr::Copy(symbol,deal.symbol);
      position_id   =deal.position_id;
      type          =deal.IsBuy() ? IMTPosition::POSITION_BUY : IMTPosition::POSITION_SELL;
      profit        =deal.profit;
      open_reason   =deal.reason;
      open_volume   =deal.volume;
      open_time     =deal.time_create;
      open_price    =deal.price_open;
      sl            =deal.sl;
      tp            =deal.tp;
      close_volume  =0;
      close_time    =0;
      close_price   =0;
      //--- money
      profit         =deal.profit;
      storage        =deal.storage;
      commission     =deal.commission;
      commission_fee =deal.commission_fee;
      //--- additional data
      digits         =deal.digits;
      digits_currency=deal.digits_currency;
      //--- comment
      CMTStr::Copy(comment,deal.comment);
      return;
     }
//--- check positions ids
   if(position_id!=deal.position_id)
      return;
//--- trade deal
   const bool trade_deal=
   deal.reason!=IMTDeal::DEAL_REASON_ROLLOVER && deal.reason!=IMTDeal::DEAL_REASON_VMARGIN          &&
   deal.reason!=IMTDeal::DEAL_REASON_TRANSFER && deal.reason!=IMTDeal::DEAL_REASON_SYNC             &&
   deal.reason!=IMTDeal::DEAL_REASON_SPLIT    && deal.reason!=IMTDeal::DEAL_REASON_CORPORATE_ACTION;
//--- check buy in or close deal
   if((deal.IsBuy() && type==IMTPosition::POSITION_BUY) || (deal.IsSell() && type==IMTPosition::POSITION_SELL))
     {
      //--- money
      profit        =SMTMath::MoneyAdd(profit,deal.profit,digits_currency);
      storage       =SMTMath::MoneyAdd(storage,deal.storage,digits_currency);
      commission    =SMTMath::MoneyAdd(commission,deal.commission,digits_currency);
      commission_fee=SMTMath::MoneyAdd(commission_fee,deal.commission_fee,digits_currency);
      //--- if it is a variation margin or swap, we do not include it in prices and volumes
      if(trade_deal)
        {
         open_time   =std::min(open_time,deal.time_create);
         open_price  =(open_volume*open_price+deal.volume*deal.price_open)/(open_volume+deal.volume);
         open_volume+=deal.volume;
         //--- update stops
         sl          =deal.sl;
         tp          =deal.tp;
        }
      //--- if the deal type is "exit", it means that there was a reversal and it is necessary to update the closing time
      if(deal.IsOut() || deal.IsInOut())
         close_time=std::max(close_time,deal.time_create);
      //--- update comment
      if(!deal.IsService())
         if(deal.comment[0]!=L'\0')
            CMTStr::Copy(comment,deal.comment);
     }
   else
     {
      //--- money
      profit        =SMTMath::MoneyAdd(profit,deal.profit,digits_currency);
      storage       =SMTMath::MoneyAdd(storage,deal.storage,digits_currency);
      commission    =SMTMath::MoneyAdd(commission,deal.commission,digits_currency);
      commission_fee=SMTMath::MoneyAdd(commission_fee,deal.commission_fee,digits_currency);
      //--- check first close deal
      if(trade_deal)
        {
         if(!close_time)
           {
            close_time    =deal.time_create;
            close_price   =deal.price_open;
            close_volume  =deal.volume;
           }
         else
           {
            close_time    =std::max(close_time,deal.time_create);
            close_price   =(close_volume*close_price+deal.volume*deal.price_open)/(close_volume+deal.volume);
            close_volume +=deal.volume;
           }
         //--- update stops
         sl          =deal.sl;
         tp          =deal.tp;
        }
     }
//--- additional data
   digits         =deal.digits;
   digits_currency=deal.digits_currency;
  }
//+------------------------------------------------------------------+
