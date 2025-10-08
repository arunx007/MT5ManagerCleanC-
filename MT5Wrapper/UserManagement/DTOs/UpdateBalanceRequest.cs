using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Update user balance request - matches MT5 SDK dealer balance operations
    /// </summary>
    public class UpdateBalanceRequest
    {
        /// <summary>
        /// User login ID
        /// </summary>
        [Required(ErrorMessage = "Login is required")]
        [Range(1, ulong.MaxValue, ErrorMessage = "Login must be a positive number")]
        public ulong Login { get; set; }

        /// <summary>
        /// Balance adjustment amount (positive for deposit, negative for withdrawal)
        /// </summary>
        [Required(ErrorMessage = "Amount is required")]
        public double Amount { get; set; }

        /// <summary>
        /// Comment for the balance operation
        /// </summary>
        [StringLength(200, ErrorMessage = "Comment cannot exceed 200 characters")]
        public string? Comment { get; set; }

        /// <summary>
        /// Deal type (Balance adjustment, Credit, etc.)
        /// </summary>
        public DealType DealType { get; set; } = DealType.Balance;

        /// <summary>
        /// Validate request
        /// </summary>
        public (bool IsValid, string? ErrorMessage) Validate()
        {
            if (Login == 0)
                return (false, "Login must be greater than 0");

            if (Amount == 0)
                return (false, "Amount cannot be zero");

            return (true, null);
        }
    }

    /// <summary>
    /// Deal types for balance operations
    /// </summary>
    public enum DealType
    {
        Balance = 1,
        Credit = 2,
        Bonus = 3,
        Commission = 4,
        Correction = 5
    }
}