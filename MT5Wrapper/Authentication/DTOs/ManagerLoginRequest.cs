using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.Authentication.DTOs
{
    /// <summary>
    /// Manager login request - isolated DTO
    /// </summary>
    public class ManagerLoginRequest
    {
        /// <summary>
        /// Manager login (numeric string)
        /// </summary>
        [Required(ErrorMessage = "Login is required")]
        [RegularExpression(@"^\d+$", ErrorMessage = "Login must be numeric")]
        public string Login { get; set; } = string.Empty;

        /// <summary>
        /// Manager password
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