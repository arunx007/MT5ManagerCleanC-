//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "Gateway.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CGateway::CGateway(void)
   : m_api_gateway(NULL),
     m_api_gateway_config(NULL),
     m_api_exchange(NULL),
     m_trade_dispatcher(),
     m_connect_time(0),
     m_connect_attempts(0),
     m_status_api_gateway(STATUS_API_STOPPED),
     m_status_api_exchange(STATUS_DISCONNECTED)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CGateway::~CGateway(void)
  {
//--- shutdown everything
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CGateway::Initialize(IMTGatewayAPI *api_gateway)
  {
//--- check
   if(!api_gateway)
      return(false);
//--- shutdown everything
   Shutdown();
//--- remember pointer to Gateway API
   m_api_gateway=api_gateway;
//--- create object of external trading system API
   if((m_api_exchange=new CExchangeAPI(this))==NULL)
     {
      ExtLogger.OutString(MTLogErr,L"failed to create Exchange API instance");
      m_api_gateway=NULL;
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CGateway::Shutdown(void)
  {
//--- update Gateway API state
   StatusGateway(STATUS_API_STOPPED);
//--- shutdown external trading system API
   if(m_api_exchange)
     {
      //--- update state of external connection
      StatusExchange(STATUS_DISCONNECTED);
      //--- shutdown external trading system and delete pointer
      m_api_exchange->Shutdown();
      delete m_api_exchange;
      m_api_exchange=NULL;
     }
//--- clear pointer to Gateway API
   m_api_gateway=NULL;
//--- release gateway config interface
   if(m_api_gateway_config)
     {
      m_api_gateway_config->Release();
      m_api_gateway_config=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Check gateway state                                              |
//+------------------------------------------------------------------+
void CGateway::Check(void)
  {
//--- check
   if(!m_api_exchange)
      return;
//--- exit if Gateway API is not ready for work
   if(StatusGateway()<=STATUS_API_CONFIGURED)
      return;
//--- API is ready for work, check connection to external trading system
   if(StatusExchange()>=STATUS_CONNECTED)
     {
      //--- check connection to external trading system
      if(!m_api_exchange->Check())
        {
         //--- external trading system haven't responded during timeout
         ExtLogger.OutString(MTLogErr,L"exchange timed out");
         StatusExchange(STATUS_DISCONNECTED);
         return;
        }
     }
//--- analyze state, start connection in the check thread,
//--- it is safe, because external trading system API is operating asynchronously
   if(StatusExchange()<=STATUS_CONNECTING)
      Connect();
  }
//+------------------------------------------------------------------+
//| Receive gateway settings                                         |
//+------------------------------------------------------------------+
bool CGateway::OnGatewayConfig(const IMTConGateway *config)
  {
//--- check
   if(!config)
      return(false);
//--- check if settings are already received, exit
   if(StatusGateway()!=STATUS_API_STOPPED)
      return(true);
//--- create interface instance of gateway settings
   if((m_api_gateway_config=m_api_gateway->GatewayCreate())==NULL)
     {
      ExtLogger.OutString(MTLogErr,L"failed to create gateway config instance");
      return(false);
     }
//--- copy settings
   if(m_api_gateway_config->Assign(config)!=MT_RET_OK)
     {
      ExtLogger.OutString(MTLogErr,L"failed to copy gateway config instance");
      return(false);
     }
//--- update state
   StatusGateway(STATUS_API_CONFIGURED);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Clear gateway settings                                           |
//+------------------------------------------------------------------+
void CGateway::OnGatewayConfigClear(void)
  {
//--- if gateway is already connected, write to log
   if(StatusGateway()==STATUS_API_STARTED)
     {
      ExtLogger.OutString(MTLogWarn,L"config clearing skipped, gateway not stopped");
      return;
     }
//--- check
   if(m_api_gateway_config!=NULL)
     {
      //--- notify of gateway settings cleared
      ExtLogger.Out(MTLogOK,L"'%s' settings cleared",m_api_gateway_config->Name());
      //--- clear config
      m_api_gateway_config->Clear();
      //--- update state
      StatusGateway(STATUS_API_STOPPED);
     }
  }
//+------------------------------------------------------------------+
//| Notify of gateway start                                          |
//+------------------------------------------------------------------+
void CGateway::OnGatewayStart(void)
  {
//--- if gateway is already connected, write to log
   if(StatusGateway()==STATUS_API_STARTED)
     {
      ExtLogger.OutString(MTLogWarn,L"start skipped, remote connection already exist");
      return;
     }
//--- if gateway has not received settings, write to log
   if(StatusGateway()!=STATUS_API_CONFIGURED)
     {
      ExtLogger.OutString(MTLogWarn,L"start failed, gateway settings didn't receive");
      return;
     }
//--- write to log
   ExtLogger.Out(MTLogOK,L"'%s' initialized",m_api_gateway_config->Name());
//--- update state of connection to external trading system (start connection process)
   StatusExchange(STATUS_CONNECTING);
//--- update state of Gateway API
   StatusGateway(STATUS_API_STARTED);
  }
//+------------------------------------------------------------------+
//| Notify of gateway stop                                           |
//+------------------------------------------------------------------+
void CGateway::OnGatewayStop(void)
  {
//--- write to log
   ExtLogger.OutString(MTLogOK,L"stopped");
//--- update state
   StatusGateway(STATUS_API_CONFIGURED);
//--- disconnect
   Disconnect();
  }
//+------------------------------------------------------------------+
//| Lock trade request                                               |
//+------------------------------------------------------------------+
void CGateway::OnGatewayDealerLock(const MTAPIRES retcode,const IMTRequest *request)
  {
//--- update user external account
   UpdateExternalAccount(request);
//--- process request
   m_trade_dispatcher.OnGatewayDealerLock(retcode,request);
  }
//+------------------------------------------------------------------+
//| Response for trade request confirmation                          |
//+------------------------------------------------------------------+
void CGateway::OnGatewayDealerAnswer(const MTAPIRES retcode,const IMTConfirm *confirm) const
  {
//--- process
   m_trade_dispatcher.OnGatewayDealerAnswer(retcode,confirm);
  }
//+------------------------------------------------------------------+
//| Async positions request                                          |
//+------------------------------------------------------------------+
MTAPIRES CGateway::HookGatewayPositionsRequest(void)
  {
   ExchangePositionsArray positions_exchange;
   IMTPositionArray   *positions_gateway=NULL;
   MTAPIRES            res              =MT_RET_OK;
//--- get exchange positions
   if(!m_api_exchange->SendAccountDataRequest(0))
      res=MT_RET_ERROR;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Asyn account state request                                       |
//+------------------------------------------------------------------+
MTAPIRES CGateway::HookGatewayAccountRequest(uint64_t login,LPCWSTR account_id)
  {
   bool res;
//--- request account data from exchange
   res=m_api_exchange->SendAccountDataRequest(login);
//--- return result
   return(res?MT_RET_OK:MT_RET_ERROR);
  }
//+------------------------------------------------------------------+
//| Send tick to Gateway API                                         |
//+------------------------------------------------------------------+
bool CGateway::GatewayTickSend(MTTick &tick)
  {
   bool res=false;
//--- send data
   if(m_api_gateway)
      res=(m_api_gateway->SendTicks(&tick,1)==MT_RET_OK);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send book to Gateway API                                         |
//+------------------------------------------------------------------+
bool CGateway::GatewayBookSend(MTBook &book)
  {
   bool res=false;
//--- send data
   if(m_api_gateway)
      res=(m_api_gateway->SendBooks(&book,1)==MT_RET_OK);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Get external connection state                                    |
//+------------------------------------------------------------------+
LONG CGateway::StatusExchange(void)
  {
//--- get state
   return(InterlockedExchangeAdd(&m_status_api_exchange,0));
  }
//+------------------------------------------------------------------+
//| Set external connection state                                    |
//+------------------------------------------------------------------+
LONG CGateway::StatusExchange(const LONG status)
  {
   LONG old_status=InterlockedExchange(&m_status_api_exchange,status);
//--- notify of gateway state changed
   if(m_api_gateway)
     {
      //--- check state
      if(status==STATUS_SYNCHRONIZED)
        {
         //--- connect to the trading request queue
         m_api_gateway->DealerStart(IMTGatewayAPI::DEALER_FLAG_AUTOLOCK|IMTGatewayAPI::DEALER_FLAG_EXTERNAL_ACC);
         //--- update connection state
         m_api_gateway->StateConnect(1);
         //--- clear reconnect time/attempts
         m_connect_time    =0;
         m_connect_attempts=0;
        }
      else
         //--- update connection state
         m_api_gateway->StateConnect(0);
     }
//--- write to log if disconnected
   if(old_status!=STATUS_DISCONNECTED && status==STATUS_DISCONNECTED)
     {
      //--- if synchronized, disconnect from the request queue
      if(old_status==STATUS_SYNCHRONIZED)
         m_api_gateway->DealerStop();
      //--- write to log
      ExtLogger.OutString(MTLogOK,L"connect to exchange closed");
     }
//--- return old state
   return(old_status);
  }
//+------------------------------------------------------------------+
//| Notification of successful gateway synchronization               |
//+------------------------------------------------------------------+
void CGateway::OnExchangeSyncComplete(void)
  {
//--- update state
   StatusExchange(STATUS_SYNCHRONIZED);
//--- write to log
   ExtLogger.OutString(MTLogOK,L"synchronized with exchange");
  }
//+------------------------------------------------------------------+
//| Notification of closed connection                                |
//+------------------------------------------------------------------+
void CGateway::OnExchangeConnectClosed(void)
  {
//--- update state
   StatusExchange(STATUS_DISCONNECTED);
  }
//+------------------------------------------------------------------+
//| Notify of added symbol                                           |
//+------------------------------------------------------------------+
bool CGateway::OnExchangeSymbolAdd(const ExchangeSymbol &exchange_symbol)
  {
   bool res=false;
//--- check
   if(!m_api_gateway || !m_api_gateway_config)
      return(false);
//--- check if symbols import via gateway is allowed
   if((m_api_gateway_config->Flags()&IMTConGateway::GATEWAY_FLAG_IMPORT_SYMBOLS)==0)
      return(false);
//--- create symbol
   IMTConSymbol *gateway_symbol=NULL;
   if((gateway_symbol=m_api_gateway->SymbolCreate())==NULL)
     {
      ExtLogger.OutString(MTLogErr,L"failed to create symbol interface instance");
      return(false);
     }
//--- import symbol
   res=ImportSymbol(exchange_symbol,gateway_symbol);
//--- log error message
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to import symbol %s",exchange_symbol.symbol);
//--- add symbol to Gateway API
   res=res && (m_api_gateway->SymbolUpdate(gateway_symbol)==MT_RET_OK);
//--- release symbol interface
   gateway_symbol->Release();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Notify of order transaction in external trading system           |
//+------------------------------------------------------------------+
bool CGateway::OnExchangeOrderTrans(const ExchangeOrder &exchange_order,const ExchangeSymbol &symbol)
  {
//--- pass order transaction to trade dispatcher
   return(m_trade_dispatcher.OnExchangeOrderTrans(exchange_order,symbol));
  }
//+------------------------------------------------------------------+
//| Notify of deal transaction in external trading system            |
//+------------------------------------------------------------------+
bool CGateway::OnExchangeDealTrans(const ExchangeDeal &exchange_deal,const ExchangeSymbol &symbol)
  {
   return(m_trade_dispatcher.OnExchangeDealTrans(exchange_deal,symbol));
  }
//+------------------------------------------------------------------+
//| Notify of account data                                           |
//+------------------------------------------------------------------+
bool CGateway::OnExchangeAccountDataReceived(const ExchangeAccountData &account_data)
  {
   IMTUser            *user_gateway     =NULL;
   IMTAccount         *account_gateway  =NULL;
   IMTOrderArray      *orders_gateway   =NULL;
   IMTPositionArray   *positions_gateway=NULL;
   MTAPIRES            res              =MT_RET_OK;
//--- create user interface
   user_gateway=m_api_gateway->UserCreate();
//--- create account interface
   account_gateway=m_api_gateway->UserCreateAccount();
//--- create positions array interface
   positions_gateway=m_api_gateway->GatewayPositionArrayCreate();
//--- create orders array interface
   orders_gateway=m_api_gateway->GatewayOrderArrayCreate();
//--- check interfaces
   if(!user_gateway || !account_gateway || !orders_gateway || !positions_gateway)
      res=MT_RET_ERR_MEM;
//--- if login specified 
   if(account_data.login>0)
     {
      //--- request user data 
      res=m_api_gateway->UserGet(account_data.login,user_gateway);
      //--- set account's balance
      if(res==MT_RET_OK)
         res=account_gateway->Balance(account_data.balance);
     }
//--- convert orders
   if(res==MT_RET_OK && account_data.orders.Total()>0)
      res=ConvertOrders(account_data.orders,orders_gateway);
//--- convert positions
   if(res==MT_RET_OK && account_data.positions.Total()>0)
     {
      res=ConvertPositions(account_data.positions,positions_gateway);
     }
//--- answer with data
   if(res==MT_RET_OK)
     {
      //--- if login specified, answer on HookGatewayAccountRequest
      if(account_data.login)
         res=m_api_gateway->GatewayAccountAnswer(res,m_api_gateway->TimeCurrent(),user_gateway,account_gateway,orders_gateway,positions_gateway);
      else
        {
         //--- answer on HookGatewayPositionsRequest
         res=m_api_gateway->GatewayPositionsAnswer(res,m_api_gateway->TimeCurrent(),positions_gateway);
        }
     }
//--- release interfaces
   if(user_gateway)
      user_gateway->Release();
   if(account_gateway)
      account_gateway->Release();
   if(orders_gateway)
      orders_gateway->Release();
   if(positions_gateway)
      positions_gateway->Release();
//--- вернем результат
   return(res==MT_RET_OK || res==MT_RET_OK_NONE);
  }
//+------------------------------------------------------------------+
//| Connect                                                          |
//+------------------------------------------------------------------+
bool CGateway::Connect(void)
  {
   int64_t ctm=_time64(nullptr);
//--- check
   if(!m_api_exchange || !m_api_gateway_config)
     {
      StatusExchange(STATUS_DISCONNECTED);
      return(false);
     }
//--- check reconnect timeout
   if(ctm<m_connect_time)
      return(false);
//--- set connect time
   m_connect_time=ctm+((m_connect_attempts++<RECONNECT_LONG_COUNT)?RECONNECT_SHORT_TIMEOUT:RECONNECT_LONG_TIMEOUT);
//--- initialize trade dispatcher
   if(!m_trade_dispatcher.Initialize(m_api_gateway,m_api_exchange))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize trade dispatcher");
      StatusExchange(STATUS_DISCONNECTED);
      return(false);
     }
//--- update state
   StatusExchange(STATUS_CONNECTED);
//--- initialize external trading system API
   if(!m_api_exchange->Initialize(m_api_gateway_config->TradingServer(),m_api_gateway_config->TradingLogin(),m_api_gateway_config->TradingPassword()))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize Exchange API");
      StatusExchange(STATUS_DISCONNECTED);
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Disconnect from server                                           |
//+------------------------------------------------------------------+
void CGateway::Disconnect(void)
  {
//--- stop external trading system API
   if(m_api_exchange)
      m_api_exchange->Shutdown();
  }
//+------------------------------------------------------------------+
//| Import symbol data                                               |
//+------------------------------------------------------------------+
bool CGateway::ImportSymbol(const ExchangeSymbol &exchange_symbol,IMTConSymbol *gateway_symbol) const
  {
   bool res=false;
//--- check
   if(!gateway_symbol)
      return(false);
//--- symbol name
   res=gateway_symbol->Symbol(exchange_symbol.symbol)==MT_RET_OK;
//--- path
   if(res)
     {
      CMTStr256 path;
      path.Format(L"%s\\%s",exchange_symbol.path,exchange_symbol.symbol);
      res=gateway_symbol->Path(path.Str())==MT_RET_OK;
     }
//--- description
   res=res && gateway_symbol->Description(exchange_symbol.description)==MT_RET_OK;
//--- page
   res=res && gateway_symbol->Page(exchange_symbol.page)==MT_RET_OK;
//--- base currency
   res=res && gateway_symbol->CurrencyBase(exchange_symbol.currency_base)==MT_RET_OK;
//--- profit currency
   res=res && gateway_symbol->CurrencyProfit(exchange_symbol.currency_profit)==MT_RET_OK;
//--- margin currency
   res=res && gateway_symbol->CurrencyMargin(exchange_symbol.currency_margin)==MT_RET_OK;
//--- digits
   res=res && gateway_symbol->Digits(exchange_symbol.digits)==MT_RET_OK;
//--- tick flags
   res=res && gateway_symbol->TickFlags(exchange_symbol.tick_flags)==MT_RET_OK;
//--- calc mode
   res=res && gateway_symbol->CalcMode(exchange_symbol.calc_mode)==MT_RET_OK;
//--- exec mode
   res=res && gateway_symbol->ExecMode(exchange_symbol.exec_mode)==MT_RET_OK;
//--- chart mode
   res=res && gateway_symbol->ChartMode(exchange_symbol.chart_mode)==MT_RET_OK;
//--- fill flags
   res=res && gateway_symbol->FillFlags(exchange_symbol.fill_flags)==MT_RET_OK;
//--- expiration flags
   res=res && gateway_symbol->ExpirFlags(exchange_symbol.expir_flags)==MT_RET_OK;
//--- tick value
   res=res && gateway_symbol->TickValue(exchange_symbol.tick_value)==MT_RET_OK;
//--- tick size
   res=res && gateway_symbol->TickSize(exchange_symbol.tick_size)==MT_RET_OK;
//--- contract size
   res=res && gateway_symbol->ContractSize(exchange_symbol.contract_size)==MT_RET_OK;
//--- min volume
   res=res && gateway_symbol->VolumeMin(SMTMath::VolumeToInt(exchange_symbol.volume_min))==MT_RET_OK;
//--- max volume
   res=res && gateway_symbol->VolumeMax(SMTMath::VolumeToInt(exchange_symbol.volume_max))==MT_RET_OK;
//--- step volume
   res=res && gateway_symbol->VolumeStep(SMTMath::VolumeToInt(exchange_symbol.volume_step))==MT_RET_OK;
//--- margin flags
   res=res && gateway_symbol->MarginFlags(exchange_symbol.margin_flags)==MT_RET_OK;
//--- market depth
   res=res && gateway_symbol->TickBookDepth(exchange_symbol.market_depth)==MT_RET_OK;
//--- margin initial
   if(exchange_symbol.margin_initial>0)
      res=res && gateway_symbol->MarginInitial(exchange_symbol.margin_initial)==MT_RET_OK;
//--- margin maintenance
   if(exchange_symbol.margin_maintenance>0)
      res=res && gateway_symbol->MarginMaintenance(exchange_symbol.margin_maintenance)==MT_RET_OK;
//--- long margin
   res=res && gateway_symbol->MarginLong(exchange_symbol.margin_long)==MT_RET_OK;
//--- short margin
   res=res && gateway_symbol->MarginShort(exchange_symbol.margin_short)==MT_RET_OK;
//--- margin limit
   if(exchange_symbol.margin_limit>0)
      res=res && gateway_symbol->MarginLimit(exchange_symbol.margin_limit)==MT_RET_OK;
//--- margin stop
   if(exchange_symbol.margin_stop>0)
      res=res && gateway_symbol->MarginStop(exchange_symbol.margin_stop)==MT_RET_OK;
//--- margin stop limit
   if(exchange_symbol.margin_stop_limit>0)
      res=res && gateway_symbol->MarginStopLimit(exchange_symbol.margin_stop_limit)==MT_RET_OK;
//--- settlement price
   if(exchange_symbol.settlement_price>0)
      res=res && gateway_symbol->PriceSettle(exchange_symbol.settlement_price)==MT_RET_OK;
//--- price limit max
   if(exchange_symbol.price_limit_max>0)
      res=res && gateway_symbol->PriceLimitMax(exchange_symbol.price_limit_max)==MT_RET_OK;
//--- price limit min
   if(exchange_symbol.price_limit_min>0)
      res=res && gateway_symbol->PriceLimitMin(exchange_symbol.price_limit_min)==MT_RET_OK;
//--- time start
   if(exchange_symbol.time_start!=0)
      res=res && gateway_symbol->TimeStart(exchange_symbol.time_start)==MT_RET_OK;
//--- time expiration
   if(exchange_symbol.time_expiration!=0)
      res=res && gateway_symbol->TimeExpiration(exchange_symbol.time_expiration)==MT_RET_OK;
//--- trade mode
   res=res && gateway_symbol->TradeMode(exchange_symbol.trade_mode)==MT_RET_OK;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Update external account for client                               |
//+------------------------------------------------------------------+
MTAPIRES CGateway::UpdateExternalAccount(const IMTRequest *request) const
  {
   MTAPIRES res=MT_RET_OK_NONE;
//--- check if external account already equals to MT login
   if(_wtoi64(request->ExternalAccount())==request->Login())
      return(MT_RET_OK_NONE);
//--- check API
   if(!m_api_gateway || !m_api_gateway_config)
      return(MT_RET_ERROR);
//--- format external account
   CMTStr32 external_account;
   external_account.Format(L"%I64u",request->Login());
//--- get user account interface
   IMTUser* user_gateway=m_api_gateway->UserCreate();
//--- check interface      
   if(!user_gateway)
      return(MT_RET_ERR_MEM);
//--- clear interface
   user_gateway->Clear();
//--- set MT login
   res=user_gateway->Login(request->Login());
//--- set external account
   if(res==MT_RET_OK)
      res=user_gateway->ExternalAccountAdd(m_api_gateway_config->ID(),external_account.Str());
//--- update user
   if(res==MT_RET_OK)
      res=m_api_gateway->GatewayAccountSet(0,user_gateway,NULL,NULL,NULL);
//--- release interface
   if(user_gateway)
      user_gateway->Release();
//--- log if success
   if(res==MT_RET_OK)
      ExtLogger.Out(MTLogOK,L"'%I64u': external account '%s' was set for dealer '%I64u'",request->Login(),external_account.Str(),m_api_gateway_config->ID());
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Convert orders to Gateway API format                             |
//+------------------------------------------------------------------+
MTAPIRES CGateway::ConvertOrders(const ExchangeOrdersArray &exchange_orders,IMTOrderArray *gateway_orders) const
  {
   IMTOrder      *gateway_order=NULL;
   ExchangeSymbol exchange_symbol;
   CMTStr32       str;
   MTAPIRES       res          =MT_RET_OK;
//--- check
   if(!m_api_gateway || !gateway_orders)
      return(MT_RET_ERR_PARAMS);
//--- go through all orders
   for(uint32_t i=0;i<exchange_orders.Total() && res==MT_RET_OK;i++)
     {
      //--- skip empty orders
      if(exchange_orders[i].volume==0)
         continue;
      //--- get Gateway API order interface
      if((gateway_order=m_api_gateway->OrderCreate())!=NULL)
        {
         //--- set order ticket
         res=gateway_order->OrderSet(exchange_orders[i].order_mt_id);
         //--- format order external id
         str.Format(L"%I64u",exchange_orders[i].order_exchange_id);
         //--- set order external id
         if(res==MT_RET_OK)
            res=gateway_order->ExternalID(str.Str());
         //--- set symbol of order
         if(res==MT_RET_OK)
            res=gateway_order->Symbol(exchange_orders[i].symbol);
         //--- set order volume
         if(res==MT_RET_OK)
            res=gateway_order->VolumeInitial(SMTMath::VolumeToInt((double)exchange_orders[i].volume));
         if(res==MT_RET_OK)
            res=gateway_order->VolumeCurrent(SMTMath::VolumeToInt((double)exchange_orders[i].volume));
         //--- set order prices
         if(res==MT_RET_OK)
            res=gateway_order->PriceOrder(exchange_orders[i].price_order);
         if(res==MT_RET_OK)
            res=gateway_order->PriceSL(exchange_orders[i].price_SL);
         if(res==MT_RET_OK)
            res=gateway_order->PriceTP(exchange_orders[i].price_TP);
         //--- set order type
         if(res==MT_RET_OK)
            res=gateway_order->Type(exchange_orders[i].type_order);
         //--- set order expiration type and time
         if(res==MT_RET_OK)
            res=gateway_order->TypeTime(exchange_orders[i].type_time);
         if(res==MT_RET_OK)
            res=gateway_order->TimeExpiration(exchange_orders[i].expiration_time);
         //--- set activation flags for exchange execution
         if(res==MT_RET_OK && m_api_exchange->SymbolGet(exchange_orders[i].symbol,exchange_symbol) && exchange_symbol.exec_mode==ExchangeSymbol::EXECUTION_EXCHANGE)
            res=gateway_order->ActivationFlags(IMTOrder::ACTIV_FLAGS_NO_EXPIRATION|IMTOrder::ACTIV_FLAGS_NO_LIMIT);
         //--- put login to comment
         str.Format(L"%I64u",exchange_orders[i].login);
         if(res==MT_RET_OK)
            res=gateway_order->Comment(str.Str());
         //--- set order setup time
         if(res==MT_RET_OK)
            res=gateway_order->TimeSetup(m_api_gateway->TimeCurrent());
         //--- add order to array
         if(res==MT_RET_OK)
            res=gateway_orders->Add(gateway_order);
         else
            gateway_order->Release();
        }
      else
         res=MT_RET_ERR_MEM;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Convert positions to Gateway API format                          |
//+------------------------------------------------------------------+
MTAPIRES CGateway::ConvertPositions(const ExchangePositionsArray &exchange_positions,IMTPositionArray *gateway_positions) const
  {
//--- check
   if(!m_api_gateway || !gateway_positions)
      return(MT_RET_ERR_PARAMS);
//--- go through all positions
   IMTPosition   *gateway_position=NULL;
   MTAPIRES       res             =MT_RET_OK;

   for(uint32_t i=0;i<exchange_positions.Total() && res==MT_RET_OK;i++)
     {
      //--- skip empty positions
      if(exchange_positions[i].volume==0)
         continue;
      //--- get Gateway API position interface
      if((gateway_position=m_api_gateway->PositionCreate())!=NULL)
        {
         //--- set position symbol
         if(res==MT_RET_OK)
            res=gateway_position->Symbol(exchange_positions[i].symbol);
         //--- set position volume
         if(res==MT_RET_OK)
            gateway_position->Volume(SMTMath::VolumeToInt((double)abs(exchange_positions[i].volume)));
         //--- set position action
         if(res==MT_RET_OK)
           {
            if(exchange_positions[i].volume>0)
               res=gateway_position->Action(IMTPosition::POSITION_BUY);
            else
               res=gateway_position->Action(IMTPosition::POSITION_SELL);
           }
         //--- set position open price
         if(res==MT_RET_OK)
            res=gateway_position->PriceOpen(exchange_positions[i].price);
         //--- write login to the comment
         if(res==MT_RET_OK)
           {
            CMTStr32 comment;
            comment.Format(L"%I64u",exchange_positions[i].login);
            res=gateway_position->Comment(comment.Str());
           }
         //--- set digits amount
         if(res==MT_RET_OK)
            res=gateway_position->Digits(exchange_positions[i].digits);
         //--- add position to array
         if(res==MT_RET_OK)
            res=gateway_positions->Add(gateway_position);
         else
            gateway_position->Release();
        }
      else
         res=MT_RET_ERR_MEM;
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
