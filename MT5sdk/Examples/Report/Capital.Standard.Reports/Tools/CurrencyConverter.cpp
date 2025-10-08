//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#include "stdafx.h"
#include "CurrencyConverter.h"
//+------------------------------------------------------------------+
//| Constructor                                                      |
//+------------------------------------------------------------------+
CCurrencyConverter::CCurrencyConverter(void) :
   m_api(nullptr),m_currency_digits(0),m_rate_last(0)
  {
  }
//+------------------------------------------------------------------+
//| Destructor                                                       |
//+------------------------------------------------------------------+
CCurrencyConverter::~CCurrencyConverter(void)
  {
   Clear();
  }
//+------------------------------------------------------------------+
//| Initialization                                                   |
//+------------------------------------------------------------------+
MTAPIRES CCurrencyConverter::Initialize(IMTReportAPI *api,CReportParameter &params,LPCWSTR param_name,LPCWSTR currency_default)
  {
//--- clear
   Clear();
//--- checks
   if(!api || !param_name || !*param_name || !currency_default || !*currency_default)
      return(MT_RET_ERR_PARAMS);
//--- get currency parameter value
   LPCWSTR currency=nullptr;
   MTAPIRES res=params.ValueString(currency,param_name,currency_default);
   if(res!=MT_RET_OK)
      return(res);
//--- check value
   if(!currency || !*currency)
      return(MT_RET_ERR_PARAMS);
//--- initalize parameters
   m_currency.Assign(currency);
   m_currency_digits=SMTMath::MoneyDigits(currency);
//--- store report api
   m_api=api;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Clear                                                            |
//+------------------------------------------------------------------+
void CCurrencyConverter::Clear(void)
  {
//--- clear parameters
   m_api=nullptr;
   m_currency.Clear();
   m_currency_digits=0;
   m_currency_last.Clear();
   m_rate_last=0.0;
  }
//+------------------------------------------------------------------+
//| Check currency for conversion needed                             |
//+------------------------------------------------------------------+
bool CCurrencyConverter::NeedConversion(LPCWSTR currency) const
  {
//--- compare currency
   return(m_currency.Compare(currency));
  }
//+------------------------------------------------------------------+
//| Currency conversion rate                                         |
//+------------------------------------------------------------------+
MTAPIRES CCurrencyConverter::CurrencyRate(double &rate,LPCWSTR currency)
  {
//--- checks
   if(!m_api || !currency || !*currency)
      return(MT_RET_ERR_PARAMS);
//--- check last currency
   if(!m_currency_last.Compare(currency))
     {
      rate=m_rate_last;
      return(MT_RET_OK);
     }
//--- get conversion rate from deal profit currency to common currency
   MTAPIRES res=m_api->TradeRateSell(currency,m_currency.Str(),rate);
   if(res!=MT_RET_OK)
      return(res);
//--- store currency
   m_currency_last.Assign(currency);
   m_rate_last=rate;
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Convert currency                                                 | 
//+------------------------------------------------------------------+
double CCurrencyConverter::Convert(const double value,const double rate) const
  {
//--- convert and normalize
   return(MoneyNormalize(value*rate));
  }
//+------------------------------------------------------------------+
//| Add money                                                        | 
//+------------------------------------------------------------------+
double CCurrencyConverter::MoneyAdd(const double amount,const double value) const
  {
//--- add money
   return(SMTMath::MoneyAdd(amount,value,m_currency_digits));
  }
//+------------------------------------------------------------------+
//| Normalize money                                                  | 
//+------------------------------------------------------------------+
double CCurrencyConverter::MoneyNormalize(const double value) const
  {
//--- normalize money
   return(SMTMath::PriceNormalize(value,m_currency_digits));
  }
//+------------------------------------------------------------------+
//| Update column currency                                           | 
//+------------------------------------------------------------------+
MTAPIRES CCurrencyConverter::UpdateColumn(IMTDatasetColumn &column) const
  {
//--- get column name
   CMTStr256 name;
   if(LPCWSTR column_name=column.Name())
      name.Assign(column_name);
//--- append currency
   Append(name);
//--- set column name
   MTAPIRES res=column.Name(name.Str());
   if(res!=MT_RET_OK)
      return(res);
//--- set currency digits
   if((res=column.Digits(m_currency_digits))!=MT_RET_OK)
      return(res);
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
//| Append currency to string                                        |
//+------------------------------------------------------------------+
const CMTStr& CCurrencyConverter::Append(CMTStr &str) const
  {
   str.Append(L", ");
   str.Append(m_currency);
   return(str);
  }
//+------------------------------------------------------------------+
//| Format money round                                               |
//+------------------------------------------------------------------+
const CMTStr& CCurrencyConverter::FormatMoneyRound(CMTStr &str,const double value) const
  {
   SMTFormat::FormatMoney(str,MoneyNormalize(value),m_currency_digits>2 ? m_currency_digits-2 : 0);
   return(str);
  }
//+------------------------------------------------------------------+
