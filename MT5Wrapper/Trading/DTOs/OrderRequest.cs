using System;
using MT5Wrapper.OrderManagement.DTOs;

namespace MT5Wrapper.Trading.DTOs
{
    public class OrderRequest
    {
        public string Symbol { get; set; } = "";
        public OrderType Type { get; set; }
        public double Volume { get; set; }
        public double Price { get; set; }
        public double StopLoss { get; set; }
        public double TakeProfit { get; set; }
        public double StopLimit { get; set; }
        public DateTime? Expiration { get; set; }
        public string Comment { get; set; } = "";
        public int Magic { get; set; }
    }

    public class ModifyOrderRequest
    {
        public double? Price { get; set; }
        public double? StopLoss { get; set; }
        public double? TakeProfit { get; set; }
        public double? StopLimit { get; set; }
        public DateTime? Expiration { get; set; }
    }

    public class ModifyPositionRequest
    {
        public double? StopLoss { get; set; }
        public double? TakeProfit { get; set; }
    }

    public class BulkOrderRequest
    {
        public ulong ClientLogin { get; set; }
        public OrderRequest OrderRequest { get; set; } = new();
    }

    public class BulkPositionRequest
    {
        public ulong ClientLogin { get; set; }
        public ulong PositionId { get; set; }
        public string Symbol { get; set; } = "";
    }
}