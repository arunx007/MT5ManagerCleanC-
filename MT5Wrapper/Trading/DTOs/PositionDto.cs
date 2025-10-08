using System;

namespace MT5Wrapper.Trading.DTOs
{
    public class PositionDto
    {
        public ulong PositionId { get; set; }
        public ulong ClientLogin { get; set; }
        public string Symbol { get; set; } = "";
        public PositionType Type { get; set; }
        public double Volume { get; set; }
        public double PriceOpen { get; set; }
        public double PriceCurrent { get; set; }
        public double StopLoss { get; set; }
        public double TakeProfit { get; set; }
        public double Profit { get; set; }
        public double Storage { get; set; }
        public double Commission { get; set; }
        public double Tax { get; set; }
        public double Swap { get; set; }
        public DateTime TimeCreate { get; set; }
        public DateTime TimeUpdate { get; set; }
        public string Comment { get; set; } = "";
        public int Magic { get; set; }
        public ulong OrderId { get; set; }
        public double PriceSL { get; set; }
        public double PriceTP { get; set; }
    }

    public enum PositionType
    {
        Buy = 0,
        Sell = 1
    }
}