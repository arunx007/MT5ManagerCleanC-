using System;

namespace MT5Wrapper.OrderManagement.DTOs
{
    public class CreateOrderRequest
    {
        public ulong Login { get; set; }
        public string Symbol { get; set; } = string.Empty;
        public OrderType Type { get; set; }
        public double Volume { get; set; }
        public double Price { get; set; }
        public double PriceStopLoss { get; set; }
        public double PriceTakeProfit { get; set; }
        public long Magic { get; set; }
        public string Comment { get; set; } = string.Empty;
        public DateTimeOffset? Expiration { get; set; }
        public string ExternalId { get; set; } = string.Empty;
    }

    public class CreatePendingOrderRequest : CreateOrderRequest
    {
        public double PriceOrder { get; set; } // Entry price for pending orders
    }

    public class ModifyOrderRequest
    {
        public ulong Ticket { get; set; }
        public double? Price { get; set; }
        public double? PriceStopLoss { get; set; }
        public double? PriceTakeProfit { get; set; }
        public DateTimeOffset? Expiration { get; set; }
    }

    public class ModifyPendingOrderRequest : ModifyOrderRequest
    {
        public double? PriceOrder { get; set; }
    }

    public class CancelOrderRequest
    {
        public ulong Ticket { get; set; }
        public string Reason { get; set; } = string.Empty;
    }

    public class CloseOrderRequest
    {
        public ulong Ticket { get; set; }
        public double Volume { get; set; } // Volume to close (0 = close all)
        public double Price { get; set; }
    }

    public class CancelPendingOrderRequest : CancelOrderRequest
    {
        // Inherits all properties from CancelOrderRequest
    }
}