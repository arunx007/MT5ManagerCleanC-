namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Client token validation result - isolated DTO
    /// </summary>
    public class ClientTokenValidationResult
    {
        /// <summary>
        /// Whether the token is valid
        /// </summary>
        public bool IsValid { get; set; }

        /// <summary>
        /// Client information if token is valid
        /// </summary>
        public ClientInfo? Client { get; set; }

        /// <summary>
        /// Error message if token is invalid
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Token expiry timestamp (-1 for unlimited)
        /// </summary>
        public long ExpiresAt { get; set; }

        /// <summary>
        /// Time remaining until expiry (seconds, -1 for unlimited)
        /// </summary>
        public long TimeUntilExpiry { get; set; }

        /// <summary>
        /// Create valid token result
        /// </summary>
        public static ClientTokenValidationResult CreateValid(ClientInfo client, long expiresAt, long timeUntilExpiry)
        {
            return new ClientTokenValidationResult
            {
                IsValid = true,
                Client = client,
                ExpiresAt = expiresAt,
                TimeUntilExpiry = timeUntilExpiry
            };
        }

        /// <summary>
        /// Create invalid token result
        /// </summary>
        public static ClientTokenValidationResult CreateInvalid(string errorMessage)
        {
            return new ClientTokenValidationResult
            {
                IsValid = false,
                ErrorMessage = errorMessage
            };
        }
    }
}