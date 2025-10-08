//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeContext.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeContext::CExchangeContext(CExchangeAPI &exchange_api): m_exchange_api(exchange_api),
     m_socket(NULL),
     m_port(0),
     m_buffer_receive(NULL),
     m_buffer_receive_max(0),
     m_exchange_protocol(*this),
     m_thread_workflag(0),
     m_thread_checktime(_time64(nullptr)+THREAD_TIMEOUT),
     m_state(STATE_STOPPED),
     m_logon(0),
     m_heartbeat_time(0)
  {
//--- clear strings
   m_address[0] =L'\0';
   m_login[0]   =L'\0';
   m_password[0]=L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeContext::~CExchangeContext(void)
  {
//--- shutdown
   Shutdown();
//--- delete packets buffer
   if(m_buffer_receive)
     {
      delete[] m_buffer_receive;
      m_buffer_receive    =NULL;
      m_buffer_receive_max=0;
     }
//--- delete socket
   if(m_socket)
     {
      delete m_socket;
      m_socket=NULL;
     }
  }
//+------------------------------------------------------------------+
//| Initialize context                                               |
//+------------------------------------------------------------------+
bool CExchangeContext::Initialize(LPCWSTR address,const uint32_t port,LPCWSTR login,LPCWSTR password)
  {
//--- check parameters
   if(!address || !login || !password || port==0)
      return(false);
//--- check if context is running, exit
   if(StateGet()!=STATE_STOPPED)
     {
      ExtLogger.Out(MTLogErr,L"failed to initialize context, context already initialized (%u)",StateGet());
      return(false);
     }
//--- check if socket object exists
   if(m_socket==NULL)
     {
      //--- create socket object if needed
      if((m_socket=new(std::nothrow)CExchangeSocket)==NULL)
        {
         ExtLogger.Out(MTLogErr,L"failed to create raw socket instance (%d)",::GetLastError());
         return(false);
        }
     }
//--- remember connection address
   CMTStr::Copy(m_address,_countof(m_address),address);
//--- remember connection port
   m_port=port;
//--- remember login and password
   CMTStr::Copy(m_login,_countof(m_login),login);
   CMTStr::Copy(m_password,_countof(m_password),password);
//--- initialize buffers of receiving/sending data packets before starting thread
   if(!InitializeBuffers())
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize context buffers");
      return(false);
     }
//--- update time before starting processing thread
   UpdateTime((int64_t)THREAD_TIMEOUT);
//--- set thread working flag
   InterlockedExchange(&m_thread_workflag,1);
//--- start thread of processing external connection data
   if(!m_thread.Start(ProcessThreadWrapper,this,STACK_SIZE_COMMON))
     {
      ExtLogger.Out(MTLogErr,L"failed to start context thread (%d)",::GetLastError());
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CExchangeContext::Shutdown(void)
  {
//--- if logged in, send logout message
   if(IsLogon())
      SendLogoutSync();
//--- reset flag of thread operation
   InterlockedExchange(&m_thread_workflag,0);
//--- shutdown thread
   m_thread.Shutdown();
//--- update context state
   StateSet(STATE_STOPPED);
//--- shutdown protocol
   m_exchange_protocol.Shutdown();
//--- lock
   m_trans_sync.Lock();
//--- clear all transaction arrays
   m_trans_symbols.Clear();
   m_trans_symbols_tmp.Clear();
   m_trans_ticks.Clear();
   m_trans_ticks_tmp.Clear();
   m_trans_orders.Clear();
   m_trans_orders_tmp.Clear();
//--- unlock
   m_trans_sync.Unlock();
  }
//+------------------------------------------------------------------+
//| Set connection state                                             |
//+------------------------------------------------------------------+
uint32_t CExchangeContext::StateSet(uint32_t state)
  {
   uint32_t old_state=(uint32_t)InterlockedExchange((volatile LONG *)&m_state,state);
//--- notify after changing state
   if(old_state!=state)
      m_exchange_api.OnContextStateUpdate(state);
//--- return previous state of connection context
   return(old_state);
  }
//+------------------------------------------------------------------+
//| Get connection state                                             |
//+------------------------------------------------------------------+
uint32_t CExchangeContext::StateGet(void)
  {
//--- return state of connection context
   return((uint32_t)InterlockedExchangeAdd((volatile LONG *)&m_state,0));
  }
//+------------------------------------------------------------------+
//| Send data                                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::Send(const char *buffer,const uint32_t buffer_size)
  {
   bool res=true;
//--- check
   if(!buffer || buffer_size==0 || buffer_size>MAX_BUFFER_SEND)
      return(false);
//--- check connection state
   if(StateGet()==STATE_STOPPED)
      return(false);
//--- lock outbound packets buffer
   m_buffer_send_sync.Lock();
//--- check size of send packets buffer
   if(m_buffer_send.Len()>MAX_BUFFER_SEND)
     {
      ExtLogger.Out(MTLogErr,L"failed to add data to send buffer, send buffer overflow (%u)",buffer_size);
      res=false;
     }
//--- add data for sending
   res=res && m_buffer_send.Add(buffer,buffer_size);
//--- unlock outbound packets buffer
   m_buffer_send_sync.Unlock();
//--- store last heartbeat sending time
   m_heartbeat_time=_time64(nullptr);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Check context                                                    |
//+------------------------------------------------------------------+
bool CExchangeContext::Check(const int64_t curtime)
  {
   bool res=false;
//--- send heartbeat
   res=SendHeartBeat(curtime);
//--- check time of context activity
   res=res && curtime<m_thread_checktime;
//--- successful
   return(res);
  }
//+------------------------------------------------------------------+
//| Process received transactions                                    |
//+------------------------------------------------------------------+
bool CExchangeContext::TransApply(CExchangeSymbols &symbols,bool &trans_applied)
  {
   bool res=false;
//--- results of processing transactions
   bool symbols_applied=false,ticks_applied=false,books_applied=false,orders_applied=false,deals_applied=false;
//--- clear arrays
   m_trans_symbols_tmp.Clear();
   m_trans_ticks_tmp.Clear();
   m_trans_books_tmp.Clear();
   m_trans_orders_tmp.Clear();
   m_trans_deals_tmp.Clear();
//--- lock
   m_trans_sync.Lock();
//--- get contents of arrays
   m_trans_symbols.Swap(m_trans_symbols_tmp);
   m_trans_ticks.Swap(m_trans_ticks_tmp);
   m_trans_books.Swap(m_trans_books_tmp);
   m_trans_orders.Swap(m_trans_orders_tmp);
   m_trans_deals.Swap(m_trans_deals_tmp);
//--- unlock
   m_trans_sync.Unlock();
//--- process received transactions
   res=TransApplySymbols(symbols,m_trans_symbols_tmp,symbols_applied);
   res=res && TransApplyTicks(symbols,m_trans_ticks_tmp,ticks_applied);
   res=res && TransApplyBooks(symbols,m_trans_books_tmp,books_applied);
   res=res && TransApplyOrders(symbols,m_trans_orders_tmp,orders_applied);
   res=res && TransApplyDeals(symbols,m_trans_deals_tmp,deals_applied);
//--- flag of data availability
   trans_applied=symbols_applied || ticks_applied || books_applied || orders_applied || deals_applied;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process symbol transactions                                      |
//+------------------------------------------------------------------+
bool CExchangeContext::TransApplySymbols(CExchangeSymbols &symbols_base,ExchangeSymbolsArray &trans_symbols,bool &trans_applied)
  {
   bool res=true;
//--- iterate over all symbol transactions
   for(uint32_t i=0;i<trans_symbols.Total();i++)
     {
      //--- process transactions
      if(symbols_base.OnSymbolAdd(trans_symbols[i]))
        {
         //--- transaction accepted
         trans_applied=true;
         //--- imported last symbol - update state
         if(trans_symbols[i].index==0)
            StateSet(STATE_SYNCHRONIZED);
        }
      else
        {
         //--- notify of error
         ExtLogger.Out(MTLogErr,L"failed to apply symbol transaction (symbol '%s')",trans_symbols[i].symbol);
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process tick transactions                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::TransApplyTicks(CExchangeSymbols &symbols_base,ExchangeTicksArray &trans_ticks,bool &trans_applied)
  {
   bool res=true;
//--- iterate over all tick transactions
   for(uint32_t i=0;i<trans_ticks.Total();i++)
     {
      //--- process transactions
      if(symbols_base.OnSymbolTickApply(trans_ticks[i]))
        {
         //--- transaction accepted
         trans_applied=true;
        }
      else
        {
         //--- notify of error
         ExtLogger.Out(MTLogErr,L"failed to apply tick transaction (symbol %s)",trans_ticks[i].symbol);
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process book transactions                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::TransApplyBooks(CExchangeSymbols &symbols_base,ExchangeBooksArray &trans_books,bool &trans_applied)
  {
   bool res=true;
//--- iterate over all order transactions
   for(uint32_t i=0;i<trans_books.Total();i++)
     {
      //--- process transactions
      if(symbols_base.OnSymbolBookApply(trans_books[i]))
        {
         //--- transaction accepted
         trans_applied=true;
        }
      else
        {
         //--- notify of error
         ExtLogger.Out(MTLogErr,L"failed to apply book transaction (symbol %s)",trans_books[i].symbol);
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process order transactions                                       |
//+------------------------------------------------------------------+
bool CExchangeContext::TransApplyOrders(CExchangeSymbols &symbols_base,ExchangeOrdersArray &trans_orders,bool &trans_applied)
  {
   bool res=true;
//--- iterate over all order transactions
   for(uint32_t i=0;i<trans_orders.Total();i++)
     {
      //--- process transactions
      if(symbols_base.OnSymbolOrderApply(trans_orders[i]))
        {
         //--- transaction accepted
         trans_applied=true;
        }
      else
        {
         //--- notify of error
         ExtLogger.Out(MTLogErr,L"failed to apply order transaction (order #%I64u)",trans_orders[i].order_mt_id);
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process deal transactions                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::TransApplyDeals(CExchangeSymbols &symbols_base,ExchangeDealsArray &trans_deals,bool &trans_applied)
  {
   bool res=true;
//--- iterate over all order transactions
   for(uint32_t i=0;i<trans_deals.Total();i++)
     {
      //--- process transactions
      if(symbols_base.OnSymbolDealApply(trans_deals[i]))
        {
         //--- transaction accepted
         trans_applied=true;
        }
      else
        {
         //--- notify of error
         ExtLogger.Out(MTLogErr,L"failed to apply deal transaction (order #%I64u)",trans_deals[i].order);
         res=false;
         break;
        }
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process login message                                            |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageLogin(const ExchangeMsgLogin &msg)
  {
//--- write result of message processing to log
   if(msg.result==MT_RET_OK)
     {
      //--- set logon flag
      LogonSet(true);
      //--- write to log
      ExtLogger.OutString(MTLogOK,L"login successful");
     }
   else
      ExtLogger.OutString(MTLogOK,L"failed to login to exchange, invalid account");
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Process logout message                                           |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageLogout(const ExchangeMsgLogout &msg)
  {
//--- set logon flag off
   LogonSet(false);
//--- set exchange state
   StateSet(STATE_STOPPED);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Process message with symbol settings                             |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageSymbol(const ExchangeMsgSymbol &msg)
  {
   bool res=false;
//--- lock
   m_trans_sync.Lock();
//--- add symbol transaction for processing
   res=m_trans_symbols.Add(&msg.symbol);
//--- unlock
   m_trans_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process tick message                                             |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageTick(const ExchangeMsgTick &msg)
  {
   bool res=false;
//--- lock
   m_trans_sync.Lock();
//--- add tick transaction for processing
   res=m_trans_ticks.Add(&msg.tick);
//--- unlock
   m_trans_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process book message                                            |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageBook(const ExchangeMsgBook &msg)
  {
   bool res=false;
//--- lock
   m_trans_sync.Lock();
//--- add order transaction for processing
   res=m_trans_books.Add(&msg.book);
//--- unlock
   m_trans_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process order message                                            |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageOrder(const ExchangeMsgOrder &msg)
  {
   bool res=false;
//--- lock
   m_trans_sync.Lock();
//--- add order transaction for processing
   res=m_trans_orders.Add(&msg.order);
//--- unlock
   m_trans_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process deal message                                            |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageDeal(const ExchangeMsgDeal &msg)
  {
   bool res=false;
//--- lock
   m_trans_sync.Lock();
//--- add order transaction for processing
   res=m_trans_deals.Add(&msg.deal);
//--- unlock
   m_trans_sync.Unlock();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process account data                                             |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageAccountData(const ExchangeMsgAccountData &msg)
  {
   bool res;
//--- notify exchange api
   res=m_exchange_api.OnAccountData(msg.account_data);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process HeartBeat message                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageHeartBeat(const ExchangeMsgHeartBeat &msg)
  {
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Send login                                                       |
//+------------------------------------------------------------------+
bool CExchangeContext::SendLogin(void)
  {
   ExchangeMsgLogin msg={0};
   bool             res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type   =MSG_TYPE_LOGIN;
//--- result
   msg.result        =MT_RET_OK;
//--- login
   CMTStr::Copy(msg.login,_countof(msg.login),m_login);
//--- password
   CMTStr::Copy(msg.password,_countof(msg.password),m_password);
//--- form buffer for sending
   res=ExchangeMsgLogin::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.OutString(MTLogErr,L"failed to send login message");
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send logout                                                      |
//+------------------------------------------------------------------+
bool CExchangeContext::SendLogoutSync(void)
  {
   ExchangeMsgLogout msg={0};
   bool              res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type   =MSG_TYPE_LOGOUT;
//--- form buffer for sending
   res=ExchangeMsgLogout::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.OutString(MTLogErr,L"failed to send logout message");
//--- send this command synchronously
   int32_t t=0;
   while(res && IsLogon())
     {
      //--- if logout wait time exceeded
      if((t+=LOGOUT_WAIT)>LOGOUT_TIME)
         break;
      //--- sleep
      Sleep(LOGOUT_WAIT);
     }
//--- clear logon state
   LogonSet(false);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send trade order                                                 |
//+------------------------------------------------------------------+
bool CExchangeContext::SendOrder(const ExchangeOrder &order)
  {
   ExchangeMsgOrder msg={0};
   bool             res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type   =MSG_TYPE_ORDER;
//--- order data
   msg.order=order;
//--- form buffer for sending
   res=ExchangeMsgOrder::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to send order #%I64u",order.order_mt_id);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send request for orders                                          |
//+------------------------------------------------------------------+
bool CExchangeContext::SendAccountDataRequest(uint64_t login)
  {
   ExchangeMsgAccountDataRequest msg={0};
   bool                          res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type   =MSG_TYPE_ACCOUNT_DATA_REQUEST;
//--- login
   msg.login=login;
//--- form buffer for sending
   res=ExchangeMsgAccountDataRequest::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to send account data request for login '%I64u'",login);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send HeartBeat                                                   |
//+------------------------------------------------------------------+
bool CExchangeContext::SendHeartBeat(const int64_t curtime)
  {
   ExchangeMsgHeartBeat msg={0};
   bool                 res=false;
//--- check heartbeat time
   if(m_heartbeat_time+HEARBEAT_TIMEOUT>curtime)
      return(true);
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type   =MSG_TYPE_HEARTBEAT;
//--- form buffer for sending
   res=ExchangeMsgHeartBeat::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.OutString(MTLogErr,L"failed to send heartbeat");
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Initialize buffers of data packets                               |
//+------------------------------------------------------------------+
bool CExchangeContext::InitializeBuffers(void)
  {
//--- create buffer to receive packets, if not already
   if(m_buffer_receive==NULL)
     {
      //--- create buffer to receive packets
      m_buffer_receive=new(std::nothrow) char[MAX_BUFFER_RECEIVE];
      //--- check
      if(m_buffer_receive==NULL)
         return(false);
      //--- remember size
      m_buffer_receive_max=MAX_BUFFER_RECEIVE;
     }
//--- clear buffer to receive packets
   ZeroMemory(m_buffer_receive,m_buffer_receive_max);
//--- lock buffer to send packets
   m_buffer_send_sync.Lock();
//--- clear buffer to send packets
   m_buffer_send.Clear();
   m_buffer_send_tmp.Clear();
//--- unlock
   m_buffer_send_sync.Unlock();
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Notify of connection                                             |
//+------------------------------------------------------------------+
bool CExchangeContext::OnConnect(void)
  {
//--- send login message
   return(SendLogin());
  }
//+------------------------------------------------------------------+
//| Receive data                                                     |
//+------------------------------------------------------------------+
bool CExchangeContext::OnReceive(char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- pass received data to protocol
   return(m_exchange_protocol.OnReceive(buffer,buffer_size));
  }
//+------------------------------------------------------------------+
//| Start thread of connection processing                            |
//+------------------------------------------------------------------+
uint32_t __stdcall CExchangeContext::ProcessThreadWrapper(LPVOID param)
  {
//--- check
   CExchangeContext *pThis=reinterpret_cast<CExchangeContext*>(param);
   if(pThis)
      pThis->ProcessThread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Process messages                                                 |
//+------------------------------------------------------------------+
__declspec(noinline) void CExchangeContext::ProcessThread(void)
  {
//--- check
   if(m_socket==NULL)
     {
      ExtLogger.OutString(MTLogErr,L"failed to start process thread, invalid socket");
      return;
     }
//--- update context state - processing thread started
   StateSet(STATE_STARTED);
//--- write result to log
   ExtLogger.Out(MTLogOK,L"connecting to exchange server (%s:%u)",m_address,m_port);
//--- initialize connection
   if(!m_socket->Connect(m_address,m_port))
     {
      //--- notify of unsuccessful connection attempt
      ExtLogger.Out(MTLogErr,L"failed to connect to %s:%u (%s)",m_address,m_port,m_socket->ErrorGet());
      //--- update context state - processing thread stopped
      StateSet(STATE_STOPPED);
      //--- speed up shutdown
      UpdateTime((int64_t)0);
      return;
     }
//--- write to log about server connection
   ExtLogger.Out(MTLogOK,L"connected to %s:%u",m_address,m_port);
//--- call connection handler
   if(!OnConnect())
     {
      //--- update context state - processing thread stopped
      StateSet(STATE_STOPPED);
      //--- close socket
      m_socket->Close();
      //--- speed up shutdown
      UpdateTime((int64_t)0);
      return;
     }
//--- flags of data processing
   bool data_send=false,data_receive=false;
//--- loop of managing external connection
   while(InterlockedExchangeAdd(&m_thread_workflag,0)>0)
     {
      //--- receive sent data
      if(!ReceiveCheck(data_receive))
        {
         StateSet(STATE_STOPPED);
         break;
        }
      //--- process data for sending
      if(!SendCheck(data_send))
        {
         StateSet(STATE_STOPPED);
         break;
        }
      //--- update time of context activity
      UpdateTime((int64_t)THREAD_TIMEOUT);
      //--- if there was no data exchange, then sleep
      if(!data_send && !data_receive)
         Sleep(THREAD_SLEEP);
     }
//--- update context state - processing thread stopped
   StateSet(STATE_STOPPED);
//--- close socket
   m_socket->Close();
//--- speed up shutdown
   UpdateTime((int64_t)0);
  }
//+------------------------------------------------------------------+
//| Send data                                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::SendCheck(bool &data_processed)
  {
   bool res=false;
//--- reset flag of data processing
   data_processed=false;
//--- check socket state
   if(m_socket==NULL || !m_socket->Connected())
      return(false);
//--- clear temporary buffer for packets to be send
   m_buffer_send_tmp.Clear();
//--- lock
   m_buffer_send_sync.Lock();
//--- get packets buffer, leave empty buffer instead
   m_buffer_send.Swap(m_buffer_send_tmp);
//--- unlock
   m_buffer_send_sync.Unlock();
//--- if we have data, send them
   if(m_buffer_send_tmp.Len()>0)
     {
      //--- if data are already sent, set flag of data processing
      if(res=m_socket->Send(m_buffer_send_tmp.Buffer(),m_buffer_send_tmp.Len()))
        {
         //--- set flag
         data_processed=true;
        }
      else
        {
         //--- write to log
         ExtLogger.Out(MTLogErr,L"failed to write data to socket %s:%u (%s)",m_address,m_port,m_socket->ErrorGet());
        }
     }
   else
      res=true;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Receive data                                                     |
//+------------------------------------------------------------------+
bool CExchangeContext::ReceiveCheck(bool &data_processed)
  {
   uint32_t buffer_received=0;
   bool res            =false;
//--- reset flag of data processing
   data_processed=false;
//--- check socket state
   if(m_socket==NULL || !m_socket->Connected() || m_buffer_receive==NULL)
      return(false);
//--- read
   if(res=m_socket->Read(m_buffer_receive,m_buffer_receive_max,buffer_received))
     {
      //--- notify after reading data
      if(buffer_received>0)
        {
         //--- if notified, set flag of data processing
         if(res=OnReceive(m_buffer_receive,buffer_received))
            data_processed=true;
        }
     }
   else
     {
      //--- write to log
      ExtLogger.Out(MTLogErr,L"failed to read data from socket %s:%u (%s)",m_address,m_port,m_socket->ErrorGet());
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
