//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Web;
//---  
namespace WebTrader.Helpers
  {
  /// <summary>
  /// Helper for users needs
  /// </summary>
  public static class UserHelper
    {
    /// <summary>
    /// Verification code saved in session
    /// </summary>
    internal static string VerificationCodeInSession
      {
      get
        {
        return (string)HttpContext.Current.Session["captcha"];
        }
      set { HttpContext.Current.Session["captcha"] = value; }
      }
    }
  }
