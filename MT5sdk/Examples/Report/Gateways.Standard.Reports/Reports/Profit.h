//+------------------------------------------------------------------+
//|                           MetaTrader 5 Gateways.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
//+------------------------------------------------------------------+
//| Gateways Profit Report                                           |
//+------------------------------------------------------------------+
class CProfit : public IMTReportContext
  {
private:
   //--- constants
   enum constants
     {
      REPORT_INDEX=1,
     };
   //--- profit
   struct Profit
     {
      wchar_t           gateway[16];         // gateway
      uint64_t          gateway_id;          // gateway id
      wchar_t           symbol[32];          // symbol
      uint32_t          deals;               // amount of deals
      uint64_t          lots;                // lots
      double            amount_raw;          // raw amount
      wchar_t           amount_currency[32]; // raw amount currency
      double            amount;              // amount
      double            amount_rate;         // rate of amount currency to our currency
      int64_t           profit_pips;         // profit in pips
      double            profit_raw;          // raw profit
      wchar_t           profit_currency[32]; // raw profit currency
      double            profit;              // profit
      double            profit_rate;         // rate of profit currency to our currency
     };
   //--- profit array
   typedef TMTArray<Profit,1024> ProfitArray;
   //--- rate info
   struct RateInfo
     {
      wchar_t           currency[32];        // base currency
      double            rate_buy;            // rate sell
      double            rate_sell;           // rate buy
     };
   //--- rate array
   typedef TMTArray<RateInfo,256> RateInfoArray;

private:
   static MTReportInfo s_info;            // report information
   //--- api data
   IMTReportAPI     *m_api;               // api interface
   IMTConReport     *m_report;            // report
   IMTConParam      *m_param;             // report param
   IMTConGroup      *m_group;             // group
   IMTConSymbol     *m_symbol;            // symbol
   IMTDealArray     *m_deals;             // deals
   //--- report data
   wchar_t           m_currency[32];      // currency
   uint32_t          m_currency_digits;   // currency digits
   ProfitArray       m_profits;           // profits

public:
   //--- constructor/destructor
                     CProfit(void);
   virtual          ~CProfit(void);
   //--- get information about report
   static uint32_t   Index(void) { return(REPORT_INDEX); }
   static void       Info(MTReportInfo& info);
   //--- release plug-in
   virtual void      Release(void);
   //--- report generation
   virtual MTAPIRES  Generate(const uint32_t type,IMTReportAPI *api);

private:
   //--- clear
   void              Clear(void);
   //--- collecting data
   MTAPIRES          GenerateCollect(void);
   //--- collecting data by user
   MTAPIRES          GenerateUser(const uint64_t login);
   //--- generating HTML
   MTAPIRES          GenerateHtml(void);
   //--- gateway iterator
   bool              GatewayGet(const uint32_t pos,uint32_t& index);
   //--- sorting/searching
   static int32_t    SortBySymbol(const void *left,const void *right);
   static int32_t    SortByProfit(const void *left,const void *right);
   static int32_t    SortRates(const void *left,const void *right);
   static int32_t    SearchRates(const void *left,const void *right);
  };
//+------------------------------------------------------------------+

