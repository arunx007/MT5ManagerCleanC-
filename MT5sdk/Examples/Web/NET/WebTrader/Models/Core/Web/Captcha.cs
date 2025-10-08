//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
using System.Text;
//---   
namespace WebTrader.Models.Core.Web
  {
  /// <summary>
  /// Binary HTTP-response
  /// </summary>
  public class Captcha
    {
    /// <summary>
    /// Captcha generation
    /// </summary>
    internal static string GenerateCaptchaText()
      {
      StringBuilder random = new StringBuilder();
      //---
      Random r = new Random(DateTime.Now.Millisecond);
      int textLength = r.Next(3,4);

      for(int i = 0; i < textLength; i++)
        {
        random.Append((char)r.Next(65,91)); // a-z
        }
      //---
      return random.ToString();
      }
    }
  }

