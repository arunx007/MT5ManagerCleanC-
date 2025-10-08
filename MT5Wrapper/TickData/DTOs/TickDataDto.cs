using System;
using System.ComponentModel.DataAnnotations;
using MetaQuotes.MT5CommonAPI;

namespace MT5Wrapper.TickData.DTOs
{
    /// <summary>
    /// Tick data transfer object - represents real-time market data
    /// </summary>
    public class TickDataDto
    {
        /// <summary>
        /// Trading symbol (e.g., EURUSD, GBPJPY)
        /// </summary>
        [Required]
        public string Symbol { get; set; } = string.Empty;

        /// <summary>
        /// Bid price
        /// </summary>
        public double Bid { get; set; }

        /// <summary>
        /// Ask price
        /// </summary>
        public double Ask { get; set; }

        /// <summary>
        /// Last trade price
        /// </summary>
        public double Last { get; set; }

        /// <summary>
        /// Volume of the tick
        /// </summary>
        public long Volume { get; set; }

        /// <summary>
        /// Timestamp in milliseconds since Unix epoch
        /// </summary>
        public long Timestamp { get; set; }

        /// <summary>
        /// Spread in points
        /// </summary>
        public int Spread { get; set; }

        /// <summary>
        /// Tick flags (internal MT5 flags)
        /// </summary>
        public uint Flags { get; set; }

        /// <summary>
        /// Change from previous tick
        /// </summary>
        public double Change { get; set; }

        /// <summary>
        /// Percentage change from previous tick
        /// </summary>
        public double ChangePercent { get; set; }

        /// <summary>
        /// High price of the day
        /// </summary>
        public double DayHigh { get; set; }

        /// <summary>
        /// Low price of the day
        /// </summary>
        public double DayLow { get; set; }

        /// <summary>
        /// Daily volume
        /// </summary>
        public long DayVolume { get; set; }

        /// <summary>
        /// Tenant ID (for multi-tenant isolation)
        /// </summary>
        public string TenantId { get; set; } = string.Empty;

        /// <summary>
        /// Data source (MT5 Manager ID)
        /// </summary>
        public string Source { get; set; } = string.Empty;

        /// <summary>
        /// Create DTO from MT5 tick data
        /// </summary>
        public static TickDataDto FromMT5Tick(string symbol, MTTickShort tick, string tenantId, string source)
        {
            return new TickDataDto
            {
                Symbol = symbol,
                Bid = tick.bid,
                Ask = tick.ask,
                Last = tick.last,
                Volume = (long)tick.volume,
                Timestamp = (long)tick.datetime_msc,
                Flags = (uint)tick.flags,
                Spread = (int)((tick.ask - tick.bid) * 100000), // Convert to points
                TenantId = tenantId,
                Source = source
            };
        }

        /// <summary>
        /// Convert timestamp to DateTime
        /// </summary>
        public DateTime GetTimestampAsDateTime()
        {
            return DateTimeOffset.FromUnixTimeMilliseconds(Timestamp).UtcDateTime;
        }

        /// <summary>
        /// Get mid price (average of bid and ask)
        /// </summary>
        public double GetMidPrice()
        {
            return (Bid + Ask) / 2;
        }

        /// <summary>
        /// Check if tick data is valid
        /// </summary>
        public bool IsValid()
        {
            return !string.IsNullOrEmpty(Symbol) &&
                   Bid > 0 && Ask > 0 && Last > 0 &&
                   Timestamp > 0;
        }
    }
}