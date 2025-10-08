//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyConverter.h"
//+------------------------------------------------------------------+
//| Report money structure                                           |
//+------------------------------------------------------------------+
struct ReportMoney
  {
   //--- Money structure add operation functor class
   class COperationAdd
     {
   private:
      const CCurrencyConverter &m_currency;     // currency converter reference

   public:
                        COperationAdd(const CCurrencyConverter &currency) : m_currency(currency) {}
      void              operator()(ReportMoney &l,const ReportMoney &r) const { l.Add(r,m_currency); }
     };
   //--- fields
   uint32_t          total;                     // count
   double            amount;                    // amount
   double            medium;                    // medium
   double            profit;                    // profit
   //--- add deal
   void              DealAdd(const double value,const uint32_t count,const CCurrencyConverter &currency);
   //--- add deals with profit
   void              DealProfitAdd(const uint32_t count,const double value,const double prof,const CCurrencyConverter &currency);
   //--- calculate medium value
   double            Medium(const CCurrencyConverter &currency) const;
   void              CalculateMedium(const CCurrencyConverter &currency);
   //--- add money structure
   void              Add(const ReportMoney &money,const CCurrencyConverter &currency);
   //--- sort money by amount descending
   static int32_t    SortAmountDesc(const void *left,const void *right);
   //--- compare money by amount descending
   static int32_t    CompareAmountDesc(const ReportMoney &l,const ReportMoney &r);
  };
//+------------------------------------------------------------------+
//| Report money section structure                                   |
//+------------------------------------------------------------------+
struct ReportSection
  {
   //--- Money section structure add operation functor class
   class COperationAdd
     {
   private:
      const CCurrencyConverter &m_currency;     // currency converter reference

   public:
                        COperationAdd(const CCurrencyConverter &currency) : m_currency(currency) {}
      void              operator()(ReportSection &l,const ReportSection &r) const { l.Add(r,m_currency); }
     };
   //--- fields
   int64_t           in_first_time;             // first deposit time
   ReportMoney       in_first;                  // first deposit
   ReportMoney       in;                        // deposit
   ReportMoney       out;                       // withdrawal
   double            in_out_medium;             // deposit-withdrawal medium
   //--- add deal
   void              DealAdd(const double value,const uint32_t count,const CCurrencyConverter &currency);
   //--- add first deposit deal to section
   void              DealInFirstAdd(const double value,const uint32_t count,const int64_t time,const CCurrencyConverter &currency);
   //--- calculate medium values
   void              CalculateMedium(const CCurrencyConverter &currency);
   //--- add money section structure
   void              Add(const ReportSection &section,const CCurrencyConverter &currency);
   //--- in-out amount delta calculation
   double            InOutAmountDelta(double *delta_accumulated) const;
   //--- in-out profit delta calculation
   double            InOutProfitDelta(double *delta_accumulated) const;
   //--- sort money section by in amount descending
   static int32_t    SortInAmountDesc(const void *left,const void *right);
   //--- sort money section by amount descending
   static int32_t    SortAmountDesc(const void *left,const void *right);
  };
//+------------------------------------------------------------------+
//| add deal                                                         |
//+------------------------------------------------------------------+
inline void ReportMoney::DealAdd(const double value,const uint32_t count,const CCurrencyConverter &currency)
  {
   amount=currency.MoneyAdd(amount,value);
   total+=count;
  }
//+------------------------------------------------------------------+
//| add deals with profit                                            |
//+------------------------------------------------------------------+
inline void ReportMoney::DealProfitAdd(const uint32_t count,const double value,const double prof,const CCurrencyConverter &currency)
  {
   if(!count)
      return;
   amount=currency.MoneyAdd(amount,value);
   profit=currency.MoneyAdd(profit,prof);
   total+=count;
  }
//+------------------------------------------------------------------+
//| calculate medium value                                           |
//+------------------------------------------------------------------+
inline double ReportMoney::Medium(const CCurrencyConverter &currency) const
  {
//--- check empty
   if(!total)
      return(0.);
//--- return medium
   return(currency.MoneyNormalize(amount/total));
  }
//+------------------------------------------------------------------+
//| calculate medium value                                           |
//+------------------------------------------------------------------+
inline void ReportMoney::CalculateMedium(const CCurrencyConverter &currency)
  {
   if(total)
      medium=Medium(currency);
  }
//+------------------------------------------------------------------+
//| add money structure                                              |
//+------------------------------------------------------------------+
inline void ReportMoney::Add(const ReportMoney &money,const CCurrencyConverter &currency)
  {
   amount=currency.MoneyAdd(amount,money.amount);
   total+=money.total;
  }
//+------------------------------------------------------------------+
//| sort money by amount descending                                  |
//+------------------------------------------------------------------+
inline int32_t ReportMoney::SortAmountDesc(const void *left,const void *right)
  {
//--- types conversion
   const ReportMoney *lft=*(const ReportMoney* const*)left;
   const ReportMoney *rgh=*(const ReportMoney* const*)right;
//--- compare amount
   return(CompareAmountDesc(*lft,*rgh));
  }
//+------------------------------------------------------------------+
//| compare money by amount descending                               |
//+------------------------------------------------------------------+
inline int32_t ReportMoney::CompareAmountDesc(const ReportMoney &l,const ReportMoney &r)
  {
//--- compare amount
   if(l.amount<r.amount)
      return(1);
   if(l.amount>r.amount)
      return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
//| add deal to section                                              |
//+------------------------------------------------------------------+
inline void ReportSection::DealAdd(const double value,const uint32_t count,const CCurrencyConverter &currency)
  {
//--- add deal depending on value sign
   if(value>DBL_EPSILON)
      in.DealAdd(value,count,currency);
   else
      if(value<-DBL_EPSILON)
         out.DealAdd(-value,count,currency);
  }
//+------------------------------------------------------------------+
//| add first deposit deal to section                                |
//+------------------------------------------------------------------+
inline void ReportSection::DealInFirstAdd(const double value,const uint32_t count,const int64_t time,const CCurrencyConverter &currency)
  {
//--- check arguments
   if(value<=DBL_EPSILON || !time)
      return;
//--- update first deposit in section
   if(!in_first_time || in_first_time>time)
      in_first_time=time;
//--- add first deal
   in_first.DealAdd(value,count,currency);
  }
//+------------------------------------------------------------------+
//| calculate medium values                                          |
//+------------------------------------------------------------------+
inline void ReportSection::CalculateMedium(const CCurrencyConverter &currency)
  {
//--- calculate medium values
   in_first .CalculateMedium(currency);
   in       .CalculateMedium(currency);
   out      .CalculateMedium(currency);
//--- calculatte deposit-withdrawal medium
   if(const uint32_t total=in.total+out.total)
      in_out_medium=currency.MoneyNormalize((in.amount-out.amount)/total);
  }
//+------------------------------------------------------------------+
//| add money section structure                                      |
//+------------------------------------------------------------------+
inline void ReportSection::Add(const ReportSection &section,const CCurrencyConverter &currency)
  {
//--- calculate medium values
   in_first .Add(section.in_first,currency);
   in       .Add(section.in      ,currency);
   out      .Add(section.out     ,currency);
//--- calculate deposit-withdrawal medium
   if(const uint32_t total=in.total+out.total)
      in_out_medium=currency.MoneyNormalize((in.amount-out.amount)/total);
  }
//+------------------------------------------------------------------+
//| in-out amount delta calculation                                  |
//+------------------------------------------------------------------+
inline double ReportSection::InOutAmountDelta(double *delta_accumulated) const
  {
//--- check empty section
   if(!in.total && !out.total)
      return(NAN);
//--- delta
   const double delta=in.amount-out.amount;
//--- accumulate delta
   if(delta_accumulated)
     {
      if(isnan(*delta_accumulated))
         *delta_accumulated=delta;
      else
         *delta_accumulated+=delta;
     }
   return(delta);
  }
//+------------------------------------------------------------------+
//| in-out profit delta calculation                                  |
//+------------------------------------------------------------------+
inline double ReportSection::InOutProfitDelta(double *delta_accumulated) const
  {
//--- check empty section
   if(!in.total && !out.total)
      return(NAN);
//--- delta
   const double delta=in.profit+out.profit;
//--- accumulate delta
   if(delta_accumulated)
     {
      if(isnan(*delta_accumulated))
         *delta_accumulated=delta;
      else
         *delta_accumulated+=delta;
     }
   return(delta);
  }
//+------------------------------------------------------------------+
//| sort money section by in amount descending                       |
//+------------------------------------------------------------------+
inline int32_t ReportSection::SortInAmountDesc(const void *left,const void *right)
  {
//--- types conversion
   const ReportSection *lft=*(const ReportSection* const*)left;
   const ReportSection *rgh=*(const ReportSection* const*)right;
//--- compare in amount
   return(ReportMoney::CompareAmountDesc(lft->in,rgh->in));
  }
//+------------------------------------------------------------------+
//| sort money section by amount descending                          |
//+------------------------------------------------------------------+
inline int32_t ReportSection::SortAmountDesc(const void *left,const void *right)
  {
//--- types conversion
   const ReportSection *lft=*(const ReportSection* const*)left;
   const ReportSection *rgh=*(const ReportSection* const*)right;
//--- amounts
   const double l=lft->in.amount+lft->out.amount;
   const double r=rgh->in.amount+rgh->out.amount;
//--- compare amount
   if(l<r)
      return(1);
   if(l>r)
      return(-1);
   return(0);
  }
//+------------------------------------------------------------------+
