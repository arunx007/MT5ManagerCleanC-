using System;

namespace MT5Wrapper.Trading.DTOs
{
    public class AccountTradingInfoDto
    {
        public ulong ClientLogin { get; set; }
        public double Balance { get; set; }
        public double Credit { get; set; }
        public double Profit { get; set; }
        public double Equity { get; set; }
        public double Margin { get; set; }
        public double MarginFree { get; set; }
        public double MarginLevel { get; set; }
        public double MarginLeverage { get; set; }
        public int TotalOrders { get; set; }
        public int TotalPositions { get; set; }
        public int TotalDeals { get; set; }
        public DateTime LastUpdate { get; set; }
        public AccountTradingStatus Status { get; set; }
    }

    public class MarginInfoDto
    {
        public ulong ClientLogin { get; set; }
        public double Margin { get; set; }
        public double MarginFree { get; set; }
        public double MarginLevel { get; set; }
        public double MarginLeverage { get; set; }
        public double MarginInitial { get; set; }
        public double MarginMaintenance { get; set; }
        public bool MarginCall { get; set; }
        public bool StopOut { get; set; }
        public DateTime LastUpdate { get; set; }
    }

    public enum AccountTradingStatus
    {
        Normal = 0,
        MarginCall = 1,
        StopOut = 2,
        Disabled = 3,
        Blocked = 4
    }
}