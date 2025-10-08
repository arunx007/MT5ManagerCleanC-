//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Web.Mvc;
//---  
namespace WebTrader.Models.Core.Controllers
  {
  /// <summary>
  /// Handler for master controller 
  /// </summary>  
  public delegate void MasterControllerDelegate(ViewDataDictionary viewData);
  //--- Attribute removes input parameters validation
  [ValidateInput(false)]
  public class BaseAsyncController : AsyncController
    {
    /// <summary>
    /// This public action is called by ApplicationError OR directly from code
    /// </summary>
    /// <param name="errorMessage">error message</param>    
    public ActionResult Http500Error(string errorMessage)
      {
      Log.Write(LogType.Error,"Http500Error",errorMessage);
      //--- 
      try
        {
        ViewData["Message"] = errorMessage ?? Request.QueryString["errorMessage"];
        ViewData["Details"] = Request.QueryString["details"];
        }
      catch { }
      //--- Looks into Shared -> Error view
      Response.Clear();
      Response.StatusCode = 500;
      return View("Http500");
      }
    /// <summary>
    /// Returns 404 code
    /// </summary>
    /// <param name="errorMessage">error message</param>    
    public ActionResult Http404NotFound(string errorMessage)
      {
      if(!string.IsNullOrEmpty(errorMessage))
        {
        string resultMessage = string.Format("404: {0}",errorMessage);
        if(Request.UrlReferrer != null)
          {
          resultMessage += string.Format(". Referrer '<a href='{0}'>{0}</a>'",Request.UrlReferrer);
          }
        //--- Write to log
        Log.Write(LogType.Error,"BaseAsyncController.Http404NotFound",resultMessage);
        }
      //---
      Response.Clear();
      Response.StatusCode = 404;
      return Request.IsAjaxRequest() ? (ActionResult)new EmptyResult() : View("Http404");
      }
    /// <summary>
    /// Is Post request
    /// </summary>
    public bool IsPostback { get { return Request.HttpMethod == "POST"; } }
    }
  }


