using System;
using System.Collections.Generic;

namespace MT5Wrapper.OrderManagement.DTOs
{
    public class OrderDto
    {
        public ulong Ticket { get; set; }
        public ulong Login { get; set; }
        public string Symbol { get; set; } = string.Empty;
        public OrderType Type { get; set; }
        public OrderState State { get; set; }
        public double Volume { get; set; }
        public double VolumeCurrent { get; set; }
        public double Price { get; set; }
        public double PriceOrder { get; set; }
        public double PriceStopLoss { get; set; }
        public double PriceTakeProfit { get; set; }
        public long Magic { get; set; }
        public string Comment { get; set; } = string.Empty;
        public DateTimeOffset TimeSetup { get; set; }
        public DateTimeOffset TimeExpiration { get; set; }
        public DateTimeOffset TimeDone { get; set; }
        public double Profit { get; set; }
        public double Commission { get; set; }
        public double Swap { get; set; }
        public double Fee { get; set; }
        public string ExternalId { get; set; } = string.Empty;
        public string Group { get; set; } = string.Empty;
        public int Reason { get; set; }
    }

    public enum OrderType
    {
        OP_BUY = 0,
        OP_SELL = 1,
        OP_BUYLIMIT = 2,
        OP_SELLLIMIT = 3,
        OP_BUYSTOP = 4,
        OP_SELLSTOP = 5,
        OP_BUYSTOPLIMIT = 6,
        OP_SELLSTOPLIMIT = 7
    }

    public enum OrderState
    {
        ORDER_STATE_STARTED = 0,
        ORDER_STATE_PLACED = 1,
        ORDER_STATE_CANCELED = 2,
        ORDER_STATE_PARTIAL = 3,
        ORDER_STATE_FILLED = 4,
        ORDER_STATE_REJECTED = 5,
        ORDER_STATE_EXPIRED = 6,
        ORDER_STATE_REQUEST_ADD = 7,
        ORDER_STATE_REQUEST_MODIFY = 8,
        ORDER_STATE_REQUEST_CANCEL = 9
    }
}