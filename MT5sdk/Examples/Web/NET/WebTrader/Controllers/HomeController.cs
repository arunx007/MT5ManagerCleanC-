//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Web.Mvc;
using MetaQuotes.MT5WebAPI;
using MetaQuotes.MT5WebAPI.Common;
using WebTrader.Helpers;
using WebTrader.Models;
using WebTrader.Models.Core.Controllers;
using WebTrader.Models.Core.Web;
using WebTrader.Models.Users;
using WebTrader.Models.Views;
//---
namespace WebTrader.Controllers
  {

  public class HomeController : BaseAsyncController
    {
    /// <summary>
    /// user login
    /// </summary>
    private ulong m_Login;
    /// <summary>
    /// current user data
    /// </summary>
    private MTUser m_User;
    /// <summary>
    /// is register on mt server
    /// </summary>
    private bool m_IsRegistered;
    /// <summary>
    /// does check user on MT server
    /// </summary>
    private bool m_IsCheckUser;
    public void IndexAsync(string login,string password)
      {
      m_IsCheckUser = false;
      //---
      if(IsPostback)
        {
        //---
        ulong.TryParse(login,out m_Login);
        //---
        AsyncManager.OutstandingOperations.Increment();
        //---
        QueueData data = new QueueData { Callback = OnCheckUser };
        //---
        CheckLogin checkLogin = new CheckLogin { Login = m_Login,Password = password };
        data.Data = checkLogin;
        data.Name = "UserPasswordCheck";
        //---
        QueueRequest.Add(data);
        }
      }
    /// <summary>
    /// ready to view tempalte
    /// </summary>
    /// <returns></returns>
    public ActionResult IndexCompleted()
      {
      if(IsPostback)
        {
        if(m_IsCheckUser && m_User != null) UserState.Login(m_User);
        }
      return View("Index");
      }
    /// <summary>
    /// Result chek users
    /// </summary>
    /// <param name="result">code from mt server</param>
    /// <param name="data">data of result</param>
    public void OnCheckUser(MTRetCode result,object data)
      {
      if(result != MTRetCode.MT_RET_OK)
        {
        Log.Write(LogType.Error,"OnCheckUser","user not found, login: " + m_Login);
        ViewData.ModelState.AddModelError("common","Login or password incorrect");
        }
      else
        {
        m_IsCheckUser = true;
        m_User = new MTUser { Login = m_Login };
        Log.Write(LogType.Error,"OnCheckUser","user log in, " + m_Login);
        }
      //---
      AsyncManager.OutstandingOperations.Decrement();
      }
    /// <summary>
    /// Registration
    /// </summary>
    public void RegisterAsync(MTUser user)
      {
      if(IsPostback)
        {
        m_User = user;
        //--- check captcha
        if(string.Compare(Request["captcha"],UserHelper.VerificationCodeInSession,true) != 0)
          {
          ViewData.ModelState.AddModelError("common","Invalid verification code");
          return;
          }
        //---
        if(string.IsNullOrEmpty(user.Name)) ViewData.ModelState.AddModelError("name","Empty Name");
        if(string.IsNullOrEmpty(user.Email)) ViewData.ModelState.AddModelError("email","Empty Email");
        if(string.IsNullOrEmpty(user.MainPassword) || user.MainPassword.Length < 5) ViewData.ModelState.AddModelError("mainpassword","main password invalid");
        if(string.IsNullOrEmpty(user.InvestPassword) || user.InvestPassword.Length < 5) ViewData.ModelState.AddModelError("investpassword","Invest password invalid");
        //---
        if(!ViewData.ModelState.IsValid)
          {
          Log.Write(LogType.Error,"RegisterAsync","some fields invalid");
          return;
          }
        //--- send data to MT server
        AsyncManager.OutstandingOperations.Increment();
        //---
        QueueData data = new QueueData { Callback = OnUserAdd };
        //---
        m_User.Rights = MTUser.EnUsersRights.USER_RIGHT_DEFAULT;
        data.Data = m_User;
        data.Name = "UserAdd";
        //---
        QueueRequest.Add(data);
        }
      else
        {
        //---
        m_User = new MTUser();
        }
      }
    /// <summary>
    /// Result chek users
    /// </summary>
    /// <param name="result">code from mt server</param>
    /// <param name="data">data of result</param>
    public void OnUserAdd(MTRetCode result,object data)
      {
      if(result != MTRetCode.MT_RET_OK)
        {
        Log.Write(LogType.Error,"OnUserAdd","user not add");
        ViewData.ModelState.AddModelError("common","User not add to MetaTrader server: " + MTFormat.GetError(result));
        }
      else
        {
        MTUser mtUser = data as MTUser;
        if(mtUser == null)
          {
          ViewData.ModelState.AddModelError("common","User not add to MetaTrader server, unknow error");
          return;
          }
        //---
        m_User = mtUser;
        m_IsRegistered = true;
        Log.Write(LogType.Error,"OnCheckUser",string.Format("user added {0}, {1}",m_User.Login,m_User.Name));
        }
      //---
      AsyncManager.OutstandingOperations.Decrement();
      }
    /// <summary>
    /// ready to view tempalte
    /// </summary>
    /// <returns></returns>
    public ActionResult RegisterCompleted()
      {
      ViewRegister view = new ViewRegister();
      //---
      view.User = m_User;
      //---
      if(!m_IsRegistered)
        {
        //--- create captcha
        string code = Captcha.GenerateCaptchaText();
        //---
        UserHelper.VerificationCodeInSession = code;
        view.VerificationCode = code;
        //---
        return View("Register",view);
        }
      //--- registered users
      return View("RegisteredUser",view);
      }
    /// <summary>
    /// Log out
    /// </summary>
    /// <returns></returns>
    public ActionResult Logout()
      {
      if(!UserState.IsLoggedIn) return RedirectToAction("Index","Home");
      ulong login = UserState.User.Login;
      UserState.Logout();
      Log.Write(LogType.Info,"Logout",string.Format("user log out {0}",login));
      return RedirectToAction("Index","Home");
      }
    }
  }
