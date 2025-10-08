using System;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Token refresh result - isolated DTO
    /// </summary>
    public class TokenRefreshResult
    {
        /// <summary>
        /// Whether refresh was successful
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// New JWT token
        /// </summary>
        public string? NewToken { get; set; }

        /// <summary>
        /// New token expiry timestamp
        /// </summary>
        public long NewExpiresAt { get; set; }

        /// <summary>
        /// Error message if refresh failed
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Create successful refresh result
        /// </summary>
        public static TokenRefreshResult CreateSuccess(string newToken, int expiryDays)
        {
            var expiryTimestamp = DateTimeOffset.UtcNow.AddDays(expiryDays).ToUnixTimeSeconds();

            return new TokenRefreshResult
            {
                Success = true,
                NewToken = newToken,
                NewExpiresAt = expiryTimestamp
            };
        }

        /// <summary>
        /// Create failed refresh result
        /// </summary>
        public static TokenRefreshResult CreateFailure(string errorMessage)
        {
            return new TokenRefreshResult
            {
                Success = false,
                ErrorMessage = errorMessage
            };
        }
    }
}