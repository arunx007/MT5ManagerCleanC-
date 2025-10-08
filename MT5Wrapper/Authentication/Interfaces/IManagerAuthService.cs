using System.Threading.Tasks;
using MT5Wrapper.Authentication.DTOs;

namespace MT5Wrapper.Authentication.Interfaces
{
    /// <summary>
    /// Manager authentication service interface - isolated from other services
    /// </summary>
    public interface IManagerAuthService
    {
        /// <summary>
        /// Authenticate manager with MT5 credentials and return JWT token
        /// </summary>
        /// <param name="request">Manager login request</param>
        /// <returns>Authentication result with token</returns>
        Task<ManagerAuthResult> AuthenticateManagerAsync(ManagerLoginRequest request);

        /// <summary>
        /// Validate manager JWT token
        /// </summary>
        /// <param name="token">JWT token to validate</param>
        /// <returns>Validation result</returns>
        Task<TokenValidationResult> ValidateManagerTokenAsync(string token);

        /// <summary>
        /// Refresh manager token
        /// </summary>
        /// <param name="token">Current valid token</param>
        /// <returns>New token result</returns>
        Task<TokenRefreshResult> RefreshManagerTokenAsync(string token);

        /// <summary>
        /// Revoke manager token
        /// </summary>
        /// <param name="token">Token to revoke</param>
        /// <returns>Revocation result</returns>
        Task<RevokeTokenResult> RevokeManagerTokenAsync(string token);

        /// <summary>
        /// Get manager information from token
        /// </summary>
        /// <param name="token">Valid JWT token</param>
        /// <returns>Manager information</returns>
        Task<ManagerInfo?> GetManagerInfoFromTokenAsync(string token);
    }
}