//+------------------------------------------------------------------+
//|                          MetaTrader 5 Trades.Transaction.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
#include "TradeTransactionReport.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL),m_daily_time(-1),m_config(NULL),m_param(NULL),
                                         m_symbol(NULL),
                                         m_group(NULL),m_user(NULL),
                                         m_daily_workflag(false),m_daily_last(0),
                                         m_dealer_orders_last(0)

  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CPluginInstance::~CPluginInstance(void)
  {
   Stop();
  }
//+------------------------------------------------------------------+
//| Plugin release function                                          |
//+------------------------------------------------------------------+
void CPluginInstance::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Plugin start notification function                               |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Start(IMTServerAPI* api)
  {
   MTAPIRES retcode=MT_RET_OK;
//---
   Stop();
//--- check parameters
   if(!api)
      return(MT_RET_ERR_PARAMS);
   m_api=api;
//--- create plugin config
   if((m_config=m_api->PluginCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- create plugin param
   if((m_param=m_api->PluginParamCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- read parameters
   if((retcode=ReadParams())!=MT_RET_OK)
      return(retcode);
//--- create symbol
   if(!(m_symbol=m_api->SymbolCreate()))
      return(MT_RET_ERR_MEM);
//--- create user interface
   if(!(m_user=m_api->UserCreate()))
      return(MT_RET_ERR_MEM);
//--- create group interface
   if(!(m_group=m_api->GroupCreate()))
      return(MT_RET_ERR_MEM);
//--- subscribe to plugin config updates
   if((retcode=m_api->PluginSubscribe(this))!=MT_RET_OK)
      return(retcode);
//--- subscribe to trade requests
   if((retcode=m_api->TradeSubscribe(this))!=MT_RET_OK)
      return(retcode);
//--- subscribe to deals request
   if((retcode=m_api->DealSubscribe(this))!=MT_RET_OK)
      return(retcode);
//--- subscribe to end of day
   if((retcode=m_api->TradeSubscribeEOD(this))!=MT_RET_OK)
      return(retcode);
//--- start daily thread if needed
   DailyStart();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
//--- stop daily thread
   DailyStop();
//--- by lock
   m_sync.Lock();
//--- unsubscribe 
   if(m_api)
     {
      m_api->PluginUnsubscribe(this);
      m_api->TradeUnsubscribe(this);
      m_api->DealUnsubscribe(this);
      m_api->TradeUnsubscribeEOD(this);
     }
//--- clear all parameters
   m_group_mask.Clear();
   m_symbol_mask.Clear();
   m_dealer_orders.Clear();
//--- delete interfaces
   if(m_config)    { m_config->Release();    m_config   =NULL; }
   if(m_param)     { m_param->Release();     m_param    =NULL; }
   if(m_symbol)    { m_symbol->Release();    m_symbol   =NULL; }
   if(m_group)     { m_group->Release();     m_group    =NULL; }
   if(m_user)      { m_user->Release();      m_user     =NULL; }
//--- reset api
   m_api=NULL;
//---
   m_sync.Unlock();
//--- clear dealer orders
   m_dealer_orders_sync.Lock();
   m_dealer_orders.Shutdown();
   m_dealer_orders_sync.Unlock();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Check group mask                                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::CheckGroup(LPCWSTR group)
  {
//--- lock
   m_sync.Lock();
   bool res=false;
   if(group)
      res=CMTStr::CheckGroupMask(m_group_mask.Str(),group);
   m_sync.Unlock();
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Check symbol mask                                                |
//+------------------------------------------------------------------+
bool CPluginInstance::CheckSymbol(LPCWSTR symbol)
  {
//--- lock
   m_sync.Lock();
   bool res=false;
   if(symbol)
      res=CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol);
   m_sync.Unlock();
//--- result
   return(res);
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ReadParams(void)
  {
   MTAPIRES   retcode=MT_RET_OK;
   int32_t        pos    =0;
//--- check pointers
   if(!m_api || !m_config || !m_param)
      return(MT_RET_ERR_PARAMS);
//--- get current plugin configuration
   if((retcode=m_api->PluginCurrent(m_config))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"current plugin configuration get error [%s (%u)]",SMTFormat::FormatError(retcode),retcode);
      return(retcode);
     }
//--- lock parameters
   m_sync.Lock();
//--- reset all parameters
   m_group_mask.Clear();
   m_symbol_mask.Clear();
   m_daily_path.Clear();
   m_daily_time=-1;
//--- get group mask parameter
   if((retcode=m_config->ParameterGet(L"Groups",m_param))!=MT_RET_OK)
     {
      //--- not found?
      if(retcode==MT_RET_ERR_NOTFOUND)
        {
         m_param->Clear();
         m_param->Name(L"Groups");
         m_param->ValueGroups(DEFAULT_GROUPS);
         m_config->ParameterAdd(m_param);
         m_api->PluginAdd(m_config);
        }
      else
        {
         m_api->LoggerOut(MTLogErr,L"group mask get error [%u]",retcode);
         m_sync.Unlock();
         return(MT_RET_ERR_PARAMS);
        }
     }
//--- check param type
   if(m_param->Type()!=IMTConParam::TYPE_GROUPS)
     {
      m_param->Type(IMTConParam::TYPE_GROUPS);
      ParamUpdate(m_config,m_param);
     }
   m_group_mask.Assign(m_param->ValueGroups());
//--- get symbol mask
   if((retcode=m_config->ParameterGet(L"Symbols",m_param))!=MT_RET_OK)
     {
      //--- not found?
      if(retcode==MT_RET_ERR_NOTFOUND)
        {
         m_param->Clear();
         m_param->Name(L"Symbols");
         m_param->ValueSymbols(DEFAULT_SYMBOLS);
         m_config->ParameterAdd(m_param);
         m_api->PluginAdd(m_config);
        }
      else
        {
         m_api->LoggerOut(MTLogErr,L"symbol mask get error [%u]",retcode);
         m_sync.Unlock();
         return(MT_RET_ERR_PARAMS);
        }
     }
   m_symbol_mask.Assign(m_param->ValueSymbols());
//--- get database directory
   if((retcode=m_config->ParameterGet(L"Base Directory",m_param))!=MT_RET_OK)
     {
      //--- not found?
      if(retcode==MT_RET_ERR_NOTFOUND)
        {
         m_param->Clear();
         m_param->Name(L"Base Directory");
         m_param->ValueString(DEFAULT_BASE_DIRECTORY);
         m_config->ParameterAdd(m_param);
         m_api->PluginAdd(m_config);
        }
      else
        {
         m_api->LoggerOut(MTLogErr,L"base directory get error [%u]",retcode);
         m_sync.Unlock();
         return(MT_RET_ERR_PARAMS);
        }
     }
//--- prepapre base path
   CMTStrPath path;
   GetModuleFileNameW(NULL,path.Buffer(),path.Max());
   path.Refresh();
   if((pos=path.FindRChar(L'\\'))>0)
      path.Trim(pos);
   m_base_name.Assign(m_config->Name());
   m_base_path.Format(L"%s\\Reports\\Trades.Transaction.Reports\\%s\\",
      path.Str(),m_param->ValueString() ? m_param->ValueString() : DEFAULT_BASE_DIRECTORY);
//--- check directory
   if(!CMTFile::DirectoryCreate(m_base_path))
     {
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
//--- initialize transaction base
   if(!m_base.Initialize(m_base_path,m_api))
     {
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
//--- get daily report path
   if((retcode=m_config->ParameterGet(L"Daily Report Path",m_param))!=MT_RET_OK)
     {
      //--- not found?
      if(retcode==MT_RET_ERR_NOTFOUND)
        {
         m_param->Clear();
         m_param->Name(L"Daily Report Path");
         m_param->ValueString(DEFAULT_DAILY_PATH);
         m_config->ParameterAdd(m_param);
         m_api->PluginAdd(m_config);
        }
      else
        {
         m_api->LoggerOut(MTLogErr,L"daily report path get error [%u]",retcode);
         m_sync.Unlock();
         return(MT_RET_ERR_PARAMS);
        }
     }
   m_daily_path.Assign(m_param->ValueString());
//--- get daily report time
   if((retcode=m_config->ParameterGet(L"Daily Report Time",m_param))!=MT_RET_OK)
     {
      //--- not found?
      if(retcode!=MT_RET_ERR_NOTFOUND)
        {
         m_api->LoggerOut(MTLogErr,L"daily report time get error [%u]",retcode);
         m_sync.Unlock();
         return(MT_RET_ERR_PARAMS);
        }
     }
   else
     {
      //--- check parameter type
      if(m_param->Type()!=IMTConParam::TYPE_TIME)
        {
         m_param->Type(IMTConParam::TYPE_TIME);
         ParamUpdate(m_config,m_param);
        }
      m_daily_time=m_param->ValueTime();
     }
//--- unlock parameters
   m_sync.Unlock();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
void CPluginInstance::OnPluginUpdate(const IMTConPlugin* plugin)
  {
//--- check parameters
   if(plugin==NULL || m_api==NULL || m_config==NULL)
      return;
//--- update config
   if(CMTStr::Compare(plugin->Name(),m_config->Name())==0 && plugin->Server()==m_config->Server())
     {
      //--- stop thread
      DailyStop();
      //--- under lock
      m_sync.Lock();
      //--- read params
      ReadParams();
      //--- reset last day
      m_daily_last=0;
      //--- start daily thread if needed
      DailyStart();
      //--- unlock
      m_sync.Unlock();
     }
  }
//+------------------------------------------------------------------+
//| Write positions at the end of day (called from many threads!!!)  |
//+------------------------------------------------------------------+
void CPluginInstance::OnEODGroupFinish(const int64_t datetime,const int64_t prev_datetime,const IMTConGroup* group)
  {
   IMTAccount       *account     =NULL;
   IMTConSymbol     *symbol      =NULL;
   uint64_t           *logins      =NULL;
   uint32_t          logins_total=0;
   IMTPositionArray *positions   =NULL;
   IMTPosition      *position    =NULL;
   IMTOrderArray    *orders      =NULL;
   IMTOrder         *order       =NULL;
   MTAPIRES          retcode     =MT_RET_ERROR;
   MTTickShort       tick        ={0};
   TransactionRecord record      ={0};
//--- checks
   if(!group)
      return;
//--- check group
   if(!CheckGroup(group->Group()))
     {
      m_api->LoggerOut(MTLogOK,L"'%s' skipped groups param of plugin [eod]",group->Group());
      return;
     }
//--- get all users in the group
   if((retcode=m_api->UserLogins(group->Group(),logins,logins_total))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user logins get error [%u, eod]",retcode);
      return;
     }
//--- create user trade account
   if((account=m_api->UserCreateAccount())==NULL)
     {
      m_api->Free(logins);
      return;
     }
//--- create symbol interface
   if((symbol=m_api->SymbolCreate())==NULL)
     {
      account->Release();
      m_api->Free(logins);
      return;
     }
//--- create position array interface
   if((positions=m_api->PositionCreateArray())==NULL)
     {
      account->Release();
      symbol->Release();
      m_api->Free(logins);
      return;
     }
//--- create orders array interface
   if((orders=m_api->OrderCreateArray())==NULL)
     {
      account->Release();
      symbol->Release();
      positions->Release();
      m_api->Free(logins);
      return;
     }
//--- for all logins get position
   uint32_t positions_written=0,positions_failed=0,positions_total=0;
   for(uint32_t i=0;i<logins_total;i++)
     {
      //--- get user's positions
      if((retcode=m_api->PositionGet(logins[i],positions))!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr,L"position for %I64u get error [%u, eod]",logins[i],retcode);
         continue;
        }
      //--- for all positions
      for(uint32_t j=0;(position=positions->Next(j));j++,positions_total++)
        {
         //--- clean the record
         ZeroMemory(&record,sizeof(record));
         //--- get the symbol
         if((retcode=m_api->SymbolGet(position->Symbol(),symbol))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"symbol %s get error [%u, eod]",position->Symbol(),retcode);
            continue;
           }
         //--- check symbol mask
         if(!CheckSymbol(symbol->Symbol()) && !CheckSymbol(symbol->Path()))
            continue;
         //--- get the last tick
         if((retcode=m_api->TickLast(symbol,tick))!=MT_RET_OK)
           {
            //--- ok, set all fields to zero
            ZeroMemory(&tick,sizeof(tick));
           }
         //---- get account interface
         if((retcode=m_api->UserAccountGet(logins[i],account))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"user account for %I64u get error [%u, eod]",logins[i],retcode);
            continue;
           }
         //--- fill the record
         record.position       =position->Position();
         record.login          =logins[i];
         record.timestamp      =datetime;
         record.lots           =position->VolumeExt();
         record.sl             =position->PriceSL();
         record.tp             =position->PriceTP();
         record.bid            =tick.bid;
         record.ask            =tick.ask;
         record.leverage       =account->MarginLeverage();
         record.price          =position->PriceCurrent();
         record.margin_rate    =position->RateMargin();
         record.swap           =position->Storage();
         record.profit         =position->Profit();
         record.profit_rate    =position->RateProfit();
         record.price_position =position->PriceOpen();
         record.amount         =CalcAmountPosition(symbol,position);
         record.digits         =position->Digits();
         record.digits_currency=position->DigitsCurrency();
         CMTStr::Copy(record.position_id    ,position->ExternalID());
         CMTStr::Copy(record.currency       ,group->Currency());
         CMTStr::Copy(record.group_owner    ,group->Company());
         CMTStr::Copy(record.symbol         ,symbol->Symbol());
         CMTStr::Copy(record.profit_currency,symbol->CurrencyProfit());
         //--- calc raw profit
         if(record.profit_rate)
            record.profit_raw=SMTMath::PriceNormalize(record.profit/record.profit_rate,symbol->Digits());
         //--- set type of operation
         record.action=TransactionRecord::ACTION_DAILY_POSITION;
         record.entry =TransactionRecord::ENTRY_UNKNOWN;
         //--- set type
         switch(position->Action())
           {
            case IMTPosition::POSITION_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
            case IMTPosition::POSITION_SELL: record.type=TransactionRecord::TYPE_SELL; break;
            default:
               continue;
           }
         //--- write it
         m_sync.Lock();
         const bool res=m_base.Write(record);
         m_sync.Unlock();
         //--- calculate statistics
         if(res)
            positions_written++;
         else
            positions_failed++;
        }
     }
//--- for all users
   uint32_t orders_written=0,orders_failed=0,orders_total=0;
   for(uint32_t i=0;i<logins_total;i++)
     {
      //--- get all orders
      if((retcode=m_api->OrderGet(logins[i],orders))!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogOK,L"order for %I64u get error [%u]",logins[i],retcode);
         continue;
        }
      //--- order by order
      for(uint32_t j=0;(order=orders->Next(j));j++,orders_total++)
        {
         //--- clean the record
         ZeroMemory(&record,sizeof(record));
         //--- get the symbol
         if((retcode=m_api->SymbolGet(order->Symbol(),symbol))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"symbol %s get error [%u, eod]",order->Symbol(),retcode);
            continue;
           }
         //--- check symbol mask
         if(!CheckSymbol(symbol->Symbol()) && !CheckSymbol(symbol->Path()))
            continue;
         //--- get the last tick
         if((retcode=m_api->TickLast(symbol,tick))!=MT_RET_OK)
           {
            //--- ok, set all fields to zero
            ZeroMemory(&tick,sizeof(tick));
           }
         //---- get account interface
         if((retcode=m_api->UserAccountGet(logins[i],account))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"user account for %I64u get error [%u, eod]",logins[i],retcode);
            continue;
           }
         //--- fill the record
         record.login          =logins[i];
         record.timestamp      =datetime;
         record.leverage       =account->MarginLeverage();
         record.order          =order->Order();
         record.sl             =order->PriceSL();
         record.tp             =order->PriceTP();
         record.lots           =order->VolumeCurrentExt();
         record.bid            =tick.bid;
         record.ask            =tick.ask;
         record.amount         =CalcAmountOrder(symbol,order);
         record.digits         =order->Digits();
         record.digits_currency=order->DigitsCurrency();
         CMTStr::Copy(record.currency       ,group->Currency());
         CMTStr::Copy(record.group_owner    ,group->Company());
         CMTStr::Copy(record.symbol         ,symbol->Symbol());
         CMTStr::Copy(record.profit_currency,symbol->CurrencyProfit());
         //--- set type of operation
         record.action=TransactionRecord::ACTION_DAILY_ORDER;
         record.entry =TransactionRecord::ENTRY_UNKNOWN;
         //--- set type
         switch(order->Type())
           {
            case IMTOrder::OP_BUY:
               record.type=TransactionRecord::TYPE_BUY;
               record.price=order->PriceOrder();
               break;
            case IMTOrder::OP_SELL           :
               record.type=TransactionRecord::TYPE_SELL;
               record.price=order->PriceOrder();
               break;
            case IMTOrder::OP_BUY_LIMIT      :
               record.type=TransactionRecord::TYPE_BUY_LIMIT;
               record.price=order->PriceOrder();
               break;
            case IMTOrder::OP_SELL_LIMIT     :
               record.type=TransactionRecord::TYPE_SELL_LIMIT;
               record.price=order->PriceOrder();
               break;
            case IMTOrder::OP_BUY_STOP       :
               record.type=TransactionRecord::TYPE_BUY_STOP;
               record.price=order->PriceOrder();
               break;
            case IMTOrder::OP_SELL_STOP      :
               record.type=TransactionRecord::TYPE_SELL_STOP;
               record.price=order->PriceOrder();
               break;
            case IMTOrder::OP_BUY_STOP_LIMIT :
               record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
               record.price=order->PriceTrigger();
               break;
            case IMTOrder::OP_SELL_STOP_LIMIT:
               record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
               record.price=order->PriceTrigger();
               break;
            case IMTOrder::OP_CLOSE_BY:
               record.type=TransactionRecord::TYPE_CLOSE_BY;
               record.price=order->PriceOrder();
               break;
            default:
               m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, eod]",order->Order(),order->Type());
               continue;
           }
         //--- write it
         m_sync.Lock();
         const bool res=m_base.Write(record);
         m_sync.Unlock();
         //--- calculate statistics
         if(res)
            orders_written++;
         else
            orders_failed++;
        }
     }
//--- logging statisitics
   if(logins_total)
     {
      if(!positions_failed && !orders_failed)
         m_api->LoggerOut(MTLogOK,L"'%s' processed successfully [%u accounts, %u positions of %u, %u orders of %u, eod]",group->Group(),logins_total,positions_written,positions_total,orders_written,orders_total);
      else
         m_api->LoggerOut(MTLogErr,L"'%s' processing failed [%u accounts, %u positions written and %u failed of %u, %u orders written and %u failed of %u, eod]",
            group->Group(),logins_total,positions_written,positions_failed,positions_total,orders_written,orders_failed,orders_total);
     }
   else
      m_api->LoggerOut(MTLogOK,L"'%s' is empty [eod]",group->Group());
//--- free interfaces
   account->Release();
   symbol->Release();
   positions->Release();
   orders->Release();
//--- free logins
   m_api->Free(logins);
  }
//+------------------------------------------------------------------+
//| End of day                                                       |
//+------------------------------------------------------------------+
void CPluginInstance::OnEODFinish(const int64_t datetime,const int64_t prev_datetime)
  {
//--- process end of daily report
   DailyProcess(datetime,prev_datetime,true);
  }
//+------------------------------------------------------------------+
//| Add server's balances and rollovers                              |
//+------------------------------------------------------------------+
void CPluginInstance::OnDealPerform(const IMTDeal* deal,IMTAccount* account,IMTPosition* position)
  {
   MTTickShort       tick   ={0};
   TransactionRecord record ={0};
   MTAPIRES          retcode=MT_RET_ERROR;
//--- checks
   if(!deal || !account)
      return;
//--- deposit?
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BALANCE         :
      case IMTDeal::DEAL_CREDIT          :
      case IMTDeal::DEAL_CHARGE          :
      case IMTDeal::DEAL_CORRECTION      :
      case IMTDeal::DEAL_BONUS           :
      case IMTDeal::DEAL_COMMISSION      :
      case IMTDeal::DEAL_DIVIDEND        :
      case IMTDeal::DEAL_DIVIDEND_FRANKED:
      case IMTDeal::DEAL_TAX             :
      case IMTDeal::DEAL_AGENT           :
      case IMTDeal::DEAL_SO_COMPENSATION :
      case IMTDeal::DEAL_SO_COMPENSATION_CREDIT:
         OnDealPerformBalance(deal,account);
         break;
     }
//--- check position
   if(!position)
      return;
//--- check rollover and vmargin only
   if(deal->Reason()!=IMTDeal::DEAL_REASON_ROLLOVER && deal->Reason()!=IMTDeal::DEAL_REASON_VMARGIN)
      return;
//--- under lock
   m_sync.Lock();
//--- get user's group
   if((retcode=m_api->UserGet(account->Login(),m_user))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user %I64u get error [%u, deal perform]",account->Login(),retcode);
      m_sync.Unlock();
      return;
     }
   if((retcode=m_api->GroupGet(m_user->Group(),m_group))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"group %s for user %I64u get error [%u, deal perform]",m_user->Group(),m_user->Login(),retcode);
      m_sync.Unlock();
      return;
     }
//--- check group mask
   if(!CMTStr::CheckGroupMask(m_group_mask.Str(),m_group->Group()))
     {
      m_sync.Unlock();
      return;
     }
//--- get and check symbol
   if((retcode=m_api->SymbolGet(deal->Symbol(),m_group,m_symbol))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"symbol %s get error [%u, deal perform]",deal->Symbol(),retcode);
      m_sync.Unlock();
      return;
     }
//--- check symbol mask
   if(!CMTStr::CheckGroupMask(m_symbol_mask.Str(),m_symbol->Symbol()) &&
      !CMTStr::CheckGroupMask(m_symbol_mask.Str(),m_symbol->Path()))
     {
      m_sync.Unlock();
      return;
     }
//--- get the last tick
   if((retcode=m_api->TickLast(m_symbol,tick))!=MT_RET_OK)
     {
      //--- ok, set all fields to zero
      ZeroMemory(&tick,sizeof(tick));
      //--- write an error to the log file
      m_api->LoggerOut(MTLogErr,L"prices for %s get error [%u, deal perform]",m_symbol->Symbol(),retcode);
     }
//--- general options
   record.login          =account->Login();
   record.leverage       =account->MarginLeverage();
   record.lots           =position->VolumeExt();
   record.bid            =tick.bid;
   record.ask            =tick.ask;
   record.position       =position->Position();
   record.sl             =position->PriceSL();
   record.tp             =position->PriceTP();
   record.reason         =deal->Reason();
   record.digits         =deal->Digits();
   record.digits_currency=deal->DigitsCurrency();
   CMTStr::Copy(record.position_id    ,position->ExternalID());
   CMTStr::Copy(record.currency       ,m_group->Currency());
   CMTStr::Copy(record.group_owner    ,m_group->Company());
   CMTStr::Copy(record.symbol         ,m_symbol->Symbol());
   CMTStr::Copy(record.profit_currency,m_symbol->CurrencyProfit());
//--- set type of operation
   switch(deal->Reason())
     {
      case IMTDeal::DEAL_REASON_ROLLOVER: record.action=TransactionRecord::ACTION_ROLLOVER; break;
      case IMTDeal::DEAL_REASON_VMARGIN : record.action=TransactionRecord::ACTION_VMARGIN;  break;
      default:
         m_sync.Unlock();
         return;
     }
//--- set entry 
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
         //--- unknown value
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, deal perform]",deal->Deal(),deal->Entry());
         m_sync.Unlock();
         return;
     }
//--- set type
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, deal perform]",deal->Deal(),deal->Entry());
         m_sync.Unlock();
         return;
     }
//--- set options
   record.order           =deal->Order();
   record.timestamp       =deal->Time();
   record.deal            =deal->Deal();
   record.price           =deal->Price();
   record.price_position  =deal->PricePosition();
   record.commission      =deal->Commission();
   record.swap            =deal->Storage();
   record.profit          =deal->Profit();
   record.profit_rate     =deal->RateProfit();
   record.profit_raw      =deal->ProfitRaw();
   record.price_gateway   =deal->PriceGateway();
   record.amount_closed   =CalcAmountClosed(m_symbol,deal);
   record.amount          =CalcAmountDeal(m_symbol,deal);
   record.margin_rate     =deal->RateMargin();
   record.margin_amount   =CalcMarginAmount(m_symbol,deal,deal->VolumeExt());
//--- write it
   m_base.Write(record);
//--- unlock
   m_sync.Unlock();
//--- ok
   return;
  }
//+------------------------------------------------------------------+
//| Add server's balances                                            |
//+------------------------------------------------------------------+
void CPluginInstance::OnDealPerformBalance(const IMTDeal* deal,IMTAccount* account)
  {
   TransactionRecord record ={0};
   MTAPIRES          retcode=MT_RET_ERROR;
//--- checks
   if(!deal || !account)
      return;
//--- check dealer
   if(deal->Dealer())
      return;
//--- under lock
   m_sync.Lock();
//--- get user's group
   if((retcode=m_api->UserGet(account->Login(),m_user))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user %I64u get error [%u, deal perform]",account->Login(),retcode);
      m_sync.Unlock();
      return;
     }
   if((retcode=m_api->GroupGet(m_user->Group(),m_group))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"group %s for user %I64u get error [%u, deal perform]",m_user->Group(),m_user->Login(),retcode);
      m_sync.Unlock();
      return;
     }
//--- check group mask
   if(!CMTStr::CheckGroupMask(m_group_mask.Str(),m_group->Group()))
     {
      m_sync.Unlock();
      return;
     }
//--- general options
   record.retcode    =MT_RET_REQUEST_DONE;
   record.login      =deal->Login();
   record.leverage   =m_user->Leverage();
   record.lots       =deal->VolumeExt();
   record.bid        =0;
   record.ask        =0;
   record.sl         =0;
   record.tp         =0;
   CMTStr::Copy(record.currency   ,m_group->Currency());
   CMTStr::Copy(record.group_owner,m_group->Company());
//--- by default entry is unknown
   record.entry=TransactionRecord::ENTRY_UNKNOWN;
//--- reason
   record.reason         =deal->Reason();
   record.digits         =deal->Digits();
   record.digits_currency=deal->DigitsCurrency();
//--- process balance
   if(ProcessBalance(deal,m_symbol,m_group,record))
      m_base.Write(record);
//--- unlock
   m_sync.Unlock();
//--- ok
   return;
  }
//+------------------------------------------------------------------+
//| Add new operation event                                          |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestProcess(const IMTRequest   *request,
                                            const IMTConfirm   *confirm,
                                            const IMTConGroup  *group,
                                            const IMTConSymbol *symbol,
                                            const IMTPosition  *position,
                                            const IMTOrder     *order,
                                            const IMTDeal      *deal)
  {
   TransactionRecord   record    ={0};
   const IMTConSymbol *symbol_ptr=NULL;
   MTAPIRES            retcode   =MT_RET_ERROR;
   bool                res       =false;
//--- checks
   if(!m_api || !m_user || !m_symbol|| !request || !confirm || !group)
      return;
//--- under lock
   m_sync.Lock();
//--- check group mask
   if(!CMTStr::CheckGroupMask(m_group_mask.Str(),group->Group()))
     {
      m_sync.Unlock();
      return;
     }
//--- get the symbol
   if(!symbol)
     {
      if(request)                        retcode=m_api->SymbolGet(request->Symbol() ,m_symbol);
      if(position && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(position->Symbol(),m_symbol);
      if(order    && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(order->Symbol()   ,m_symbol);
      if(deal     && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(deal->Symbol()    ,m_symbol);
      //--- is it ok?
      if(retcode!=MT_RET_OK)
         symbol_ptr=NULL;
      else
         symbol_ptr=m_symbol;
     }
   else
      symbol_ptr=symbol;
//--- check symbol mask
   if(symbol_ptr)
      if(!CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Symbol()) &&
         !CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Path()))
        {
         m_sync.Unlock();
         return;
        }
//--- get user info
   if((retcode=m_api->UserGet(request->Login(),m_user))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user %I64u get error [%u, request process]",request->Login(),retcode);
      m_sync.Unlock();
      return;
     }
//--- general options
   record.retcode    =request->ResultRetcode();
   record.login      =request->Login();
   record.dealer     =request->SourceLogin();
   record.leverage   =m_user->Leverage();
   record.lots       =request->VolumeExt();
   record.bid        =request->ResultMarketBid();
   record.ask        =request->ResultMarketAsk();
   record.sl         =request->PriceSL();
   record.tp         =request->PriceTP();
   CMTStr::Copy(record.ip         ,request->IP());
   CMTStr::Copy(record.currency   ,group->Currency());
   CMTStr::Copy(record.group_owner,group->Company());
   if(symbol_ptr)
     {
      CMTStr::Copy(record.symbol         ,symbol_ptr->Symbol());
      CMTStr::Copy(record.profit_currency,symbol_ptr->CurrencyProfit());
     }
   else
     {
      ZeroMemory(record.symbol         ,sizeof(record.symbol));
      ZeroMemory(record.profit_currency,sizeof(record.profit_currency));
     }
//--- by default entry is unknown
   record.entry=TransactionRecord::ENTRY_UNKNOWN;
//--- reason
   record.reason=UINT_MAX;
   record.digits=request->Digits();
   record.digits_currency=2;
   if(order)
     {
      record.reason         =order->Reason();
      record.digits         =order->Digits();
      record.digits_currency=order->DigitsCurrency();
     }
   else if(deal)
        {
         record.reason         =deal->Reason();
         record.digits         =deal->Digits();
         record.digits_currency=deal->DigitsCurrency();
        }
//--- copy external id
   if(order)
      CMTStr::Copy(record.order_id,order->ExternalID());
   if(deal)
      CMTStr::Copy(record.deal_id,deal->ExternalID());
//--- position
   if(position)
     {
      record.position=position->Position();
      CMTStr::Copy(record.position_id,position->ExternalID());
     }
//--- by transaction action?
   switch(request->Action())
     {
      //--- deal actions
      case IMTRequest::TA_REQUEST           :
      case IMTRequest::TA_INSTANT           :
      case IMTRequest::TA_MARKET            :
      case IMTRequest::TA_EXCHANGE          :
         res=ProcessMarket(deal,symbol_ptr,false,record);
         break;
      case IMTRequest::TA_DEALER_POS_EXECUTE:
         res=ProcessMarket(deal,symbol_ptr,true,record);
         break;
         //--- pending order
      case IMTRequest::TA_PENDING           :
         res=ProcessPendingOrder(order,symbol_ptr,false,record);
         break;
      case IMTRequest::TA_DEALER_ORD_PENDING:
         res=ProcessPendingOrder(order,symbol_ptr,true,record);
         break;
         //--- position modify
      case IMTRequest::TA_SLTP             :
         res=ProcessPositionModify(position,symbol_ptr,false,record);
         break;
      case IMTRequest::TA_DEALER_POS_MODIFY:
         res=ProcessPositionModify(position,symbol_ptr,true,record);
         break;
         //--- pending order modify
      case IMTRequest::TA_MODIFY           :
         res=ProcessOrderModify(order,symbol_ptr,false,record);
         break;
      case IMTRequest::TA_DEALER_ORD_MODIFY:
         res=ProcessOrderModify(order,symbol_ptr,true,record);
         break;
         //--- pending order remove
      case IMTRequest::TA_REMOVE:
         res=ProcessOrderRemove(order,symbol_ptr,false,record);
         break;
      case IMTRequest::TA_DEALER_ORD_REMOVE:
         res=ProcessOrderRemove(order,symbol_ptr,true,record);
         break;
         //--- process pending order by expiration time
      case IMTRequest::TA_EXPIRATION:
         res=ProcessExpiration(order,symbol_ptr,record);
         break;
         //--- stop loss
      case IMTRequest::TA_ACTIVATE_SL:
         res=ProcessSLActivation(deal,symbol_ptr,record);
         break;
         //--- take profit
      case IMTRequest::TA_ACTIVATE_TP:
         res=ProcessTPActivation(deal,symbol_ptr,record);
         break;
         //--- orders activations
      case IMTRequest::TA_ACTIVATE:
         res=ProcessOrderActivation(deal,order,symbol_ptr,false,record);
         break;
      case IMTRequest::TA_DEALER_ORD_ACTIVATE:
         res=ProcessOrderActivation(deal,order,symbol_ptr,true,record);
         break;
         //--- dealer deposit operations
      case IMTRequest::TA_DEALER_BALANCE:
         res=ProcessBalance(deal,symbol_ptr,group,record);
         break;
         //--- activate stop-limit - create new order
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
         res=ProcessStopLimit(order,symbol_ptr,record);
         break;
         //--- stop out orders
      case IMTRequest::TA_STOPOUT_ORDER:
         res=ProcessOrderStopOut(order,symbol_ptr,record);
         break;
         //--- stop out position
      case IMTRequest::TA_STOPOUT_POSITION:
         res=ProcessPositionStopOut(deal,symbol_ptr,record);
         break;
     }
//--- time checks
   if(!record.timestamp)
      record.timestamp=m_api->TimeCurrent();
//--- exists to write?
   if(res)
     {
      //--- write it
      m_base.Write(record);
     }
//--- and unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Trade request process event                                      |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestProcessCloseBy(const IMTRequest*   request,
                                                   const IMTConfirm*   confirm,
                                                   const IMTConGroup*  group,
                                                   const IMTConSymbol* symbol,
                                                   const IMTPosition*  position,
                                                   const IMTOrder*     order,
                                                   const IMTDeal*      deal,
                                                   const IMTDeal*      deal_by)
  {
   TransactionRecord   record    ={0};
   TransactionRecord   record_by ={0};
   const IMTConSymbol *symbol_ptr=NULL;
   MTAPIRES            retcode   =MT_RET_ERROR;
//--- checks
   if(!deal || !deal_by || !m_api || !m_user || !m_symbol|| !request || !confirm || !group)
      return;
//--- check
   if(request->Action()!=IMTRequest::TA_CLOSE_BY &&
      request->Action()!=IMTRequest::TA_DEALER_CLOSE_BY)
      return;
//--- under lock
   m_sync.Lock();
//--- check group mask
   if(!CMTStr::CheckGroupMask(m_group_mask.Str(),group->Group()))
     {
      m_sync.Unlock();
      return;
     }
//--- get the symbol
   if(!symbol)
     {
      if(request)                        retcode=m_api->SymbolGet(request->Symbol() ,m_symbol);
      if(position && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(position->Symbol(),m_symbol);
      if(order    && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(order->Symbol()   ,m_symbol);
      if(deal     && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(deal->Symbol()    ,m_symbol);
      //--- is it ok?
      if(retcode!=MT_RET_OK)
         symbol_ptr=NULL;
      else
         symbol_ptr=m_symbol;
     }
   else
      symbol_ptr=symbol;
//--- check symbol mask
   if(symbol_ptr)
      if(!CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Symbol()) &&
         !CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Path()))
        {
         m_sync.Unlock();
         return;
        }
//--- get user info
   if((retcode=m_api->UserGet(request->Login(),m_user))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user %I64u get error [%u, request process close-by]",request->Login(),retcode);
      m_sync.Unlock();
      return;
     }
//--- common data
   record.retcode    =request->ResultRetcode();
   record.login      =request->Login();
   record.dealer     =request->SourceLogin();
   record.leverage   =m_user->Leverage();
   record.bid        =request->ResultMarketBid();
   record.ask        =request->ResultMarketAsk();
   record.sl         =request->PriceSL();
   record.tp         =request->PriceTP();
   CMTStr::Copy(record.ip         ,request->IP());
   CMTStr::Copy(record.currency   ,group->Currency());
   CMTStr::Copy(record.group_owner,group->Company());
   if(symbol_ptr)
     {
      CMTStr::Copy(record.symbol         ,symbol_ptr->Symbol());
      CMTStr::Copy(record.profit_currency,symbol_ptr->CurrencyProfit());
     }
   else
     {
      ZeroMemory(record.symbol         ,sizeof(record.symbol));
      ZeroMemory(record.profit_currency,sizeof(record.profit_currency));
     }
//--- reason
   record.reason         =deal->Reason();
   record.digits         =deal->Digits();
   record.digits_currency=deal->DigitsCurrency();
//--- copy external id
   if(order)
      CMTStr::Copy(record.order_id,order->ExternalID());
//--- set type of operation
   switch(request->Action())
     {
      case IMTRequest::TA_CLOSE_BY:
         record.action=TransactionRecord::ACTION_CLOSE_BY;
         break;
      case IMTRequest::TA_DEALER_CLOSE_BY:
         record.action=TransactionRecord::ACTION_DEALER_CLOSE_BY;
         break;
      default:
         m_sync.Unlock();
         return;
     }
//--- copy record to record_by
   memcpy(&record_by,&record,sizeof(record));
//--- set entry 
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
         //--- unknown value
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, request process close-by]",deal->Deal(),deal->Entry());
         m_sync.Unlock();
         return;
     }
//--- set type
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, request process close-by]",deal->Deal(),deal->Entry());
         m_sync.Unlock();
         return;
     }
//--- set options
   CMTStr::Copy(record.deal_id,deal->ExternalID());
   record.position      =deal->PositionID();
   record.position_by   =deal_by->PositionID();
   record.order         =deal->Order();
   record.deal          =deal->Deal();
   record.lots          =deal->VolumeExt();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- set entry for deal by
   switch(deal_by->Entry())
     {
      case IMTDeal::ENTRY_OUT   : record_by.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_IN    : record_by.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_INOUT : record_by.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record_by.entry=TransactionRecord::ENTRY_OUT_BY; break;
         //--- unknown value
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, request process close-by]",deal->Deal(),deal->Entry());
         m_sync.Unlock();
         return;
     }
//--- set type for deal by
   switch(deal_by->Action())
     {
      case IMTDeal::DEAL_BUY : record_by.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record_by.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, request process close-by]",deal_by->Deal(),deal_by->Entry());
         m_sync.Unlock();
         return;
     }
//--- for deal by
   record_by.position      =deal_by->PositionID();
   record_by.position_by   =deal->PositionID();
   record_by.order         =deal_by->Order();
   record_by.deal          =deal_by->Deal();
   record_by.lots          =deal_by->VolumeExt();
   record_by.amount        =CalcAmountDeal(symbol,deal_by);
   record_by.timestamp     =deal_by->Time();
   record_by.price         =deal_by->Price();
   record_by.price_position=deal_by->PricePosition();
   record_by.margin_rate   =deal_by->RateMargin();
   record_by.margin_amount =CalcMarginAmount(symbol,deal_by,deal_by->VolumeExt());
   record_by.commission    =deal_by->Commission();
   record_by.swap          =deal_by->Storage();
   record_by.profit        =deal_by->Profit();
   record_by.profit_rate   =deal_by->RateProfit();
   record_by.profit_raw    =deal_by->ProfitRaw();
   record_by.price_gateway =deal_by->PriceGateway();
   record_by.amount_closed =CalcAmountClosed(symbol,deal_by);
//--- write it
   m_base.Write(record);
   m_base.Write(record_by);
//--- and unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Process market transactions                                      |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessMarket(const IMTDeal *deal,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record)
  {
//--- checks
   if(!deal)
      return(false);
//--- set type of operation
   record.action=dealer ? TransactionRecord::ACTION_DEALER_MARKET : TransactionRecord::ACTION_MARKET;
//--- set entry 
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
         //--- unknown value
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, process market]",deal->Deal(),deal->Entry());
         return(false);
     }
//--- set type
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, process market]",deal->Deal(),deal->Entry());
         return(false);
     }
//--- set options
   record.order         =deal->Order();
   record.deal          =deal->Deal();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process pending order transaction                                |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessPendingOrder(const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   record.action=dealer ? TransactionRecord::ACTION_DEALER_PENDING : TransactionRecord::ACTION_PENDING;
//--- set type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, pending order]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.timestamp     =order->TimeSetup();
   record.amount        =CalcAmountOrder(symbol,order);
   record.price         =order->PriceOrder();
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.deal          =0;
   record.price_position=0;
   record.margin_amount =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process position modify transaction                              |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessPositionModify(const IMTPosition *position,const IMTConSymbol *symbol,const bool dealer,TransactionRecord&  record)
  {
//--- checks
   if(!position)
      return(false);
//--- set action
   record.action=dealer ? TransactionRecord::ACTION_DEALER_POS_MODIFY : TransactionRecord::ACTION_POSITION_MODIFY;
//--- set type
   switch(position->Action())
     {
      case IMTPosition::POSITION_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTPosition::POSITION_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for position %s for %I64u [%u, position modify]",position->Symbol(),position->Login(),position->Action());
         return(false);
     }
//--- set options
   record.amount        =CalcAmountPosition(symbol,position);
   record.timestamp     =m_api->TimeCurrent();
   record.price         =position->PriceCurrent();
   record.margin_rate   =position->RateMargin();
//--- haven't order and deal
   record.order         =0;
   record.deal          =0;
   record.price_position=0;
   record.margin_amount =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order modify transaction                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessOrderModify(const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//---
   record.action=dealer ? TransactionRecord::ACTION_DEALER_PENDING_MODIFY : TransactionRecord::ACTION_PENDING_MODIFY;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order modify]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.timestamp     =m_api->TimeCurrent();
   record.amount        =CalcAmountOrder(symbol,order);
   record.price         =order->PriceOrder();
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.price_position=0;
   record.deal          =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order remove transaction                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessOrderRemove(const IMTOrder *order,const IMTConSymbol *symbol,const bool dealer,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   record.action=dealer ? TransactionRecord::ACTION_DEALER_PENDING_CANCEL : TransactionRecord::ACTION_PENDING_CANCEL;
//--- set type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order remove]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.timestamp     =order->TimeDone();
   record.amount        =CalcAmountOrder(symbol,order);
   record.price         =order->PriceOrder();
   record.margin_rate   =order->RateMargin();
//--- haven't deal and position
   record.deal          =0;
   record.price_position=0;
   record.margin_amount =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process expiration order transaction                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessExpiration(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_PENDING_EXPIRATION;
//--- set type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, expiration]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.timestamp     =order->TimeDone();
   record.amount        =CalcAmountOrder(symbol,order);
   record.price         =order->PriceOrder();
   record.margin_rate   =order->RateMargin();
//--- haven't deal and position
   record.deal          =0;
   record.price_position=0;
   record.margin_amount =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process activation order transaction                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessOrderActivation(const IMTDeal      *deal,
                                             const IMTOrder     *order,
                                             const IMTConSymbol *symbol,
                                             const bool          dealer,
                                             TransactionRecord&  record)
  {
//--- checks
   if(!order || !deal)
      return(false);
//--- set action
   record.action=dealer ? TransactionRecord::ACTION_DEALER_PENDING_ACTIVATION : TransactionRecord::ACTION_PENDING_ACTIVATION;
//--- set type of action
   switch(order->Type())
     {
      case IMTOrder::OP_BUY            : record.type=TransactionRecord::TYPE_BUY;             break;
      case IMTOrder::OP_SELL           : record.type=TransactionRecord::TYPE_SELL;            break;
      case IMTOrder::OP_BUY_LIMIT      : record.type=TransactionRecord::TYPE_BUY_LIMIT;       break;
      case IMTOrder::OP_SELL_LIMIT     : record.type=TransactionRecord::TYPE_SELL_LIMIT;      break;
      case IMTOrder::OP_BUY_STOP       : record.type=TransactionRecord::TYPE_BUY_STOP;        break;
      case IMTOrder::OP_SELL_STOP      : record.type=TransactionRecord::TYPE_SELL_STOP;       break;
      case IMTOrder::OP_BUY_STOP_LIMIT : record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;  break;
      case IMTOrder::OP_SELL_STOP_LIMIT: record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT; break;
      case IMTOrder::OP_CLOSE_BY       : record.type=TransactionRecord::TYPE_CLOSE_BY;        break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order activation]",order->Order(),order->Type());
         return(false);
     }
//--- set margin by deal flag
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, order activation]",deal->Deal(),deal->Entry());
         break;
     }
//--- set options
   record.order         =order->Order();
   record.deal          =deal->Deal();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process stop loss transactions                                   |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessSLActivation(const IMTDeal *deal,const IMTConSymbol *symbol,TransactionRecord&  record)
  {
//--- checks
   if(!deal)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_SL;
//--- set type
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, sl]",deal->Deal(),deal->Action());
         return(false);
     }
//--- set entry
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, sl]",deal->Deal(),deal->Entry());
         break;
     }
//--- set options
   record.order         =deal->Order();
   record.deal          =deal->Deal();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process take profit transactions                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessTPActivation(const IMTDeal *deal,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!deal)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_TP;
//--- set type
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, tp]",deal->Deal(),deal->Action());
         return(false);
     }
//--- set entry
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, tp]",deal->Deal(),deal->Entry());
         break;
     }
//--- set options
   record.order         =deal->Order();
   record.deal          =deal->Deal();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process balance transactions                                     |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessBalance(const IMTDeal *deal,const IMTConSymbol *symbol,const IMTConGroup *group,TransactionRecord& record)
  {
//--- checks
   if(!deal)
      return(false);
//--- set actions
   record.action=TransactionRecord::ACTION_DEPOSIT;
//--- set action's type
   switch(deal->Action())
     {
      //--- balance transaction
      case IMTDeal::DEAL_BALANCE   :
         record.type=TransactionRecord::TYPE_DEPOSIT;
         break;
         //--- credit transaction
      case IMTDeal::DEAL_CREDIT    :
         record.type=TransactionRecord::TYPE_CREDIT;
         break;
         //--- charge transaction
      case IMTDeal::DEAL_CHARGE    :
         record.type=TransactionRecord::TYPE_CHARGE;
         break;
         //--- correction transaction
      case IMTDeal::DEAL_CORRECTION:
         record.type=TransactionRecord::TYPE_CORRECTION;
         break;
         //--- bonus translation
      case IMTDeal::DEAL_BONUS     :
         record.type=TransactionRecord::TYPE_BONUS;
         break;
         //--- commission translation
      case IMTDeal::DEAL_COMMISSION:
         record.type=TransactionRecord::TYPE_COMMISSION;
         break;
         //--- commission translation
      case IMTDeal::DEAL_COMMISSION_DAILY:
         record.type=TransactionRecord::TYPE_COMMISSION_DAILY;
         break;
         //--- commission translation
      case IMTDeal::DEAL_COMMISSION_MONTHLY:
         record.type=TransactionRecord::TYPE_COMMISSION_MONTHLY;
         break;
         //--- agent commission translation
      case IMTDeal::DEAL_AGENT_DAILY:
         record.type=TransactionRecord::TYPE_AGENT_DAILY;
         break;
         //--- agent commission translation
      case IMTDeal::DEAL_AGENT_MONTHLY:
         record.type=TransactionRecord::TYPE_AGENT_MONTHLY;
         break;
      case IMTDeal::DEAL_INTERESTRATE:
         record.type=TransactionRecord::TYPE_INTERESTRATE;
         break;
      case IMTDeal::DEAL_BUY_CANCELED:
         record.type=TransactionRecord::TYPE_BUY_CANCELED;
         break;
      case IMTDeal::DEAL_SELL_CANCELED:
         record.type=TransactionRecord::TYPE_SELL_CANCELED;
         break;
      case IMTDeal::DEAL_DIVIDEND:
         record.type=TransactionRecord::TYPE_DIVIDEND;
         break;
      case IMTDeal::DEAL_DIVIDEND_FRANKED:
         record.type=TransactionRecord::TYPE_DIVIDEND_FRANKED;
         break;
      case IMTDeal::DEAL_TAX:
         record.type=TransactionRecord::TYPE_TAX;
         break;
      case IMTDeal::DEAL_AGENT:
         record.type=TransactionRecord::TYPE_AGENT;
         break;
      case IMTDeal::DEAL_SO_COMPENSATION:
         record.type=TransactionRecord::TYPE_SO_COMPENSATION;
         break;
      case IMTDeal::DEAL_SO_COMPENSATION_CREDIT:
         record.type=TransactionRecord::TYPE_SO_COMPENSATION_CREDIT;
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for balance #%I64u [%u, balance]",deal->Deal(),deal->Action());
         return(false);
     }
//--- fill for balance only
   if(group)
     {
      CMTStr::Copy(record.profit_currency,group->Currency());
      record.digits_currency=group->CurrencyDigits();
     }
//--- set options
   record.order         =deal->Order();
   record.deal          =deal->Deal();
   record.amount        =0;
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process stop limit order                                         |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessStopLimit(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_PENDING_ACTIVATION;
//--- set type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, stop limit]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.timestamp     =order->TimeSetup();
   record.amount        =CalcAmountOrder(symbol,order);
   record.price         =order->PriceOrder();
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.deal          =0;
   record.price_position=0;
   record.margin_amount =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process stop out order                                           |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessOrderStopOut(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_STOPOUT_ORDER;
//--- set type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order stopout]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.timestamp     =order->TimeSetup();
   record.amount        =CalcAmountOrder(symbol,order);
   record.price         =order->PriceOrder();
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.deal          =0;
   record.price_position=0;
   record.margin_amount =0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process stop out position                                        |
//+------------------------------------------------------------------+
bool CPluginInstance::ProcessPositionStopOut(const IMTDeal *deal,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!deal || !symbol)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_STOPOUT_POSITION;
//--- set type
   switch(deal->Action())
     {
      case IMTDeal::DEAL_BUY : record.type=TransactionRecord::TYPE_BUY;  break;
      case IMTDeal::DEAL_SELL: record.type=TransactionRecord::TYPE_SELL; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown action for deal #%I64u [%u, process stopout]",deal->Deal(),deal->Action());
         return(false);
     }
//--- set entry
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, process stopout]",deal->Deal(),deal->Entry());
         break;
     }
//--- set options
   record.order         =deal->Order();
   record.deal          =deal->Deal();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeExecution(const IMTConGateway *gateway,
                                       const IMTExecution  *execution,
                                       const IMTConGroup   *group,
                                       const IMTConSymbol  *symbol,
                                       const IMTPosition   *position,
                                       const IMTOrder      *order,
                                       const IMTDeal       *deal)
  {
   TransactionRecord   record    ={0};
   const IMTConSymbol *symbol_ptr=NULL;
   MTAPIRES            retcode   =MT_RET_ERROR;
   bool                res       =false;
   MTTickShort         tick      ={0};
//--- checks
   if(!m_api || !m_user || !m_symbol || !execution || !group)
      return;
//--- under lock
   m_sync.Lock();
//--- check group mask
   if(!CMTStr::CheckGroupMask(m_group_mask.Str(),group->Group()))
     {
      m_sync.Unlock();
      return;
     }
//--- get symbol
   if(!symbol)
     {
      if(execution)                      retcode=m_api->SymbolGet(execution->Symbol(),m_symbol);
      if(position && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(position->Symbol() ,m_symbol);
      if(order    && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(order->Symbol()    ,m_symbol);
      if(deal     && retcode!=MT_RET_OK) retcode=m_api->SymbolGet(deal->Symbol()     ,m_symbol);
      //--- is it ok?
      if(retcode!=MT_RET_OK)
        {
         m_sync.Unlock();
         return;
        }
      //--- set symbol pointer
      symbol_ptr=m_symbol;
     }
   else
      symbol_ptr=symbol;
//--- check symbol mask
   if(!symbol_ptr)
     {
      m_sync.Unlock();
      return;
     }
   if(!CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Symbol()) &&
      !CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Path()))
     {
      m_sync.Unlock();
      return;
     }
//--- general options
   record.login=execution->Login();
   CMTStr::Copy(record.symbol,execution->Symbol());
//--- get user info
   if((retcode=m_api->UserGet(execution->Login(),m_user))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user %I64u get error [%u, execution, action: %u]",execution->Login(),retcode,execution->Action());
      m_sync.Unlock();
      return;
     }
//--- get the last tick
   if((retcode=m_api->TickLast(symbol_ptr,tick))!=MT_RET_OK)
     {
      //--- ok, set all fields to zero
      ZeroMemory(&tick,sizeof(tick));
      //--- write an error to the log file
      m_api->LoggerOut(MTLogErr,L"prices for %s get error [%u, execution]",symbol_ptr->Symbol(),retcode);
     }
//--- general options
   record.retcode    =MT_RET_REQUEST_DONE;
   record.login      =execution->Login();
   record.leverage   =m_user->Leverage();
   record.bid        =tick.bid;
   record.ask        =tick.ask;
   record.sl         =execution->OrderPriceSL();
   record.tp         =execution->OrderPriceTP();
   CMTStr::Copy(record.currency       ,group->Currency());
   CMTStr::Copy(record.group_owner    ,group->Company());
   CMTStr::Copy(record.profit_currency,symbol_ptr->CurrencyProfit());
   CMTStr::Copy(record.symbol         ,symbol_ptr->Symbol());
//--- find dealer for the order
   record.dealer=DealerOrderFind(execution,order);
//--- reason
   record.reason=UINT_MAX;
   if(order)
      record.reason=order->Reason();
   else
      if(deal)
         record.reason=deal->Reason();
//--- copy external id
   if(order)
      CMTStr::Copy(record.order_id,order->ExternalID());
   if(deal)
      CMTStr::Copy(record.deal_id,deal->ExternalID());
//--- position
   if(position)
     {
      record.position=position->Position();
      CMTStr::Copy(record.position_id,position->ExternalID());
     }
//--- digits
   record.digits         =symbol_ptr->Digits();
   record.digits_currency=symbol_ptr->CurrencyProfitDigits();
//--- can't get IP and set it to zero
   record.ip[0]=L'\0';
//--- set default entry
   record.entry=TransactionRecord::ENTRY_UNKNOWN;
//--- use execution time by default
   record.timestamp=execution->Datetime();
//--- by transaction action?
   switch(execution->Action())
     {
      //--- create order
      case IMTExecution::TE_ORDER_NEW:
         res=ExecutionOrderNew(order,symbol_ptr,record);
         break;
         //--- fill order
      case IMTExecution::TE_ORDER_FILL:
         res=ExecutionOrderFill(deal,order,symbol_ptr,record);
         break;
         //--- reject order
      case IMTExecution::TE_ORDER_REJECT:
         res=ExecutionOrderReject(order,symbol_ptr,record);
         break;
         //--- order modify
      case IMTExecution::TE_ORDER_MODIFY:
         res=ExecutionOrderModify(order,symbol_ptr,record);
         break;
         //--- order modify reject
      case IMTExecution::TE_ORDER_MODIFY_REJECT:
         res=ExecutionOrderModifyReject(order,symbol_ptr,record);
         break;
         //--- order cancel
      case IMTExecution::TE_ORDER_CANCEL:
         res=ExecutionOrderCancel(order,symbol_ptr,record);
         break;
         //--- order cancel reject
      case IMTExecution::TE_ORDER_CANCEL_REJECT:
         res=ExecutionOrderCancelReject(order,symbol_ptr,record);
         break;
     }
//--- time checks
   if(!record.timestamp)
      record.timestamp=m_api->TimeCurrent();
//--- exists to write?
   if(res)
     {
      //--- write it
      m_base.Write(record);
     }
//--- and unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Process order new                                                |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderNew(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   if(record.dealer)
      record.action=TransactionRecord::ACTION_DEALER_PENDING;
   else
      record.action=TransactionRecord::ACTION_PENDING;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order new]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.timestamp     =order->TimeSetup();
   record.order         =order->Order();
   record.sl            =order->PriceSL();
   record.tp            =order->PriceTP();
   record.lots          =order->VolumeCurrentExt();
   record.amount        =CalcAmountOrder(symbol,order);
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.margin_amount =0;
   record.deal          =0;
   record.price_position=0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order fill                                               |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderFill(const IMTDeal *deal,const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!deal || !order)
      return(false);
//--- set action
   record.action=TransactionRecord::ACTION_PENDING_ACTIVATION;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order fill]",order->Order(),order->Type());
         return(false);
     }
//--- set entry 
   switch(deal->Entry())
     {
      case IMTDeal::ENTRY_OUT   : record.entry=TransactionRecord::ENTRY_OUT;    break;
      case IMTDeal::ENTRY_IN    : record.entry=TransactionRecord::ENTRY_IN;     break;
      case IMTDeal::ENTRY_INOUT : record.entry=TransactionRecord::ENTRY_INOUT;  break;
      case IMTDeal::ENTRY_OUT_BY: record.entry=TransactionRecord::ENTRY_OUT_BY; break;
         //--- unknown value
      default:
         m_api->LoggerOut(MTLogErr,L"unknown entry for deal #%I64u [%u, order fill]",deal->Deal(),deal->Entry());
         m_sync.Unlock();
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.deal          =deal->Deal();
   record.lots          =deal->VolumeExt();
   record.amount        =CalcAmountDeal(symbol,deal);
   record.timestamp     =deal->Time();
   record.price         =deal->Price();
   record.price_position=deal->PricePosition();
   record.margin_rate   =deal->RateMargin();
   record.margin_amount =CalcMarginAmount(symbol,deal,deal->VolumeExt());
   record.commission    =deal->Commission();
   record.swap          =deal->Storage();
   record.profit        =deal->Profit();
   record.profit_rate   =deal->RateProfit();
   record.profit_raw    =deal->ProfitRaw();
   record.price_gateway =deal->PriceGateway();
   record.amount_closed =CalcAmountClosed(symbol,deal);
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order reject                                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderReject(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- setup retcode
   record.retcode=MT_RET_REQUEST_REJECT;
//--- set action
   if(record.dealer)
      record.action=TransactionRecord::ACTION_DEALER_PENDING;
   else
      record.action=TransactionRecord::ACTION_PENDING;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order reject]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.timestamp     =order->TimeDone();
   record.order         =order->Order();
   record.sl            =order->PriceSL();
   record.tp            =order->PriceTP();
   record.lots          =order->VolumeCurrentExt();
   record.amount        =CalcAmountOrder(symbol,order);
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.margin_amount =0;
   record.deal          =0;
   record.price_position=0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order modify                                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderModify(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   if(record.dealer)
      record.action=TransactionRecord::ACTION_DEALER_PENDING_MODIFY;
   else
      record.action=TransactionRecord::ACTION_PENDING_MODIFY;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order modify]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.order         =order->Order();
   record.sl            =order->PriceSL();
   record.tp            =order->PriceTP();
   record.lots          =order->VolumeCurrentExt();
   record.amount        =CalcAmountOrder(symbol,order);
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.margin_amount =0;
   record.deal          =0;
   record.price_position=0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order reject                                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderModifyReject(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- setup retcode
   record.retcode=MT_RET_REQUEST_REJECT;
//--- set action
   if(record.dealer)
      record.action=TransactionRecord::ACTION_DEALER_PENDING_MODIFY;
   else
      record.action=TransactionRecord::ACTION_PENDING_MODIFY;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order reject]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.timestamp     =order->TimeDone();
   record.order         =order->Order();
   record.sl            =order->PriceSL();
   record.tp            =order->PriceTP();
   record.lots          =order->VolumeCurrentExt();
   record.amount        =CalcAmountOrder(symbol,order);
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.margin_amount =0;
   record.deal          =0;
   record.price_position=0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order cancel                                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderCancel(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- set action
   if(record.dealer)
      record.action=TransactionRecord::ACTION_DEALER_PENDING_CANCEL;
   else
      record.action=TransactionRecord::ACTION_PENDING_CANCEL;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order cancel]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.timestamp     =order->TimeDone();
   record.order         =order->Order();
   record.sl            =order->PriceSL();
   record.tp            =order->PriceTP();
   record.lots          =order->VolumeCurrentExt();
   record.amount        =CalcAmountOrder(symbol,order);
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.margin_amount =0;
   record.deal          =0;
   record.price_position=0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process order reject                                             |
//+------------------------------------------------------------------+
bool CPluginInstance::ExecutionOrderCancelReject(const IMTOrder *order,const IMTConSymbol *symbol,TransactionRecord& record)
  {
//--- checks
   if(!order)
      return(false);
//--- setup retcode
   record.retcode=MT_RET_REQUEST_REJECT;
//--- set action
   if(record.dealer)
      record.action=TransactionRecord::ACTION_DEALER_PENDING_CANCEL;
   else
      record.action=TransactionRecord::ACTION_PENDING_CANCEL;
//--- set record type
   switch(order->Type())
     {
      case IMTOrder::OP_BUY:
         record.type=TransactionRecord::TYPE_BUY;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL           :
         record.type=TransactionRecord::TYPE_SELL;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_LIMIT      :
         record.type=TransactionRecord::TYPE_BUY_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_LIMIT     :
         record.type=TransactionRecord::TYPE_SELL_LIMIT;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP       :
         record.type=TransactionRecord::TYPE_BUY_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_SELL_STOP      :
         record.type=TransactionRecord::TYPE_SELL_STOP;
         record.price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
         record.type=TransactionRecord::TYPE_BUY_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_SELL_STOP_LIMIT:
         record.type=TransactionRecord::TYPE_SELL_STOP_LIMIT;
         record.price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         record.type=TransactionRecord::TYPE_CLOSE_BY;
         record.price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, order cancel reject]",order->Order(),order->Type());
         return(false);
     }
//--- set options
   record.timestamp     =order->TimeDone();
   record.order         =order->Order();
   record.sl            =order->PriceSL();
   record.tp            =order->PriceTP();
   record.lots          =order->VolumeCurrentExt();
   record.amount        =CalcAmountOrder(symbol,order);
   record.margin_rate   =order->RateMargin();
//--- haven't position and deal
   record.margin_amount =0;
   record.deal          =0;
   record.price_position=0;
   record.commission    =0;
   record.swap          =0;
   record.profit        =0;
   record.profit_rate   =0;
   record.profit_raw    =0;
   record.price_gateway =0;
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Process add request                                              |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestAdd(const IMTRequest*   request,
                                        const IMTConGroup*  group,
                                        const IMTConSymbol* symbol,
                                        const IMTPosition*  position,
                                        const IMTOrder*     order)
  {
//--- check dealer requests
   if(request && request->Action()>=IMTRequest::TA_DEALER_FIRST && request->Action()<=IMTRequest::TA_DEALER_LAST)
     {
      DealerOrderAdd(request,order);
      return;
     }
  }
//+------------------------------------------------------------------+
//| Process delete request                                           |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestDelete(const IMTRequest* request)
  {
//--- checks
   if(request)
     {
      //--- it is dealer request?
      if(request->Action()>=IMTRequest::TA_DEALER_FIRST && request->Action()<=IMTRequest::TA_DEALER_LAST)
        {
         if(request->ResultRetcode()!=MT_RET_REQUEST_PLACED)
            DealerOrderDelete(request);
         else
            DealerOrderRefresh();
        }
      //--- check retcode
      switch(request->ResultRetcode())
        {
         case MT_RET_OK:
         case MT_RET_REQUEST_DONE:
         case MT_RET_REQUEST_PLACED:
         case MT_RET_REQUEST_DONE_PARTIAL:
            return;
         default:
            ProcessReject(request);
        }
     }
  }
//+------------------------------------------------------------------+
//| Process cancel request                                           |
//+------------------------------------------------------------------+
void CPluginInstance::OnTradeRequestRefuse(const IMTRequest* request)
  {
   ProcessReject(request);
  }
//+------------------------------------------------------------------+
//| Process reject request                                           |
//+------------------------------------------------------------------+
void CPluginInstance::ProcessReject(const IMTRequest *request)
  {
   TransactionRecord   record    ={0};
   const IMTConSymbol *symbol_ptr=NULL;
   MTAPIRES            retcode   =MT_RET_ERROR;
//--- checks
   if(!m_api || !m_user || !m_symbol|| !request)
      return;
//--- under lock
   m_sync.Lock();
//--- check group mask
   if(!CMTStr::CheckGroupMask(m_group_mask.Str(),request->Group()))
     {
      m_sync.Unlock();
      return;
     }
//--- get the symbol
   retcode=m_api->SymbolGet(request->Symbol(),m_symbol);
//--- is it ok?
   if(retcode!=MT_RET_OK)
      symbol_ptr=NULL;
   else
      symbol_ptr=m_symbol;
//--- check symbol mask
   if(symbol_ptr)
      if(!CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Symbol()) &&
         !CMTStr::CheckGroupMask(m_symbol_mask.Str(),symbol_ptr->Path()))
        {
         m_sync.Unlock();
         return;
        }
//--- get user info
   if((retcode=m_api->UserGet(request->Login(),m_user))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"user %I64u get error [%u, reject]",request->Login(),retcode);
      m_sync.Unlock();
      return;
     }
//--- get group info
   if((retcode=m_api->GroupGet(request->Group(),m_group))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"group for login %I64u get failed [%u, reject]",request->Login(),retcode);
      m_sync.Unlock();
      return;
     }
//--- general options
   record.retcode    =request->ResultRetcode();
   record.timestamp  =m_api->TimeCurrent();
   record.login      =request->Login();
   record.dealer     =request->SourceLogin();
   record.leverage   =m_user->Leverage();
   record.lots       =request->VolumeExt();
   record.bid        =request->ResultMarketBid();
   record.ask        =request->ResultMarketAsk();
   record.sl         =request->PriceSL();
   record.tp         =request->PriceTP();
   record.order      =request->Order();
   record.reason     =UINT_MAX;
   record.digits     =request->Digits();
   CMTStr::Copy(record.ip         ,request->IP());
   CMTStr::Copy(record.currency   ,m_group->Currency());
   CMTStr::Copy(record.group_owner,m_group->Company());
   if(symbol_ptr)
     {
      record.digits     =symbol_ptr->CurrencyProfitDigits();
      CMTStr::Copy(record.symbol         ,symbol_ptr->Symbol());
      CMTStr::Copy(record.profit_currency,symbol_ptr->CurrencyProfit());
     }
   else
     {
      ZeroMemory(record.symbol         ,sizeof(record.symbol));
      ZeroMemory(record.profit_currency,sizeof(record.profit_currency));
     }
//--- by default entry is unknown
   record.entry=TransactionRecord::ENTRY_UNKNOWN;
//--- get information from request
   record.type  =request->Type();
   record.price =request->PriceOrder();
//--- by transaction action?
   switch(request->Action())
     {
      //--- deal actions
      case IMTRequest::TA_REQUEST           :
      case IMTRequest::TA_INSTANT           :
      case IMTRequest::TA_MARKET            :
      case IMTRequest::TA_EXCHANGE          :
         record.action=TransactionRecord::ACTION_MARKET;
         break;
      case IMTRequest::TA_DEALER_POS_EXECUTE:
         record.action=TransactionRecord::ACTION_DEALER_MARKET;
         break;
         //--- pending order
      case IMTRequest::TA_PENDING           :
         record.action=TransactionRecord::ACTION_PENDING;
         break;
      case IMTRequest::TA_DEALER_ORD_PENDING:
         record.action=TransactionRecord::ACTION_DEALER_PENDING;
         break;
         //--- position modify
      case IMTRequest::TA_SLTP             :
         record.action=TransactionRecord::ACTION_POSITION_MODIFY;
         break;
         //--- position modify
      case IMTRequest::TA_DEALER_POS_MODIFY:
         record.action=TransactionRecord::ACTION_DEALER_POS_MODIFY;
         break;
         //--- pending order modify
      case IMTRequest::TA_MODIFY           :
         record.action=TransactionRecord::ACTION_PENDING_MODIFY;
         break;
         //--- pending order modify
      case IMTRequest::TA_DEALER_ORD_MODIFY:
         record.action=TransactionRecord::ACTION_DEALER_PENDING_MODIFY;
         break;
         //--- pending order remove
      case IMTRequest::TA_REMOVE:
         record.action=TransactionRecord::ACTION_PENDING_CANCEL;
         break;
      case IMTRequest::TA_DEALER_ORD_REMOVE:
         record.action=TransactionRecord::ACTION_DEALER_PENDING_CANCEL;
         break;
         //--- process pending order by expiration time
      case IMTRequest::TA_EXPIRATION:
         record.action=TransactionRecord::ACTION_PENDING_EXPIRATION;
         break;
         //--- stop loss
      case IMTRequest::TA_ACTIVATE_SL:
         record.action=TransactionRecord::ACTION_SL;
         break;
         //--- take profit
      case IMTRequest::TA_ACTIVATE_TP:
         record.action=TransactionRecord::ACTION_TP;
         break;
         //--- orders activations
      case IMTRequest::TA_ACTIVATE:
         record.action=TransactionRecord::ACTION_PENDING_ACTIVATION;
         break;
      case IMTRequest::TA_DEALER_ORD_ACTIVATE:
         record.action=TransactionRecord::ACTION_DEALER_PENDING_ACTIVATION;
         break;
         //--- dealer deposit operations
      case IMTRequest::TA_DEALER_BALANCE:
         record.action=TransactionRecord::ACTION_DEPOSIT;
         break;
         //--- activate stop-limit - create new order
      case IMTRequest::TA_ACTIVATE_STOPLIMIT:
         record.action=TransactionRecord::ACTION_PENDING_ACTIVATION;
         break;
         //--- stop out orders
      case IMTRequest::TA_STOPOUT_ORDER:
         record.action=TransactionRecord::ACTION_STOPOUT_ORDER;
         break;
         //--- stop out position
      case IMTRequest::TA_STOPOUT_POSITION:
         record.action=TransactionRecord::ACTION_STOPOUT_POSITION;
         break;
     }
//--- write it
   m_base.Write(record);
//--- and unlock
   m_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Calculate amount in base currency for deal                       |
//+------------------------------------------------------------------+
double CPluginInstance::CalcAmountDeal(const IMTConSymbol *symbol,const IMTDeal *deal) const
  {
   double amount=0,tick_size;
//--- checks
   if(!symbol || !deal)
      return(0);
//--- calculate amount
   switch(symbol->CalcMode())
     {
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount=SMTMath::VolumeExtToSize(deal->VolumeExt(),deal->ContractSize());
         break;
      case IMTConSymbol::TRADE_MODE_FUTURES           :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
         amount=deal->Price()*SMTMath::VolumeExtToDouble(deal->VolumeExt())*deal->TickValue();
         tick_size=deal->TickSize();
         if(tick_size)
            amount/=tick_size;
         break;
      default:
         amount=deal->Price()*SMTMath::VolumeExtToSize(deal->VolumeExt(),deal->ContractSize());
         break;
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(amount,symbol->CurrencyBaseDigits()));
  }
//+------------------------------------------------------------------+
//| Calculate closed amount in base currency for deal                |
//+------------------------------------------------------------------+
double CPluginInstance::CalcAmountClosed(const IMTConSymbol *symbol,const IMTDeal *deal) const
  {
   double amount=0,tick_size;
//--- checks
   if(!symbol || !deal)
      return(0);
//--- calculate amount
   switch(symbol->CalcMode())
     {
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount=SMTMath::VolumeExtToSize(deal->VolumeClosedExt(),deal->ContractSize());
         break;
      case IMTConSymbol::TRADE_MODE_FUTURES           :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
         amount=deal->Price()*SMTMath::VolumeExtToDouble(deal->VolumeClosedExt())*deal->TickValue();
         tick_size=deal->TickSize();
         if(tick_size)
            amount/=tick_size;
         break;
      default:
         amount=deal->Price()*SMTMath::VolumeExtToSize(deal->VolumeClosedExt(),deal->ContractSize());
         break;
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(amount,symbol->CurrencyBaseDigits()));
  }
//+------------------------------------------------------------------+
//| Calculate amount in base currency for order                      |
//+------------------------------------------------------------------+
double CPluginInstance::CalcAmountOrder(const IMTConSymbol *symbol,const IMTOrder *order) const
  {
   double amount=0,price=0,tick_size;
//--- checks
   if(!symbol || !order)
      return(0);
//--- set price
   switch(order->Type())
     {
      case IMTOrder::OP_BUY       :
      case IMTOrder::OP_SELL      :
      case IMTOrder::OP_BUY_LIMIT :
      case IMTOrder::OP_SELL_LIMIT:
      case IMTOrder::OP_BUY_STOP  :
      case IMTOrder::OP_SELL_STOP :
         price=order->PriceOrder();
         break;
      case IMTOrder::OP_BUY_STOP_LIMIT :
      case IMTOrder::OP_SELL_STOP_LIMIT:
         price=order->PriceTrigger();
         break;
      case IMTOrder::OP_CLOSE_BY:
         price=order->PriceOrder();
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown type for order #%I64u [%u, calc amount]",order->Order(),order->Type());
         return(0);
     }
//--- calculate amount
   switch(symbol->CalcMode())
     {
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount=SMTMath::VolumeExtToSize(order->VolumeCurrentExt(),order->ContractSize());
         break;
      case IMTConSymbol::TRADE_MODE_FUTURES           :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
         amount=price*SMTMath::VolumeToDouble(order->VolumeCurrentExt())*symbol->TickValue();
         tick_size=symbol->TickSize();
         if(tick_size)
            amount/=tick_size;
         break;
      default:
         amount=price*SMTMath::VolumeExtToSize(order->VolumeCurrentExt(),order->ContractSize());
         break;
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(amount,symbol->CurrencyBaseDigits()));
  }
//+------------------------------------------------------------------+
//| Calculate amount in base currency for position                   |
//+------------------------------------------------------------------+
double CPluginInstance::CalcAmountPosition(const IMTConSymbol *symbol,const IMTPosition *position) const
  {
   double amount=0,tick_size;
//--- checks
   if(!symbol || !position)
      return(0);
//--- calculate amount
   switch(symbol->CalcMode())
     {
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount=SMTMath::VolumeExtToSize(position->VolumeExt(),position->ContractSize());
         break;
      case IMTConSymbol::TRADE_MODE_FUTURES           :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
         amount=position->PriceCurrent()*SMTMath::VolumeExtToDouble(position->VolumeExt())*symbol->TickValue();
         tick_size=symbol->TickSize();
         if(tick_size)
            amount/=tick_size;
         break;
      default:
         amount=position->PriceCurrent()*SMTMath::VolumeExtToSize(position->VolumeExt(),position->ContractSize());
         break;
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(amount,symbol->CurrencyBaseDigits()));
  }
//+------------------------------------------------------------------+
//| Calculate margin amount in base currency for deal                |
//+------------------------------------------------------------------+
double CPluginInstance::CalcMarginAmount(const IMTConSymbol *symbol,const IMTDeal *deal,const uint64_t volume_ext) const
  {
   double amount=0,margin_maintenance,margin_initial;
//--- checks
   if(!symbol || !deal)
      return(0);
//--- calculate margin amount
   margin_maintenance=symbol->MarginMaintenance();
   if(margin_maintenance)
      return(SMTMath::PriceNormalize(margin_maintenance*deal->RateMargin()*SMTMath::VolumeExtToDouble(volume_ext),symbol->CurrencyBaseDigits()));
   margin_initial=symbol->MarginInitial();
   if(margin_initial)
      return(SMTMath::PriceNormalize(margin_initial*deal->RateMargin()*SMTMath::VolumeExtToDouble(volume_ext),symbol->CurrencyBaseDigits()));
//--- if haven't anything - calculate by calc mode
   switch(symbol->CalcMode())
     {
      //--- forex
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount=SMTMath::VolumeExtToDouble(volume_ext)*deal->RateMargin()*deal->ContractSize();
         break;
         //--- cfd
      case IMTConSymbol::TRADE_MODE_CFD           :
      case IMTConSymbol::TRADE_MODE_CFDINDEX      :
      case IMTConSymbol::TRADE_MODE_CFDLEVERAGE   :
      case IMTConSymbol::TRADE_MODE_EXCH_STOCKS   :
         amount=SMTMath::VolumeExtToDouble(volume_ext)*deal->ContractSize()*deal->Price()*deal->RateMargin();
         break;
         //--- futures & options
      case IMTConSymbol::TRADE_MODE_FUTURES:
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES:
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
      case IMTConSymbol::TRADE_MODE_EXCH_OPTIONS_MARGIN:
         //--- it should be calculated used margin_initial or margin_maintenance
         amount=0;
         break;
         //--- bonds
      case IMTConSymbol::TRADE_MODE_EXCH_BONDS:
         amount=SMTMath::PriceNormalize(SMTMath::VolumeExtToDouble(volume_ext)*deal->ContractSize()*symbol->FaceValue()*deal->Price()/100.0,symbol->CurrencyBaseDigits());
         break;
      default:
         return(0);
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(amount,symbol->CurrencyBaseDigits()));
  }
//+------------------------------------------------------------------+
//| Start daily report thread                                        |
//+------------------------------------------------------------------+
void CPluginInstance::DailyStart(void)
  {
//--- stop thread
   DailyStop();
//--- check if needed
   if(m_daily_time>=0)
     {
      //--- setup work flag
      m_daily_workflag=true;
      //--- start thread
      m_daily_thread.Start(DailyThreadWrapper,this,0);
     }
  }
//+------------------------------------------------------------------+
//| Stop daily report thread                                         |
//+------------------------------------------------------------------+
void CPluginInstance::DailyStop(void)
  {
//--- stop flag
   m_daily_workflag=false;
//--- wait and shutdown
   m_daily_thread.Shutdown(INFINITE);
  }
//+------------------------------------------------------------------+
//| Process daily report                                             |
//+------------------------------------------------------------------+
bool CPluginInstance::DailyProcess(const int64_t ctm,const int64_t prev,const bool eod)
  {
   int64_t      from=0,to=0;
   CMTStrPath base_path;
//--- checks
   if(!m_api)
      return(false);
//--- sync
   m_sync.Lock();
//--- has daily time?
   if(m_daily_time>=0)
     {
      //--- eod?
      if(eod)
        {
         m_sync.Unlock();
         return(false);
        }
      //--- calculate current time with offset
      int64_t currtime=ctm-m_daily_time;
      //--- check time
      if(m_daily_last==0)
         m_daily_last=currtime;
      //--- check day changed
      if((currtime/SECONDS_IN_DAY)==(m_daily_last/SECONDS_IN_DAY))
        {
         m_sync.Unlock();
         return(false);
        }
      //--- save last day
      m_daily_last=currtime;
      //--- setup from and to
      to  =ctm-(m_daily_time ? currtime%SECONDS_IN_DAY : 0);
      from=to-SECONDS_IN_DAY;
     }
   else
     {
      //--- not eod?
      if(!eod)
        {
         m_sync.Unlock();
         return(false);
        }
      //--- setup from and to
      from=prev;
      to  =ctm;
     }
//--- check it
   if(m_base_path.Empty() || m_daily_path.Empty())
     {
      m_sync.Unlock();
      return(false);
     }
//--- prepare report path
   CMTStrPath report_path,tmp;
   int32_t        pos;
   GetModuleFileNameW(NULL,report_path.Buffer(),report_path.Max());
   report_path.Refresh();
   if((pos=report_path.FindRChar(L'\\'))>0)
      report_path.Trim(pos);
   tm ttm={};
   SMTTime::ParseTime(to,&ttm);
   tmp.Format(L"\\%s\\%04d.%02d.%02d.trans",m_daily_path.Str(),ttm.tm_year+1900,ttm.tm_mon+1,ttm.tm_mday);
   report_path.Append(tmp);
//--- check path
   CMTFile::DirectoryCreate(report_path);
//--- add report
   tmp.Format(L"\\%s.csv",m_base_name.Str());
   report_path.Append(tmp);
//--- get base path
   base_path.Assign(m_base_path);
//--- unlock
   m_sync.Unlock();
//--- wait for base standby
   if(!m_base.WaitStandby(BASE_WAIT_STANDBY_TIMEOUT))
      m_api->LoggerOutString(MTLogErr,L"current base standby wait timeout");
//--- logs out
   m_api->LoggerOutString(MTLogOK,L"daily report generation started");
//--- open transaction base and output report
   CTransactionBase base;
   CMTFile          file;
   if(base.Initialize(base_path,m_api) && file.OpenWrite(report_path.Str()) && DailyWritePrepare(file))
     {
      //--- process days
      for(int64_t day=from;day<=to;day+=SECONDS_IN_DAY)
        {
         //--- open day
         if(base.OpenRead(day))
           {
            TransactionRecord record;
            //--- process all records
            while(base.Next(record))
              {
               //--- check time
               if(record.timestamp>=from && record.timestamp<=to)
                 {
                  //--- write to output
                  if(!DailyWrite(file,record))
                    {
                     //--- process error
                     file.Close();
                     DeleteFileW(report_path.Str());
                     base.Close();
                     m_api->LoggerOutString(MTLogErr,L"daily report generation failed");
                     return(false);
                    }
                 }
              }
           }
        }
     }
   else
     {
      file.Close();
      DeleteFileW(report_path.Str());
      base.Close();
      m_api->LoggerOutString(MTLogErr,L"daily report generation failed");
      return(false);
     }
//--- logs out
   m_api->LoggerOutString(MTLogOK,L"daily report generation finished");
//--- ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Daily write header                                               |
//+------------------------------------------------------------------+
bool CPluginInstance::DailyWritePrepare(CMTFile& file)
  {
//--- setup compression for low disk usage
   uint16_t format=COMPRESSION_FORMAT_DEFAULT;
   DWORD  dummy=0;
   DeviceIoControl(file.Handle(),FSCTL_SET_COMPRESSION,&format,sizeof(uint16_t),NULL,0,&dummy,NULL);
//--- prepare header
   int32_t len=CMTStr::FormatStr(m_daily_buf,
                             L"LOGIN;LEVERAGE;DEALER;IP;CURRENCY;ORDER;ORDER_ID;DEAL;DEAL_ID;POSITION;POSITION_ID;ACTION;TYPE;ENTRY;SYMBOL;POSITION_BY;LOTS;"
                             L"AMOUNT;TIME;PRICE;BID;ASK;POSITION_PRICE;STOPLOSS;TAKEPROFIT;"
                             L"MARGIN_RATE;MARGIN_AMOUNT;COMMISSION;SWAP;PROFIT;PROFIT_CURRENCY;PROFIT_RATE;"
                             L"PROFIT_RAW;AMOUNT_CLOSED;GATEWAY_PRICE;REASON;RETCODE;GROUP_OWNER\r\n");
//--- write to file header with BOM start
   uint8_t unicode[2]={0xff,0xfe};
   return(file.Write(unicode,sizeof(unicode))==sizeof(unicode) && file.Write(m_daily_buf,len*sizeof(wchar_t))==len*sizeof(wchar_t));
  }
//+------------------------------------------------------------------+
//| Daily write record                                               |
//+------------------------------------------------------------------+
bool CPluginInstance::DailyWrite(CMTFile& file,TransactionRecord& record)
  {
   CMTStr32 dealer,deal,order,position,position_by,price,price_pos,sl,tp,bid,ask,volume,timestamp,profit_rate,price_gateway;
//--- checks
   if(!record.digits && !record.digits_currency)
     {
      record.digits=5;
      record.digits_currency=2;
     }
//---
   if(record.dealer)
      dealer.Format(L"%I64u",record.dealer);
//---
   if(record.deal)
      deal.Format(L"%I64u",record.deal);
//---
   if(record.order)
      order.Format(L"%I64u",record.order);
//---
   if(record.position)
      position.Format(L"%I64u",record.position);
//---
   if(record.position_by)
      position_by.Format(L"%I64u",record.position_by);
//--- prices
   if(record.price)
      SMTFormat::FormatPrice(price,record.price,record.digits);
   if(record.price_position)
      SMTFormat::FormatPrice(price_pos,record.price_position,record.digits,3);
   if(record.sl)
      SMTFormat::FormatPrice(sl,record.sl,record.digits);
   if(record.tp)
      SMTFormat::FormatPrice(sl,record.tp,record.digits);
   SMTFormat::FormatVolumeExt(volume,record.lots,false);
//---
   SMTFormat::FormatPrice(bid,record.bid,record.digits);
   SMTFormat::FormatPrice(ask,record.ask,record.digits);
   SMTFormat::FormatPrice(profit_rate,record.profit_rate,record.digits);
   SMTFormat::FormatPrice(price_gateway,record.price_gateway,record.digits);
//---
   SMTFormat::FormatDateTime(timestamp,record.timestamp,true,true);
//--- prepare row
   int32_t len=CMTStr::FormatStr(m_daily_buf,
                                 L"%I64u;%u;%s;%s;%s;"
                                 L"%s;%s;%s;%s;%s;%s;"
                                 L"%s;%s;"
                                 L"%s;%s;%s;"
                                 L"%s;%.2lf;"
                                 L"%s;%s;%s;%s;"
                                 L"%s;%s;%s;"
                                 L"%.2lf;%.2lf;"
                                 L"%.2lf;%.2lf;"
                                 L"%.2lf;%s;%s;%.2lf;%.2lf;"
                                 L"%s;%s;%s;%s\r\n",
      record.login,record.leverage,dealer.Str(),record.ip,record.currency,
      order.Str(),record.order_id,deal.Str(),record.deal_id,position.Str(),record.position_id,
      CTradeTransactionReport::PrintAction(record.action),CTradeTransactionReport::PrintType(record.type),
      CTradeTransactionReport::PrintEntry(record.entry),record.symbol,position_by.Str(),
      volume.Str(),record.amount,
      timestamp.Str(),price.Str(),bid.Str(),ask.Str(),
      price_pos.Str(),sl.Str(),tp.Str(),
      record.margin_rate,record.margin_amount,
      record.commission,record.swap,
      record.profit,record.profit_currency,profit_rate.Str(),record.profit_raw,record.amount_closed,
      price_gateway.Str(),CTradeTransactionReport::PrintReason(record.reason),SMTFormat::FormatError(record.retcode),record.group_owner);
//--- пишем в файл
   return(file.Write(m_daily_buf,len*sizeof(wchar_t))==len*sizeof(wchar_t));
  }
//+------------------------------------------------------------------+
//| Daily thread                                                     |
//+------------------------------------------------------------------+
uint32_t __stdcall CPluginInstance::DailyThreadWrapper(LPVOID param)
  {
//--- checks
   CPluginInstance *pThis=reinterpret_cast<CPluginInstance*>(param);
   if(pThis)
      pThis->DailyThread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Daily thread                                                     |
//+------------------------------------------------------------------+
__declspec(noinline) void CPluginInstance::DailyThread(void)
  {
//--- check work flag
   while(m_daily_workflag)
     {
      //--- check
      if(m_api)
         DailyProcess(m_api->TimeCurrent(),0,false);
      //--- sleep
      Sleep(500);
     }
  }
//+------------------------------------------------------------------+
//| Parameter update                                                 |
//+------------------------------------------------------------------+
bool CPluginInstance::ParamUpdate(IMTConPlugin *config,IMTConParam *param)
  {
//--- checks
   if(m_api && config && param)
     {
      IMTConParam *tmp=m_api->PluginParamCreate();
      //--- checks
      if(tmp)
        {
         //--- find the parameter by name
         for(uint32_t pos=0;config->ParameterNext(pos,tmp)==MT_RET_OK;pos++)
           {
            //--- check name
            if(CMTStr::Compare(param->Name(),tmp->Name())==0)
              {
               //--- update
               config->ParameterUpdate(pos,param);
               tmp->Release();
               return(m_api->PluginAdd(config)==MT_RET_OK);
              }
           }
         //--- release
         tmp->Release();
        }
     }
//--- not found
   return(false);
  }
//+------------------------------------------------------------------+
//| Dealer order requests cache                                      |
//+------------------------------------------------------------------+
void CPluginInstance::DealerOrderAdd(const IMTRequest *request,const IMTOrder *order)
  {
//--- checks
   if(request && order && m_api && order->Order())
     {
      //--- prepare dealer order

      DealerOrder dealer_order={};
      dealer_order.request     =request->ID();
      dealer_order.ctm         =m_api->TimeCurrent();
      dealer_order.order       =order->Order();
      dealer_order.login       =order->Login();
      dealer_order.dealer     =request->SourceLogin();
      dealer_order.action     =request->Action();
      //--- add to dealer orders cache
      m_dealer_orders_sync.Lock();
      if(!m_dealer_orders.Total() || (dealer_order.order>m_dealer_orders[m_dealer_orders.Total()-1].order))
         m_dealer_orders.Add(&dealer_order);
      else
        {
         DealerOrder *ptr=m_dealer_orders.Search(&dealer_order,SortDealerOrders);
         if(ptr)
            *ptr=dealer_order;
         else
            m_dealer_orders.Insert(&dealer_order,SortDealerOrders);
        }
      m_dealer_orders_sync.Unlock();
     }
  }
//+------------------------------------------------------------------+
//| Dealer order requests cache                                      |
//+------------------------------------------------------------------+
void CPluginInstance::DealerOrderDelete(const IMTRequest *request)
  {
//--- checks
   if(request)
     {
      m_dealer_orders_sync.Lock();
      //--- checks
      if(m_dealer_orders.Total())
        {
         //--- find order
         uint64_t find_order=request->Order();
         DealerOrder *ptr=m_dealer_orders.Search(&find_order,SearchDealerOrders);
         if(ptr)
            m_dealer_orders.Delete(ptr);
        }
      //--- unlock
      m_dealer_orders_sync.Unlock();
     }
  }
//+------------------------------------------------------------------+
//| Dealer order requests cache                                      |
//+------------------------------------------------------------------+
uint64_t CPluginInstance::DealerOrderFind(const IMTExecution *execution,const IMTOrder *order)
  {
   uint64_t dealer=0;
//--- checks
   if(execution && order)
     {
      m_dealer_orders_sync.Lock();
      //--- find order
      uint64_t find_order =order->Order();
      uint64_t find_login =order->Login();
      DealerOrder *ptr=m_dealer_orders.Search(&find_order,SearchDealerOrders);
      if(ptr && ptr->login==find_login)
        {
         //--- detect our action
         switch(execution->Action())
           {
            case IMTExecution::TE_ORDER_NEW:
               if(ptr->action==IMTRequest::TA_DEALER_POS_EXECUTE || ptr->action==IMTRequest::TA_DEALER_ORD_PENDING)
                  dealer=ptr->dealer;
               break;
            case IMTExecution::TE_ORDER_REJECT:
               if(ptr->action==IMTRequest::TA_DEALER_POS_EXECUTE || ptr->action==IMTRequest::TA_DEALER_ORD_PENDING)
                 {
                  dealer=ptr->dealer;
                  m_dealer_orders.Delete(ptr);
                 }
               break;
            case IMTExecution::TE_ORDER_CANCEL:
            case IMTExecution::TE_ORDER_CANCEL_REJECT:
               if(ptr->action==IMTRequest::TA_DEALER_ORD_REMOVE)
                 {
                  dealer=ptr->dealer;
                  m_dealer_orders.Delete(ptr);
                 }
               break;
            case IMTExecution::TE_ORDER_MODIFY:
            case IMTExecution::TE_ORDER_MODIFY_REJECT:
               if(ptr->action==IMTRequest::TA_DEALER_ORD_MODIFY)
                 {
                  dealer=ptr->dealer;
                  m_dealer_orders.Delete(ptr);
                 }
               break;
            case IMTExecution::TE_ORDER_FILL:
               if(ptr->action==IMTRequest::TA_DEALER_POS_EXECUTE || ptr->action==IMTRequest::TA_DEALER_ORD_PENDING)
                 {
                  dealer=ptr->dealer;
                  if(order->VolumeCurrent()==0)
                     m_dealer_orders.Delete(ptr);
                 }
               break;
           }
        }
      //--- unlock
      m_dealer_orders_sync.Unlock();
     }
//--- result
   return(dealer);
  }
//+------------------------------------------------------------------+
//| Dealer order requests cache                                      |
//+------------------------------------------------------------------+
void CPluginInstance::DealerOrderRefresh(void)
  {
//--- checks
   if(m_api && m_dealer_orders.Total())
     {
      //--- current time
      int64_t ctm=m_api->TimeCurrent();
      //--- check time
      if((ctm-m_dealer_orders_last)>DEALER_ORDERS_TIMEOUT)
        {
         m_dealer_orders_sync.Lock();
         //--- delete old orders
         DealerOrderArray dealer_orders_swap;
         dealer_orders_swap.Reserve(m_dealer_orders.Total());
         for(uint32_t i=0;i<m_dealer_orders.Total();i++)
            if((ctm-m_dealer_orders[i].ctm)<DEALER_ORDERS_TIMEOUT)
               dealer_orders_swap.Add(&m_dealer_orders[i]);
         m_dealer_orders.Swap(dealer_orders_swap);
         m_dealer_orders_sync.Unlock();
         //--- update last time
         m_dealer_orders_last=ctm;
        }
     }
  }
//+------------------------------------------------------------------+
//| Sort orders in the array                                         |
//+------------------------------------------------------------------+
int32_t CPluginInstance::SortDealerOrders(const void* left,const void* right)
  {
   DealerOrder *lft=(DealerOrder*)left;
   DealerOrder *rgh=(DealerOrder*)right;
//---
   if(lft->order>rgh->order) return(1);
   if(lft->order<rgh->order) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Search order in the array                                        |
//+------------------------------------------------------------------+
int32_t CPluginInstance::SearchDealerOrders(const void* left,const void* right)
  {
   uint64_t       lft=*(uint64_t*)left;
   DealerOrder *rgh=(DealerOrder*)right;
//---
   if(lft>rgh->order) return(1);
   if(lft<rgh->order) return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
