using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.MultiTenant.Models
{
    /// <summary>
    /// Manager tenant model for commercial API platform
    /// </summary>
    public class ManagerTenant
    {
        /// <summary>
        /// Unique tenant identifier (e.g., "mgr_a1b2c3d4")
        /// </summary>
        [Required]
        public string TenantId { get; set; } = string.Empty;

        /// <summary>
        /// Manager company name
        /// </summary>
        [Required]
        public string ManagerName { get; set; } = string.Empty;

        /// <summary>
        /// Company name
        /// </summary>
        public string? CompanyName { get; set; }

        /// <summary>
        /// Contact email
        /// </summary>
        [Required]
        [EmailAddress]
        public string Email { get; set; } = string.Empty;

        /// <summary>
        /// Subscription information
        /// </summary>
        [Required]
        public SubscriptionInfo Subscription { get; set; } = new();

        /// <summary>
        /// Tenant status
        /// </summary>
        public TenantStatus Status { get; set; } = TenantStatus.Active;

        /// <summary>
        /// API endpoints for this tenant
        /// </summary>
        [Required]
        public ApiEndpoints ApiEndpoints { get; set; } = new();

        /// <summary>
        /// Tenant-specific configuration
        /// </summary>
        [Required]
        public TenantConfiguration Configuration { get; set; } = new();

        /// <summary>
        /// Creation timestamp
        /// </summary>
        public DateTime CreatedAt { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Last update timestamp
        /// </summary>
        public DateTime? UpdatedAt { get; set; }

        /// <summary>
        /// MT5 Manager configurations for this tenant
        /// </summary>
        public List<MT5ManagerConfig> MT5Managers { get; set; } = new();
    }

    /// <summary>
    /// Tenant status enumeration
    /// </summary>
    public enum TenantStatus
    {
        Active,
        Suspended,
        Cancelled,
        Trial
    }

    /// <summary>
    /// API endpoints for a tenant
    /// </summary>
    public class ApiEndpoints
    {
        public string BaseUrl { get; set; } = string.Empty;
        public string AuthEndpoint { get; set; } = string.Empty;
        public string DataEndpoint { get; set; } = string.Empty;
        public string WebSocketEndpoint { get; set; } = string.Empty;
    }

    /// <summary>
    /// Tenant-specific configuration
    /// </summary>
    public class TenantConfiguration
    {
        public string DatabaseConnectionString { get; set; } = string.Empty;
        public string RedisConnectionString { get; set; } = string.Empty;
        public int MaxConcurrentConnections { get; set; } = 10;
        public int RateLimitPerMinute { get; set; } = 1000;
        public List<string> Features { get; set; } = new();
    }

    /// <summary>
    /// MT5 Manager configuration for a tenant
    /// </summary>
    public class MT5ManagerConfig
    {
        public string ManagerId { get; set; } = string.Empty;
        public string Server { get; set; } = string.Empty;
        public string Login { get; set; } = string.Empty;
        public string Password { get; set; } = string.Empty; // Encrypted
        public bool IsActive { get; set; } = true;
        public int Priority { get; set; } = 1;
    }
}