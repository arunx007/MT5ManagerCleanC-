using System;

namespace MT5Wrapper.Core.Models
{
    /// <summary>
    /// Connection statistics - isolated model
    /// </summary>
    public class ConnectionStats
    {
        /// <summary>
        /// Total number of connection attempts
        /// </summary>
        public int TotalConnectionAttempts { get; set; }

        /// <summary>
        /// Number of successful connections
        /// </summary>
        public int SuccessfulConnections { get; set; }

        /// <summary>
        /// Number of failed connections
        /// </summary>
        public int FailedConnections { get; set; }

        /// <summary>
        /// Current connection state
        /// </summary>
        public string ConnectionState { get; set; } = "Disconnected";

        /// <summary>
        /// Connection uptime in seconds (if connected)
        /// </summary>
        public long? UptimeSeconds { get; set; }

        /// <summary>
        /// Last connection attempt timestamp
        /// </summary>
        public DateTime? LastConnectionAttempt { get; set; }

        /// <summary>
        /// Last successful connection timestamp
        /// </summary>
        public DateTime? LastSuccessfulConnection { get; set; }

        /// <summary>
        /// Average connection time in milliseconds
        /// </summary>
        public double AverageConnectionTimeMs { get; set; }

        /// <summary>
        /// Connection success rate percentage
        /// </summary>
        public double SuccessRate => TotalConnectionAttempts > 0
            ? (double)SuccessfulConnections / TotalConnectionAttempts * 100
            : 0;

        /// <summary>
        /// Whether currently connected
        /// </summary>
        public bool IsConnected => ConnectionState == "Connected";

        /// <summary>
        /// Reset statistics
        /// </summary>
        public void Reset()
        {
            TotalConnectionAttempts = 0;
            SuccessfulConnections = 0;
            FailedConnections = 0;
            ConnectionState = "Disconnected";
            UptimeSeconds = null;
            LastConnectionAttempt = null;
            LastSuccessfulConnection = null;
            AverageConnectionTimeMs = 0;
        }
    }
}