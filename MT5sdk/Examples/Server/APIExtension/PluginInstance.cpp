//+------------------------------------------------------------------+
//|                                       MetaTrader 5 API Extension |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL)
  {
  }
//+------------------------------------------------------------------+
//| Distructor                                                       |
//+------------------------------------------------------------------+
CPluginInstance::~CPluginInstance(void)
  {
   Stop();
  }
//+------------------------------------------------------------------+
//| Plugin release                                                   |
//+------------------------------------------------------------------+
void CPluginInstance::Release(void)
  {
   delete this;
  }
//+------------------------------------------------------------------+
//| Plugin start                                                     |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Start(IMTServerAPI *api)
  {
//--- check pointer
   if(!api)
      return(MT_RET_ERR_PARAMS);
//--- save pointer to Server API interface
   m_api=api;
   MTAPIRES retcode;
//--- subscribe
   if((retcode=m_api->CustomSubscribe(this))!=MT_RET_OK)
      m_api->LoggerOut(MTLogErr,L"CustomSubscribe failed [%u]",retcode);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Stop Plugin                                                      |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
   if(m_api)
     {
      //--- unsubscribe from all events
      m_api->CustomUnsubscribe(this);
      //--- clear Server API pointer
      m_api=NULL;
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get total open positions in group                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::PositionGetTotal(const MTAPISTR group,uint32_t &total)
  {
   MTAPIRES    retcode=MT_RET_ERROR;
   LoginArray  logins;
//--- clear in-out params
   total=0;
//--- check in-params
   if(group[0]==L'\0')
      return(MT_RET_ERR_PARAMS);
//--- get logins array
   if((retcode=UserLogins(group,logins))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"UserLogins failed [%u]",retcode);
      return(retcode);
     }
//--- if group is empty - all done
   if(logins.Total()==0)
      return(MT_RET_OK);
//--- else start calculating
//--- create array of position      
   IMTPositionArray* positions=m_api->PositionCreateArray();
   if(positions==NULL)
     {
      m_api->LoggerOutString(MTLogErr,L"PositionCreateArray failed");
      return(MT_RET_ERR_MEM);
     }
//--- get count of open positions
   for(uint32_t i=0;i<logins.Total();i++)
     {
      //--- Clear array of position
      positions->Clear();
      if((retcode=m_api->PositionGet(logins[i],positions))!= MT_RET_OK)
        {
         //--- free mem and exit
         positions->Release();
         return(retcode);
        }
      //--- if all right, accumulate total positions
      total+=positions->Total();
     }
//--- free mem
   positions->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get total open orders in group                                   |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::OrdersGetTotal(const MTAPISTR group,uint32_t &total)
  {
   MTAPIRES    retcode=MT_RET_ERROR;
   LoginArray  logins;
//--- clear in-out params
   total=0;
//--- check in-params
   if(group[0]==L'\0')
      return(MT_RET_ERR_PARAMS);
//--- get logins array
   if((retcode=UserLogins(group,logins))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"UserLogins failed [%u]",retcode);
      return(retcode);
     }
//--- if group is empty - all done
   if(logins.Total()==0)
      return(MT_RET_OK);
//--- else start calculating
//--- create array of orders      
   IMTOrderArray* orders=m_api->OrderCreateArray();
   if(orders==NULL)
     {
      //--- release mem
      m_api->LoggerOutString(MTLogErr,L"PositionCreateArray failed");
      return(MT_RET_ERR_MEM);
     }
//--- get count of open orders
   for(uint32_t i=0;i<logins.Total();i++)
     {
      //--- Clear array of orders
      orders->Clear();
      if((retcode=m_api->OrderGet(logins[i],orders))!= MT_RET_OK)
        {
         //--- free mem and exit
         orders->Release();
         return(retcode);
        }
      //--- if all right, accumulate total orders
      total+=orders->Total();
     }
//--- free mem
   orders->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get total users in group mask                                    |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::UsersGetTotal(const MTAPISTR group_mask,uint32_t &total)
  {
   MTAPIRES   retcode=MT_RET_ERROR;
   LoginArray logins;
//--- clear in-out val
   total=0;
//--- check input params
   if(group_mask[0]==L'\0')
      return(MT_RET_ERR_PARAMS);
   if((retcode=UserLogins(group_mask,logins))!=MT_RET_OK)
      return(retcode);
   total=logins.Total();
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Custom Subscribe Hook                                            |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookManagerCommand(const uint64_t         session,
                                             LPCWSTR              ip,
                                             const IMTConManager *manager,
                                             IMTByteStream       *indata,
                                             IMTByteStream       *outdata)
  {
   MTAPIRES retcode   =MT_RET_ERROR;
   uint32_t command_id=0;
   double   rate      =0;
   uint32_t count     =0;
   char     prefix[6] ={0};
   MTAPISTR base={0},currency={0},group={0};
//--- check input params
   if(manager==NULL || indata==NULL || outdata==NULL)
      return(MT_RET_ERR_PARAMS);
//--- check plugin prefix
   if((retcode=indata->Read(prefix,sizeof(prefix)))!=MT_RET_OK || memcmp(prefix,"APIEXT",sizeof(prefix))!=0)
      return(MT_RET_OK_NONE);
//--- get command from stream
   if((retcode=indata->ReadUInt(command_id))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"ReadUInt from stream failed [%u]",retcode);
      return(retcode);
     }
//--- do command by its id
   switch(command_id)
     {
      //--- get rate of buy
      case COMMAND_RATE_GET_BUY:
         //--- get params from stream
         if(((retcode=indata->ReadStr(base))!=MT_RET_OK) ||
            ((retcode=indata->ReadStr(currency))!=MT_RET_OK))
           {
            m_api->LoggerOut(MTLogErr,L"Read from stream failed [%u]",retcode);
            return(retcode);
           }
         //--- calculate BUY rate
         if((retcode=m_api->TradeRateBuy(base,currency,rate))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"TradeRateBuy failed [%u]",retcode);
            return(retcode);
           }
         //--- push out result
         if((retcode=outdata->AddDouble(rate))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"AddDouble to stream failed [%u]",retcode);
            return(retcode);
           }
         break;
         //--- get rate of sell
      case COMMAND_RATE_GET_SELL:
         //--- get params from stream
         if(((retcode=indata->ReadStr(base))!=MT_RET_OK) ||
            ((retcode=indata->ReadStr(currency))!=MT_RET_OK))
           {
            m_api->LoggerOut(MTLogErr,L"ReadStr from stream failed [%u]",retcode);
            return(retcode);
           }
         //--- calculate SELL rate
         if((retcode=m_api->TradeRateSell(base,currency,rate))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"TradeRateSell failed [%u]",retcode);
            return(retcode);
           }
         //--- push out result
         if((retcode=outdata->AddDouble(rate))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"AddDouble to stream failed [%u]",retcode);
            return(retcode);
           }
         break;
         //--- get total open position in the group of users
      case COMMAND_POSITIONS_TOTAL:
         //--- get params from stream
         if((retcode=indata->ReadStr(group))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"ReadStr from stream failed [%u]",retcode);
            return(retcode);
           }
         //--- calculate
         if((retcode=PositionGetTotal(group,count))!=MT_RET_OK)
            return(retcode);
         //--- push out result
         if((retcode=outdata->AddUInt(count))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"AddUInt to stream failed [%u]",retcode);
            return(retcode);
           }
         break;
         //--- get total open orders in the group of users
      case COMMAND_ORDERS_TOTAL:
         //--- get params from stream
         if((retcode=indata->ReadStr(group))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"ReadStr from stream failed [%u]",retcode);
            return(retcode);
           }
         //--- calculate
         if((retcode=OrdersGetTotal(group,count))!=MT_RET_OK)
            return(retcode);
         //--- push out result
         if((retcode=outdata->AddUInt(count))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"AddUInt to stream failed [%u]",retcode);
            return(retcode);
           }
         break;
         //--- get total users in the group
      case COMMAND_USERS_TOTAL:
         //--- get params from stream
         if((retcode=indata->ReadStr(group))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"ReadStr from stram failed [%u]",retcode);
            return(retcode);
           }
         //--- calculate
         if((retcode=UsersGetTotal(group,count))!=MT_RET_OK)
            return(retcode);
         //--- push out result
         if((retcode=outdata->AddUInt(count))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"AddUInt to stream failed [%u]",retcode);
            return(retcode);
           }
         break;
         //--- Unknown EnCommand
      default:
         m_api->LoggerOut(MTLogErr,L"Invalid command [%u]",command_id);
         return(MT_RET_ERROR);
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| HookWebAPICommand                                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookWebAPICommand(const uint64_t         session,
                                            LPCWSTR              ip,
                                            const IMTConManager *manager,
                                            LPCWSTR              command,
                                            IMTByteStream       *indata,
                                            IMTByteStream       *outdata)
  {
   MTAPIRES retcode=MT_RET_ERROR;
   uint32_t cmd_id =0;
   MTAPISTR cmd={0},name={0};
//--- ret vals of command functions
   double   rate=0;
   uint32_t count=0;
//--- params for command functions 
   MTAPISTR base={0},currency={0},group={0};
//--- check input params
   if(command==NULL || indata==NULL || outdata==NULL)
      return(MT_RET_ERR_PARAMS);
//--- read command from stream
   if((retcode=indata->WebReadCommand(cmd))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"WebReadCommand failed [%u]",retcode);
      return(retcode);
     }
//--- if it's not mine command - do nothing   
   if((cmd_id=CommandGetId(cmd))==0) return(MT_RET_OK_NONE);
//--- read params
   while((retcode=indata->WebReadParamName(name))==MT_RET_OK)
     {
      //--- read base param
      if(CMTStr::Compare(name,L"base")==0)
        {
         if((retcode=indata->WebReadParamStr(base))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"WebReadParamStr failed [%u]",retcode);
            return(retcode);
           }
         continue;
        }
      //--- read currency param  
      if(CMTStr::Compare(name,L"currency")==0)
        {
         if((retcode=indata->WebReadParamStr(currency))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"WebReadParamStr failed [%u]",retcode);
            return(retcode);
           }
         continue;
        }
      //--- read group param
      if(CMTStr::Compare(name,L"group")==0)
        {
         if((retcode=indata->WebReadParamStr(group))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"WebReadParamStr failed [%u]",retcode);
            return(retcode);
           }
         continue;
        }
      //--- command not mine  
      if((retcode=indata->WebReadParamSkip())!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr,L"WebReadParamSkip failed [%u]",retcode);
         return(retcode);
        }
     }
//--- do command buy cmd_id
   switch(cmd_id)
     {
      case COMMAND_RATE_GET_BUY:
      case COMMAND_RATE_GET_BUY_JSON:
         //--- calculate BUY rate
         if((retcode=m_api->TradeRateBuy(base,currency,rate))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"TradeRateBuy failed [%u]",retcode);
            return(retcode);
           }
         //--- push out result to stream
         if(cmd_id!=COMMAND_RATE_GET_BUY_JSON)
           {
            //--- RAW-mode
            if((retcode=outdata->WebAddParamDouble(L"RATE",rate,RATE_DIGITS))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"WebAddParamDouble failed [%u]",retcode);
               return(retcode);
              }
           }
         else
           {
            //--- JSON-mode
            CMTStr128 buffer;
            buffer.Format(L"{\"RATE\":%f}",rate);
            if((retcode=outdata->AddStr(buffer.Str()))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"AddStr failed [%u]",retcode);
               return(retcode);
              }
           }
         break;
      case COMMAND_RATE_GET_SELL:
      case COMMAND_RATE_GET_SELL_JSON:
         //--- calculate SELL rate
         if((retcode=m_api->TradeRateSell(base,currency,rate))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"TradeRateSell failed [%u]",retcode);
            return(retcode);
           }
         //--- push out result to stream
         if(cmd_id!=COMMAND_RATE_GET_SELL_JSON)
           {
            //--- RAW-mode
            if((retcode=outdata->WebAddParamDouble(L"RATE",rate,RATE_DIGITS))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"WebAddParamDouble failed [%u]",retcode);
               return(retcode);
              }
           }
         else
           {
            //--- JSON-mode
            CMTStr128 buffer;
            buffer.Format(L"{\"RATE\":%f}",rate);
            if((retcode=outdata->AddStr(buffer.Str()))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"AddStr failed [%u]",retcode);
               return(retcode);
              }
           }
         break;
      case COMMAND_POSITIONS_TOTAL:
      case COMMAND_POSITIONS_TOTAL_JSON:
         //--- calculate total open positions
         if((retcode=PositionGetTotal(group,count))!=MT_RET_OK)
            return(retcode);
         //--- push out result to stream
         if(cmd_id!=COMMAND_POSITIONS_TOTAL_JSON)
           {
            //--- RAW-mode
            if((retcode=outdata->WebAddParamUInt(L"TOTAL",count))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"WebAddParamUInt failed [%u]",retcode);
               return(retcode);
              }
           }
         else
           {
            //--- JSON-mode
            CMTStr128 buffer;
            buffer.Format(L"{\"TOTAL\":%u}",count);
            if((retcode=outdata->AddStr(buffer.Str()))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"AddStr failed [%u]",retcode);
               return(retcode);
              }
           }
         break;
      case COMMAND_ORDERS_TOTAL:
      case COMMAND_ORDERS_TOTAL_JSON:
         //--- calculate total open orders
         if((retcode=OrdersGetTotal(group,count))!=MT_RET_OK)
            return(retcode);
         //--- push out result to stream            
         if(cmd_id!=COMMAND_ORDERS_TOTAL_JSON)
           {
            //--- RAW-mode
            if((retcode=outdata->WebAddParamUInt(L"TOTAL",count))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"WebAddParamUInt failed [%u]",retcode);
               return(retcode);
              }
           }
         else
           {
            //--- JSON-mode
            CMTStr128 buffer;
            buffer.Format(L"{\"TOTAL\":%u}",count);
            if((retcode=outdata->AddStr(buffer.Str()))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"AddStr failed [%u]",retcode);
               return(retcode);
              }
           }
         break;
      case COMMAND_USERS_TOTAL:
      case COMMAND_USERS_TOTAL_JSON:
         //--- calculate total users in groups by mask
         if((retcode=UsersGetTotal(group,count))!=MT_RET_OK)
            return(retcode);
         //--- push out result to stream
         if(cmd_id!=COMMAND_USERS_TOTAL_JSON)
           {
            //--- RAW-mode
            if((retcode=outdata->WebAddParamUInt(L"TOTAL",count))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"WebAddParamUInt failed [%u]",retcode);
               return(retcode);
              }
           }
         else
           {
            //--- JSON-mode
            CMTStr128 buffer;
            buffer.Format(L"{\"TOTAL\":%u}",count);
            if((retcode=outdata->AddStr(buffer.Str()))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"AddStr failed [%u]",retcode);
               return(retcode);
              }
           }
         break;
      default:
         m_api->LoggerOut(MTLogErr,L"unknown command id [%u]",cmd_id);
         return(MT_RET_ERR_PARAMS);
         break;
     }
//--- all done ok
   if((retcode=outdata->WebAddParamFinalize())!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"WebAddParamFinalize failed [%u]",retcode);
      return(retcode);
     }
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Get command id by its string name. New webcmd add here.          |
//+------------------------------------------------------------------+
uint32_t CPluginInstance::CommandGetId(LPCWSTR command)
  {
//--- Get rate buy command  
   if(CMTStr::Compare(command,L"APIEXT_RATE_BUY")==0)
      return(COMMAND_RATE_GET_BUY);
//--- get rate sell command      
   if(CMTStr::Compare(command,L"APIEXT_RATE_SELL")==0)
      return(COMMAND_RATE_GET_SELL);
//--- get total opened positions command      
   if(CMTStr::Compare(command,L"APIEXT_TOTAL_POSITIONS")==0)
      return(COMMAND_POSITIONS_TOTAL);
//--- get total opened orders command
   if(CMTStr::Compare(command,L"APIEXT_TOTAL_ORDERS")==0)
      return(COMMAND_ORDERS_TOTAL);
//--- get total users in groups like mask      
   if(CMTStr::Compare(command,L"APIEXT_TOTAL_USERS")==0)
      return(COMMAND_USERS_TOTAL);
//--- Get rate buy command (JSON mode)
   if(CMTStr::Compare(command,L"APIEXT_RATE_BUY_JSON")==0)
      return(COMMAND_RATE_GET_BUY_JSON);
//--- get rate sell command (JSON mode)
   if(CMTStr::Compare(command,L"APIEXT_RATE_SELL_JSON")==0)
      return(COMMAND_RATE_GET_SELL_JSON);
//--- get total opened positions command (JSON mode)
   if(CMTStr::Compare(command,L"APIEXT_TOTAL_POSITIONS_JSON")==0)
      return(COMMAND_POSITIONS_TOTAL_JSON);
//--- get total opened orders command (JSON mode)
   if(CMTStr::Compare(command,L"APIEXT_TOTAL_ORDERS_JSON")==0)
      return(COMMAND_ORDERS_TOTAL_JSON);
//--- get total users in groups like mask (JSON mode)
   if(CMTStr::Compare(command,L"APIEXT_TOTAL_USERS_JSON")==0)
      return(COMMAND_USERS_TOTAL_JSON);
//--- this command isn't mine      
   return(0);
  }
//+------------------------------------------------------------------+
//| Get all user logins in the groups like group-mask                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::UserLogins(const MTAPISTR group_mask,LoginArray &logins)
  {
   uint64_t     *users      =NULL;
   uint32_t    users_total=0;
   MTAPIRES    retcode    =MT_RET_ERROR;
   LPCWSTR     group_name =NULL;
//--- clean logins array
   logins.Clear();
//--- check input params
   if(group_mask[0]==L'\0')
      return(MT_RET_ERR_PARAMS);
//--- create group var (must be release)
   IMTConGroup *group=m_api->GroupCreate();
   if(group==NULL)
     {
      m_api->LoggerOut(MTLogErr,L"GroupCreate failed [%u]",retcode);
      return(retcode);
     }
//--- check all groups in the loop
   for(uint32_t pos=0;m_api->GroupNext(pos,group)==MT_RET_OK;pos++)
     {
      if((group_name=group->Group())==NULL)
        {
         //--- release and exit
         group->Release();
         m_api->LoggerOutString(MTLogErr,L"Group return NULL");
         return(MT_RET_ERROR);
        }
      if(CMTStr::CheckGroupMask(group_mask,group_name))
        {
         //--- good group name. get all logins
         if((retcode=m_api->UserLogins(group_name,users,users_total))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"UserLogins failed [%u]",retcode);
            //--- release and exit
            group->Release();
            return(retcode);
           }
         //--- save logins
         logins.Add(users,users_total);
         //--- release static logins array
         if(users!=NULL)
           {
            m_api->Free(users);
            users=NULL;
           }
        }
     }
//--- release
   group->Release();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+