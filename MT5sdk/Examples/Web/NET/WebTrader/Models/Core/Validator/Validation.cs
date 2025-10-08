//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Web;
using System.Text;
using System.Web.Mvc;
using System.Web.Routing;
using System.Web.Mvc.Html;
//---
namespace WebTrader.Models.Core.Validator
  {
  /// <summary>
  /// Class create javascript validator on client side
  /// </summary>
  public static class Validation
    {
    private const string ELEMENT_FORMAT = "<span class='field-validation-error'>{0}</span>";
    ///<summary>
    /// vew form and validator
    ///</summary>
    public static MvcForm BeginFormValidated(this HtmlHelper helper,string actionName,string controllerName,FormMethod method)
      {
      return helper.BeginFormValidated(actionName,controllerName,method,null);
      }
    ///<summary>
    /// vew form and validator
    ///</summary>
    public static MvcForm BeginFormValidated(this HtmlHelper helper,string actionName,string controllerName,FormMethod method,object htmlAttributes)
      {
      HttpContext.Current.Response.Write("<script type='text/javascript'>if(! window.V) var V=[];</script>\r\n");
      RouteValueDictionary vals = new RouteValueDictionary(htmlAttributes);
      if(vals.ContainsKey("onsubmit"))
        {
        vals["onsubmit"] = vals["onsubmit"];
        }
      else
      { vals.Add("onsubmit","return Validate(this);"); }
      return helper.BeginForm(actionName,controllerName,method,vals);
      }
    ///<summary>
    /// vew form and validator
    ///</summary>
    public static MvcForm BeginFormValidated(this HtmlHelper helper,string actionName,string controllerName,FormMethod method,object htmlAttributes,RouteValueDictionary route)
      {
      HttpContext.Current.Response.Write("<script type='text/javascript'>if(! window.V) var V=[];</script>\r\n");
      RouteValueDictionary vals = new RouteValueDictionary(htmlAttributes);
      if(vals.ContainsKey("onsubmit"))
        {
        vals["onsubmit"] = vals["onsubmit"];
        }
      else
      { vals.Add("onsubmit","return Validate(this);"); }
      return helper.BeginForm(actionName,controllerName,route,method,vals);
      }
    ///<summary>
    /// vew form and validator
    ///</summary>
    public static string Validator(this HtmlHelper helper,string controlToValidate,params object[] validators)
      {
      StringBuilder result = new StringBuilder();
      //--- если есть какие-то сообщения выведем их
      if(helper.ViewData.ModelState.ContainsKey(controlToValidate))
        {
        ModelState ms = helper.ViewData.ModelState[controlToValidate];
        foreach(ModelError error in ms.Errors)
          {
          result.AppendFormat(ELEMENT_FORMAT,error.ErrorMessage);
          }
        }
      //---
      if(validators.Length > 0)
        {
        result.AppendFormat("<script type='text/javascript' id='validate_{0}'>",controlToValidate);
        int l = validators.Length;
        for(int i = 0; i < l; ++i)
          {
          if(validators[i] == null) continue;
          if((validators[i] as ValidatorBase).Type == 0) continue;
          //--- register the clients validator
          result.AppendFormat("V.push(['{0}',{1},'{2}'",controlToValidate,(int)(validators[i] as ValidatorBase).Type,(validators[i] as ValidatorBase).Message.Replace("'","\\'"));
          if(validators[i].GetType() == typeof(ValidatorLength))
            result.AppendFormat(",{0},{1}",(validators[i] as ValidatorLength).MinLength,(validators[i] as ValidatorLength).MaxLength);
          else if(validators[i].GetType() == typeof(ValidatorEqual))
            result.AppendFormat(",'{0}',{1}",(validators[i] as ValidatorEqual).Example,(validators[i] as ValidatorEqual).IgnoreCase ? "true" : "false");
          else if(validators[i].GetType() == typeof(ValidatorRegexp))
            result.AppendFormat(",'{0}','{1}'",(validators[i] as ValidatorRegexp).RegExp.Replace(@"\",@"\\"),(validators[i] as ValidatorRegexp).Flags.Replace(@"\",@"\\"));
          //---
          result.Append("]);");
          }
        result.Append("</script>");
        }

      return result.ToString();
      }
    }
  }
