using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.UserManagement.Interfaces;
using MT5Wrapper.UserManagement.DTOs;
using Microsoft.Extensions.Logging;

namespace MT5Wrapper.Controllers
{
    [ApiController]
    [Route("api/users")]
    [Authorize]
    public class UserManagementController : ControllerBase
    {
        private readonly IUserManagementService _userManagementService;
        private readonly ILogger<UserManagementController> _logger;

        public UserManagementController(
            IUserManagementService userManagementService,
            ILogger<UserManagementController> logger)
        {
            _userManagementService = userManagementService ?? throw new ArgumentNullException(nameof(userManagementService));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        [HttpGet("{login}")]
        public async Task<IActionResult> GetUser(ulong login)
        {
            try
            {
                _logger.LogInformation("Getting user {Login}", login);

                // Check if user has manager permissions
                if (!HasManagerPermissions())
                {
                    return StatusCode(403, new { error = "Insufficient permissions", message = "Manager permissions required for user management" });
                }

                // Extract tenant ID from JWT token
                var tenantId = GetTenantIdFromToken();
                if (string.IsNullOrEmpty(tenantId))
                {
                    return BadRequest(new { error = "Invalid token", message = "Tenant ID not found in token" });
                }

                var user = await _userManagementService.GetUserAsync(tenantId, login);
                if (user == null)
                {
                    return NotFound(new { error = "User not found", message = $"User {login} not found" });
                }

                return Ok(user);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting user {Login}", login);
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpGet]
        public async Task<IActionResult> GetUsers([FromQuery] string? group = null, [FromQuery] int offset = 0, [FromQuery] int limit = 100)
        {
            try
            {
                _logger.LogInformation("Getting users with group: {Group}, offset: {Offset}, limit: {Limit}", group, offset, limit);

                // Extract tenant ID from JWT token
                var tenantId = GetTenantIdFromToken();
                if (string.IsNullOrEmpty(tenantId))
                {
                    return BadRequest(new { error = "Invalid token", message = "Tenant ID not found in token" });
                }

                var users = await _userManagementService.GetUsersAsync(tenantId, group);
                var result = users.Skip(offset).Take(limit);

                return Ok(new
                {
                    users = result,
                    total = users.Count(),
                    offset,
                    limit
                });
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting users");
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpPost]
        public async Task<IActionResult> CreateUser([FromBody] CreateUserRequest request)
        {
            try
            {
                _logger.LogInformation("Creating user {Login}", request.Login);

                // Extract tenant ID from JWT token
                var tenantId = GetTenantIdFromToken();
                if (string.IsNullOrEmpty(tenantId))
                {
                    return BadRequest(new { error = "Invalid token", message = "Tenant ID not found in token" });
                }

                var result = await _userManagementService.CreateUserAsync(tenantId, request);
                if (result.Success)
                {
                    return CreatedAtAction(nameof(GetUser), new { login = request.Login }, result);
                }

                return BadRequest(result);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception creating user");
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpPut("{login}")]
        public async Task<IActionResult> UpdateUser(ulong login, [FromBody] UpdateUserRequest request)
        {
            try
            {
                _logger.LogInformation("Updating user {Login}", login);

                // Ensure login matches
                request.Login = login;

                // Extract tenant ID from JWT token
                var tenantId = GetTenantIdFromToken();
                if (string.IsNullOrEmpty(tenantId))
                {
                    return BadRequest(new { error = "Invalid token", message = "Tenant ID not found in token" });
                }

                var result = await _userManagementService.UpdateUserAsync(tenantId, request);
                if (result.Success)
                {
                    return Ok(result);
                }

                return BadRequest(result);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception updating user {Login}", login);
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpDelete("{login}")]
        public async Task<IActionResult> DeleteUser(ulong login)
        {
            try
            {
                _logger.LogInformation("Deleting user {Login}", login);

                // Extract tenant ID from JWT token
                var tenantId = GetTenantIdFromToken();
                if (string.IsNullOrEmpty(tenantId))
                {
                    return BadRequest(new { error = "Invalid token", message = "Tenant ID not found in token" });
                }

                var result = await _userManagementService.DeleteUserAsync(tenantId, login);
                if (result.Success)
                {
                    return Ok(result);
                }

                return BadRequest(result);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception deleting user {Login}", login);
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpPost("{login}/balance")]
        public async Task<IActionResult> UpdateUserBalance(ulong login, [FromBody] UpdateBalanceRequest request)
        {
            try
            {
                _logger.LogInformation("Updating balance for user {Login}", login);

                // Ensure login matches
                request.Login = login;

                // Extract tenant ID from JWT token
                var tenantId = GetTenantIdFromToken();
                if (string.IsNullOrEmpty(tenantId))
                {
                    return BadRequest(new { error = "Invalid token", message = "Tenant ID not found in token" });
                }

                var result = await _userManagementService.UpdateUserBalanceAsync(tenantId, request);
                if (result.Success)
                {
                    return Ok(result);
                }

                return BadRequest(result);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception updating balance for user {Login}", login);
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpGet("test")]
        [AllowAnonymous]
        public IActionResult Test()
        {
            return Ok(new { message = "UserManagementController is working", timestamp = DateTime.UtcNow });
        }

        private string? GetTenantIdFromToken()
        {
            // Extract tenant ID from JWT token claims
            var tenantIdClaim = User.FindFirst("manager_id")?.Value ?? User.FindFirst("client_id")?.Value;
            return tenantIdClaim;
        }

        private bool IsManagerToken()
        {
            // Check if this is a manager token (not a client token)
            var tokenType = User.FindFirst("token_type")?.Value;
            return tokenType == "manager";
        }

        private bool HasManagerPermissions()
        {
            // Check if token has manager permissions
            if (!IsManagerToken()) return false;

            var permissionsJson = User.FindFirst("permissions")?.Value;
            if (string.IsNullOrEmpty(permissionsJson)) return false;

            try
            {
                // Parse permissions to check CanModifyUsers
                var permissions = Newtonsoft.Json.JsonConvert.DeserializeObject<dynamic>(permissionsJson);
                return permissions?.CanModifyUsers == true;
            }
            catch
            {
                return false;
            }
        }
    }
}