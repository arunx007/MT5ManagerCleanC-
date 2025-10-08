using System;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;
using MT5Wrapper.MarketData.DTOs;
using MT5Wrapper.MarketData.Interfaces;
using MT5Wrapper.MultiTenant.Interfaces;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Linq;
using System.Threading.Tasks;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// 📊 MARKET DATA CONTROLLER - Charts, Quotes, and Historical Price Data
    /// Provides OHLC/candlestick data, real-time quotes, and historical price information
    /// </summary>
    [ApiController]
    [Route("api/[controller]")]
    [Produces("application/json")]
    public class MarketDataController : ControllerBase
    {
        private readonly ILogger<MarketDataController> _logger;
        private readonly IMarketDataService _marketDataService;
        private readonly ITenantRoutingService _tenantRouting;

        public MarketDataController(
            ILogger<MarketDataController> logger,
            IMarketDataService marketDataService,
            ITenantRoutingService tenantRouting)
        {
            _logger = logger;
            _marketDataService = marketDataService;
            _tenantRouting = tenantRouting;
        }

        /// <summary>
        /// 📈 GET OHLC CHART DATA - Historical price bars for technical analysis
        /// </summary>
        /// <remarks>
        /// **📈 OHLC PRICE DATA**
        ///
        /// Get historical Open, High, Low, Close (OHLC) price data for any trading symbol.
        /// Perfect for creating candlestick charts, technical analysis, and backtesting.
        ///
        /// **⏰ Supported Timeframes:**
        /// - **1** = 1 minute
        /// - **5** = 5 minutes
        /// - **15** = 15 minutes
        /// - **30** = 30 minutes
        /// - **60** = 1 hour
        /// - **240** = 4 hours
        /// - **1440** = 1 day
        /// - **10080** = 1 week
        /// - **43200** = 1 month
        ///
        /// **📋 Request Body Example:**
        /// ```json
        /// {
        ///   "symbol": "EURUSD",
        ///   "timeframe": 60,
        ///   "startTime": 1640995200,
        ///   "endTime": 1641081600,
        ///   "limit": 1000
        /// }
        /// ```
        ///
        /// **📊 Response includes:**
        /// - Open, High, Low, Close prices
        /// - Volume data
        /// - Timestamp for each bar
        /// - Spread information
        /// </remarks>
        /// <param name="request">Chart data request parameters including symbol, timeframe, and date range</param>
        /// <returns>OHLC chart data response with historical price bars</returns>
        /// <response code="200">Returns the chart data</response>
        /// <response code="400">Invalid request parameters</response>
        /// <response code="404">Symbol not found</response>
        /// <response code="500">Internal server error</response>
        [HttpPost("chart")]
        [ProducesResponseType(typeof(ChartDataResponse), 200)]
        [ProducesResponseType(400)]
        [ProducesResponseType(404)]
        [ProducesResponseType(500)]
        public async Task<ActionResult<ChartDataResponse>> GetChartData([FromBody] ChartDataRequest request)
        {
            try
            {
                _logger.LogInformation($"Getting chart data for {request.Symbol}, timeframe: {request.Timeframe}");

                if (!IsValidTimeframe(request.Timeframe))
                {
                    return BadRequest($"Invalid timeframe: {request.Timeframe}. Supported: 1,5,15,30,60,240,1440,10080,43200");
                }

                if (request.Limit > 10000)
                {
                    return BadRequest("Limit cannot exceed 10000 bars");
                }

                var chartData = await _marketDataService.GetChartDataAsync(request);

                if (chartData == null)
                {
                    return NotFound($"No data found for symbol: {request.Symbol}");
                }

                return Ok(chartData);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting chart data for {request.Symbol}");
                return StatusCode(500, "Internal server error");
            }
        }

        /// <summary>
        /// 💰 Get current live tick data for a symbol
        /// </summary>
        /// <remarks>
        /// **💰 REAL-TIME TICK DATA**
        ///
        /// Get current bid/ask prices and market information for any trading symbol.
        /// Includes spread calculation and volume data.
        ///
        /// **📊 Response includes:**
        /// - Bid and Ask prices
        /// - Last traded price
        /// - Trading volume
        /// - Spread in points
        /// - Timestamp
        /// </remarks>
        /// <param name="symbol">Symbol name (e.g., EURUSD)</param>
        /// <returns>Current live tick data</returns>
        /// <response code="200">Returns the current tick data</response>
        /// <response code="404">Symbol not found</response>
        /// <response code="500">Internal server error</response>
        [HttpGet("tick/{symbol}")]
        [ProducesResponseType(typeof(TickDataDto), 200)]
        [ProducesResponseType(404)]
        [ProducesResponseType(500)]
        public async Task<ActionResult<TickDataDto>> GetCurrentTick([FromRoute] string symbol)
        {
            try
            {
                // Extract tenant ID from request
                var tenant = _tenantRouting.GetTenantFromRequest(Request);
                var tenantId = tenant?.TenantId;

                _logger.LogInformation($"Getting current tick for {symbol} (Tenant: {tenantId ?? "default"})");

                var tickData = await _marketDataService.GetCurrentTickAsync(symbol, tenantId);

                if (tickData == null)
                {
                    return NotFound($"No tick data found for symbol: {symbol}");
                }

                return Ok(tickData);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting current tick for {symbol}");
                return StatusCode(500, "Internal server error");
            }
        }

        /// <summary>
        /// 📜 Get historical price data (ticks or bars)
        /// </summary>
        /// <remarks>
        /// **📜 HISTORICAL PRICE DATA**
        ///
        /// Retrieve historical tick data or OHLC bars for any symbol and date range.
        /// Supports both tick-by-tick data and aggregated bar data.
        ///
        /// **📋 Request Body Example:**
        /// ```json
        /// {
        ///   "symbol": "EURUSD",
        ///   "dataType": "bars",
        ///   "timeframe": 60,
        ///   "startDate": 1640995200,
        ///   "endDate": 1641081600,
        ///   "limit": 1000
        /// }
        /// ```
        ///
        /// **🔧 Data Types:**
        /// - **"ticks"**: Individual tick data (bid/ask/last prices)
        /// - **"bars"**: OHLC aggregated data (requires timeframe)
        /// </remarks>
        /// <param name="request">Historical data request parameters</param>
        /// <returns>Historical price data</returns>
        /// <response code="200">Returns the historical data</response>
        /// <response code="400">Invalid request parameters</response>
        /// <response code="404">Symbol not found</response>
        /// <response code="500">Internal server error</response>
        [HttpPost("historical")]
        [ProducesResponseType(typeof(HistoricalDataResponse), 200)]
        [ProducesResponseType(400)]
        [ProducesResponseType(404)]
        [ProducesResponseType(500)]
        public async Task<ActionResult<HistoricalDataResponse>> GetHistoricalData([FromBody] HistoricalDataRequest request)
        {
            try
            {
                _logger.LogInformation($"Getting historical {request.DataType} data for {request.Symbol}");

                if (request.DataType.ToLower() == "bars" && request.Timeframe == null)
                {
                    return BadRequest("Timeframe is required for bars data type");
                }

                var historicalData = await _marketDataService.GetHistoricalDataAsync(request);

                if (historicalData == null)
                {
                    return NotFound($"No historical data found for symbol: {request.Symbol}");
                }

                return Ok(historicalData);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting historical data for {request.Symbol}");
                return StatusCode(500, "Internal server error");
            }
        }

        /// <summary>
        /// 📋 Get available symbols for chart data
        /// </summary>
        /// <remarks>
        /// **📋 AVAILABLE SYMBOLS**
        ///
        /// Get a complete list of all trading symbols available for market data.
        /// Includes forex pairs, commodities, indices, and other instruments.
        /// </remarks>
        /// <returns>List of available symbols</returns>
        /// <response code="200">Returns the list of symbols</response>
        /// <response code="500">Internal server error</response>
        [HttpGet("symbols")]
        [ProducesResponseType(typeof(List<string>), 200)]
        [ProducesResponseType(500)]
        public async Task<ActionResult<List<string>>> GetAvailableSymbols()
        {
            Console.WriteLine("MarketDataController.GetAvailableSymbols called");
            try
            {
                _logger.LogInformation("Getting available symbols");

                var symbols = await _marketDataService.GetAvailableSymbolsAsync();
                Console.WriteLine($"MarketDataController.GetAvailableSymbols returning {symbols?.Count ?? 0} symbols");
                return Ok(symbols);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting available symbols");
                Console.WriteLine($"MarketDataController.GetAvailableSymbols exception: {ex.Message}");
                return StatusCode(500, "Internal server error");
            }
        }

        /// <summary>
        /// ⏰ Get supported timeframes
        /// </summary>
        /// <remarks>
        /// **⏰ CHART TIMEFRAMES**
        ///
        /// Get all supported timeframe options for chart data and technical analysis.
        /// </remarks>
        /// <returns>List of supported timeframes with descriptions</returns>
        /// <response code="200">Returns the supported timeframes</response>
        [HttpGet("timeframes")]
        [ProducesResponseType(typeof(List<TimeframeInfo>), 200)]
        public async Task<ActionResult<List<TimeframeInfo>>> GetSupportedTimeframes()
        {
            var timeframes = await _marketDataService.GetSupportedTimeframesAsync();
            return Ok(timeframes);
        }

        /// <summary>
        /// 📊 Get market data service status
        /// </summary>
        /// <remarks>
        /// **📊 SERVICE STATUS**
        ///
        /// Get current status of the market data service including connection state,
        /// available symbols, and service health information.
        /// </remarks>
        /// <returns>Market data service status</returns>
        /// <response code="200">Returns the service status</response>
        [HttpGet("status")]
        [ProducesResponseType(typeof(MarketDataStatus), 200)]
        public async Task<ActionResult<MarketDataStatus>> GetStatus()
        {
            var status = await _marketDataService.GetStreamStatusAsync();
            return Ok(status);
        }

        private static bool IsValidTimeframe(int timeframe)
        {
            var validTimeframes = new[] { 1, 5, 15, 30, 60, 240, 1440, 10080, 43200 };
            return validTimeframes.Contains(timeframe);
        }
    }
}