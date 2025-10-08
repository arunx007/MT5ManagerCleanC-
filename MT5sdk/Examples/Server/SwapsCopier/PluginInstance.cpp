//+------------------------------------------------------------------+
//|                                  MetaTrader 5 API Server Example |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL), m_manager(NULL),
                                         m_config(NULL), m_login(0),
                                         m_workflag(false), m_reconnect(false)
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
   MTAPIRES res=MT_RET_OK;
   uint32_t version=0;
//--- check parameters
   if(!api)
      return(MT_RET_ERR_PARAMS);
   m_api=api;
//--- create plugin config
   if((m_config=m_api->PluginCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- subscribe to plugin config updates
   if((res=m_api->PluginSubscribe(this))!=MT_RET_OK)
      return(res);
//--- load Manager API from plugins directory
   if((res=m_factory.Initialize(L"plugins\\"))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr, L"failed to initialize manager API factory [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- check version
   if((res=m_factory.Version(version))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr, L"failed to get manager API version [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
   if(version!=MTManagerAPIVersion)
     {
      m_api->LoggerOut(MTLogErr, L"invalid manager API version %u, plugin version %u",
                       version, MTManagerAPIVersion);
      return(res);
     }
//--- create manager interface
   if((res=m_factory.CreateManager(MTManagerAPIVersion,&m_manager))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr, L"failed to create manager interface [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- read plugin parameters
   if((res=ParametersRead())!=MT_RET_OK)
      return(res);
//--- run manager connection thread
   if((res=ThreadStart())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
   MTAPIRES res=MT_RET_OK;
//--- check duplicate stops
   if(m_api==NULL)
      return(MT_RET_OK);
//--- stop connection thread
   ThreadStop();
//--- release manager object
   if(m_manager!=NULL)
     {
      m_manager->Release();
      m_manager=NULL;
     }
//--- shutdown factory
   if((res=m_factory.Shutdown())!=MT_RET_OK)
      m_api->LoggerOut(MTLogErr, L"failed to shutdown the factory [%s (%u)]",
                       SMTFormat::FormatError(res),res);
//--- clean config object
   if(m_config!=NULL)
     {
      m_config->Release();
      m_config=NULL;
     }
//--- unsubscribe from plugin config updates & clean API
   if((res=m_api->PluginUnsubscribe(this))!=MT_RET_OK && res!=MT_RET_ERR_NOTFOUND)
      m_api->LoggerOut(MTLogErr, L"failed to unsubscribe from plugin config updates [%s (%u)]",
                       SMTFormat::FormatError(res),res);
//---
   m_api=NULL;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ParametersRead(void)
  {
   MTAPIRES     res=MT_RET_OK;
   IMTConParam* param=NULL;
   bool         updated=false;
//--- check pointers
   if(m_api==NULL || m_config==NULL)
      return(MT_RET_ERR_PARAMS);
//--- get current plugin configuration
   if((res=m_api->PluginCurrent(m_config))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr, L"failed to get current plugin configuration [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- create plugin parameter object   
   if((param=m_api->PluginParamCreate())==NULL)
     {
      m_api->LoggerOutString(MTLogErr, L"failed to create plugin parameter object");
      return(MT_RET_ERR_MEM);
     }
//--- lock parameters
   m_sync.Lock();
//--- get parameters
//--- address
   if((res=m_config->ParameterGet(L"Server Address", param))!=MT_RET_OK ||
      param->Type()!=IMTConParam::TYPE_STRING                           ||
      param->ValueString()[0]==0)
     {
      m_api->LoggerOutString(MTLogErr, L"server address is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
//--- compare with previous value
   if(m_server.Compare(param->ValueString())!=0)
     {
      m_server.Assign(param->ValueString());
      updated=true;
     }
//--- login
   if((res=m_config->ParameterGet(L"Manager Login", param))!=MT_RET_OK ||
      param->Type()!=IMTConParam::TYPE_INT)
     {
      m_api->LoggerOutString(MTLogErr, L"manager login is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
//--- compare with previous value
   if(m_login!=param->ValueInt())
     {
      m_login=param->ValueInt();
      updated=true;
     }
//--- password
   if((res=m_config->ParameterGet(L"Manager Password", param))!=MT_RET_OK ||
      param->Type()!=IMTConParam::TYPE_STRING                             ||
      param->ValueString()[0]==0)
     {
      m_api->LoggerOutString(MTLogErr, L"manager password is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
//--- compare with previous value     
   if(m_password.Compare(param->ValueString())!=0)
     {
      m_password.Assign(param->ValueString());
      updated=true;
     }
//--- symbols list
   if((res=m_config->ParameterGet(L"Symbols", param))!=MT_RET_OK ||
      param->Type()!=IMTConParam::TYPE_STRING                    ||
      param->ValueString()[0]==0)
     {
      //--- add default value
      param->Name(L"Symbols");
      param->ValueString(L"Forex\\Major\\*");
      //--- add parameter and update config     
      if((res=m_config->ParameterAdd(param))!=MT_RET_OK || (res=m_api->PluginAdd(m_config))!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr, L"failed to add parameter and update plugin configuration [%s (%u)]",
                          SMTFormat::FormatError(res),res);
         param->Release();
         m_sync.Unlock();
         return(res);
        }
     }
//--- symbols is not connection parameter, no need to compare
   m_symbols.Assign(param->ValueString());
//--- unlock parameters
   m_sync.Unlock();
//--- set reconnect flag
   m_reconnect=updated;
//--- free objects
   param->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin config update notification                                |
//+------------------------------------------------------------------+
void CPluginInstance::OnPluginUpdate(const IMTConPlugin* plugin)
  {
//--- check parameters
   if(plugin==NULL || m_api==NULL || m_config==NULL)
      return;
//--- update config
   if(CMTStr::Compare(plugin->Name(), m_config->Name())==0 &&
      plugin->Server()==m_config->Server())
      ParametersRead();
  }
//+------------------------------------------------------------------+
//| Connect with Manager API in separate thread                      |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ThreadStart(void)
  {
//--- check pointers
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- set work flag
   m_workflag=true;
//--- start thread
   if(!m_thread.Start(&CPluginInstance::ThreadWrapper,this,0))
     {
      m_api->LoggerOut(MTLogErr, L"failed to start connection thread [%u]",GetLastError());
      return(MT_RET_ERROR);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Stop connection thread                                           |
//+------------------------------------------------------------------+
void CPluginInstance::ThreadStop(void)
  {
//--- unset work flag
   m_workflag=false;
//--- wait until the thread stops
   m_thread.Shutdown();
  }
//+------------------------------------------------------------------+
//| Thread wrapper                                                   |
//+------------------------------------------------------------------+
uint32_t __stdcall CPluginInstance::ThreadWrapper(void* lpParam)
  {
   CPluginInstance* plugin=static_cast<CPluginInstance*>(lpParam);
//--- check pointer
   if(plugin!=NULL)
      plugin->Thread();
//--- ok
   return(0);
  }
//+------------------------------------------------------------------+
//| Thread manager function                                          |
//+------------------------------------------------------------------+
__declspec(noinline) void CPluginInstance::Thread(void)
  {
//--- run thread loop
   while(m_workflag)
     {
      //--- reconnect
      if(m_reconnect)
        {
         if(ManagerDisconnect()==MT_RET_OK)
            ManagerConnect();
         m_reconnect=false;
        }
      //--- sleep thread;
      ::Sleep(RECONNECT_TIMEOUT);
     }
//--- disconnect
   ManagerDisconnect();
  }
//+------------------------------------------------------------------+
//| Connect with Manager API                                         |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ManagerConnect(void)
  {
   MTAPIRES  res=MT_RET_OK;
   CMTStr128 server, password;
   uint64_t    login=0;
//--- check pointer
   if(m_api==NULL || m_manager==NULL)
      return(MT_RET_ERR_PARAMS);
//--- subscribe to symbol events
   if((res=m_manager->SymbolSubscribe(this))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"failed to subscribe to symbol events [%s (%u)]",
         SMTFormat::FormatError(res),res);
      return(res);
     }
//--- get parameters values
   m_sync.Lock();
   server.Assign(m_server);
   login=m_login;
   password.Assign(m_password);
   m_sync.Unlock();
//--- connect
   if((res=m_manager->Connect(server.Str(),login,password.Str(),L"",
                              IMTManagerAPI::PUMP_MODE_SYMBOLS,
                              CONNECTION_TIMEOUT))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"failed to connect with manager API [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Disconnect manager                                               |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ManagerDisconnect(void)
  {
   MTAPIRES res=MT_RET_OK;
//--- check pointers
   if(m_api==NULL || m_manager==NULL)
      return(MT_RET_ERR_PARAMS);
//--- unsubscribe from symbol events
   if((res=m_manager->SymbolUnsubscribe(this))!=MT_RET_OK && res!=MT_RET_ERR_NOTFOUND)
     {
      m_api->LoggerOut(MTLogErr,L"failed to unsubscribe from symbol events [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- ok
   m_manager->Disconnect();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| A handler of the event of updating symbol settings               |
//+------------------------------------------------------------------+
void CPluginInstance::OnSymbolUpdate(const IMTConSymbol* src_symbol)
  {
   MTAPIRES      res=MT_RET_OK;
   IMTConSymbol* dst_symbol=NULL;
//--- check parameters and pointers
   if(src_symbol==NULL || m_api==NULL)
      return;
//--- check exact matching names or path mask
//--- lock parameters
   m_sync.Lock();
   if(!CMTStr::CheckGroupMask(m_symbols.Str(),src_symbol->Symbol()) &&
      !CMTStr::CheckGroupMask(m_symbols.Str(),src_symbol->Path()))
     {
      m_sync.Unlock();
      return;
     }
//--- unlock parameters
   m_sync.Unlock();
//--- create symbol configuration
   if((dst_symbol=m_api->SymbolCreate())==NULL)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create symbol");
      return;
     }
//--- get symbol configuration from server
   if((res=m_api->SymbolGet(src_symbol->Symbol(),dst_symbol))!=MT_RET_OK)
     {
      //--- if there is no such symbol just exit
      if(res!=MT_RET_ERR_NOTFOUND)
         m_api->LoggerOut(MTLogErr,L"failed to get symbol '%s' configuration [%s (%u)]",
                          src_symbol->Symbol(),SMTFormat::FormatError(res),res);
      dst_symbol->Release();
      return;
     }
//--- copy swap settings
//--- copy mode
   if(src_symbol->SwapMode()!=dst_symbol->SwapMode())
     {
      if((res=dst_symbol->SwapMode(src_symbol->SwapMode()))!=MT_RET_OK)
         m_api->LoggerOut(MTLogErr,L"failed to copy symbol '%s' swap mode setting [%s (%u)]",
                          src_symbol->Symbol(),SMTFormat::FormatError(res),res);
      else
         m_api->LoggerOut(MTLogOK,L"copied symbol '%s' swap mode setting", src_symbol->Symbol());
     }
//--- copy long
   if(src_symbol->SwapLong()!=dst_symbol->SwapLong())
     {
      if((res=dst_symbol->SwapLong(src_symbol->SwapLong()))!=MT_RET_OK)
         m_api->LoggerOut(MTLogErr,L"failed to copy symbol '%s' swap long setting [%s (%u)]",
                          src_symbol->Symbol(),SMTFormat::FormatError(res),res);
      else
         m_api->LoggerOut(MTLogOK,L"copied symbol '%s' swap long setting", src_symbol->Symbol());
     }
//--- copy short
   if(src_symbol->SwapShort()!=dst_symbol->SwapShort())
     {
      if((res=dst_symbol->SwapShort(src_symbol->SwapShort()))!=MT_RET_OK)
         m_api->LoggerOut(MTLogErr,L"failed to copy symbol '%s' swap short setting [%s (%u)]",
                          src_symbol->Symbol(),SMTFormat::FormatError(res),res);
      else
         m_api->LoggerOut(MTLogOK,L"copied symbol '%s' swap short setting", src_symbol->Symbol());
     }
//--- copy 3Day
   if(src_symbol->Swap3Day()!=dst_symbol->Swap3Day())
     {
      if((res=dst_symbol->Swap3Day(src_symbol->Swap3Day()))!=MT_RET_OK)
         m_api->LoggerOut(MTLogErr,L"failed to copy symbol '%s' swap 3day setting [%s (%u)]",
                          src_symbol->Symbol(),SMTFormat::FormatError(res),res);
      else
         m_api->LoggerOut(MTLogOK,L"copied symbol '%s' swap 3day setting", src_symbol->Symbol());
     }
//--- update configuration
   if((res=m_api->SymbolAdd(dst_symbol))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"failed to update symbol '%s' configuration [%s (%u)]",
                       src_symbol->Symbol(),SMTFormat::FormatError(res),res);
      dst_symbol->Release();
      return;
     }
//--- delete symbol configuration
   dst_symbol->Release();
  }
//+------------------------------------------------------------------+
//| A handler of the event of symbols synchronization                |
//+------------------------------------------------------------------+
void CPluginInstance::OnSymbolSync(void)
  {
   IMTConSymbol* src_symbol=NULL;
//--- check parameters and pointers
   if(m_api==NULL || m_manager==NULL)
      return;
//--- create symbol configuration
   if((src_symbol=m_manager->SymbolCreate())==NULL)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create symbol");
      return;
     }
//--- synchronize each symbol
   for(uint32_t i=0;m_manager->SymbolNext(i,src_symbol)==MT_RET_OK;i++)
      OnSymbolUpdate(src_symbol);
//--- delete symbol configuration
   src_symbol->Release();
  }
//+------------------------------------------------------------------+