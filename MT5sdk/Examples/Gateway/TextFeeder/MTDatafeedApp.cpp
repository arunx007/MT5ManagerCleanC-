//+------------------------------------------------------------------+
//|                                         MetaTrader 5 Text Feeder |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MTDatafeedApp.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMTDatafeedApp::CMTDatafeedApp() : m_gateway(NULL),m_source(NULL),m_workflag(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMTDatafeedApp::~CMTDatafeedApp(void)
  {
//--- complete the operation of all
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Datafeed information                                             |
//+------------------------------------------------------------------+
void CMTDatafeedApp::Info(MTGatewayInfo& info)
  {
//--- form the info
   ZeroMemory(&info,sizeof(info));
//--- data
   info.version    =ProgramBuild;
   info.version_api=MTGatewayAPIVersion;
   CMTStr::Copy(info.name_default,_countof(info.name_default),ProgramName);
   CMTStr::Copy(info.copyright,_countof(info.copyright),Copyright);
   CMTStr::Copy(info.build_date,_countof(info.build_date),ProgramBuildDate);
   CMTStr::Copy(info.build_api_date,_countof(info.build_api_date),MTGatewayAPIDate);
   CMTStr::Copy(info.server_default,_countof(info.server_default),L"localhost");
   CMTStr::Copy(info.login_default,_countof(info.login_default),L"");
   CMTStr::Copy(info.password_default,_countof(info.password_default),L"");
   CMTStr::Copy(info.parameters_default,_countof(info.parameters_default),L"TickPeriod=5000\\n"
                                                                        L"TickFile=_ticks.txt\\n"
                                                                        L"BookPeriod=5000\\n"
                                                                        L"BookFile=_books.txt\\n"
                                                                        L"NewsPeriod=10000\\n"
                                                                        L"NewsFile=_news.txt\\n"
                                                                        L"EventsPeriod=50000\\n"
                                                                        L"EventsFile=_events.txt\n");
   info.mode  =MTGatewayInfo::GATEWAY_MODE_QUOTES|MTGatewayInfo::GATEWAY_MODE_NEWS;
   info.fields=MTGatewayInfo::GATEWAY_FIELD_ALL;
   CMTStr::Copy(info.description,_countof(info.description)-1,ProgramDescription);
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CMTDatafeedApp::Initialize(int32_t argc,wchar_t** argv)
  {
   MTGatewayInfo info;
   MTAPIRES      ret=MT_RET_OK;
//--- checking
   if(!argv)
      return(false);
//--- library: initialization
   if(m_apifactory.Initialize()!=MT_RET_OK)
      return(false);
//--- fill MTGatewayInfo
   Info(info);
//--- gateway: creation
   ret=m_apifactory.Create(info,&m_gateway,argc,argv);
//--- getting the description, exit
   if(ret==MT_RET_OK_NONE)
      return(false);
//--- checking
   if(ret!=MT_RET_OK || m_gateway==NULL)
      return(false);
//--- journal initialization
   ExtLogger.SetGateway(m_gateway);
//--- datasource: creation
   if((m_source=new CDataSource())==NULL)
      return(0);
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Operation                                                        |
//+------------------------------------------------------------------+
bool CMTDatafeedApp::Run(void)
  {
   bool started=false;
//--- checking
   if(!m_gateway || !m_source)
      return(false);
//--- start the processing of the external connection
   if(started=m_source->Start(m_gateway))
     {
      //--- start the receipt of incoming connections
      started=m_gateway->Start(this)==MT_RET_OK;
     }
//--- if it has started, set the operation flag
   if(started)
      InterlockedExchange(&m_workflag,1);
//--- the main loop of the external connection control
   while(InterlockedExchangeAdd(&m_workflag,0))
     {
      //--- check the external connection
      if(!m_source->Check())
         break;
      //--- sleep
      Sleep(SLEEP_TIMEOUT);
     }
//--- stop the gateway
   m_gateway->Stop();
//--- complete the operation of the datasource
   m_source->Shutdown();
//--- everything is ok
   return(true);
  }
//+------------------------------------------------------------------+
//| Completing operation                                             |
//+------------------------------------------------------------------+
void CMTDatafeedApp::Shutdown(void)
  {
//--- delete the datasource
   if(m_source)
     {
      delete m_source;
      m_source=NULL;
     }
//--- refresh the journal
   ExtLogger.SetGateway(NULL);
//--- release the gateway interface
   if(m_gateway)
     {
      m_gateway->Release();
      m_gateway=NULL;
     }
//--- complete the operation of the library
   m_apifactory.Shutdown();
  }
//+------------------------------------------------------------------+
//| Getting the datasource description                               |
//+------------------------------------------------------------------+
void CMTDatafeedApp::OnGatewayConfig(const uint64_t login,const IMTConFeeder* config)
  {
//--- complete the operation
   if(m_source)
      m_source->Init(config);
  }
//+------------------------------------------------------------------+
//| Completing the operation of the datafeed                         |
//+------------------------------------------------------------------+
void CMTDatafeedApp::OnGatewayShutdown(uint64_t login)
  {
//--- reset the operation flag
   InterlockedExchange(&m_workflag,0);
  }
//+------------------------------------------------------------------+
