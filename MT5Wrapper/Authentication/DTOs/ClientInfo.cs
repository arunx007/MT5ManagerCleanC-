using System;
using System.Collections.Generic;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Client information for React Native apps - isolated DTO
    /// </summary>
    public class ClientInfo
    {
        /// <summary>
        /// Client unique identifier
        /// </summary>
        public string ClientId { get; set; } = string.Empty;

        /// <summary>
        /// React Native app ID
        /// </summary>
        public string AppId { get; set; } = string.Empty;

        /// <summary>
        /// Client display name
        /// </summary>
        public string? DisplayName { get; set; }

        /// <summary>
        /// Client email (optional)
        /// </summary>
        public string? Email { get; set; }

        /// <summary>
        /// Manager ID for multi-tenant isolation
        /// </summary>
        public string ManagerId { get; set; } = string.Empty;

        /// <summary>
        /// Allowed MT5 account logins for this client
        /// </summary>
        public List<string> AllowedAccountLogins { get; set; } = new();

        /// <summary>
        /// Client permissions
        /// </summary>
        public ClientPermissions Permissions { get; set; } = new();

        /// <summary>
        /// Device information
        /// </summary>
        public DeviceInfo? Device { get; set; }

        /// <summary>
        /// Whether the client is active
        /// </summary>
        public bool IsActive { get; set; } = true;

        /// <summary>
        /// Creation timestamp
        /// </summary>
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Last login timestamp
        /// </summary>
        public DateTime? LastLoginAt { get; set; }

        /// <summary>
        /// Subscription information
        /// </summary>
        public ClientSubscription? Subscription { get; set; }
    }

    /// <summary>
    /// Client permissions (limited compared to managers)
    /// </summary>
    public class ClientPermissions
    {
        /// <summary>
        /// Can read account information
        /// </summary>
        public bool CanReadAccount { get; set; } = true;

        /// <summary>
        /// Can read tick data
        /// </summary>
        public bool CanReadTickData { get; set; } = true;

        /// <summary>
        /// Can read chart data
        /// </summary>
        public bool CanReadCharts { get; set; } = true;

        /// <summary>
        /// Can read symbols
        /// </summary>
        public bool CanReadSymbols { get; set; } = true;

        /// <summary>
        /// Can modify accounts (false for clients)
        /// </summary>
        public bool CanModifyAccounts { get; set; } = false;

        /// <summary>
        /// Can access all accounts or only allowed ones
        /// </summary>
        public bool CanAccessAllAccounts { get; set; } = false;

        /// <summary>
        /// Maximum number of accounts this client can access
        /// </summary>
        public int MaxAccountAccess { get; set; } = 10;
    }

    /// <summary>
    /// Device information
    /// </summary>
    public class DeviceInfo
    {
        /// <summary>
        /// Device platform (iOS/Android)
        /// </summary>
        public string Platform { get; set; } = string.Empty;

        /// <summary>
        /// Device model
        /// </summary>
        public string Model { get; set; } = string.Empty;

        /// <summary>
        /// Device UUID
        /// </summary>
        public string DeviceId { get; set; } = string.Empty;

        /// <summary>
        /// App version
        /// </summary>
        public string AppVersion { get; set; } = string.Empty;

        /// <summary>
        /// OS version
        /// </summary>
        public string OSVersion { get; set; } = string.Empty;
    }

    /// <summary>
    /// Client subscription information
    /// </summary>
    public class ClientSubscription
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