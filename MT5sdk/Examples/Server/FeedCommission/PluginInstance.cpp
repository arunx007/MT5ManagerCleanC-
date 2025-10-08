//+------------------------------------------------------------------+
//|                                           Feed Commission Plugin |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "PluginInstance.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CPluginInstance::CPluginInstance(void) : m_api(NULL),m_config(NULL),m_cost(0),
                                         m_deposit_max(0),m_overturn_max(0),m_skip_disabled(false),
                                         m_deals(NULL),m_group(NULL),m_group_symbol(NULL),m_user(NULL),
                                         m_symbol(NULL)
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
   MTAPIRES res    =MT_RET_OK;
//--- 
   Stop();
//--- check parameters
   if(!api)
      return(MT_RET_ERR_PARAMS);
   m_api=api;
//--- create plugin config
   if((m_config=m_api->PluginCreate())==NULL)
      return(MT_RET_ERR_MEM);
//--- create interfaces
//--- group
   if(!(m_group=m_api->GroupCreate()))
     {
      m_api->LoggerOutString(MTLogErr,L"GroupCreate failed");
      return(MT_RET_ERR_MEM);
     }
//--- group symbol
   if(!(m_group_symbol=m_api->GroupSymbolCreate()))
     {
      m_api->LoggerOutString(MTLogErr,L"GroupSymbolCreate failed");
      return(MT_RET_ERR_MEM);
     }
//--- user
   if(!(m_user=m_api->UserCreate()))
     {
      m_api->LoggerOutString(MTLogErr,L"UserCreateAccount failed");
      return(MT_RET_ERR_MEM);
     }
//--- deals
   if(!(m_deals=m_api->DealCreateArray()))
     {
      m_api->LoggerOutString(MTLogErr,L"DealCreateArray failed");
      return(MT_RET_ERR_MEM);
     }
//--- deal symbol
   if(!(m_symbol=m_api->SymbolCreate()))
     {
      m_api->LoggerOutString(MTLogErr,L"SymbolCreate failed");
      return(MT_RET_ERR_MEM);
     }
//--- read parameters
   if((res=ParametersRead())!=MT_RET_OK)
      return(res);
//--- subscribe to plugin config updates
   if((res=m_api->PluginSubscribe(this))!=MT_RET_OK)
      return(res);
//--- subscribe to trade's end of day
   if((res=m_api->TradeSubscribeEOD(this))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"subscribe to end of day failed [%u]",res);
      return(res);
     }
//--- subscribe to trade's end of day
   if((res=m_api->UserSubscribe(this))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"subscribe to users failed [%u]",res);
      return(res);
     }

//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Plugin stop notification function                                |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::Stop(void)
  {
   m_sync.Lock();
//--- unsubscribe
   if(m_api)
     {
      m_api->PluginUnsubscribe(this);
      m_api->TradeUnsubscribeEOD(this);
      m_api->UserUnsubscribe(this);
     }
//--- clear all parameters
   m_symbols.Clear();
   m_group_mask.Clear();
   m_currency.Clear();
//--- 
   m_cost           =0;
   m_deposit_max    =0;
   m_overturn_max   =0;
   m_skip_disabled=false;
//--- delete interfaces
   if(m_deals)        { m_deals->Release();        m_deals       =NULL; }
   if(m_group)        { m_group->Release();        m_group       =NULL; }
   if(m_group_symbol) { m_group_symbol->Release(); m_group_symbol=NULL; }
   if(m_user)         { m_user->Release();         m_user        =NULL; }
   if(m_symbol)       { m_symbol->Release();       m_symbol      =NULL; }
   if(m_config)       { m_config->Release();       m_config      =NULL; }
//--- reset api
   m_api=NULL;
//--- 
   m_sync.Unlock();
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void CPluginInstance::OnUserLogin(LPCWSTR /*ip*/,const IMTUser* user,const uint32_t /*type*/)
  {
   m_api->LoggerOut(MTLogOK,L">>>>>>>>>>>>>> LOGIN: %I64u",user->Login());
  }

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::HookUserLogin(LPCWSTR /*ip*/,const IMTUser* user,const uint32_t /*type*/)
  {
   if(rand()%2)
     {
      m_api->LoggerOut(MTLogOK,L">>>>>>>>>>>>>> ALLOW: %I64u",user->Login());
      return(MT_RET_OK);
     }
   else
     {
      m_api->LoggerOut(MTLogOK,L">>>>>>>>>>>>>> DENIED: %I64u",user->Login());
      return(MT_RET_ERR_DUPLICATE);
     }
  }

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void CPluginInstance::OnUserLogout(LPCWSTR /*ip*/,const IMTUser* user,const uint32_t /*type*/)
  {
   m_api->LoggerOut(MTLogOK,L"<<<<<<<<<<< LOGOUT: %I64u",user->Login());
  }
//+------------------------------------------------------------------+
//| Plugin parameters read function                                  |
//+------------------------------------------------------------------+
MTAPIRES CPluginInstance::ParametersRead(void)
  {
   MTAPIRES     res    =MT_RET_OK;
   IMTConParam* param  =NULL;
   CMTStr128    tmp;
//--- check pointers
   if(!m_api || !m_config || !m_symbol)
      return(MT_RET_ERR_PARAMS);
//--- get current plugin configuration
   if((res=m_api->PluginCurrent(m_config))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"failed to get current plugin configuration [%s (%u)]",
                       SMTFormat::FormatError(res),res);
      return(res);
     }
//--- create plugin parameter object   
   if((param=m_api->PluginParamCreate())==NULL)
     {
      m_api->LoggerOutString(MTLogErr,L"failed to create plugin parameter object");
      return(MT_RET_ERR_MEM);
     }
//--- lock parameters
   m_sync.Lock();
//--- get parameters
//--- securities
   if((res=m_config->ParameterGet(L"Symbols",param))!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr,L"symbols is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
   m_symbols.Assign(param->ValueSymbols());
//--- groups
   if((res=m_config->ParameterGet(L"Groups",param))!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr,L"groups is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
   m_group_mask.Assign(param->ValueGroups());
//--- cost
   if((res=m_config->ParameterGet(L"Monthly cost",param))!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr,L"monthly cost is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
//--- charge is negative number
   m_cost=-fabs(param->ValueFloat());
//--- currency
   if((res=m_config->ParameterGet(L"Currency",param))!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr,L"currency is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
   m_currency.Assign(param->ValueSymbols());
//--- normalize monthly cost
   if(!(res=m_api->SymbolGet(m_currency.Str(),m_symbol))!=MT_RET_OK)
     {
      m_api->LoggerOut(MTLogErr,L"SymbolGet failed [%u]",res);
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
   m_cost=SMTMath::PriceNormalize(m_cost,m_symbol->Digits());
//--- disabled users
   if((res=m_config->ParameterGet(L"Skip disabled users",param))!=MT_RET_OK)
     {
      m_api->LoggerOutString(MTLogErr,L"ignore flag is missing");
      param->Release();
      m_sync.Unlock();
      return(MT_RET_ERR_PARAMS);
     }
   if(CMTStr::CompareNoCase(L"Yes" ,param->Value())==0 ||
      CMTStr::CompareNoCase(L"True",param->Value())==0)
      m_skip_disabled=true;
   else
      m_skip_disabled=false;
//--- Max deposit to charge
   if((res=m_config->ParameterGet(L"Max deposit to charge",param))!=MT_RET_OK)
     {
      //--- set default
      m_deposit_max=0;
     }
   else
      m_deposit_max=param->ValueFloat();
//--- Max overturn to charge
   if(m_config->ParameterGet(L"Max overturn to charge",param)!=MT_RET_OK)
     {
      //--- set default
      m_overturn_max=0;
     }
   else
      m_overturn_max=param->ValueFloat();
//--- show parameters
   m_api->LoggerOut(MTLogOK,L"Parameters: Symbols: \"%s\", Groups: \"%s\","
                    L" Monthly cost: %s, Currency: %s, Skip disabled users: %s,"
                    L" Max deposit to charge: %.2f, Max overturn to charge: %.2f",
                    m_symbols.Str(),m_group_mask.Str(),
                    SMTFormat::FormatMoney(tmp,m_cost,SMTMath::MoneyDigits(m_currency.Str())),
                    m_currency.Str(),m_skip_disabled ? L"Yes" : L"No",
                    m_deposit_max,m_overturn_max);

//--- unlock parameters
   m_sync.Unlock();
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
   if(CMTStr::Compare(plugin->Name(),m_config->Name())==0 &&
      plugin->Server()==m_config->Server())
      ParametersRead();
  }
//+------------------------------------------------------------------+
//| End of month event                                               |
//+------------------------------------------------------------------+
void CPluginInstance::OnEOMFinish(const int64_t datetime,const int64_t prev_datetime)
  {
   uint64_t       *logins      =NULL;
   uint32_t      logins_total=0;
   LPCWSTR       group_name  =NULL;
   MTAPIRES      retcode     =MT_RET_OK;
   uint64_t        deal_id     =0;
   double        charge      =0;
   CMTStr32      money;
//--- checks
   if(!m_api || !m_deals || !m_group || !m_user || !m_symbol)
      return;
//--- under lock
   m_sync.Lock();
//--- check all groups in the loop
   for(uint32_t pos=0;m_api->GroupNext(pos,m_group)==MT_RET_OK;pos++)
     {
      //--- is group located at our server?
      if(m_group->Server()!=m_config->Server())
         continue;
      if((group_name=m_group->Group())==NULL)
        {
         //--- release and exit
         m_group->Release();
         m_api->LoggerOutString(MTLogErr,L"Group failed");
         return;
        }
      //--- check group name
      if(CMTStr::CheckGroupMask(m_group_mask.Str(),group_name))
        {
         //--- check symbols
         for(uint32_t s=0;s<m_group->SymbolTotal();s++)
           {
            if((retcode=m_group->SymbolNext(s,m_group_symbol))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogAtt,L"get group symbol failed [%u]",retcode);
               continue;
              }
            //--- check symbols of group
            if(!SymbolCheck(m_symbols.Str(),m_group_symbol->Path()))
              {
               m_api->LoggerOut(MTLogOK,L"group %s was skipped",m_group->Group());
               continue;
              }
           }
         //--- get all logins
         if((retcode=m_api->UserLogins(group_name,logins,logins_total))!=MT_RET_OK)
           {
            m_api->LoggerOut(MTLogErr,L"UserLogins failed [%u]",retcode);
            //--- release and exit
            m_group->Release();
            return;
           }
         //--- check logins
         for(uint32_t i=0;i<logins_total;i++)
           {
            //--- get user
            if((retcode=m_api->UserGet(logins[i],m_user))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"UserAccoutGet failed [%u], user %I64u was skipped",retcode,logins[i]);
               continue;
              }
            //--- check enable
            if(m_skip_disabled)
               if(!(m_user->Rights() & IMTUser::USER_RIGHT_ENABLED))
                 {
                  m_api->LoggerOut(MTLogOK,L"user %I64u was skipped (user is disabled)",m_user->Login());
                  continue;
                 }
            //--- check deposit
            if(m_deposit_max)
              {
               //--- user's deposit greater than max?
               if(m_user->Balance()>m_deposit_max)
                 {
                  //--- user charge free
                  m_api->LoggerOut(MTLogOK,L"user %I64u has %s on deposit and was skipped",m_user->Login(),SMTFormat::FormatMoney(money,m_user->Balance(),m_group->CurrencyDigits()));
                  continue;
                 }
              }
            //--- check overturn
            if((retcode=m_api->DealGet(prev_datetime,datetime,logins[i],m_deals))!=MT_RET_OK)
              {
               m_api->LoggerOut(MTLogErr,L"DealGet failed [%u]. User %I64u was skipped",retcode,m_user->Login());
               continue;
              }
            //--- calculate deposit
            double   overturn=0;
            for(uint32_t j=0;j<m_deals->Total();j++)
              {
               //--- check max of overturn
               if(m_overturn_max<overturn)
                  break;
               //--- get the next deal
               IMTDeal *deal;
               if(!(deal=m_deals->Next(j)))
                 {
                  m_api->LoggerOutString(MTLogErr,L"get the next deal failed");
                  continue;
                 }
               //--- only buy/sell
               if(deal->Action()!=IMTDeal::DEAL_BUY && deal->Action()!=IMTDeal::DEAL_SELL)
                  continue;
               //--- get base symbol of deal
               if((retcode=m_api->SymbolGet(deal->Symbol(),m_symbol))!=MT_RET_OK)
                 {
                  m_api->LoggerOut(MTLogErr,L"get deals(%I64u) symbol \"%s\" failed [%u]",deal->Deal(),deal->Symbol(),retcode);
                  continue;
                 }
               //--- add to summary
               overturn=SMTMath::MoneyAdd(overturn,CalcAmountRaw(m_symbol,m_group->Currency(),deal),m_group->CurrencyDigits());
              }
            //--- check max of overturn again
            if(m_overturn_max<overturn)
              {
               //--- user charge free
               m_api->LoggerOut(MTLogOK,L"user %I64u has %s of overturn and was skipped",m_user->Login(),SMTFormat::FormatMoney(money,overturn,m_group->CurrencyDigits()));
               continue;
              }
            //--- convert cost
            charge=Convert(m_cost,m_currency.Str(),m_group->Currency());
            //--- get monthly charge
            if((retcode=m_api->UserDepositChange(m_user->Login(),charge,IMTDeal::DEAL_CHARGE,L"Monthly subscribe charge",deal_id))!=MT_RET_OK)
              {
               if(retcode==MT_RET_REQUEST_NO_MONEY)
                  m_api->LoggerOut(MTLogAtt,L"user %I64u hasn't enough money",m_user->Login());
               else
                  m_api->LoggerOut(MTLogErr,L"UserDepositChange failed [%u]",retcode);
               continue;
              }
            else
               m_api->LoggerOut(MTLogOK,L"monthly charge from %I64u, deal ID: %I64u, taken from the account: %s",m_user->Login(),deal_id,SMTFormat::FormatMoney(money,charge,m_group->CurrencyDigits()));
           }
        }
     }
   m_sync.Unlock();
//--- ok
  }
//+------------------------------------------------------------------+
//| Calculate raw amount in base currency for deal                   |
//+------------------------------------------------------------------+
inline double CPluginInstance::CalcAmountRaw(const IMTConSymbol *symbol_deal,const LPCWSTR currency,const IMTDeal *deal) const
  {
   double amount=0;
//--- checks
   if(!deal || !symbol_deal || !currency)
      return(0);
//--- calculate raw amount
   switch(symbol_deal->CalcMode())
     {
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount=SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
         break;
      case IMTConSymbol::TRADE_MODE_FUTURES           :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
         amount=deal->Price()*SMTMath::VolumeToDouble(deal->Volume())*deal->TickValue();
         if(deal->TickSize()) amount/=deal->TickSize();
         break;
      default:
         amount=deal->Price()*SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
         break;
     }
//--- normalize
   amount=SMTMath::PriceNormalize(amount,symbol_deal->CurrencyBaseDigits());
//---  and return result
   return(Convert(amount,symbol_deal->CurrencyBase(),currency));
  }
//+------------------------------------------------------------------+
//| Convert currency                                                 |
//+------------------------------------------------------------------+
double CPluginInstance::Convert(const double value,LPCWSTR base,LPCWSTR currency) const
  {
   MTAPIRES retcode=MT_RET_OK;
   double   rate   =0;
//--- checks
   if(!m_api || !base || !currency)
      return(0);
//--- buy or sell rate?
   if(value<0)
     {
      if((retcode=m_api->TradeRateBuy(base,currency,rate))!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr,L"TradeRateBuy failed [%u]",retcode);
         return(0);
        }
     }
   else
     {
      if((retcode=m_api->TradeRateSell(base,currency,rate))!=MT_RET_OK)
        {
         m_api->LoggerOut(MTLogErr,L"TradeRateSell failed [%u]",retcode);
         return(0);
        }
     }
//--- ok
   return(SMTMath::PriceNormalize(rate*value,SMTMath::MoneyDigits(currency)));
  }
//+------------------------------------------------------------------+
//| Check intersection of two masks of symbols                       |
//+------------------------------------------------------------------+
bool CPluginInstance::SymbolCheck(LPCWSTR mask,LPCWSTR group_mask) const
  {
   CMTStrPath chunk;
   CMTStrPath gmask;
   wchar_t   *chr    =(wchar_t*)mask;
   int32_t        pos    =0;
//--- check parameters
   if(!mask || !group_mask)
      return(false);
//--- all symbols?
   if(CMTStr::Compare(L"*",group_mask,_countof(L"*"))==0)
      return(true);
//--- is it mask or symbol
   if((pos=CMTStr::FindChar(group_mask,L'*'))!=-1)
      gmask.Assign(group_mask,pos);
   else
      gmask.Assign(group_mask);
//--- every mask compared with mask symbol of group
   do
     {
      //--- get next chunk
      if((pos=CMTStr::FindChar(chr,L','))>=0)
        {
         chunk.Assign(chr,pos);
         if(CMTStr::Len(chr)>=(uint32_t)pos)
            chr+=pos+1;
        }
      else
         chunk.Assign(chr);
      //--- all symbols?
      if(chunk.Compare(L"*")==0)
         return(true);
      //--- delete asterisk is exists
      chunk.ReplaceChar(L'*',NULL);
      chunk.Refresh();
      //--- compare by shorter string
      if(gmask.CompareNoCase(chunk.Str(),std::min(gmask.Len(),chunk.Len()))==0)
         return(true);
     }
   while(pos!=-1);
//--- not found
   return(false);
  }
//+------------------------------------------------------------------+