//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
using System.Text;
using System.Web;
using System.Web.Mvc;
using System.Web.Mvc.Html;
using System.Web.Routing;
using WebTrader.Models;
//---  
namespace WebTrader.Helpers
  {
  public delegate string MvcCacheCallback(HttpContextBase context);
  /// <summary>
  /// Helper for site needs
  /// </summary>
  public static class SiteHelper
    {
    /// <summary>
    /// Build main menu
    /// </summary>
    /// <returns>string of menu</returns>
    public static string BuildMenu(this HtmlHelper htmlHelper)
      {
      string[] segments = HttpContext.Current.Request.Url.Segments;
      StringBuilder sb = new StringBuilder();
      //---
      string currentPage = "";
      if(segments.Length > 2)
        currentPage = segments[2].TrimEnd('/');
      //--- orders
      sb.Append(string.Format("<span><a href='/orders'{0}>Orders</a></span>",currentPage == "orders" ? " class='active'" : ""));
      //--- positions
      sb.Append(string.Format("<span><a href='/positions'{0}>Positions</a></span>",currentPage == "positions" ? " class='active'" : ""));
      //--- history
      sb.Append(string.Format("<span><a href='/history'{0}>History</a></span>",currentPage == "positions" ? " class='active'" : ""));
      return sb.ToString();
      }
    /// <summary>
    /// Get captcha by string
    /// </summary>    
    public static string Captcha(this HtmlHelper htmlHelper,string textToShow,string title)
      {
      if(textToShow == null)
        return null;
      //---
      if(textToShow.Length > 5)
        {
        Log.Write(LogType.Error,"Captcha","Argument for drawing mustn't be greater than 5");
        return null;
        }
      //---
      Random random = new Random(DateTime.Now.Millisecond);
      HttpContext.Current.Session["captcha"] = textToShow;
      //---  
      return string.Format("<img width='100' height='40' alt='captcha' class='captcha' src='/captcha/{0}' title='{1}'>",random.Next(1000),title);
      }
    /// <summary>
    /// get absolut link /users/edit
    /// </summary>
    /// <param name="helper"></param>
    /// <param name="text">text</param>
    /// <param name="routeName">router name for link</param>
    /// <param name="values"></param>
    /// <param name="htmlAttributes">more html attributes</param>
    /// <returns></returns>
    public static string RouteLinkAbsolut(this HtmlHelper helper,string text,string routeName,object values,object htmlAttributes)
      {
      Route tempRoute = ((Route)helper.RouteCollection[routeName]);
      //---  
      RouteValueDictionary rvd = new RouteValueDictionary(values);
      if(!rvd.ContainsKey("controller"))
        rvd.Add("controller",tempRoute.Defaults["controller"]);
      if(!rvd.ContainsKey("action"))
        rvd.Add("action",tempRoute.Defaults["action"]);
      //---
      if(string.IsNullOrEmpty(text)) text = "Unknown";
      return helper.RouteLink(text,routeName,rvd,htmlAttributes == null ? null : new RouteValueDictionary(htmlAttributes)).ToString();
      }
    }
  }
