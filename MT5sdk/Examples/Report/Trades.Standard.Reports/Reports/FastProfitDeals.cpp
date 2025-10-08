//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "FastProfitDeals.h"
#include "..\Tools\RequestParameter.h"
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define PARAMETER_DURATION           L"Duration, sec"
#define PARAMETER_DAILY_PROFIT_USD   L"Total daily profit, USD"
#define PARAMETER_PROFIT_USD_PER_LOT L"Profitable deals, USD/lot"
#define PARAMETER_PROFIT_SPREADS     L"Profitable deals, spreads"
//+------------------------------------------------------------------+
//| Plugin description structure                                     |
//+------------------------------------------------------------------+
const MTReportInfo CFastProfitDeals::s_info=
  {
   100,
   MTReportAPIVersion,
   0,
   L"Fast Profit Deals",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   0,
   MTReportInfo::TYPE_TABLE,
   L"Trades",
                     // params
     {{ MTReportParam::TYPE_INT,    PARAMETER_DURATION          , L"30" },
      { MTReportParam::TYPE_FLOAT,  PARAMETER_DAILY_PROFIT_USD  , L"50" },
      { MTReportParam::TYPE_FLOAT,  PARAMETER_PROFIT_USD_PER_LOT, L"10" },
      { MTReportParam::TYPE_FLOAT,  PARAMETER_PROFIT_SPREADS    , L""   },
      { MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS          , L"*"  },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM },
     },6,            // params_total
  };
//+------------------------------------------------------------------+
//| Column descriptions                                              |
//+------------------------------------------------------------------+
const ReportColumn CFastProfitDeals::s_columns[]=
  //--- id,                name,                       type,                                    width, width_max, offset,                      size,                          digits_column,          flags
  {{ COLUMN_LOGIN          ,L"Login",                  IMTDatasetColumn::TYPE_USER_LOGIN,       10,   0, offsetof(TableRecord,login),          0,                             0,              IMTDatasetColumn::FLAG_SORT_DEFAULT },
   { COLUMN_NAME           ,L"Name",                   IMTDatasetColumn::TYPE_STRING,           20,   0, offsetof(TableRecord,name),           MtFieldSize(TableRecord,name)  },
   { COLUMN_DEAL           ,L"Deal",                   IMTDatasetColumn::TYPE_UINT64,           10,   0, offsetof(TableRecord,deal),           0,                             0,              IMTDatasetColumn::FLAG_PRIMARY|IMTDatasetColumn::FLAG_LEFT },
   { COLUMN_ID             ,L"ID",                     IMTDatasetColumn::TYPE_STRING,           10,   0, offsetof(TableRecord,id),             MtFieldSize(TableRecord,id),   0,              IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_ORDER          ,L"Order",                  IMTDatasetColumn::TYPE_UINT64,           10,   0, offsetof(TableRecord,order),          0,                             0,              IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_POSITION       ,L"Position",               IMTDatasetColumn::TYPE_UINT64,           10,   0, offsetof(TableRecord,position_id),    },
   { COLUMN_OPEN_TIME      ,L"Time",                   IMTDatasetColumn::TYPE_DATETIME_MSC,     18, 190, offsetof(TableRecord,open_time),      0,                             0,              IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_TYPE           ,L"Type",                   IMTDatasetColumn::TYPE_DEAL_ACTION,       6, 100, offsetof(TableRecord,type)            },
   { COLUMN_SYMBOL         ,L"Symbol",                 IMTDatasetColumn::TYPE_STRING,           10,   0, offsetof(TableRecord,symbol),         MtFieldSize(TableRecord,symbol) },
   { COLUMN_VOLUME         ,L"Volume",                 IMTDatasetColumn::TYPE_VOLUME_EXT,       10,   0, offsetof(TableRecord,open_volume),    0,                             0,              IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_OPEN_PRICE     ,L"Price",                  IMTDatasetColumn::TYPE_PRICE_POSITION,   10,   0, offsetof(TableRecord,open_price),     0,                             COLUMN_DIGITS },
   { COLUMN_SL             ,L"S / L",                  IMTDatasetColumn::TYPE_PRICE,            10,   0, offsetof(TableRecord,sl),             0,                             COLUMN_DIGITS , IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_TP             ,L"T / P",                  IMTDatasetColumn::TYPE_PRICE,            10,   0, offsetof(TableRecord,tp),             0,                             COLUMN_DIGITS , IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_CLOSE_TIME     ,L"Close Time",             IMTDatasetColumn::TYPE_DATETIME_MSC,     18, 190, offsetof(TableRecord,close_time)      },
   { COLUMN_DURATION       ,L"Duration, ms",           IMTDatasetColumn::TYPE_INT32,            10,   0, offsetof(TableRecord,duration)        },
   { COLUMN_CLOSE_VOLUME   ,L"Close Volume",           IMTDatasetColumn::TYPE_VOLUME_EXT,       10,   0, offsetof(TableRecord,close_volume),   },
   { COLUMN_CLOSE_PRICE    ,L"Close Price",            IMTDatasetColumn::TYPE_PRICE,            10,   0, offsetof(TableRecord,close_price),    0,                             COLUMN_DIGITS },
   { COLUMN_REASON         ,L"Reason",                 IMTDatasetColumn::TYPE_ORDER_TYPE_REASON,10,   0, offsetof(TableRecord,reason)          },
   { COLUMN_COMMISSION     ,L"Commission",             IMTDatasetColumn::TYPE_MONEY,            10,   0, offsetof(TableRecord,commission),     0,                             COLUMN_DIGITS_CURRENCY , IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_FEE            ,L"Fee",                    IMTDatasetColumn::TYPE_MONEY,            10,   0, offsetof(TableRecord,commission_fee), 0,                             COLUMN_DIGITS_CURRENCY , IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_SWAP           ,L"Swap",                   IMTDatasetColumn::TYPE_MONEY,            10,   0, offsetof(TableRecord,storage),        0,                             COLUMN_DIGITS_CURRENCY , IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_PROFIT         ,L"Profit",                 IMTDatasetColumn::TYPE_MONEY,            10,   0, offsetof(TableRecord,profit),         0,                             COLUMN_DIGITS_CURRENCY },
   { COLUMN_CURRENCY       ,L"Currency",               IMTDatasetColumn::TYPE_STRING,            9,  70, offsetof(TableRecord,currency),       MtFieldSize(TableRecord,currency) },
   { COLUMN_PROFIT_CURRENCY,L"Profit, USD",            IMTDatasetColumn::TYPE_MONEY,            10,   0, offsetof(TableRecord,profit_currency),0,                             -int(SMTMath::MoneyDigits(DEFAULT_CURRENCY))},
   { COLUMN_PROFIT_SPREADS ,L"Profit, spreads",        IMTDatasetColumn::TYPE_DOUBLE,           10,   0, offsetof(TableRecord,profit_spreads) ,0,                             -1 },
   { COLUMN_DAILY_PROFIT   ,L"Total Daily Profit, USD",IMTDatasetColumn::TYPE_MONEY,            15,   0, offsetof(TableRecord,daily_profit),   0,                             -int(SMTMath::MoneyDigits(DEFAULT_CURRENCY))},
   { COLUMN_COMMENT        ,L"Comment",                IMTDatasetColumn::TYPE_STRING,           20,   0, offsetof(TableRecord,comment),        MtFieldSize(TableRecord,comment), 0,           IMTDatasetColumn::FLAG_INVISIBLE_DEFAULT },
   { COLUMN_DIGITS         ,L"Digits",                 IMTDatasetColumn::TYPE_UINT32,            0,   0, offsetof(TableRecord,digits),         0,                             0,              IMTDatasetColumn::FLAG_HIDDEN },
   { COLUMN_DIGITS_CURRENCY,L"Currency Digits",        IMTDatasetColumn::TYPE_UINT32,            0,   0, offsetof(TableRecord,digits_currency),0,                             0,              IMTDatasetColumn::FLAG_HIDDEN }
  };
//--- User request fields descriptions
const DatasetField CFastProfitDeals::s_user_fields[]=
   //--- id                                     , select, offset                              , size
  {{ IMTDatasetField::FIELD_USER_LOGIN          , true, offsetof(UserRecord,login)            },
   { IMTDatasetField::FIELD_USER_NAME           , true, offsetof(UserRecord,name)             , MtFieldSize(UserRecord,name)},
   { IMTDatasetField::FIELD_USER_GROUP          , true, offsetof(UserRecord,group)            , MtFieldSize(UserRecord,group)},
  };
//--- Deal brief request fields descriptions
const DatasetField CFastProfitDeals::s_deal_brief_fields[]=
   //--- id                                     , select, offset
  {{ IMTDatasetField::FIELD_DEAL_LOGIN          , true, offsetof(DealBriefRecord,login)       },
   { IMTDatasetField::FIELD_DEAL_POSITION_ID    , true, offsetof(DealBriefRecord,position)    },
   { IMTDatasetField::FIELD_DEAL_PROFIT         , true, offsetof(DealBriefRecord,profit)      },
   { IMTDatasetField::FIELD_DEAL_DIGITS_CURRENCY, true, offsetof(DealBriefRecord,digits_currency) },
   { IMTDatasetField::FIELD_DEAL_TIME          },
  };
//--- Deal request fields descriptions
const DatasetField CFastProfitDeals::s_deal_fields[]=
   //--- id                                     , select, offset                              , size
  {{ IMTDatasetField::FIELD_DEAL_LOGIN          , true, offsetof(DealRecord,login)            },
   { IMTDatasetField::FIELD_DEAL_DEAL           , true, offsetof(DealRecord,deal)             },
   { IMTDatasetField::FIELD_DEAL_EXTERNAL_ID    , true, offsetof(DealRecord,id)               , MtFieldSize(DealRecord,id) },
   { IMTDatasetField::FIELD_DEAL_ORDER          , true, offsetof(DealRecord,order)            },
   { IMTDatasetField::FIELD_DEAL_POSITION_ID    , true, offsetof(DealRecord,position_id)      },
   { IMTDatasetField::FIELD_DEAL_TIME          },
   { IMTDatasetField::FIELD_DEAL_TIME_MSC       , true, offsetof(DealRecord,time_create)      },
   { IMTDatasetField::FIELD_DEAL_ACTION         , true, offsetof(DealRecord,action)           },
   { IMTDatasetField::FIELD_DEAL_SYMBOL         , true, offsetof(DealRecord,symbol)           , MtFieldSize(DealRecord,symbol) },
   { IMTDatasetField::FIELD_DEAL_VOLUME_EXT     , true, offsetof(DealRecord,volume)           },
   { IMTDatasetField::FIELD_DEAL_PRICE          , true, offsetof(DealRecord,price_open)       },
   { IMTDatasetField::FIELD_DEAL_PRICE_SL       , true, offsetof(DealRecord,sl)               },
   { IMTDatasetField::FIELD_DEAL_PRICE_TP       , true, offsetof(DealRecord,tp)               },
   { IMTDatasetField::FIELD_DEAL_MARKET_BID     , true, offsetof(DealRecord,market_bid)       },
   { IMTDatasetField::FIELD_DEAL_MARKET_ASK     , true, offsetof(DealRecord,market_ask)       },
   { IMTDatasetField::FIELD_DEAL_COMMISSION     , true, offsetof(DealRecord,commission)       },
   { IMTDatasetField::FIELD_DEAL_FEE            , true, offsetof(DealRecord,commission_fee)   },
   { IMTDatasetField::FIELD_DEAL_STORAGE        , true, offsetof(DealRecord,storage)          },
   { IMTDatasetField::FIELD_DEAL_PROFIT         , true, offsetof(DealRecord,profit)           },
   { IMTDatasetField::FIELD_DEAL_REASON         , true, offsetof(DealRecord,reason)           },
   { IMTDatasetField::FIELD_DEAL_COMMENT        , true, offsetof(DealRecord,comment)          , MtFieldSize(DealRecord,comment) },
   { IMTDatasetField::FIELD_DEAL_DIGITS         , true, offsetof(DealRecord,digits)           },
   { IMTDatasetField::FIELD_DEAL_DIGITS_CURRENCY, true, offsetof(DealRecord,digits_currency)  },
  };
//--- deal request actions
const uint64_t CFastProfitDeals::s_deal_actions[]={ IMTDeal::DEAL_BUY, IMTDeal::DEAL_SELL };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CFastProfitDeals::CFastProfitDeals(void) :
   CReportGroup(s_info),
   m_ctm(0),m_duration(0),m_daily_profit_usd(0),m_profit_usd_lot(0),m_profit_spreads(0),
   m_currency_digits(SMTMath::MoneyDigits(DEFAULT_CURRENCY)),
   m_deal_logins(nullptr),m_position_ids(nullptr),m_record{},m_user{},m_summary(nullptr)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CFastProfitDeals::~CFastProfitDeals(void)
  {
   CFastProfitDeals::Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CFastProfitDeals::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CFastProfitDeals::Clear(void)
  {
//--- base call
   CReportGroup::Clear();
//--- reset parameters
   m_ctm=0;
   m_duration=0;
   m_daily_profit_usd=0;
   m_profit_usd_lot=0;
   m_profit_spreads=0;
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
//--- clear records
   m_record={};
   m_user_profits.Shutdown();
   m_positions.Shutdown();
   m_summaries.Clear();
   m_summary=nullptr;
   m_allocator.Shutdown();
  }
//+------------------------------------------------------------------+
//| Report preparation                                               |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::Prepare(void)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- base call
   MTAPIRES res=CReportGroup::Prepare();
   if(res!=MT_RET_OK)
      return(res);
//--- report interval
   m_ctm=SMTTime::DayBegin(m_api->ParamFrom());
   if(m_ctm<0)
      return(MT_RET_ERR_PARAMS);
//--- duration parameter
   CRequestParameter param(*m_api);
   if((res=param.ParamGetInt(m_duration,PARAMETER_DURATION))!=MT_RET_OK)
      return(res);
   if(m_duration<0)
      return(MT_RET_ERR_PARAMS);
   m_duration*=1000;
//--- minimal total daily profit, USD
   if((res=param.ParamGetFloat(m_daily_profit_usd,PARAMETER_DAILY_PROFIT_USD))!=MT_RET_OK)
      return(res);
//--- minimal deal profit, USD/lot
   if((res=param.ParamGetFloat(m_profit_usd_lot,PARAMETER_PROFIT_USD_PER_LOT,DBL_MAX))!=MT_RET_OK)
      return(res);
   if(m_profit_usd_lot<0)
      return(MT_RET_ERR_PARAMS);
//--- minimal deal profit, spreads
   if((res=param.ParamGetFloat(m_profit_spreads,PARAMETER_PROFIT_SPREADS,DBL_MAX))!=MT_RET_OK)
      return(res);
   if(m_profit_spreads<0)
      return(MT_RET_ERR_PARAMS);
//--- create interfaces
   if(!(m_deal_logins=m_api->KeySetCreate()))
      return(MT_RET_ERR_MEM);
   if(!(m_position_ids=m_api->KeySetCreate()))
      return(MT_RET_ERR_MEM);
//--- prepare user request
   if((res=m_user_select.Prepare(m_api,s_user_fields,_countof(s_user_fields),s_user_request_limit))!=MT_RET_OK)
      return(res);
//--- prepare deal history brief request
   if((res=PrepareDealRequest(s_deal_brief_fields,_countof(s_deal_brief_fields)))!=MT_RET_OK)
      return(res);
//--- prepare table
   return(ReportColumn::ReportTablePrepare(*m_api,s_columns,_countof(s_columns)));
  }
//+------------------------------------------------------------------+
//| Prepare deal request                                             |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::PrepareDealRequest(const DatasetField *fields,uint32_t fields_total)
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
MTAPIRES CFastProfitDeals::Write(void)
  {
//--- checks
   if(!m_api || !m_report_logins || !m_deal_logins || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- collect deals brief information
   MTAPIRES res=CollectDealsBrief();
   if(res!=MT_RET_OK)
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
//| Collect deals brief information                                  |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::CollectDealsBrief(void)
  {
//--- checks
   if(!m_api || !m_report_logins || !m_deal_logins || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- select deals history by time
   MTAPIRES res=m_deal_select.Select(m_ctm,m_ctm+SECONDS_IN_DAY-1,m_report_logins);
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
      DealBriefRecord deal{};
      for(uint32_t pos=0;pos<total;pos++)
        {
         //--- read record
         if((res=dataset->RowRead(pos,&deal,sizeof(deal)))!=MT_RET_OK)
            return(res);
         //--- store last login
         if(login<deal.login)
            login=deal.login;
         //--- check position id
         if(!deal.position)
            continue;
         //--- add login to set
         if((res=m_deal_logins->Insert(deal.login))!=MT_RET_OK)
            return(res);
         //--- add position to set
         if((res=m_position_ids->Insert(deal.position))!=MT_RET_OK)
            return(res);
         //--- add deal to table
         if(!m_positions.Find(deal.position))
            if(!m_positions.Insert(deal.position))
               return(MT_RET_ERR_MEM);
         //--- calculate total daily profit
         if((res=CalcDailyProfit(deal))!=MT_RET_OK)
            return(res);
        }
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Write users by logins                                            |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::WriteLogins(void)
  {
//--- checks
   if(!m_api || !m_deal_logins || !m_position_ids)
      return(MT_RET_ERR_PARAMS);
//--- check deals logins
   if(!m_deal_logins->Total())
      return(MT_RET_OK);
//--- select deals history by time
   MTAPIRES res=m_deal_select.Select(m_ctm,m_ctm+SECONDS_IN_DAY-1,m_deal_logins);
   if(res!=MT_RET_OK)
      return(res);
//--- select deals part by part
   CGroupCache groups(*m_api);
   for(uint64_t login=0;IMTDataset *dataset=m_deal_select.Next(res,login);login++)
     {
      //--- read logins from dataset
      if((res=ReadLogins(*m_position_ids,*dataset,login))!=MT_RET_OK)
         return(res);
      //--- write positions by logins
      res=WritePositions(*dataset,*m_position_ids,groups);
      if(res!=MT_RET_OK && res!=MT_RET_OK_NONE)
         return(m_log.ErrorTime(res,L"Deals processing with time",m_ctm,m_ctm+SECONDS_IN_DAY-1));
      //--- clear dataset rows
      if((res=dataset->RowClear())!=MT_RET_OK)
         return(res);
     }
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Write positions by logins                                        |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::WritePositions(IMTDataset &dataset,IMTReportCacheKeySet &logins,CGroupCache &groups)
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
   double rate{};
   CMTStr32 currency;
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
            //--- check currency changed
            if(currency.Compare(m_record.currency)!=0)
              {
               //--- store currency
               currency.Assign(m_record.currency);
               //--- update currency conversion rate
               if(currency.Compare(DEFAULT_CURRENCY)==0)
                  rate=1.0;
               else
                  if(m_api->TradeRateSell(currency.Str(),DEFAULT_CURRENCY,rate)!=MT_RET_OK)
                     rate=0.0;
              }
           }
         //--- daily profit in report's currency
         const UserProfit *user_profit=m_user_profits.Find(deal.login);
         m_record.daily_profit=user_profit ? SMTMath::PriceNormalize(user_profit->daily_profit*rate,m_currency_digits) : 0.0;
        }
      else
         if(!m_user_valid)
            continue;
      //--- check daily profit limit
      if(m_record.daily_profit<m_daily_profit_usd)
         continue;
      //--- add deal record to positions
      if((res=PositionsAdd(deal,rate))!=MT_RET_OK)
         return(res);
     }
//--- clear user dataset
   return(users->RowClear());
  }
//+------------------------------------------------------------------+
//| Calculate total daily profit                                     |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::CalcDailyProfit(const DealBriefRecord &deal)
  {
//--- skip no profit deals
   if(deal.profit==0)
      return MT_RET_OK;
//--- find existing trading account
   UserProfit *user=m_user_profits.Find(deal.login);
   if(user)
     {
      //--- add profit
      user->daily_profit=SMTMath::MoneyAdd(user->daily_profit,deal.profit,deal.digits_currency);
     }
   else
     {
      //--- insert new trading account with profit
      if(!m_user_profits.Insert(deal.login,deal.profit))
         return MT_RET_ERR_MEM;
     }
   return MT_RET_OK;
  }
//+------------------------------------------------------------------+
//| Read user from dataset                                           |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::ReadUser(const uint64_t login,IMTDataset &dataset)
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
MTAPIRES CFastProfitDeals::PositionsAdd(const DealRecord &deal,const double rate)
  {
//--- фильтрация балансовых сделок
   if(deal.IsBalance())
      return(MT_RET_OK);
//--- id
   const uint64_t id=deal.position_id;
//--- find position item
   PositionItem *item=m_positions.Find(id);
   if(!item)
      return(MT_RET_OK);
//--- check record exists
   if(item->record)
     {
      //--- add deal to record
      if(item->record->DealAdd(deal))
        {
         //--- check position and deal
         if(IsFastPosition(*item->record,deal) && IsDealProfit(*item->record,deal,rate,m_record.profit_spreads))
           {
            //--- store position
            static_cast<PositionRecord&>(m_record)=*item->record;
            //--- fill table record by deal
            m_record.DealFill(deal);
            //--- profit in report's currency
            m_record.profit_currency=SMTMath::PriceNormalize(m_record.profit*rate,m_currency_digits);
            //--- save record
            const MTAPIRES res=WritePosition(m_record);
            if(res!=MT_RET_OK)
               return(res);
           }
        }
      //--- check position close
      if(!item->record->open_volume)
        {
         //--- remove item
         m_positions.Remove(id);
         m_allocator.Free(item->record);
        }
     }
   else
     {
      //--- allocate new record
      item->record=(PositionRecord*)m_allocator.Allocate(sizeof(PositionRecord));
      if(!item->record)
         return(MT_RET_ERR_MEM);
      //--- initialize new record
      new(item->record) PositionRecord{id};
      if(!item->record->DealInit(deal))
        {
         //--- remove item
         m_positions.Remove(id);
         m_allocator.Free(item->record);
        }
      //--- add deal to new record
      item->record->DealAdd(deal);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write position                                                   |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::WritePosition(const TableRecord &record)
  {
//--- checks
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- skip open position
   if(!record.close_volume)
      return(MT_RET_OK);
//--- write record
   const MTAPIRES res=m_api->TableRowWrite(&record,sizeof(record));
   if(res!=MT_RET_OK)
      return(res);
//--- summary for currency
   TableRecord *summary=SummaryGet(record);
   if(!summary)
      return(MT_RET_ERR_MEM);
//--- add position to summary
   SummaryAdd(*m_summary,record);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| check fast position                                              |
//+------------------------------------------------------------------+
bool CFastProfitDeals::IsFastPosition(const PositionRecord &position,const DealRecord &deal) const
  {
   return(position.open_time+m_duration>=deal.time_create);
  }
//+------------------------------------------------------------------+
//| check profit deal                                                |
//+------------------------------------------------------------------+
bool CFastProfitDeals::IsDealProfit(const PositionRecord &position,const DealRecord &deal,const double rate,double &profit_spreads) const
  {
   if(!deal.volume || deal.profit<0.0)
      return(false);
//--- profit in spreads
   const double spread=deal.market_ask-deal.market_bid;
   double delta_price=deal.price_open-position.open_price;
   if(deal.IsBuy())
      delta_price=-delta_price;
   profit_spreads=spread>0.0 && delta_price>0.0 ? SMTMath::PriceNormalize(delta_price/spread,1) : 0.0;
//--- check deal profit limit in spreads
   if(profit_spreads>=m_profit_spreads)
      return(true);
//--- check deal profit limit in USD/lot
   if(m_profit_usd_lot<DBL_MAX)
     {
      //--- check rate
      if(rate<=0.0)
         return(false);
      //--- deal volume
      const double volume=SMTMath::VolumeExtToDouble(deal.volume);
      //--- check profit per lot
      if(volume>0 && deal.profit*rate/volume>=m_profit_usd_lot)
         return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Summary for currency                                             |
//+------------------------------------------------------------------+
CFastProfitDeals::TableRecord* CFastProfitDeals::SummaryGet(const TableRecord &record)
  {
//---- check summary
   if(m_summary && !CMTStr::Compare(m_summary->currency,record.currency))
      return(m_summary);
//--- prepare summary
   TableRecord summary{};
   CMTStr::Copy(summary.currency,record.currency);
//--- search summary
   m_summary=m_summaries.Search(&summary,SortSummary);
   if(!m_summary)
     {
      //--- insert new summary
      summary.digits_currency=record.digits_currency;
      m_summary=m_summaries.Insert(&summary,SortSummary);
     }
   return(m_summary);
  }
//+------------------------------------------------------------------+
//| Add deal to summary                                              |
//+------------------------------------------------------------------+
void CFastProfitDeals::SummaryAdd(TableRecord &summary,const TableRecord &record)
  {
//--- update summary
   summary.commission     =SMTMath::MoneyAdd(summary.commission     ,record.commission     ,summary.digits_currency);
   summary.commission_fee =SMTMath::MoneyAdd(summary.commission_fee ,record.commission_fee ,summary.digits_currency);
   summary.storage        =SMTMath::MoneyAdd(summary.storage        ,record.storage        ,summary.digits_currency);
   summary.profit         =SMTMath::MoneyAdd(summary.profit         ,record.profit         ,summary.digits_currency);
   summary.profit_currency=SMTMath::MoneyAdd(summary.profit_currency,record.profit_currency,m_currency_digits);
   summary.profit_spreads =SMTMath::MoneyAdd(summary.profit_spreads ,record.profit_spreads ,1);
  }
//+------------------------------------------------------------------+
//| Write summary                                                    |
//+------------------------------------------------------------------+
MTAPIRES CFastProfitDeals::SummaryWrite(void)
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
      //--- total commission
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
      //--- total fee
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
      //--- total swap
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
      //--- total profit
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
      //--- clear summary
      summary->Clear();
      //--- total profit in report's currency
      summary->ColumnID(COLUMN_PROFIT_CURRENCY);
      summary->Line(line);
      summary->ValueMoney(record->profit_currency);
      summary->Digits(m_currency_digits);
      //--- add summary
      if((res=m_api->TableSummaryAdd(summary))!=MT_RET_OK)
        {
         summary->Release();
         return(res);
        }
      //--- total amount
      summary->ColumnID(COLUMN_PROFIT_SPREADS);
      summary->Line(line);
      summary->ValueMoney(record->profit_spreads);
      summary->Digits(1);
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
int CFastProfitDeals::SortSummary(const void *left,const void *right)
  {
   const TableRecord *lft=(const TableRecord*)left;
   const TableRecord *rgh=(const TableRecord*)right;
//--- sort by currency
   return(CMTStr::CompareNoCase(lft->currency,rgh->currency));
  }
//+------------------------------------------------------------------+
//| check balance operation                                          |
//+------------------------------------------------------------------+
inline bool CFastProfitDeals::DealRecord::IsBalance(const uint32_t deal_action)
  {
   return(deal_action==IMTDeal::DEAL_BALANCE      || deal_action==IMTDeal::DEAL_CREDIT           || deal_action==IMTDeal::DEAL_CHARGE             || deal_action==IMTDeal::DEAL_CORRECTION  || deal_action==IMTDeal::DEAL_BONUS         ||
          deal_action==IMTDeal::DEAL_COMMISSION   || deal_action==IMTDeal::DEAL_COMMISSION_DAILY || deal_action==IMTDeal::DEAL_COMMISSION_MONTHLY || deal_action==IMTDeal::DEAL_AGENT_DAILY || deal_action==IMTDeal::DEAL_AGENT_MONTHLY ||
          deal_action==IMTDeal::DEAL_INTERESTRATE || deal_action==IMTDeal::DEAL_DIVIDEND         || deal_action==IMTDeal::DEAL_DIVIDEND_FRANKED   || deal_action==IMTDeal::DEAL_TAX         || deal_action==IMTDeal::DEAL_AGENT         ||
          deal_action==IMTDeal::DEAL_SO_COMPENSATION || deal_action==IMTDeal::DEAL_SO_COMPENSATION_CREDIT);
  }
//+------------------------------------------------------------------+
//| check service deal                                               |
//+------------------------------------------------------------------+
inline bool CFastProfitDeals::DealRecord::IsService(void) const
  {
   return(reason==IMTDeal::DEAL_REASON_ROLLOVER   || reason==IMTDeal::DEAL_REASON_VMARGIN  ||
          reason==IMTDeal::DEAL_REASON_SETTLEMENT || reason==IMTDeal::DEAL_REASON_TRANSFER ||
          reason==IMTDeal::DEAL_REASON_SYNC       || reason==IMTDeal::DEAL_REASON_EXTERNAL_SERVICE || reason==IMTDeal::DEAL_REASON_SPLIT);
  }
//+------------------------------------------------------------------+
//| Initialize position history by deal                              |
//+------------------------------------------------------------------+
inline bool CFastProfitDeals::PositionRecord::DealInit(const DealRecord &deal)
  {
//--- check balance operation
   if(deal.IsBalance())
      return(false);
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
inline bool CFastProfitDeals::PositionRecord::DealAdd(const DealRecord &deal)
  {
//--- check balance operation
   if(deal.IsBalance())
      return(false);
//--- check position id
   if(!deal.position_id)
      return(false);
//--- check first deal
   if(!open_volume)
     {
      position_id   =deal.position_id;
      type          =deal.IsBuy() ? IMTPosition::POSITION_BUY : IMTPosition::POSITION_SELL;
      open_volume   =deal.volume;
      open_time     =deal.time_create;
      open_price    =deal.price_open;
      return(false);
     }
//--- check positions ids
   if(position_id!=deal.position_id)
      return(false);
//--- trade deal
   const bool trade_deal=
   deal.reason!=IMTDeal::DEAL_REASON_ROLLOVER && deal.reason!=IMTDeal::DEAL_REASON_VMARGIN          &&
   deal.reason!=IMTDeal::DEAL_REASON_TRANSFER && deal.reason!=IMTDeal::DEAL_REASON_SYNC             &&
   deal.reason!=IMTDeal::DEAL_REASON_SPLIT    && deal.reason!=IMTDeal::DEAL_REASON_CORPORATE_ACTION;
//--- check buy in or close deal
   if((deal.IsBuy() && type==IMTPosition::POSITION_BUY) || (deal.IsSell() && type==IMTPosition::POSITION_SELL))
     {
      //--- if it is a variation margin or swap, we do not include it in prices and volumes
      if(trade_deal)
        {
         open_time   =std::max(open_time,deal.time_create);
         open_price  =(open_volume*open_price+deal.volume*deal.price_open)/(open_volume+deal.volume);
         open_volume+=deal.volume;
        }
     }
   else
     {
      //--- check close deal type
      if(trade_deal)
         return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| fill table record by deal                                        |
//+------------------------------------------------------------------+
void CFastProfitDeals::TableRecord::DealFill(const DealRecord &deal_record)
  {
   type           =deal_record.action;
   deal           =deal_record.deal;
   CMTStr::Copy(id,deal_record.id);
   order          =deal_record.order;
   CMTStr::Copy(symbol,deal_record.symbol);
   sl             =deal_record.sl;
   tp             =deal_record.tp;
   close_volume   =deal_record.volume;
   close_time     =deal_record.time_create;
   duration       =int(close_time-open_time);
   close_price    =deal_record.price_open;
   reason         =deal_record.reason;
   commission     =deal_record.commission;
   commission_fee =deal_record.commission_fee;
   storage        =deal_record.storage;
   profit         =deal_record.profit;
   CMTStr::Copy(comment,deal_record.comment);
   digits         =deal_record.digits;
   digits_currency=deal_record.digits_currency;
  }
//+------------------------------------------------------------------+
