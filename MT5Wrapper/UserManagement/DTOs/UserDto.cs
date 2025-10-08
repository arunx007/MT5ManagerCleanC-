using System;
using System;
using System.ComponentModel.DataAnnotations;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// User data transfer object - matches MT5 SDK structure
    /// </summary>
    public class UserDto
    {
        /// <summary>
        /// User login ID
        /// </summary>
        [Required]
        public ulong Login { get; set; }

        /// <summary>
        /// User display name
        /// </summary>
        public string? Name { get; set; }

        /// <summary>
        /// User group
        /// </summary>
        public string? Group { get; set; }

        /// <summary>
        /// Email address
        /// </summary>
        public string? Email { get; set; }

        /// <summary>
        /// Account balance
        /// </summary>
        public double Balance { get; set; }

        /// <summary>
        /// Account credit
        /// </summary>
        public double Credit { get; set; }

        /// <summary>
        /// Account equity
        /// </summary>
        public double Equity { get; set; }

        /// <summary>
        /// Used margin
        /// </summary>
        public double Margin { get; set; }

        /// <summary>
        /// Free margin
        /// </summary>
        public double MarginFree { get; set; }

        /// <summary>
        /// Margin level percentage
        /// </summary>
        public double MarginLevel { get; set; }

        /// <summary>
        /// Total profit
        /// </summary>
        public double Profit { get; set; }

        /// <summary>
        /// Country
        /// </summary>
        public string? Country { get; set; }

        /// <summary>
        /// City
        /// </summary>
        public string? City { get; set; }

        /// <summary>
        /// State/Province
        /// </summary>
        public string? State { get; set; }

        /// <summary>
        /// ZIP/Postal code
        /// </summary>
        public string? ZipCode { get; set; }

        /// <summary>
        /// Address
        /// </summary>
        public string? Address { get; set; }

        /// <summary>
        /// Phone number
        /// </summary>
        public string? Phone { get; set; }

        /// <summary>
        /// Comment/Notes
        /// </summary>
        public string? Comment { get; set; }

        /// <summary>
        /// Registration timestamp (Unix timestamp)
        /// </summary>
        public ulong RegistrationTime { get; set; }

        /// <summary>
        /// Last access timestamp (Unix timestamp)
        /// </summary>
        public ulong LastAccessTime { get; set; }

        /// <summary>
        /// Whether user is enabled
        /// </summary>
        public bool IsEnabled { get; set; }

        /// <summary>
        /// Account leverage
        /// </summary>
        public int Leverage { get; set; }

        /// <summary>
        /// Tenant ID (for multi-tenant isolation)
        /// </summary>
        public string TenantId { get; set; } = string.Empty;

        /// <summary>
        /// Create DTO from MT5 CIMTUser and CIMTAccount objects
        /// </summary>
        public static UserDto FromMT5User(CIMTUser user, CIMTAccount? account, string tenantId)
        {
            return new UserDto
            {
                Login = user.Login(),
                Name = user.Name(),
                Group = user.Group(),
                Email = user.EMail(), // Email from MT5 user
                Balance = account?.Balance() ?? 0,
                Credit = account?.Credit() ?? 0,
                Equity = account?.Equity() ?? 0,
                Margin = account?.Margin() ?? 0,
                MarginFree = account?.MarginFree() ?? 0,
                MarginLevel = account?.MarginLevel() ?? 0,
                Profit = account?.Profit() ?? 0,
                Country = user.Country(),
                City = user.City(),
                State = user.State(),
                ZipCode = user.ZIPCode(),
                Address = user.Address(),
                Phone = user.Phone(),
                Comment = user.Comment(),
                RegistrationTime = (ulong)DateTimeOffset.FromUnixTimeSeconds((long)user.Registration()).ToUnixTimeSeconds(),
                LastAccessTime = (ulong)DateTimeOffset.FromUnixTimeSeconds((long)user.LastAccess()).ToUnixTimeSeconds(),
                IsEnabled = (user.Rights() & CIMTUser.EnUsersRights.USER_RIGHT_ENABLED) != 0,
                Leverage = (int)user.Leverage(),
                TenantId = tenantId
            };
        }

        /// <summary>
        /// Convert to MT5 CIMTUser object for SDK operations
        /// </summary>
        public CIMTUser ToMT5User(CIMTManagerAPI manager)
        {
            var user = manager.UserCreate();
            if (user == null) throw new InvalidOperationException("Failed to create MT5 user object");

            // Set basic properties
            user.Login(Login);
            if (!string.IsNullOrEmpty(Name)) user.Name(Name);
            if (!string.IsNullOrEmpty(Group)) user.Group(Group);
            if (!string.IsNullOrEmpty(Country)) user.Country(Country);
            if (!string.IsNullOrEmpty(City)) user.City(City);
            if (!string.IsNullOrEmpty(State)) user.State(State);
            if (!string.IsNullOrEmpty(ZipCode)) user.ZIPCode(ZipCode);
            if (!string.IsNullOrEmpty(Address)) user.Address(Address);
            if (!string.IsNullOrEmpty(Phone)) user.Phone(Phone);
            if (!string.IsNullOrEmpty(Comment)) user.Comment(Comment);
            if (!string.IsNullOrEmpty(Email)) user.EMail(Email);

            // Set leverage
            user.Leverage((uint)Leverage);

            // Set rights (enable user by default)
            var rights = CIMTUser.EnUsersRights.USER_RIGHT_ENABLED;
            user.Rights(rights);

            return user;
        }
    }
}