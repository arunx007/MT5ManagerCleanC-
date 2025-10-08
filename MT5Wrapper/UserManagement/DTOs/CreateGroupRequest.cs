using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Create group request - matches MT5 SDK requirements
    /// </summary>
    public class CreateGroupRequest
    {
        /// <summary>
        /// Group name (required)
        /// </summary>
        [Required(ErrorMessage = "Group name is required")]
        [StringLength(50, ErrorMessage = "Group name cannot exceed 50 characters")]
        public string GroupName { get; set; } = string.Empty;

        /// <summary>
        /// Server name
        /// </summary>
        [StringLength(50, ErrorMessage = "Server name cannot exceed 50 characters")]
        public string? Server { get; set; }

        /// <summary>
        /// Company name
        /// </summary>
        [StringLength(100, ErrorMessage = "Company name cannot exceed 100 characters")]
        public string? Company { get; set; }

        /// <summary>
        /// Currency (e.g., USD, EUR, GBP)
        /// </summary>
        [Required(ErrorMessage = "Currency is required")]
        [StringLength(10, ErrorMessage = "Currency cannot exceed 10 characters")]
        public string Currency { get; set; } = string.Empty;

        /// <summary>
        /// Default leverage
        /// </summary>
        [Range(1, 10000, ErrorMessage = "Default leverage must be between 1 and 10000")]
        public uint DefaultLeverage { get; set; } = 100;

        /// <summary>
        /// Maximum leverage
        /// </summary>
        [Range(1, 10000, ErrorMessage = "Maximum leverage must be between 1 and 10000")]
        public uint MaximumLeverage { get; set; } = 500;

        /// <summary>
        /// Stop-out level (percentage)
        /// </summary>
        [Range(0, 100, ErrorMessage = "Stop-out level must be between 0 and 100")]
        public double StopOutLevel { get; set; } = 50;

        /// <summary>
        /// Margin call level (percentage)
        /// </summary>
        [Range(0, 100, ErrorMessage = "Margin call level must be between 0 and 100")]
        public double MarginCallLevel { get; set; } = 100;

        /// <summary>
        /// Validate request
        /// </summary>
        public (bool IsValid, string? ErrorMessage) Validate()
        {
            if (string.IsNullOrWhiteSpace(GroupName))
                return (false, "Group name is required");

            if (string.IsNullOrWhiteSpace(Currency))
                return (false, "Currency is required");

            if (DefaultLeverage > MaximumLeverage)
                return (false, "Default leverage cannot be greater than maximum leverage");

            if (StopOutLevel >= MarginCallLevel)
                return (false, "Stop-out level must be less than margin call level");

            return (true, null);
        }
    }
}