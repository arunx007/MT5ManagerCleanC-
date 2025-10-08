using System;

namespace MT5Wrapper.Trading.DTOs
{
    public class OrderDto
    {
        public ulong OrderId { get; set; }
        public ulong ClientLogin { get; set; }
        public string Symbol { get; set; } = "";
        public TradingOrderType Type { get; set; }
        public OrderSide Side { get; set; }
        public double Volume { get; set; }
        public double Price { get; set; }
        public double StopLoss { get; set; }
        public double TakeProfit { get; set; }
        public double StopLimit { get; set; }
        public DateTime TimeSetup { get; set; }
        public DateTime TimeExpiration { get; set; }
        public TradingOrderState State { get; set; }
        public string Comment { get; set; } = "";
        public int Magic { get; set; }
        public DateTime TimeCreate { get; set; }
        public DateTime TimeUpdate { get; set; }
    }

    public enum TradingOrderType
    {
        Buy = 0,
        Sell = 1,
        BuyLimit = 2,
        SellLimit = 3,
        BuyStop = 4,
        SellStop = 5,
        BuyStopLimit = 6,
        SellStopLimit = 7
    }

    public enum OrderSide
    {
        Buy = 0,
        Sell = 1
    }

    public enum TradingOrderState
    {
        Started = 0,
        Placed = 1,
        Canceled = 2,
        Partial = 3,
        Filled = 4,
        Rejected = 5,
        Expired = 6,
        RequestAdd = 7,
        RequestModify = 8,
        RequestCancel = 9
    }
}