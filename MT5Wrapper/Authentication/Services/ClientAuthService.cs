using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Security.Claims;
using System.Text;
using MT5Wrapper.Authentication.Interfaces;
using MT5Wrapper.Authentication.DTOs;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Core.Models;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Microsoft.IdentityModel.Tokens;
using System.IdentityModel.Tokens.Jwt;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.Authentication.Services
{
    /// <summary>
    /// Client authentication service - handles client account authentication
    /// Follows same pattern as ManagerAuthService for consistency
    /// </summary>
    public class ClientAuthService : IClientAuthService
    {
        private readonly ILogger<ClientAuthService> _logger;
        private readonly MT5WrapperConfig _config;
        private readonly IMT5ConnectionService _connectionService;

        public ClientAuthService(
            ILogger<ClientAuthService> logger,
            IOptions<MT5WrapperConfig> config,
            IMT5ConnectionService connectionService)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _config = config?.Value ?? throw new ArgumentNullException(nameof(config));
            _connectionService = connectionService ?? throw new ArgumentNullException(nameof(connectionService));
        }

        /// <inheritdoc/>
        public async Task<ClientAuthResult> AuthenticateClientAsync(ClientLoginRequest request)
        {
            try
            {
                _logger.LogInformation("Authenticating client {Login} from {Server} for manager {ManagerId}",
                    request.Login, request.Server, request.ManagerId);

                // Validate request
                var validationResult = ValidateLoginRequest(request);
                if (!validationResult.IsValid)
                {
                    return ClientAuthResult.CreateFailure(validationResult.ErrorMessage!);
                }

                // Validate client credentials against MT5 server
                var isValid = await ValidateClientCredentialsAsync(request);
                if (!isValid)
                {
                    _logger.LogWarning("Invalid client credentials for login: {Login}", request.Login);
                    return ClientAuthResult.CreateFailure("Invalid login credentials");
                }

                // Create client info
                var clientInfo = new ClientInfo
                {
                    ClientId = request.Login,
                    AppId = "mt5-client", // Default app ID for direct MT5 clients
                    ManagerId = request.ManagerId,
                    DisplayName = $"Client {request.Login}",
                    IsActive = true,
                    AllowedAccountLogins = new List<string> { request.Login }, // Client can only access their own account
                    Permissions = new ClientPermissions
                    {
                        CanReadAccount = true,
                        CanReadTickData = true,
                        CanReadCharts = true,
                        CanReadSymbols = true,
                        CanModifyAccounts = false, // Clients cannot modify accounts
                        CanAccessAllAccounts = false,
                        MaxAccountAccess = 1 // Only their own account
                    },
                    Device = new DeviceInfo
                    {
                        Platform = "MT5",
                        Model = "Direct Client",
                        DeviceId = $"client-{request.Login}",
                        AppVersion = "1.0.0",
                        OSVersion = "MT5"
                    },
                    LastLoginAt = DateTime.UtcNow
                };

                // Generate tokens (unlimited duration for client tokens)
                var token = GenerateClientToken(clientInfo);

                _logger.LogInformation("Client {Login} authenticated successfully for manager {ManagerId}",
                    request.Login, request.ManagerId);

                return ClientAuthResult.CreateSuccess(token, null, clientInfo); // No refresh token for clients
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while authenticating client {Login}", request.Login);
                return ClientAuthResult.CreateFailure("Authentication service error");
            }
        }

        /// <inheritdoc/>
        public async Task<ClientTokenValidationResult> ValidateClientTokenAsync(string token)
        {
            try
            {
                // Validate JWT token
                var claimsPrincipal = ValidateJwtToken(token);
                if (claimsPrincipal == null)
                {
                    return ClientTokenValidationResult.CreateInvalid("Invalid token");
                }

                // Extract client info from claims
                var clientInfo = ExtractClientInfoFromClaims(claimsPrincipal.Claims);
                if (clientInfo == null)
                {
                    return ClientTokenValidationResult.CreateInvalid("Invalid token claims");
                }

                // Check if the token type is correct
                var tokenType = claimsPrincipal.FindFirst("token_type")?.Value;
                if (tokenType != "client")
                {
                    return ClientTokenValidationResult.CreateInvalid("Invalid token type");
                }

                return ClientTokenValidationResult.CreateValid(clientInfo, -1, -1); // Unlimited
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while validating client token");
                return ClientTokenValidationResult.CreateInvalid("Token validation error");
            }
        }

        /// <inheritdoc/>
        public async Task<ClientTokenRefreshResult> RefreshClientTokenAsync(string token)
        {
            // Client tokens don't support refresh (they're unlimited)
            return ClientTokenRefreshResult.CreateFailure("Client tokens do not support refresh");
        }

        /// <inheritdoc/>
        public async Task<ClientRevokeTokenResult> RevokeClientTokenAsync(string token)
        {
            // For now, just return success (in production, you'd blacklist the token)
            return ClientRevokeTokenResult.CreateSuccess("Token revoked");
        }

        /// <inheritdoc/>
        public async Task<ClientInfo?> GetClientInfoFromTokenAsync(string token)
        {
            var validationResult = await ValidateClientTokenAsync(token);
            return validationResult.IsValid ? validationResult.Client : null;
        }

        /// <inheritdoc/>
        public async Task<bool> ValidateReactNativeAppAsync(string appId, string appSecret)
        {
            // Simplified - always return true for direct MT5 clients
            return true;
        }

        private (bool IsValid, string? ErrorMessage) ValidateLoginRequest(ClientLoginRequest request)
        {
            if (string.IsNullOrWhiteSpace(request.Login))
                return (false, "Login is required");

            if (string.IsNullOrWhiteSpace(request.Password))
                return (false, "Password is required");

            if (string.IsNullOrWhiteSpace(request.Server))
                return (false, "Server address is required");

            if (string.IsNullOrWhiteSpace(request.ManagerId))
                return (false, "Manager ID is required");

            return (true, null);
        }

        private async Task<bool> ValidateClientCredentialsAsync(ClientLoginRequest request)
        {
            try
            {
                // For client authentication, we validate against MT5 by attempting to get user info
                // This is simpler than full connection validation
                var manager = _connectionService.Manager;
                if (manager == null)
                {
                    _logger.LogWarning("MT5 manager not connected for client validation");
                    return false;
                }

                // Try to parse login to ulong
                if (!ulong.TryParse(request.Login, out ulong loginId))
                {
                    _logger.LogWarning("Invalid login format: {Login}", request.Login);
                    return false;
                }

                // Create user object and try to get user info
                var user = manager.UserCreate();
                if (user == null)
                {
                    _logger.LogWarning("Failed to create MT5 user object for validation");
                    return false;
                }

                var result = manager.UserGet(loginId, user);
                user.Release();

                if (result != MTRetCode.MT_RET_OK)
                {
                    _logger.LogWarning("User {Login} not found or invalid: {Result}", request.Login, (int)result);
                    return false;
                }

                return true;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception during client credential validation for {Login}", request.Login);
                return false;
            }
        }

        private string GenerateClientToken(ClientInfo client)
        {
            var securityKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(_config.Authentication.Jwt.SecretKey));
            var credentials = new SigningCredentials(securityKey, SecurityAlgorithms.HmacSha256);

            var claims = new[]
            {
                new Claim(JwtRegisteredClaimNames.Sub, client.ClientId),
                new Claim(JwtRegisteredClaimNames.Jti, Guid.NewGuid().ToString()),
                new Claim("client_id", client.ClientId),
                new Claim("app_id", client.AppId),
                new Claim("manager_id", client.ManagerId),
                new Claim("login", client.ClientId), // Add login claim for consistency
                new Claim("permissions", Newtonsoft.Json.JsonConvert.SerializeObject(client.Permissions)),
                new Claim("allowed_accounts", Newtonsoft.Json.JsonConvert.SerializeObject(client.AllowedAccountLogins)),
                new Claim("token_type", "client"),
                new Claim("device_id", client.Device?.DeviceId ?? ""),
                new Claim("platform", client.Device?.Platform ?? "")
            };

            var token = new JwtSecurityToken(
                issuer: _config.Authentication.Jwt.Issuer,
                audience: _config.Authentication.Jwt.Audience,
                claims: claims,
                expires: null, // No expiration for client tokens (unlimited)
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
                    ValidateLifetime = false, // Don't validate lifetime for unlimited tokens
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

        private ClientInfo? ExtractClientInfoFromClaims(IEnumerable<Claim> claims)
        {
            try
            {
                var clientId = claims.FirstOrDefault(c => c.Type == "client_id")?.Value;
                var appId = claims.FirstOrDefault(c => c.Type == "app_id")?.Value;
                var managerId = claims.FirstOrDefault(c => c.Type == "manager_id")?.Value;
                var permissionsJson = claims.FirstOrDefault(c => c.Type == "permissions")?.Value;
                var allowedAccountsJson = claims.FirstOrDefault(c => c.Type == "allowed_accounts")?.Value;
                var deviceId = claims.FirstOrDefault(c => c.Type == "device_id")?.Value;
                var platform = claims.FirstOrDefault(c => c.Type == "platform")?.Value;

                if (string.IsNullOrEmpty(clientId) || string.IsNullOrEmpty(appId) || string.IsNullOrEmpty(managerId))
                    return null;

                var permissions = Newtonsoft.Json.JsonConvert.DeserializeObject<ClientPermissions>(permissionsJson ?? "{}");
                var allowedAccounts = Newtonsoft.Json.JsonConvert.DeserializeObject<List<string>>(allowedAccountsJson ?? "[]");

                return new ClientInfo
                {
                    ClientId = clientId,
                    AppId = appId,
                    ManagerId = managerId,
                    Permissions = permissions ?? new ClientPermissions(),
                    AllowedAccountLogins = allowedAccounts ?? new List<string>(),
                    IsActive = true,
                    Device = new DeviceInfo
                    {
                        DeviceId = deviceId ?? "",
                        Platform = platform ?? "Unknown"
                    }
                };
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error extracting client info from token claims");
                return null;
            }
        }
    }
}