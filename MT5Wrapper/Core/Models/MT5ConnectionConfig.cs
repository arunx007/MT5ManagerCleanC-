using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.Core.Models
{
    /// <summary>
    /// Configuration for MT5 Manager connection - isolated model
    /// </summary>
    public class MT5ConnectionConfig
    {
        /// <summary>
        /// MT5 Manager server address (e.g., "185.82.73.132:443")
        /// </summary>
        [Required]
        public string Server { get; set; } = string.Empty;

        /// <summary>
        /// Manager login (numeric string)
        /// </summary>
        [Required]
        public string Login { get; set; } = string.Empty;

        /// <summary>
        /// Manager password
        /// </summary>
        [Required]
        public string Password { get; set; } = string.Empty;

        /// <summary>
        /// Connection timeout in milliseconds
        /// </summary>
        public int ConnectionTimeout { get; set; } = 10000;

        /// <summary>
        /// Manager ID for multi-tenant isolation
        /// </summary>
        [Required]
        public string ManagerId { get; set; } = string.Empty;

        /// <summary>
        /// Optional description for this connection
        /// </summary>
        public string? Description { get; set; }

        /// <summary>
        /// Whether to enable automatic reconnection
        /// </summary>
        public bool EnableAutoReconnect { get; set; } = true;

        /// <summary>
        /// Auto-reconnect interval in seconds
        /// </summary>
        public int AutoReconnectInterval { get; set; } = 30;
    }
}