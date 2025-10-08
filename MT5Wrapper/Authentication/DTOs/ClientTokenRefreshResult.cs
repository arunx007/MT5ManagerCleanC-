namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Client token refresh result - isolated DTO
    /// </summary>
    public class ClientTokenRefreshResult
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
        /// New refresh token
        /// </summary>
        public string? NewRefreshToken { get; set; }

        /// <summary>
        /// New token expiry timestamp (-1 for unlimited)
        /// </summary>
        public long NewExpiresAt { get; set; }

        /// <summary>
        /// Error message if refresh failed
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Create successful refresh result
        /// </summary>
        public static ClientTokenRefreshResult CreateSuccess(string newToken, string newRefreshToken)
        {
            return new ClientTokenRefreshResult
            {
                Success = true,
                NewToken = newToken,
                NewRefreshToken = newRefreshToken,
                NewExpiresAt = -1 // Unlimited
            };
        }

        /// <summary>
        /// Create failed refresh result
        /// </summary>
        public static ClientTokenRefreshResult CreateFailure(string errorMessage)
        {
            return new ClientTokenRefreshResult
            {
                Success = false,
                ErrorMessage = errorMessage
            };
        }
    }
}