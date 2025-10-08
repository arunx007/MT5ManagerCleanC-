using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// User search criteria - matches MT5 SDK search capabilities
    /// </summary>
    public class UserSearchCriteria
    {
        /// <summary>
        /// Search by login ID (exact match)
        /// </summary>
        public ulong? Login { get; set; }

        /// <summary>
        /// Search by name (partial match)
        /// </summary>
        [StringLength(100, ErrorMessage = "Name cannot exceed 100 characters")]
        public string? Name { get; set; }

        /// <summary>
        /// Search by group (exact match)
        /// </summary>
        public string? Group { get; set; }

        /// <summary>
        /// Search by email (partial match)
        /// </summary>
        [EmailAddress(ErrorMessage = "Invalid email format")]
        public string? Email { get; set; }

        /// <summary>
        /// Search by country (exact match)
        /// </summary>
        [StringLength(50, ErrorMessage = "Country cannot exceed 50 characters")]
        public string? Country { get; set; }

        /// <summary>
        /// Search by city (partial match)
        /// </summary>
        [StringLength(50, ErrorMessage = "City cannot exceed 50 characters")]
        public string? City { get; set; }

        /// <summary>
        /// Filter by enabled/disabled users only
        /// </summary>
        public bool? IsEnabled { get; set; }

        /// <summary>
        /// Filter by minimum balance
        /// </summary>
        [Range(0, double.MaxValue, ErrorMessage = "Minimum balance cannot be negative")]
        public double? MinimumBalance { get; set; }

        /// <summary>
        /// Filter by maximum balance
        /// </summary>
        [Range(0, double.MaxValue, ErrorMessage = "Maximum balance cannot be negative")]
        public double? MaximumBalance { get; set; }

        /// <summary>
        /// Registration date from (Unix timestamp)
        /// </summary>
        public ulong? RegistrationFrom { get; set; }

        /// <summary>
        /// Registration date to (Unix timestamp)
        /// </summary>
        public ulong? RegistrationTo { get; set; }

        /// <summary>
        /// Last access date from (Unix timestamp)
        /// </summary>
        public ulong? LastAccessFrom { get; set; }

        /// <summary>
        /// Last access date to (Unix timestamp)
        /// </summary>
        public ulong? LastAccessTo { get; set; }

        /// <summary>
        /// Maximum number of results to return
        /// </summary>
        [Range(1, 10000, ErrorMessage = "Limit must be between 1 and 10000")]
        public int Limit { get; set; } = 1000;

        /// <summary>
        /// Offset for pagination
        /// </summary>
        [Range(0, int.MaxValue, ErrorMessage = "Offset cannot be negative")]
        public int Offset { get; set; } = 0;

        /// <summary>
        /// Validate search criteria
        /// </summary>
        public (bool IsValid, string? ErrorMessage) Validate()
        {
            if (Limit < 1 || Limit > 10000)
                return (false, "Limit must be between 1 and 10000");

            if (Offset < 0)
                return (false, "Offset cannot be negative");

            if (MinimumBalance.HasValue && MaximumBalance.HasValue && MinimumBalance > MaximumBalance)
                return (false, "Minimum balance cannot be greater than maximum balance");

            return (true, null);
        }

        /// <summary>
        /// Check if any search criteria is specified
        /// </summary>
        public bool HasCriteria()
        {
            return Login.HasValue || !string.IsNullOrEmpty(Name) || !string.IsNullOrEmpty(Group) ||
                   !string.IsNullOrEmpty(Email) || !string.IsNullOrEmpty(Country) || !string.IsNullOrEmpty(City) ||
                   IsEnabled.HasValue || MinimumBalance.HasValue || MaximumBalance.HasValue ||
                   RegistrationFrom.HasValue || RegistrationTo.HasValue || LastAccessFrom.HasValue || LastAccessTo.HasValue;
        }
    }
}