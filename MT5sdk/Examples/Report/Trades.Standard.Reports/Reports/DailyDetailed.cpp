//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyDetailed.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CDailyDetailed::s_info=
{
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Daily Reports Detailed",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_HTML,
   L"Trades",
                   // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
     },2            // params_total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyDetailed::CDailyDetailed(void) : m_api(NULL),m_orders(NULL),
                                       m_daily(NULL),m_deals(NULL),m_group(NULL),m_server(NULL),m_report(NULL),
                                       m_user(NULL),m_position(NULL),m_order(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyDetailed::~CDailyDetailed(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDailyDetailed::Info(MTReportInfo& info)
  {
//--- get info information  
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CDailyDetailed::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
bool CDailyDetailed::Initialize(void)
  {
//--- check
   if(!m_api) return(false);
//--- create order array interface
   if(m_orders==NULL && (m_orders=m_api->OrderCreateArray())==NULL)
      return(false);
//--- create daily interface
   if(m_daily==NULL && (m_daily=m_api->DailyCreate())==NULL)
      return(false);
//--- create group interface
   if(m_group==NULL && (m_group=m_api->GroupCreate())==NULL)
      return(false);
//--- create server interface
   if(m_server==NULL && (m_server=m_api->NetServerCreate())==NULL)
      return(false);
//--- create user interface
   if(m_user==NULL && (m_user=m_api->UserCreate())==NULL)
      return(false);
//--- create report interface
   if(m_report==NULL && (m_report=m_api->ReportCreate())==NULL)
      return(false);
//--- create position interface
   if(m_position==NULL && (m_position=m_api->PositionCreate())==NULL)
      return(false);
//--- create order interface
   if(m_order==NULL && (m_order=m_api->OrderCreate())==NULL)
      return(false);
//--- create deal array interface
   if(m_deals==NULL && (m_deals=m_api->DealCreateArray())==NULL)
      return(false);
   return(true);
  }
//+------------------------------------------------------------------+
//| Load info for report                                             |
//+------------------------------------------------------------------+
bool CDailyDetailed::LoadInfo(MTAPIRES& res)
  {
   uint64_t        *logins =NULL;
   uint32_t       total  =0;
   IMTDailyArray *dailies=NULL;
//--- get report interface
   m_report->Clear();
   if((res=m_api->ReportCurrent(m_report))!=MT_RET_OK)
      return(false);
//--- get first login 
   if((res=m_api->ParamLogins(logins,total))!=MT_RET_OK)
      return(false);
//--- check logins
//--- parameter must be one login
   if(total>1 || total==0)
     {
      res=WriteErrorHtml(BAD_REQUEST);
      m_api->Free(logins);
      return(false);
     }
   m_user->Clear();
   if((res=m_api->UserGetLight(logins[0],m_user))!=MT_RET_OK)
     {
      m_api->Free(logins);
      return(false);
     }
   m_api->Free(logins);
//--- get group interface
   m_group->Clear();
   if((res=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
      return(false);
//--- get first daily
   if((dailies=m_api->DailyCreateArray())==NULL)
     {
      res=MT_RET_ERR_MEM;
      return(false);
     }
//--- request daily
   m_daily->Clear();
   if((res=m_api->DailyGet(m_user->Login(),m_api->ParamFrom(),m_api->TimeCurrent(),dailies))!=MT_RET_OK)
     {
      dailies->Release();
      return(false);
     }
//--- get first daily record
   if(dailies->Total()>0)
      m_daily->Assign(dailies->Next(0));
   else
     {
      //--- no daily records! no data for report!
      dailies->Release();
      res=WriteErrorHtml(DAILY_NOT_FOUND);
      return(false);
     }
   dailies->Release();
//--- check overnight mode
   uint32_t overnight_mode=IMTConServerTrade::OVERNIGHT_END_DAY;
   if(m_api->NetServerGet(m_group->Server(),m_server)==MT_RET_OK)
      if(m_server->TradeServer())
         overnight_mode=m_server->TradeServer()->OvernightMode();
//--- request history
   if(overnight_mode==IMTConServerTrade::OVERNIGHT_END_DAY)
     {
      //--- get closed orders array
      m_orders->Clear();
      if((res=m_api->HistoryGet(m_user->Login(),m_daily->DatetimePrev()+1,m_daily->Datetime(),m_orders))!=MT_RET_OK)
         return(false);
      //--- get history of deals
      m_deals->Clear();
      if((res=m_api->DealGet(m_user->Login(),m_daily->DatetimePrev()+1,m_daily->Datetime(),m_deals))!=MT_RET_OK)
         return(false);
     }
   else
     {
      //--- get closed orders array
      m_orders->Clear();
      if((res=m_api->HistoryGet(m_user->Login(),m_daily->DatetimePrev()+2,m_daily->Datetime()+1,m_orders))!=MT_RET_OK)
         return(false);
      //--- get history of deals
      m_deals->Clear();
      if((res=m_api->DealGet(m_user->Login(),m_daily->DatetimePrev()+2,m_daily->Datetime()+1,m_deals))!=MT_RET_OK)
         return(false);
     }
//--- ok
   res=MT_RET_OK;
   return(true);
  }
//+------------------------------------------------------------------+
//| Write html error message                                         |
//+------------------------------------------------------------------+
MTAPIRES CDailyDetailed::WriteErrorHtml(int32_t type)
  {
   MTAPISTR tag;
   uint32_t counter=0;
   MTAPIRES res;
//--- load template from resource
   if((res=m_api->HtmlTplLoadResource(IDR_DAILY_DETAILED_ERROR,RT_HTML))!=MT_RET_OK)
      return(res);
//--- loop of process template tags
//--- tags in html have format <mt5:xxx>
//--- HtmlTplNext return tag string and counter,that shows how many times tag has been used
   while((res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- if bad user request
      if(CMTStr::CompareNoCase(tag,L"bad_request")==0 && counter==0 && type==BAD_REQUEST)
        {
         //--- print everything inside the <mt5:no_bad_request>..</mt5:no_bad_request> (bad request)
         if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- if daily not found for the given user
      if(CMTStr::CompareNoCase(tag,L"daily_not_found")==0 && counter==0 && type==DAILY_NOT_FOUND)
        {
         //--- print everything inside the <mt5:daily_not_found>..</mt5:daily_not_found> (Daily not found)
         if((res=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- print the report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         if((res=m_api->HtmlWriteSafe(m_report->Name(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            return(res);
         continue;
        }
     }
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Release interface                                                |
//+------------------------------------------------------------------+
void CDailyDetailed::Clear(void)
  {
//--- order array interface
   if(m_orders)
     {
      m_orders->Release();
      m_orders=NULL;
     }
//--- order array interface
   if(m_daily)
     {
      m_daily->Release();
      m_daily=NULL;
     }
//--- deal array interface
   if(m_deals)
     {
      m_deals->Release();
      m_deals=NULL;
     }
//--- group interface
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
//--- server interface
   if(m_server)
     {
      m_server->Release();
      m_server=NULL;
     }
//--- user interface
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
//--- report config interface
   if(m_report)
     {
      m_report->Release();
      m_report=NULL;
     }
//--- position interface
   if(m_position)
     {
      m_position->Release();
      m_position=NULL;
     }
//--- order interface
   if(m_order)
     {
      m_order->Release();
      m_order=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CDailyDetailed::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res;
//--- checks
   if(!api) return(MT_RET_ERR_PARAMS);
//--- checks
   if(type!=MTReportInfo::TYPE_HTML) return(MT_RET_ERR_NOTIMPLEMENT);
//--- save API pointer
   m_api=api;
//--- Initialize interface
   if(!Initialize()) return(MT_RET_ERR_MEM);
//--- load data for report
   if(!LoadInfo(res)) return(res);
//--- write html
   if((res=WriteHtml())!=MT_RET_OK) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write html                                                       |
//+------------------------------------------------------------------+
MTAPIRES CDailyDetailed::WriteHtml(void)
  {
   MTAPISTR tag;
   uint32_t counter=0;
   MTAPIRES res;
//--- load template from resource
   if((res=m_api->HtmlTplLoadResource(IDR_DAILY_DETAILED,RT_HTML))!=MT_RET_OK)
      return(res);
//--- loop of process template tags
//--- tags in html have format <mt5:xxx>
//--- HtmlTplNext return tag string and counter,that shows how many times tag has been used
   while((res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- write header html
      if(WriteHead(tag,res))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write table closed orders
      if(WriteClosedOrder(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write table closed deals
      if(WriteClosedDeals(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write summary after table closed deals
      if(WriteDealsSummary(tag,res))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write summary for orders and deals table
      if(WriteOrdersDealsSummary(tag,res))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write open position table
      if(WriteOpenPosition(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write summary after positions table 
      if(WritePositionsSummary(tag,res))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write open order table
      if(WriteOrder(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write total summary
      if(WriteTotalSummary(tag,res))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
     }
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Summary after positions table                                    |
//+------------------------------------------------------------------+
bool CDailyDetailed::WritePositionsSummary(MTAPISTR& tag,MTAPIRES& retcode)
  {
//---
   if(CMTStr::CompareNoCase(tag,L"floating_storage")==0)
     {
      CMTStr32 storage;
      SMTFormat::FormatMoney(storage,m_daily->ProfitStorage(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(storage.Str());
      return(true);
     }
//---
   if(CMTStr::CompareNoCase(tag,L"floating_profit")==0)
     {
      CMTStr32 profit;
      SMTFormat::FormatMoney(profit,m_daily->Profit(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(profit.Str());
      return(true);
     }
//---
   if(CMTStr::CompareNoCase(tag,L"floating_pl")==0)
     {
      CMTStr32 total;
      double value=0;
      //--- calculate floating profit
      value=SMTMath::MoneyAdd(value,m_daily->Profit(),          m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->ProfitStorage(),   m_daily->CurrencyDigits());
      //--- write floating profit
      SMTFormat::FormatMoney(total,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(total.Str());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Summary after deals table                                        |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteDealsSummary(MTAPISTR& tag,MTAPIRES& retcode)
  {
   CMTStr64 str;
//--- 
   if(CMTStr::CompareNoCase(tag,L"closed_commission_instant")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyCommInstant(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- 
   if(CMTStr::CompareNoCase(tag,L"closed_fee")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyCommFee(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- 
   if(CMTStr::CompareNoCase(tag,L"closed_storage")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyStorage(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- 
   if(CMTStr::CompareNoCase(tag,L"closed_profit")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyProfit(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write total summary on html                                      |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteTotalSummary(MTAPISTR& tag,MTAPIRES& retcode)
  {
   CMTStr64 str;
//--- print the Closed Trade P/L:
   if(CMTStr::CompareNoCase(tag,L"closed_pl")==0)
     {
      double value=0;
      //--- calculate closed p/l
      value=SMTMath::MoneyAdd(value,m_daily->DailyProfit(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyStorage(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommInstant(),m_daily->CurrencyDigits());
      //--- write closed p/l
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Previous Ledger Balance: 
   if(CMTStr::CompareNoCase(tag,L"prev_balance_daily")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->BalancePrevDay(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Deposit/Withdrawal:
   if(CMTStr::CompareNoCase(tag,L"closed_deposit")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyBalance(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Previous Equity: 
   if(CMTStr::CompareNoCase(tag,L"prev_equity_daily")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->EquityPrevDay(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Total Credit Facility:
   if(CMTStr::CompareNoCase(tag,L"credit")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->Credit(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Balance: 
   if(CMTStr::CompareNoCase(tag,L"balance")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->Balance(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Round Commission:
   if(CMTStr::CompareNoCase(tag,L"round_commission")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyCommRound(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Instant Commission: 
   if(CMTStr::CompareNoCase(tag,L"instant_commission")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyCommInstant(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Equity:
   if(CMTStr::CompareNoCase(tag,L"floating_equity")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->ProfitEquity(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Additional Operations:
   if(CMTStr::CompareNoCase(tag,L"additional")==0)
     {
      double value=0;
      //--- calculate additional operations
      value=SMTMath::MoneyAdd(value,m_daily->DailyCharge(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCorrection(),m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyBonus(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyAgent(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyInterest(),  m_daily->CurrencyDigits());
      //--- write additional operations
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Floating P/L:
   if(CMTStr::CompareNoCase(tag,L"floating_pl")==0)
     {
      double value=0;
      //--- calculate floating profit
      value=SMTMath::MoneyAdd(value,m_daily->Profit(),          m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->ProfitStorage(),   m_daily->CurrencyDigits());
      //--- write floating profit
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Total:
   if(CMTStr::CompareNoCase(tag,L"closed_total")==0)
     {
      double value=0;
      //--- calculate total
      value=SMTMath::MoneyAdd(value,m_daily->DailyProfit(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyStorage(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyBalance(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCredit(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCharge(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCorrection(), m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyBonus(),      m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommInstant(),m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommRound(),  m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyAgent(),      m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyInterest(),   m_daily->CurrencyDigits());
      //--- write total
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Margin Requirement:
   if(CMTStr::CompareNoCase(tag,L"margin")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->Margin(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Available Margin:
   if(CMTStr::CompareNoCase(tag,L"margin_free")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->MarginFree(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write closed orders and deals summary on html                    |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteOrdersDealsSummary(MTAPISTR& tag,MTAPIRES& retcode)
  {
   CMTStr64 str;
//--- print the Closed P/L: 
   if(CMTStr::CompareNoCase(tag,L"closed_pl")==0)
     {
      double value=0;
      //--- calculate closed p/l
      value=SMTMath::MoneyAdd(value,m_daily->DailyProfit(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyStorage(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommInstant(),m_daily->CurrencyDigits());
      //--- write closed p/l
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Deposit/Withdrawal:
   if(CMTStr::CompareNoCase(tag,L"closed_deposit")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyBalance(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Credit Facility:
   if(CMTStr::CompareNoCase(tag,L"closed_credit")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyCredit(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Round Commission: 
   if(CMTStr::CompareNoCase(tag,L"closed_commission_round")==0)
     {
      SMTFormat::FormatMoney(str,m_daily->DailyCommRound(),m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Additional Operations:
   if(CMTStr::CompareNoCase(tag,L"closed_additional")==0)
     {
      double value=0;
      //--- calculate additional operations
      value=SMTMath::MoneyAdd(value,m_daily->DailyCharge(),        m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCorrection(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyBonus(),         m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyAgent(),         m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyInterest(),      m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyDividend(),      m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyTaxes(),         m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailySOCompensation(),m_daily->CurrencyDigits());
      //--- write additional operations
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
//--- print the Total:
   if(CMTStr::CompareNoCase(tag,L"closed_total")==0)
     {
      double value=0;
      //--- calculate total
      value=SMTMath::MoneyAdd(value,m_daily->DailyProfit(),        m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyStorage(),       m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyBalance(),       m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCredit(),        m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCharge(),        m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCorrection(),    m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyBonus(),         m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommInstant(),   m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommRound(),     m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyCommFee(),       m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyAgent(),         m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyInterest(),      m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyDividend(),      m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailyTaxes(),         m_daily->CurrencyDigits());
      value=SMTMath::MoneyAdd(value,m_daily->DailySOCompensation(),m_daily->CurrencyDigits());
      //--- write total
      SMTFormat::FormatMoney(str,value,m_daily->CurrencyDigits());
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write head html                                                  |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteHead(MTAPISTR& tag,MTAPIRES& retcode)
  {
   CMTStr64 str;
//--- print the report name
   if(CMTStr::CompareNoCase(tag,L"report_name")==0)
     {
      retcode=m_api->HtmlWriteSafe(m_report->Name(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- print the company
   if(CMTStr::CompareNoCase(tag,L"company")==0)
     {
      retcode=m_api->HtmlWriteSafe(m_group->Company(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- print the A/C No:
   if(CMTStr::CompareNoCase(tag,L"account")==0)
     {
      retcode=m_api->HtmlWrite(L"%I64u",m_user->Login());
      return(true);
     }
//--- print the Name:
   if(CMTStr::CompareNoCase(tag,L"name")==0)
     {
      retcode=m_api->HtmlWriteSafe(m_user->Name(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- print the Currency:
   if(CMTStr::CompareNoCase(tag,L"currency")==0)
     {
      retcode=m_api->HtmlWriteSafe(m_group->Currency(),IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- print the daily time
   if(CMTStr::CompareNoCase(tag,L"fulltime")==0)
     {
      SMTFormat::FormatDateTime(str,m_daily->Datetime(),true,false);
      retcode=m_api->HtmlWriteString(str.Str());
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write daily order                                                |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteOrder(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64 str;
//--- are there no orders?
   bool notrans=(m_daily->OrderTotal()==0);
//--- block that is processed, when there are no orders
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_trans_order")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_order>..</mt5:no_trans_order> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:orders> tag (beginning tag of orders table) and
//--- do we have orders?
   if(CMTStr::CompareNoCase(tag,L"orders")==0 && !notrans && counter==0)
     {
      m_order->Clear();
      //--- get first order
      if((retcode=m_daily->OrderNext(counter,m_order))!=MT_RET_OK)
         return(true);
      //--- process tags inside <mt5:orders>..</mt5:orders> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- return to beginning of table - <mt5:orders>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"orders")==0)
           {
            //--- have any records left?
            if(counter<m_daily->OrderTotal())
              {
               //--- process tags inside <mt5:orders>..</mt5:orders> for the next record
               if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  return(true);
              }
            else
              {
               //--- no more records
               //--- don't repeat <mt5:orders>..</mt5:orders> block anymore (don't call HtmlTplProcess)
               retcode=MT_RET_OK;
               return(true);
              }
            //--- get next order
            m_order->Clear();
            if((retcode=m_daily->OrderNext(counter,m_order))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- process tags inside <mt5:orders>..</mt5:orders>
         //--- print the color of string
         if(CMTStr::CompareNoCase(tag,L"color")==0)
           {
            if(counter%2==0)
               retcode=m_api->HtmlWriteString(L"#FFFFFF");
            else
               retcode=m_api->HtmlWriteString(L"#E0E0E0");
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            SMTFormat::FormatDateTime(str,m_order->TimeSetup(),true,true);
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",m_order->Order());
            continue;
           }
         //--- print the Type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            SMTFormat::FormatOrderType(str,m_order->Type());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Size
         if(CMTStr::CompareNoCase(tag,L"size")==0)
           {
            SMTFormat::FormatVolumeExtOrder(str,m_order->VolumeInitialExt(),m_order->VolumeCurrentExt());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"item")==0)
           {
            retcode=m_api->HtmlWriteSafe(m_order->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"price")==0)
           {
            SMTFormat::FormatOrderPrice(str,m_order->PriceOrder(),m_order->PriceTrigger(),m_order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the S / L
         if(CMTStr::CompareNoCase(tag,L"sl")==0)
           {
            SMTFormat::FormatPrice(str,m_order->PriceSL(),m_order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the T / P
         if(CMTStr::CompareNoCase(tag,L"tp")==0)
           {
            SMTFormat::FormatPrice(str,m_order->PriceTP(),m_order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Market Price
         if(CMTStr::CompareNoCase(tag,L"market")==0)
           {
            SMTFormat::FormatPrice(str,m_order->PriceCurrent(),m_order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Comment
         if(CMTStr::CompareNoCase(tag,L"comment")==0)
           {
            retcode=m_api->HtmlWriteSafe(m_order->Comment(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write closed order                                               |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteClosedOrder(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64 str;
//--- are there no orders?
   bool notrans=(m_orders->Total()==0);
//--- block that is processed, when there are no orders
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_trans_closed")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_closed>..</mt5:no_trans_closed> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:closed_orders> tag (beginning tag of orders table) and
//--- do we have orders?
   if(CMTStr::CompareNoCase(tag,L"closed_orders")==0 && !notrans && counter==0)
     {
      IMTOrder *order;
      //--- get first order
      if((order=m_orders->Next(counter))==NULL)
        {
         retcode=MT_RET_ERR_PARAMS;
         return(true);
        }
      //--- process tags inside <mt5:closed_orders>..</mt5:closed_orders> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- return to beginning of table - <mt5:closed_orders>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"closed_orders")==0)
           {
            //--- have any records left?
            if(counter<m_orders->Total())
              {
               //--- process tags inside <mt5:closed_orders>..</mt5:closed_orders> for the next record
               if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  return(true);
              }
            else
              {
               //--- no more records
               //--- don't repeat <mt5:closed_orders>..</mt5:closed_orders> block anymore (don't call HtmlTplProcess)
               retcode=MT_RET_OK;
               return(true);
              }
            //--- get next order
            if((order=m_orders->Next(counter))==NULL)
              {
               retcode=MT_RET_ERR_PARAMS;
               return(true);
              }
            continue;
           }
         //--- process tags inside <mt5:closed_orders>..</mt5:closed_orders>
         //--- print the color of string
         if(CMTStr::CompareNoCase(tag,L"color")==0)
           {
            if(counter%2==0)
               retcode=m_api->HtmlWriteString(L"#FFFFFF");
            else
               retcode=m_api->HtmlWriteString(L"#E0E0E0");
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            SMTFormat::FormatDateTime(str,order->TimeSetup(),true,true);
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",order->Order());
            continue;
           }
         //--- print the Type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            SMTFormat::FormatOrderType(str,order->Type());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Size
         if(CMTStr::CompareNoCase(tag,L"size")==0)
           {
            SMTFormat::FormatVolumeExtOrder(str,order->VolumeInitialExt(),order->VolumeCurrentExt());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"item")==0)
           {
            retcode=m_api->HtmlWriteSafe(order->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"price")==0)
           {
            SMTFormat::FormatOrderPrice(str,order->PriceOrder(),order->PriceTrigger(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the S / L
         if(CMTStr::CompareNoCase(tag,L"sl")==0)
           {
            SMTFormat::FormatPrice(str,order->PriceSL(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the T / P
         if(CMTStr::CompareNoCase(tag,L"tp")==0)
           {
            SMTFormat::FormatPrice(str,order->PriceTP(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Time
         if(CMTStr::CompareNoCase(tag,L"close_time")==0)
           {
            SMTFormat::FormatDateTime(str,order->TimeDone(),true,true);
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the State
         if(CMTStr::CompareNoCase(tag,L"state")==0)
           {
            SMTFormat::FormatOrderStatus(str,order->State());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Comment
         if(CMTStr::CompareNoCase(tag,L"comment")==0)
           {
            retcode=m_api->HtmlWriteSafe(order->Comment(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write closed deals                                               |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteClosedDeals(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64 str;
//--- are there no deals?
   bool     notrans=m_deals->Total()>0?false:true;
//--- block that is processed, when there are no deals
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_trans_deals")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_deals>..</mt5:no_trans_deals> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:closed_deals> tag (beginning tag of deals table) and
//--- do we have deals?
   if(CMTStr::CompareNoCase(tag,L"closed_deals")==0 && !notrans && counter==0)
     {
      IMTDeal *deal;
      //--- get first deal
      if((deal=m_deals->Next(counter))==NULL)
        {
         retcode=MT_RET_ERR_PARAMS;
         return(true);
        }
      //--- process tags inside <mt5:closed_deals>..</mt5:closed_deals> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- return to beginning of table - <mt5:closed_deals>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"closed_deals")==0)
           {
            //--- have any records left?
            if(counter<m_deals->Total())
              {
               //--- process tags inside <mt5:closed_deals>..</mt5:closed_deals> for the next record
               if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  return(true);
              }
            else
              {
               //--- no more records
               //--- don't repeat <mt5:closed_deals>..</mt5:closed_deals> block anymore (don't call HtmlTplProcess)
               retcode=MT_RET_OK;
               return(true);
              }
            //--- get next deal
            if((deal=m_deals->Next(counter))==NULL)
              {
               retcode=MT_RET_ERR_PARAMS;
               return(true);
              }
            continue;
           }
         //--- process tags inside <mt5:closed_deals>..</mt5:closed_deals>
         //--- print the color of string
         if(CMTStr::CompareNoCase(tag,L"color")==0)
           {
            if(counter%2==0)
               retcode=m_api->HtmlWriteString(L"#FFFFFF");
            else
               retcode=m_api->HtmlWriteString(L"#E0E0E0");
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            CMTStr32 timesetup;
            SMTFormat::FormatDateTime(timesetup,deal->Time(),true,true);
            retcode=m_api->HtmlWriteString(timesetup.Str());
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",deal->Deal());
            continue;
           }
         //--- print the Type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            SMTFormat::FormatDealAction(str,deal->Action());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Size
         if(CMTStr::CompareNoCase(tag,L"size")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               SMTFormat::FormatVolume(str,deal->Volume());
            else
               str.Assign(L"&nbsp;");
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"item")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               retcode=m_api->HtmlWriteSafe(deal->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            else
               retcode=m_api->HtmlWriteString(L"&nbsp;");
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"price")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               SMTFormat::FormatPrice(str,deal->Price(),deal->Digits());
            else
               str.Assign(L"&nbsp;");
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Order
         if(CMTStr::CompareNoCase(tag,L"order")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               retcode=m_api->HtmlWrite(L"%I64u",deal->Order());
            else
               retcode=m_api->HtmlWriteString(L"&nbsp;");
            continue;
           }
         //--- print the Comment
         if(CMTStr::CompareNoCase(tag,L"comment")==0)
           {
            retcode=m_api->HtmlWriteSafe(deal->Comment(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Entry
         if(CMTStr::CompareNoCase(tag,L"entry")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               SMTFormat::FormatDealEntry(str,deal->Entry());
            else
               str.Assign(L"&nbsp;");
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Commission
         if(CMTStr::CompareNoCase(tag,L"commission")==0)
           {
            SMTFormat::FormatMoney(str,deal->Commission(),deal->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Fee
         if(CMTStr::CompareNoCase(tag,L"fee")==0)
           {
            SMTFormat::FormatMoney(str,deal->Fee(),deal->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Swap
         if(CMTStr::CompareNoCase(tag,L"swap")==0)
           {
            SMTFormat::FormatMoney(str,deal->Storage(),deal->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Profit
         if(CMTStr::CompareNoCase(tag,L"profit")==0)
           {
            SMTFormat::FormatMoney(str,deal->Profit(),deal->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write daily position                                             |
//+------------------------------------------------------------------+
bool CDailyDetailed::WriteOpenPosition(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64 str;
//--- are there no positions?
   bool notrans=(m_daily->PositionTotal()==0);
//--- block that is processed, when there are no positions
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_trans_position")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_position>..</mt5:no_trans_position> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:positions> tag (beginning tag of positions table) and
//--- do we have positions?
   if(CMTStr::CompareNoCase(tag,L"positions")==0 && !notrans && counter==0)
     {
      m_position->Clear();
      //--- get first position
      if((retcode=m_daily->PositionNext(counter,m_position))!=MT_RET_OK)
         return(true);
      //--- process tags inside <mt5:positions>..</mt5:positions> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- return to beginning of table - <mt5:positions>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"positions")==0)
           {
            //--- have any records left?
            if(counter<m_daily->PositionTotal())
              {
               //--- process tags inside <mt5:positions>..</mt5:positions> for the next record
               if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  return(true);
              }
            else
              {
               //--- no more records
               //--- don't repeat <mt5:positions>..</mt5:positions> block anymore (don't call HtmlTplProcess)
               retcode=MT_RET_OK;
               return(true);
              }
            //--- get next position
            m_position->Clear();
            if((retcode=m_daily->PositionNext(counter,m_position))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- process tags inside <mt5:positions>..</mt5:positions>
         //--- print the color of string
         if(CMTStr::CompareNoCase(tag,L"color")==0)
           {
            if(counter%2==0)
               retcode=m_api->HtmlWriteString(L"#FFFFFF");
            else
               retcode=m_api->HtmlWriteString(L"#E0E0E0");
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            SMTFormat::FormatDateTime(str,m_position->TimeCreate(),true,true);
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",m_position->Position());
            continue;
           }
         //--- print the Type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            SMTFormat::FormatDealAction(str,m_position->Action());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Size
         if(CMTStr::CompareNoCase(tag,L"size")==0)
           {
            SMTFormat::FormatVolume(str,m_position->Volume());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"item")==0)
           {
            retcode=m_api->HtmlWriteSafe(m_position->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"price")==0)
           {
            SMTFormat::FormatPrice(str,m_position->PriceOpen(),m_position->Digits(),EXTRA_DIGITS);
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the S / L
         if(CMTStr::CompareNoCase(tag,L"sl")==0)
           {
            SMTFormat::FormatPrice(str,m_position->PriceSL(),m_position->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the T / P
         if(CMTStr::CompareNoCase(tag,L"tp")==0)
           {
            SMTFormat::FormatPrice(str,m_position->PriceTP(),m_position->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Market Price
         if(CMTStr::CompareNoCase(tag,L"market")==0)
           {
            SMTFormat::FormatPrice(str,m_position->PriceCurrent(),m_position->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Swap
         if(CMTStr::CompareNoCase(tag,L"swap")==0)
           {
            SMTFormat::FormatMoney(str,m_position->Storage(),m_position->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Profit
         if(CMTStr::CompareNoCase(tag,L"profit")==0)
           {
            SMTFormat::FormatMoney(str,m_position->Profit(),m_position->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
