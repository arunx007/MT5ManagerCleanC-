using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;
using Microsoft.Extensions.Logging;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Core.Services;
using MT5Wrapper.MarketData.DTOs;
using MT5Wrapper.MarketData.Interfaces;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace MT5Wrapper.MarketData.Services
{
    /// <summary>
    /// 📊 MARKET DATA SERVICE - Implementation for charts, quotes, and historical data
    /// Provides OHLC/candlestick data, real-time quotes, and historical price information
    /// </summary>
    public class MarketDataService : IMarketDataService, IDisposable
    {
        private readonly ILogger<MarketDataService> _logger;
        private readonly IMT5ConnectionService _mt5Connection;
        private bool _disposed;

        public MarketDataService(
            ILogger<MarketDataService> logger,
            IMT5ConnectionService mt5Connection)
        {
            Console.WriteLine("MarketDataService constructor called");
            _logger = logger;
            _mt5Connection = mt5Connection;
            Console.WriteLine("MarketDataService constructor completed");
        }

        /// <summary>
        /// 📈 Get OHLC chart data for technical analysis
        /// </summary>
        public async Task<ChartDataResponse?> GetChartDataAsync(ChartDataRequest request)
        {
            try
            {
                _logger.LogInformation($"Getting chart data for {request.Symbol}, timeframe: {request.Timeframe}");

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    _logger.LogError("MT5 Manager not connected");
                    return null;
                }

                // Use ChartRequest method to get historical chart data
                MTRetCode result;
                var chartArray = manager.ChartRequest(request.Symbol, request.StartTime, request.EndTime, out result);

                if (result != MTRetCode.MT_RET_OK || chartArray == null || chartArray.Length == 0)
                {
                    _logger.LogWarning($"No chart data returned for {request.Symbol}");
                    return null;
                }

                var bars = new List<ChartBar>();
                var total = Math.Min((uint)chartArray.Length, (uint)request.Limit);

                for (uint i = 0; i < total; i++)
                {
                    var bar = chartArray[i];
                    bars.Add(new ChartBar
                    {
                        Symbol = request.Symbol,
                        Timestamp = (long)bar.datetime_msc, // Already in milliseconds
                        Open = bar.open,
                        High = bar.high,
                        Low = bar.low,
                        Close = bar.close,
                        Volume = bar.tick_volume,
                        Spread = (int)bar.spread,
                        RealVolume = (ulong)bar.volume
                    });
                }

                return new ChartDataResponse
                {
                    Symbol = request.Symbol,
                    Timeframe = request.Timeframe,
                    Bars = bars,
                    RequestTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                };
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting chart data for {request.Symbol}");
                return null;
            }
        }

        /// <summary>
        /// 💰 Get current live tick data (bid/ask prices)
        /// </summary>
        public async Task<TickDataDto?> GetCurrentTickAsync(string symbol, string tenantId = null)
        {
            try
            {
                _logger.LogInformation($"Getting current tick for {symbol} (Tenant: {tenantId ?? "default"})");

                // Get manager for specific tenant
                dynamic manager = string.IsNullOrEmpty(tenantId)
                    ? _mt5Connection.Manager
                    : (_mt5Connection as dynamic).GetManagerForTenant(tenantId);

                if (manager == null)
                {
                    _logger.LogWarning($"MT5 Manager not connected for tenant {tenantId}, using mock data");
                    return GetMockTickData(symbol);
                }

                // Use TickLast method for current tick data
                var tick = new MTTickShort();
                var result = manager.TickLast(symbol, out tick);

                if (result == MTRetCode.MT_RET_OK)
                {
                    var tickData = new TickDataDto
                    {
                        Symbol = symbol,
                        Bid = tick.bid,
                        Ask = tick.ask,
                        Last = tick.last,
                        Volume = (long)tick.volume,
                        Timestamp = (ulong)tick.datetime_msc,
                        Spread = (int)((tick.ask - tick.bid) * 100000), // Calculate spread in points
                        Flags = (uint)tick.flags
                    };

                    return tickData;
                }
                else
                {
                    _logger.LogWarning($"Failed to get current tick for {symbol}: {result}, using mock data");
                    return GetMockTickData(symbol);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting current tick for {symbol}, using mock data");
                return GetMockTickData(symbol);
            }
        }

        /// <summary>
        /// Generate mock tick data for testing when MT5 server is not available
        /// </summary>
        private TickDataDto GetMockTickData(string symbol)
        {
            // Generate realistic mock data based on symbol
            double basePrice = 1.0;
            double spread = 0.0002; // 2 pips spread

            switch (symbol.ToUpper())
            {
                case "EURUSD":
                    basePrice = 1.0850 + (DateTime.Now.Second % 100) * 0.0001;
                    break;
                case "GBPUSD":
                    basePrice = 1.2750 + (DateTime.Now.Second % 100) * 0.0001;
                    break;
                case "USDJPY":
                    basePrice = 157.50 + (DateTime.Now.Second % 100) * 0.01;
                    spread = 0.02;
                    break;
                case "AUDUSD":
                    basePrice = 0.6650 + (DateTime.Now.Second % 100) * 0.0001;
                    break;
                case "USDCAD":
                    basePrice = 1.3550 + (DateTime.Now.Second % 100) * 0.0001;
                    break;
                case "USDCHF":
                    basePrice = 0.9050 + (DateTime.Now.Second % 100) * 0.0001;
                    break;
                default:
                    basePrice = 1.0000 + (DateTime.Now.Second % 100) * 0.0001;
                    break;
            }

            var bid = Math.Round(basePrice, symbol.Contains("JPY") ? 3 : 5);
            var ask = Math.Round(bid + spread, symbol.Contains("JPY") ? 3 : 5);

            return new TickDataDto
            {
                Symbol = symbol,
                Bid = bid,
                Ask = ask,
                Last = bid,
                Volume = 1000000 + (DateTime.Now.Second * 10000),
                Timestamp = (ulong)DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
                Spread = (int)(spread * (symbol.Contains("JPY") ? 1000 : 100000)), // Spread in points
                Flags = 2 // TICK_FLAG_BID | TICK_FLAG_ASK
            };
        }

        /// <summary>
        /// 📜 Get historical price data (ticks or bars)
        /// </summary>
        public async Task<HistoricalDataResponse?> GetHistoricalDataAsync(HistoricalDataRequest request)
        {
            try
            {
                _logger.LogInformation($"Getting historical {request.DataType} data for {request.Symbol}");

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    _logger.LogError("MT5 Manager not connected");
                    return null;
                }

                if (request.DataType.ToLower() == "ticks")
                {
                    return await GetHistoricalTicksAsync(request);
                }
                else if (request.DataType.ToLower() == "bars")
                {
                    return await GetHistoricalBarsAsync(request);
                }
                else
                {
                    _logger.LogWarning($"Unsupported data type: {request.DataType}");
                    return null;
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting historical data for {request.Symbol}");
                return null;
            }
        }

        /// <summary>
        /// 📋 Get list of available symbols for trading
        /// </summary>
        public async Task<List<string>> GetAvailableSymbolsAsync()
        {
            try
            {
                _logger.LogInformation("Getting available symbols");

                _logger.LogInformation($"MT5ConnectionService.IsConnected: {_mt5Connection.IsConnected}");
                _logger.LogInformation($"MT5ConnectionService.Manager is null: {_mt5Connection.Manager == null}");

                // Try to test the connection
                var testResult = await _mt5Connection.TestConnectionAsync();
                _logger.LogInformation($"Connection test result: {testResult.IsSuccessful}, ServerInfo: {testResult.ServerInfo}, Error: {testResult.ErrorMessage}");

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    _logger.LogError("MT5 Manager not connected - manager object is null");
                    return new List<string>();
                }

                // Use Selected symbols approach as per MT5 SDK
                var symbols = new List<string>();
        
                // First, select all symbols
                var selectResult = manager.SelectedAddAll();
                _logger.LogInformation($"SelectedAddAll result: {selectResult}");
        
                if (selectResult != MTRetCode.MT_RET_OK)
                {
                    _logger.LogWarning($"Failed to select all symbols, result: {selectResult}");
                    return symbols;
                }
        
                // Get total selected symbols
                var total = manager.SelectedTotal();
                _logger.LogInformation($"SelectedTotal returned: {total}");
        
                if (total == 0)
                {
                    _logger.LogWarning("No symbols selected in MT5 server. This could mean:");
                    _logger.LogWarning("1. The MT5 server has no symbols configured");
                    _logger.LogWarning("2. The manager account doesn't have permission to access symbols");
                    _logger.LogWarning("3. Symbols are group-specific and need to be accessed differently");
                    return symbols;
                }
        
                // Get each symbol using SymbolNext
                for (uint i = 0; i < total; i++)
                {
                    var symbol = manager.SymbolCreate();
                    if (symbol != null)
                    {
                        var result = manager.SymbolNext(i, symbol);
                        _logger.LogInformation($"SymbolNext({i}) result: {result}");

                        if (result == MTRetCode.MT_RET_OK)
                        {
                            var symbolName = symbol.Symbol();
                            if (!string.IsNullOrEmpty(symbolName))
                            {
                                symbols.Add(symbolName);
                                _logger.LogInformation($"Added symbol: {symbolName}");
                            }
                        }
                        else
                        {
                            _logger.LogWarning($"Failed to get symbol at position {i}, result: {result}");
                        }
                        symbol.Release();
                    }
                    else
                    {
                        _logger.LogWarning($"Failed to create symbol object at position {i}");
                    }
                }
        
                _logger.LogInformation($"Successfully retrieved {symbols.Count} symbols out of {total} selected");
                return symbols;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting available symbols");
                return new List<string>();
            }
        }

        /// <summary>
        /// ⏰ Get supported timeframes for chart data
        /// </summary>
        public async Task<List<TimeframeInfo>> GetSupportedTimeframesAsync()
        {
            return await Task.FromResult(new List<TimeframeInfo>
            {
                new TimeframeInfo { Value = 1, Name = "M1", Description = "1 minute" },
                new TimeframeInfo { Value = 5, Name = "M5", Description = "5 minutes" },
                new TimeframeInfo { Value = 15, Name = "M15", Description = "15 minutes" },
                new TimeframeInfo { Value = 30, Name = "M30", Description = "30 minutes" },
                new TimeframeInfo { Value = 60, Name = "H1", Description = "1 hour" },
                new TimeframeInfo { Value = 240, Name = "H4", Description = "4 hours" },
                new TimeframeInfo { Value = 1440, Name = "D1", Description = "1 day" },
                new TimeframeInfo { Value = 10080, Name = "W1", Description = "1 week" },
                new TimeframeInfo { Value = 43200, Name = "MN1", Description = "1 month" }
            });
        }

        /// <summary>
        /// 📊 Get market data stream status
        /// </summary>
        public async Task<MarketDataStatus> GetStreamStatusAsync()
        {
            var manager = _mt5Connection.Manager;
            var symbols = await GetAvailableSymbolsAsync();

            return new MarketDataStatus
            {
                IsConnected = manager != null,
                ActiveSubscriptions = 0, // TODO: Track active subscriptions
                TotalSymbols = symbols.Count,
                LastUpdate = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
                WebSocketEndpoint = "/hubs/marketdata", // TODO: Implement WebSocket
                UptimeSeconds = 0 // TODO: Track connection uptime
            };
        }

        private async Task<HistoricalDataResponse?> GetHistoricalTicksAsync(HistoricalDataRequest request)
        {
            try
            {
                var manager = _mt5Connection.Manager;
                if (manager == null) return null;

                // Use TickHistoryRequest method to get historical tick data
                MTRetCode result;
                var ticks = manager.TickHistoryRequest(request.Symbol, request.StartDate, request.EndDate, out result);

                if (result != MTRetCode.MT_RET_OK || ticks == null || ticks.Length == 0)
                {
                    return null;
                }

                var tickList = new List<TickDataDto>();
                var limit = Math.Min(ticks.Length, request.Limit);

                for (int i = 0; i < limit; i++)
                {
                    var tick = ticks[i];
                    if (tick.datetime_msc > 0) // Check if tick is valid
                    {
                        tickList.Add(new TickDataDto
                        {
                            Symbol = request.Symbol,
                            Bid = tick.bid,
                            Ask = tick.ask,
                            Last = tick.last,
                            Volume = tick.volume,
                            Timestamp = tick.datetime_msc,
                            Spread = (int)((tick.ask - tick.bid) * 100000),
                            Flags = (uint)tick.flags
                        });
                    }
                }

                return new HistoricalDataResponse
                {
                    Symbol = request.Symbol,
                    DataType = "ticks",
                    Ticks = tickList,
                    RequestTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                };
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting historical ticks for {request.Symbol}");
                return null;
            }
        }

        private async Task<HistoricalDataResponse?> GetHistoricalBarsAsync(HistoricalDataRequest request)
        {
            if (request.Timeframe == null) return null;

            var chartRequest = new ChartDataRequest
            {
                Symbol = request.Symbol,
                Timeframe = (int)request.Timeframe,
                StartTime = request.StartDate,
                EndTime = request.EndDate,
                Limit = request.Limit
            };

            var chartData = await GetChartDataAsync(chartRequest);
            if (chartData == null) return null;

            return new HistoricalDataResponse
            {
                Symbol = request.Symbol,
                DataType = "bars",
                Timeframe = request.Timeframe,
                Bars = chartData.Bars,
                RequestTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            };
        }

        public void Dispose()
        {
            if (_disposed) return;
            _disposed = true;
        }
    }
}