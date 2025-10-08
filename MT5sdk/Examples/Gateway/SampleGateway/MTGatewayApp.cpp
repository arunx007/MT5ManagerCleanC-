//+------------------------------------------------------------------+
//|                                      MetaTrader 5 Sample Gateway |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MTGatewayApp.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMTGatewayApp::CMTGatewayApp(void)
   : m_api_gateway(NULL),
     m_gateway(NULL),
     m_working_flag(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMTGatewayApp::~CMTGatewayApp(void)
  {
//--- shutdown everything
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Description                                                      |
//+------------------------------------------------------------------+
void CMTGatewayApp::Info(MTGatewayInfo &info)
  {
//--- generate description
   ZeroMemory(&info,sizeof(info));
//--- data
   info.version    =ProgramBuild;
   info.version_api=MTGatewayAPIVersion;
   CMTStr::Copy(info.name_default,      _countof(info.name_default),      ProgramName);
   CMTStr::Copy(info.module_id,         _countof(info.module_id),         ProgramModule);
   CMTStr::Copy(info.copyright,         _countof(info.copyright),         Copyright);
   CMTStr::Copy(info.build_date,        _countof(info.build_date),        ProgramBuildDate);
   CMTStr::Copy(info.build_api_date,    _countof(info.build_api_date),    MTGatewayAPIDate);
   CMTStr::Copy(info.server_default,    _countof(info.server_default),    L"127.0.0.1");
   CMTStr::Copy(info.login_default,     _countof(info.login_default),     L"default");
   CMTStr::Copy(info.password_default,  _countof(info.password_default),  L"default");
   info.mode  =MTGatewayInfo::GATEWAY_MODE_QUOTES|MTGatewayInfo::GATEWAY_MODE_POSITIONS;
   info.fields=MTGatewayInfo::GATEWAY_FIELD_ALL;
   CMTStr::Copy(info.description,       _countof(info.description)-1,ProgramDescription);
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
bool CMTGatewayApp::Initialize(int32_t argc,wchar_t** argv)
  {
   MTGatewayInfo info;
   MTAPIRES      res=MT_RET_OK;
//--- check
   if(!argv)
      return(false);
//--- shutdown everything
   Shutdown();
//--- initialize Gateway API library
   if(m_api_factory.Initialize()!=MT_RET_OK)
      return(false);
//--- generate description
   Info(info);
//--- create Gateway API instance
   res=m_api_factory.Create(info,&m_api_gateway,argc,argv);
//--- exit if error
   if(res==MT_RET_OK_NONE)
      return(false);
//--- check
   if(res!=MT_RET_OK || m_api_gateway==NULL)
      return(false);
//--- initialize logger
   ExtLogger.SetGatewayAPI(m_api_gateway);
//--- log gateway banner
   ExtLogger.OutString(MTLogOK,L"");
   ExtLogger.Out(MTLogOK,L"%s %d.%02d build %d, %s",ProgramName,ProgramVersion/100,ProgramVersion%100,ProgramBuild,ProgramBuildDate);
   ExtLogger.OutString(MTLogOK,L"Copyright 2000-2025, MetaQuotes Ltd.");
//--- create gateway object
   if((m_gateway=new(std::nothrow) CGateway())==NULL)
     {
      ExtLogger.OutString(MTLogErr,L"MTGatewayApp: failed to create gateway instance");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Run application                                                  |
//+------------------------------------------------------------------+
bool CMTGatewayApp::Run(void)
  {
//--- check
   if(!m_api_gateway || !m_gateway)
      return(false);
//--- start threads of processing, initialize gateway
   if(!m_gateway->Initialize(m_api_gateway))
     {
      ExtLogger.OutString(MTLogErr,L"MTGatewayApp: failed to initialize gateway");
      return(false);
     }
//--- start Gateway API
   if(m_api_gateway->Start(this)!=MT_RET_OK)
     {
      ExtLogger.OutString(MTLogErr,L"MTGatewayApp: failed to start MetaTrader 5 Gateway API");
      return(false);
     }
//--- set working flag
   Working(1);
//---- main loop of check
   while(Working())
     {
      //--- check gateway state
      m_gateway->Check();
      //--- sleep
      Sleep(TIMEOUT_CHECK_STATE);
     }
//--- stop Gateway API
   m_api_gateway->Stop();
//--- shutdown gateway
   m_gateway->Shutdown();
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Shutdown                                                         |
//+------------------------------------------------------------------+
void CMTGatewayApp::Shutdown(void)
  {
//--- delete gateway pointer
   if(m_gateway)
     {
      delete m_gateway;
      m_gateway=NULL;
     }
//--- update the gateway instance in logger
   ExtLogger.SetGatewayAPI(NULL);
//--- release Gateway API interface
   if(m_api_gateway)
     {
      m_api_gateway->Release();
      m_api_gateway=NULL;
     }
//--- shutdown library
   m_api_factory.Shutdown();
  }
//+------------------------------------------------------------------+
//| Notify of server disconnect                                      |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnServerDisconnect(LPCWSTR address,const uint32_t type,const uint64_t login)
  {
//--- if history server disconnected, clear gateway config
//--- settings should update on next connection of history server
   if(m_gateway && type==IMTConServer::NET_HISTORY_SERVER)
      m_gateway->OnGatewayConfigClear();
  }
//+------------------------------------------------------------------+
//| Get gateway description                                          |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnGatewayConfig(uint64_t login,const IMTConGateway* config)
  {
//--- notify gateway
   if(m_gateway)
      m_gateway->OnGatewayConfig(config);
  }
//+------------------------------------------------------------------+
//| Start gateway                                                    |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnGatewayStart(void)
  {
//--- notify gateway
   if(m_gateway)
      m_gateway->OnGatewayStart();
  }
//+------------------------------------------------------------------+
//| Stop gateway                                                     |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnGatewayStop(void)
  {
//--- notify gateway
   if(m_gateway)
      m_gateway->OnGatewayStop();
  }
//+------------------------------------------------------------------+
//| Shutdown gateway                                                 |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnGatewayShutdown(uint64_t login)
  {
//--- reset working flag
   Working(0);
  }
//+------------------------------------------------------------------+
//| Lock trading request                                             |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnDealerLock(const MTAPIRES retcode,const IMTRequest *request,
                                 const IMTUser *user,const IMTAccount *account,
                                 const IMTOrder *order,const IMTPosition *position)
  {
//--- notify gateway
   if(m_gateway)
      m_gateway->OnGatewayDealerLock(retcode,request);
  }
//+------------------------------------------------------------------+
//| Response to confirmed request                                    |
//+------------------------------------------------------------------+
void CMTGatewayApp::OnDealerAnswer(const MTAPIRES retcode,const IMTConfirm *confirm)
  {
//--- notify gateway
   if(m_gateway)
      m_gateway->OnGatewayDealerAnswer(retcode,confirm);
  }
//+------------------------------------------------------------------+
//| Async positions request                                          |
//+------------------------------------------------------------------+
MTAPIRES CMTGatewayApp::HookGatewayPositionsRequest(void)
  {
   MTAPIRES res=MT_RET_ERROR;
//--- notify gateway
   if(m_gateway)
      res=m_gateway->HookGatewayPositionsRequest();
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
//| Async account state request                                      |
//+------------------------------------------------------------------+
MTAPIRES CMTGatewayApp::HookGatewayAccountRequest(uint64_t login,LPCWSTR account_id)
  {
   MTAPIRES res=MT_RET_ERROR;
//--- notify gateway
   if(m_gateway)
      res=m_gateway->HookGatewayAccountRequest(login,account_id);
//--- return result
   return(res);
  }
//+------------------------------------------------------------------+
