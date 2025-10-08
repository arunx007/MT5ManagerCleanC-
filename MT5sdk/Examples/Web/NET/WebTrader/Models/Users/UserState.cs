//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Web;
using System;
using MetaQuotes.MT5WebAPI.Common;
//---
namespace WebTrader.Models.Users
  {
  public class UserState
    {
    /// <summary>
    /// Data of user
    /// </summary>
    public static MTUser User
      {
      get
        {
        if(IsLoggedIn)
          return (MTUser)HttpContext.Current.Session["user"];
        //---
        return null;
        }
      internal set
        {
        //--- 
        if(HttpContext.Current != null && HttpContext.Current.Session != null)
          HttpContext.Current.Session["user"] = value;
        }
      }
    /// <summary>
    /// Is log in user
    /// </summary>
    public static bool IsLoggedIn
      {
      get
        {
        return ((HttpContext.Current != null) && HttpContext.Current.Session != null) && (HttpContext.Current.Session["user"] != null);
        }
      }
    /// <summary>
    /// Единая точка входа
    /// </summary>    
    public static void Login(MTUser user)
      {
      //--- Save in session
      User = user;
      }
    /// <summary>
    /// Полный Logout с вычищением кук и сессии
    /// </summary> 
    public static void Logout()
      {
      if(User != null)
        Log.Write(LogType.Info,"UserState.Logout",string.Format("user '{0}' log out",User.Name));
      //--- clear session
      try
        {
        if(HttpContext.Current.Session != null)
          HttpContext.Current.Session.Clear();
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"UserState.Logout",string.Format("deleting session failed. {0}",e));
        }
      User = null;
      }
    }
  }
