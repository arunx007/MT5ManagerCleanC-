//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "ReportParameter.h"
//+------------------------------------------------------------------+
//| Default values                                                   |
//+------------------------------------------------------------------+
#define DEFAULT_CURRENCY                  L"USD"      // default: common currency
//+------------------------------------------------------------------+
//| Parameters                                                       |
//+------------------------------------------------------------------+
#define PARAMETER_CURRENCY                L"Currency"
//+------------------------------------------------------------------+
//| Currency converter class                                         |
//+------------------------------------------------------------------+
class CCurrencyConverter
  {
protected:
   CMTStr32          m_currency;                   // currency

private:
   IMTReportAPI     *m_api;                        // report api
   uint32_t          m_currency_digits;            // currency digits
   CMTStr32          m_currency_last;              // last conversion currency
   double            m_rate_last;                  // last conversion rate

public:
                     CCurrencyConverter(void);
                    ~CCurrencyConverter(void);
   //--- initialization
   MTAPIRES          Initialize(IMTReportAPI *api,CReportParameter &params,LPCWSTR param_name=PARAMETER_CURRENCY,LPCWSTR currency_default=DEFAULT_CURRENCY);
   //--- clear
   void              Clear(void);
   //--- check currency for conversion needed
   bool              NeedConversion(LPCWSTR currency) const;
   //--- currency conversion rate
   MTAPIRES          CurrencyRate(double &rate,LPCWSTR currency);
   //--- convert currency
   double            Convert(const double value,const double rate) const;
   //--- add money
   double            MoneyAdd(const double amount,const double value) const;
   //--- normalize money
   double            MoneyNormalize(const double value) const;
   //--- update column currency
   MTAPIRES          UpdateColumn(IMTDatasetColumn &column) const;
   //--- append currency to string
   const CMTStr&     Append(CMTStr &str) const;
   //--- format money round
   const CMTStr&     FormatMoneyRound(CMTStr &str,const double value) const;
  };
//+------------------------------------------------------------------+
