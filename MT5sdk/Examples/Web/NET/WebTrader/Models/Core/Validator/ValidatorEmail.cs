//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Text.RegularExpressions;
namespace WebTrader.Models.Core.Validator
  {
  /// <summary>
  /// Email validator
  /// </summary>
   public class ValidatorEmail : ValidatorBase
     {
      public static bool Validate(string email)
        {
        return email.Length < 255 && Regex.IsMatch(email,@"^\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*$",RegexOptions.IgnoreCase);
        }
      public ValidatorEmail(string message) : base (ValidatorType.Email,message) {}
     }
  }
