using System.Threading.Tasks;
using System.Collections.Generic;
using System.Threading.Tasks;
using MT5Wrapper.MarketData.DTOs;

namespace MT5Wrapper.MarketData.Interfaces
{
    /// <summary>
    /// 📊 MARKET DATA SERVICE - Charts, Quotes, and Historical Price Data
    /// Provides OHLC/candlestick data, real-time quotes, and historical price information
    /// </summary>
    public interface IMarketDataService
    {
        /// <summary>
        /// 📈 Get OHLC chart data for technical analysis
        /// </summary>
        /// <param name="request">Chart data request with symbol, timeframe, and date range</param>
        /// <returns>OHLC chart data response</returns>
        Task<ChartDataResponse?> GetChartDataAsync(ChartDataRequest request);

        /// <summary>
        /// 💰 Get current live tick data (bid/ask prices)
        /// </summary>
        /// <param name="symbol">Trading symbol (e.g., EURUSD)</param>
        /// <param name="tenantId">Tenant ID for multi-manager support</param>
        /// <returns>Current tick data</returns>
        Task<TickDataDto?> GetCurrentTickAsync(string symbol, string tenantId = null);

        /// <summary>
        /// 📜 Get historical price data (ticks or bars)
        /// </summary>
        /// <param name="request">Historical data request parameters</param>
        /// <returns>Historical price data</returns>
        Task<HistoricalDataResponse?> GetHistoricalDataAsync(HistoricalDataRequest request);

        /// <summary>
        /// 📋 Get list of available symbols for trading
        /// </summary>
        /// <returns>List of available symbols</returns>
        Task<List<string>> GetAvailableSymbolsAsync();

        /// <summary>
        /// ⏰ Get supported timeframes for chart data
        /// </summary>
        /// <returns>List of supported timeframes</returns>
        Task<List<TimeframeInfo>> GetSupportedTimeframesAsync();

        /// <summary>
        /// 📊 Get market data stream status
        /// </summary>
        /// <returns>Stream status information</returns>
        Task<MarketDataStatus> GetStreamStatusAsync();
    }
}