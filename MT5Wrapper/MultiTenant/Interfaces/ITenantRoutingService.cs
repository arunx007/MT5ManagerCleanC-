using System.Collections.Generic;
using Microsoft.AspNetCore.Http;
using MT5Wrapper.MultiTenant.Models;

namespace MT5Wrapper.MultiTenant.Interfaces
{
    /// <summary>
    /// Tenant routing service interface for API endpoint isolation
    /// </summary>
    public interface ITenantRoutingService
    {
        /// <summary>
        /// Get tenant information from HTTP request
        /// </summary>
        /// <param name="request">HTTP request</param>
        /// <returns>Tenant information or null if not found</returns>
        ManagerTenant? GetTenantFromRequest(HttpRequest request);

        /// <summary>
        /// Get API base URL for a tenant
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>API base URL</returns>
        string GetTenantApiBaseUrl(string tenantId);

        /// <summary>
        /// Check if tenant is active and can receive requests
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>Active status</returns>
        bool IsTenantActive(string tenantId);

        /// <summary>
        /// Check if tenant is within rate limits
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="endpoint">API endpoint being accessed</param>
        /// <returns>Within limits status</returns>
        bool CheckRateLimit(string tenantId, string endpoint);
    }
}