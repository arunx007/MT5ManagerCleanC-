//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyServerReport.h"
//+------------------------------------------------------------------+
//| Report colors                                                    |
//+------------------------------------------------------------------+
#define COLOR_TOTAL      0x0382CE
#define COLOR_CRITICAL   0xE1441D
#define COLOR_FLOODERS   0x3F68E4
#define COLOR_OTHERS     0xFCC503
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CDailyServerReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_9,
   L"Daily Server Logs",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_HTML|MTReportInfo::TYPE_DASHBOARD,
   L"Daily",
                    // params
     {{ MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM },
     },1            // params_total
  };
//+------------------------------------------------------------------+
//| Column description Daily Log                                     |
//+------------------------------------------------------------------+
ReportColumn CDailyServerReport::s_columns_log[]=
  {
   //--- id,name,       type,                         width,  width_max, offset,             size,digits_column,flags
     { 1,L"Date",       IMTDatasetColumn::TYPE_DATE   ,20,0, offsetof(DailyLogRecord,date)    ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Total",      IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(DailyLogRecord,total)   ,0,0,0 },
   { 3,  L"Critical",   IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(DailyLogRecord,critical),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Build                                         |
//+------------------------------------------------------------------+
ReportColumn CDailyServerReport::s_columns_build[]=
  {
   //--- id,name,       type,                         width,  width_max, offset,          size,digits_column,flags
     { 1,L"Build",      IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(BuildRecord,build)   ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Terminal",   IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(BuildRecord,client)  ,0,0,0 },
   { 3,  L"iPhone",     IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(BuildRecord,iphone)  ,0,0,0 },
   { 4,  L"Android",    IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(BuildRecord,android) ,0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description User connection                               |
//+------------------------------------------------------------------+
ReportColumn CDailyServerReport::s_columns_connection[]=
  {
   //--- id,name,       type,                         width,  width_max, offset,          size,digits_column,flags
     { 1,L"Name",       IMTDatasetColumn::TYPE_STRING ,40,0, offsetof(UserConnectionRecord,name)    ,MtFieldSize(UserConnectionRecord,name),0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Messages",   IMTDatasetColumn::TYPE_UINT32 ,20,0, offsetof(UserConnectionRecord,messages),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description User                                          |
//+------------------------------------------------------------------+
ReportColumn CDailyServerReport::s_columns_user[]=
  {
   //--- id,name,  type,                            width,  width_max, offset,          size,digits_column,flags
     { 1,L"Login", IMTDatasetColumn::TYPE_USER_LOGIN,10,0, offsetof(UserRecord,login)  ,0, 0, IMTDatasetColumn::FLAG_PRIMARY },
   { 2,L"Name",    IMTDatasetColumn::TYPE_STRING    ,40,0, offsetof(UserRecord,name)   ,MtFieldSize(UserRecord,name) },
   { 3,L"Group",   IMTDatasetColumn::TYPE_STRING    ,20,0, offsetof(UserRecord,group)  ,MtFieldSize(UserRecord,group) },
   { 4,L"Last IP", IMTDatasetColumn::TYPE_STRING    ,15,0, offsetof(UserRecord,ip)     ,MtFieldSize(UserRecord,ip) },
   { 5,L"Country", IMTDatasetColumn::TYPE_STRING    ,15,0, offsetof(UserRecord,country),MtFieldSize(UserRecord,country) },
   { 6,L"EMail",   IMTDatasetColumn::TYPE_STRING    ,25,0, offsetof(UserRecord,email)  ,MtFieldSize(UserRecord,email) },
   { 7,L"Messages",IMTDatasetColumn::TYPE_UINT32    ,15,0, offsetof(UserRecord,messages),0,0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Message                                       |
//+------------------------------------------------------------------+
ReportColumn CDailyServerReport::s_columns_message[]=
  {
   //--- id,name,      type,                         width,  width_max, offset,              size,digits_column,flags
     { 1,L"Count",     IMTDatasetColumn::TYPE_UINT64  ,10,0, offsetof(ServerMessage,number)  ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Message",   IMTDatasetColumn::TYPE_STRING  ,90,0, offsetof(ServerMessage,message) ,MtFieldSize(ServerMessage,message),0,IMTDatasetColumn::FLAG_LEFT },
  };
//+------------------------------------------------------------------+
//| Column description Manager                                       |
//+------------------------------------------------------------------+
ReportColumn CDailyServerReport::s_columns_manager[]=
  {
   //--- id,name,     type,                         width,  width_max, offset,               size,digits_column,flags
     { 1,L"IP",       IMTDatasetColumn::TYPE_STRING ,20,0, offsetof(ManagerConnection,ip)    ,MtFieldSize(ManagerConnection,ip)   ,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Logins",   IMTDatasetColumn::TYPE_STRING ,80,0, offsetof(ManagerConnection,login) ,MtFieldSize(ManagerConnection,login),0,IMTDatasetColumn::FLAG_LEFT },
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyServerReport::CDailyServerReport(void) : m_api(NULL),m_message_chart(NULL),
                                               m_user(NULL),m_manager(NULL),
                                               m_total(0),m_flooders_chart(NULL),
                                               m_builds_chart(NULL),m_stop_check_counter(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyServerReport::~CDailyServerReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDailyServerReport::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CDailyServerReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::Generate(const uint32_t type,IMTReportAPI *api)
  {
   MTAPIRES res=MT_RET_OK;
//--- checks
   if(api==NULL)
      return(MT_RET_ERR_PARAMS);
   if(type!=MTReportInfo::TYPE_HTML && type!=MTReportInfo::TYPE_DASHBOARD)
      res=MT_RET_ERR_NOTIMPLEMENT;
//--- save api pointer
   m_api=api;
   Clear();
//--- create interfaces
   if(res==MT_RET_OK)
      if(!CreateInterfaces())
         res=MT_RET_ERR_MEM;
//--- check manager rights
   if(res==MT_RET_OK)
      res=CheckRights();
//--- load all info
   if(res==MT_RET_OK)
      res=LoadInfo();
//--- load daily messages
   if(res==MT_RET_OK)
      res=LoadMessages();
//--- sort critical by number
   m_critical.Sort(SortByNumber);
//--- prepare builds
   if(res==MT_RET_OK)
      if(!PrepareBuildsFlooders())
         res=MT_RET_ERR_MEM;
//--- sort flooders array for presentation
   m_users.Sort(SortFlooderByCount);
//--- prepare html charts
   if(type==MTReportInfo::TYPE_HTML)
     {
      //--- prepare summary messages chart
      if(res==MT_RET_OK)
         res=PrepareHistoryChart();
      //--- prepare summary messages chart
      if(res==MT_RET_OK)
         res=PrepareFloodersChart();
      //--- prepare builds chart
      if(res==MT_RET_OK)
         res=PrepareBuildsChart();
      //--- write all into html
      if(res==MT_RET_OK)
         res=WriteAll();
     }
   else
     {
      //--- generate dashboard
      if(res==MT_RET_OK)
         res=GenerateDashboard();
     }
//--- show error page if error exists
   if(res!=MT_RET_OK && type==MTReportInfo::TYPE_HTML)
      res=CReportError::Write(api,L"Daily Server Report",L"Report generation failed. For more information see server's journal.");
//--- cleanup
   Clear();
   return(res);
  }
//+------------------------------------------------------------------+
//| Clear all                                                        |
//+------------------------------------------------------------------+
void CDailyServerReport::Clear(void)
  {
   if(m_message_chart)
     {
      m_message_chart->Release();
      m_message_chart=NULL;
     }
   if(m_flooders_chart)
     {
      m_flooders_chart->Release();
      m_flooders_chart=NULL;
     }
   if(m_builds_chart)
     {
      m_builds_chart->Release();
      m_builds_chart=NULL;
     }
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
   if(m_manager)
     {
      m_manager->Release();
      m_manager=NULL;
     }
   m_users.Clear();
   m_critical.Clear();
   m_daily_logs.Clear();
   m_managers.Clear();
   m_builds.Clear();
   m_total=0;
   m_stop_check_counter=0;
  }
//+------------------------------------------------------------------+
//| Create arrays interfaces                                         |
//+------------------------------------------------------------------+
bool CDailyServerReport::CreateInterfaces(void)
  {
//--- clear old
   Clear();
//--- creating
   if((m_message_chart =m_api->ChartCreate())  ==NULL)
      return(false);
   if((m_flooders_chart=m_api->ChartCreate())  ==NULL)
      return(false);
   if((m_builds_chart  =m_api->ChartCreate())  ==NULL)
      return(false);
   if((m_user          =m_api->UserCreate())   ==NULL)
      return(false);
   if((m_manager       =m_api->ManagerCreate())==NULL)
      return(false);
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Check manager rights                                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::CheckRights(void)
  {
//--- get current manager
   MTAPIRES res=m_api->ManagerCurrent(m_manager);
   if(res!=MT_RET_OK)
      return(res);
//--- check permission to access server logs
   if(m_manager->Right(IMTConManager::RIGHT_SRV_JOURNALS)==IMTConManager::RIGHT_FLAGS_DENIED)
      return(MT_RET_ERR_PERMISSIONS);
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Load all info                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::LoadInfo(void)
  {
   DailyLog  logs;
//--- first day
   int64_t first_day=SMTTime::DayBegin(m_api->ParamFrom())-DAILY_LOGS_PERIOD;
//--- end day
   int64_t end_day  =SMTTime::DayBegin(m_api->ParamFrom());
//--- for all days
   for(int64_t day=first_day;day<end_day;day=day+SECONDS_IN_DAY)
     {
      //--- fill daily logs
      const MTAPIRES res=FillDailyLogs(day,logs);
      if(res!=MT_RET_OK)
         return(res);
      //---
      if(!m_daily_logs.Add(&logs))
         return(MT_RET_ERR_MEM);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare builds and flooders                                      |
//+------------------------------------------------------------------+
bool CDailyServerReport::PrepareBuildsFlooders(void)
  {
   BuildInfo  build_info;
   BuildInfo *build=NULL;
//--- for all connected users
   for(uint32_t i=0;i<m_users.Total();i++)
     {
      //--- check user
      if(m_api->UserGetLight(m_users[i].login,m_user)!=MT_RET_OK ||
         m_api->ManagerGet(m_users[i].login,m_manager)==MT_RET_OK)
         continue;
      //--- count flooders
      m_total+=m_users[i].total_messages;
      //--- skip empty
      if(!m_users[i].last_build)
         continue;
      //--- search existing build
      if((build=m_builds.Search(&m_users[i].last_build,SearchBuild)))
         IncrementType(m_users[i].last_type,*build);
      else // insert new build by order
        {
         ZeroMemory(&build_info,sizeof(build_info));
         build_info.build=m_users[i].last_build;
         IncrementType(m_users[i].last_type,build_info);
         if(!m_builds.Insert(&build_info,SortBuild))
            return(false);
        }
     }
   return(true);
  }
//+------------------------------------------------------------------+
//| Load and process messages                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::LoadMessages(void)
  {
   CMTStr1024     path;
   CMTStr1024     message;
   uint64_t         login;
   uint32_t       line   =0;
   LPCWSTR        current=NULL;
   ServerMessage *msg    =NULL;
   MTAPIRES       res;
   ServerMessage  server_message;
   DailyLog       daily_logs;
//--- clear structure
   ZeroMemory(&daily_logs,sizeof(daily_logs));
   daily_logs.date=SMTTime::DayBegin(m_api->ParamFrom());
//--- get file name
   if(!GetFileName(path,SMTTime::DayBegin(m_api->ParamFrom())))
      return(MT_RET_ERR_NOTFOUND);
//--- open file
   if(!m_file_reader.Open(path.Str()))
      return(MT_RET_ERR_NOTFOUND);
//--- for all lines in log file
   while((current=m_file_reader.GetNextLine(line)))
     {
      //--- check
      if(CMTStr::Len(current)<16)
         continue;
      //--- check for stop
      if((res=IsStopped())!=MT_RET_OK)
         return(res);
      //---
      uint32_t   code=_wtoi(&current[3]);
      message.Assign(current);
      message.Delete(0,20);
      message.ReplaceChar(L'\t',L' ');
      //--- if it is error message
      if(code==MTLogAtt)
        {
         //--- fill daily structure
         daily_logs.critical++;
         //---
         if((msg=m_critical.Search(message.Str(),SearchByAlphabetical)))
            msg->number++;
         else
           {
            //--- clear structure
            ZeroMemory(&server_message,sizeof(server_message));
            server_message.number=1;
            CMTStr::Copy(server_message.message,message.Str());
            if(!m_critical.Insert(&server_message,SortByAlphabetical))
               return(MT_RET_ERR_MEM);
           }
        }
      //--- fill daily structure
      daily_logs.total++;
      //--- is there message from some login?
      if(MessageHasLogin(message,login))
        {
         if((res=ProcessLoginMessage(message,login))!=MT_RET_OK)
            return(res);
        }
     }
//--- close file
   m_file_reader.Close();
//--- add daily structure
   if(!m_daily_logs.Add(&daily_logs))
      return(MT_RET_ERR_MEM);
//--- all right
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Process login message                                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::ProcessLoginMessage(const CMTStr& message,const uint64_t& login)
  {
   ManagerConnection  manager_con;
   CMTStr32           ip;
   CMTStr32           temp_login;
   UserConnection     user_info;
   ManagerConnection *manager    =NULL;
   int32_t                first_space=0;
   int32_t                pos        =0;
   UserConnection    *user       =NULL;
//--- search exsisting flooders
   if((user=m_users.Search(&login,SearchUser)))
      user->total_messages++;
   else // insert new flooder by order
     {
      ZeroMemory(&user_info,sizeof(user_info));
      user_info.login=login;
      user_info.total_messages=1;
      if(!m_users.Insert(&user_info,SortUser))
         return(MT_RET_ERR_MEM);
     }
//--- is it login message?
   if((pos=message.Find(L"': login ("))>=0)
     {
      pos+=CMTStr::Len(L"': login (");
      //--- determine type of connection
      if((first_space=message.Find(L" ",pos))<0)
         return(MT_RET_OK);
      CMTStr32 type;
      type.Assign(message.Str()+pos,first_space-pos);
      //--- determine type of client
      uint32_t type_client=GetClientType(message.Str()+pos);
      //--- it is client connection?
      if(type_client>=TERMINAL_FIRST && type_client<=TERMINAL_LAST)
        {
         if((pos=message.Find(L"build "))>=0)
           {
            CMTStr16 build_string;
            int32_t      first_bracket=0;
            //---
            if((first_bracket=message.FindChar(L')'))>=0)
              {
               pos+=CMTStr::Len(L"build ");
               build_string.Assign(message.Str()+pos,first_bracket-pos);
               uint32_t build=_wtoi(build_string.Str());
               //--- search an existing user
               if((user=m_users.Search(&login,SearchUser)))
                 {
                  user->last_type =type_client;
                  user->last_build=build;
                 }
               else
                 {
                  ZeroMemory(&user_info,sizeof(user_info));
                  user_info.login     =login;
                  user_info.last_build=build;
                  user_info.last_type =type_client;
                  if(!m_users.Insert(&user_info,SortUser))
                     return(MT_RET_ERR_MEM);
                 }
              }
           }
        }
      //--- is it manager connection?
      if(type_client==TERMINAL_ADMIN       ||
         type_client==TERMINAL_MANAGER     ||
         type_client==TERMINAL_MANAGER_API ||
         type_client==TERMINAL_MANAGER_WEB_API)
        {
         //--- determine ip adress from message
         if((first_space=message.FindChar(L'\''))<0)
            return(MT_RET_OK);
         ip.Assign(message);
         ip.Trim(first_space-1);
         //--- login
         temp_login.Format(L"%I64u",login);
         //--- search existing manager
         if((manager=m_managers.Search(ip.Str(),SearchManagerByIp)))
           {
            //--- does login exist already?
            if(CMTStr::Find(manager->login,temp_login.Str())<0)
              {
               //--- append login adress at the end of string
               CMTStr::Append(manager->login,L", ");
               CMTStr::Append(manager->login,temp_login.Str());
               CMTStr::Append(manager->login,L" (");
               CMTStr::Append(manager->login,type.Str());
               CMTStr::Append(manager->login,L")");
              }
           }
         else // insert new manager by order
           {
            ZeroMemory(&manager_con,sizeof(manager_con));
            CMTStr::Copy(manager_con.login,temp_login.Str());
            CMTStr::Append(manager_con.login,L" (");
            CMTStr::Append(manager_con.login,type.Str());
            CMTStr::Append(manager_con.login,L")");
            CMTStr::Copy(manager_con.ip,ip.Str());
            if(!m_managers.Insert(&manager_con,SortManagerByIp))
               return(MT_RET_ERR_MEM);
           }
        }
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Increment build counter by type                                  |
//+------------------------------------------------------------------+
void CDailyServerReport::IncrementType(const uint32_t type,BuildInfo& build)
  {
   switch(type)
     {
      case TERMINAL_IPHONE:
         build.iphone_number++;
         break;
      case TERMINAL_ANDROID:
         build.android_number++;
         break;
      case TERMINAL_CLIENT:
         build.client_number++;
         break;
     }
  }
//+------------------------------------------------------------------+
//| Get client`s type by string                                      |
//+------------------------------------------------------------------+
uint32_t CDailyServerReport::GetClientType(const LPCWSTR type)
  {
//--- managers
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_ADMIN))          >=0)
      return(TERMINAL_ADMIN);
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_MANAGER))        >=0)
      return(TERMINAL_MANAGER);
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_MANAGER_API))    >=0)
      return(TERMINAL_MANAGER_API);
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_MANAGER_WEB_API))>=0)
      return(TERMINAL_MANAGER_WEB_API);
//--- clients
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_IPHONE))         >=0)
      return(TERMINAL_IPHONE);
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_ANDROID))        >=0)
      return(TERMINAL_ANDROID);
   if(CMTStr::Find(type,ConnectionStr(TERMINAL_CLIENT))         >=0)
      return(TERMINAL_CLIENT);
//--- error
   return(-1);
  }
//+------------------------------------------------------------------+
//| if message has login                                             |
//+------------------------------------------------------------------+
bool CDailyServerReport::MessageHasLogin(const CMTStr& message,uint64_t& login)
  {
   int32_t      pos=0;
   CMTStr32 login_str;
//---
   login=0;
//---
   if((pos=message.FindChar(L'\''))<=0)
      return(false);
   login_str.Assign(message.Str()+pos+1,login_str.Max());
   if((pos=login_str.FindChar(L'\''))<=0)
      return(false);
   login_str.Trim(pos);
   if(!(login=_wtoi64(login_str.Str())))
      return(false);
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Fill daily logs                                                  |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::FillDailyLogs(const int64_t day,DailyLog& daily_logs)
  {
   CMTStr1024 path;
   uint32_t   line=0;
   LPCWSTR    temp=NULL;
//--- clear structure
   ZeroMemory(&daily_logs,sizeof(daily_logs));
   daily_logs.date=day;
//--- get file name
   if(!GetFileName(path,day))
      return(MT_RET_OK);
//--- open file
   if(!m_file_reader.Open(path.Str()))
      return(MT_RET_OK);
//---
   while((temp=m_file_reader.GetNextLine(line)))
     {
      if(CMTStr::Len(temp)<16)
         continue;
      //--- check for stop
      const MTAPIRES res=IsStopped();
      if(res!=MT_RET_OK)
         return(res);
      //---
      uint32_t code=_wtoi(&temp[3]);
      //---
      if(code==MTLogAtt)
         daily_logs.critical++;
      daily_logs.total++;
     }
//--- close file
   m_file_reader.Close();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare summary messages chart                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::PrepareBuildsChart(void)
  {
   IMTReportSeries *series=NULL;
   MTAPIRES         res;
   CMTStr256        tooltip;
//--- checks
   if(m_api==NULL) return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_builds_chart->Type(IMTReportChart::TYPE_GRAPH_ACCUMULATION);
   m_builds_chart->Title(L"Client Builds");
//--- prepare title series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_TITLE);
//--- for all clients
   for(uint32_t i=0;i<m_builds.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_builds[i].build))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_builds_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare client series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(MTAPI_COLOR_TERMINAL_CLIENT);
   series->Title(ConnectionStr(TERMINAL_CLIENT,true));
   tooltip.Format(L"Client: %s<BR>Number: ",ConnectionStr(TERMINAL_CLIENT,true));
   tooltip.Append(L"%VALUE%");
   series->Tooltip(tooltip.Str());
//---
   for(uint32_t i=0;i<m_builds.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_builds[i].client_number))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_builds_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare IPhone series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(MTAPI_COLOR_TERMINAL_IPHONE);
   series->Title(ConnectionStr(TERMINAL_IPHONE,true));
   tooltip.Format(L"Client: %s<BR>Number: ",ConnectionStr(TERMINAL_IPHONE,true));
   tooltip.Append(L"%VALUE%");
   series->Tooltip(tooltip.Str());
//---
   for(uint32_t i=0;i<m_builds.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_builds[i].iphone_number))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_builds_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare Android series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(MTAPI_COLOR_TERMINAL_ANDROID);
   series->Title(ConnectionStr(TERMINAL_ANDROID,true));
   tooltip.Format(L"Client: %s<BR>Number: ",ConnectionStr(TERMINAL_ANDROID,true));
   tooltip.Append(L"%VALUE%");
   series->Tooltip(tooltip.Str());
//---
   for(uint32_t i=0;i<m_builds.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_builds[i].android_number))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_builds_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare summary messages chart                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::PrepareHistoryChart(void)
  {
   IMTReportSeries *series=NULL;
   MTAPIRES         res;
   CMTStr32         str;
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_message_chart->Type(IMTReportChart::TYPE_GRAPH);
   m_message_chart->Title(L"Server Messages");
//--- prepare title series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_TITLE);
//---
   for(uint32_t i=0;i<m_daily_logs.Total();i++)
     {
      //--- format date for current time
      str.Format(L"%02u.%02u.%04u",
         SMTTime::Day(m_daily_logs[i].date),
         SMTTime::Month(m_daily_logs[i].date),
         SMTTime::Year(m_daily_logs[i].date));
      //--- add value
      if((res=series->ValueAdd(str.Str()))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_message_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare config series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_TOTAL);
   series->Title(L"Total");
   series->Tooltip(L"Number: %VALUE%");
//---
   for(uint32_t i=0;i<m_daily_logs.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_daily_logs[i].total))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_message_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- prepare network series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
   series->Type(IMTReportSeries::TYPE_BAR);
   series->Color(COLOR_CRITICAL);
   series->Title(L"Critical");
   series->Tooltip(L"Number: %VALUE%");
//---
   for(uint32_t i=0;i<m_daily_logs.Total();i++)
     {
      //--- add value
      if((res=series->ValueAddInt(m_daily_logs[i].critical))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- add series (after adding series will be released by chart)
   if((res=m_message_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Prepare summary messages chart                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::PrepareFloodersChart(void)
  {
   IMTReportSeries *series=NULL;
   MTAPIRES         res;
   CMTStr128        str;
   uint32_t         others=m_total;
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- prepare chart
   m_flooders_chart->Type(IMTReportChart::TYPE_PIE);
   m_flooders_chart->Title(L"Top Flooders");
//--- setup pie tooltip format
   m_flooders_chart->PieceTooltip(L"%DESCRIPTION%<BR>Messages: %VALUE%");
//---
   for(uint32_t i=0;i<MAX_TOP_FLOODERS && i<m_users.Total();i++)
     {
      //--- check user
      if(m_api->UserGetLight(m_users[i].login,m_user)!=MT_RET_OK ||
         !m_users[i].total_messages                              ||
         m_api->ManagerGet(m_users[i].login,m_manager)==MT_RET_OK) continue;
      //--- prepare flooders series
      if((series=m_api->ChartCreateSeries())==NULL)
         return(MT_RET_ERR_MEM);
      //---
      series->Type(IMTReportSeries::TYPE_PIECE);
      series->Title(m_user->Name());
      series->Color(COLOR_FLOODERS+((30*i)<<8)+i*5);
      others-=m_users[i].total_messages;
      if((res=series->ValueAddInt(m_users[i].total_messages))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
      //--- add description
      str.Format(L"Login: %I64u<BR>Name: %s<BR>Group: %s",m_user->Login(),m_user->Name(),m_user->Group());
      series->ValueDescription(0,str.Str());
      //--- add series (after adding series will be released by chart)
      if((res=m_flooders_chart->SeriesAdd(series))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- prepare others series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Others");
   series->Color(COLOR_OTHERS);
   if((series->ValueAddInt(others))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
   series->ValueDescription(0,L"Others");
//--- add series (after adding series will be released by chart)
   if((res=m_flooders_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get log file name by date                                        |
//+------------------------------------------------------------------+
bool CDailyServerReport::GetFileName(CMTStr& path,const int64_t day)
  {
   CMTStr32 name;
   int32_t      pos=-1;
//---
   path.Clear();
   name.Format(L"%04u%02u%02u.log",SMTTime::Year(day),
      SMTTime::Month(day),SMTTime::Day(day));
//--- get path
   if(!GetModuleFileNameW(NULL,path.Buffer(),path.Max()))
      return(false);
   path.Refresh();
//--- 
   if((pos=path.FindRChar(L'\\'))>0)
      path.Trim(pos);
   else
      return(false);
   path.Refresh();
//--- get full path to log file
   path.Append(L"\\logs\\");
   path.Append(name);
   return(true);
  }
//+------------------------------------------------------------------+
//| Write all into html                                              |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::WriteAll(void)
  {
   MTAPISTR      tag;
   uint32_t      counter;
   MTAPIRES      res   =MT_RET_OK;
   IMTConReport *report=NULL;
//--- create report interface
   if((report=m_api->ReportCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- get report interface
   report->Clear();
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
      return(res);
//--- use template
   if((res=m_api->HtmlTplLoadResource(IDR_HTML_DAILY_SERVER_REPORT,RT_HTML))!=MT_RET_OK)
     {
      report->Release();
      return(res);
     }
//--- process tags
   while((res=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
     {
      //--- print the report name
      if(CMTStr::CompareNoCase(tag,L"report_name")==0)
        {
         if((res=m_api->HtmlWriteSafe(report->Name(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- messages chart
      if(CMTStr::CompareNoCase(tag,L"chart_messages")==0)
        {
         if((res=m_api->ChartWriteHtml(m_message_chart))!=MT_RET_OK)
            break;
         continue;
        }
      //--- flooders chart
      if(CMTStr::CompareNoCase(tag,L"flooders_chart")==0)
        {
         if((res=m_api->ChartWriteHtml(m_flooders_chart))!=MT_RET_OK)
            break;
         continue;
        }
      //--- builds chart
      if(CMTStr::CompareNoCase(tag,L"builds_chart")==0)
        {
         if((res=m_api->ChartWriteHtml(m_builds_chart))!=MT_RET_OK)
            break;
         continue;
        }
      //--- date
      if(CMTStr::CompareNoCase(tag,L"date")==0)
        {
         CMTStr256 str;
         str.Format(L"%02u.%02u.%04u",
             SMTTime::Day(m_api->ParamFrom()),
             SMTTime::Month(m_api->ParamFrom()),
             SMTTime::Year(m_api->ParamFrom()));
         if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
            break;
         continue;
        }
      //--- write message table
      if(WriteMessages(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write managers table
      if(WriteManagers(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
     }
   report->Release();
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write message table                                              |
//+------------------------------------------------------------------+
bool CDailyServerReport::WriteMessages(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
//--- if no messages
   if(CMTStr::CompareNoCase(tag,L"no_messages")==0 && counter==0 && m_critical.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- all messages
   while(CMTStr::CompareNoCase(tag,L"messages")==0 && counter<m_critical.Total())
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- print number of repetition
         if(CMTStr::CompareNoCase(tag,L"count")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_critical[save_counter].number))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print message
         if(CMTStr::CompareNoCase(tag,L"message")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_critical[save_counter].message,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print the color of string
         if(CMTStr::CompareNoCase(tag,L"line")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%u",save_counter%2))!=MT_RET_OK)
               return(true);
            continue;
           }
         break;
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Get type connection                                              |
//+------------------------------------------------------------------+
LPCWSTR CDailyServerReport::ConnectionStr(const uint32_t type,const bool short_name/*=false*/)
  {
//--- смотрим
   switch(type)
     {
      case TERMINAL_IPHONE         : return(short_name ?  L"iPhone" : L"Client iPhone");
      case TERMINAL_ANDROID        : return(short_name ?  L"Android" : L"Client Android");
      case TERMINAL_CLIENT         : return(short_name ?  L"Terminal" : L"Client");
      case TERMINAL_ADMIN          : return(L"Admin");
      case TERMINAL_MANAGER        : return(L"Manager");
      case TERMINAL_MANAGER_API    : return(L"ManagerAPI");
      case TERMINAL_MANAGER_WEB_API: return(L"Manager WebAPI");
     }
//--- по умолчанию не знаем
   return(L"unknown");
  }
//+------------------------------------------------------------------+
//| Write message table                                              |
//+------------------------------------------------------------------+
bool CDailyServerReport::WriteManagers(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
//--- if no messages
   if(CMTStr::CompareNoCase(tag,L"no_managers")==0 && counter==0 && m_managers.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- all messages
   while(CMTStr::CompareNoCase(tag,L"managers")==0 && counter<m_managers.Total())
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- write ips
         if(CMTStr::CompareNoCase(tag,L"ip")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_managers[save_counter].ip,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print login
         if(CMTStr::CompareNoCase(tag,L"logins")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_managers[save_counter].login,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print the color of string
         if(CMTStr::CompareNoCase(tag,L"line")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%u",save_counter%2))!=MT_RET_OK)
               return(true);
            continue;
           }
         break;
        }
     }
   return(false);
  }
//+------------------------------------------------------------------+
//| Generate dashboard report                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::GenerateDashboard(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- add history chart
   MTAPIRES res=AddChartHistory();
   if(res!=MT_RET_OK)
      return(res);
//--- add table message
   if((res=AddTableMessage())!=MT_RET_OK)
      return(res);
//--- add flood chart
   if((res=AddChartFlood())!=MT_RET_OK)
      return(res);
//--- add table manager
   if((res=AddTableManager())!=MT_RET_OK)
      return(res);
//--- add build chart
   if((res=AddChartBuild())!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Generate dashboard chart history                                 |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::AddChartHistory(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_log,_countof(s_columns_log));
   if(res!=MT_RET_OK)
      return(res);
//--- for all clients
   for(uint32_t i=0,total=m_daily_logs.Total();i<total;i++)
     {
      //--- fill record
      DailyLogRecord row;
      const DailyLog &log=m_daily_logs[i];
      row.date=log.date;
      row.total=log.total;
      row.critical=log.critical;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- add builds charts
   return(AddDashboardChart(data,L"Server Messages",true,false));
  }
//+------------------------------------------------------------------+
//| Generate dashboard chart build                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::AddChartBuild(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_build,_countof(s_columns_build));
   if(res!=MT_RET_OK)
      return(res);
//--- for all clients
   for(uint32_t i=0,total=m_builds.Total();i<total;i++)
     {
      //--- fill record
      BuildRecord row;
      const BuildInfo &build=m_builds[i];
      row.build=build.build;
      row.client=build.client_number;
      row.iphone=build.iphone_number;
      row.android=build.android_number;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
     }
//--- add builds charts
   return(AddDashboardChart(data,L"Client Builds",true,true));
  }
//+------------------------------------------------------------------+
//| Generate dashboard chart flood                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::AddChartFlood(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   IMTDataset *data=m_api->DatasetAppend();
   if(!data)
      return(MT_RET_ERR_MEM);
//--- create dataset
   IMTDataset *data_user=m_api->DatasetAppend();
   if(!data_user)
      return(MT_RET_ERR_MEM);
//--- create dataset columns
   MTAPIRES res=ReportColumn::ReportColumnsAdd(*data,s_columns_connection,_countof(s_columns_connection));
   if(res!=MT_RET_OK)
      return(res);
//--- create dataset columns
   res=ReportColumn::ReportColumnsAdd(*data_user,s_columns_user,_countof(s_columns_user));
   if(res!=MT_RET_OK)
      return(res);
//--- for all clients
   for(uint32_t i=0,total=std::min(m_users.Total(),(uint32_t)MAX_TOP_FLOODERS_DASH);i<total;i++)
     {
      //--- check user
      const UserConnection &conn=m_users[i];
      if(m_api->UserGet(conn.login,m_user)!=MT_RET_OK || !conn.total_messages || m_api->ManagerGet(conn.login,m_manager)==MT_RET_OK)
         continue;
      //--- fill record
      UserConnectionRecord row;
      CMTStr::Copy(row.name,m_user->Name());
      row.messages=conn.total_messages;
      //--- write row
      if((res=data->RowWrite(&row,sizeof(row)))!=MT_RET_OK)
         return(res);
      //--- fill record
      UserRecord user;
      user.login=conn.login;
      CMTStr::Copy(user.group,m_user->Group());
      MTAPISTR ip;
      CMTStr::Copy(user.ip,m_user->LastIP(ip));
      CMTStr::Copy(user.country,m_user->Country());
      CMTStr::Copy(user.email,m_user->EMail());
      CMTStr::Copy(user.name,m_user->Name());
      user.messages=conn.total_messages;
      //--- write user row
      if((res=data_user->RowWrite(&user,sizeof(user)))!=MT_RET_OK)
         return(res);
     }
//--- add chart
   res=AddDashboardChart(data,L"Top Flooders",false,false);
   if(res!=MT_RET_OK)
      return(res);
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data_user,L"Top Flooders Details") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard table message                                 |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::AddTableMessage(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   MTAPIRES res=MT_RET_OK;
   IMTDataset *data=ReportColumn::DatasetFromArray(*m_api,res,s_columns_message,_countof(s_columns_message),m_critical);
   if(data==NULL)
      return(res);
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data,L"Critical Errors") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard table manager                                 |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::AddTableManager(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   MTAPIRES res=MT_RET_OK;
   IMTDataset *data=ReportColumn::DatasetFromArray(*m_api,res,s_columns_manager,_countof(s_columns_manager),m_managers);
   if(data==NULL)
      return(res);
//--- add table
   return(ReportColumn::TableAdd(*m_api,res,data,L"Managers Connections") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Add Chart                                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::AddDashboardChart(IMTDataset *data,LPCWSTR title,bool bar,bool acc)
  {
//--- checks
   if(m_api==NULL || data==NULL || title==NULL)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(bar ? IMTReportDashboardWidget::WIDGET_TYPE_CHART_LINE : IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE);
   if(res!=MT_RET_OK)
      return(res);
//--- set accumulation
   if(acc)
      if((res=chart->ChartStackType(IMTReportDashboardWidget::CHART_STACK_ACCUMULATION))!=MT_RET_OK)
         return(res);
//--- set title
   if((res=chart->Title(title))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Check for stopped state                                          |
//+------------------------------------------------------------------+
MTAPIRES CDailyServerReport::IsStopped(void)
  {
//--- check stop counter
   m_stop_check_counter++;
   if(m_stop_check_counter&0xfff)
      return(MT_RET_OK);
//--- check api
   if(!m_api)
      return(MT_RET_ERR_PARAMS);
//--- check stopped
   return(m_api->IsStopped());
  }
//+------------------------------------------------------------------+
//| Sort message in lexicographical order                            |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SortByAlphabetical(const void *left,const void *right)
  {
   ServerMessage* lft=(ServerMessage*)left;
   ServerMessage* rgh=(ServerMessage*)right;
//--- compare string
   return(CMTStr::Compare(lft->message,rgh->message));
  }
//+------------------------------------------------------------------+
//| Search message in lexicographical order                          |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SearchByAlphabetical(const void *left,const void *right)
  {
   LPCWSTR        lft=(LPCWSTR)left;
   ServerMessage* rgh=(ServerMessage*)right;
//--- compare string
   return(CMTStr::Compare(lft,rgh->message,MAX_SYMBOLS_COMPARING));
  }
//+------------------------------------------------------------------+
//| Sort message by count                                            |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SortByNumber(const void *left,const void *right)
  {
   ServerMessage* lft=(ServerMessage*)left;
   ServerMessage* rgh=(ServerMessage*)right;
//---
   if(lft->number>rgh->number)
      return(-1);
   if(lft->number<rgh->number)
      return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort flooder by count                                            |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SortFlooderByCount(const void *left,const void *right)
  {
   UserConnection* lft=(UserConnection*)left;
   UserConnection* rgh=(UserConnection*)right;
//---
   if(lft->total_messages>rgh->total_messages)
      return(-1);
   if(lft->total_messages<rgh->total_messages)
      return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort manager by login                                            |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SortManagerByIp(const void *left,const void *right)
  {
   ManagerConnection* lft=(ManagerConnection*)left;
   ManagerConnection* rgh=(ManagerConnection*)right;
//--- compare string
   return(CMTStr::Compare(lft->ip,rgh->ip));
  }
//+------------------------------------------------------------------+
//| Search manager by login                                          |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SearchManagerByIp(const void *left,const void *right)
  {
   LPCWSTR            lft=(LPCWSTR)left;
   ManagerConnection *rgh=(ManagerConnection*)right;
//--- compare string
   return(CMTStr::Compare(lft,rgh->ip));
  }
//+------------------------------------------------------------------+
//| Sort build                                                       |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SortBuild(const void *left,const void *right)
  {
   BuildInfo* lft=(BuildInfo*)left;
   BuildInfo* rgh=(BuildInfo*)right;
//---
   if(lft->build>rgh->build)
      return(1);
   if(lft->build<rgh->build)
      return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Search build                                                     |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SearchBuild(const void *left,const void *right)
  {
   uint32_t*  lft=(uint32_t*)left;
   BuildInfo* rgh=(BuildInfo*)right;
//---
   if((*lft)>rgh->build)
      return(1);
   if((*lft)<rgh->build)
      return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort user                                                        |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SortUser(const void *left,const void *right)
  {
   UserConnection* lft=(UserConnection*)left;
   UserConnection* rgh=(UserConnection*)right;
//---
   if(lft->login>rgh->login)
      return(1);
   if(lft->login<rgh->login)
      return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Search user                                                      |
//+------------------------------------------------------------------+
int32_t CDailyServerReport::SearchUser(const void *left,const void *right)
  {
   uint64_t*         lft=(uint64_t*)left;
   UserConnection* rgh=(UserConnection*)right;
//---
   if((*lft)>rgh->login)
      return(1);
   if((*lft)<rgh->login)
      return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
