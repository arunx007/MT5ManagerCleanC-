using MetaQuotes.MT5CommonAPI;

using System;

namespace MT5Wrapper.Core.Models
{
    /// <summary>
    /// Result of connection operations - isolated model
    /// </summary>
    public class ConnectionResult
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
        /// Connection duration in milliseconds
        /// </summary>
        public long ConnectionTimeMs { get; set; }

        /// <summary>
        /// Timestamp when the operation completed
        /// </summary>
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Create a successful result
        /// </summary>
        public static ConnectionResult CreateSuccess(string message = "Operation completed successfully", long connectionTimeMs = 0)
        {
            return new ConnectionResult
            {
                Success = true,
                ResultCode = MTRetCode.MT_RET_OK,
                Message = message,
                ConnectionTimeMs = connectionTimeMs
            };
        }

        /// <summary>
        /// Create a failed result
        /// </summary>
        public static ConnectionResult CreateFailure(MTRetCode resultCode, string message, long connectionTimeMs = 0)
        {
            return new ConnectionResult
            {
                Success = false,
                ResultCode = resultCode,
                Message = message,
                ConnectionTimeMs = connectionTimeMs
            };
        }
    }
}