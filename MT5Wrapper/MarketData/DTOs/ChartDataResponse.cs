using System.Collections.Generic;

namespace MT5Wrapper.MarketData.DTOs
{
    /// <summary>
    /// 📊 Chart Data Response - OHLC/candlestick data with metadata
    /// </summary>
    public class ChartDataResponse
    {
        /// <summary>
        /// Trading symbol
        /// </summary>
        public string Symbol { get; set; } = "";

        /// <summary>
        /// Timeframe in minutes
        /// </summary>
        public int Timeframe { get; set; }

        /// <summary>
        /// List of OHLC bars
        /// </summary>
        public List<ChartBar> Bars { get; set; } = new List<ChartBar>();

        /// <summary>
        /// Number of bars returned
        /// </summary>
        public int Count => Bars.Count;

        /// <summary>
        /// Request timestamp (milliseconds since epoch)
        /// </summary>
        public long RequestTime { get; set; }
    }

    /// <summary>
    /// 📈 Individual OHLC bar data
    /// </summary>
    public class ChartBar
    {
        /// <summary>
        /// Trading symbol
        /// </summary>
        public string Symbol { get; set; } = "";

        /// <summary>
        /// Bar timestamp (milliseconds since epoch)
        /// </summary>
        public long Timestamp { get; set; }

        /// <summary>
        /// Open price
        /// </summary>
        public double Open { get; set; }

        /// <summary>
        /// High price
        /// </summary>
        public double High { get; set; }

        /// <summary>
        /// Low price
        /// </summary>
        public double Low { get; set; }

        /// <summary>
        /// Close price
        /// </summary>
        public double Close { get; set; }

        /// <summary>
        /// Tick volume (number of ticks)
        /// </summary>
        public long Volume { get; set; }

        /// <summary>
        /// Spread in points
        /// </summary>
        public int Spread { get; set; }

        /// <summary>
        /// Real volume (lot size)
        /// </summary>
        public ulong RealVolume { get; set; }
    }
}