using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Security.Claims;
using System.Security.Cryptography;
using System.Text;
using MT5Wrapper.Authentication.Interfaces;
using MT5Wrapper.Authentication.DTOs;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Core.Models;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Microsoft.IdentityModel.Tokens;
using System.IdentityModel.Tokens.Jwt;

namespace MT5Wrapper.Authentication.Services
{
    /// <summary>
    /// Manager authentication service - handles 30-day JWT tokens for managers
    /// This service is isolated from other services to prevent cascading failures
    /// </summary>
    public class ManagerAuthService : IManagerAuthService
    {
        private readonly ILogger<ManagerAuthService> _logger;
        private readonly MT5WrapperConfig _config;
        private readonly IMT5ConnectionService _connectionService;
        private readonly Dictionary<string, string> _blacklistedTokens;

        public ManagerAuthService(
            ILogger<ManagerAuthService> logger,
            IOptions<MT5WrapperConfig> config,
            IMT5ConnectionService connectionService)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _config = config?.Value ?? throw new ArgumentNullException(nameof(config));
            _connectionService = connectionService ?? throw new ArgumentNullException(nameof(connectionService));
            _blacklistedTokens = new Dictionary<string, string>();
        }

        /// <inheritdoc/>
        public async Task<ManagerAuthResult> AuthenticateManagerAsync(ManagerLoginRequest request)
        {
            try
            {
                _logger.LogInformation("Authenticating manager {ManagerId} from {Server}", request.ManagerId, request.Server);

                // Validate request
                var validationResult = ValidateLoginRequest(request);
                if (!validationResult.IsValid)
                {
                    return ManagerAuthResult.CreateFailure(validationResult.ErrorMessage!);
                }

                // Test MT5 connection with provided credentials
                var connectionConfig = new MT5ConnectionConfig
                {
                    Server = request.Server,
                    Login = request.Login,
                    Password = request.Password,
                    ManagerId = request.ManagerId,
                    Description = request.Description,
                    EnableAutoReconnect = true,
                    AutoReconnectInterval = 30
                };

                var connectionResult = await _connectionService.ConnectAsync(connectionConfig);

                if (!connectionResult.Success)
                {
                    _logger.LogWarning("MT5 connection failed for manager {ManagerId}: {Error}", request.ManagerId, connectionResult.Message);
                    return ManagerAuthResult.CreateFailure("Invalid MT5 credentials or server unreachable");
                }

                // Create manager info
                var managerInfo = new ManagerInfo
                {
                    ManagerId = request.ManagerId,
                    Login = request.Login,
                    Server = request.Server,
                    Description = request.Description,
                    IsActive = true,
                    Permissions = new ManagerPermissions
                    {
                        CanAccessAllAccounts = true,
                        CanModifyUsers = true,
                        CanModifyGroups = true,
                        CanAccessTradingData = true,
                        CanAccessLiveData = true,
                        CanAccessHistoricalData = true
                    },
                    LastLoginAt = DateTime.UtcNow
                };

                // Generate JWT token with 30-day expiry
                var token = GenerateManagerToken(managerInfo, _config.Authentication.ManagerTokenExpiryDays);

                _logger.LogInformation("Manager {ManagerId} authenticated successfully", request.ManagerId);

                return ManagerAuthResult.CreateSuccess(token, managerInfo, _config.Authentication.ManagerTokenExpiryDays);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while authenticating manager {ManagerId}", request.ManagerId);
                return ManagerAuthResult.CreateFailure("Authentication service error");
            }
        }

        /// <inheritdoc/>
        public async Task<MT5Wrapper.Authentication.DTOs.TokenValidationResult> ValidateManagerTokenAsync(string token)
        {
            try
            {
                // Check if token is blacklisted
                if (_blacklistedTokens.ContainsKey(token))
                {
                    return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateInvalid("Token has been revoked");
                }

                // Validate JWT token
                var claimsPrincipal = ValidateJwtToken(token);
                if (claimsPrincipal == null)
                {
                    return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateInvalid("Invalid token");
                }

                // Extract manager info from claims
                var managerInfo = ExtractManagerInfoFromClaims(claimsPrincipal.Claims);
                if (managerInfo == null)
                {
                    return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateInvalid("Invalid token claims");
                }

                // Check if token is expired
                var expiryClaim = claimsPrincipal.FindFirst("exp");
                if (expiryClaim == null || !long.TryParse(expiryClaim.Value, out long expiryTimestamp))
                {
                    return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateInvalid("Invalid token expiry");
                }

                var expiryDateTime = DateTimeOffset.FromUnixTimeSeconds(expiryTimestamp).UtcDateTime;
                var now = DateTime.UtcNow;

                if (now >= expiryDateTime)
                {
                    return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateInvalid("Token has expired");
                }

                var timeUntilExpiry = (long)(expiryDateTime - now).TotalSeconds;

                return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateValid(managerInfo, expiryTimestamp, timeUntilExpiry);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while validating manager token");
                return MT5Wrapper.Authentication.DTOs.TokenValidationResult.CreateInvalid("Token validation error");
            }
        }

        /// <inheritdoc/>
        public async Task<TokenRefreshResult> RefreshManagerTokenAsync(string token)
        {
            try
            {
                // Validate current token
                var validationResult = await ValidateManagerTokenAsync(token);
                if (!validationResult.IsValid || validationResult.Manager == null)
                {
                    return TokenRefreshResult.CreateFailure("Invalid token");
                }

                // Check if token is close to expiry (less than 7 days)
                if (validationResult.TimeUntilExpiry > 7 * 24 * 3600)
                {
                    return TokenRefreshResult.CreateFailure("Token is not eligible for refresh yet");
                }

                // Generate new token
                var newToken = GenerateManagerToken(validationResult.Manager, _config.Authentication.ManagerTokenExpiryDays);

                _logger.LogInformation("Manager token refreshed for {ManagerId}", validationResult.Manager.ManagerId);

                return TokenRefreshResult.CreateSuccess(newToken, _config.Authentication.ManagerTokenExpiryDays);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while refreshing manager token");
                return TokenRefreshResult.CreateFailure("Token refresh error");
            }
        }

        /// <inheritdoc/>
        public async Task<RevokeTokenResult> RevokeManagerTokenAsync(string token)
        {
            try
            {
                // Validate token first
                var validationResult = await ValidateManagerTokenAsync(token);
                if (!validationResult.IsValid || validationResult.Manager == null)
                {
                    return RevokeTokenResult.CreateFailure("Invalid token");
                }

                // Add token to blacklist
                _blacklistedTokens[token] = validationResult.Manager.ManagerId;

                _logger.LogInformation("Manager token revoked for {ManagerId}", validationResult.Manager.ManagerId);

                return RevokeTokenResult.CreateSuccess(validationResult.Manager.ManagerId);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while revoking manager token");
                return RevokeTokenResult.CreateFailure("Token revocation error");
            }
        }

        /// <inheritdoc/>
        public async Task<ManagerInfo?> GetManagerInfoFromTokenAsync(string token)
        {
            var validationResult = await ValidateManagerTokenAsync(token);
            return validationResult.IsValid ? validationResult.Manager : null;
        }

        private (bool IsValid, string? ErrorMessage) ValidateLoginRequest(ManagerLoginRequest request)
        {
            if (string.IsNullOrWhiteSpace(request.Login))
                return (false, "Login is required");

            if (string.IsNullOrWhiteSpace(request.Password))
                return (false, "Password is required");

            if (string.IsNullOrWhiteSpace(request.Server))
                return (false, "Server address is required");

            if (string.IsNullOrWhiteSpace(request.ManagerId))
                return (false, "Manager ID is required");

            // Validate login is numeric
            if (!ulong.TryParse(request.Login, out _))
                return (false, "Login must be numeric");

            return (true, null);
        }

        private string GenerateManagerToken(ManagerInfo manager, int expiryDays)
        {
            var securityKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_config.Authentication.Jwt.SecretKey));
            var credentials = new SigningCredentials(securityKey, SecurityAlgorithms.HmacSha256);

            var claims = new[]
            {
                new Claim(JwtRegisteredClaimNames.Sub, manager.ManagerId),
                new Claim(JwtRegisteredClaimNames.Jti, Guid.NewGuid().ToString()),
                new Claim("manager_id", manager.ManagerId),
                new Claim("login", manager.Login),
                new Claim("server", manager.Server),
                new Claim("permissions", Newtonsoft.Json.JsonConvert.SerializeObject(manager.Permissions)),
                new Claim("token_type", "manager"),
                new Claim("expiry_days", expiryDays.ToString())
            };

            var token = new JwtSecurityToken(
                issuer: _config.Authentication.Jwt.Issuer,
                audience: _config.Authentication.Jwt.Audience,
                claims: claims,
                expires: DateTime.UtcNow.AddDays(expiryDays),
                signingCredentials: credentials);

            return new JwtSecurityTokenHandler().WriteToken(token);
        }

        private ClaimsPrincipal? ValidateJwtToken(string token)
        {
            try
            {
                var tokenHandler = new JwtSecurityTokenHandler();
                var securityKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_config.Authentication.Jwt.SecretKey));

                var validationParameters = new TokenValidationParameters
                {
                    ValidateIssuer = true,
                    ValidateAudience = true,
                    ValidateLifetime = true,
                    ValidateIssuerSigningKey = true,
                    ValidIssuer = _config.Authentication.Jwt.Issuer,
                    ValidAudience = _config.Authentication.Jwt.Audience,
                    IssuerSigningKey = securityKey,
                    ClockSkew = TimeSpan.Zero
                };

                return tokenHandler.ValidateToken(token, validationParameters, out _);
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "JWT token validation failed");
                return null;
            }
        }

        private ManagerInfo? ExtractManagerInfoFromClaims(IEnumerable<Claim> claims)
        {
            try
            {
                var managerId = claims.FirstOrDefault(c => c.Type == "manager_id")?.Value;
                var login = claims.FirstOrDefault(c => c.Type == "login")?.Value;
                var server = claims.FirstOrDefault(c => c.Type == "server")?.Value;
                var permissionsJson = claims.FirstOrDefault(c => c.Type == "permissions")?.Value;

                if (string.IsNullOrEmpty(managerId) || string.IsNullOrEmpty(login) || string.IsNullOrEmpty(server))
                    return null;

                var permissions = Newtonsoft.Json.JsonConvert.DeserializeObject<ManagerPermissions>(permissionsJson ?? "{}");

                return new ManagerInfo
                {
                    ManagerId = managerId,
                    Login = login,
                    Server = server,
                    Permissions = permissions ?? new ManagerPermissions(),
                    IsActive = true
                };
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error extracting manager info from token claims");
                return null;
            }
        }
    }
}