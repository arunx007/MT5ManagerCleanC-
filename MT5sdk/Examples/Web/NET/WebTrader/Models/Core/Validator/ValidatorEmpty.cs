//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Text.RegularExpressions;
//---
namespace WebTrader.Models.Core.Validator
  {
   public class ValidatorEmpty : ValidatorBase
     {
      public ValidatorEmpty(string message) : base (ValidatorType.Empty,message)
        {}
      /// <summary>
      /// Validate empty string
      /// </summary>
      /// <param name="text">input string</param>
      /// <returns></returns>
      static public bool Validate(string text)
        {
         return Regex.IsMatch(text,"[^ \t\n\r\f\v]",RegexOptions.IgnoreCase);
        }
     }
  }
