using System;

namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// 💰 Live Tick Data - Real-time bid/ask prices and market information
    /// </summary>
    public class TickDataDto
    {
        /// <summary>
        /// Trading symbol
        /// </summary>
        public string Symbol { get; set; } = "";

        /// <summary>
        /// Bid price (sell price)
        /// </summary>
        public double Bid { get; set; }

        /// <summary>
        /// Ask price (buy price)
        /// </summary>
        public double Ask { get; set; }

        /// <summary>
        /// Last traded price
        /// </summary>
        public double Last { get; set; }

        /// <summary>
        /// Trading volume
        /// </summary>
        public long Volume { get; set; }

        /// <summary>
        /// Timestamp (milliseconds since epoch)
        /// </summary>
        public ulong Timestamp { get; set; }

        /// <summary>
        /// Spread in points (calculated as (ask - bid) * 100000 for 5-digit brokers)
        /// </summary>
        public int Spread { get; set; }

        /// <summary>
        /// Tick flags (bitmask indicating tick properties)
        /// </summary>
        public uint Flags { get; set; }

        /// <summary>
        /// Request timestamp when data was retrieved
        /// </summary>
        public long RequestTime { get; set; } = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
    }
}