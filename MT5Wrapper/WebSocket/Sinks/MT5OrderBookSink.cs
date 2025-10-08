using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using static System.Threading.Interlocked;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;
using Microsoft.Extensions.Logging;
using MT5Wrapper.WebSocket.DTOs;
using MT5Wrapper.WebSocket.Interfaces;

namespace MT5Wrapper.WebSocket.Sinks
{
    /// <summary>
    /// 📊 MT5 Order Book Sink - Real-time order book data receiver
    /// Polls for order book updates from MT5 since IMTBookSink is sealed
    /// Provides high-performance order book data streaming
    /// </summary>
    public class MT5OrderBookSink : IDisposable
    {
        private readonly ILogger<MT5OrderBookSink> _logger;
        private readonly IOrderBookService _orderBookService;
        private readonly ConcurrentDictionary<string, OrderBookDto> _orderBooks;
        private readonly object _lock = new object();
        private bool _disposed;

        // Performance tracking
        private long _totalUpdates;
        private long _lastUpdateTimestamp;
        private DateTime _startTime;

        public MT5OrderBookSink(
            ILogger<MT5OrderBookSink> logger,
            IOrderBookService orderBookService)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _orderBookService = orderBookService ?? throw new ArgumentNullException(nameof(orderBookService));
            _orderBooks = new ConcurrentDictionary<string, OrderBookDto>();
            _startTime = DateTime.UtcNow;

            _logger.LogInformation("MT5OrderBookSink initialized for real-time order book streaming");
        }


        /// <summary>
        /// 📊 Poll for order book updates for a specific symbol
        /// This method polls MT5 for current order book data
        /// </summary>
        public async Task<OrderBookDto?> PollOrderBookAsync(string symbol)
        {
            try
            {
                var manager = _orderBookService.GetType().GetProperty("MT5Manager", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)?
                    .GetValue(_orderBookService) as dynamic;

                if (manager == null)
                {
                    _logger.LogWarning("MT5 manager not available for order book polling");
                    return null;
                }

                // Get order book from MT5
                var book = new MTBook();
                var result = manager.BookGet(symbol, book);

                if (result != MTRetCode.MT_RET_OK)
                {
                    // _logger.LogDebug("No order book data available for {Symbol}: {Result}", symbol, result);
                    return null;
                }

                // Convert MT5 book data to our DTO format
                var orderBook = ConvertMT5BookToOrderBook(book);

                // Update internal cache
                _orderBooks[symbol] = orderBook;

                // Update performance metrics
                Interlocked.Increment(ref _totalUpdates);
                _lastUpdateTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

                // _logger.LogInformation("Polled order book for {Symbol}: {BidCount} bids, {AskCount} asks",
                //     symbol, orderBook.BidCount, orderBook.AskCount);

                return orderBook;

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error polling order book for {Symbol}", symbol);
                return null;
            }
        }

        /// <summary>
        /// 🔄 Convert MT5 MTBook structure to our OrderBookDto
        /// Processes the items array to separate bids and asks
        /// </summary>
        private OrderBookDto ConvertMT5BookToOrderBook(MTBook mt5Book)
        {
            var orderBook = new OrderBookDto
            {
                Symbol = mt5Book.symbol,
                Timestamp = (long)mt5Book.datetime_msc,
                ServerTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
                UpdateType = "update"
            };

            // Process items array to separate bids and asks
            var bids = new List<OrderBookEntry>();
            var asks = new List<OrderBookEntry>();

            for (uint i = 0; i < mt5Book.items_total && i < mt5Book.items.Length; i++)
            {
                var item = mt5Book.items[i];

                var entry = new OrderBookEntry
                {
                    Price = item.price,
                    Volume = item.volume,
                    Orders = 1, // MT5 doesn't provide order count, default to 1
                };

                // Determine if it's a bid or ask based on type
                switch ((MTBookItem.EnBookItem)item.type)
                {
                    case MTBookItem.EnBookItem.ItemBuy:
                    case MTBookItem.EnBookItem.ItemBuyMarket:
                        entry.Type = "bid";
                        bids.Add(entry);
                        break;
                    case MTBookItem.EnBookItem.ItemSell:
                    case MTBookItem.EnBookItem.ItemSellMarket:
                        entry.Type = "ask";
                        asks.Add(entry);
                        break;
                    case MTBookItem.EnBookItem.ItemReset:
                        // Reset item - ignore for now
                        break;
                }
            }

            // Sort bids by price descending (best bid first)
            orderBook.Bids = bids.OrderByDescending(b => b.Price).ToList();

            // Sort asks by price ascending (best ask first)
            orderBook.Asks = asks.OrderBy(a => a.Price).ToList();

            return orderBook;
        }

        /// <summary>
        /// 📊 Get cached order book for a symbol
        /// </summary>
        public OrderBookDto? GetOrderBook(string symbol)
        {
            return _orderBooks.TryGetValue(symbol, out var orderBook) ? orderBook : null;
        }

        /// <summary>
        /// 🧹 Clear cached order book for a symbol
        /// </summary>
        public void ClearOrderBook(string symbol)
        {
            _orderBooks.TryRemove(symbol, out _);
        }

        /// <summary>
        /// 📈 Get sink performance statistics
        /// </summary>
        public MT5OrderBookSinkStats GetStats()
        {
            return new MT5OrderBookSinkStats
            {
                TotalUpdates = _totalUpdates,
                ActiveSymbols = _orderBooks.Count,
                LastUpdateTimestamp = _lastUpdateTimestamp,
                UptimeSeconds = (long)(DateTime.UtcNow - _startTime).TotalSeconds,
                MemoryUsage = _orderBooks.Count * 1024L // Rough estimate
            };
        }

        /// <summary>
        /// 🗑️ Dispose resources
        /// </summary>
        public void Dispose()
        {
            if (_disposed) return;

            try
            {
                _orderBooks.Clear();
                _disposed = true;

                _logger.LogInformation("MT5OrderBookSink disposed. Total updates processed: {TotalUpdates}", _totalUpdates);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing MT5OrderBookSink");
            }
        }
    }

    /// <summary>
    /// 📊 MT5 Order Book Sink Statistics
    /// </summary>
    public class MT5OrderBookSinkStats
    {
        /// <summary>
        /// Total order book updates processed
        /// </summary>
        public long TotalUpdates { get; set; }

        /// <summary>
        /// Number of symbols currently being tracked
        /// </summary>
        public int ActiveSymbols { get; set; }

        /// <summary>
        /// Timestamp of last update (milliseconds since epoch)
        /// </summary>
        public long LastUpdateTimestamp { get; set; }

        /// <summary>
        /// Sink uptime in seconds
        /// </summary>
        public long UptimeSeconds { get; set; }

        /// <summary>
        /// Estimated memory usage in bytes
        /// </summary>
        public long MemoryUsage { get; set; }
    }
}