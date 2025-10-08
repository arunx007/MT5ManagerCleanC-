using System.Threading.Tasks;
using MT5Wrapper.MultiTenant.Models;

namespace MT5Wrapper.MultiTenant.Interfaces
{
    /// <summary>
    /// Tenant manager service interface for commercial API platform
    /// </summary>
    public interface ITenantManagerService
    {
        /// <summary>
        /// Create a new manager tenant
        /// </summary>
        /// <param name="request">Tenant creation request</param>
        /// <returns>Created tenant information</returns>
        Task<ManagerTenant> CreateTenantAsync(CreateTenantRequest request);

        /// <summary>
        /// Get tenant by ID
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>Tenant information or null if not found</returns>
        ManagerTenant? GetTenant(string tenantId);

        /// <summary>
        /// Update tenant subscription
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="subscription">New subscription information</param>
        /// <returns>Success status</returns>
        Task<bool> UpdateSubscriptionAsync(string tenantId, SubscriptionInfo subscription);

        /// <summary>
        /// Suspend a tenant (disable API access)
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>Success status</returns>
        Task<bool> SuspendTenantAsync(string tenantId);
    }
}