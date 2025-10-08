using System;

namespace MT5Wrapper.Trading.DTOs
{
    public class TradeDto
    {
        public ulong TradeId { get; set; }
        public ulong ClientLogin { get; set; }
        public ulong OrderId { get; set; }
        public ulong PositionId { get; set; }
        public string Symbol { get; set; } = "";
        public TradeType Type { get; set; }
        public double Volume { get; set; }
        public double Price { get; set; }
        public double Profit { get; set; }
        public double Commission { get; set; }
        public double Swap { get; set; }
        public double Fee { get; set; }
        public DateTime Time { get; set; }
        public string Comment { get; set; } = "";
        public int Magic { get; set; }
        public string ExternalId { get; set; } = "";
        public TradeReason Reason { get; set; }
    }

    public enum TradeType
    {
        Buy = 0,
        Sell = 1,
        Balance = 2,
        Credit = 3,
        Charge = 4,
        Correction = 5,
        Bonus = 6,
        Commission = 7,
        CommissionDaily = 8,
        CommissionMonthly = 9,
        CommissionAgentDaily = 10,
        CommissionAgentMonthly = 11,
        Interest = 12,
        BuyCanceled = 13,
        SellCanceled = 14,
        Dividend = 15,
        DividendFranked = 16,
        Tax = 17
    }

    public enum TradeReason
    {
        Client = 0,
        Expert = 1,
        Dealer = 2,
        Signal = 3,
        Gateway = 4,
        Mobile = 5,
        Web = 6,
        Api = 7
    }
}