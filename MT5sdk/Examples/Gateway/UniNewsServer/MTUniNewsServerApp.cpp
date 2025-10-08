//+------------------------------------------------------------------+
//|                               MetaTrader 5 Universal News Server |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "MTUniNewsServerApp.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CMTUniNewsServerApp::CMTUniNewsServerApp() : m_server_port(m_source)
  {
//--- clear strings
   m_address[0]=L'\0';
   m_login[0]=L'\0';
   m_password[0]=L'\0';
   m_news_path[0]=L'\0';
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CMTUniNewsServerApp::~CMTUniNewsServerApp(void)
  {
   Shutdown();
  }
//+------------------------------------------------------------------+
//| Initialize application                                           |
//+------------------------------------------------------------------+
bool CMTUniNewsServerApp::Initialize(int32_t argc,wchar_t **argv)
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
   ExtLogger.OutString(MTLogOK,L"This is a simple example of news server for working with MetaTrader5 UniNewsFeeder datafeed.");
   ExtLogger.OutString(MTLogOK,L"To specify binding address start application using the /address:address:port command line argument.");
   ExtLogger.Out(MTLogOK,L"Default binding address is %s:%u.",DEFAULT_NEWS_SERVER_ADDRESS,DEFAULT_NEWS_SERVER_PORT);
   ExtLogger.OutString(MTLogOK,L"To specify clients login and password use the /login:user_name and /password:user_password command line arguments.");
   ExtLogger.Out(MTLogOK,L"Default login is '%s' and password is '%s'.",DEFAULT_NEWS_SERVER_LOGIN,DEFAULT_NEWS_SERVER_PASSWORD);
   ExtLogger.OutString(MTLogOK,L"To specify path with news files use /news_path:path.");
   ExtLogger.Out(MTLogOK,L"Default news path is '%s'.",DEFAULT_NEWS_DIRECTORY);
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
//--- initialize server port
   if(!m_server_port.Initialize(m_address,m_login,m_password))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize server port");
      return(false);
     }
//--- initialize news source
   if(!m_source.Initialize(m_news_path))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize news source");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Run application                                                  |
//+------------------------------------------------------------------+
void CMTUniNewsServerApp::Run(void)
  {
   bool need_restart=false;
//--- main loop, unless the Esc key is not pressed
   while(true)
     {
      //--- exit when pressing the Escape key
      if(_kbhit() && _getch()==27)
        {
         ExtLogger.OutString(MTLogWarn,L"server stopping");
         break;
        }
      //--- check server port
      need_restart=!m_server_port.Check();
      //--- process news source data
      if(!need_restart)
         need_restart=!m_source.DataProcess(m_server_port);
      //--- check if we need to restart application
      if(need_restart)
        {
         //--- restart application
         if(!Restart())
            break;
         //--- reset flag of application restart necessity
         need_restart=false;
        }
      //--- sleep
      Sleep(10);
     }
  }
//+------------------------------------------------------------------+
//| Application shutdown                                             |
//+------------------------------------------------------------------+
void CMTUniNewsServerApp::Shutdown(void)
  {
//--- shutdown server port
   m_server_port.Shutdown();
//--- shutdown news source
   m_source.Shutdown();
//--- shutdown winsock
   WSACleanup();
//--- write to log about shutdown
   ExtLogger.OutString(MTLogOK,L"server stopped");
  }
//+------------------------------------------------------------------+
//| Restart application                                              |
//+------------------------------------------------------------------+
bool CMTUniNewsServerApp::Restart(void)
  {
//--- shutdown server port
   m_server_port.Shutdown();
//--- shutdown news source
   m_source.Shutdown();
//--- initialize server port
   if(!m_server_port.Initialize(m_address,m_login,m_password))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize server port");
      return(false);
     }
//--- initialize news source
   if(!m_source.Initialize(m_news_path))
     {
      ExtLogger.OutString(MTLogErr,L"failed to initialize news source");
      return(false);
     }
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
//| Process command line arguments                                   |
//+------------------------------------------------------------------+
bool CMTUniNewsServerApp::ProcessCommandLine(int32_t argc,wchar_t** argv)
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
         wchar_t *tmp=wcschr(argv[i],L':');
         //--- search for the colon in parameter value (address:port)
         if(CMTStr::FindChar(++tmp,L':')<0)
           {
            //--- if port is not specified, use default value
            CMTStr::FormatStr(m_address,_countof(m_address),L"%s:%u",tmp,DEFAULT_NEWS_SERVER_PORT);
           }
         else
           {
            //--- if specified, use this value
            CMTStr::Copy(m_address,tmp);
           }
         continue;
        }
      //--- analyze /login parameter
      if(CMTStr::Find(argv[i],L"/login:")==0)
        {
         //--- search for the colon position after parameter name
         if(wchar_t *tmp=wcschr(argv[i],L':'))
           {
            //--- skip the colon
            tmp++;
            //--- use specified login
            CMTStr::Copy(m_login,tmp);
           }
         continue;
        }
      //--- analyze /password parameter
      if(CMTStr::Find(argv[i],L"/password:")==0)
        {
         //--- search for the colon position after parameter name
         if(wchar_t *tmp=wcschr(argv[i],L':'))
           {
            //--- skip the colon
            tmp++;
            //--- use specified login
            CMTStr::Copy(m_password,tmp);
           }
         continue;
        }
      //--- analyze /news_path parameter
      if(CMTStr::Find(argv[i],L"/news_path:")==0)
        {
         //--- search for the colon position after parameter name
         if(wchar_t *tmp=wcschr(argv[i],L':'))
           {
            //--- skip the colon
            tmp++;
            //--- use specified news path
            CMTStr::Copy(m_news_path,tmp);
           }
         continue;
        }
      //--- analyze /? parameter
      if(CMTStr::Compare(argv[i],L"/?")==0)
        {
         wprintf_s(L"\r\nUsage:\r\n");
         wprintf_s(L"   /address:ip:port        - TCP address of news server\r\n");
         wprintf_s(L"   /login:user_name        - login for client connections\r\n");
         wprintf_s(L"   /password:user_password - password for client connections\r\n");
         wprintf_s(L"   /news_path:path         - path which contains news files\r\n");
         return(false);
        }
     }
//--- for empty value of parameter, set default value for address
   if(CMTStr::Len(m_address)==0)
      CMTStr::FormatStr(m_address,L"%s:%u",DEFAULT_NEWS_SERVER_ADDRESS,DEFAULT_NEWS_SERVER_PORT);
//--- for empty value of parameter, set default value for login
   if(CMTStr::Len(m_login)==0)
      CMTStr::Copy(m_login,DEFAULT_NEWS_SERVER_LOGIN);
//--- for empty value of parameter, set default value for password
   if(CMTStr::Len(m_password)==0)
      CMTStr::Copy(m_password,DEFAULT_NEWS_SERVER_PASSWORD);
//--- for empty value of parameter, set default news path
   if(CMTStr::Len(m_news_path)==0)
      CMTStr::Copy(m_news_path,DEFAULT_NEWS_DIRECTORY);
//--- successful
   return(true);
  }
//+------------------------------------------------------------------+
