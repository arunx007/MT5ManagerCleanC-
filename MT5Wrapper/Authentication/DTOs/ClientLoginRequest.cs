using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Client login request - simplified for direct MT5 client authentication
    /// Follows same pattern as ManagerLoginRequest for consistency
    /// </summary>
    public class ClientLoginRequest
    {
        /// <summary>
        /// Client login (numeric string) - trading account number
        /// </summary>
        [Required(ErrorMessage = "Login is required")]
        [RegularExpression(@"^\d+$", ErrorMessage = "Login must be numeric")]
        public string Login { get; set; } = string.Empty;

        /// <summary>
        /// Client password
        /// </summary>
        [Required(ErrorMessage = "Password is required")]
        [MinLength(1, ErrorMessage = "Password cannot be empty")]
        public string Password { get; set; } = string.Empty;

        /// <summary>
        /// MT5 server address (e.g., "185.82.73.132:443")
        /// </summary>
        [Required(ErrorMessage = "Server address is required")]
        public string Server { get; set; } = string.Empty;

        /// <summary>
        /// Manager ID for multi-tenant isolation
        /// </summary>
        [Required(ErrorMessage = "Manager ID is required")]
        public string ManagerId { get; set; } = string.Empty;

        /// <summary>
        /// Optional description
        /// </summary>
        public string? Description { get; set; }
    }
}