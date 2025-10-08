using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Create user request - matches MT5 SDK requirements
    /// </summary>
    public class CreateUserRequest
    {
        /// <summary>
        /// User login ID (must be unique)
        /// </summary>
        [Required(ErrorMessage = "Login is required")]
        [Range(1, ulong.MaxValue, ErrorMessage = "Login must be a positive number")]
        public ulong Login { get; set; }

        /// <summary>
        /// User display name
        /// </summary>
        [Required(ErrorMessage = "Name is required")]
        [StringLength(100, ErrorMessage = "Name cannot exceed 100 characters")]
        public string Name { get; set; } = string.Empty;

        /// <summary>
        /// User group
        /// </summary>
        [Required(ErrorMessage = "Group is required")]
        public string Group { get; set; } = string.Empty;

        /// <summary>
        /// Email address
        /// </summary>
        [EmailAddress(ErrorMessage = "Invalid email format")]
        public string? Email { get; set; }

        /// <summary>
        /// Phone number
        /// </summary>
        [Phone(ErrorMessage = "Invalid phone number format")]
        public string? Phone { get; set; }

        /// <summary>
        /// Country
        /// </summary>
        [StringLength(50, ErrorMessage = "Country cannot exceed 50 characters")]
        public string? Country { get; set; }

        /// <summary>
        /// City
        /// </summary>
        [StringLength(50, ErrorMessage = "City cannot exceed 50 characters")]
        public string? City { get; set; }

        /// <summary>
        /// State/Province
        /// </summary>
        [StringLength(50, ErrorMessage = "State cannot exceed 50 characters")]
        public string? State { get; set; }

        /// <summary>
        /// ZIP/Postal code
        /// </summary>
        [StringLength(20, ErrorMessage = "ZIP code cannot exceed 20 characters")]
        public string? ZipCode { get; set; }

        /// <summary>
        /// Address
        /// </summary>
        [StringLength(200, ErrorMessage = "Address cannot exceed 200 characters")]
        public string? Address { get; set; }

        /// <summary>
        /// Comment/Notes
        /// </summary>
        [StringLength(500, ErrorMessage = "Comment cannot exceed 500 characters")]
        public string? Comment { get; set; }

        /// <summary>
        /// Account leverage
        /// </summary>
        [Range(1, 10000, ErrorMessage = "Leverage must be between 1 and 10000")]
        public int Leverage { get; set; } = 100;

        /// <summary>
        /// Initial balance
        /// </summary>
        [Range(0, double.MaxValue, ErrorMessage = "Balance cannot be negative")]
        public double InitialBalance { get; set; } = 0;

        /// <summary>
        /// Password (optional - MT5 may generate if not provided)
        /// </summary>
        [StringLength(50, ErrorMessage = "Password cannot exceed 50 characters")]
        public string? Password { get; set; }

        /// <summary>
        /// Whether to enable the user immediately
        /// </summary>
        public bool EnableUser { get; set; } = true;

        /// <summary>
        /// Validate request before sending to MT5
        /// </summary>
        public (bool IsValid, string? ErrorMessage) Validate()
        {
            if (Login == 0)
                return (false, "Login must be greater than 0");

            if (string.IsNullOrWhiteSpace(Name))
                return (false, "Name is required");

            if (string.IsNullOrWhiteSpace(Group))
                return (false, "Group is required");

            if (Leverage < 1 || Leverage > 10000)
                return (false, "Leverage must be between 1 and 10000");

            return (true, null);
        }
    }
}