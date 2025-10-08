using System;

using System;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// User account data transfer object - matches MT5 SDK CIMTAccount structure
    /// </summary>
    public class UserAccountDto
    {
        /// <summary>
        /// User login ID
        /// </summary>
        public ulong Login { get; set; }

        /// <summary>
        /// Currency digits
        /// </summary>
        public uint CurrencyDigits { get; set; }

        /// <summary>
        /// Account balance
        /// </summary>
        public double Balance { get; set; }

        /// <summary>
        /// Account credit
        /// </summary>
        public double Credit { get; set; }

        /// <summary>
        /// Used margin
        /// </summary>
        public double Margin { get; set; }

        /// <summary>
        /// Free margin
        /// </summary>
        public double MarginFree { get; set; }

        /// <summary>
        /// Margin level percentage
        /// </summary>
        public double MarginLevel { get; set; }

        /// <summary>
        /// Margin leverage
        /// </summary>
        public uint MarginLeverage { get; set; }

        /// <summary>
        /// Total profit
        /// </summary>
        public double Profit { get; set; }

        /// <summary>
        /// Storage fee
        /// </summary>
        public double Storage { get; set; }

        /// <summary>
        /// Floating P/L
        /// </summary>
        public double Floating { get; set; }

        /// <summary>
        /// Account equity
        /// </summary>
        public double Equity { get; set; }

        /// <summary>
        /// Stop-out activation level
        /// </summary>
        public double SOActivation { get; set; }

        /// <summary>
        /// Stop-out time
        /// </summary>
        public uint SOTime { get; set; }

        /// <summary>
        /// Stop-out level
        /// </summary>
        public double SOLevel { get; set; }

        /// <summary>
        /// Stop-out equity
        /// </summary>
        public double SOEquity { get; set; }

        /// <summary>
        /// Stop-out margin
        /// </summary>
        public double SOMargin { get; set; }

        /// <summary>
        /// Blocked commission
        /// </summary>
        public double BlockedCommission { get; set; }

        /// <summary>
        /// Blocked profit
        /// </summary>
        public double BlockedProfit { get; set; }

        /// <summary>
        /// Initial margin requirement
        /// </summary>
        public double MarginInitial { get; set; }

        /// <summary>
        /// Maintenance margin requirement
        /// </summary>
        public double MarginMaintenance { get; set; }

        /// <summary>
        /// Total assets
        /// </summary>
        public double Assets { get; set; }

        /// <summary>
        /// Total liabilities
        /// </summary>
        public double Liabilities { get; set; }

        /// <summary>
        /// Tenant ID (for multi-tenant isolation)
        /// </summary>
        public string TenantId { get; set; } = string.Empty;

        /// <summary>
        /// Create DTO from MT5 CIMTAccount object
        /// </summary>
        public static UserAccountDto FromMT5Account(CIMTAccount account, string tenantId)
        {
            return new UserAccountDto
            {
                Login = account.Login(),
                CurrencyDigits = account.CurrencyDigits(),
                Balance = account.Balance(),
                Credit = account.Credit(),
                Margin = account.Margin(),
                MarginFree = account.MarginFree(),
                MarginLevel = account.MarginLevel(),
                MarginLeverage = account.MarginLeverage(),
                Profit = account.Profit(),
                Storage = account.Storage(),
                Floating = account.Floating(),
                Equity = account.Equity(),
                SOActivation = account.SOActivation(),
                SOTime = (uint)account.SOTime(),
                SOLevel = account.SOLevel(),
                SOEquity = account.SOEquity(),
                SOMargin = account.SOMargin(),
                BlockedCommission = account.BlockedCommission(),
                BlockedProfit = account.BlockedProfit(),
                MarginInitial = account.MarginInitial(),
                MarginMaintenance = account.MarginMaintenance(),
                Assets = account.Assets(),
                Liabilities = account.Liabilities(),
                TenantId = tenantId
            };
        }

        /// <summary>
        /// Check if account is valid
        /// </summary>
        public bool IsValid()
        {
            return Login > 0 && Balance >= 0;
        }

        /// <summary>
        /// Get total account value (balance + credit)
        /// </summary>
        public double GetTotalValue()
        {
            return Balance + Credit;
        }

        /// <summary>
        /// Check if account has sufficient margin
        /// </summary>
        public bool HasSufficientMargin()
        {
            return MarginLevel > 100; // Standard MT5 margin level check
        }
    }
}