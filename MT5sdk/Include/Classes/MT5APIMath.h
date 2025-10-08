//+------------------------------------------------------------------+
//|                                                 MetaTrader 5 API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
#pragma once
#include <string.h>
#include <math.h>
//+------------------------------------------------------------------+
//| Volume constants                                                 |
//+------------------------------------------------------------------+
#define MTAPI_VOLUME_DIV        (10000.0)
#define MTAPI_VOLUME_DIGITS     (4)
#define MTAPI_VOLUME_MAX        ((uint64_t)10000000000)
//+------------------------------------------------------------------+
//| Volume with extended accuracy constants                          |
//+------------------------------------------------------------------+
#define MTAPI_VOLUME_EXT_DIV    (100000000.0)
#define MTAPI_VOLUME_EXT_DIGITS (8)
#define MTAPI_VOLUME_EXT_MAX    ((uint64_t)10000000000000000000u)
//+------------------------------------------------------------------+
//| Price constants                                                  |
//+------------------------------------------------------------------+
#define MTAPI_PRICE_MAX         (100000000)
#define MTAPI_PRICE_DIGITS_MAX  (11)
//+------------------------------------------------------------------+
//| Mathematical operations                                          |
//+------------------------------------------------------------------+
class SMTMath
  {
private:
   static const double s_decimal[16];
   static const double s_fdecimal[16];
   static const int64_t s_decimal_i[16];
   static const double s_double_max[16];
   static const double s_rounder_math;

public:
   //--- common functions
   static double     DecPow(INT digits);
   //--- price functions
   static double     PriceNormalize(const double price,uint32_t digits);
   static int64_t    PriceToInt(const double price,const uint32_t digits);
   static uint64_t   PriceToIntPos(const double price,const uint32_t digits);
   static double     PriceToDouble(const int64_t value,uint32_t digits);
   //--- volume functions
   static uint64_t   VolumeToInt(const double volume);
   static double     VolumeToDouble(const uint64_t volume);
   static double     VolumeToSize(const uint64_t volume,double contract_size);
   static uint64_t   VolumeFromSize(const double size,double contract_size);
   static uint64_t   VolumeFromVolumeExt(const uint64_t volume);
   //--- volume with extended accuracy functions
   static uint64_t   VolumeExtToInt(const double volume);
   static double     VolumeExtToDouble(const uint64_t volume);
   static double     VolumeExtToSize(const uint64_t volume,double contract_size);
   static uint64_t   VolumeExtFromSize(const double size,double contract_size);
   static uint64_t   VolumeExtFromVolume(const uint64_t volume);
   //--- money digits
   static double     MoneyAdd(const double left,const double right,const uint8_t digits);
   static bool       MoneyEqual(const double left,const double right,const uint8_t digits);
   static uint32_t   MoneyDigits(LPCWSTR currency);
  };
//+------------------------------------------------------------------+
//| 16 powers                                                        |
//+------------------------------------------------------------------+
const __declspec(selectany) double SMTMath::s_decimal[16]=
  {
   1.0,
   10.0,
   100.0,
   1000.0,
   10000.0,
   100000.0,
   1000000.0,
   10000000.0,
   100000000.0,
   1000000000.0,
   10000000000.0,
   100000000000.0,
   1000000000000.0,
   10000000000000.0,
   100000000000000.0,
   1000000000000000.0,
   };
//+------------------------------------------------------------------+
//| Negative 16 powers                                               |
//+------------------------------------------------------------------+
const __declspec(selectany) double SMTMath::s_fdecimal[16]=
  {
   1.0,
   0.1,
   0.01,
   0.001,
   0.0001,
   0.00001,
   0.000001,
   0.0000001,
   0.00000001,
   0.000000001,
   0.0000000001,
   0.00000000001,
   0.000000000001,
   0.0000000000001,
   0.00000000000001,
   0.000000000000001,
   };
//+------------------------------------------------------------------+
//| Rounder constant                                                 |
//+------------------------------------------------------------------+
const __declspec(selectany) double SMTMath::s_rounder_math=0.5000001;
//+------------------------------------------------------------------+
//| An array of limit values that can be multiplied by 10 ^ Digits   |
//| without loss of accuracy on the subsequent                       |
//  rounding (2 ^ 51/10 ^ Digits)                                    |
//+------------------------------------------------------------------+
const __declspec(selectany) double SMTMath::s_double_max[16]=
   {
    2251799813685248.,
    225179981368524.8,
    22517998136852.48,
    2251799813685.248,
    225179981368.5248,
    22517998136.85248,
    2251799813.685248,
    225179981.3685248,
    22517998.13685248,
    2251799.813685248,
    225179.9813685248,
    22517.99813685248,
    2251.799813685248,
    225.1799813685248,
    22.51799813685248,
    2.251799813685248
   };
//+------------------------------------------------------------------+
//| 16 powers                                                        |
//+------------------------------------------------------------------+
const __declspec(selectany) int64_t SMTMath::s_decimal_i[16]=
   {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000
   };
//+------------------------------------------------------------------+
//| Decimal power                                                    |
//+------------------------------------------------------------------+
inline double SMTMath::DecPow(INT digits)
  {
//--- check sign
   if(digits>0 && digits<_countof(s_decimal))
      return(s_decimal[digits]);
   else
      if(digits<0 && -digits<_countof(s_fdecimal))
         return(s_fdecimal[-digits]);
//--- use standard pow
   return(pow(10.0,digits));
  }
//+------------------------------------------------------------------+
//| Price normalization                                              |
//+------------------------------------------------------------------+
inline double SMTMath::PriceNormalize(const double val,uint32_t digits)
  {
   double dbl_integer;
//--- check digits
   if(digits>MTAPI_PRICE_DIGITS_MAX)
      digits=MTAPI_PRICE_DIGITS_MAX;
//--- calculate dividers
   double p=s_decimal[digits];
   dbl_integer=double(__int64(val));
   double dbl_fract=(val-dbl_integer)*p;
//--- check sign
   if(val>0) dbl_fract+=s_rounder_math;
   else      dbl_fract-=s_rounder_math;
//--- calc fractional part
   dbl_fract=double(__int64(dbl_fract));
//--- summary
   return(dbl_integer+dbl_fract/p);
  }
//+------------------------------------------------------------------+
//| Price conversion from double to integer                          |
//+------------------------------------------------------------------+
inline int64_t SMTMath::PriceToInt(const double price,const uint32_t digits)
  {
//--- check
   if(digits>MTAPI_PRICE_DIGITS_MAX || price==0.0 || price>double(_I64_MAX))
      return(0);
//--- positive
   if(price>=0.0)
     {
      //--- check complexity
      if(price<s_double_max[digits])
         return((int64_t)(price*s_decimal[digits]+s_rounder_math));
      //--- 
      int64_t i=int64_t(price);
      //--- convert by integer multiply
      return(i*s_decimal_i[digits]+int64_t((price-i)*s_decimal[digits]+s_rounder_math));
     }
//--- check complexity
   if(price>-s_double_max[digits])
      return((int64_t)(price*s_decimal[digits]-s_rounder_math));
//--- 
   int64_t i=int64_t(price);
//--- convert by integer multiply
   return(i*s_decimal_i[digits]+int64_t((price-i)*s_decimal[digits]-s_rounder_math));
  }
//+------------------------------------------------------------------+
//| Price conversion from double to integer                          |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::PriceToIntPos(const double price,const uint32_t digits)
  {
//--- check
   if(price<=0.0)
      return(PriceToInt(price,digits));
   else
     {
      //--- check complexity
      if(price<s_double_max[digits])
         return((uint64_t)(price*s_decimal[digits]+s_rounder_math));
     }
//--- 
   uint64_t i=uint64_t(price);
//--- convert by integer multiply
   return(i*s_decimal_i[digits]+uint64_t((price-i)*s_decimal[digits]+s_rounder_math));
  }
//+------------------------------------------------------------------+
//| Price conversion from integer to double                          |
//+------------------------------------------------------------------+
inline double SMTMath::PriceToDouble(const int64_t value,uint32_t digits)
  {
//--- check
   if(digits>15)
      digits=15;
//--- the number satisfies the accuracy of Double (2 ^ 53-1), divide and normalize
   if(value>-0x1FFFFFFFFFFFFF && value<0x1FFFFFFFFFFFFF)
      return(PriceNormalize(double(value)/s_decimal[digits],digits));
//---- divide the integer part separately
   return(PriceNormalize((value/s_decimal_i[digits])+(value%s_decimal_i[digits])/s_decimal[digits],digits));
  }
//+------------------------------------------------------------------+
//| Volume conversion from double to integer                         |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::VolumeToInt(const double volume)
  {
   return(PriceToIntPos(volume,MTAPI_VOLUME_DIGITS));
  }
//+------------------------------------------------------------------+
//| Volume conversion from integer to double                         |
//+------------------------------------------------------------------+
inline double SMTMath::VolumeToDouble(const uint64_t volume)
  {
   return(PriceNormalize(volume/double(MTAPI_VOLUME_DIV),MTAPI_VOLUME_DIGITS));
  }
//+------------------------------------------------------------------+
//| Volume conversion from lots to amount                            |
//+------------------------------------------------------------------+
inline double SMTMath::VolumeToSize(const uint64_t volume,double contract_size)
  {
   return(PriceNormalize(volume/double(MTAPI_VOLUME_DIV)*contract_size,MTAPI_VOLUME_DIGITS));
  }
//+------------------------------------------------------------------+
//| Volume conversion from amount to lots                            |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::VolumeFromSize(const double size,double contract_size)
  {
   return((uint64_t)PriceNormalize((size/contract_size)*double(MTAPI_VOLUME_DIV),0));
  }
//+------------------------------------------------------------------+
//| Volume conversion from extended accuracy volume                  |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::VolumeFromVolumeExt(const uint64_t volume_ext)
  {
   return(volume_ext/10000);
  }
//+------------------------------------------------------------------+
//| Volume conversion from double to integer                         |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::VolumeExtToInt(const double volume)
  {
   return(PriceToIntPos(volume,MTAPI_VOLUME_EXT_DIGITS));
  }
//+------------------------------------------------------------------+
//| Volume conversion from integer to double                         |
//+------------------------------------------------------------------+
inline double SMTMath::VolumeExtToDouble(const uint64_t volume)
  {
   return(PriceNormalize(volume/double(MTAPI_VOLUME_EXT_DIV),MTAPI_VOLUME_EXT_DIGITS));
  }
//+------------------------------------------------------------------+
//| Volume conversion from lots to amount                            |
//+------------------------------------------------------------------+
inline double SMTMath::VolumeExtToSize(const uint64_t volume,double contract_size)
  {
   return(PriceNormalize(volume/double(MTAPI_VOLUME_EXT_DIV)*contract_size,MTAPI_VOLUME_EXT_DIGITS));
  }
//+------------------------------------------------------------------+
//| Volume conversion from amount to lots                            |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::VolumeExtFromSize(const double size,double contract_size)
  {
   return((uint64_t)PriceNormalize((size/contract_size)*double(MTAPI_VOLUME_EXT_DIV),0));
  }
//+------------------------------------------------------------------+
//| Volume conversion to extended accuracy volume                  |
//+------------------------------------------------------------------+
inline uint64_t SMTMath::VolumeExtFromVolume(const uint64_t volume)
  {
   return(volume*10000);
  }
//+------------------------------------------------------------------+
//| Money add                                                        |
//+------------------------------------------------------------------+
inline double SMTMath::MoneyAdd(const double left,const double right,const uint8_t digits)
  {
   return(PriceNormalize(left+right,digits));
  }
//+------------------------------------------------------------------+
//| Money Compare                                                    |
//+------------------------------------------------------------------+
inline bool SMTMath::MoneyEqual(const double left,const double right,const uint8_t digits)
  {
   return(fabs(left-right)<DecPow(-(digits+1)));
  }
//+------------------------------------------------------------------+
//| Digits by currency                                               |
//+------------------------------------------------------------------+
inline uint32_t SMTMath::MoneyDigits(LPCWSTR currency)
  {
//--- check param
   if(currency)
     {
      if(_wcsicmp(currency,L"JPY")==0) return(0);
      if(_wcsicmp(currency,L"VND")==0) return(0);
     }
//--- 2 digits by default
   return(2);
  }
//+------------------------------------------------------------------+

