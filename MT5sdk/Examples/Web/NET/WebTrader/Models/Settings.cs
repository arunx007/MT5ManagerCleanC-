//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
//---
using System;
using System.Configuration;
using MetaQuotes.MT5WebAPI;
namespace WebTrader.Models
  {
  public class Settings
    {
    public static MT5WebAPI.EnPumpModes MTPumps { get; private set; }
    public static string MTServer { get; private set; }
    public static int MTServerPort { get; private set; }
    public static ulong MTLogin { get; private set; }
    public static string MTPassword { get; private set; }
    public static MT5WebAPI.EnCryptModes MTCrypt { get; private set; }
    public static double MTDefaultDeposit { get; private set; }
    /// <summary>
    /// Init settings
    /// </summary>
    public static void Init()
      {
      int temp;
      double tempDouble;
      //---
      try
        {
        MTServer = ConfigurationManager.AppSettings["metatrader_server"];
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_server failed: {0}",e));
        }
      //---
      try
        {
        int.TryParse(ConfigurationManager.AppSettings["metatrader_port"],out temp);
        MTServerPort = temp;
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_port failed: {0}",e));
        }
      //---
      try
        {
        ulong longTemp;
        ulong.TryParse(ConfigurationManager.AppSettings["metatrader_login"],out longTemp);
        MTLogin = longTemp;
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_login failed: {0}",e));
        }
      //---
      try
        {
        MTPassword = ConfigurationManager.AppSettings["metatrader_password"];
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_password failed: {0}",e));
        }
      //---
      try
        {
        int.TryParse(ConfigurationManager.AppSettings["metatrader_crypt"],out temp);

        MTCrypt = (MT5WebAPI.EnCryptModes)temp;
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_crypt failed: {0}",e));
        }
      //---
      try
        {
        int.TryParse(ConfigurationManager.AppSettings["metatrader_pumps"],out temp);
        MTPumps = (MT5WebAPI.EnPumpModes)temp;
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_pumps failed: {0}",e));
        }
      //---
      try
        {
        double.TryParse(ConfigurationManager.AppSettings["metatrader_default_deposit"],out tempDouble);
        MTDefaultDeposit = tempDouble;
        }
      catch(Exception e)
        {
        Log.Write(LogType.Error,"Settings.Init",string.Format("init settings metatrader_default_deposit failed: {0}",e));
        }
      }
    }
  }