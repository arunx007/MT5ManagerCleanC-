//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
//---
namespace WebTrader.Models.Core.Validator
  {
  /// <summary>
  /// Validate number
  /// </summary>
   public class ValidatorNumeric : ValidatorBase
     {
      public ValidatorNumeric(string message) : base (ValidatorType.Numeric,message) {}
      /// <summary>
      /// Try get number
      /// </summary>
      /// <param name="text">input string</param>
      /// <returns>text is number or not</returns>
      static public bool Validate(string text)
        {
         Int64 num;
         return Int64.TryParse(text,out num);
        }
     }
  }
