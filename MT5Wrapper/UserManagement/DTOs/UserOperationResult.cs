using System;
using System;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// User operation result - matches MT5 SDK return codes
    /// </summary>
    public class UserOperationResult
    {
        /// <summary>
        /// Whether the operation was successful
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// MT5 API return code
        /// </summary>
        public MTRetCode ResultCode { get; set; }

        /// <summary>
        /// Human-readable message
        /// </summary>
        public string Message { get; set; } = string.Empty;

        /// <summary>
        /// User data if operation was successful
        /// </summary>
        public UserDto? User { get; set; }

        /// <summary>
        /// Deal ID if balance operation was performed
        /// </summary>
        public ulong? DealId { get; set; }

        /// <summary>
        /// Timestamp of the operation
        /// </summary>
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Create successful result
        /// </summary>
        public static UserOperationResult CreateSuccess(string message, UserDto? user = null, ulong? dealId = null)
        {
            return new UserOperationResult
            {
                Success = true,
                ResultCode = MTRetCode.MT_RET_OK,
                Message = message,
                User = user,
                DealId = dealId
            };
        }

        /// <summary>
        /// Create failed result
        /// </summary>
        public static UserOperationResult CreateFailure(MTRetCode resultCode, string message)
        {
            return new UserOperationResult
            {
                Success = false,
                ResultCode = resultCode,
                Message = message
            };
        }

        /// <summary>
        /// Get user-friendly error message
        /// </summary>
        public string GetUserFriendlyMessage()
        {
            if (Success)
                return Message;

            return ResultCode switch
            {
                MTRetCode.MT_RET_ERR_PARAMS => "Invalid parameters provided",
                MTRetCode.MT_RET_ERR_DATA => "Invalid user data",
                MTRetCode.MT_RET_ERR_NOTFOUND => "User not found",
                MTRetCode.MT_RET_ERR_DUPLICATE => "User already exists",
                MTRetCode.MT_RET_ERR_PERMISSIONS => "Access denied",
                MTRetCode.MT_RET_ERR_TIMEOUT => "Operation timeout",
                MTRetCode.MT_RET_ERR_CONNECTION => "Connection error",
                _ => $"Operation failed: {Message}"
            };
        }
    }
}