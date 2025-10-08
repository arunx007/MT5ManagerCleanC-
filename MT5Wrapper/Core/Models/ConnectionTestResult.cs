using System;

namespace MT5Wrapper.Core.Models
{
    /// <summary>
    /// Result of connection test operations - isolated model
    /// </summary>
    public class ConnectionTestResult
    {
        /// <summary>
        /// Whether the connection test was successful
        /// </summary>
        public bool IsSuccessful { get; set; }

        /// <summary>
        /// Response time in milliseconds
        /// </summary>
        public long ResponseTimeMs { get; set; }

        /// <summary>
        /// Server information if available
        /// </summary>
        public string? ServerInfo { get; set; }

        /// <summary>
        /// Error message if test failed
        /// </summary>
        public string? ErrorMessage { get; set; }

        /// <summary>
        /// Timestamp of the test
        /// </summary>
        public DateTime TestTimestamp { get; set; } = DateTime.UtcNow;

        /// <summary>
        /// Create a successful test result
        /// </summary>
        public static ConnectionTestResult CreateSuccess(long responseTimeMs, string? serverInfo = null)
        {
            return new ConnectionTestResult
            {
                IsSuccessful = true,
                ResponseTimeMs = responseTimeMs,
                ServerInfo = serverInfo
            };
        }

        /// <summary>
        /// Create a failed test result
        /// </summary>
        public static ConnectionTestResult CreateFailure(string errorMessage, long responseTimeMs = 0)
        {
            return new ConnectionTestResult
            {
                IsSuccessful = false,
                ResponseTimeMs = responseTimeMs,
                ErrorMessage = errorMessage
            };
        }
    }
}