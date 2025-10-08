using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Update group request - matches MT5 SDK requirements
    /// </summary>
    public class UpdateGroupRequest
    {
        /// <summary>
        /// Group name (cannot be changed)
        /// </summary>
        [Required]
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
        /// Default leverage
        /// </summary>
        [Range(1, 10000, ErrorMessage = "Default leverage must be between 1 and 10000")]
        public uint? DefaultLeverage { get; set; }

        /// <summary>
        /// Maximum leverage
        /// </summary>
        [Range(1, 10000, ErrorMessage = "Maximum leverage must be between 1 and 10000")]
        public uint? MaximumLeverage { get; set; }

        /// <summary>
        /// Stop-out level (percentage)
        /// </summary>
        [Range(0, 100, ErrorMessage = "Stop-out level must be between 0 and 100")]
        public double? StopOutLevel { get; set; }

        /// <summary>
        /// Margin call level (percentage)
        /// </summary>
        [Range(0, 100, ErrorMessage = "Margin call level must be between 0 and 100")]
        public double? MarginCallLevel { get; set; }

        /// <summary>
        /// Validate request - at least one field should be provided for update
        /// </summary>
        public (bool IsValid, string? ErrorMessage) Validate()
        {
            if (string.IsNullOrWhiteSpace(GroupName))
                return (false, "Group name is required");

            // Check if at least one field is provided for update
            var hasUpdates = Server != null || Company != null ||
                           DefaultLeverage != null || MaximumLeverage != null ||
                           StopOutLevel != null || MarginCallLevel != null;

            if (!hasUpdates)
                return (false, "At least one field must be provided for update");

            // Validate leverage relationship
            if (DefaultLeverage.HasValue && MaximumLeverage.HasValue && DefaultLeverage > MaximumLeverage)
                return (false, "Default leverage cannot be greater than maximum leverage");

            // Validate margin levels
            if (StopOutLevel.HasValue && MarginCallLevel.HasValue && StopOutLevel >= MarginCallLevel)
                return (false, "Stop-out level must be less than margin call level");

            return (true, null);
        }
    }
}