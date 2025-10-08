//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyTradeReport.h"
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define PARAMETER_TOP_PROFIT_DEALS     L"Top Profit Deals"
#define PARAMETER_TOP_LOSS_DEALS       L"Top Loss Deals"
#define PARAMETER_TOP_PROFIT_POSITIONS L"Top Profit Positions"
#define PARAMETER_TOP_LOSS_POSITIONS   L"Top Loss Positions"
#define PARAMETER_TOP_PROFIT_ACCOUNTS  L"Top Profit Accounts"
#define DEFAULT_TOP_COUNT              L"10"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CDailyTradeReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_9,
   L"Daily Trades",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_ALL_FULL,
   MTReportInfo::TYPE_HTML|MTReportInfo::TYPE_DASHBOARD,
   L"Daily",
                    // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*"     },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM             },
     },2,           // params_total
     {              // configs
      { MTReportParam::TYPE_STRING, L"Currency"                   , DEFAULT_CURRENCY  },
      { MTReportParam::TYPE_INT,    PARAMETER_TOP_PROFIT_DEALS    , DEFAULT_TOP_COUNT },
      { MTReportParam::TYPE_INT,    PARAMETER_TOP_LOSS_DEALS      , DEFAULT_TOP_COUNT },
      { MTReportParam::TYPE_INT,    PARAMETER_TOP_PROFIT_POSITIONS, DEFAULT_TOP_COUNT },
      { MTReportParam::TYPE_INT,    PARAMETER_TOP_LOSS_POSITIONS  , DEFAULT_TOP_COUNT },
      { MTReportParam::TYPE_INT,    PARAMETER_TOP_PROFIT_ACCOUNTS , DEFAULT_TOP_COUNT },
     },6            // configs_total
  };
//+------------------------------------------------------------------+
//| Column description Daily Deal                                    |
//+------------------------------------------------------------------+
ReportColumn CDailyTradeReport::s_columns_daily[]=
  {
   //--- id,name,          type,                         width,  width_max, offset,                size,digits_column,flags
     { 1,L"Date",          IMTDatasetColumn::TYPE_DATE   ,20,0, offsetof(DailyRecord,date)          ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Profit",        IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(DailyRecord,profit)        ,0,0,0 },
   { 3,  L"Loss",          IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(DailyRecord,loss)          ,0,0,0 },
   { 4,  L"Profit/Loss",   IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(DailyRecord,profit_loss)   ,0,0,0 },
   { 5,  L"Profit Count",  IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(DailyRecord,profit_count)  ,0,0,0 },
   { 6,  L"Loss Count",    IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(DailyRecord,loss_count)    ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Total                                         |
//+------------------------------------------------------------------+
ReportColumn CDailyTradeReport::s_columns_total[]=
  {
   //--- id,name,          type,                         width,  width_max, offset,                size,digits_column,flags
     { 1,L"Type",          IMTDatasetColumn::TYPE_STRING ,40,0, offsetof(TotalRecord,type)          ,MtFieldSize(TotalRecord,type),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Value",         IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(TotalRecord,value)         ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Deal                                          |
//+------------------------------------------------------------------+
ReportColumn CDailyTradeReport::s_columns_deal[]=
  {
   //--- id,name,          type,                             width,  width_max, offset,                size,digits_column,flags
     { 1,L"Deal",          IMTDatasetColumn::TYPE_UINT64     , 8,0, offsetof(DealRecord,deal)           ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN , 8,0, offsetof(DealRecord,login)      ,0,0,0 },
   { 3,  L"Name",          IMTDatasetColumn::TYPE_STRING     ,20,0, offsetof(DealRecord,name)           ,MtFieldSize(DealRecord,name)  ,0,0 },
   { 4,  L"Symbol",        IMTDatasetColumn::TYPE_STRING     ,10,0, offsetof(DealRecord,symbol)         ,MtFieldSize(DealRecord,symbol),0,0 },
   { 5,  L"Group",         IMTDatasetColumn::TYPE_STRING     ,15,0, offsetof(DealRecord,group)          ,MtFieldSize(DealRecord,group) ,0,0 },
   { 6,  L"Type",          IMTDatasetColumn::TYPE_DEAL_ACTION,10,0, offsetof(DealRecord,type)           ,0,0,0 },
   { 7,  L"Volume",        IMTDatasetColumn::TYPE_VOLUME_EXT ,10,0, offsetof(DealRecord,volume)         ,0,0,0 },
   { 8,  L"Price",         IMTDatasetColumn::TYPE_PRICE      ,10,0, offsetof(DealRecord,price)          ,0,0,0 },
   { 9,  L"Swap",          IMTDatasetColumn::TYPE_MONEY      ,10,0, offsetof(DealRecord,swap)           ,0,0,0 },
   {10,  L"Profit",        IMTDatasetColumn::TYPE_MONEY      ,10,0, offsetof(DealRecord,profit)         ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Account                                       |
//+------------------------------------------------------------------+
ReportColumn CDailyTradeReport::s_columns_account[]=
  {
   //--- id,name,          type,                               width,  width_max, offset,                size,digits_column,flags
     { 1,L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN    , 8,0, offsetof(AccountRecord,login)      ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Name",          IMTDatasetColumn::TYPE_STRING        ,20,0, offsetof(AccountRecord,name)       ,MtFieldSize(AccountRecord,name)  ,0,0 },
   { 3,  L"Group",         IMTDatasetColumn::TYPE_STRING        ,15,0, offsetof(AccountRecord,group)      ,MtFieldSize(AccountRecord,group) ,0,0 },
   { 4,  L"Leverage",      IMTDatasetColumn::TYPE_USER_LEVERAGE ,10,0, offsetof(AccountRecord,leverage)   ,0,0,0 },
   { 5,  L"Placed Orders", IMTDatasetColumn::TYPE_UINT32        ,10,0, offsetof(AccountRecord,placed_orders),0,0,0 },
   { 6,  L"Orders",        IMTDatasetColumn::TYPE_UINT32        ,10,0, offsetof(AccountRecord,orders)     ,0,0,0 },
   { 7,  L"Deals",         IMTDatasetColumn::TYPE_UINT32        ,10,0, offsetof(AccountRecord,deals)      ,0,0,0 },
   { 8,  L"Balance",       IMTDatasetColumn::TYPE_MONEY         ,10,0, offsetof(AccountRecord,balance)    ,0,0,0 },
   { 9,  L"Floating P/L",  IMTDatasetColumn::TYPE_MONEY         ,10,0, offsetof(AccountRecord,floating_pl),0,0,0 },
   {10,  L"Closed P/L",    IMTDatasetColumn::TYPE_MONEY         ,10,0, offsetof(AccountRecord,closed_pl)  ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Position                                      |
//+------------------------------------------------------------------+
ReportColumn CDailyTradeReport::s_columns_position[]=
  {
   //--- id,name,          type,                               width,  width_max, offset,                size,digits_column,flags
     { 1,L"Login",         IMTDatasetColumn::TYPE_USER_LOGIN , 8,0, offsetof(PositionRecord,login)        ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Ticket",        IMTDatasetColumn::TYPE_UINT64     , 8,0, offsetof(PositionRecord,ticket)       ,0,0,0 },
   { 3,  L"Name",          IMTDatasetColumn::TYPE_STRING     ,20,0, offsetof(PositionRecord,name)         ,MtFieldSize(PositionRecord,name)  ,0,0 },
   { 4,  L"Symbol",        IMTDatasetColumn::TYPE_STRING     ,10,0, offsetof(PositionRecord,symbol)       ,MtFieldSize(PositionRecord,symbol),0,0 },
   { 5,  L"Group",         IMTDatasetColumn::TYPE_STRING     ,15,0, offsetof(PositionRecord,group)        ,MtFieldSize(PositionRecord,group) ,0,0 },
   { 6,  L"Type",          IMTDatasetColumn::TYPE_POSITION_TYPE,10,0, offsetof(PositionRecord,type)       ,0,0,0 },
   { 7,  L"Volume",        IMTDatasetColumn::TYPE_VOLUME_EXT ,10,0, offsetof(PositionRecord,volume)       ,0,0,0 },
   { 8,  L"Open Price",    IMTDatasetColumn::TYPE_PRICE_POSITION,10,0, offsetof(PositionRecord,open_price)   ,0,0,0 },
   { 9,  L"S / L",         IMTDatasetColumn::TYPE_PRICE      ,10,0, offsetof(PositionRecord,sl)           ,0,0,0 },
   {10,  L"T / P",         IMTDatasetColumn::TYPE_PRICE      ,10,0, offsetof(PositionRecord,tp)           ,0,0,0 },
   {11,  L"Market Price",  IMTDatasetColumn::TYPE_PRICE      ,10,0, offsetof(PositionRecord,market_price) ,0,0,0 },
   {12,  L"Swap",          IMTDatasetColumn::TYPE_MONEY      ,10,0, offsetof(PositionRecord,swap)         ,0,0,0 },
   {13,  L"Points",        IMTDatasetColumn::TYPE_INT32      ,10,0, offsetof(PositionRecord,points)       ,0,0,0 },
   {14,  L"Profit",        IMTDatasetColumn::TYPE_MONEY      ,10,0, offsetof(PositionRecord,profit)       ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyTradeReport::CDailyTradeReport(void) : m_api(NULL),m_digits(0),
                                       m_top_profit_deals(0),m_top_loss_deals(0),m_top_profit_positions(0),m_top_loss_positions(0),m_top_profit_accounts(0),
                                       m_user(NULL),m_group(NULL),m_positions(NULL),m_orders(NULL),
                                       m_chart_profit(NULL),m_chart_number(NULL),m_chart_position(NULL),
                                       m_position(NULL),m_postion_total_loss(0.0),m_postion_total_profit(0.0),
                                       m_deal(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyTradeReport::~CDailyTradeReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDailyTradeReport::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CDailyTradeReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res=MT_RET_OK;
//--- checks
   if(api==NULL)
      return(MT_RET_ERR_PARAMS);
   if(type!=MTReportInfo::TYPE_HTML && type!=MTReportInfo::TYPE_DASHBOARD)
      res=MT_RET_ERR_NOTIMPLEMENT;
//--- save api pointer
   m_api=api;
   Clear();
//--- create positions interfaces
   if(res==MT_RET_OK)
      if(!CreateInterfaces())
         res=MT_RET_ERR_MEM;
//--- get report parameters
   if(res==MT_RET_OK)
      res=GetParameters();
//--- load info into arrays
   if(res==MT_RET_OK)
      res=LoadInfo();
//--- prepare html charts
   if(type==MTReportInfo::TYPE_HTML)
     {
      //--- prepare profit loss chart
      if(res==MT_RET_OK)
         res=PrepareProfitLossChart();
      //--- prepare chart
      if(res==MT_RET_OK)
         res=PrepareNumberChart();
      //--- prepare pie chart
      if(res==MT_RET_OK)
         res=PreparePositionsChart();
      //--- write all into html
      if(res==MT_RET_OK)
         res=WriteAll();
     }
   else
     {
      //--- generate dashboard
      if(res==MT_RET_OK)
         res=GenerateDashboard();
     }
//--- show error page if error exists
   switch(res)
     {
      //--- all right
      case MT_RET_OK:
         break;
         //--- logins array is empty
      case MT_RET_OK_NONE:
         if(type==MTReportInfo::TYPE_HTML)
            res=CReportError::Write(api,L"Daily Trade Report",L"Report is empty for the group, that you use.");
         break;
         //--- some error
      default:
         if(type==MTReportInfo::TYPE_HTML)
            res=CReportError::Write(api,L"Daily Trade Report",L"Report generation failed. For more information see server's journal.");
     }
//--- cleanup
   Clear();
   return(res);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDailyTradeReport::Clear(void)
  {
   if(m_chart_number)
     {
      m_chart_number->Release();
      m_chart_number=NULL;
     }
   if(m_chart_profit)
     {
      m_chart_profit->Release();
      m_chart_profit=NULL;
     }
   if(m_chart_position)
     {
      m_chart_position->Release();
      m_chart_position=NULL;
     }
   m_currency.Clear();
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
   if(m_position)
     {
      m_position->Release();
      m_position=NULL;
     }
   if(m_deal)
     {
      m_deal->Release();
      m_deal=NULL;
     }
   if(m_positions)
     {
      m_positions->Release();
      m_positions=NULL;
     }
   if(m_orders)
     {
      m_orders->Release();
      m_orders=NULL;
     }
   m_digits=0;
   m_top_profit_deals=0;
   m_top_loss_deals=0;
   m_top_profit_positions=0;
   m_top_loss_positions=0;
   m_top_profit_accounts=0;
   m_postion_total_loss  =0.0;
   m_postion_total_profit=0.0;
   m_daily_deals.Clear();
   m_positions_profit.Clear();
   m_positions_loss.Clear();
   m_accounts.Clear();
  }
//+------------------------------------------------------------------+
//| Create arrays interfaces                                         |
//+------------------------------------------------------------------+
bool CDailyTradeReport::CreateInterfaces(void)
  {
//--- clear old
   Clear();
//--- creating
   if((m_chart_number  =m_api->ChartCreate())        ==NULL) return(false);
   if((m_chart_profit  =m_api->ChartCreate())        ==NULL) return(false);
   if((m_chart_position=m_api->ChartCreate())        ==NULL) return(false);
   if((m_group         =m_api->GroupCreate())        ==NULL) return(false);
   if((m_user          =m_api->UserCreate())         ==NULL) return(false);
   if((m_position      =m_api->PositionCreate())     ==NULL) return(false);
   if((m_deal          =m_api->DealCreate())         ==NULL) return(false);
   if((m_positions     =m_api->PositionCreateArray())==NULL) return(false);
   if((m_orders        =m_api->OrderCreateArray())   ==NULL) return(false);
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Get report parameters                                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::GetParameters(void)
  {
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- parameters access initialization
   CReportParameter params;
   MTAPIRES res=params.Initialize(*m_api);
   if(res!=MT_RET_OK)
      return(res);
//--- get currency parameter value
   LPCWSTR currency=nullptr;
   if(params.ValueString(currency,L"Currency",DEFAULT_CURRENCY)!=MT_RET_OK)
      currency=DEFAULT_CURRENCY;
   m_currency.Assign(currency);
//--- currency digits
   m_digits=SMTMath::MoneyDigits(m_currency.Str());
//--- get top count report parameters values
   TopCount(params,m_top_profit_deals    ,PARAMETER_TOP_PROFIT_DEALS    );
   TopCount(params,m_top_loss_deals      ,PARAMETER_TOP_LOSS_DEALS      );
   TopCount(params,m_top_profit_positions,PARAMETER_TOP_PROFIT_POSITIONS);
   TopCount(params,m_top_loss_positions  ,PARAMETER_TOP_LOSS_POSITIONS  );
   TopCount(params,m_top_profit_accounts ,PARAMETER_TOP_PROFIT_ACCOUNTS );
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Load info into array                                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::LoadInfo(void)
  {
   MTAPIRES res;
//--- load positions into arrays
   if((res=LoadPositions())!=MT_RET_OK)
      return(res);
//--- load deals into arrays
   if((res=LoadDeals())!=MT_RET_OK)
      return(res);
//--- compute gistory of deals
   if((res=ComputeDailyDeals())!=MT_RET_OK)
      return(res);
//--- compute accounts
   if((res=ComputeAccounts())!=MT_RET_OK)
      return(res);
//--- sorting all
   m_accounts.Sort(SortByClosedPLAccount);
   m_deals_profit.Sort(SortByProfitDeal);
   m_deals_loss.Sort(SortByLossDeal);
   m_positions_loss.Sort(SortByLossPosition);
   m_positions_profit.Sort(SortByProfitPosition);
//--- all right
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Load positions into array                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::LoadPositions(void)
  {
   MTAPIRES          res;
   uint32_t          total;
   uint64_t*           logins   =NULL;
   IMTPositionArray* positions=NULL;
   IMTPosition*      position =NULL;
   PositionInfo      info;
//---
   if((positions=m_api->PositionCreateArray())==NULL) return(MT_RET_ERR_MEM);
//--- get all logins
   if((res=m_api->ParamLogins(logins,total))!=MT_RET_OK)
     {
      positions->Release();
      return(res);
     }
//--- if logins empty
   if(!total)
      return(MT_RET_OK_NONE);
//--- for all logins
   for(uint32_t i=0;i<total;i++)
     {
      //--- get all positions from login
      if((res=m_api->PositionGet(logins[i],positions))!=MT_RET_OK)
        {
         positions->Release();
         m_api->Free(logins);
         return(res);
        }
      if(!m_positions_profit.Reserve(m_positions_profit.Total()+positions->Total()) ||
         !m_positions_loss.Reserve(m_positions_loss.Total()+positions->Total())) return(MT_RET_ERR_MEM);
      //--- for all positions in array
      for(uint32_t j=0;j<positions->Total();j++)
        {
         position=NULL;
         if((position=positions->Next(j))==NULL)
           {
            m_api->Free(logins);
            positions->Release();
            return(MT_RET_OK);
           }
         //--- fill structure
         ZeroMemory(&info,sizeof(info));
         //--- convert currency for loss
         if((res=ConvertMoney(position->Profit(),logins[i],info.profit))!=MT_RET_OK)
           {
            positions->Release();
            m_api->Free(logins);
            return(res);
           }
         info.position=position->Position();
         //--- loss positions
         if(position->Profit()<0.0)
           {
            if(!m_positions_loss.Add(&info))
               return(MT_RET_ERR_MEM);
            m_postion_total_loss=SMTMath::MoneyAdd(m_postion_total_loss,info.profit,m_digits);
           }
         else // profit positions
           {
            if(!m_positions_profit.Add(&info))
               return(MT_RET_ERR_MEM);
            m_postion_total_profit=SMTMath::MoneyAdd(m_postion_total_profit,info.profit,m_digits);
           }
        }
     }
//--- free array
   m_api->Free(logins);
//--- all right
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Load deals into array                                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::LoadDeals(void)
  {
   MTAPIRES          res;
   uint32_t          total;
   uint64_t*           logins=NULL;
   IMTDealArray*     deals =NULL;
   IMTDeal*          deal  =NULL;
   DealInfo          info;
//---
   if((deals=m_api->DealCreateArray())==NULL) return(MT_RET_ERR_MEM);
//--- get all logins
   if((res=m_api->ParamLogins(logins,total))!=MT_RET_OK)
     {
      deals->Release();
      return(res);
     }
//--- if logins empty
   if(!total)
      return(MT_RET_OK_NONE);
//--- for all logins
   for(uint32_t i=0;i<total;i++)
     {
      //--- get all deals from login for a day
      if((res=m_api->DealGet(logins[i],m_api->ParamFrom(),m_api->ParamFrom()+SECONDS_IN_DAY,deals))!=MT_RET_OK)
        {
         deals->Release();
         m_api->Free(logins);
         return(res);
        }
      if(!m_deals_loss.Reserve(m_deals_loss.Total()+deals->Total()) ||
         !m_deals_profit.Reserve(m_deals_profit.Total()+deals->Total())) return(MT_RET_ERR_MEM);
      //--- for all deals in array
      for(uint32_t j=0;j<deals->Total();j++)
        {
         deal=NULL;
         if((deal=deals->Next(j))==NULL)
           {
            //--- free array
            m_api->Free(logins);
            deals->Release();
            return(MT_RET_OK);
           }
         //--- only for out buy/sell deals
         if((deal->Entry()!=IMTDeal::ENTRY_OUT && deal->Entry()!=IMTDeal::ENTRY_INOUT) ||
           !(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)) continue;
         //--- fill deal
         ZeroMemory(&info,sizeof(info));
         info.ticket=deal->Deal();
         info.login =deal->Login();
         //--- convert currency
         if((res=ConvertMoney(deal->Profit(),logins[i],info.profit))!=MT_RET_OK)
           {
            deals->Release();
            m_api->Free(logins);
            return(res);
           }
         //--- loss deals
         if(info.profit<0.0)
           {
            //---
            if(!m_deals_loss.Add(&info))
               return(MT_RET_ERR_MEM);
           }
         else // profit deals
           {
            //---
            if(!m_deals_profit.Add(&info))
               return(MT_RET_ERR_MEM);
           }
        }
     }
//--- free array
   m_api->Free(logins);
   deals->Release();
//--- all right
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Compute accounts                                                 |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::ComputeAccounts(void)
  {
   AccountInfo info;
   uint32_t    total=m_deals_profit.Total();
//---
   ZeroMemory(&info,sizeof(info));
//--- sort by login
   m_deals_profit.Sort(SortByLoginDeal);
//--- for all profit deals
   for(uint32_t i=0;i<total;i++)
     {
      //--- for first use
      if(info.login==0) info.login=m_deals_profit[i].login;
      //---
      if(info.login!=m_deals_profit[i].login)
        {
         if(info.closed_pl)
            if(!m_accounts.Add(&info))
               return(MT_RET_ERR_MEM);
         ZeroMemory(&info,sizeof(info));
         info.login=m_deals_profit[i].login;
        }
      if(info.login==m_deals_profit[i].login)
        {
         info.closed_pl+=m_deals_profit[i].profit;
         info.total_deals++;
        }
     }
   if(total && info.closed_pl)
      if(!m_accounts.Add(&info))
         return(MT_RET_ERR_MEM);
//--- all right
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Compute daily info for deals                                     |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::ComputeDailyDeals(void)
  {
   DailyDealsInfo deal_info;
   MTAPIRES       res;
   uint32_t       total;
   uint64_t*        logins   =NULL;
   IMTDealArray*  deals    =NULL;
   IMTDeal*       deal     =NULL;
   double         loss     =0.0;
   double         profit   =0.0;
//--- get all logins
   if((res=m_api->ParamLogins(logins,total))!=MT_RET_OK)
      return(res);
//--- if logins empty
   if(!total)
      return(MT_RET_OK_NONE);
//--- deal create array
   if((deals=m_api->DealCreateArray())==NULL)
     {
      m_api->Free(logins);
      return(MT_RET_ERR_MEM);
     }
//--- first day
   int64_t first_day=SMTTime::DayBegin(m_api->ParamFrom())-DAILY_DEALS_PERIOD;
//--- end day
   int64_t end_day  =SMTTime::DayBegin(m_api->ParamFrom())+SECONDS_IN_DAY;
//--- for all days
   for(int64_t day=first_day;day<end_day;day=day+SECONDS_IN_DAY)
     {
      //--- clear structure
      ZeroMemory(&deal_info,sizeof(deal_info));
      //--- current day
      deal_info.date=day;
      //--- for all logins
      for(uint32_t i=0;i<total;i++)
        {
         //--- get all deals from login
         if((res=m_api->DealGet(logins[i],day,day+SECONDS_IN_DAY,deals))!=MT_RET_OK)
           {
            deals->Release();
            m_api->Free(logins);
            return(res);
           }
         loss  =0.0;
         profit=0.0;
         //--- for all deals from array
         for(uint32_t j=0;j<deals->Total();j++)
           {
            deal=NULL;
            //--- next deal
            deal=deals->Next(j);
            if(deal==NULL)
               continue;
            if(deal->Entry()!=IMTDeal::ENTRY_OUT && deal->Entry()!=IMTDeal::ENTRY_INOUT)
               continue;
            if(!(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL))
               continue;
            //---
            if(deal->Profit()<0)
              {
               loss=SMTMath::MoneyAdd(loss,deal->Profit(),deal->DigitsCurrency());
               deal_info.loss_count++;
              }
            else
              {
               profit=SMTMath::MoneyAdd(profit,deal->Profit(),deal->DigitsCurrency());
               deal_info.profit_count++;
              }
           }
         //--- convert currency for loss
         double converted;
         if((res=ConvertMoney(loss,logins[i],converted))!=MT_RET_OK)
           {
            deals->Release();
            m_api->Free(logins);
            return(res);
           }
         deal_info.loss=SMTMath::MoneyAdd(deal_info.loss,converted,m_digits);
         //--- convert currency for profit
         if((res=ConvertMoney(profit,logins[i],converted))!=MT_RET_OK)
           {
            deals->Release();
            m_api->Free(logins);
            return(res);
           }
         deal_info.profit=SMTMath::MoneyAdd(deal_info.profit,converted,m_digits);
        }
      if(!m_daily_deals.Add(&deal_info))
         return(MT_RET_ERR_MEM);
     }
   m_api->Free(logins);
   deals->Release();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Convert money by currency                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::ConvertMoney(const double profit,const uint64_t login,double& converted)
  {
   MTAPIRES res;
   double   rate;
//--- zeroing result
   converted=0.0;
//--- checking profit
   if(!profit) return(MT_RET_OK);
//--- check arguments
   if(!m_user || !m_group) return(MT_RET_ERR_PARAMS);
//--- get user
   if((res=m_api->UserGetLight(login,m_user))!=MT_RET_OK)
      return(res);
//--- get user`s group
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(res);
//--- write group currency
   if((res=m_api->TradeRateSell(m_group->Currency(),m_currency.Str(),rate))!=MT_RET_OK)
      return(res);
//--- return result
   converted=SMTMath::PriceNormalize(profit*rate,m_digits);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare profit-loss chart                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::PrepareProfitLossChart(void)
  {
   IMTReportSeries *series;
   MTAPIRES         res;
   CMTStr256        str;
//--- checks
   if(m_api==NULL)    return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_chart_profit->Type(IMTReportChart::TYPE_GRAPH);
   m_chart_profit->Title(L"Profit and Loss of Clients");
   m_chart_profit->Digits(SMTMath::MoneyDigits(m_currency.Str()));
//--- prepare title series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_TITLE);
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- format date for current time
      str.Format(L"%02u.%02u.%04u",
         SMTTime::Day(m_daily_deals[i].date),
         SMTTime::Month(m_daily_deals[i].date),
         SMTTime::Year(m_daily_deals[i].date));
      //--- add value
      if((res=series->ValueAdd(str.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare profit series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_HISTOGRAM);
   series->Color(MTAPI_COLOR_PROFIT);
   series->Tooltip(L"Date: %VARIABLE%<BR>Profit: %VALUE%, %DESCRIPTION%");
   series->Title(L"Profit");
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt((int64_t)(m_daily_deals[i].profit)))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
      //--- add currency description
      if((res=series->ValueDescription(i,m_currency.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare loss series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_HISTOGRAM);
   series->Color(MTAPI_COLOR_LOSS);
   series->Tooltip(L"Date: %VARIABLE%<BR>Loss: %VALUE%, %DESCRIPTION%");
   series->Title(L"Loss");
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt((int64_t)(m_daily_deals[i].loss)))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
      //--- add currency description
      if((res=series->ValueDescription(i,m_currency.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare profit-loss series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_HISTOGRAM);
   series->Color(MTAPI_COLOR_NET_PROFIT);
   series->Tooltip(L"Date: %VARIABLE%<BR>Profit/Loss: %VALUE%, %DESCRIPTION%");
   series->Title(L"Profit/Loss");
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt((int64_t)(m_daily_deals[i].profit+m_daily_deals[i].loss)))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
      //--- add currency description
      if((res=series->ValueDescription(i,m_currency.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_profit->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare number of deals chart                                    |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::PrepareNumberChart(void)
  {
   IMTReportSeries *series;
   MTAPIRES         res;
   CMTStr256        str;
//--- checks
   if(m_api==NULL) return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_chart_number->Type(IMTReportChart::TYPE_GRAPH_ACCUMULATION);
   m_chart_number->Title(L"Number of Client Trades");
//--- prepare title series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_TITLE);
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- format date for current time
      str.Format(L"%02u.%02u.%04u",
         SMTTime::Day(m_daily_deals[i].date),
         SMTTime::Month(m_daily_deals[i].date),
         SMTTime::Year(m_daily_deals[i].date));
      //--- add value
      if((res=series->ValueAdd(str.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_number->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare profit series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_HISTOGRAM);
   series->Color(MTAPI_COLOR_PROFIT);
   series->Tooltip(L"Date: %VARIABLE%<BR>Number: %VALUE%");
   series->Title(L"Profit");
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_daily_deals[i].profit_count))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_number->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare loss series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_HISTOGRAM);
   series->Color(MTAPI_COLOR_LOSS);
   series->Tooltip(L"Date: %VARIABLE%<BR>Number: %VALUE%");
   series->Title(L"Loss");
//---
   for(uint32_t i=0;i<m_daily_deals.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_daily_deals[i].loss_count))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_chart_number->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare positions pie chart                                      |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::PreparePositionsChart(void)
  {
   IMTReportSeries* series=NULL;
   MTAPIRES         res;
   CMTStr64         str;
//--- prepare chart
   m_chart_position->Type(IMTReportChart::TYPE_PIE);
   m_chart_position->Title(L"Total Profit/Loss of Current Client Positions");
//--- setup pie tooltip format
   m_chart_position->PieceTooltip(L"%VARIABLE%: %VALUE%, %DESCRIPTION%");
//--- prepare profit series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Profit");
   series->Color(MTAPI_COLOR_PROFIT);
   series->ValueAddInt((int64_t)m_postion_total_profit);
//--- add description
   str.Format(L"%s<BR>Profit Positions: %u",m_currency.Str(),m_positions_profit.Total());
   series->ValueDescription(0,str.Str());
//--- add series (after adding series will be released by chart)
   if((res=m_chart_position->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare loss series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Loss");
   series->Color(MTAPI_COLOR_LOSS);
   series->ValueAddInt((int64_t)(-m_postion_total_loss));
//--- add description
   str.Format(L"%s<BR>Loss Positions: %u",m_currency.Str(),m_positions_loss.Total());
   series->ValueDescription(0,str.Str());
//--- add series (after adding series will be released by chart)
   if((res=m_chart_position->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write all into html                                              |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::WriteAll(void)
  {
   MTAPIRES      res;
   MTAPISTR      tag;
   uint32_t      counter;
   IMTConReport* report        =NULL;
//--- create report interface
   if((report=m_api->ReportCreate())==NULL) return(MT_RET_ERR_MEM);
//--- get report interface
   report->Clear();
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
      return(res);
//--- use template
   if((res=m_api->HtmlTplLoadResource(IDR_HTML_DAILY_TRADE_REPORT,RT_HTML))!=MT_RET_OK)
     {
      report->Release();
      return(res);
     }
//--- process tags
   while((res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- print the report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         if((res=m_api->HtmlWriteSafe(report->Name(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- print currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((res=m_api->HtmlWriteSafe(m_currency.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- Profit/Loss Deals chart
      if(CMTStr::CompareNoCase(tag,L"chart_profit_loss")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_profit))!=MT_RET_OK)
            break;
         continue;
        }
      //--- deals chart
      if(CMTStr::CompareNoCase(tag,L"chart_number_of_deals")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_number))!=MT_RET_OK)
            break;
         continue;
        }
      //--- position pie chart
      if(CMTStr::CompareNoCase(tag,L"chart_position_pie")==0)
        {
         if((res=m_api->ChartWriteHtml(m_chart_position))!=MT_RET_OK)
            break;
         continue;
        }
      //--- date
      if(CMTStr::CompareNoCase(tag,L"date")==0)
        {
         CMTStr256 str;
         str.Format(L"%02u.%02u.%04u",
             SMTTime::Day(m_api->ParamFrom()),
             SMTTime::Month(m_api->ParamFrom()),
             SMTTime::Year(m_api->ParamFrom()));
         if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
            break;
         continue;
        }
      //--- write top loss-profit deals tables
      if(WriteTopTrades(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write top loss-profit position tables
      if(WriteTopPosition(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
     }
   report->Release();
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write positions table                                            |
//+------------------------------------------------------------------+
bool CDailyTradeReport::WriteTablePositions(MTAPISTR& tag,MTAPIRES& res,const uint32_t counter,bool profit)
  {
   PositionInfo info;
//--- get deal for processing
   if(profit)
      info=m_positions_profit[counter];
   else
      info=m_positions_loss[counter];
//---
   if((res=m_api->PositionGetByTicket(info.position,m_position))!=MT_RET_OK)
      return(true);
//--- table tags
//--- login
   if(CMTStr::CompareNoCase(tag,L"login")==0)
     {
      res=m_api->HtmlWrite(L"%I64u",m_position->Login());
      return(true);
     }
//--- ticket
   if(CMTStr::CompareNoCase(tag,L"ticket")==0)
     {
      res=m_api->HtmlWrite(L"%I64u",m_position->Position());
      return(true);
     }
//--- symbol
   if(CMTStr::CompareNoCase(tag,L"symbol")==0)
     {
      res=m_api->HtmlWriteSafe(m_position->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- group
   if(CMTStr::CompareNoCase(tag,L"group")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(m_position->Login(),m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWriteSafe(m_user->Group(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- name
   if(CMTStr::CompareNoCase(tag,L"name")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(m_position->Login(),m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWriteSafe(m_user->Name(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- type
   if(CMTStr::CompareNoCase(tag,L"type")==0)
     {
      if(m_position->Action()==IMTPosition::POSITION_BUY)
         res=m_api->HtmlWrite(L"buy");
      else
         res=m_api->HtmlWrite(L"sell");
      return(true);
     }
//--- print the color of string
   if(CMTStr::CompareNoCase(tag,L"line")==0)
     {
      res=m_api->HtmlWrite(L"%u",counter%2);
      return(true);
     }
//--- volume
   if(CMTStr::CompareNoCase(tag,L"volume")==0)
     {
      CMTStr32 size;
      SMTFormat::FormatVolumeExt(size,m_position->VolumeExt());
      res=m_api->HtmlWriteString(size.Str());
      return(true);
     }
//--- open price
   if(CMTStr::CompareNoCase(tag,L"open_price")==0)
     {
      CMTStr16 op;
      SMTFormat::FormatPrice(op,m_position->PriceOpen(),m_position->Digits());
      res=m_api->HtmlWriteString(op.Str());
      return(true);
     }
//--- sl
   if(CMTStr::CompareNoCase(tag,L"sl")==0)
     {
      CMTStr16 sl;
      SMTFormat::FormatPrice(sl,m_position->PriceSL(),m_position->Digits());
      res=m_api->HtmlWriteString(sl.Str());
      return(true);
     }
//--- tp
   if(CMTStr::CompareNoCase(tag,L"tp")==0)
     {
      CMTStr16 tp;
      SMTFormat::FormatPrice(tp,m_position->PriceTP(),m_position->Digits());
      res=m_api->HtmlWriteString(tp.Str());
      return(true);
     }
//--- market price
   if(CMTStr::CompareNoCase(tag,L"market_price")==0)
     {
      CMTStr16 mp;
      SMTFormat::FormatPrice(mp,m_position->PriceCurrent(),m_position->Digits());
      res=m_api->HtmlWriteString(mp.Str());
      return(true);
     }
//--- swap
   if(CMTStr::CompareNoCase(tag,L"swap")==0)
     {
      CMTStr16 swap;
      SMTFormat::FormatMoney(swap,m_position->Storage(),m_position->DigitsCurrency());
      res=m_api->HtmlWriteString(swap.Str());
      return(true);
     }
//--- points
   if(CMTStr::CompareNoCase(tag,L"points")==0)
     {
      int32_t points;
      //--- calculate profit in points
      if(m_position->Action()==IMTPosition::POSITION_BUY)
         points=(int)SMTMath::PriceNormalize((m_position->PriceCurrent()-m_position->PriceOpen())*SMTMath::DecPow(m_position->Digits()),0);
      else
         points=(int)SMTMath::PriceNormalize((m_position->PriceOpen()-m_position->PriceCurrent())*SMTMath::DecPow(m_position->Digits()),0);
      //--- write points profit
      res=m_api->HtmlWrite(L"%d",points);
      return(true);
     }
//--- profit
   if(CMTStr::CompareNoCase(tag,L"profit")==0)
     {
      CMTStr16 profit;
      SMTFormat::FormatMoney(profit,info.profit,CURRENCY_DIGITS);
      res=m_api->HtmlWriteString(profit.Str());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write deals table                                                |
//+------------------------------------------------------------------+
bool CDailyTradeReport::WriteTableDeals(MTAPISTR& tag,MTAPIRES& res,const uint32_t counter,bool profit)
  {
   DealInfo info;
//--- get deal for processing
   if(profit)
      info=m_deals_profit[counter];
   else
      info=m_deals_loss[counter];
//---
   if((res=m_api->DealGet(info.ticket,m_deal))!=MT_RET_OK)
      return(true);
//--- table tags
//--- deal
   if(CMTStr::CompareNoCase(tag,L"deal")==0)
     {
      res=m_api->HtmlWrite(L"%I64u",info.ticket);
      return(true);
     }
//--- login
   if(CMTStr::CompareNoCase(tag,L"login")==0)
     {
      res=m_api->HtmlWrite(L"%I64u",m_deal->Login());
      return(true);
     }
//--- name
   if(CMTStr::CompareNoCase(tag,L"name")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(m_deal->Login(),m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWriteSafe(m_user->Name(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- symbol
   if(CMTStr::CompareNoCase(tag,L"symbol")==0)
     {
      res=m_api->HtmlWriteSafe(m_deal->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- group
   if(CMTStr::CompareNoCase(tag,L"group")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(m_deal->Login(),m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWriteSafe(m_user->Group(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- type
   if(CMTStr::CompareNoCase(tag,L"type")==0)
     {
      if(m_deal->Action()==IMTDeal::DEAL_BUY)
         res=m_api->HtmlWrite(L"buy");
      else
         res=m_api->HtmlWrite(L"sell");
      return(true);
     }
//--- volume
   if(CMTStr::CompareNoCase(tag,L"volume")==0)
     {
      CMTStr32 size;
      SMTFormat::FormatVolumeExt(size,m_deal->VolumeExt());
      res=m_api->HtmlWriteString(size.Str());
      return(true);
     }
//--- open price
   if(CMTStr::CompareNoCase(tag,L"price")==0)
     {
      CMTStr16 op;
      SMTFormat::FormatPrice(op,m_deal->Price(),m_deal->Digits());
      res=m_api->HtmlWriteString(op.Str());
      return(true);
     }
//--- swap
   if(CMTStr::CompareNoCase(tag,L"swap")==0)
     {
      CMTStr16 swap;
      SMTFormat::FormatMoney(swap,m_deal->Storage(),m_deal->DigitsCurrency());
      res=m_api->HtmlWriteString(swap.Str());
      return(true);
     }
//--- profit
   if(CMTStr::CompareNoCase(tag,L"profit")==0)
     {
      CMTStr16 profit;
      SMTFormat::FormatMoney(profit,info.profit,CURRENCY_DIGITS);
      res=m_api->HtmlWriteString(profit.Str());
      return(true);
     }
//--- print the color of string
   if(CMTStr::CompareNoCase(tag,L"line")==0)
     {
      res=m_api->HtmlWrite(L"%u",counter%2);
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write top deals                                                  |
//+------------------------------------------------------------------+
bool CDailyTradeReport::WriteTopTrades(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
//--- if no profit
   if(CMTStr::CompareNoCase(tag,L"no_top_profit_deal")==0 && counter==0 && m_deals_profit.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no loss
   if(CMTStr::CompareNoCase(tag,L"no_top_loss_deal")==0 && counter==0 && m_deals_loss.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no profit account
   if(CMTStr::CompareNoCase(tag,L"no_top_accounts")==0 && counter==0 && m_accounts.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- top profits
   while(CMTStr::CompareNoCase(tag,L"top_profit_deal")==0 && counter<m_deals_profit.Total() && counter<m_top_profit_deals)
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         if(!WriteTableDeals(tag,retcode,save_counter,true))
            break;
         if(retcode!=MT_RET_OK) return(true);
        }
     }
//--- top loss
   while(CMTStr::CompareNoCase(tag,L"top_loss_deal")==0 && counter<m_deals_loss.Total() && counter<m_top_loss_deals)
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         if(!WriteTableDeals(tag,retcode,save_counter,false))
            break;
         if(retcode!=MT_RET_OK) return(true);
        }
     }
//--- top accounts
   while(CMTStr::CompareNoCase(tag,L"top_accounts")==0 && counter<m_accounts.Total() && counter<m_top_profit_accounts)
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         if(!WriteTableAccounts(tag,retcode,save_counter,true))
            break;
         if(retcode!=MT_RET_OK) return(true);
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write deals table                                                |
//+------------------------------------------------------------------+
bool CDailyTradeReport::WriteTableAccounts(MTAPISTR& tag,MTAPIRES& res,const uint32_t counter,bool profit)
  {
   AccountInfo info;
//--- get account for processing
   info=m_accounts[counter];
//--- table tags
//--- login
   if(CMTStr::CompareNoCase(tag,L"login")==0)
     {
      res=m_api->HtmlWrite(L"%I64u",info.login);
      return(true);
     }
//--- name
   if(CMTStr::CompareNoCase(tag,L"name")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(info.login,m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWriteSafe(m_user->Name(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- leverage
   if(CMTStr::CompareNoCase(tag,L"leverage")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(info.login,m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWrite(L"1 : %u",m_user->Leverage());
      return(true);
     }
//--- balance
   if(CMTStr::CompareNoCase(tag,L"balance")==0)
     {
      double   converted;
      CMTStr32 balance;
      //--- get user
      if((res=m_api->UserGetLight(info.login,m_user))!=MT_RET_OK)
         return(true);
      //--- convert currency for loss
      if((res=ConvertMoney(m_user->Balance(),info.login,converted))!=MT_RET_OK)
         return(true);
      SMTFormat::FormatMoney(balance,converted,CURRENCY_DIGITS);
      res=m_api->HtmlWriteString(balance.Str());
      return(true);
     }
//--- group
   if(CMTStr::CompareNoCase(tag,L"group")==0)
     {
      //--- get user
      if((res=m_api->UserGetLight(info.login,m_user))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWriteSafe(m_user->Group(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- deals total
   if(CMTStr::CompareNoCase(tag,L"deals")==0)
     {
      res=m_api->HtmlWrite(L"%u",info.total_deals);
      return(true);
     }
//--- placed orders
   if(CMTStr::CompareNoCase(tag,L"placed_orders")==0)
     {
      if((res=m_api->OrderGet(info.login,m_orders))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWrite(L"%u",m_orders->Total());
      return(true);
     }
//--- orders
   if(CMTStr::CompareNoCase(tag,L"orders")==0)
     {
      if((res=m_api->HistoryGet(info.login,m_api->ParamFrom(),m_api->ParamFrom()+SECONDS_IN_DAY,m_orders))!=MT_RET_OK)
         return(true);
      res=m_api->HtmlWrite(L"%u",m_orders->Total());
      return(true);
     }
//--- closed p/l
   if(CMTStr::CompareNoCase(tag,L"closed_pl")==0)
     {
      CMTStr32 pl;
      SMTFormat::FormatMoney(pl,info.closed_pl,CURRENCY_DIGITS);
      res=m_api->HtmlWriteString(pl.Str());
      return(true);
     }
//--- floating p/l
   if(CMTStr::CompareNoCase(tag,L"floating_pl")==0)
     {
      CMTStr32          pl;
      double            profit   =0.0;
      double            converted;
      if((res=m_api->PositionGet(info.login,m_positions))!=MT_RET_OK)
         return(true);
      for(uint32_t i=0;i<m_positions->Total();i++)
         profit+=m_positions->Next(i)->Profit();
      //--- convert currency for loss
      if((res=ConvertMoney(profit,info.login,converted))!=MT_RET_OK)
         return(true);
      SMTFormat::FormatMoney(pl,converted,CURRENCY_DIGITS);
      res=m_api->HtmlWriteString(pl.Str());
      return(true);
     }
//--- print the color of string
   if(CMTStr::CompareNoCase(tag,L"line")==0)
     {
      res=m_api->HtmlWrite(L"%u",counter%2);
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write top positions                                              |
//+------------------------------------------------------------------+
bool CDailyTradeReport::WriteTopPosition(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
//--- if no profit
   if(CMTStr::CompareNoCase(tag,L"no_top_profit_position")==0 && counter==0 && m_positions_profit.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- if no loss
   if(CMTStr::CompareNoCase(tag,L"no_top_loss_position")==0 && counter==0 && m_positions_loss.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- top profits
   while(CMTStr::CompareNoCase(tag,L"top_profit_position")==0 && counter<m_positions_profit.Total() && counter<m_top_profit_positions)
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         if(!WriteTablePositions(tag,retcode,save_counter,true))
            break;
         if(retcode!=MT_RET_OK) return(true);
        }
     }
//--- top loss
   while(CMTStr::CompareNoCase(tag,L"top_loss_position")==0 && counter<m_positions_loss.Total() && counter<m_top_loss_positions)
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         if(!WriteTablePositions(tag,retcode,save_counter,false))
            break;
         if(retcode!=MT_RET_OK) return(true);
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Generate dashboard report                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::GenerateDashboard(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- daily chart data
   MTAPIRES res=MT_RET_OK;
   IMTDataset *data=ChartDailyData(res);
   if(!data)
      return(res);
//--- append currency to chart name
   CMTStr128 str(L"Profit and Loss of Clients");
   AppendCurrency(str);
//--- add daily charts
   if((res=AddDashboardChart(data,str.Str(),true,false,2,3))!=MT_RET_OK)
      return(res);
   if((res=AddDashboardChart(data,L"Number of Client Trades",true,true,5,2))!=MT_RET_OK)
      return(res);
//--- add table profit deal
   if((res=AddTableDeal(L"Top Profit Deals",m_deals_profit,m_top_profit_deals))!=MT_RET_OK)
      return(res);
//--- add table loss deal
   if((res=AddTableDeal(L"Top Loss Deals",m_deals_loss,m_top_loss_deals))!=MT_RET_OK)
      return(res);
//--- add table account
   if((res=AddTableAccount())!=MT_RET_OK)
      return(res);
//--- add total chart
   if((res=AddChartTotal())!=MT_RET_OK)
      return(res);
//--- add table profit positions
   if((res=AddTablePositions(L"Top Profit Positions",m_positions_profit,m_top_profit_positions))!=MT_RET_OK)
      return(res);
//--- add table loss positions
   if((res=AddTablePositions(L"Top Loss Positions",m_positions_loss,m_top_loss_positions))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Generate dashboard chart daily data                              |
//+------------------------------------------------------------------+
IMTDataset* CDailyTradeReport::ChartDailyData(MTAPIRES &res)
  {
//--- checks
   if(m_api==NULL)
     {
      res=MT_RET_ERR_PARAMS;
      return(nullptr);
     }
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
     {
      res=MT_RET_ERR_MEM;
      return(nullptr);
     }
//--- create dataset columns
   if((res=ReportColumn::ReportColumnsAdd(*data,s_columns_daily,_countof(s_columns_daily)))!=MT_RET_OK)
      return(nullptr);
//--- for all deals
   for(uint32_t i=0,total=m_daily_deals.Total();i<total;i++)
     {
      //--- fill record
      const DailyDealsInfo &info=m_daily_deals[i];
      DailyRecord row;
      row.date=info.date;
      row.profit=info.profit;
      row.loss=info.loss;
      row.profit_loss=info.profit+info.loss;
      row.profit_count=info.profit_count;
      row.loss_count=info.loss_count;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(nullptr);
     }
//--- add builds charts
   return(data);
  }
//+------------------------------------------------------------------+
//| Generate dashboard chart total                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::AddChartTotal(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_total,_countof(s_columns_total));
   if(res!=MT_RET_OK)
      return(res);
//--- fill record
   TotalRecord row;
   CMTStr::Copy(row.type,L"Profit");
   row.value=m_postion_total_profit;
//--- write row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
   CMTStr::Copy(row.type,L"Loss");
   row.value=-m_postion_total_loss;
//--- write row
   if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
      return(res);
//--- append currency to chart name
   CMTStr128 str(L"Total Profit/Loss of Current Client Positions");
   AppendCurrency(str);
//--- add total chart
   return(AddDashboardChart(data,str.Str(),false,false,0,0));
  }
//+------------------------------------------------------------------+
//| Add Chart                                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::AddDashboardChart(IMTDataset *data,LPCWSTR title,bool bar,bool acc,uint32_t column_id,uint32_t column_count)
  {
//--- checks
   if(m_api==NULL || data==NULL || title==NULL)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(bar ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE : IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE);
   if(res!=MT_RET_OK)
      return(res);
//--- set accumulation
   if(acc)
      if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
         return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- add columns
   for(uint32_t column=0;column<column_count;column++,column_id++)
      if((res=chart->DataColumnAdd(column_id))!=MT_RET_OK)
         return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Generate dashboard table deals                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::AddTableDeal(LPCWSTR title,const DealsArray &deals,const uint32_t top_count)
  {
//--- checks
   if(m_api==NULL || m_deal==NULL || m_user==NULL || title==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_deal,_countof(s_columns_deal));
   if(res!=MT_RET_OK)
      return(res);
//--- for all deals
   for(uint32_t i=0,total=std::min(deals.Total(),top_count);i<total;i++)
     {
      //--- deal info
      const DealInfo &info=deals[i];
      if((res=m_api->DealGet(info.ticket,m_deal))!=MT_RET_OK)
         continue;
      //--- fill record
      DealRecord row={0};
      row.deal=info.ticket;
      row.login=m_deal->Login();
      //--- get user
      if((res=m_api->UserGetLight(m_deal->Login(),m_user))==MT_RET_OK)
        {
         //--- user name
         if(LPCWSTR name=m_user->Name())
            CMTStr::Copy(row.name,name);
         //--- user group
         if(LPCWSTR group=m_user->Group())
            CMTStr::Copy(row.group,group);
        }
      //--- symbol
      if(LPCWSTR symbol=m_deal->Symbol())
         CMTStr::Copy(row.symbol,symbol);
      //-- other fields
      row.type=m_deal->Action();
      row.volume=m_deal->VolumeExt();
      row.price=m_deal->Price();
      row.swap=m_deal->Storage();
      row.profit=info.profit;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data,title) ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard table accounts                                |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::AddTableAccount(void)
  {
//--- checks
   if(m_api==NULL || m_orders==NULL || m_user==NULL || m_positions==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_account,_countof(s_columns_account));
   if(res!=MT_RET_OK)
      return(res);
//--- for all accounts
   for(uint32_t i=0,total=std::min(m_accounts.Total(),m_top_profit_accounts);i<total;i++)
     {
      //--- account info
      const AccountInfo &info=m_accounts[i];
      //--- fill record
      AccountRecord row={0};
      row.login=info.login;
      //--- get user
      if((res=m_api->UserGetLight(info.login,m_user))==MT_RET_OK)
        {
         //--- user name
         if(LPCWSTR name=m_user->Name())
            CMTStr::Copy(row.name,name);
         //--- user group
         if(LPCWSTR group=m_user->Group())
            CMTStr::Copy(row.group,group);
         //--- leversage
         row.leverage=m_user->Leverage();
         //--- convert currency for loss
         ConvertMoney(m_user->Balance(),info.login,row.balance);
        }
      //--- orders
      if((res=m_api->OrderGet(info.login,m_orders))==MT_RET_OK)
         row.placed_orders=m_orders->Total();
      if((res=m_api->HistoryGet(info.login,m_api->ParamFrom(),m_api->ParamFrom()+SECONDS_IN_DAY,m_orders))==MT_RET_OK)
         row.orders=m_orders->Total();
      //--- other fields
      row.deals=info.total_deals;
      row.closed_pl=info.closed_pl;
      //--- floating p/l
      double profit=0.0;
      if((res=m_api->PositionGet(info.login,m_positions))==MT_RET_OK)
        {
         for(uint32_t i=0;i<m_positions->Total();i++)
            profit+=m_positions->Next(i)->Profit();
         //--- convert currency for loss
         ConvertMoney(profit,info.login,row.floating_pl);
        }
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data,L"Top Profit Accounts") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard table positions                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::AddTablePositions(LPCWSTR title,const PositionArray &positions,const uint32_t top_count)
  {
//--- checks
   if(m_api==NULL || m_position==NULL || m_user==NULL || title==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_position,_countof(s_columns_position));
   if(res!=MT_RET_OK)
      return(res);
//--- for all positions
   for(uint32_t i=0,total=std::min(positions.Total(),top_count);i<total;i++)
     {
      //--- position info
      const PositionInfo &info=positions[i];
      if((res=m_api->PositionGetByTicket(info.position,m_position))!=MT_RET_OK)
         continue;
      //--- fill record
      PositionRecord row={0};
      row.login=m_position->Login();
      row.ticket=m_position->Position();
      //--- get user
      if((res=m_api->UserGetLight(m_position->Login(),m_user))==MT_RET_OK)
        {
         //--- user name
         if(LPCWSTR name=m_user->Name())
            CMTStr::Copy(row.name,name);
         //--- user group
         if(LPCWSTR group=m_user->Group())
            CMTStr::Copy(row.group,group);
        }
      //--- symbol
      if(LPCWSTR symbol=m_position->Symbol())
         CMTStr::Copy(row.symbol,symbol);
      //--- other fields
      row.type=m_position->Action();
      row.volume=m_position->VolumeExt();
      row.open_price=m_position->PriceOpen();
      row.sl=m_position->PriceSL();
      row.tp=m_position->PriceTP();
      row.market_price=m_position->PriceCurrent();
      row.swap=m_position->Storage();
      //--- calculate profit in points
      if(m_position->Action()==IMTPosition::POSITION_BUY)
         row.points=(int)SMTMath::PriceNormalize((m_position->PriceCurrent()-m_position->PriceOpen())*SMTMath::DecPow(m_position->Digits()),0);
      else
         row.points=(int)SMTMath::PriceNormalize((m_position->PriceOpen()-m_position->PriceCurrent())*SMTMath::DecPow(m_position->Digits()),0);
      //--- profit
      row.profit=info.profit;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data,title) ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| get top count report parameter value                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyTradeReport::TopCount(CReportParameter &params,uint32_t &top_count,LPCWSTR param_name)
  {
//--- checks
   if(!param_name || !*param_name)
      return(MT_RET_ERR_PARAMS);
//--- get top count from report parameters
   int64_t value=0;
   const MTAPIRES res=params.ValueInt(value,param_name,DEFAULT_TOP_COUNT);
   if(res!=MT_RET_OK)
      return(res);
//--- check top count
   if(value<0 || value>TOP_COUNT_MAX)
      return(MT_RET_ERR_PARAMS);
//--- check for empty
   top_count=(uint32_t)value;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Append currency to string                                        |
//+------------------------------------------------------------------+
void CDailyTradeReport::AppendCurrency(CMTStr &str) const
  {
   if(!m_currency || !m_currency.Str() || !*m_currency.Str())
      return;
//--- append currency to string
   str.Append(L", ");
   str.Append(m_currency.Str());
  }
//+------------------------------------------------------------------+
//| Sort by profit position                                          |
//+------------------------------------------------------------------+
int32_t CDailyTradeReport::SortByProfitPosition(const void *left, const void *right)
  {
   PositionInfo* pos_left =(PositionInfo*)left;
   PositionInfo* pos_right=(PositionInfo*)right;
//---
   if(pos_right->profit>pos_left->profit) return(1);
   if(pos_right->profit<pos_left->profit) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort by loss position                                            |
//+------------------------------------------------------------------+
int32_t CDailyTradeReport::SortByLossPosition(const void *left, const void *right)
  {
   PositionInfo* pos_left =(PositionInfo*)left;
   PositionInfo* pos_right=(PositionInfo*)right;
//---
   if(pos_right->profit>pos_left->profit) return(-1);
   if(pos_right->profit<pos_left->profit) return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort by loss deal                                                |
//+------------------------------------------------------------------+
int32_t CDailyTradeReport::SortByLossDeal(const void *left, const void *right)
  {
   DealInfo* deal_left =(DealInfo*)left;
   DealInfo* deal_right=(DealInfo*)right;
//---
   if(deal_right->profit>deal_left->profit) return(-1);
   if(deal_right->profit<deal_left->profit) return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort deal by login                                               |
//+------------------------------------------------------------------+
int32_t CDailyTradeReport::SortByLoginDeal(const void *left, const void *right)
  {
   DealInfo* deal_left =(DealInfo*)left;
   DealInfo* deal_right=(DealInfo*)right;
//---
   if(deal_right->login>deal_left->login) return(1);
   if(deal_right->login<deal_left->login) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort by profit deal                                              |
//+------------------------------------------------------------------+
int32_t CDailyTradeReport::SortByProfitDeal(const void *left, const void *right)
  {
   DealInfo* deal_left =(DealInfo*)left;
   DealInfo* deal_right=(DealInfo*)right;
//---
   if(deal_right->profit>deal_left->profit) return(1);
   if(deal_right->profit<deal_left->profit) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort by closed p/l account                                       |
//+------------------------------------------------------------------+
int32_t CDailyTradeReport::SortByClosedPLAccount(const void *left, const void *right)
  {
   AccountInfo* acc_left =(AccountInfo*)left;
   AccountInfo* acc_right=(AccountInfo*)right;
//---
   if(acc_right->closed_pl>acc_left->closed_pl) return(1);
   if(acc_right->closed_pl<acc_left->closed_pl) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
