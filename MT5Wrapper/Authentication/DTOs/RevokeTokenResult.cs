using System;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Token revocation result - isolated DTO
    /// </summary>
    public class RevokeTokenResult
    {
        /// <summary>
        /// Whether revocation was successful
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// Manager ID that was revoked
        /// </summary>
        public string? ManagerId { get; set; }

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
        public static RevokeTokenResult CreateSuccess(string managerId)
        {
            return new RevokeTokenResult
            {
                Success = true,
                ManagerId = managerId
            };
        }

        /// <summary>
        /// Create failed revocation result
        /// </summary>
        public static RevokeTokenResult CreateFailure(string errorMessage)
        {
            return new RevokeTokenResult
            {
                Success = false,
                ErrorMessage = errorMessage
            };
        }
    }
}