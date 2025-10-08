//+------------------------------------------------------------------+
//|                                           MetaTrader 5 UniFeeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DataSource.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDataSource::CDataSource() : m_gateway(NULL),m_workflag(0),m_initalized(0),m_state(STATE_DISCONNECTED),
                             m_unisocket(NULL),m_symbols(NULL)
  {
//--- null the strings
   m_address[0] ='\0';
   m_login[0]   ='\0';
   m_password[0]='\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDataSource::~CDataSource(void)
  {
//--- close all
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Thread of the external connection handling                       |
//+------------------------------------------------------------------+
bool CDataSource::Start(IMTGatewayAPI *gateway)
  {
//--- checking
   if(!gateway)
     {
      ExtLogger.OutString(MTLogErr,L"invalid datafeed parameters");
      return(false);
     }
//--- remember the pointer to the gateway interface
   m_gateway=gateway;
//--- set the thread operation flag
   InterlockedExchange(&m_workflag,1);
//--- start the thread of external connection data processing
   if(!m_thread.Start(ProcessThreadWrapper,this,STACK_SIZE_COMMON))
     {
      ExtLogger.OutString(MTLogErr,L"failed to start remote server connection thread");
      m_gateway=NULL;
      return(false);
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Stop the handling thread                                         |
//+------------------------------------------------------------------+
bool CDataSource::Shutdown(void)
  {
//--- close the socket
   if(m_unisocket)
      m_unisocket->Close();
//--- reset the initialization sign
   InterlockedExchange(&m_initalized,0);
//--- set the thread operation completion sign
   InterlockedExchange(&m_workflag,0);
//--- complete the operation of the quote feed thread
   if(m_thread.Handle())
      if(!m_thread.Shutdown(THREAD_TIMEOUT))
         m_thread.Terminate();
//--- delete CUniFeederSocket object if it exist
   if(m_unisocket)
     {
      delete m_unisocket;
      m_unisocket=NULL;
     }
//--- reset the pointer to the gateway interface
   m_gateway=NULL;
//--- delete the buffer of symbols
   if(m_symbols)
     {
      delete m_symbols;
      m_symbols=NULL;
     }
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Initialization of the quote feed                                 |
//+------------------------------------------------------------------+
bool CDataSource::Init(const IMTConFeeder *config)
  {
   char str[MAX_PATH]={0};
   uint32_t size=0;
//--- checking
   if(!config)
     {
      ExtLogger.OutString(MTLogErr,L"invalid datafeed config interface");
      return(false);
     }
//--- check, if already initialized, quit
   if(InterlockedExchangeAdd(&m_initalized,0))
      return(true);
//--- remember settings
   _snprintf_s(m_address, _countof(m_address), _TRUNCATE,"%S",config->FeedServer());
   _snprintf_s(m_login,   _countof(m_login),   _TRUNCATE,"%S",config->FeedLogin());
   _snprintf_s(m_password,_countof(m_password),_TRUNCATE,"%S",config->FeedPassword());
//--- remember symbols, free the old buffer
   if(m_symbols)
     {
      delete[] m_symbols;
      m_symbols=NULL;
     }
//--- calculate symbol buffer size
   for(uint32_t i=0;i<config->SymbolTotal();i++)
      size+=(uint32_t)(::WideCharToMultiByte(CP_ACP,0,config->SymbolNext(i),-1,NULL,0,NULL,NULL)+1);
//--- allocate a new one
   if((m_symbols=new char[size+1])==NULL)
     {
      ExtLogger.Out(MTLogOK,L"no enough memory for symbols buffer(%d bytes)",size);
      return(false);
     }
//--- null the string
   m_symbols[0]='\0';
//--- go through all parameters
   for(uint32_t i=0;i<config->SymbolTotal();i++)
     {
      //--- convert Unicode to ANSI and append to string
      if((::WideCharToMultiByte(CP_ACP,0,config->SymbolNext(i),-1,str,_countof(str),NULL,NULL))!=0)
        {
         strncat_s(m_symbols,size,str,_TRUNCATE);
         strncat_s(m_symbols,size,";",_TRUNCATE);
        }
     }
//--- null the end of the string
   m_symbols[size]=0;
//--- record to journal
   ExtLogger.OutString(MTLogOK,L"datafeed initialized");
//--- refresh the state
   StateSet(STATE_CONNECTSTART);
//--- set the feeder initialization sign
   InterlockedExchange(&m_initalized,1);
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Checking timeouts                                                |
//+------------------------------------------------------------------+
bool CDataSource::Check(void)
  {
//--- if not initialized, quit
   if(!InterlockedExchangeAdd(&m_initalized,0))
      return(true);
//--- check the state
   if(StateGet()==STATE_DISCONNECTED)
      return(false);
//--- check the socket
   return(m_unisocket && m_unisocket->Check());
  }
//+------------------------------------------------------------------+
//| Getting state                                                    |
//+------------------------------------------------------------------+
LONG CDataSource::StateGet(void)
  {
   return(InterlockedExchangeAdd(&m_state,0));
  }
//+------------------------------------------------------------------+
//| Setting state                                                    |
//+------------------------------------------------------------------+
LONG CDataSource::StateSet(LONG state)
  {
//--- notify Gateway API about connect state changing
   if(m_gateway)
      m_gateway->StateConnect(state==STATE_CONNECTED);
//--- update state
   return(InterlockedExchange(&m_state,state));
  }
//+------------------------------------------------------------------+
//| Count traffic                                                    |
//+------------------------------------------------------------------+
void CDataSource::StateTraffic(uint32_t received_bytes,uint32_t sent_bytes)
  {
//--- count traffic using Gateway API
   if(m_gateway)
      m_gateway->StateTraffic(received_bytes,sent_bytes);
  }
//+------------------------------------------------------------------+
//| Starting the thread handling data of the external connection     |
//+------------------------------------------------------------------+
uint32_t __stdcall CDataSource::ProcessThreadWrapper(LPVOID param)
  {
//--- checking
   CDataSource *pThis=reinterpret_cast<CDataSource*>(param);
   if(pThis)
      pThis->ProcessThread();
//--- ok
   return(0);
  }
//+------------------------------------------------------------------+
//| Processing data of the external connection                       |
//+------------------------------------------------------------------+
__declspec(noinline) void CDataSource::ProcessThread(void)
  {
//--- external connection management loop
   while(InterlockedExchangeAdd(&m_workflag,0)>0)
     {
      //--- analyze the state, start connection, connect
      if(StateGet()==STATE_CONNECTSTART)
         ProcessConnect();
      //--- connected, receive data
      if(StateGet()==STATE_CONNECTED)
         ProcessData();
      //--- sleep
      Sleep(100);
     }
  }
//+------------------------------------------------------------------+
//| Connection                                                       |
//+------------------------------------------------------------------+
bool CDataSource::ProcessConnect(void)
  {
   bool res=false;
//--- create CUniFeederSocket object if it does not exist
   if(!m_unisocket && (m_unisocket=new CUniFeederSocket(*this))==NULL)
      return(false);
//--- connect to the UniDDE server
   res=m_unisocket->Connect(m_address,m_login,m_password,m_symbols);
//--- write the result to the journal and update the state
   if(res)
     {
      ExtLogger.Out(MTLogOK,L"datafeed connected to '%S' with login '%S'",m_address,m_login);
      StateSet(STATE_CONNECTED);
     }
   else
     {
      ExtLogger.OutString(MTLogOK,L"datafeed connect failed");
      StateSet(STATE_DISCONNECTED);
     }
//--- return the result
   return(true);
  }
//+------------------------------------------------------------------+
//| Receiving quotes                                                 |
//+------------------------------------------------------------------+
bool CDataSource::ProcessData(void)
  {
   MTAPIRES res=MT_RET_OK;
//--- buffer prepare
   m_ticks_buffer.Clear();
//--- receive ticks from data source
   if(!m_unisocket || !m_unisocket->ReadTicks(m_ticks_buffer))
      res=MT_RET_ERROR;
//--- send ticks to MT5
   for(uint32_t i=0;res==MT_RET_OK && i<m_ticks_buffer.Total();i++)
     {
      if(m_gateway)
        {
         res=m_gateway->SendTicks(&m_ticks_buffer[i],1);
         if(res!=MT_RET_OK)
            ExtLogger.Out(MTLogErr,L"failed to send ticks, symbol %s, error %d",m_ticks_buffer[i].symbol,res);
        }
     }
//--- if error, refresh the state
   if(res!=MT_RET_OK)
      StateSet(STATE_DISCONNECTED);
//--- return the result
   return(res==MT_RET_OK);
  }
//+------------------------------------------------------------------+
