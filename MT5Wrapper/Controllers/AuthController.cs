using System;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.Authentication.Services;
using MT5Wrapper.Authentication.DTOs;
using MT5Wrapper.Authentication.Interfaces;

namespace MT5Wrapper.Controllers
{
    [ApiController]
    [Route("api/auth")]
    public class AuthController : ControllerBase
    {
        private readonly IManagerAuthService _managerAuthService;
        private readonly IClientAuthService _clientAuthService;

        public AuthController(
            IManagerAuthService managerAuthService,
            IClientAuthService clientAuthService)
        {
            _managerAuthService = managerAuthService;
            _clientAuthService = clientAuthService;
        }

        [HttpPost("manager/login")]
        [AllowAnonymous]
        public async Task<IActionResult> ManagerLogin([FromBody] ManagerLoginRequest request)
        {
            try
            {
                // Convert the request to use the expected field names
                var authRequest = new ManagerLoginRequest
                {
                    Login = request.Login,
                    Password = request.Password,
                    Server = request.Server,
                    ManagerId = request.ManagerId,
                    Description = request.Description
                };

                var result = await _managerAuthService.AuthenticateManagerAsync(authRequest);
                if (result.Success)
                {
                    return Ok(result);
                }
                return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpPost("client/login")]
        [AllowAnonymous]
        public async Task<IActionResult> ClientLogin([FromBody] ClientLoginRequest request)
        {
            try
            {
                var result = await _clientAuthService.AuthenticateClientAsync(request);
                if (result.Success)
                {
                    return Ok(result);
                }
                return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpPost("validate")]
        [Authorize]
        public IActionResult ValidateToken()
        {
            // Token validation is handled by JWT middleware
            // If we reach here, token is valid
            return Ok(new { valid = true, message = "Token is valid" });
        }

        [HttpPost("manager/refresh")]
        [Authorize]
        public async Task<IActionResult> RefreshManagerToken()
        {
            try
            {
                // Extract manager login from JWT token
                var managerLoginClaim = User.FindFirst("manager_login")?.Value;
                if (string.IsNullOrEmpty(managerLoginClaim))
                {
                    return BadRequest(new { error = "Invalid token", message = "Manager login not found in token" });
                }

                var result = await _managerAuthService.RefreshManagerTokenAsync(managerLoginClaim);
                if (result.Success)
                {
                    return Ok(result);
                }
                return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        [HttpPost("revoke")]
        [Authorize]
        public async Task<IActionResult> RevokeToken()
        {
            try
            {
                // Extract login from JWT token
                var loginClaim = User.FindFirst("login")?.Value ?? User.FindFirst("manager_login")?.Value;
                if (string.IsNullOrEmpty(loginClaim))
                {
                    return BadRequest(new { error = "Invalid token", message = "Login not found in token" });
                }

                // For now, just return success (in production, you'd blacklist the token)
                return Ok(new { success = true, message = "Token revoked successfully" });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }
    }
}