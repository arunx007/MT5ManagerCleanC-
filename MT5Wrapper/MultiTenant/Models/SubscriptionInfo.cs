using System;
using System.Collections.Generic;

namespace MT5Wrapper.MultiTenant.Models
{
    /// <summary>
    /// Subscription information for commercial API platform
    /// </summary>
    public class SubscriptionInfo
    {
        /// <summary>
        /// Subscription plan identifier
        /// </summary>
        public string PlanId { get; set; } = string.Empty;

        /// <summary>
        /// Plan name (Starter, Professional, Enterprise)
        /// </summary>
        public string PlanName { get; set; } = string.Empty;

        /// <summary>
        /// Subscription status
        /// </summary>
        public SubscriptionStatus Status { get; set; } = SubscriptionStatus.Active;

        /// <summary>
        /// Monthly price in USD
        /// </summary>
        public decimal MonthlyPrice { get; set; }

        /// <summary>
        /// API call limit per month
        /// </summary>
        public long MonthlyCallLimit { get; set; }

        /// <summary>
        /// Current month API calls used
        /// </summary>
        public long CurrentMonthCalls { get; set; }

        /// <summary>
        /// Maximum concurrent connections
        /// </summary>
        public int MaxConnections { get; set; }

        /// <summary>
        /// Rate limit per minute
        /// </summary>
        public int RateLimit { get; set; }

        /// <summary>
        /// Maximum number of MT5 managers
        /// </summary>
        public int MaxMT5Managers { get; set; }

        /// <summary>
        /// Maximum number of client apps
        /// </summary>
        public int MaxClientApps { get; set; }

        /// <summary>
        /// WebSocket connections limit
        /// </summary>
        public int MaxWebSocketConnections { get; set; }

        /// <summary>
        /// Enabled features for this plan
        /// </summary>
        public List<string> Features { get; set; } = new();

        /// <summary>
        /// Subscription start date
        /// </summary>
        public DateTime StartDate { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Subscription end date (for prepaid plans)
        /// </summary>
        public DateTime? EndDate { get; set; }

        /// <summary>
        /// Auto-renewal enabled
        /// </summary>
        public bool AutoRenewal { get; set; } = true;

        /// <summary>
        /// Payment method identifier
        /// </summary>
        public string? PaymentMethodId { get; set; }

        /// <summary>
        /// Whether subscription is active
        /// </summary>
        public bool IsActive => Status == SubscriptionStatus.Active;

        /// <summary>
        /// Days until subscription expires
        /// </summary>
        public int DaysUntilExpiry
        {
            get
            {
                if (EndDate.HasValue)
                {
                    return Math.Max(0, (int)(EndDate.Value - DateTime.UtcNow).TotalDays);
                }
                return int.MaxValue; // No expiry for auto-renewal
            }
        }

        /// <summary>
        /// Usage percentage of monthly limit
        /// </summary>
        public double UsagePercentage => MonthlyCallLimit > 0 ? (double)CurrentMonthCalls / MonthlyCallLimit * 100 : 0;
    }

    /// <summary>
    /// Subscription status enumeration
    /// </summary>
    public enum SubscriptionStatus
    {
        Active,
        Suspended,
        Cancelled,
        Expired,
        Trial
    }
}