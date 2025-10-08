using System;
using System.Collections.Generic;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using MT5Wrapper.MultiTenant.Interfaces;
using MT5Wrapper.MultiTenant.Models;

namespace MT5Wrapper.MultiTenant.Services
{
    /// <summary>
    /// Tenant routing service - handles API endpoint isolation for multiple managers
    /// </summary>
    public class TenantRoutingService : ITenantRoutingService
    {
        private readonly ILogger<TenantRoutingService> _logger;
        private readonly ITenantManagerService _tenantManager;
        private readonly Dictionary<string, ManagerTenant> _routeCache;

        public TenantRoutingService(
            ILogger<TenantRoutingService> logger,
            ITenantManagerService tenantManager)
        {
            _logger = logger;
            _tenantManager = tenantManager;
            _routeCache = new Dictionary<string, ManagerTenant>();
        }

        /// <inheritdoc/>
        public ManagerTenant? GetTenantFromRequest(HttpRequest request)
        {
            // Try different methods to identify tenant

            // 1. From subdomain (mgr_a1b2c3d4.yourapi.com)
            var tenantFromSubdomain = GetTenantFromSubdomain(request.Host.Host);
            if (tenantFromSubdomain != null)
            {
                return tenantFromSubdomain;
            }

            // 2. From URL path (/api/mgr_a1b2c3d4/data)
            var tenantFromPath = GetTenantFromPath(request.Path);
            if (tenantFromPath != null)
            {
                return tenantFromPath;
            }

            // 3. From header (X-Manager-ID)
            var tenantFromHeader = GetTenantFromHeader(request.Headers);
            if (tenantFromHeader != null)
            {
                return tenantFromHeader;
            }

            _logger.LogWarning("No tenant identified for request: {Path}", request.Path);
            return null;
        }

        /// <inheritdoc/>
        public string GetTenantApiBaseUrl(string tenantId)
        {
            var tenant = _tenantManager.GetTenant(tenantId);
            return tenant?.ApiEndpoints.BaseUrl ?? $"https://{tenantId}.yourapi.com";
        }

        /// <inheritdoc/>
        public bool IsTenantActive(string tenantId)
        {
            var tenant = _tenantManager.GetTenant(tenantId);
            return tenant?.Status == TenantStatus.Active;
        }

        /// <inheritdoc/>
        public bool CheckRateLimit(string tenantId, string endpoint)
        {
            var tenant = _tenantManager.GetTenant(tenantId);
            if (tenant == null || tenant.Status != TenantStatus.Active)
            {
                return false;
            }

            // Check if tenant is within rate limits
            // Implementation would track requests per minute
            return true; // Simplified for example
        }

        private ManagerTenant? GetTenantFromSubdomain(string host)
        {
            // Extract tenant ID from subdomain
            // Example: mgr_a1b2c3d4.yourapi.com -> mgr_a1b2c3d4
            if (host.EndsWith(".yourapi.com"))
            {
                var subdomain = host.Replace(".yourapi.com", "");
                return _tenantManager.GetTenant(subdomain);
            }

            return null;
        }

        private ManagerTenant? GetTenantFromPath(string path)
        {
            // Extract tenant ID from path
            // Example: /api/mgr_a1b2c3d4/data/ticks -> mgr_a1b2c3d4
            var segments = path.Split('/', StringSplitOptions.RemoveEmptyEntries);
            if (segments.Length >= 2 && segments[0] == "api")
            {
                var tenantId = segments[1];
                return _tenantManager.GetTenant(tenantId);
            }

            return null;
        }

        private ManagerTenant? GetTenantFromHeader(IHeaderDictionary headers)
        {
            // Extract tenant ID from header
            if (headers.TryGetValue("X-Manager-ID", out var tenantId))
            {
                return _tenantManager.GetTenant(tenantId.ToString());
            }

            return null;
        }
    }
}