//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System;
namespace WebTrader.Models
  {
  public class Date
    {
    private static readonly DateTime m_ZeroDate = new DateTime(1970,1,1,0,0,0,0);
    /// <summary>
    /// Convert from DateTime to Unix
    /// </summary>
    /// <param name="date">date in DateTime format</param>
    /// <returns></returns>
    public static long ConvertToUnixTimestamp(DateTime date)
      {
      DateTime origin = new DateTime(1970,1,1,0,0,0,0);
      TimeSpan diff = date - origin;
      return (long)Math.Floor(diff.TotalSeconds);
      }
    /// <summary>
    /// From unix to DateTime format
    /// </summary>
    /// <param name="seconds">количество секунд</param>
    /// <returns>Время</returns>
    public static DateTime ConvertFromUnixTime(ulong seconds)
      {
      DateTime origin = new DateTime(1970,1,1,0,0,0,0);
      return origin.AddSeconds(seconds);
      }
    /// <summary>
    /// Get zero date 1/1/1970 0:0:0
    /// </summary>
    /// <returns></returns>
    public static DateTime ZeroDate { get { return m_ZeroDate; } }
    }
  }