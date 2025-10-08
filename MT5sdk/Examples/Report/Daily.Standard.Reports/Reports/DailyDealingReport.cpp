//+------------------------------------------------------------------+
//|                               MetaTrader 5 Daily.Standard.Report |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "DailyDealingReport.h"
//+------------------------------------------------------------------+
//| Report colors                                                    |
//+------------------------------------------------------------------+
#define COLOR_MANAGERS   0x3F68E4
#define COLOR_OTHERS     0xFCC503
//+------------------------------------------------------------------+
//| Plug-in description structure                                    |
//+------------------------------------------------------------------+
MTReportInfo CDailyDealingReport::s_info=
  {
   100,
   MTReportAPIVersion,
   MTReportInfo::IE_VERSION_9,
   L"Daily Dealing",
   L"Copyright 2000-2025, MetaQuotes Ltd.",
   L"MetaTrader 5 Report API plug-in",
   MTReportInfo::SNAPSHOT_NONE,
   MTReportInfo::TYPE_HTML|MTReportInfo::TYPE_DASHBOARD,
   L"Daily",
                    // params
     {{ MTReportParam::TYPE_DATE,   MTAPI_PARAM_FROM                                           },
     },1,           // params_total
     {              // configs
      { MTReportParam::TYPE_STRING, L"Currency",                    DEFAULT_CURRENCY           },
      { MTReportParam::TYPE_STRING, L"Groups",                      DEFAULT_GROUPS             },
      { MTReportParam::TYPE_INT,    L"Dangerous Tick Deviation",    DEFAULT_TICK_PIPS_TRESHOLD },
      { MTReportParam::TYPE_FLOAT,  L"Dangerous Profit",            DEFAULT_PROFIT_TRESHOLD    },
      { MTReportParam::TYPE_INT,    L"Max Dangerous Actions Shown", DEFAULT_MAX_LINE           },
     },5            // configs_total
  };
//+------------------------------------------------------------------+
//| Column description Manager                                       |
//+------------------------------------------------------------------+
ReportColumn CDailyDealingReport::s_columns_manager[]=
  {
   //--- id,name,       type,                         width,  width_max, offset,                size,digits_column,flags
     { 1,L"Login",      IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(Manager,login)             ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Manager",    IMTDatasetColumn::TYPE_STRING ,40,0, offsetof(Manager,name)              ,MtFieldSize(Manager,name),0,0 },
   { 3,  L"Connections",IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(Manager,connections_count) ,0,0,0 },
   { 4,  L"Requests",   IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(Manager,requests_count)    ,0,0,0 },
   { 5,  L"Rejects",    IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(Manager,rejects_count)     ,0,0,0 },
   { 6,  L"Requotes",   IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(Manager,requotes_count)    ,0,0,0 },
   { 7,  L"Profit",     IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(Manager,profit_currency)   ,0,0,0 },
   { 8,  L"Profit Pips",IMTDatasetColumn::TYPE_INT64  ,20,0, offsetof(Manager,profit_pips)       ,0,0,0 }
  };
//+------------------------------------------------------------------+
//| Column description Suspect Action                                |
//+------------------------------------------------------------------+
ReportColumn CDailyDealingReport::s_columns_action[]=
  {
   //--- id,name,       type,                         width,  width_max, offset,                    size,digits_column,flags
     { 1,L"Login",      IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(SuspectAction,login)          ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Profit",     IMTDatasetColumn::TYPE_MONEY  ,20,0, offsetof(SuspectAction,profit_currency),0,0,0 },
   { 3,  L"Profit Pips",IMTDatasetColumn::TYPE_INT64  ,20,0, offsetof(SuspectAction,profit_pips)    ,0,0,0 },
   { 4,  L"Details",    IMTDatasetColumn::TYPE_STRING ,50,0, offsetof(SuspectAction,details)        ,MtFieldSize(SuspectAction,details),0,0 },
  };
//+------------------------------------------------------------------+
//| Column description Dealer Tick                                   |
//+------------------------------------------------------------------+
ReportColumn CDailyDealingReport::s_columns_tick[]=
  {
   //--- id,name,       type,                         width,  width_max, offset,                    size,digits_column,flags
     { 1,L"Login",      IMTDatasetColumn::TYPE_UINT64 ,10,0, offsetof(DealerTick,login)          ,0,0,IMTDatasetColumn::FLAG_PRIMARY },
   { 2,  L"Symbol",     IMTDatasetColumn::TYPE_STRING ,20,0, offsetof(DealerTick,symbol)         ,MtFieldSize(DealerTick,symbol),0,0 },
   { 3,  L"Bid",        IMTDatasetColumn::TYPE_UINT64 ,20,0, offsetof(DealerTick,bid)            ,0,0,0 },
   { 4,  L"Ask",        IMTDatasetColumn::TYPE_UINT64 ,20,0, offsetof(DealerTick,ask)            ,0,0,0 }
  };
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CDailyDealingReport::CDailyDealingReport(void) : m_api(NULL),m_pips(0),m_profit(0.0),m_max_line(0),
                                                 m_user(NULL),m_symbol(NULL),m_group(NULL),m_gateway(NULL),m_digits(0),
                                                 m_managers_chart(NULL)
  {
   ZeroMemory(m_currency,sizeof(m_currency));
   ZeroMemory(m_groups,sizeof(m_groups));
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CDailyDealingReport::~CDailyDealingReport(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Get information about report                                     |
//+------------------------------------------------------------------+
void CDailyDealingReport::Info(MTReportInfo& info)
  {
//--- get info information
   info=s_info;
  }
//+------------------------------------------------------------------+
//| Release                                                          |
//+------------------------------------------------------------------+
void CDailyDealingReport::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Report generation                                                |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::Generate(const uint32_t type,IMTReportAPI *api)
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
//--- get parameter currency
   if(res==MT_RET_OK)
      res=GetParameters();
//--- load all info
   if(res==MT_RET_OK)
      res=LoadInfo();
//--- prepare managers html chart
   if(type==MTReportInfo::TYPE_HTML && res==MT_RET_OK)
      res=PrepareManagersChart();
//--- sort suspect action by absolute value of profit
   m_suspect_actions.Sort(SortActionByProfit);
//--- write all to dashboard or html
   if(res==MT_RET_OK)
     {
      if(type==MTReportInfo::TYPE_DASHBOARD)
         res=GenerateDashboard();
      else
         res=WriteAll();
     }
//--- show error page if error exists
   if(res!=MT_RET_OK && type==MTReportInfo::TYPE_HTML)
      res=CReportError::Write(api,L"Daily Dealing Report",L"Report generation failed. For more information see server's journal.");
//--- cleanup
   Clear();
   return(res);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CDailyDealingReport::Clear(void)
  {
   if(m_managers_chart)
     {
      m_managers_chart->Release();
      m_managers_chart=NULL;
     }
   if(m_user)
     {
      m_user->Release();
      m_user=NULL;
     }
   if(m_symbol)
     {
      m_symbol->Release();
      m_symbol=NULL;
     }
   if(m_group)
     {
      m_group->Release();
      m_group=NULL;
     }
   if(m_gateway)
     {
      m_gateway->Release();
      m_gateway=NULL;
     }
   m_managers.Clear();
   m_ticks.Clear();
   m_suspect_actions.Clear();
  }
//+------------------------------------------------------------------+
//| Load all info                                                    |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::LoadInfo(void)
  {
   CMTStr1024     path;
   CMTStr1024     message;
   uint32_t       line           =0;
   LPCWSTR        current        =NULL;
   Manager*       manager_pointer=NULL;
   int32_t            pos            =0;
   SuspectAction  action_info;
   DealerTick     tick_info;
   Manager        manager_info;
   uint64_t         user,manager;
   CMTStr128      manager_name;
   double         pips,profit,rate;
   CMTStr32       symbol;
   MTAPIRES       ret;
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
      //--- clip the first of the message
      message.Assign(current);
      message.Delete(0,16);
      //--- if it is confirm message
      if((pos=message.Find(L": confirm "))>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- get profit,symbol and pips from the message
         if(!ParseMessage(message,profit,pips,symbol,pos+CMTStr::Len(L": confirm ")))
            continue;
         //--- get group
         if((ret=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
            continue;;
         //--- write group currency
         if((ret=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
            continue;
         //--- converted profit
         profit=profit*rate;
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            continue;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->profit_currency=SMTMath::MoneyAdd(manager_pointer->profit_currency,profit,m_digits);
            manager_pointer->profit_pips   +=(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login          =manager;
            manager_info.profit_currency=SMTMath::MoneyAdd(manager_info.profit_currency,profit,m_digits);
            manager_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- add one more action if it exceeds threshold
         if(m_profit<fabs(profit))
           {
            ZeroMemory(&action_info,sizeof(action_info));
            action_info.login          =manager;
            action_info.profit_currency=SMTMath::MoneyAdd(action_info.profit_currency,profit,m_digits);
            action_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            CMTStr::Copy(action_info.details,message.Str()+pos+2);
            if(!m_suspect_actions.Add(&action_info))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- if it is confirm message (automate)
      if((pos=message.Find(L": request confirmed"))>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUser(user,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- get profit,symbol and pips from the message
         if(!ParseMessageAutomate(message,profit,pips,symbol,pos+CMTStr::Len(L": request confirmed ")))
            continue;
         //--- get group
         if((ret=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
            continue;
         //--- write group currency
         if((ret=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
            continue;
         //--- converted profit
         profit=SMTMath::PriceNormalize(profit*rate,m_digits);
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            continue;
         //--- set manager id
         manager=AUTOMANAGER_LOGIN;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->requests_count++;
            manager_pointer->profit_currency=SMTMath::MoneyAdd(manager_pointer->profit_currency,profit,m_digits);
            manager_pointer->profit_pips   +=(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login=manager;
            manager_info.requests_count++;
            manager_info.profit_currency=SMTMath::MoneyAdd(manager_info.profit_currency,profit,m_digits);
            manager_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- done
         continue;
        }
      //--- if it is request message
      if((pos=message.Find(L": request from "))>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message,true))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
            manager_pointer->requests_count++;
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login          =manager;
            manager_info.requests_count=1;
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- is it login message?
      if((pos=message.Find(L"': login ("))>=0)
        {
         int32_t first_space;
         pos+=CMTStr::Len(L"': login (");
         //--- determine type of connection
         if((first_space=message.Find(L" ",pos))<0)
            continue;
         CMTStr32 type;
         type.Assign(message.Str()+pos,first_space-pos);
         //--- if it is manager connection
         if(type.Compare(L"Manager")==0)
           {
            //--- parse manager from the message
            if(!ParseManager(manager,message))
               continue;
            //--- increment login or add one more manager
            if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
               manager_pointer->connections_count++;
            else
              {
               ZeroMemory(&manager_info,sizeof(manager_info));
               manager_info.login=manager;
               manager_info.connections_count=1;
               GetManagerName(manager,manager_name);
               CMTStr::Copy(manager_info.name,manager_name.Str());
               if(!m_managers.Insert(&manager_info,SortManagerByLogin))
                  return(MT_RET_ERR_MEM);
              }
           }
         continue;
        }
      //--- if it is reject message
      if(message.Find(L": reject for")>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- increment reject or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
            manager_pointer->rejects_count++;
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login=manager;
            manager_info.rejects_count=1;
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- if it is requote message
      if(message.Find(L"': requote")>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- increment requote or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
            manager_pointer->requotes_count++;
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login=manager;
            manager_info.requotes_count=1;
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- if it is requoted message (automate)
      if((pos=message.Find(L": request requoted"))>=0)
        {
         //--- haven't information about profit - count only
         //--- set manager id
         manager=AUTOMANAGER_LOGIN;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->requests_count++;
            manager_pointer->requotes_count++;
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login=manager;
            manager_info.requests_count++;
            manager_info.requotes_count++;
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- done
         continue;
        }
      //--- if it is rejected message (automate)
      if((pos=message.Find(L": request rejected"))>=0)
        {
         //--- haven't information about profit - count only
         //--- set manager id
         manager=AUTOMANAGER_LOGIN;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->requests_count++;
            manager_pointer->rejects_count++;
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login=manager;
            manager_info.requests_count++;
            manager_info.rejects_count++;
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- done
         continue;
        }
      //--- if it is avtivate order message
      if((pos=message.Find(L"activate order"))>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- parse profit,pips and symbol from the avtivate order message
         if(!ParseActivateOrder(message,profit,pips,symbol))
            continue;
         //--- get group
         if((ret=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
            continue;
         //--- write group currency
         if((ret=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
            continue;
         //--- converted profit
         profit=SMTMath::PriceNormalize(profit*rate,m_digits);
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            return(ret);
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->profit_currency=SMTMath::MoneyAdd(manager_pointer->profit_currency,profit,m_digits);
            manager_pointer->profit_pips   +=(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login          =manager;
            manager_info.profit_currency=SMTMath::MoneyAdd(manager_info.profit_currency,profit,m_digits);
            manager_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- add one more action if it exceeds threshold
         if(m_profit<fabs(profit))
           {
            ZeroMemory(&action_info,sizeof(action_info));
            action_info.login          =manager;
            action_info.profit_currency=SMTMath::MoneyAdd(action_info.profit_currency,profit,m_digits);
            action_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            CMTStr::Copy(action_info.details,message.Str()+pos);
            if(!m_suspect_actions.Add(&action_info))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- if it is modify position message
      if((pos=message.Find(L" modify "))>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- parse profit,pips and symbol from the modify position message
         if(!ParseModifyPosition(message,profit,pips,symbol,pos))
            continue;
         //--- get group
         if((ret=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
            continue;
         //--- write group currency
         if((ret=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
            continue;
         //--- converted profit
         profit=profit*rate;
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            continue;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->profit_currency=SMTMath::MoneyAdd(manager_pointer->profit_currency,profit,m_digits);
            manager_pointer->profit_pips   +=(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login          =manager;
            manager_info.profit_currency=SMTMath::MoneyAdd(manager_info.profit_currency,profit,m_digits);
            manager_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- add one more action if it exceeds threshold
         if(m_profit<fabs(profit))
           {
            ZeroMemory(&action_info,sizeof(action_info));
            action_info.login          =manager;
            action_info.profit_currency=SMTMath::MoneyAdd(action_info.profit_currency,profit,m_digits);
            action_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            CMTStr::Copy(action_info.details,message.Str()+pos+1);
            if(!m_suspect_actions.Add(&action_info))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- if it is tick message
      if((pos=message.Find(L": add tick "))>=0)
        {
         double bid,ask;
         INT bid_pips,ask_pips;
         //--- parse tick from the message
         if(!ParseTick(message,symbol,bid,ask,pos))
            continue;
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            return(ret);
         //--- convert into integer
         bid_pips=(INT)SMTMath::PriceNormalize(bid*SMTMath::DecPow(m_symbol->Digits()),0);
         ask_pips=(INT)SMTMath::PriceNormalize(ask*SMTMath::DecPow(m_symbol->Digits()),0);
         //--- if bid or ask exceed threshold
         if(m_pips<bid_pips || m_pips<ask_pips)
           {
            //--- parse manager from the message
            if(!ParseManager(manager,message))
               continue;
            //--- add one more tick
            ZeroMemory(&tick_info,sizeof(tick_info));
            tick_info.login=manager;
            tick_info.bid  =bid_pips;
            tick_info.ask  =ask_pips;
            CMTStr::Copy(tick_info.symbol,symbol.Str());
            if(!m_ticks.Add(&tick_info))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- if it is delete position message
      if((pos=message.Find(L" position deleted ["))>=0)
        {
         //--- parse user and manager from the message
         if(!ParseUserManager(user,manager,message))
            continue;
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- parse profit,pips and symbol from the delete position message 
         if(!ParseDeletedPosition(message,profit,pips,symbol,pos))
            continue;
         //--- get group
         if((ret=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
            continue;
         //--- write group currency
         if((ret=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
            continue;
         //--- converted profit
         profit=SMTMath::PriceNormalize(profit*rate,m_digits);
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            continue;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->profit_currency=SMTMath::MoneyAdd(manager_pointer->profit_currency,profit,m_digits);
            manager_pointer->profit_pips   +=(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login          =manager;
            manager_info.profit_currency=SMTMath::MoneyAdd(manager_info.profit_currency,profit,m_digits);
            manager_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- add one more action if it exceeds threshold
         if(m_profit<fabs(profit))
           {
            ZeroMemory(&action_info,sizeof(action_info));
            action_info.login          =manager;
            action_info.profit_currency=SMTMath::MoneyAdd(action_info.profit_currency,profit,m_digits);
            action_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            CMTStr::Copy(action_info.details,message.Str()+pos+1);
            if(!m_suspect_actions.Add(&action_info))
               return(MT_RET_ERR_MEM);
           }
         continue;
        }
      //--- parse user and manager from the message (for simple buy/sell message)
      if(ParseUserManager(user,manager,message))
        {
         //--- get user
         if((ret=m_api->UserGetLight(user,m_user))!=MT_RET_OK)
            continue;
         //--- check group
         if(!CMTStr::CheckGroupMask(m_groups,m_user->Group()))
            continue;
         //--- calculate first position for parsing
         if((pos=message.Find(L"for \'"))<0)
            continue;
         if((pos=message.Find(L" ",pos+CMTStr::Len(L"for \'")))<0)
            continue;
         //--- parse profit,pips and symbol from buy/sell message
         if(!ParseMessage(message,profit,pips,symbol,pos+1,false))
            continue;
         //--- get group
         if((ret=m_api->GroupGetLight(m_user->Group(),m_group))!=MT_RET_OK)
            continue;
         //--- write group currency
         if((ret=m_api->TradeRateSell(m_group->Currency(),m_currency,rate))!=MT_RET_OK)
            return(ret);
         //--- converted profit
         profit=profit*rate;
         //--- get symbol
         if((ret=m_api->SymbolGetLight(symbol.Str(),m_symbol))!=MT_RET_OK)
            continue;
         //--- increment profit or add one more manager
         if((manager_pointer=m_managers.Search(&manager,SearchManagerByLogin)))
           {
            manager_pointer->profit_currency=SMTMath::MoneyAdd(manager_pointer->profit_currency,profit,m_digits);
            manager_pointer->profit_pips   +=(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
           }
         else
           {
            ZeroMemory(&manager_info,sizeof(manager_info));
            manager_info.login          =manager;
            manager_info.profit_currency=SMTMath::MoneyAdd(manager_info.profit_currency,profit,m_digits);
            manager_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            GetManagerName(manager,manager_name);
            CMTStr::Copy(manager_info.name,manager_name.Str());
            if(!m_managers.Insert(&manager_info,SortManagerByLogin))
               return(MT_RET_ERR_MEM);
           }
         //--- add one more action if it exceeds threshold
         if(m_profit<fabs(profit))
           {
            ZeroMemory(&action_info,sizeof(action_info));
            action_info.login          =manager;
            action_info.profit_currency=SMTMath::MoneyAdd(action_info.profit_currency,profit,m_digits);
            action_info.profit_pips    =(INT)SMTMath::PriceNormalize(pips*SMTMath::DecPow(m_symbol->Digits()),0);
            CMTStr::Copy(action_info.details,message.Str()+pos+1);
            if(!m_suspect_actions.Add(&action_info))
               return(MT_RET_ERR_MEM);
           }
        }
     }
//--- close file
   m_file_reader.Close();
//--- all right
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Parse activate order from the message                            |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseActivateOrder(const CMTStr& message,double& profit,double& pips,CMTStr& symbol)
  {
   CMTStr32 str;
   int32_t      pos;
   double   price,volume,bid_ask,profit_rate;
   bool     buy =false;
   bool     sell=false;
//---
   symbol.Clear();
//--- define type of order
   if((pos=message.Find(L"buy stop"))>=0   ||
      (pos=message.Find(L"buy limit"))>=0)
      buy=true;
   else
     {
      if((pos=message.Find(L"sell stop"))>=0  ||
         (pos=message.Find(L"sell limit"))>=0)
         sell=true;
     }
   if(!buy && !sell)
      return(false);
//--- calculate position for parsing symbol
   pos=message.Find(L" ",pos);
   pos=message.Find(L" ",pos+1);
   pos=message.Find(L" ",pos+1);
//--- parse symbol
   symbol.Assign(message.Str()+pos+1,symbol.Max());
   if(symbol.FindChar(L' ')<=0)
      return(false);
   symbol.Trim(symbol.FindChar(L' '));
//--- parse volume
   if((pos=message.Find(L" as ",pos+1))<0) return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" as "),str.Max());
   if((pos=str.FindChar(L' '))<=0)
      return(false);
   str.Trim(pos);
   if(!(volume=_wtof(str.Str())))
      return(false);
//--- parse confirm price
   if((pos=message.Find(L" at ",pos+1))<0) return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" at "),str.Max());
   if((pos=str.FindChar(L' '))<=0)
      return(false);
   str.Trim(pos);
   if(!(price=_wtof(str.Str())))
      return(false);
//--- parse bid/ask
   if(buy)
     {
      if((pos=message.Find(L" / "))<=0)
         return(false);
      str.Assign(message.Str()+pos+CMTStr::Len(L" / "),str.Max());
      if((pos=str.FindChar(L')'))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=price-bid_ask;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_BUY,SMTMath::VolumeToInt(volume),bid_ask,price,profit,profit_rate)!=MT_RET_OK) return(false);
     }
//--- parse bid
   if(sell)
     {
      if((pos=message.Find(L"("))<=0)
         return(false);
      str.Assign(message.Str()+pos+1,str.Max());
      if((pos=str.FindChar(L' '))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=bid_ask-price;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_SELL,SMTMath::VolumeToInt(volume),bid_ask,price,profit,profit_rate)!=MT_RET_OK) return(false);
     }
//--- parsing complete
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse manager from message                                       |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseManager(uint64_t& manager,const CMTStr& message)
  {
   CMTStr32 login_str;
   int32_t      pos=0;
//---
   manager=0;
//--- parse manager
   if((pos=message.FindChar(L'\''))<=0) return(false);
   login_str.Assign(message.Str()+pos+1,login_str.Max());
   if((pos=login_str.FindChar(L'\''))<=0) return(false);
   login_str.Trim(pos);
   if(!(manager=_wtoi64(login_str.Str()))) return(false);
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse tick from message                                          |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseTick(const CMTStr& message,CMTStr& symbol,double& bid,double& ask,int pos)
  {
   CMTStr32 str;
   double old_bid,old_ask,new_bid,new_ask;
//---
   symbol.Clear();
   bid=0.0;
   ask=0.0;
//---
   pos+=CMTStr::Len(L": add tick ");
//--- parse symbol
   symbol.Assign(message.Str()+pos,symbol.Max());
   if(symbol.FindChar(L' ')<=0)
      return(false);
   symbol.Trim(symbol.FindChar(L' '));
//--- parse new bid
   pos=message.Find(L" ",pos+1);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(new_bid=_wtof(str.Str())))
      return(false);
//--- parse new ask
   if((pos=message.Find(L" / "))<=0)
      return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" / "),str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(new_ask=_wtof(str.Str())))
      return(false);
//--- parse old bid
   if((pos=message.Find(L"("))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(old_bid=_wtof(str.Str())))
      return(false);
//--- parse old_ask
   if((pos=message.Find(L" / ",pos))<=0)
      return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" / "),str.Max());
   if(str.FindChar(L')')<=0)
      return(false);
   str.Trim(str.FindChar(L')'));
   if(!(old_ask=_wtof(str.Str())))
      return(false);
//--- return result
   bid=fabs(old_bid-new_bid);
   ask=fabs(old_ask-new_ask);
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse user and manager from message                              |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseUserManager(uint64_t& user,uint64_t& manager,const CMTStr& message,bool request/*=false*/)
  {
   int32_t      pos=0;
   CMTStr32 login_str;
//---
   user   =0;
   manager=0;
//--- parse user
   if(request)
     {
      if((pos=message.Find(L" from '"))<=0) return(false);
      login_str.Assign(message.Str()+pos+CMTStr::Len(L" from '"),login_str.Max());
     }
   else
     {
      if((pos=message.Find(L" for '"))<=0) return(false);
      login_str.Assign(message.Str()+pos+CMTStr::Len(L" for '"),login_str.Max());
     }
   if((pos=login_str.FindChar(L'\''))<=0) return(false);
   login_str.Trim(pos);
   if(!(user=_wtoi64(login_str.Str()))) return(false);
//--- parse manager
   if((pos=message.FindChar(L'\''))<=0) return(false);
   login_str.Assign(message.Str()+pos+1,login_str.Max());
   if((pos=login_str.FindChar(L'\''))<=0) return(false);
   login_str.Trim(pos);
   if(!(manager=_wtoi64(login_str.Str()))) return(false);
//---
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse users login                                                |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseUser(uint64_t& user,const CMTStr& message)
  {
   CMTStr32 login_str;
   uint32_t pos=0;
//---
   user=0;
//--- parse user
   if((pos=message.FindChar(L'\''))<=0)   return(false);
   login_str.Assign(message.Str()+pos+1,login_str.Max());
   if((pos=login_str.FindChar(L'\''))<=0) return(false);
   login_str.Trim(pos);
   if(!(user=_wtoi64(login_str.Str())))   return(false);
//--- 
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse dealer operation the message                               |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseMessage(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos,bool confirm/*=true*/)
  {
   CMTStr16 str;
   double   volume,bid_ask,profit_rate,price;
   bool     buy =false;
   bool     sell=false;
//---
   pips   =0.0;
   profit =0.0;
   symbol.Clear();
//--- parse type
   if(message.Find(L"buy")==pos)  buy =true;
   if(message.Find(L"sell")==pos) sell=true;
   if(!buy && !sell) return(false);
//--- parse volume
   if((pos=message.Find(L" ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(volume=_wtof(str.Str())))
      return(false);
//--- parse symbol
   if((pos=message.Find(L" ",pos+str.Len()))<=0)
      return(false);
   symbol.Assign(message.Str()+pos+1,symbol.Max());
   if(symbol.FindChar(L' ')<=0)
      return(false);
   symbol.Trim(symbol.FindChar(L' '));
//--- parse confirm price
   if((pos=message.Find(L" at ",pos))<=0)
      return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" at "),str.Max());
   if((pos=str.FindChar(L' '))<=0)
      return(false);
   str.Trim(pos);
   if(!(price=_wtof(str.Str())))
      return(false);
//--- parse ask
   if(buy)
     {
      if((pos=message.Find(L" / "))<=0)
         return(false);
      str.Assign(message.Str()+pos+3,str.Max());
      if((pos=str.FindChar(L')'))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=price-bid_ask;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_BUY,SMTMath::VolumeToInt(volume),bid_ask,price,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parse bid
   if(sell)
     {
      if((pos=message.Find(L"("))<=0)
         return(false);
      if(confirm)
        {
         if((pos=message.Find(L"(",pos+1))<=0)
            return(false);
        }
      str.Assign(message.Str()+pos+1,str.Max());
      if((pos=str.FindChar(L' '))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=bid_ask-price;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_SELL,SMTMath::VolumeToInt(volume),bid_ask,price,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parsing complete
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse message from Server Automate                               |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseMessageAutomate(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos)
  {
   CMTStr16       str;
   double         volume,bid_ask,profit_rate,price;
   int32_t            tmp     =0;
   bool           buy     =false;
   bool           sell    =false;
   bool           is_order=false;
//---
   pips  =0.0;
   profit=0.0;
   symbol.Clear();
//--- checks
   if(message.Find(L"at client price")!=pos &&
      message.Find(L"at market price")!=pos)
      return(false);
//--- parse confirm price
   if((pos=message.Find(L" price ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" price "),str.Max());
   if(str.FindChar(L',')<=0)
      return(false);
   str.Trim(str.FindChar(L','));
   if(!(price=_wtof(str.Str())))
      return(false);
//--- parse type
   if((pos=message.Find(L"(instant "))>0)
      pos+=CMTStr::Len(L"(instant ");
   else if((pos=message.Find(L"(activate order"))>0)
        {
         pos+=CMTStr::Len(L"(activate order");
         is_order=true;
        }
      else if((pos=message.Find(L"(activate "))>0)
            pos+=CMTStr::Len(L"(activate ");
         else if((pos=message.Find(L"(market "))>0)
               pos+=CMTStr::Len(L"(market ");
            else if((pos=message.Find(L"(request"))>0)
                  pos+=CMTStr::Len(L"(request");
               else
                  return(false);
//--- buy or sell?
   if((tmp=message.Find(L"buy"))>=pos)
     {
      pos=tmp;
      buy=true;
     }
   else
      if((tmp=message.Find(L"sell"))>=pos)
        {
         pos=tmp;
         sell=true;
        }
   if(!buy && !sell)
     {
      m_api->LoggerOutString(MTLogAtt,L"not buy not sell");
      return(false);
     }
   if(is_order)
     {
      if((message.Find(L"buy limit")==pos) || (message.Find(L"buy stop")==pos))
         pos+=_countof(L"buy ");
      else if((message.Find(L"sell stop")==pos) || (message.Find(L"sell limit")==pos))
            pos+=_countof(L"sell ");
         else
            return(false);
     }
//--- parse volume
   if((pos=message.Find(L" ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Len()-pos-1);
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(volume=_wtof(str.Str())))
      return(false);
//--- parse symbol
   if((pos=message.Find(L" ",pos+str.Len()))<=0)
      return(false);
   symbol.Assign(message.Str()+pos+1,symbol.Max());
   if(symbol.FindChar(L' ')<=0)
      return(false);
   symbol.Trim(symbol.FindChar(L' '));
//--- find bid/ask block
   if((pos=message.Find(L")("))<=0)
      return(false);
//--- parse ask
   if(buy)
     {
      if((pos=message.Find(L" / ",pos))<=0)
         return(false);
      str.Assign(message.Str()+pos+3,str.Max());
      if((pos=str.FindChar(L')'))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=price-bid_ask;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_BUY,SMTMath::VolumeToInt(volume),bid_ask,price,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parse bid
   if(sell)
     {
      if((pos=message.Find(L")("),pos)<=0)
         return(false);
      str.Assign(message.Str()+pos+CMTStr::Len(L")("),str.Max());
      if((pos=str.FindChar(L' '))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=bid_ask-price;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_SELL,SMTMath::VolumeToInt(volume),bid_ask,price,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parsing complete
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse modify position message                                    |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseModifyPosition(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos)
  {
   CMTStr16 str;
   double   volume,old_price,profit_rate,new_price;
   bool     buy =false;
   bool     sell=false;
//---
   pips   =0.0;
   profit =0.0;
   symbol.Clear();
   pos+=CMTStr::Len(L" modify ");
//--- parse type
   if(message.Find(L"buy")==pos)  buy =true;
   if(message.Find(L"sell")==pos) sell=true;
   if(!buy && !sell) return(false);
//--- parse volume
   if((pos=message.Find(L" ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(volume=_wtof(str.Str())))
      return(false);
//--- parse symbol
   if((pos=message.Find(L" ",pos+str.Len()))<=0)
      return(false);
   symbol.Assign(message.Str()+pos+1,symbol.Max());
   if(symbol.FindChar(L' ')<=0)
      return(false);
   symbol.Trim(symbol.FindChar(L' '));
//--- parse old price
   if((pos=message.Find(L" ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(old_price=_wtof(str.Str())))
      return(false);
//--- parse new price
   if((pos=message.Find(L" -> ",pos))<=0)
      return(false);
   str.Assign(message.Str()+pos+CMTStr::Len(L" -> "),str.Max());
   if((pos=str.FindChar(L' '))<=0)
      return(false);
   str.Trim(pos);
   if(!(new_price=_wtof(str.Str())))
      return(false);
//--- parse ask
   if(buy)
     {
      pips=new_price-old_price;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_BUY,SMTMath::VolumeToInt(volume),old_price,new_price,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parse bid
   if(sell)
     {
      pips=old_price-new_price;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_SELL,SMTMath::VolumeToInt(volume),new_price,old_price,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parsing complete
   return(true);
  }
//+------------------------------------------------------------------+
//| Parse deleted position message                                   |
//+------------------------------------------------------------------+
bool CDailyDealingReport::ParseDeletedPosition(const CMTStr& message,double& profit,double& pips,CMTStr& symbol,int pos)
  {
   CMTStr16 str;
   double   volume,price,profit_rate,bid_ask;
   bool     buy =false;
   bool     sell=false;
//---
   pips   =0.0;
   profit =0.0;
   symbol.Clear();
   pos+=CMTStr::Len(L" position deleted [");
//--- parse type
   if(message.Find(L"buy")==pos)  buy =true;
   if(message.Find(L"sell")==pos) sell=true;
   if(!buy && !sell) return(false);
//--- parse volume
   if((pos=message.Find(L" ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(volume=_wtof(str.Str())))
      return(false);
//--- parse symbol
   if((pos=message.Find(L" ",pos+str.Len()))<=0)
      return(false);
   symbol.Assign(message.Str()+pos+1,symbol.Max());
   if(symbol.FindChar(L' ')<=0)
      return(false);
   symbol.Trim(symbol.FindChar(L' '));
//--- parse price
   if((pos=message.Find(L" ",pos+1))<=0)
      return(false);
   str.Assign(message.Str()+pos+1,str.Max());
   if(str.FindChar(L' ')<=0)
      return(false);
   str.Trim(str.FindChar(L' '));
   if(!(price=_wtof(str.Str())))
      return(false);
//--- parse ask
   if(buy)
     {
      if((pos=message.Find(L" / "))<=0)
         return(false);
      str.Assign(message.Str()+pos+CMTStr::Len(L" / "),str.Max());
      if((pos=str.FindChar(L')'))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=bid_ask-price;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_BUY,SMTMath::VolumeToInt(volume),price,bid_ask,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parse bid
   if(sell)
     {
      if((pos=message.Find(L"("))<=0)
         return(false);
      str.Assign(message.Str()+pos+1,str.Max());
      if((pos=str.FindChar(L' '))<=0)
         return(false);
      str.Trim(pos);
      if(!(bid_ask=_wtof(str.Str())))
         return(false);
      pips=price-bid_ask;
      //---
      if(m_api->TradeProfit(m_user->Group(),symbol.Str(),IMTPosition::POSITION_SELL,SMTMath::VolumeToInt(volume),price,bid_ask,profit,profit_rate)!=MT_RET_OK)
         return(false);
     }
//--- parsing complete
   return(true);
  }
//+------------------------------------------------------------------+
//| Get parameters                                                   |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::GetParameters(void)
  {
   MTAPIRES      res;
   IMTConReport* report   =NULL;
   IMTConParam*  parameter=NULL;
//--- create report interface
   if((report=m_api->ReportCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- create parameter interface
   if((parameter=m_api->ParamCreate())==NULL)
     {
      report->Release();
      return(MT_RET_ERR_MEM);
     }
//--- get report
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
     {
      parameter->Release();
      report->Release();
      return(res);
     }
//--- get currency parameter
   if((res=report->ParameterGet(L"Currency",parameter))!=MT_RET_OK)
     {
      //--- fill parameter manually
      parameter->Name(L"Currency");
      parameter->Type(IMTConParam::TYPE_STRING);
      parameter->Value(DEFAULT_CURRENCY);
     }
   CMTStr::Copy(m_currency,parameter->Value());
//--- get groups parameter
   if((res=report->ParameterGet(L"Groups",parameter))!=MT_RET_OK)
     {
      //--- fill parameter manually
      parameter->Name(L"Groups");
      parameter->Type(IMTConParam::TYPE_STRING);
      parameter->Value(DEFAULT_GROUPS);
     }
   CMTStr::Copy(m_groups,parameter->Value());
//--- get threshold pips parameter
   if((res=report->ParameterGet(L"Dangerous Tick Deviation",parameter))!=MT_RET_OK)
     {
      //--- fill parameter manually
      parameter->Name(L"Dangerous Tick Deviation");
      parameter->Type(IMTConParam::TYPE_INT);
      parameter->ValueInt(_wtoi64(DEFAULT_TICK_PIPS_TRESHOLD));
     }
   m_pips=parameter->ValueInt();
//--- get threshold profit parameter
   if((res=report->ParameterGet(L"Dangerous Profit",parameter))!=MT_RET_OK)
     {
      //--- fill parameter manually
      parameter->Name(L"Dangerous Profit");
      parameter->Type(IMTConParam::TYPE_FLOAT);
      parameter->ValueFloat(_wtof(DEFAULT_PROFIT_TRESHOLD));
     }
   m_profit=parameter->ValueFloat();
//--- get max line parameter
   if((res=report->ParameterGet(L"Max Dangerous Actions Shown",parameter))!=MT_RET_OK)
     {
      //--- fill parameter manually
      parameter->Name(L"Max Dangerous Actions Shown");
      parameter->Type(IMTConParam::TYPE_INT);
      parameter->ValueInt(_wtoi(DEFAULT_MAX_LINE));
     }
   m_max_line=parameter->ValueInt();
   m_digits=SMTMath::MoneyDigits(m_currency);
   report->Release();
   parameter->Release();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Create arrays interfaces                                         |
//+------------------------------------------------------------------+
bool CDailyDealingReport::CreateInterfaces(void)
  {
//--- clear old
   Clear();
//--- creating
   if((m_managers_chart=m_api->ChartCreate())==NULL)
      return(false);
   if((m_user=m_api->UserCreate())==NULL)
      return(false);
   if((m_symbol=m_api->SymbolCreate())==NULL)
      return(false);
   if((m_group=m_api->GroupCreate())==NULL)
      return(false);
   if((m_gateway=m_api->GatewayCreate())==NULL)
      return(false);
//--- all right
   return(true);
  }
//+------------------------------------------------------------------+
//| Get manager name by login                                        |
//+------------------------------------------------------------------+
void CDailyDealingReport::GetManagerName(uint64_t login,CMTStr& name)
  {
   Manager* manager_pointer=NULL;
//--- clear name
   name.Clear();
//--- manager is server automate (have invalid login = 0)
   if(login==AUTOMANAGER_LOGIN)
     {
      name.Assign(AUTOMANAGER_NAME);
      return;
     }
//--- search manager in managers list
   if((manager_pointer=m_managers.Search(&login,SearchManagerByLogin)))
     {
      name.Assign(manager_pointer->name);
      return;
     }
//--- check if manager is human (have valid login)
   if(m_api->UserGetLight(login,m_user)==MT_RET_OK)
     {
      name.Assign(m_user->Name());
      return;
     }
//--- search manager in gateways list
   for(uint32_t i=0;m_api->GatewayNext(i,m_gateway)==MT_RET_OK;i++)
      if(m_gateway->ID()==login)
        {
         name.Assign(m_gateway->Name());
         return;
        }
//--- manager's name is unknown
   name.Assign(UNKNOWN_NAME);
  }
//+------------------------------------------------------------------+
//| Get log file name by date                                        |
//+------------------------------------------------------------------+
bool CDailyDealingReport::GetFileName(CMTStr& path,const int64_t day)
  {
   CMTStr32 name;
   int32_t      pos=-1;
//---
   path.Clear();
   name.Format(L"%04u%02u%02u.log",SMTTime::Year(day),
      SMTTime::Month(day),SMTTime::Day(day));
//--- get path
   if(!GetModuleFileNameW(NULL,path.Buffer(),path.Max())) return(false);
   path.Refresh();
//--- 
   if((pos=path.FindRChar(L'\\'))>0) path.Trim(pos);
   else
      return(false);
   path.Refresh();
//--- get full path to log file
   path.Append(L"\\logs\\");
   path.Append(name);
   return(true);
  }
//+------------------------------------------------------------------+
//| Prepare manager pie chart of requests                            |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::PrepareManagersChart(void)
  {
   IMTReportSeries *series         =NULL;
   uint64_t           others_requests=0;
   double           others_profit  =0;
   MTAPIRES         res;
   CMTStr128        str;
   CMTStr128        money_formated;
//--- checks
   if(m_api==NULL)            return(MT_RET_ERR_PARAMS);
   if(m_managers_chart==NULL) return(MT_RET_ERR_PARAMS);
//--- sort managers
   m_managers.Sort(SortManagerByRequests);
//--- get total requests
   for(uint32_t i=0;i<m_managers.Total();i++)
     {
      others_requests+=m_managers[i].requests_count;
      others_profit=SMTMath::MoneyAdd(others_profit,m_managers[i].profit_currency,m_digits);
     }
//--- prepare chart
   m_managers_chart->Type(IMTReportChart::TYPE_PIE);
   m_managers_chart->Title(L"Top Managers");
//--- setup pie tooltip format
   m_managers_chart->PieceTooltip(L"%DESCRIPTION%<BR>Requests: %VALUE%");
//---
   for(uint32_t i=0;i<MAX_MANAGERS && i<m_managers.Total();i++)
     {
      //--- check managers
      if(!m_managers[i].requests_count)
         continue;
      //--- prepare managers series
      if((series=m_api->ChartCreateSeries())==NULL)
         return(MT_RET_ERR_MEM);
      //---
      series->Type(IMTReportSeries::TYPE_PIECE);
      series->Title(m_managers[i].name);
      series->Color(COLOR_MANAGERS+((30*i)<<8)+i*5);
      others_requests-=m_managers[i].requests_count;
      others_profit  -=m_managers[i].profit_currency;
      if((res=series->ValueAddInt(m_managers[i].requests_count))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
      //--- add description
      SMTFormat::FormatMoney(money_formated,m_managers[i].profit_currency,m_digits);
      str.Format(L"Login: %I64u<BR>Name: %s<BR>Profit: %s",m_managers[i].login,m_managers[i].name,money_formated.Str());
      series->ValueDescription(0,str.Str());
      //--- add series (after adding series will be released by chart)
      if((res=m_managers_chart->SeriesAdd(series))!=MT_RET_OK)
        {
         series->Release();
         return(res);
        }
     }
//--- check existence of others series
   if((m_managers.Total()<MAX_MANAGERS) || (others_requests==0))
      return(MT_RET_OK);
//--- prepare others series
   if((series=m_api->ChartCreateSeries())==NULL)
      return(MT_RET_ERR_MEM);
//---
   series->Type(IMTReportSeries::TYPE_PIECE);
   series->Title(L"Others");
   series->Color(COLOR_OTHERS);
   if((series->ValueAddInt(others_requests))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
   str.Format(L"Others<BR>Profit: %.2f",others_profit);
   series->ValueDescription(0,str.Str());
//--- add series (after adding series will be released by chart)
   if((res=m_managers_chart->SeriesAdd(series))!=MT_RET_OK)
     {
      series->Release();
      return(res);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write all into html                                              |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::WriteAll(void)
  {
   MTAPISTR      tag;
   CMTStr256     str;
   uint32_t      counter;
   MTAPIRES      res   =MT_RET_OK;
   IMTConReport *report=NULL;
//--- create report interface
   if((report=m_api->ReportCreate())==NULL) return(MT_RET_ERR_MEM);
//--- get report interface
   report->Clear();
   if((res=m_api->ReportCurrent(report))!=MT_RET_OK)
      return(res);
//--- use template
   if((res=m_api->HtmlTplLoadResource(IDR_HTML_DAILY_DEALING_REPORT,RT_HTML))!=MT_RET_OK)
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
      //--- print currency
      if(CMTStr::CompareNoCase(tag,L"currency")==0)
        {
         if((res=m_api->HtmlWriteSafe(m_currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
            break;
         continue;
        }
      //--- date
      if(CMTStr::CompareNoCase(tag,L"date")==0)
        {
         str.Format(L"%02u.%02u.%04u",
             SMTTime::Day(m_api->ParamFrom()),
             SMTTime::Month(m_api->ParamFrom()),
             SMTTime::Year(m_api->ParamFrom()));
         if((res=m_api->HtmlWriteString(str.Str()))!=MT_RET_OK)
            break;
         continue;
        }
      //--- managers chart
      if(CMTStr::CompareNoCase(tag,L"managers_chart")==0)
        {
         if((res=m_api->ChartWriteHtml(m_managers_chart))!=MT_RET_OK)
            break;
         continue;
        }
      //--- write managers table
      if(WriteManagers(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write suspect operation table
      if(WriteSuspectActions(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
      //--- write suspect ticks table
      if(WriteSuspectTicks(tag,res,counter))
        {
         if(res!=MT_RET_OK)
            return(res);
         continue;
        }
     }
   report->Release();
//--- checks
   if(res!=MT_RET_REPORT_TEMPLATE_END) return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Write manager table                                              |
//+------------------------------------------------------------------+
bool CDailyDealingReport::WriteManagers(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr128 money_formated;
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
         //--- write descriptions
         if(CMTStr::CompareNoCase(tag,L"login")==0)
           {
            if(m_managers[save_counter].login==AUTOMANAGER_LOGIN)
              {
               if((retcode=m_api->HtmlWriteSafe(L"",IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
                  return(true);
               continue;
              }
            if((retcode=m_api->HtmlWrite(L"%I64u",m_managers[save_counter].login))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print name
         if(CMTStr::CompareNoCase(tag,L"name")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_managers[save_counter].name,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print connections
         if(CMTStr::CompareNoCase(tag,L"connections")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_managers[save_counter].connections_count))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print requests
         if(CMTStr::CompareNoCase(tag,L"requests")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_managers[save_counter].requests_count))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print requotes
         if(CMTStr::CompareNoCase(tag,L"requotes")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_managers[save_counter].requotes_count))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print rejects
         if(CMTStr::CompareNoCase(tag,L"rejects")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_managers[save_counter].rejects_count))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print dealing_currency
         if(CMTStr::CompareNoCase(tag,L"dealing_currency")==0)
           {
            SMTFormat::FormatMoney(money_formated,m_managers[save_counter].profit_currency,m_digits);
            if((retcode=m_api->HtmlWriteSafe(money_formated.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print dealing_pips
         if(CMTStr::CompareNoCase(tag,L"dealing_pips")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64d",m_managers[save_counter].profit_pips))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print currency
         if(CMTStr::CompareNoCase(tag,L"currency")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print the color of background
         if(CMTStr::CompareNoCase(tag,L"profit_loss")==0)
           {
            if(m_managers[save_counter].profit_currency>0)
              {
               if((retcode=m_api->HtmlWrite(L"class=\"profit\""))!=MT_RET_OK)
                  return(true);
              }
            if(m_managers[save_counter].profit_currency<0)
              {
               if((retcode=m_api->HtmlWrite(L"class=\"loss\""))!=MT_RET_OK)
                  return(true);
              }
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
//| Write suspect operation table                                    |
//+------------------------------------------------------------------+
bool CDailyDealingReport::WriteSuspectActions(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr128 money_formated,manager_name;
//--- if no messages
   if(CMTStr::CompareNoCase(tag,L"no_action")==0 && counter==0 && m_suspect_actions.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- all messages
   while(CMTStr::CompareNoCase(tag,L"action")==0 && counter<m_suspect_actions.Total() && (m_max_line!=0 ? counter<m_max_line:true) )
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      uint32_t save_counter=counter;
      //--- process tags inside table
      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- write descriptions
         if(CMTStr::CompareNoCase(tag,L"manager")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_suspect_actions[save_counter].login))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print name
         if(CMTStr::CompareNoCase(tag,L"name")==0)
           {
            GetManagerName(m_suspect_actions[save_counter].login,manager_name);
            if((retcode=m_api->HtmlWriteSafe(manager_name.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print connections
         if(CMTStr::CompareNoCase(tag,L"details")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_suspect_actions[save_counter].details,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print dealing_currency
         if(CMTStr::CompareNoCase(tag,L"dealing_currency")==0)
           {
            money_formated.Clear();
            SMTFormat::FormatMoney(money_formated,m_suspect_actions[save_counter].profit_currency,m_digits);
            if((retcode=m_api->HtmlWriteString(money_formated.Str()))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print dealing_pips
         if(CMTStr::CompareNoCase(tag,L"dealing_pips")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64d",m_suspect_actions[save_counter].profit_pips))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print currency
         if(CMTStr::CompareNoCase(tag,L"currency")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_currency,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print the color of background
         if(CMTStr::CompareNoCase(tag,L"profit_loss")==0)
           {
            if(m_suspect_actions[save_counter].profit_currency>0)
              {
               if((retcode=m_api->HtmlWrite(L"class=\"profit\""))!=MT_RET_OK)
                  return(true);
              }
            if(m_suspect_actions[save_counter].profit_currency<0)
              {
               if((retcode=m_api->HtmlWrite(L"class=\"loss\""))!=MT_RET_OK)
                  return(true);
              }
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
//| Write suspect ticks table                                        |
//+------------------------------------------------------------------+
bool CDailyDealingReport::WriteSuspectTicks(MTAPISTR& tag,MTAPIRES& retcode,uint32_t& counter)
  {
   CMTStr128 manager_name;
//--- if no messages
   if(CMTStr::CompareNoCase(tag,L"no_ticks")==0 && counter==0 && m_ticks.Total()==0)
     {
      retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE);
      return(true);
     }
//--- all messages
   while(CMTStr::CompareNoCase(tag,L"ticks")==0 && counter<m_ticks.Total())
     {
      if((retcode=m_api->HtmlTplProcess(IMTReportAPI::TPL_PROCESS_NONE))!=MT_RET_OK)
         return(true);
      //--- process tags inside table
      uint32_t save_counter=counter;

      while((retcode=m_api->HtmlTplNext(tag,&counter))==MT_RET_OK)
        {
         //--- write descriptions
         if(CMTStr::CompareNoCase(tag,L"manager")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_ticks[save_counter].login))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print name
         if(CMTStr::CompareNoCase(tag,L"name")==0)
           {
            GetManagerName(m_ticks[save_counter].login,manager_name);
            if((retcode=m_api->HtmlWriteSafe(manager_name.Str(),IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print symbol
         if(CMTStr::CompareNoCase(tag,L"symbol")==0)
           {
            if((retcode=m_api->HtmlWriteSafe(m_ticks[save_counter].symbol,IMTReportAPI::HTML_SAFE_USENOBSP))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print bid deviation
         if(CMTStr::CompareNoCase(tag,L"bid_deviation")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_ticks[save_counter].bid))!=MT_RET_OK)
               return(true);
            continue;
           }
         //--- print ask deviation
         if(CMTStr::CompareNoCase(tag,L"ask_deviation")==0)
           {
            if((retcode=m_api->HtmlWrite(L"%I64u",m_ticks[save_counter].ask))!=MT_RET_OK)
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
MTAPIRES CDailyDealingReport::GenerateDashboard(void)
  {
//--- generate dashboard report
   MTAPIRES res=GenerateDashboardManager();
   if(res!=MT_RET_OK)
      return(res);
//--- generate dashboard suspect action of dealer
   if((res=GenerateDashboardAction())!=MT_RET_OK)
      return(res);
//--- generate dashboard ticks from dealer
   return(GenerateDashboardTick());
  }
//+------------------------------------------------------------------+
//| Generate dashboard report                                        |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::GenerateDashboardManager(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- sort managers
   m_managers.Sort(SortManagerByRequests);
//--- create dataset
   MTAPIRES res=MT_RET_OK;
   IMTDataset *data=ReportColumn::DatasetFromArray(*m_api,res,s_columns_manager,_countof(s_columns_manager),m_managers);
   if(data==NULL)
      return(res);
//--- add manager charts
   if((res=AddManagersChart(data,3,0))!=MT_RET_OK)
      return(res);
   if((res=AddManagersChart(data,6,32))!=MT_RET_OK)
      return(res);
//--- add manager table
   return(ReportColumn::TableAdd(*m_api,res,data,L"Managers") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard suspect action of dealer                      |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::GenerateDashboardAction(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   MTAPIRES res=MT_RET_OK;
   IMTDataset *data=ReportColumn::DatasetFromArray(*m_api,res,s_columns_action,_countof(s_columns_action),m_suspect_actions);
   if(data==NULL)
      return(res);
//--- add action table
   return(ReportColumn::TableAdd(*m_api,res,data,L"Suspect Actions of Dealer") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Generate dashboard ticks from dealer                             |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::GenerateDashboardTick(void)
  {
//--- checks
   if(m_api==NULL)
      return(MT_RET_ERR_PARAMS);
//--- create dataset
   MTAPIRES res=MT_RET_OK;
   IMTDataset *data=ReportColumn::DatasetFromArray(*m_api,res,s_columns_tick,_countof(s_columns_tick),m_ticks);
   if(data==NULL)
      return(res);
//--- add tick table
   return(ReportColumn::TableAdd(*m_api,res,data,L"Ticks from Dealer") ? MT_RET_OK : res);
  }
//+------------------------------------------------------------------+
//| Add Managers Chart                                               |
//+------------------------------------------------------------------+
MTAPIRES CDailyDealingReport::AddManagersChart(IMTDataset *data,uint32_t column,uint32_t left)
  {
//--- checks
   if(m_api==NULL || data==NULL)
      return(MT_RET_ERR_PARAMS);
//--- add widget
   IMTReportDashboardWidget *chart=m_api->DashboardWidgetAppend();
   if(!chart)
      return(MT_RET_ERR_MEM);
//--- prepare chart
   MTAPIRES res=chart->Type(IMTReportDashboardWidget::WIDGET_TYPE_CHART_PIE);
   if(res!=MT_RET_OK)
      return(res);
//--- set title
   CMTStr128 title(L"Top Managers ");
   title.Append(s_columns_manager[column].name);
   if((res=chart->Title(title.Str()))!=MT_RET_OK)
      return(res);
//--- set left
   if((res=chart->Left(left))!=MT_RET_OK)
      return(res);
//--- set top
   if((res=chart->Top(0))!=MT_RET_OK)
      return(res);
//--- set width
   if((res=chart->Width(32))!=MT_RET_OK)
      return(res);
//--- add dataset
   if((res=chart->Data(data))!=MT_RET_OK)
      return(res);
//--- set title column
   if((res=chart->DataColumnTitle(2))!=MT_RET_OK)
      return(res);
//--- add data column
   if((res=chart->DataColumnAdd(s_columns_manager[column].id))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Sort manager by login                                            |
//+------------------------------------------------------------------+
int32_t CDailyDealingReport::SortManagerByLogin(const void *left,const void *right)
  {
   Manager* lft=(Manager*)left;
   Manager* rgh=(Manager*)right;
//---
   if(lft->login>rgh->login) return(1);
   if(lft->login<rgh->login) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Search manager by login                                          |
//+------------------------------------------------------------------+
int32_t CDailyDealingReport::SearchManagerByLogin(const void *left,const void *right)
  {
   uint64_t*  lft=(uint64_t*)left;
   Manager* rgh=(Manager*)right;
//---
   if((*lft)>rgh->login) return(1);
   if((*lft)<rgh->login) return(-1);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort managers action by requests                                 |
//+------------------------------------------------------------------+
int32_t CDailyDealingReport::SortManagerByRequests(const void *left,const void *right)
  {
   Manager* lft=(Manager*)left;
   Manager* rgt=(Manager*)right;
//--- 
   if(lft->requests_count<rgt->requests_count) return(-1);
   if(lft->requests_count>rgt->requests_count) return(1);
//--- 
   return(0);
  }
//+------------------------------------------------------------------+
//| Sort suspect action by pips                                      |
//+------------------------------------------------------------------+
int32_t CDailyDealingReport::SortActionByProfit(const void *left,const void *right)
  {
   SuspectAction* lft=(SuspectAction*)left;
   SuspectAction* rgh=(SuspectAction*)right;
//---
   if(fabs(lft->profit_currency)>fabs(rgh->profit_currency)) return(-1);
   if(fabs(lft->profit_currency)<fabs(rgh->profit_currency)) return(1);
//---
   return(0);
  }
//+------------------------------------------------------------------+