using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Group data transfer object - matches MT5 SDK CIMTConGroup structure
    /// </summary>
    public class GroupDto
    {
        /// <summary>
        /// Group name
        /// </summary>
        public string? GroupName { get; set; }

        /// <summary>
        /// Server name
        /// </summary>
        public string? Server { get; set; }

        /// <summary>
        /// Group path
        /// </summary>
        public string? GroupPath { get; set; }

        /// <summary>
        /// Copyright information
        /// </summary>
        public string? Copyright { get; set; }

        /// <summary>
        /// Website URL
        /// </summary>
        public string? Website { get; set; }

        /// <summary>
        /// Email address
        /// </summary>
        public string? Email { get; set; }

        /// <summary>
        /// Company name
        /// </summary>
        public string? Company { get; set; }

        /// <summary>
        /// Currency name
        /// </summary>
        public string? Currency { get; set; }

        /// <summary>
        /// Currency digits
        /// </summary>
        public uint CurrencyDigits { get; set; }

        /// <summary>
        /// Default leverage
        /// </summary>
        public uint LeverageDefault { get; set; }

        /// <summary>
        /// Maximum leverage
        /// </summary>
        public uint LeverageMax { get; set; }

        /// <summary>
        /// Stop-out level
        /// </summary>
        public double StopOutLevel { get; set; }

        /// <summary>
        /// Margin call level
        /// </summary>
        public double MarginCallLevel { get; set; }

        /// <summary>
        /// Tenant ID (for multi-tenant isolation)
        /// </summary>
        public string TenantId { get; set; } = string.Empty;

        /// <summary>
        /// Create DTO from MT5 CIMTConGroup object
        /// </summary>
        public static GroupDto FromMT5Group(CIMTConGroup group, string tenantId)
        {
            return new GroupDto
            {
                GroupName = group.Group(),
                Server = group.Server().ToString(),
                GroupPath = "", // Not available in IMTConGroup
                Copyright = "", // Not available in IMTConGroup
                Website = group.CompanyPage(),
                Email = group.CompanyEmail(),
                Company = group.Company(),
                Currency = group.Currency(),
                CurrencyDigits = group.CurrencyDigits(),
                LeverageDefault = 100, // Default value, not available in IMTConGroup
                LeverageMax = 1000, // Default value, not available in IMTConGroup
                StopOutLevel = 30.0, // Default value, not available in IMTConGroup
                MarginCallLevel = 100.0, // Default value, not available in IMTConGroup
                TenantId = tenantId
            };
        }

        /// <summary>
        /// Check if group is valid
        /// </summary>
        public bool IsValid()
        {
            return !string.IsNullOrEmpty(GroupName) && !string.IsNullOrEmpty(Currency);
        }
    }
}