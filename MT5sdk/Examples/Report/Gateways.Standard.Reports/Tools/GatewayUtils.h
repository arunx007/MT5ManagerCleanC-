//+------------------------------------------------------------------+
//|                           MetaTrader 5 Gateways.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Login of ECN Dealer                                              |
//+------------------------------------------------------------------+
#define DEALER_ECN_LOGIN UINT64_MAX
//+------------------------------------------------------------------+
//| Gateway utils                                                    |
//+------------------------------------------------------------------+
class SGatewayUtils
  {
public:
   //--- get gateway name by gateway id
   static bool       GatewayName(IMTReportAPI *api,LPCTSTR gateway,const uint64_t id,CMTStr& name);
   //--- calculate methods
   static double     CalcAmountRaw(const IMTConSymbol *symbol,const IMTDeal *deal);
   static double     CalcProfitRaw(const IMTConSymbol *symbol,const IMTDeal *deal,int64_t& profit_pips);
  };
//+------------------------------------------------------------------+
//| Get gateway name by gateway id                                   |
//+------------------------------------------------------------------+
inline bool SGatewayUtils::GatewayName(IMTReportAPI *api,LPCTSTR gateway,const uint64_t id,CMTStr& name)
  {
   IMTConGateway       *config=NULL;
   IMTConGatewayModule *module=NULL;
//--- clear name
   name.Clear();
//--- check dealer login of ECN
   if(id==DEALER_ECN_LOGIN)
     {
      //--- assing full name
      name.Assign(L"ECN");
      return(true);
     }
//--- checks
   if(!api ||  !gateway || gateway[0]==L'\0') return(false);
//--- create interface
   if((config=api->GatewayCreate())==NULL) return(false);
//--- iterate gateways
   for(uint32_t pos=0;api->GatewayNext(pos,config)==MT_RET_OK;pos++)
      if(config->ID()==id && CMTStr::Compare(config->Gateway(),gateway)==0)
        {
         //--- assing full name
         name.Assign(config->Name());
         //--- release config
         config->Release();
         //--- ok
         return(!name.Empty());
        }
//--- release config
   config->Release();
//--- create interface
   if((module=api->GatewayModuleCreate())==NULL) return(false);
//--- iterate gateways modules
   for(uint32_t pos=0;api->GatewayModuleNext(pos,module)==MT_RET_OK;pos++)
      if(CMTStr::Compare(module->Gateway(),gateway)==0)
        {
         //--- assing full name
         name.Assign(module->Name());
         //--- release module
         module->Release();
         //--- ok
         return(!name.Empty());
        }
//--- release module
   module->Release();
//--- not found
   return(false);
  }
//+------------------------------------------------------------------+
//| Calculate raw amount in base currency for deal                   |
//+------------------------------------------------------------------+
inline double SGatewayUtils::CalcAmountRaw(const IMTConSymbol *symbol,const IMTDeal *deal)
  {
   double amount_raw=0;
//--- checks
   if(!symbol || !deal) return(0);
//--- calculate raw amount
   switch(symbol->CalcMode())
     {
      case IMTConSymbol::TRADE_MODE_FOREX:
      case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
         amount_raw=SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
         break;
      case IMTConSymbol::TRADE_MODE_FUTURES           :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
      case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
         amount_raw=deal->Price()*SMTMath::VolumeToDouble(deal->Volume())*deal->TickValue();
         if(deal->TickSize()) amount_raw/=deal->TickSize();
         break;
      default:
         amount_raw=deal->Price()*SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
         break;
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(amount_raw,symbol->CurrencyBaseDigits()));
  }
//+------------------------------------------------------------------+
//| Calculate raw gateway profit in profit currency for deal         |
//+------------------------------------------------------------------+
inline double SGatewayUtils::CalcProfitRaw(const IMTConSymbol *symbol,const IMTDeal *deal,int64_t& profit_pips)
  {
   double profit_raw=0,size=0,deal_size=0,gateway_size=0;
//--- clear profit in pips
   profit_pips=0;
//--- checks
   if(!symbol || !deal) return(0);
//--- check gateway price
   if(deal->PriceGateway())
     {
      //--- calculate pips profit
      if(deal->Action()==IMTDeal::DEAL_BUY)
         profit_pips=(int64_t)SMTMath::PriceNormalize((deal->Price()-deal->PriceGateway())*SMTMath::DecPow(symbol->Digits()),0);
      else
         profit_pips=(int64_t)SMTMath::PriceNormalize((deal->PriceGateway()-deal->Price())*SMTMath::DecPow(symbol->Digits()),0);
      //--- calculate raw profit
      switch(symbol->CalcMode())
        {
         case IMTConSymbol::TRADE_MODE_FOREX:
         case IMTConSymbol::TRADE_MODE_FOREX_NO_LEVERAGE:
            size=SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
            if(deal->Action()==IMTDeal::DEAL_BUY)
               profit_raw=SMTMath::PriceNormalize(deal->Price()*size,symbol->CurrencyProfitDigits())-SMTMath::PriceNormalize(deal->PriceGateway()*size,symbol->CurrencyProfitDigits());
            else
               profit_raw=SMTMath::PriceNormalize(deal->PriceGateway()*size,symbol->CurrencyProfitDigits())-SMTMath::PriceNormalize(deal->Price()*size,symbol->CurrencyProfitDigits());
            break;
         case IMTConSymbol::TRADE_MODE_FUTURES           :
         case IMTConSymbol::TRADE_MODE_EXCH_FUTURES      :
            size=SMTMath::VolumeToDouble(deal->Volume());
            if(deal->Action()==IMTDeal::DEAL_BUY)
               profit_raw=(deal->Price()-deal->PriceGateway())*size*symbol->TickValue();
            else
               profit_raw=(deal->PriceGateway()-deal->Price())*size*symbol->TickValue();
            if(symbol->TickSize())
               profit_raw/=symbol->TickSize();
            break;
         case IMTConSymbol::TRADE_MODE_EXCH_FUTURES_FORTS:
            size=SMTMath::VolumeToDouble(deal->Volume());
            //--- проверим размер тика
            if(symbol->TickSize())
              {
               deal_size   =SMTMath::PriceNormalize(deal->Price()*symbol->TickValue()/symbol->TickSize(),symbol->CurrencyProfitDigits());
               gateway_size=SMTMath::PriceNormalize(deal->PriceGateway()*symbol->TickValue()/symbol->TickSize(),symbol->CurrencyProfitDigits());
              }
            else
              {
               deal_size   =SMTMath::PriceNormalize(deal->Price()*symbol->TickValue(),symbol->CurrencyProfitDigits());
               gateway_size=SMTMath::PriceNormalize(deal->PriceGateway()*symbol->TickValue(),symbol->CurrencyProfitDigits());
              }
            //--- считаем итоговую прибыль
            if(deal->Action()==IMTDeal::DEAL_BUY)
               profit_raw=(deal_size-gateway_size)*size;
            else
               profit_raw=(gateway_size-deal_size)*size;
            break;
         default:
            size=SMTMath::VolumeToSize(deal->Volume(),deal->ContractSize());
            if(deal->Action()==IMTDeal::DEAL_BUY)
               profit_raw=deal->Price()*size-deal->PriceGateway()*size;
            else
               profit_raw=deal->PriceGateway()*size-deal->Price()*size;
            break;
        }
     }
//--- normalize and return result
   return(SMTMath::PriceNormalize(profit_raw,symbol->CurrencyProfitDigits()));
  }
//+------------------------------------------------------------------+
  