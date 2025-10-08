using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.UserManagement.Interfaces;
using MT5Wrapper.UserManagement.DTOs;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// Administrative Controller - Handles group and user administration
    /// Manager-only operations for system administration
    /// </summary>
    [ApiController]
    [Route("api/admin")]
    [Authorize] // All admin operations require authentication
    public class AdminController : ControllerBase
    {
        private readonly IUserManagementService _userManagementService;

        public AdminController(IUserManagementService userManagementService)
        {
            _userManagementService = userManagementService ?? throw new ArgumentNullException(nameof(userManagementService));
        }

        /// <summary>
        /// Get all groups for the tenant
        /// </summary>
        [HttpGet("groups")]
        public async Task<IActionResult> GetGroups()
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                var groups = await _userManagementService.GetGroupsAsync(tenantId);
                return Ok(new { groups = groups, count = groups.Count() });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Create a new user group
        /// </summary>
        [HttpPost("groups")]
        public async Task<IActionResult> CreateGroup([FromBody] CreateGroupRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Group request cannot be null" });

                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                var result = await _userManagementService.CreateGroupAsync(tenantId, request);

                if (result.Success)
                    return Ok(result);
                else
                    return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Update group settings
        /// </summary>
        [HttpPut("groups")]
        public async Task<IActionResult> UpdateGroup([FromBody] UpdateGroupRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Group update request cannot be null" });

                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                var result = await _userManagementService.UpdateGroupAsync(tenantId, request);

                if (result.Success)
                    return Ok(result);
                else
                    return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Delete a user group
        /// </summary>
        [HttpDelete("groups/{groupName}")]
        public async Task<IActionResult> DeleteGroup(string groupName)
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                // Note: MT5 API may not support group deletion
                // This would typically disable the group instead
                return BadRequest(new { error = "Operation not supported", message = "Group deletion is not supported by MT5 API" });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get users in a specific group
        /// </summary>
        [HttpGet("groups/{groupName}/users")]
        public async Task<IActionResult> GetGroupUsers(string groupName)
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                var users = await _userManagementService.GetUsersByGroupAsync(tenantId, groupName);
                return Ok(new { group = groupName, users = users, count = users.Count() });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Update group membership for users
        /// </summary>
        [HttpPut("groups/users")]
        public async Task<IActionResult> UpdateGroupUsers([FromBody] UpdateGroupUsersRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Group users update request cannot be null" });

                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                // This would typically involve updating user records
                // For now, return not implemented
                return BadRequest(new { error = "Operation not implemented", message = "Group membership updates require custom implementation" });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get system statistics
        /// </summary>
        [HttpGet("stats")]
        public async Task<IActionResult> GetSystemStats()
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                var totalUsers = await _userManagementService.GetTotalUsersAsync(tenantId);
                var groups = await _userManagementService.GetGroupsAsync(tenantId);

                return Ok(new
                {
                    tenantId = tenantId,
                    totalUsers = totalUsers,
                    totalGroups = groups.Count(),
                    timestamp = DateTimeOffset.UtcNow
                });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        // Helper methods to extract data from JWT tokens
        private string GetTenantIdFromJwt()
        {
            // Extract tenant ID from JWT claims
            var tenantId = User.FindFirst("tenant_id")?.Value ??
                          User.FindFirst("manager_id")?.Value ??
                          "test_mgr_001"; // Default fallback
            return tenantId;
        }
    }

    // Request DTOs for the admin controller
    public class UpdateGroupUsersRequest
    {
        public string GroupName { get; set; } = "";
        public IEnumerable<ulong> UserLogins { get; set; } = new List<ulong>();
        public bool AddToGroup { get; set; } = true; // true = add, false = remove
    }
}