//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Globalization;
using System.Web.Mvc;
//---  
namespace WebTrader.Models.core.Validator
  {
  public static class ModelStateExtensions
    {
    /// <summary>
    /// This method processes situations when not all fields are exist on form (deleted by firebug, etc.)
    /// Used info from topic "RC2 HtmlHelper GetModelStateValue Bug": http://forums.asp.net/p/1393191/2987476.aspx#2987476
    /// </summary>
    /// <param name="dict"></param>
    /// <param name="fieldName">Name of the field on form</param>
    /// <param name="message">Error message</param>
    public static void AddModelErrorEx(this ModelStateDictionary dict,string fieldName,string message)
      {
      ModelState resValue;
      if(!dict.TryGetValue(fieldName,out resValue)) { dict.SetModelValue(fieldName,new ValueProviderResult("",fieldName,CultureInfo.CurrentCulture)); }
      //---  
      dict.AddModelError(fieldName,message);
      }
    }
  }
