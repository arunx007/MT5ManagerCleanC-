using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.MultiTenant.Models
{
    /// <summary>
    /// Request model for creating a new tenant
    /// </summary>
    public class CreateTenantRequest
    {
        /// <summary>
        /// Manager/Company name
        /// </summary>
        [Required(ErrorMessage = "Manager name is required")]
        [StringLength(100, ErrorMessage = "Manager name cannot exceed 100 characters")]
        public string ManagerName { get; set; } = string.Empty;

        /// <summary>
        /// Company name (optional)
        /// </summary>
        [StringLength(200, ErrorMessage = "Company name cannot exceed 200 characters")]
        public string? CompanyName { get; set; }

        /// <summary>
        /// Contact email address
        /// </summary>
        [Required(ErrorMessage = "Email is required")]
        [EmailAddress(ErrorMessage = "Invalid email format")]
        public string Email { get; set; } = string.Empty;

        /// <summary>
        /// Phone number (optional)
        /// </summary>
        [Phone(ErrorMessage = "Invalid phone number format")]
        public string? Phone { get; set; }

        /// <summary>
        /// Subscription plan
        /// </summary>
        [Required(ErrorMessage = "Subscription plan is required")]
        public SubscriptionInfo Subscription { get; set; } = new();

        /// <summary>
        /// Initial MT5 Manager configuration
        /// </summary>
        public MT5ManagerConfig? InitialMT5Manager { get; set; }

        /// <summary>
        /// Custom domain preference (optional)
        /// </summary>
        public string? PreferredDomain { get; set; }

        /// <summary>
        /// Industry type
        /// </summary>
        public string? Industry { get; set; }

        /// <summary>
        /// Expected usage description
        /// </summary>
        public string? UsageDescription { get; set; }
    }
}