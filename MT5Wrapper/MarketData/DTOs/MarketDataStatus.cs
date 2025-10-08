using System;

namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// 📊 Market Data Status - Connection and streaming information
    /// </summary>
    public class MarketDataStatus
    {
        /// <summary>
        /// Whether MT5 connection is active
        /// </summary>
        public bool IsConnected { get; set; }

        /// <summary>
        /// Number of active symbol subscriptions
        /// </summary>
        public int ActiveSubscriptions { get; set; }

        /// <summary>
        /// Total symbols available
        /// </summary>
        public int TotalSymbols { get; set; }

        /// <summary>
        /// Last successful data update timestamp
        /// </summary>
        public long LastUpdate { get; set; }

        /// <summary>
        /// WebSocket endpoint URL (if available)
        /// </summary>
        public string? WebSocketEndpoint { get; set; }

        /// <summary>
        /// Connection uptime in seconds
        /// </summary>
        public long UptimeSeconds { get; set; }

        /// <summary>
        /// Request timestamp
        /// </summary>
        public long RequestTime { get; set; } = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
    }
}