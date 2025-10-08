namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Token validation result - isolated DTO
    /// </summary>
    public class TokenValidationResult
    {
        /// <summary>
        /// Whether the token is valid
        /// </summary>
        public bool IsValid { get; set; }

        /// <summary>
        /// Manager information if token is valid
        /// </summary>
        public ManagerInfo? Manager { get; set; }

        /// <summary>
        /// Error message if token is invalid
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Token expiry timestamp
        /// </summary>
        public long ExpiresAt { get; set; }

        /// <summary>
        /// Time remaining until expiry (seconds)
        /// </summary>
        public long TimeUntilExpiry { get; set; }

        /// <summary>
        /// Create valid token result
        /// </summary>
        public static TokenValidationResult CreateValid(ManagerInfo manager, long expiresAt, long timeUntilExpiry)
        {
            return new TokenValidationResult
            {
                IsValid = true,
                Manager = manager,
                ExpiresAt = expiresAt,
                TimeUntilExpiry = timeUntilExpiry
            };
        }

        /// <summary>
        /// Create invalid token result
        /// </summary>
        public static TokenValidationResult CreateInvalid(string errorMessage)
        {
            return new TokenValidationResult
            {
                IsValid = false,
                ErrorMessage = errorMessage
            };
        }
    }
}