//+------------------------------------------------------------------+
//|                            MetaTrader 5 Capital.Standard.Reports |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include "CurrencyConverter.h"
//+------------------------------------------------------------------+
//| Report money group static class                                  |
//| [0-10, 10-30, 30-50, 50-100, ...]                                |
//+------------------------------------------------------------------+
class SReportMoneyGroup
  {
public:
   //--- group index by value
   static uint32_t   GroupIndex(const double value);
   //--- group name by index
   static const CMTStr& GroupName(const uint32_t index,CMTStr &str,const CCurrencyConverter &currency);
   //--- fill top sections names by money group
   static MTAPIRES   FillTopNames(const CReportTopBase &top,CReportDictionary &names,const CCurrencyConverter &currency);

private:
   //--- break value to mantissa and logarithm base 10
   static double     BreakValue(double value,uint32_t &logarithm10);
   //--- calculate value range
   static double     GroupRange(const uint32_t index,double &end);
  };
//+------------------------------------------------------------------+
//| group index by value                                             |
//+------------------------------------------------------------------+
inline uint32_t SReportMoneyGroup::GroupIndex(const double value)
  {
//--- check range [0-10)
   if(value<10)
      return(0);
//---- break value to mantissa and logarithm base 10
   uint32_t logarithm10=0;
   double mantissa=BreakValue(value,logarithm10);
   uint32_t index=logarithm10*4;
//--- calculate mantissa range index
   if(mantissa<1)
      return(index);
   else if(mantissa<3)
         return(index+1);
      else if(mantissa<5)
            return(index+2);
   return(index+3);
  }
//+------------------------------------------------------------------+
//| group name by index                                              |
//+------------------------------------------------------------------+
inline const CMTStr& SReportMoneyGroup::GroupName(const uint32_t index,CMTStr &str,const CCurrencyConverter &currency)
  {
//--- calculate group range
   double end=0.;
   const double begin=GroupRange(index,end);
//--- format value range
   currency.FormatMoneyRound(str,begin);
   str.Append(L"-");
   CMTStr64 money;
   str.Append(currency.FormatMoneyRound(money,end));
//--- return string
   return(str);
  }
//+------------------------------------------------------------------+
//| calculate value range                                            |
//+------------------------------------------------------------------+
inline double SReportMoneyGroup::GroupRange(const uint32_t index,double &end)
  {
//--- check range [0-10)
   if(!index)
     {
      end=10.;
      return(0.);
     }
//--- calculate factor of 10
   const double factor10=SMTMath::DecPow(index/4);
//--- calculate range
   switch(index%4)
     {
      case 0: end=  factor10; return(  factor10/2);
      case 1: end=3*factor10; return(  factor10);
      case 2: end=5*factor10; return(3*factor10);
     }
   end=10*factor10;
   return(5*factor10);
  }
//+------------------------------------------------------------------+
//| break value to mantissa and logarithm base 10                    |
//+------------------------------------------------------------------+
inline double SReportMoneyGroup::BreakValue(double value,uint32_t &logarithm10)
  {
//--- check range [0-10)
   if(value<10)
     {
      logarithm10=0;
      return(0);
     }
//--- calculate logarithm base 10
   logarithm10=(uint32_t)log10(value);
//--- calculate mantissa
   const double factor10=SMTMath::DecPow(logarithm10);
   return(factor10 ? value/factor10 : value);
  }
//+------------------------------------------------------------------+
//| fill top sections names by money group                           |
//+------------------------------------------------------------------+
inline MTAPIRES SReportMoneyGroup::FillTopNames(const CReportTopBase &top,CReportDictionary &names,const CCurrencyConverter &currency)
  {
//--- top count
   const uint32_t total=top.Total();
   if(!total)
      return(MT_RET_OK);
//--- initialize top names
   if(!names.Initialize(total))
      return(MT_RET_ERR_MEM);
//--- initialize top names
   for(uint32_t i=0;i<total;i++)
     {
      //--- get top pos
      const uint32_t *pos=top.Pos(i);
      if(!pos)
         return(MT_RET_ERROR);
      //--- check other
      if(*pos==CReportTopBase::TOP_POS_OTHER)
        {
         //--- add other top name
         if(!names.String(i,L"Other"))
            return(MT_RET_ERR_MEM);
        }
      else
        {
         //--- store top name
         CMTStr128 str;
         if(!names.String(i,GroupName(*pos,str,currency).Str()))
            return(MT_RET_ERR_MEM);
        }
     }
//--- ok
   return(MT_RET_OK);
  }
//+------------------------------------------------------------------+
