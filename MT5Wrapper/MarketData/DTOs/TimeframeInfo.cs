namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// ⏰ Timeframe Information - Supported chart timeframes
    /// </summary>
    public class TimeframeInfo
    {
        /// <summary>
        /// Timeframe value in minutes
        /// </summary>
        public int Value { get; set; }

        /// <summary>
        /// Timeframe name (e.g., "M1", "H1", "D1")
        /// </summary>
        public string Name { get; set; } = "";

        /// <summary>
        /// Human-readable description
        /// </summary>
        public string Description { get; set; } = "";
    }
}