//+------------------------------------------------------------------+
//|                                             MetaTrader 5 Web API |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
using System.Collections.Generic;
using MetaQuotes.MT5WebAPI.Common;
//---
namespace WebTrader.Models.Views
  {
  public class ViewHistory
    {
    public List<MTOrder> Orders { get; set; }
    public uint CurrentPage { get; set; }
    }
  }