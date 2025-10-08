using System.ComponentModel.DataAnnotations;

namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// 📜 Historical Data Request - Parameters for retrieving historical price data
    /// </summary>
    public class HistoricalDataRequest
    {
        /// <summary>
        /// Trading symbol (e.g., EURUSD, GBPUSD)
        /// </summary>
        [Required]
        [StringLength(32, MinimumLength = 1)]
        public string Symbol { get; set; } = "";

        /// <summary>
        /// Data type: "ticks" or "bars"
        /// </summary>
        [Required]
        public string DataType { get; set; } = "bars";

        /// <summary>
        /// Timeframe in minutes (required for bars, ignored for ticks)
        /// Supported: 1, 5, 15, 30, 60, 240, 1440, 10080, 43200
        /// </summary>
        public int? Timeframe { get; set; }

        /// <summary>
        /// Start date/time (Unix timestamp in seconds)
        /// </summary>
        [Required]
        public long StartDate { get; set; }

        /// <summary>
        /// End date/time (Unix timestamp in seconds)
        /// </summary>
        [Required]
        public long EndDate { get; set; }

        /// <summary>
        /// Maximum number of records to return (default: 1000, max: 10000)
        /// </summary>
        [Range(1, 10000)]
        public int Limit { get; set; } = 1000;
    }
}