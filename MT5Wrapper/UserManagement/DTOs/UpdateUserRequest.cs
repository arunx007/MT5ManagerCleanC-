using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Update user request - matches MT5 SDK requirements
    /// </summary>
    public class UpdateUserRequest
    {
        /// <summary>
        /// User login ID (cannot be changed)
        /// </summary>
        [Required]
        public ulong Login { get; set; }

        /// <summary>
        /// User display name
        /// </summary>
        [StringLength(100, ErrorMessage = "Name cannot exceed 100 characters")]
        public string? Name { get; set; }

        /// <summary>
        /// User group
        /// </summary>
        public string? Group { get; set; }

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
        public int? Leverage { get; set; }

        /// <summary>
        /// Whether to enable/disable the user
        /// </summary>
        public bool? EnableUser { get; set; }

        /// <summary>
        /// Validate request - at least one field should be provided for update
        /// </summary>
        public (bool IsValid, string? ErrorMessage) Validate()
        {
            if (Login == 0)
                return (false, "Login is required");

            // Check if at least one field is provided for update
            var hasUpdates = Name != null || Group != null || Email != null ||
                           Phone != null || Country != null || City != null ||
                           State != null || ZipCode != null || Address != null ||
                           Comment != null || Leverage != null || EnableUser != null;

            if (!hasUpdates)
                return (false, "At least one field must be provided for update");

            return (true, null);
        }
    }
}