//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeContext.h"
//+------------------------------------------------------------------+
//| 'this' : used in base member initializer list                    |
//+------------------------------------------------------------------+
#pragma warning(disable: 4355)
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeContext::CExchangeContext(CTradeDispatcher &trade_dispatcher)
   : m_socket(NULL),
     m_exchange_protocol(*this),
     m_port(0),
     m_buffer_receive(NULL),
     m_buffer_receive_max(0),
     m_thread_workflag(0),
     m_thread_checktime(_time64(nullptr)+THREAD_TIMEOUT),
     m_state(STATE_STOPPED),
     m_heartbeat_time(0),
     m_trade_dispatcher(trade_dispatcher)
  {
//--- clear strings
   m_address[0]=L'\0';
  }
#pragma warning(default: 4355)
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeContext::~CExchangeContext(void)
  {
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
//| Initialize gateway connection context                            |
//+------------------------------------------------------------------+
bool CExchangeContext::Initialize(LPCWSTR address)
  {
   int32_t pos=0;
//--- check
   if(!address)
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
      if((m_socket=new(std::nothrow)CExchangeSocket())==NULL)
        {
         ExtLogger.Out(MTLogErr,L"failed to create raw socket instance (%d)",::GetLastError());
         return(false);
        }
     }
//--- remember connection address
   CMTStr::Copy(m_address,_countof(m_address),address);
//--- remember connection port
   if((pos=CMTStr::FindChar(m_address,L':'))>0)
     {
      m_port=_wtoi(m_address+pos+1);
      m_address[pos]=L'\0';
     }
//--- initialize buffers of receiving/sending data packets before starting thread
   if(!InitializeBuffers())
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize context buffers");
      return(false);
     }
//--- update time before starting processing thread
   UpdateTime((int64_t)THREAD_TIMEOUT);
//--- set flag of thread operation
   InterlockedExchange(&m_thread_workflag,1);
//--- start thread of processing external connection data
   if(!m_thread.Start(ProcessThreadWrapper,this,STACK_SIZE_COMMON))
     {
      ExtLogger.Out(MTLogErr,L"failed to start context thread (%d)",::GetLastError());
      return(false);
     }
//--- update context state - processing thread started
   StateSet(STATE_STARTED);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CExchangeContext::Shutdown(void)
  {
//--- if connected, send logout
   if(StateGet()>STATE_CONNECTED)
     {
      //--- send logout message with waiting for complete send
      SendLogoutSync();
     }
//--- reset thread working flag
   InterlockedExchange(&m_thread_workflag,0);
//--- shutdown thread
   m_thread.Shutdown();
//--- shutdown protocol
   m_exchange_protocol.Shutdown();
//--- update context state
   StateSet(STATE_STOPPED);
//--- speed up shutdown
   UpdateTime((int64_t)0);
  }
//+------------------------------------------------------------------+
//| Set connection state                                             |
//+------------------------------------------------------------------+
uint32_t CExchangeContext::StateSet(uint32_t state)
  {
//--- return previous state of connection context
   return((uint32_t)InterlockedExchange((volatile LONG *)&m_state,state));
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
//--- in case of context not connected state will not checked
   if(StateGet()==STATE_STARTED)
      return(true);
//--- send HeartBeat
   res=SendHeartBeat(curtime);
//--- check time of context activity
   res=res && curtime<m_thread_checktime;
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Process login message                                            |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageLogin(const ExchangeMsgLogin &msg)
  {
//--- form response message, don't check login and password, 
//--- allow to connect using any login and password
   ExchangeMsgLogin msg_answer=msg;
   msg_answer.result          =MT_RET_OK;
//--- form response message buffer for sending
   if(!ExchangeMsgLogin::Write(msg_answer,*this))
     {
      ExtLogger.OutString(MTLogErr,L"failed to send login message answer");
      return(false);
     }
//--- write to log
   ExtLogger.Out(MTLogOK,L"user %s login successful",msg.login);
//--- send symbol settings
   if(!m_trade_dispatcher.SymbolsSend(*this))
      return(false);
//--- update context state - connection synchronized
   StateSet(STATE_SYNCHRONIZED);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Process logout message                                           |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageLogout(const ExchangeMsgLogout &msg)
  {
//--- send logout to gateway
   SendLogout();
//--- update context state
   StateSet(STATE_STARTED);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Process message with order                                       |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageOrder(const ExchangeMsgOrder &msg)
  {
//--- process order transaction
   return(m_trade_dispatcher.OrderAdd(msg.order));
  }
//+------------------------------------------------------------------+
//| Process message with account data request                        |
//+------------------------------------------------------------------+
bool CExchangeContext::OnMessageAccountDataRequest(const ExchangeMsgAccountDataRequest &msg)
  {
//--- process order transaction
   return(m_trade_dispatcher.AccountDataSend(*this,msg.login));
  }
//+------------------------------------------------------------------+
//| Create and send message with symbol settings                     |
//+------------------------------------------------------------------+
bool CExchangeContext::SendSymbol(const ExchangeSymbol &symbol,uint32_t index)
  {
   ExchangeMsgSymbol msg={0};
   bool              res=false;
//--- fill message header
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
   msg.header.type   =MSG_TYPE_SYMBOL;
//--- fill symbol
   msg.symbol        =symbol;
//--- fill symbol index
   msg.symbol.index  =index;
//--- form buffer for sending
   res=ExchangeMsgSymbol::Write(msg,*this);
//--- log result
   if(res)
      ExtLogger.Out(MTLogOK,L"symbol %s is sent",symbol.symbol);
   else
      ExtLogger.Out(MTLogErr,L"failed to send symbol %s",symbol.symbol);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Create and send message with tick                                |
//+------------------------------------------------------------------+
bool CExchangeContext::SendTick(const ExchangeTick &tick)
  {
   ExchangeMsgTick msg={0};
   bool            res=false;
//--- fill message header
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
   msg.header.type   =MSG_TYPE_TICK;
//--- fill tick
   msg.tick          =tick;
//--- form buffer for sending
   res=ExchangeMsgTick::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to send tick (symbol %s bid %.5lf ask %.5lf)",tick.symbol,tick.bid,tick.ask);
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
   msg.header.type=MSG_TYPE_ORDER;
//--- order data
   msg.order=order;
//--- form buffer for sending
   res=ExchangeMsgOrder::Write(msg,*this);
//--- log result
   if(res)
     {
      CMTStr128     order_desc;
      ExtLogger.Out(MTLogOK,L"'%I64u': order performed (%s)",order.login,ExchangeOrder::Print(order,order_desc));
     }
   else
      ExtLogger.Out(MTLogErr,L"failed to send order #%u",order.order_mt_id);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send Deal                                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::SendDeal(const ExchangeDeal &deal)
  {
   ExchangeMsgDeal msg={0};
   bool res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type=MSG_TYPE_DEAL;
//--- order data
   msg.deal=deal;
//--- form buffer for sending
   res=ExchangeMsgDeal::Write(msg,*this);
//--- log result
   if(res)
     {
      CMTStr128     deal_desc;
      ExtLogger.Out(MTLogOK,L"'%I64u': deal performed (%s)",deal.login,ExchangeDeal::Print(deal,deal_desc));
     }
   else
      ExtLogger.Out(MTLogErr,L"failed to send deal #%u",deal.order);
   return(res);
  }
//+------------------------------------------------------------------+
//| Send Book                                                        |
//+------------------------------------------------------------------+
bool CExchangeContext::SendBook(const ExchangeBook &book)
  {
   ExchangeMsgBook msg={0};
   bool res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type=MSG_TYPE_BOOK;
//--- order data
   msg.book=book;
//--- form buffer for sending
   res=ExchangeMsgBook::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.Out(MTLogErr,L"failed to send book (symbol %s)",book.symbol);
   return(res);
  }
//+------------------------------------------------------------------+
//| Send orders list                                                 |
//+------------------------------------------------------------------+
bool CExchangeContext::SendAccountData(const ExchangeAccountData &account_data)
  {
   ExchangeMsgAccountData msg={0};
   bool res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type=MSG_TYPE_ACCOUNT_DATA;
//--- account data
   msg.account_data=account_data;
//--- form buffer for sending
   res=ExchangeMsgAccountData::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.OutString(MTLogErr,L"failed to send account data");
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
   msg.header.type=MSG_TYPE_HEARTBEAT;
//--- form buffer for sending
   res=ExchangeMsgHeartBeat::Write(msg,*this);
//--- write to log in case of error
   if(!res)
      ExtLogger.OutString(MTLogErr,L"failed to send heartbeat");
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send logout                                                      |
//+------------------------------------------------------------------+
bool CExchangeContext::SendLogout(void)
  {
   ExchangeMsgLogout msg={0};
   bool              res=false;
//--- protocol version
   msg.header.version=EXCHANGE_PROTOCOL_VERSION;
//--- message type
   msg.header.type=MSG_TYPE_LOGOUT;
//--- form buffer for sending
   res=ExchangeMsgLogout::Write(msg,*this);
//--- write to log
   if(res)
      ExtLogger.OutString(MTLogOK,L"user logout successful");
   else
      ExtLogger.OutString(MTLogErr,L"failed to send logout");
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Send logout with waiting for complete send                       |
//+------------------------------------------------------------------+
bool CExchangeContext::SendLogoutSync(void)
  {
   int32_t  t  =0;
   bool res=false;
//--- send logout message
   res=SendLogout();
//--- waiting for complete send
   while(res && SendSizeGet())
     {
      //--- if logout wait time exceeded
      if((t+=LOGOUT_WAIT)>LOGOUT_TIME)
         break;
      //--- sleep
      Sleep(LOGOUT_WAIT);
     }
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Notify of client connection                                      |
//+------------------------------------------------------------------+
bool CExchangeContext::OnConnect(void)
  {
//--- shutdown protocol
   m_exchange_protocol.Shutdown();
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Receive data                                                     |
//+------------------------------------------------------------------+
bool CExchangeContext::OnReceive(const char *buffer,const uint32_t buffer_size)
  {
//--- check
   if(!buffer || buffer_size==0)
      return(false);
//--- notify protocol of data receipt
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
//--- flags of data processing
   bool data_send=false,data_receive=false;
//--- write to log
   ExtLogger.Out(MTLogOK,L"waiting for incoming connection on %s:%u",m_address,m_port);
//--- loop of managing gateway connection
   while(InterlockedExchangeAdd(&m_thread_workflag,0)>0)
     {
      //--- check gateway connection
      if(StateGet()<STATE_CONNECTED)
        {
         //--- accept incoming connection
         if(m_socket->ClientConnect(m_address,m_port))
           {
            //--- notify of connection
            OnConnect();
            //--- state = connected
            StateSet(STATE_CONNECTED);
           }
        }
      else
        {
         //--- receive sent data and process data for sending
         if(!ReceiveCheck(data_receive) || !SendCheck(data_send))
            StateSet(STATE_STARTED);
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
   m_socket->ClientClose();
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
   if(m_socket==NULL || !m_socket->ClientConnected())
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
      if(res=m_socket->ClientSend(m_buffer_send_tmp.Buffer(),m_buffer_send_tmp.Len()))
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
   bool res=false;
//--- reset flag of data processing
   data_processed=false;
//--- check socket state
   if(m_socket==NULL || !m_socket->ClientConnected() || m_buffer_receive==NULL)
      return(false);
//--- read
   if(res=m_socket->ClientRead(m_buffer_receive,m_buffer_receive_max,buffer_received))
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
//| Initialize buffers of data packets                               |
//+------------------------------------------------------------------+
bool CExchangeContext::InitializeBuffers(void)
  {
//--- create buffer to receive packets, if not already
   if(!m_buffer_receive)
     {
      //--- create packets
      m_buffer_receive=new(std::nothrow) char[MAX_BUFFER_RECEIVE];
      //--- check
      if(!m_buffer_receive)
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
//| Get send queue size                                              |
//+------------------------------------------------------------------+
uint32_t CExchangeContext::SendSizeGet(void)
  {
//--- lock outbound packets buffer
   m_buffer_send_sync.Lock();
//--- get size of send packets buffer
   uint32_t len=m_buffer_send.Len();
//--- unlock outbound packets buffer
   m_buffer_send_sync.Unlock();
//--- return result
   return(len);
  }
//+------------------------------------------------------------------+
