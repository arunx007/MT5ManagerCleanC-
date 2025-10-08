using System.Threading.Tasks;
using MT5Wrapper.Authentication.DTOs;

namespace MT5Wrapper.Authentication.Interfaces
{
    /// <summary>
    /// Client authentication service interface for React Native apps - isolated from other services
    /// </summary>
    public interface IClientAuthService
    {
        /// <summary>
        /// Authenticate React Native client app and return unlimited JWT token
        /// </summary>
        /// <param name="request">Client login request</param>
        /// <returns>Authentication result with unlimited token</returns>
        Task<ClientAuthResult> AuthenticateClientAsync(ClientLoginRequest request);

        /// <summary>
        /// Validate client JWT token (unlimited duration)
        /// </summary>
        /// <param name="token">JWT token to validate</param>
        /// <returns>Validation result</returns>
        Task<ClientTokenValidationResult> ValidateClientTokenAsync(string token);

        /// <summary>
        /// Refresh client token (unlimited duration tokens don't need refresh but can be regenerated)
        /// </summary>
        /// <param name="token">Current valid token</param>
        /// <returns>New token result</returns>
        Task<ClientTokenRefreshResult> RefreshClientTokenAsync(string token);

        /// <summary>
        /// Revoke client token
        /// </summary>
        /// <param name="token">Token to revoke</param>
        /// <returns>Revocation result</returns>
        Task<ClientRevokeTokenResult> RevokeClientTokenAsync(string token);

        /// <summary>
        /// Get client information from token
        /// </summary>
        /// <param name="token">Valid JWT token</param>
        /// <returns>Client information</returns>
        Task<ClientInfo?> GetClientInfoFromTokenAsync(string token);

        /// <summary>
        /// Validate React Native app credentials
        /// </summary>
        /// <param name="appId">React Native app ID</param>
        /// <param name="appSecret">React Native app secret</param>
        /// <returns>Validation result</returns>
        Task<bool> ValidateReactNativeAppAsync(string appId, string appSecret);
    }
}