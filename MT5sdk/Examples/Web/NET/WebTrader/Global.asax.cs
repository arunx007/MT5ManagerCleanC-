//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.IO;
using System.Web;
using System.Web.Mvc;
using System.Web.Routing;
using WebTrader.Models;
//---
namespace WebTrader
  {
  public class MvcApplication : HttpApplication
    {
    private static string m_SolutionDirectory;
    /// <summary>
    /// Solution directory
    /// </summary>
    public static string SolutionDirectory
      {
      get
        {
        if(m_SolutionDirectory == null)
          {
          string sitePath = HttpRuntime.AppDomainAppPath;
          DirectoryInfo parentInfo = Directory.GetParent(sitePath);
          if(parentInfo != null)
            {
            DirectoryInfo di = parentInfo.Parent;
            if(di != null)
              m_SolutionDirectory = di.FullName;
            }
          }
        //---
        return m_SolutionDirectory;
        }
      }
    public static void RegisterRoutes(RouteCollection routes)
      {
      const string NUM_CONSTRAINT = @"^[0-9]\d{0,8}$";
      routes.IgnoreRoute("{resource}.axd/{*pathInfo}");
      //--- Home page
      routes.MapRoute(
          "Default", // Route name
          "", // URL with parameters
          new { controller = "Home",action = "Index",id = UrlParameter.Optional } // Parameter defaults
      );
      //--- Login page
      routes.MapRoute(
          "Login",
          "login",
          new { controller = "Home",action = "Login" }
      );
      //--- Logout
      routes.MapRoute(
          "Logout",
          "logout",
          new { controller = "Home",action = "Logout" }
      );
      //--- Register
      routes.MapRoute(
          "Register",
          "register",
          new { controller = "Home",action = "Register" }
      );
      //--- Register
      routes.MapRoute(
          "Captcha",
          "captcha/{number}",
          new { controller = "Captcha",action = "Index" }
      );
      //--- Orders
      routes.MapRoute(
          "Orders",
          "orders",
          new { controller = "Orders",action = "Index" }
      );
      //--- Orders
      routes.MapRoute(
          "OrdersPage",
          "orders/{page}",
          new { controller = "Orders",action = "Index",page = 0 },
          new { page = NUM_CONSTRAINT }
      );
      //--- Positions
      routes.MapRoute(
          "Positions",
          "positions",
          new { controller = "Positions",action = "Index" }
      );
      //--- Positions
      routes.MapRoute(
          "PositionsPage",
          "positions/{page}",
          new { controller = "Positions",action = "Index",page = 0 },
          new { page = NUM_CONSTRAINT }
      );
      //--- History
      routes.MapRoute(
          "History",
          "history",
          new { controller = "History",action = "Index" }
      );
      //--- History
      routes.MapRoute(
          "HistoryPage",
          "history/{page}",
          new { controller = "History",action = "Index",page = 0 },
          new { page = NUM_CONSTRAINT }
      );
      }
    /// <summary>
    /// First application start
    /// </summary>
    protected void Application_Start()
      {
      Log.Start(Path.Combine(SolutionDirectory,"logs"));
      Settings.Init();
      //---
      AreaRegistration.RegisterAllAreas();
      //---
      QueueRequest.Start();
      //---
      RegisterRoutes(RouteTable.Routes);
      }
    /// <summary>
    /// Application stop
    /// </summary>
    protected void Application_Stop()
      {
      //--- requests stio
      QueueRequest.Stop();
      //--- logger stop
      Log.Stop();
      
      }
    }
  }