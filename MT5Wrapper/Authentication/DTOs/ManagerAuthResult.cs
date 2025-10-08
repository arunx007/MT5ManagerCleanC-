using System;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Manager authentication result - isolated DTO
    /// </summary>
    public class ManagerAuthResult
    {
        /// <summary>
        /// Whether authentication was successful
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// JWT token (30-day expiry)
        /// </summary>
        public string? Token { get; set; }

        /// <summary>
        /// Token expiry timestamp (Unix timestamp)
        /// </summary>
        public long ExpiresAt { get; set; }

        /// <summary>
        /// Manager information
        /// </summary>
        public ManagerInfo? Manager { get; set; }

        /// <summary>
        /// Error message if authentication failed
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Create successful authentication result
        /// </summary>
        public static ManagerAuthResult CreateSuccess(string token, ManagerInfo manager, int expiryDays)
        {
            var expiryTimestamp = DateTimeOffset.UtcNow.AddDays(expiryDays).ToUnixTimeSeconds();

            return new ManagerAuthResult
            {
                Success = true,
                Token = token,
                ExpiresAt = expiryTimestamp,
                Manager = manager
            };
        }

        /// <summary>
        /// Create failed authentication result
        /// </summary>
        public static ManagerAuthResult CreateFailure(string errorMessage)
        {
            return new ManagerAuthResult
            {
                Success = false,
                ErrorMessage = errorMessage
            };
        }
    }
}