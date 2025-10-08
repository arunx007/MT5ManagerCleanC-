//+------------------------------------------------------------------+
//|                                     MetaTrader 5 Sample Exchange |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MTExchangeApp.h"
#include "ExchangeAPI\Common\ExchangeProtocolData.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMTExchangeApp::CMTExchangeApp(void): m_context(m_trade_dispatcher)
  {
//--- clear strings
   m_address[0]='\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMTExchangeApp::~CMTExchangeApp(void)
  {
  }
//+------------------------------------------------------------------+
//| Initialize application                                           |
//+------------------------------------------------------------------+
bool CMTExchangeApp::Initialize(int32_t argc,wchar_t **argv)
  {
//--- check command line arguments
   if(!argc || !argv)
      return(false);
//--- initialize logger
   if(!ExtLogger.Initialize())
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize logger");
      return(false);
     }
//--- print banner
   ExtLogger.OutString(MTLogOK,L"");
   ExtLogger.Out(MTLogOK,L"%s %d.%02d build %d, %s",ProgramName,ProgramVersion/100,ProgramVersion%100,ProgramBuild,ProgramBuildDate);
   ExtLogger.OutString(MTLogOK,L"Copyright 2000-2025, MetaQuotes Ltd.");
   ExtLogger.OutString(MTLogOK,L"This is a simple example of external trading system for working with SampleGateway application.");
   ExtLogger.OutString(MTLogOK,L"To specify binding address start application using the /address:address:port command line argument.");
   ExtLogger.Out(MTLogOK,L"Default binding address is %s:%u.",DEFAULT_EXCHANGE_ADDRESS,DEFAULT_EXCHANGE_PORT);
   ExtLogger.OutString(MTLogOK,L"To exit from application press <esc> key.");
//--- initialize sockets
   WSADATA wsa={0};
   if(WSAStartup(0x0202,&wsa)!=0)
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize winsock library");
      return(false);
     }
//--- process command line parameters
   if(!ProcessCommandLine(argc,argv))
      return(false);
//--- initialize random number generator
   srand((uint32_t)GetTickCount());
//--- initialize trade dispatcher
   if(!m_trade_dispatcher.Initialize())
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize trade dispatcher");
      return(false);
     }
//--- initialize connection context
   if(!m_context.Initialize(m_address))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize exchange context");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Run application                                                  |
//+------------------------------------------------------------------+
void CMTExchangeApp::Run(void)
  {
   bool need_restart=false;
//--- main loop, unless the Esc key is not pressed
   while(true)
     {
      //--- exit when pressing the Escape key
      if(_kbhit() && _getch()==27)
        {
         ExtLogger.OutString(MTLogWarn,L"exchange stopping");
         break;
        }
      //--- check context
      need_restart=!m_context.Check(_time64(nullptr));
      //--- send ticks and process orders
      if(!need_restart)
         need_restart=!m_trade_dispatcher.DataProcess(m_context);
      //--- check if we need to restart application
      if(need_restart)
        {
         //--- restart application
         if(!Restart())
            break;
         //--- reset flag of application restart necessity
         need_restart=false;
        }
      //--- wait for new data
      m_trade_dispatcher.DataWait();
     }
  }
//+------------------------------------------------------------------+
//| Application shutdown                                             |
//+------------------------------------------------------------------+
void CMTExchangeApp::Shutdown(void)
  {
//--- shutdown context
   m_context.Shutdown();
//--- shutdown database of available symbols
   m_trade_dispatcher.Shutdown();
//--- shutdown winsock
   WSACleanup();
//--- write to log about shutdown
   ExtLogger.OutString(MTLogOK,L"exchange stopped");
  }
//+------------------------------------------------------------------+
//| Restart application                                              |
//+------------------------------------------------------------------+
bool CMTExchangeApp::Restart(void)
  {
//--- shutdown context
   m_context.Shutdown();
//--- shutdown database of available symbols
   m_trade_dispatcher.Shutdown();
//--- initialize symbol database
   if(!m_trade_dispatcher.Initialize())
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize order dispatcher");
      return(false);
     }
//--- initialize connection context
   if(!m_context.Initialize(m_address))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize exchange context");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Process command line arguments                                   |
//+------------------------------------------------------------------+
bool CMTExchangeApp::ProcessCommandLine(int32_t argc,wchar_t** argv)
  {
//--- check parameters
   if(!argc || !argv)
     {
      ExtLogger.OutString(MTLogErr,L"failed to parse command line arguments");
      return(false);
     }
//--- iterate over all arguments
   for(int32_t i=0;i<argc;i++)
     {
      //--- analyze /address parameter
      if(CMTStr::Find(argv[i],L"/address:")==0)
        {
         //--- search for the colon position after parameter name (it will be found in any case)
         wchar_t *tmp=wcschr(argv[i],':');
         //--- search for the colon in parameter value (address:port)
         if(CMTStr::FindChar(++tmp,':')<0)
            //--- if port is not specified, use default value
            CMTStr::FormatStr(m_address,_countof(m_address),L"%s:%u",tmp,DEFAULT_EXCHANGE_PORT);
         else
            //--- if specified, use this value
            CMTStr::Copy(m_address,_countof(m_address),tmp);
         continue;
        }
      //--- analyze /? parameter
      if(CMTStr::Compare(argv[i],L"/?")==0)
        {
         wprintf_s(L"\r\nUsage:\r\n  /address:ip:port   - TCP address of trade server");
         return(false);
        }
     }
//--- for empty value of parameter, set default value for address
   if(CMTStr::Len(m_address)==0)
      CMTStr::FormatStr(m_address,_countof(m_address),L"%s:%u",DEFAULT_EXCHANGE_ADDRESS,DEFAULT_EXCHANGE_PORT);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
