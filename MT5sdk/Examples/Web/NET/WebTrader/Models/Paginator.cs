//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
//---
namespace WebTrader.Models
  {
  public class Paginator
    {
    public uint PageCount { get; set; }
    public uint CurrentPage { get; set; }
    public string RouteName { get; set; }
    public string OrderBy { get; set; }
    public string Dir { get; set; }
    }
  }