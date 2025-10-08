using System.Collections.Generic;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Threading.Tasks;
using MT5Wrapper.UserManagement.DTOs;

namespace MT5Wrapper.UserManagement.Interfaces
{
    /// <summary>
    /// User management service interface - handles MT5 user account operations
    /// </summary>
    public interface IUserManagementService
    {
        /// <summary>
        /// Get user by login ID
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="login">User login ID</param>
        /// <returns>User information</returns>
        Task<UserDto?> GetUserAsync(string tenantId, ulong login);

        /// <summary>
        /// Get user account information (balance, equity, etc.)
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="login">User login ID</param>
        /// <returns>User account information</returns>
        Task<UserAccountDto?> GetUserAccountAsync(string tenantId, ulong login);

        /// <summary>
        /// Get all users for a tenant
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="group">Filter by group (optional)</param>
        /// <returns>List of users</returns>
        Task<IEnumerable<UserDto>> GetUsersAsync(string tenantId, string? group = null);

        /// <summary>
        /// Get users by group
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="group">Group name</param>
        /// <returns>List of users in the group</returns>
        Task<IEnumerable<UserDto>> GetUsersByGroupAsync(string tenantId, string group);

        /// <summary>
        /// Create new user
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="request">User creation request</param>
        /// <returns>Creation result</returns>
        Task<UserOperationResult> CreateUserAsync(string tenantId, CreateUserRequest request);

        /// <summary>
        /// Update existing user
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="request">User update request</param>
        /// <returns>Update result</returns>
        Task<UserOperationResult> UpdateUserAsync(string tenantId, UpdateUserRequest request);

        /// <summary>
        /// Delete user
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="login">User login ID</param>
        /// <returns>Deletion result</returns>
        Task<UserOperationResult> DeleteUserAsync(string tenantId, ulong login);

        /// <summary>
        /// Update user balance
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="request">Balance update request</param>
        /// <returns>Update result</returns>
        Task<UserOperationResult> UpdateUserBalanceAsync(string tenantId, UpdateBalanceRequest request);

        /// <summary>
        /// Get user groups
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>List of available groups</returns>
        Task<IEnumerable<GroupDto>> GetGroupsAsync(string tenantId);

        /// <summary>
        /// Create new group
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="request">Group creation request</param>
        /// <returns>Creation result</returns>
        Task<GroupOperationResult> CreateGroupAsync(string tenantId, CreateGroupRequest request);

        /// <summary>
        /// Update group settings
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="request">Group update request</param>
        /// <returns>Update result</returns>
        Task<GroupOperationResult> UpdateGroupAsync(string tenantId, UpdateGroupRequest request);

        /// <summary>
        /// Get total user count
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>Total number of users</returns>
        Task<int> GetTotalUsersAsync(string tenantId);

        /// <summary>
        /// Search users by criteria
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="criteria">Search criteria</param>
        /// <returns>Matching users</returns>
        Task<IEnumerable<UserDto>> SearchUsersAsync(string tenantId, UserSearchCriteria criteria);
    }
}