using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// 📊 Chart Data Request - Parameters for OHLC/candlestick data retrieval
    /// </summary>
    public class ChartDataRequest
    {
        /// <summary>
        /// Trading symbol (e.g., EURUSD, GBPUSD)
        /// </summary>
        [Required]
        [StringLength(32, MinimumLength = 1)]
        public string Symbol { get; set; } = "";

        /// <summary>
        /// Timeframe in minutes (1, 5, 15, 30, 60, 240, 1440, 10080, 43200)
        /// </summary>
        [Required]
        [Range(1, 43200)]
        public int Timeframe { get; set; } = 60;

        /// <summary>
        /// Start time (Unix timestamp in seconds)
        /// </summary>
        [Required]
        public long StartTime { get; set; }

        /// <summary>
        /// End time (Unix timestamp in seconds)
        /// </summary>
        [Required]
        public long EndTime { get; set; }

        /// <summary>
        /// Maximum number of bars to return (default: 1000, max: 10000)
        /// </summary>
        [Range(1, 10000)]
        public int Limit { get; set; } = 1000;
    }
}