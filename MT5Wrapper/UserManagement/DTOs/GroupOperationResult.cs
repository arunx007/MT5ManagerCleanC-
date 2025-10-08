using System;
using System;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.UserManagement.DTOs
{
    /// <summary>
    /// Group operation result - matches MT5 SDK return codes
    /// </summary>
    public class GroupOperationResult
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
        /// Group data if operation was successful
        /// </summary>
        public GroupDto? Group { get; set; }

        /// <summary>
        /// Timestamp of the operation
        /// </summary>
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Create successful result
        /// </summary>
        public static GroupOperationResult CreateSuccess(string message, GroupDto? group = null)
        {
            return new GroupOperationResult
            {
                Success = true,
                ResultCode = MTRetCode.MT_RET_OK,
                Message = message,
                Group = group
            };
        }

        /// <summary>
        /// Create failed result
        /// </summary>
        public static GroupOperationResult CreateFailure(MTRetCode resultCode, string message)
        {
            return new GroupOperationResult
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
                MTRetCode.MT_RET_ERR_DATA => "Invalid group data",
                MTRetCode.MT_RET_ERR_NOTFOUND => "Group not found",
                MTRetCode.MT_RET_ERR_DUPLICATE => "Group already exists",
                MTRetCode.MT_RET_ERR_PERMISSIONS => "Access denied",
                MTRetCode.MT_RET_ERR_TIMEOUT => "Operation timeout",
                MTRetCode.MT_RET_ERR_CONNECTION => "Connection error",
                _ => $"Operation failed: {Message}"
            };
        }
    }
}