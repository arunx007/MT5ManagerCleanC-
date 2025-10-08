//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "ExchangeAPI.h"
#include "..\Bases\Gateway.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CExchangeAPI::CExchangeAPI(CGateway *gateway): m_gateway(gateway),m_symbols(gateway),m_exchange_context(*this),m_thread_workflag(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CExchangeAPI::~CExchangeAPI(void)
  {
//--- clear pointer to gateway API interface
   m_gateway=NULL;
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CExchangeAPI::Initialize(LPCWSTR server,LPCWSTR login,LPCWSTR password)
  {
   CMTStrPath str_server;
   int32_t        pos =0;
   uint32_t   port=0;
//--- check
   if(!server || !login || !password)
      return(false);
//--- shutdown
   Shutdown();
//--- get address/port of context (server:port)
   str_server.Assign(server);
   if((pos=str_server.FindChar(L':'))>0)
     {
      port=uint32_t(_wtoi(str_server.Str()+pos+1));
      str_server.Trim(pos);
     }
//--- default value for port
   if(port==0)
      port=DEFAULT_EXCHANGE_PORT;
//--- default value for address
   if(str_server.Empty())
      str_server.Assign(DEFAULT_EXCHANGE_ADDRESS);
//--- initialize trade connection context
   if(!m_exchange_context.Initialize(str_server.Str(),port,login,password))
      return(false);
//--- set thread's working flag
   InterlockedExchange(&m_thread_workflag,1);
//--- start thread of processing data
   if(!m_thread.Start(ProcessThreadWrapper,this,STACK_SIZE_COMMON))
     {
      ExtLogger.OutString(MTLogErr,L"failed to start exchange API work thread");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CExchangeAPI::Shutdown(void)
  {
//--- reset thread's working flag
   InterlockedExchange(&m_thread_workflag,0);
//--- shutdown thread
   m_thread.Shutdown();
//--- shutdown external connection context
   m_exchange_context.Shutdown();
  }
//+------------------------------------------------------------------+
//| Notify of context state update                                   |
//+------------------------------------------------------------------+
void CExchangeAPI::OnContextStateUpdate(const uint32_t state)
  {
//--- check
   if(!m_gateway)
      return;
//--- notify gateway, if context is in the data processing state
   if(m_exchange_context.StateGet()==CExchangeContext::STATE_SYNCHRONIZED)
     {
      //--- notify gateway
      m_gateway->OnExchangeSyncComplete();
     }
//--- notify gateway, if context is stopped
   if(state==CExchangeContext::STATE_STOPPED)
     {
      //--- notify gateway
      m_gateway->OnExchangeConnectClosed();
     }
  }
//+------------------------------------------------------------------+
//| Notify of account data                                           |
//+------------------------------------------------------------------+
bool CExchangeAPI::OnAccountData(const ExchangeAccountData &account_data)
  {
//--- check
   if(!m_gateway)
      return(false);
//--- notify gateway
   return(m_gateway->OnExchangeAccountDataReceived(account_data));
  }
//+------------------------------------------------------------------+
//| Check external trading system API                                |
//+------------------------------------------------------------------+
bool CExchangeAPI::Check(void)
  {
   int64_t ctm=_time64(nullptr);
//--- check context
   return(m_exchange_context.Check(ctm));
  }
//+------------------------------------------------------------------+
//| Send order                                                       |
//+------------------------------------------------------------------+
bool CExchangeAPI::SendOrder(const ExchangeOrder &order)
  {
//--- pass the order to context
   return(m_exchange_context.SendOrder(order));
  }
//+------------------------------------------------------------------+
//| Send request for orders                                          |
//+------------------------------------------------------------------+
bool CExchangeAPI::SendAccountDataRequest(uint64_t login)
  {
//--- pass the order to context
   return(m_exchange_context.SendAccountDataRequest(login));
  }
//+------------------------------------------------------------------+
//| Find symbol by name                                              |
//+------------------------------------------------------------------+
bool CExchangeAPI::SymbolGet(LPCWSTR symbol_name,ExchangeSymbol &symbol)
  {
//--- find symbol
   return(m_symbols.SymbolGet(symbol_name,symbol));
  }
//+------------------------------------------------------------------+
//| Start thread of data processing                                  |
//+------------------------------------------------------------------+
uint32_t __stdcall CExchangeAPI::ProcessThreadWrapper(LPVOID param)
  {
//--- process work thread
   CExchangeAPI *pThis=reinterpret_cast<CExchangeAPI*>(param);
   if(pThis)
      pThis->ProcessThread();
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Exchange API work thread                                         |
//+------------------------------------------------------------------+
__declspec(noinline) void CExchangeAPI::ProcessThread(void)
  {
   bool trans_applied=false;
//--- loop of data processing
   while(InterlockedExchangeAdd(&m_thread_workflag,0)>0)
     {
      //--- process received data
      m_exchange_context.TransApply(m_symbols,trans_applied);
      //--- sleep if there are no transactions
      if(!trans_applied)
         Sleep(TRANS_WAIT_TIME);
     }
  }
//+------------------------------------------------------------------+
