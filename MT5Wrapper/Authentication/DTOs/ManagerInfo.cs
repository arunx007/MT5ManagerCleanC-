using System;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Manager information - isolated DTO
    /// </summary>
    public class ManagerInfo
    {
        /// <summary>
        /// Manager unique identifier
        /// </summary>
        public string ManagerId { get; set; } = string.Empty;

        /// <summary>
        /// Manager login (numeric)
        /// </summary>
        public string Login { get; set; } = string.Empty;

        /// <summary>
        /// Manager server address
        /// </summary>
        public string Server { get; set; } = string.Empty;

        /// <summary>
        /// Manager description
        /// </summary>
        public string? Description { get; set; }

        /// <summary>
        /// Whether the manager is active
        /// </summary>
        public bool IsActive { get; set; } = true;

        /// <summary>
        /// Manager permissions
        /// </summary>
        public ManagerPermissions Permissions { get; set; } = new();

        /// <summary>
        /// Subscription information
        /// </summary>
        public ManagerSubscription? Subscription { get; set; }

        /// <summary>
        /// Creation timestamp
        /// </summary>
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Last login timestamp
        /// </summary>
        public DateTime? LastLoginAt { get; set; }
    }

    /// <summary>
    /// Manager permissions
    /// </summary>
    public class ManagerPermissions
    {
        /// <summary>
        /// Can access all client accounts
        /// </summary>
        public bool CanAccessAllAccounts { get; set; } = true;

        /// <summary>
        /// Can modify user accounts
        /// </summary>
        public bool CanModifyUsers { get; set; } = true;

        /// <summary>
        /// Can modify groups
        /// </summary>
        public bool CanModifyGroups { get; set; } = true;

        /// <summary>
        /// Can access trading data
        /// </summary>
        public bool CanAccessTradingData { get; set; } = true;

        /// <summary>
        /// Can access live tick data
        /// </summary>
        public bool CanAccessLiveData { get; set; } = true;

        /// <summary>
        /// Can access historical data
        /// </summary>
        public bool CanAccessHistoricalData { get; set; } = true;
    }

    /// <summary>
    /// Manager subscription information
    /// </summary>
    public class ManagerSubscription
    {
        /// <summary>
        /// Subscription tier
        /// </summary>
        public string Tier { get; set; } = string.Empty;

        /// <summary>
        /// Subscription status
        /// </summary>
        public string Status { get; set; } = "Active";

        /// <summary>
        /// Monthly API call limit
        /// </summary>
        public long MonthlyCallLimit { get; set; }

        /// <summary>
        /// Current month API calls used
        /// </summary>
        public long CurrentMonthCalls { get; set; }

        /// <summary>
        /// Subscription start date
        /// </summary>
        public DateTime StartDate { get; set; }

        /// <summary>
        /// Subscription end date
        /// </summary>
        public DateTime? EndDate { get; set; }

        /// <summary>
        /// Auto-renewal enabled
        /// </summary>
        public bool AutoRenewal { get; set; }
    }
}