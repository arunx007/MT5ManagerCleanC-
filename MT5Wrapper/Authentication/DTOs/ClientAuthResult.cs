namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Client authentication result - isolated DTO
    /// </summary>
    public class ClientAuthResult
    {
        /// <summary>
        /// Whether authentication was successful
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// JWT token (unlimited expiry for React Native)
        /// </summary>
        public string? Token { get; set; }

        /// <summary>
        /// Refresh token for regenerating client tokens
        /// </summary>
        public string? RefreshToken { get; set; }

        /// <summary>
        /// Token expiry timestamp (-1 for unlimited)
        /// </summary>
        public long ExpiresAt { get; set; }

        /// <summary>
        /// Client information
        /// </summary>
        public ClientInfo? Client { get; set; }

        /// <summary>
        /// Error message if authentication failed
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Create successful authentication result
        /// </summary>
        public static ClientAuthResult CreateSuccess(string token, string refreshToken, ClientInfo client)
        {
            return new ClientAuthResult
            {
                Success = true,
                Token = token,
                RefreshToken = refreshToken,
                ExpiresAt = -1, // Unlimited
                Client = client
            };
        }

        /// <summary>
        /// Create failed authentication result
        /// </summary>
        public static ClientAuthResult CreateFailure(string errorMessage)
        {
            return new ClientAuthResult
            {
                Success = false,
                ErrorMessage = errorMessage
            };
        }
    }
}