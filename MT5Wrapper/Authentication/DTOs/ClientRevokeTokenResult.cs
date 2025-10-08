using System;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Client token revocation result - isolated DTO
    /// </summary>
    public class ClientRevokeTokenResult
    {
        /// <summary>
        /// Whether revocation was successful
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// Client ID that was revoked
        /// </summary>
        public string? ClientId { get; set; }

        /// <summary>
        /// Timestamp when token was revoked
        /// </summary>
        public DateTime RevokedAt { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Error message if revocation failed
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Create successful revocation result
        /// </summary>
        public static ClientRevokeTokenResult CreateSuccess(string clientId)
        {
            return new ClientRevokeTokenResult
            {
                Success = true,
                ClientId = clientId
            };
        }

        /// <summary>
        /// Create failed revocation result
        /// </summary>
        public static ClientRevokeTokenResult CreateFailure(string errorMessage)
        {
            return new ClientRevokeTokenResult
            {
                Success = false,
                ErrorMessage = errorMessage
            };
        }
    }
}