//+------------------------------------------------------------------+
//|                             MetaTrader 5 Trades.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "TradeModification.h"
#include "..\Tools\LoggerCache.h"
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CTradeModification::s_info=
{
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_ANY,
   L"Trade Modifications",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_ORDERS|MTReportInfo::SNAPSHOT_POSITIONS,
   MTReportInfo::TYPE_HTML,
   L"Trades",
                    // params
     {{ MTReportParam::TYPE_GROUPS, MTAPI_PARAM_GROUPS, L"*" },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM         },
      { MTReportParam::TYPE_DATE,   MTAPI_PARAM_TO           },
     },3            // params_total
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CTradeModification::CTradeModification(void) : m_api(NULL),m_report(NULL),m_param(NULL),
                                               m_positions_modified(NULL),m_positions_user(NULL),
                                               m_orders_modified(NULL),m_orders_user(NULL),
                                               m_deals_modified(NULL),m_deals_user(NULL),
                                               m_history_modified(NULL),m_history_user(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CTradeModification::~CTradeModification(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CTradeModification::Info(MTReportInfo& info)
  {
//--- get info information  
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release plug-in                                                  |
//+------------------------------------------------------------------+
void CTradeModification::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Initialize                                                       |
//+------------------------------------------------------------------+
bool CTradeModification::Initialize(void)
  {
//--- check
   if(!m_api)
      return(false);
//--- create report interface
   if(m_report==NULL && (m_report=m_api->ReportCreate())==NULL)
      return(false);
//--- get report interface
   m_report->Clear();
   if(m_api->ReportCurrent(m_report)!=MT_RET_OK)
      return(false);
//--- create param
   if(m_param==NULL && (m_param=m_api->ParamCreate())==NULL)
      return(false);
//--- create position array interface
   if(m_positions_modified==NULL && (m_positions_modified=m_api->PositionCreateArray())==NULL)
      return(false);
//--- create position array interface
   if(m_positions_user==NULL && (m_positions_user=m_api->PositionCreateArray())==NULL)
      return(false);
//--- create order array interface
   if(m_orders_modified==NULL && (m_orders_modified=m_api->OrderCreateArray())==NULL)
      return(false);
//--- create order array interface
   if(m_orders_user==NULL && (m_orders_user=m_api->OrderCreateArray())==NULL)
      return(false);
//--- create history orders array interface
   if(m_history_modified==NULL && (m_history_modified=m_api->OrderCreateArray())==NULL)
      return(false);
//--- create history orders array interface
   if(m_history_user==NULL && (m_history_user=m_api->OrderCreateArray())==NULL)
      return(false);
//--- create deal array interface
   if(m_deals_modified==NULL && (m_deals_modified=m_api->DealCreateArray())==NULL)
      return(false);
//--- create deal array interface
   if(m_deals_user==NULL && (m_deals_user=m_api->DealCreateArray())==NULL)
      return(false);
   return(true);
  }
//+------------------------------------------------------------------+
//| Release interface                                                |
//+------------------------------------------------------------------+
void CTradeModification::Clear(void)
  {
//--- check report
   if(m_report)
     {
      m_report->Release();
      m_report=NULL;
     }
//--- check param
   if(m_param)
     {
      m_param->Release();
      m_param=NULL;
     }
//---  position array interface
   if(m_positions_modified)
     {
      m_positions_modified->Release();
      m_positions_modified=NULL;
     }
//--- 
   if(m_positions_user)
     {
      m_positions_user->Release();
      m_positions_user=NULL;
     }
//--- order array interface
   if(m_orders_modified)
     {
      m_orders_modified->Release();
      m_orders_modified=NULL;
     }
//--- 
   if(m_orders_user)
     {
      m_orders_user->Release();
      m_orders_user=NULL;
     }
//--- history array interface
   if(m_history_modified)
     {
      m_history_modified->Release();
      m_history_modified=NULL;
     }
//---
   if(m_history_user)
     {
      m_history_user->Release();
      m_history_user=NULL;
     }
//--- deal array interface
   if(m_deals_modified)
     {
      m_deals_modified->Release();
      m_deals_modified=NULL;
     }
//---
   if(m_deals_user)
     {
      m_deals_user->Release();
      m_deals_user=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CTradeModification::Generate(const uint32_t type,IMTReportAPI *api)
  {
   uint64_t  *logins=NULL;
   uint32_t logins_total=0;
   MTAPIRES retcode;
//--- clear
   Clear();
//--- checks
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- checks
   if(type!=MTReportInfo::TYPE_HTML)
      return(MT_RET_ERR_NOTIMPLEMENT);
//--- save api pointer
   m_api=api;
//--- get report logins
   if((retcode=m_api->ParamLogins(logins,logins_total))!=MT_RET_OK)
     {
      //--- clear
      Clear();
      //--- return result
      return(retcode);
     }
//--- checks logins
   if(logins && logins_total)
     {
      //--- prepare deals array & user & group
      if(!Initialize())
        {
         api->Free(logins);
         Clear();
         return(MT_RET_ERR_MEM);
        }
      //--- request data by logins
      for(uint32_t i=0;i<logins_total;i++)
        {
         if((retcode=CollectModifiedPositions(logins[i]))!=MT_RET_OK ||
            (retcode=CollectModifiedOrders(logins[i]))   !=MT_RET_OK ||
            (retcode=CollectModifiedDeals(logins[i]))    !=MT_RET_OK ||
            (retcode=CollectModifiedHistory(logins[i]))  !=MT_RET_OK )
            break;
        }
      //--- write report
      if(retcode==MT_RET_OK)
         retcode=WriteHtml();
     }
//--- free logins
   if(logins)
      api->Free(logins);
//--- clear
   Clear();
//--- ok
   return(retcode);
  }
//+------------------------------------------------------------------+
//| Collect modified positions by login                              |
//+------------------------------------------------------------------+
MTAPIRES CTradeModification::CollectModifiedPositions(const uint64_t login)
  {
   IMTPosition *record;
   uint32_t     records_total,i;
   MTAPIRES     retcode;
//--- checks
   if(!login || !m_api || !m_positions_user || !m_positions_modified)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   m_positions_user->Clear();
   if((retcode=m_api->PositionGet(login,m_positions_user))!=MT_RET_OK)
      return(retcode);
//--- nothing to do?
   if(!(records_total=m_positions_user->Total()))
      return(MT_RET_OK);
//--- look through modified, move modified positions to m_positions_modified
   for(i=0;i<records_total;i++)
      if((record=m_positions_user->Next(i))!=NULL)
         if(record->ModificationFlags())
           {
            //--- move modified positions to m_positions_modified, detach copy object and delete it from array
            record=m_positions_user->Detach(i);
            m_positions_modified->Add(record);
            i--;
           }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Collect modified orders by login                                 |
//+------------------------------------------------------------------+
MTAPIRES CTradeModification::CollectModifiedOrders(const uint64_t login)
  {
   IMTOrder *record;
   uint32_t  records_total,i;
   MTAPIRES  retcode;
//--- checks
   if(!login || !m_api || !m_orders_user || !m_orders_modified)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   m_orders_user->Clear();
   if((retcode=m_api->OrderGet(login,m_orders_user))!=MT_RET_OK)
      return(retcode);
//--- nothing to do?
   if(!(records_total=m_orders_user->Total()))
      return(MT_RET_OK);
//--- look through modified, move modified positions to m_positions_modified
   for(i=0;i<records_total;i++)
      if((record=m_orders_user->Next(i))!=NULL)
         if(record->ModificationFlags())
           {
            //--- move modified positions to m_positions_modified, detach copy object and delete it from array
            record=m_orders_user->Detach(i);
            m_orders_modified->Add(record);
            i--;
           }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Collect modified deals by login                                  |
//+------------------------------------------------------------------+
MTAPIRES CTradeModification::CollectModifiedDeals(const uint64_t login)
  {
   IMTDeal  *record;
   uint32_t  records_total,i;
   MTAPIRES  retcode;
//--- checks
   if(!login || !m_api || !m_deals_user || !m_deals_modified)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   m_deals_user->Clear();
   if((retcode=m_api->DealGet(login,m_api->ParamFrom(),m_api->ParamTo(),m_deals_user))!=MT_RET_OK)
      return(retcode);
//--- nothing to do?
   if(!(records_total=m_deals_user->Total()))
      return(MT_RET_OK);
//--- look through modified, move modified positions to m_positions_modified
   for(i=0;i<records_total;i++)
      if((record=m_deals_user->Next(i))!=NULL)
         if(record->ModificationFlags())
           {
            //--- move modified positions to m_positions_modified, detach copy object and delete it from array
            record=m_deals_user->Detach(i);
            m_deals_modified->Add(record);
            i--;
           }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Collect modified history by login                                |
//+------------------------------------------------------------------+
MTAPIRES CTradeModification::CollectModifiedHistory(const uint64_t login)
  {
   IMTOrder *record;
   uint32_t  records_total,i;
   MTAPIRES  retcode;
//--- checks
   if(!login || !m_api || !m_history_user || !m_history_modified)
      return(MT_RET_ERR_PARAMS);
//--- get deals
   m_history_user->Clear();
   if((retcode=m_api->HistoryGet(login,m_api->ParamFrom(),m_api->ParamTo(),m_history_user))!=MT_RET_OK)
      return(retcode);
//--- nothing to do?
   if(!(records_total=m_history_user->Total()))
      return(MT_RET_OK);
//--- look through modified, move modified positions to m_positions_modified
   for(i=0;i<records_total;i++)
      if((record=m_history_user->Next(i))!=NULL)
         if(record->ModificationFlags())
           {
            //--- move modified positions to m_positions_modified, detach copy object and delete it from array
            record=m_history_user->Detach(i);
            m_history_modified->Add(record);
            i--;
           }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write html                                                       |
//+------------------------------------------------------------------+
MTAPIRES CTradeModification::WriteHtml(void)
  {
   MTAPISTR tag;
   uint32_t counter=0;
   MTAPIRES res;
//--- load template from resource
   if((res=m_api->HtmlTplLoadResource(IDR_TRADE_MODIFICATIONS,RT_HTML))!=MT_RET_OK)
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
      //--- write table positions
      if(WritePositions(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write table open orders
      if(WriteOrders(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write table closed orders
      if(WriteHistoryOrders(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write table deals
      if(WriteDeals(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
     }
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write head html                                                  |
//+------------------------------------------------------------------+
bool CTradeModification::WriteHead(MTAPISTR& tag,MTAPIRES& retcode)
  {
   CMTStr64 tmp;
//--- print the report name
   if(CMTStr::CompareNoCase(tag,L"report_name")==0)
     {
      retcode=m_api->HtmlWriteSafe(s_info.name,IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- company name
   if(CMTStr::CompareNoCase(tag,L"company")==0)
     {
      IMTConCommon *common=m_api->CommonCreate();
      //--- check
      if(common)
        {
         //--- get common and write owner name
         if(m_api->CommonGet(common)==MT_RET_OK)
            retcode=m_api->HtmlWriteSafe(common->Owner(),IMTReportAPI::HTML_SAFE_USENOBSP);
         common->Release();
        }
      return(true);
     }
//--- server name
   if(CMTStr::CompareNoCase(tag,L"server")==0)
     {
      MTReportServerInfo info={};
      //--- get info and write server name
      if(m_api->About(info)==MT_RET_OK)
         retcode=m_api->HtmlWriteSafe(info.platform_name,IMTReportAPI::HTML_SAFE_USENOBSP);
      return(true);
     }
//--- from
   if(CMTStr::CompareNoCase(tag,L"from")==0)
     {
      retcode=m_api->HtmlWriteString(SMTFormat::FormatDateTime(tmp,m_api->ParamFrom(),false,false));
      return(true);
     }
//--- to
   if(CMTStr::CompareNoCase(tag,L"to")==0)
     {
      retcode=m_api->HtmlWriteString(SMTFormat::FormatDateTime(tmp,m_api->ParamTo(),false,false));
      return(true);
     }
//--- groups
   if(CMTStr::CompareNoCase(tag,L"groups")==0)
     {
      if(m_param && m_api->ParamGet(MTAPI_PARAM_GROUPS,m_param)==MT_RET_OK)
         retcode=m_api->HtmlWriteSafe(m_param->ValueGroups(),IMTReportAPI::HTML_SAFE_NONE);
      return(true);
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write open orders                                                |
//+------------------------------------------------------------------+
bool CTradeModification::WriteOrders(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64     str;
   CMTStr1024   request;
//--- are there no orders?
   bool notrans=(m_orders_modified->Total()==0);
//--- block that is processed, when there are no orders
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_orders")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_order>..</mt5:no_trans_order> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:orders> tag (beginning tag of orders table) and
//--- do we have orders?
   if(CMTStr::CompareNoCase(tag,L"orders")==0 && !notrans && counter==0)
     {
      const IMTOrder *order;
      CLoggerCacheHolder logger_cache(m_api);
      //--- get first order
      if((order=logger_cache.CacheOrders(*m_orders_modified,counter,retcode))==NULL)
         return(true);
      //--- process tags inside <mt5:orders>..</mt5:orders> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while(retcode==MT_RET_OK)
        {
         //--- iterate next tag
         retcode=m_api->HtmlTplNext(tag,&counter);
         if(retcode!=MT_RET_OK)
            break;
         //--- return to beginning of table - <mt5:orders>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"orders")==0)
           {
            //--- have any records left?
            if(counter<m_orders_modified->Total())
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
            if((order=logger_cache.CacheOrders(*m_orders_modified,counter,retcode))==NULL)
               return(true);
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",order->Order());
            continue;
           }
         //--- print the Login
         if(CMTStr::CompareNoCase(tag,L"login")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",order->Login());
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            CMTStr32 timesetup;
            SMTFormat::FormatDateTime(timesetup,order->TimeSetup(),true,true);
            retcode=m_api->HtmlWriteString(timesetup.Str());
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
         if(CMTStr::CompareNoCase(tag,L"lots")==0)
           {
            SMTFormat::FormatVolumeExt(str,order->VolumeInitialExt());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"symbol")==0)
           {
            retcode=m_api->HtmlWriteSafe(order->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"open_price")==0)
           {
            //--- only for trade action
            SMTFormat::FormatPrice(str,order->PriceOrder(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"market_price")==0)
           {
            //--- only for trade action
            SMTFormat::FormatPrice(str,order->PriceCurrent(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print modified flags
         if(CMTStr::CompareNoCase(tag,L"modified")==0)
           {
            SMTFormat::FormatModifyFlags(str,order->ModificationFlags());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print logs
         if(CMTStr::CompareNoCase(tag,L"logs")==0)
           {
            retcode=logger_cache.HtmlWriteLogOrder(*order);
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write closed orders                                              |
//+------------------------------------------------------------------+
bool CTradeModification::WriteHistoryOrders(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64     str;
   CMTStr1024   request;
//--- are there no orders?
   bool notrans=(m_history_modified->Total()==0);
//--- block that is processed, when there are no orders
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_history")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_closed>..</mt5:no_trans_closed> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:history> tag (beginning tag of orders table) and
//--- do we have orders?
   if(CMTStr::CompareNoCase(tag,L"history")==0 && !notrans && counter==0)
     {
      const IMTOrder *order;
      CLoggerCacheHolder logger_cache(m_api);
      //--- get first order
      if((order=logger_cache.CacheOrders(*m_history_modified,counter,retcode))==NULL)
         return(true);
      //--- process tags inside <mt5:history>..</mt5:history> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while(retcode==MT_RET_OK)
        {
         //--- iterate next tag
         retcode=m_api->HtmlTplNext(tag,&counter);
         if(retcode!=MT_RET_OK)
            break;
         //--- return to beginning of table - <mt5:history>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"history")==0)
           {
            //--- have any records left?
            if(counter<m_history_modified->Total())
              {
               //--- process tags inside <mt5:history>..</mt5:history> for the next record
               if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  return(true);
              }
            else
              {
               //--- no more records
               //--- don't repeat <mt5:history>..</mt5:history> block anymore (don't call HtmlTplProcess)
               retcode=MT_RET_OK;
               return(true);
              }
            //--- get next order
            if((order=logger_cache.CacheOrders(*m_history_modified,counter,retcode))==NULL)
               return(true);
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",order->Order());
            continue;
           }
         //--- print the Login
         if(CMTStr::CompareNoCase(tag,L"login")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",order->Login());
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            CMTStr32 timesetup;
            SMTFormat::FormatDateTime(timesetup,order->TimeSetup(),true,true);
            retcode=m_api->HtmlWriteString(timesetup.Str());
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
         if(CMTStr::CompareNoCase(tag,L"lots")==0)
           {
            SMTFormat::FormatVolumeExt(str,order->VolumeInitialExt());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"symbol")==0)
           {
            retcode=m_api->HtmlWriteSafe(order->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"open_price")==0)
           {
            //--- only for trade action
            SMTFormat::FormatPrice(str,order->PriceOrder(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"market_price")==0)
           {
            //--- only for trade action
            SMTFormat::FormatPrice(str,order->PriceCurrent(),order->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print modified flags
         if(CMTStr::CompareNoCase(tag,L"modified")==0)
           {
            SMTFormat::FormatModifyFlags(str,order->ModificationFlags());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print logs
         if(CMTStr::CompareNoCase(tag,L"logs")==0)
           {
            retcode=logger_cache.HtmlWriteLogOrder(*order);
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write deals                                                      |
//+------------------------------------------------------------------+
bool CTradeModification::WriteDeals(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64     str;
   CMTStr1024   request;
//--- are there no deals?
   bool     notrans=m_deals_modified->Total()>0?false:true;
//--- block that is processed, when there are no deals
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_deals")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_deals>..</mt5:no_trans_deals> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:deals> tag (beginning tag of deals table) and
//--- do we have deals?
   if(CMTStr::CompareNoCase(tag,L"deals")==0 && !notrans && counter==0)
     {
      const IMTDeal *deal;
      CLoggerCacheHolder logger_cache(m_api);
      //--- get first deal
      if((deal=logger_cache.CacheDeals(*m_deals_modified,counter,retcode))==NULL)
         return(true);
      //--- process tags inside <mt5:deals>..</mt5:deals> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while(retcode==MT_RET_OK)
        {
         //--- iterate next tag
         retcode=m_api->HtmlTplNext(tag,&counter);
         if(retcode!=MT_RET_OK)
            break;
         //--- return to beginning of table - <mt5:deals>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"deals")==0)
           {
            //--- have any records left?
            if(counter<m_deals_modified->Total())
              {
               //--- process tags inside <mt5:deals>..</mt5:deals> for the next record
               if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
                  return(true);
              }
            else
              {
               //--- no more records
               //--- don't repeat <mt5:deals>..</mt5:deals> block anymore (don't call HtmlTplProcess)
               retcode=MT_RET_OK;
               return(true);
              }
            //--- get next deal
            if((deal=logger_cache.CacheDeals(*m_deals_modified,counter,retcode))==NULL)
               return(true);
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",deal->Deal());
            continue;
           }
         //--- print the Login
         if(CMTStr::CompareNoCase(tag,L"login")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",deal->Login());
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
         //--- print the Type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            SMTFormat::FormatDealAction(str,deal->Action());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Size
         if(CMTStr::CompareNoCase(tag,L"lots")==0)
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
         if(CMTStr::CompareNoCase(tag,L"symbol")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               retcode=m_api->HtmlWriteSafe(deal->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            else
               retcode=m_api->HtmlWriteString(L"&nbsp;");
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"open_price")==0)
           {
            //--- only for trade action
            if(deal->Action()==IMTDeal::DEAL_BUY || deal->Action()==IMTDeal::DEAL_SELL)
               SMTFormat::FormatPrice(str,deal->Price(),deal->Digits());
            else
               str.Assign(L"&nbsp;");
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
         //--- print modified flags
         if(CMTStr::CompareNoCase(tag,L"modified")==0)
           {
            SMTFormat::FormatModifyFlags(str,deal->ModificationFlags());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print logs
         if(CMTStr::CompareNoCase(tag,L"logs")==0)
           {
            retcode=logger_cache.HtmlWriteLogDeal(*deal);
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Write daily position                                             |
//+------------------------------------------------------------------+
bool CTradeModification::WritePositions(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr64     str;
   CMTStr1024   request;
//--- are there no positions?
   bool notrans=(m_positions_modified->Total()==0);
//--- block that is processed, when there are no positions
//--- this block must be processed only once, therefore check that counter==0
   if(CMTStr::CompareNoCase(tag,L"no_positions")==0 && notrans && counter==0)
     {
      //--- print everything inside the <mt5:no_trans_position>..</mt5:no_trans_position> (No transactions)
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- is it open the <mt5:positions> tag (beginning tag of positions table) and
//--- do we have positions?
   if(CMTStr::CompareNoCase(tag,L"positions")==0 && !notrans)
     {
      const IMTPosition *position;
      CLoggerCacheHolder logger_cache(m_api);
      //--- get first position
      if((position=logger_cache.CachePositions(*m_positions_modified,counter,retcode))==NULL)
         return(true);
      //--- process tags inside <mt5:positions>..</mt5:positions> for the first record
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      while(retcode==MT_RET_OK)
        {
         //--- iterate next tag
         retcode=m_api->HtmlTplNext(tag,&counter);
         if(retcode!=MT_RET_OK)
            break;
         //--- return to beginning of table - <mt5:positions>
         //--- the 'counter' contains index of next record
         if(CMTStr::CompareNoCase(tag,L"positions")==0)
           {
            //--- have any records left?
            if(counter<m_positions_modified->Total())
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
            if((position=logger_cache.CachePositions(*m_positions_modified,counter,retcode))==NULL)
               return(true);
            continue;
           }
         //--- print the Ticket
         if(CMTStr::CompareNoCase(tag,L"ticket")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",position->Position());
            continue;
           }
         //--- print the Login
         if(CMTStr::CompareNoCase(tag,L"login")==0)
           {
            retcode=m_api->HtmlWrite(L"%I64u",position->Login());
            continue;
           }
         //--- print the Open Time
         if(CMTStr::CompareNoCase(tag,L"open_time")==0)
           {
            CMTStr32 timesetup;
            SMTFormat::FormatDateTime(timesetup,position->TimeCreate(),true,true);
            retcode=m_api->HtmlWriteString(timesetup.Str());
            continue;
           }
         //--- print the Type
         if(CMTStr::CompareNoCase(tag,L"type")==0)
           {
            SMTFormat::FormatPositionType(str,position->Action());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Size
         if(CMTStr::CompareNoCase(tag,L"lots")==0)
           {
            SMTFormat::FormatVolume(str,position->Volume());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Item
         if(CMTStr::CompareNoCase(tag,L"symbol")==0)
           {
            retcode=m_api->HtmlWriteSafe(position->Symbol(),IMTReportAPI::HTML_SAFE_USENOBSP);
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"open_price")==0)
           {
            //--- only for trade action
            SMTFormat::FormatPrice(str,position->PriceOpen(),position->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Price
         if(CMTStr::CompareNoCase(tag,L"market_price")==0)
           {
            //--- only for trade action
            SMTFormat::FormatPrice(str,position->PriceCurrent(),position->Digits());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Swap
         if(CMTStr::CompareNoCase(tag,L"swap")==0)
           {
            SMTFormat::FormatMoney(str,position->Storage(),position->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print the Profit
         if(CMTStr::CompareNoCase(tag,L"profit")==0)
           {
            SMTFormat::FormatMoney(str,position->Profit(),position->DigitsCurrency());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print modified flags
         if(CMTStr::CompareNoCase(tag,L"modified")==0)
           {
            SMTFormat::FormatModifyFlags(str,position->ModificationFlags());
            retcode=m_api->HtmlWriteString(str.Str());
            continue;
           }
         //--- print logs
         if(CMTStr::CompareNoCase(tag,L"logs")==0)
           {
            retcode=logger_cache.HtmlWriteLogPosition(*position);
            continue;
           }
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
