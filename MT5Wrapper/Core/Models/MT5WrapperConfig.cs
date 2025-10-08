using System.Collections.Generic;

namespace MT5Wrapper.Core.Models
{
    /// <summary>
    /// Main configuration for MT5 Wrapper - isolated model
    /// </summary>
    public class MT5WrapperConfig
    {
        /// <summary>
        /// Environment name (Development, Staging, Production)
        /// </summary>
        public string Environment { get; set; } = "Development";

        /// <summary>
        /// Version of the wrapper
        /// </summary>
        public string Version { get; set; } = "1.0.0";

        /// <summary>
        /// Multi-tenant configuration
        /// </summary>
        public MultiTenantConfig MultiTenant { get; set; } = new();

        /// <summary>
        /// Authentication configuration
        /// </summary>
        public AuthenticationConfig Authentication { get; set; } = new();

        /// <summary>
        /// MT5 specific configuration
        /// </summary>
        public MT5Config MT5 { get; set; } = new();

        /// <summary>
        /// Cache configuration
        /// </summary>
        public CacheConfig Cache { get; set; } = new();

        /// <summary>
        /// AWS configuration
        /// </summary>
        public AWSConfig AWS { get; set; } = new();
    }

    /// <summary>
    /// Multi-tenant configuration
    /// </summary>
    public class MultiTenantConfig
    {
        public bool Enabled { get; set; } = true;
        public string DefaultManagerId { get; set; } = "default";
        public string ManagerHeaderName { get; set; } = "X-Manager-ID";
    }

    /// <summary>
    /// Authentication configuration
    /// </summary>
    public class AuthenticationConfig
    {
        public int ManagerTokenExpiryDays { get; set; } = 30;
        public int ClientTokenExpiryDays { get; set; } = 0; // 0 = unlimited

        public JwtConfig Jwt { get; set; } = new();
    }

    /// <summary>
    /// JWT configuration
    /// </summary>
    public class JwtConfig
    {
        public string SecretKey { get; set; } = "your-super-secret-key-change-this-in-production";
        public string Issuer { get; set; } = "MT5Wrapper";
        public string Audience { get; set; } = "MT5Wrapper-Users";
    }

    /// <summary>
    /// MT5 specific configuration
    /// </summary>
    public class MT5Config
    {
        public string ManagerDllPath { get; set; } = "Libs/MetaQuotes.MT5ManagerAPI64.dll";
        public int ConnectionTimeout { get; set; } = 10000;
        public int RetryAttempts { get; set; } = 3;
        public List<string> PumpModes { get; set; } = new()
        {
            "PUMP_MODE_SYMBOLS",
            "PUMP_MODE_GROUPS",
            "PUMP_MODE_USERS"
        };
    }

    /// <summary>
    /// Cache configuration
    /// </summary>
    public class CacheConfig
    {
        public RedisConfig Redis { get; set; } = new();
        public int DefaultExpirationMinutes { get; set; } = 30;
    }

    /// <summary>
    /// Redis configuration
    /// </summary>
    public class RedisConfig
    {
        public string ConnectionString { get; set; } = "localhost:6379";
        public string InstanceName { get; set; } = "MT5Wrapper";
    }

    /// <summary>
    /// AWS configuration
    /// </summary>
    public class AWSConfig
    {
        public string Region { get; set; } = "us-east-1";
        public string Environment { get; set; } = "dev";
    }
}