using System.Collections.Generic;

namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// 📜 Historical Data Response - Container for historical price data
    /// </summary>
    public class HistoricalDataResponse
    {
        /// <summary>
        /// Trading symbol
        /// </summary>
        public string Symbol { get; set; } = "";

        /// <summary>
        /// Data type ("ticks" or "bars")
        /// </summary>
        public string DataType { get; set; } = "";

        /// <summary>
        /// Timeframe in minutes (for bars only)
        /// </summary>
        public int? Timeframe { get; set; }

        /// <summary>
        /// Historical tick data (when DataType is "ticks")
        /// </summary>
        public List<TickDataDto>? Ticks { get; set; }

        /// <summary>
        /// Historical bar data (when DataType is "bars")
        /// </summary>
        public List<ChartBar>? Bars { get; set; }

        /// <summary>
        /// Number of records returned
        /// </summary>
        public int Count => (Ticks?.Count ?? 0) + (Bars?.Count ?? 0);

        /// <summary>
        /// Request timestamp (milliseconds since epoch)
        /// </summary>
        public long RequestTime { get; set; }
    }
}