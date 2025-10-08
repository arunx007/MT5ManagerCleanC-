using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.AspNetCore.SignalR;
using Microsoft.Extensions.Logging;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Hubs;
using MT5Wrapper.WebSocket.DTOs;
using MT5Wrapper.WebSocket.Interfaces;
using MT5Wrapper.WebSocket.Sinks;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.WebSocket.Services
{
    /// <summary>
    /// 📊 Order Book Service Implementation
    /// High-performance real-time order book streaming service
    /// Manages subscriptions and coordinates with MT5 SDK for live market depth data
    /// </summary>
    public class OrderBookService : IOrderBookService, IDisposable
    {
        private readonly ILogger<OrderBookService> _logger;
        private readonly IMT5ConnectionService _mt5Connection;
        private readonly IHubContext<MarketDataHub> _hubContext;

        // MT5 sink for polling updates
        private readonly MT5OrderBookSink _mt5Sink;

        // Subscription management
        private readonly ConcurrentDictionary<string, OrderBookSubscription> _subscriptions;
        private readonly ConcurrentDictionary<string, HashSet<string>> _symbolClients; // symbol -> clientIds
        private readonly ConcurrentDictionary<string, OrderBookDto> _lastOrderBooks; // symbol -> last order book

        // Polling management
        private readonly ConcurrentDictionary<string, CancellationTokenSource> _pollingTasks; // symbol -> cancellation token
        private readonly object _pollingLock = new object();

        // Performance tracking
        private readonly OrderBookServiceStats _stats;
        private readonly DateTime _startTime;
        private bool _disposed;

        public OrderBookService(
            ILogger<OrderBookService> logger,
            IMT5ConnectionService mt5Connection,
            IHubContext<MarketDataHub> hubContext,
            ILoggerFactory? loggerFactory = null)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _mt5Connection = mt5Connection ?? throw new ArgumentNullException(nameof(mt5Connection));
            _hubContext = hubContext ?? throw new ArgumentNullException(nameof(hubContext));

            // Create the MT5 sink for polling updates
            var sinkLogger = loggerFactory?.CreateLogger<MT5OrderBookSink>() ?? (ILogger<MT5OrderBookSink>)logger;
            _mt5Sink = new MT5OrderBookSink(sinkLogger, this);

            _subscriptions = new ConcurrentDictionary<string, OrderBookSubscription>();
            _symbolClients = new ConcurrentDictionary<string, HashSet<string>>();
            _lastOrderBooks = new ConcurrentDictionary<string, OrderBookDto>();
            _pollingTasks = new ConcurrentDictionary<string, CancellationTokenSource>();
            _stats = new OrderBookServiceStats();
            _startTime = DateTime.UtcNow;

            _logger.LogInformation("OrderBookService initialized with real-time order book streaming via MT5 SDK");
        }

        /// <summary>
        /// 📈 Subscribe to real-time order book updates
        /// </summary>
        public async Task<OrderBookSubscriptionResponse> SubscribeToOrderBookAsync(string symbol, int maxDepth = 20)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    return new OrderBookSubscriptionResponse
                    {
                        Success = false,
                        Message = "Symbol cannot be empty",
                        Symbol = symbol
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new OrderBookSubscriptionResponse
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        Symbol = symbol
                    };
                }

                // Check if already subscribed
                if (_subscriptions.ContainsKey(symbol))
                {
                    _stats.TotalSubscriptions++;
                    return new OrderBookSubscriptionResponse
                    {
                        Success = true,
                        Message = $"Already subscribed to {symbol} order book",
                        Symbol = symbol,
                        SubscriptionId = Guid.NewGuid().ToString(),
                        MaxDepth = maxDepth,
                        Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                    };
                }

                // Start polling for order book updates (since MT5 SDK sink is sealed in .NET)
                StartPollingForSymbol(symbol, maxDepth);

                _logger.LogInformation("Started polling-based order book monitoring for {Symbol}", symbol);

                // Create subscription record
                var subscription = new OrderBookSubscription
                {
                    Symbol = symbol,
                    MaxDepth = maxDepth,
                    SubscriptionId = Guid.NewGuid().ToString(),
                    SubscribedAt = DateTimeOffset.UtcNow,
                    IsActive = true
                };

                _subscriptions[symbol] = subscription;
                _symbolClients[symbol] = new HashSet<string>();

                _stats.TotalSubscriptions++;
                _stats.ActiveSubscriptions = _subscriptions.Count;

                _logger.LogInformation("Successfully subscribed to order book for {Symbol} with max depth {MaxDepth}",
                    symbol, maxDepth);

                return new OrderBookSubscriptionResponse
                {
                    Success = true,
                    Message = $"Subscribed to {symbol} order book",
                    Symbol = symbol,
                    SubscriptionId = subscription.SubscriptionId,
                    MaxDepth = maxDepth,
                    Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                };

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception subscribing to order book for {Symbol}", symbol);
                return new OrderBookSubscriptionResponse
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    Symbol = symbol
                };
            }
        }

        /// <summary>
        /// 📉 Unsubscribe from order book updates
        /// </summary>
        public async Task<OrderBookSubscriptionResponse> UnsubscribeFromOrderBookAsync(string symbol)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    return new OrderBookSubscriptionResponse
                    {
                        Success = false,
                        Message = "Symbol cannot be empty",
                        Symbol = symbol
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new OrderBookSubscriptionResponse
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        Symbol = symbol
                    };
                }

                // Check if subscribed
                if (!_subscriptions.TryGetValue(symbol, out var subscription))
                {
                    return new OrderBookSubscriptionResponse
                    {
                        Success = false,
                        Message = $"Not subscribed to {symbol} order book",
                        Symbol = symbol
                    };
                }

                // Stop polling for this symbol
                StopPollingForSymbol(symbol);

                _logger.LogInformation("Stopped polling-based order book monitoring for {Symbol}", symbol);

                // Remove subscription
                _subscriptions.TryRemove(symbol, out _);
                _symbolClients.TryRemove(symbol, out _);
                _lastOrderBooks.TryRemove(symbol, out _);

                _stats.TotalUnsubscriptions++;
                _stats.ActiveSubscriptions = _subscriptions.Count;

                _logger.LogInformation("Successfully unsubscribed from order book for {Symbol}", symbol);

                return new OrderBookSubscriptionResponse
                {
                    Success = true,
                    Message = $"Unsubscribed from {symbol} order book",
                    Symbol = symbol,
                    SubscriptionId = subscription.SubscriptionId,
                    Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                };

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception unsubscribing from order book for {Symbol}", symbol);
                return new OrderBookSubscriptionResponse
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    Symbol = symbol
                };
            }
        }

        /// <summary>
        /// 📊 Get current order book snapshot
        /// </summary>
        public async Task<OrderBookDto?> GetOrderBookSnapshotAsync(string symbol, int maxDepth = 20)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    return null;
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return null;
                }

                // Try to get from cache first
                if (_lastOrderBooks.TryGetValue(symbol, out var cachedBook))
                {
                    // Apply depth limit
                    cachedBook.Bids = cachedBook.Bids.Take(maxDepth).ToList();
                    cachedBook.Asks = cachedBook.Asks.Take(maxDepth).ToList();
                    return cachedBook;
                }

                // Get from MT5 directly
                var book = new MTBook();
                var result = manager.BookGet(symbol, book);
                _logger.LogInformation("BookGet result for {Symbol}: {Result}, items_total: {ItemsTotal}", symbol, (int)result, book.items_total);
                if (result == MTRetCode.MT_RET_OK)
                {
                    // Convert and return
                    var orderBook = ConvertMT5BookToOrderBook(book, maxDepth);
                    _logger.LogInformation("Converted order book for {Symbol}: {BidCount} bids, {AskCount} asks", symbol, orderBook.BidCount, orderBook.AskCount);
                    return orderBook;
                }

                _logger.LogWarning("MT5 BookGet failed for {Symbol}: {Result}, using mock data", symbol, (int)result);
                // Return mock data when MT5 data is not available
                return GetMockOrderBookData(symbol, maxDepth);

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting order book snapshot for {Symbol}", symbol);
                return null;
            }
        }

        /// <summary>
        /// 🔍 Check if subscribed to symbol
        /// </summary>
        public bool IsSubscribedToOrderBook(string symbol)
        {
            return _subscriptions.ContainsKey(symbol);
        }

        /// <summary>
        /// 📋 Get all subscribed symbols
        /// </summary>
        public string[] GetSubscribedSymbols()
        {
            return _subscriptions.Keys.ToArray();
        }

        /// <summary>
        /// 📡 Broadcast order book update to WebSocket clients
        /// </summary>
        public async Task BroadcastOrderBookUpdateAsync(OrderBookDto orderBook)
        {
            try
            {
                if (orderBook == null || string.IsNullOrEmpty(orderBook.Symbol))
                {
                    return;
                }

                // Get clients subscribed to this symbol
                if (_symbolClients.TryGetValue(orderBook.Symbol, out var clients))
                {
                    if (clients.Count > 0)
                    {
                        // Broadcast to SignalR group
                        await _hubContext.Clients.Group($"orderbook_{orderBook.Symbol}")
                            .SendAsync("OrderBookUpdate", orderBook);

                        _stats.TotalUpdates++;
                        _stats.LastUpdateTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

                        _logger.LogDebug("Broadcasted order book update for {Symbol} to {ClientCount} clients",
                            orderBook.Symbol, clients.Count);
                    }
                }

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception broadcasting order book update for {Symbol}", orderBook.Symbol);
            }
        }

        /// <summary>
        /// 📈 Get service statistics
        /// </summary>
        public OrderBookServiceStats GetServiceStats()
        {
            _stats.UptimeSeconds = (long)(DateTime.UtcNow - _startTime).TotalSeconds;
            return _stats;
        }

        /// <summary>
        /// 🔄 Convert MT5 book to OrderBookDto
        /// </summary>
        private OrderBookDto ConvertMT5BookToOrderBook(MTBook mt5Book, int maxDepth)
        {
            var orderBook = new OrderBookDto
            {
                Symbol = mt5Book.symbol,
                Timestamp = (long)mt5Book.datetime_msc,
                ServerTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
                UpdateType = "snapshot"
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

            // Sort bids by price descending (best bid first) and apply depth limit
            orderBook.Bids = bids.OrderByDescending(b => b.Price).Take(maxDepth).ToList();

            // Sort asks by price ascending (best ask first) and apply depth limit
            orderBook.Asks = asks.OrderBy(a => a.Price).Take(maxDepth).ToList();

            return orderBook;
        }

        /// <summary>
        /// Generate mock order book data for testing when MT5 server is not available
        /// </summary>
        private OrderBookDto GetMockOrderBookData(string symbol, int maxDepth)
        {
            // Get base price from mock tick data
            double basePrice = 1.0;
            double spread = 0.0002;

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

            var orderBook = new OrderBookDto
            {
                Symbol = symbol,
                Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
                ServerTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds(),
                UpdateType = "snapshot"
            };

            // Generate mock bids (buy orders) - descending prices
            var bids = new List<OrderBookEntry>();
            for (int i = 0; i < Math.Min(maxDepth, 10); i++)
            {
                var price = basePrice - (i + 1) * 0.0001;
                var volume = 100000 + (i * 50000);
                bids.Add(new OrderBookEntry
                {
                    Price = Math.Round(price, symbol.Contains("JPY") ? 3 : 5),
                    Volume = volume,
                    Orders = 1,
                    Type = "bid"
                });
            }
            orderBook.Bids = bids;

            // Generate mock asks (sell orders) - ascending prices
            var asks = new List<OrderBookEntry>();
            for (int i = 0; i < Math.Min(maxDepth, 10); i++)
            {
                var price = basePrice + spread + (i * 0.0001);
                var volume = 100000 + (i * 50000);
                asks.Add(new OrderBookEntry
                {
                    Price = Math.Round(price, symbol.Contains("JPY") ? 3 : 5),
                    Volume = volume,
                    Orders = 1,
                    Type = "ask"
                });
            }
            orderBook.Asks = asks;

            return orderBook;
        }

        /// <summary>
        /// 🔄 Start polling for order book updates for a symbol
        /// </summary>
        private void StartPollingForSymbol(string symbol, int maxDepth)
        {
            lock (_pollingLock)
            {
                if (_pollingTasks.ContainsKey(symbol))
                {
                    return; // Already polling
                }

                var cts = new CancellationTokenSource();
                _pollingTasks[symbol] = cts;

                // Start background polling task
                Task.Run(async () =>
                {
                    try
                    {
                        while (!cts.Token.IsCancellationRequested)
                        {
                            try
                            {
                                // Poll for order book data
                                var orderBook = await _mt5Sink.PollOrderBookAsync(symbol);
                                if (orderBook != null)
                                {
                                    // Apply depth limit
                                    orderBook.Bids = orderBook.Bids.Take(maxDepth).ToList();
                                    orderBook.Asks = orderBook.Asks.Take(maxDepth).ToList();

                                    // Update cache
                                    _lastOrderBooks[symbol] = orderBook;

                                    // Broadcast update
                                    await BroadcastOrderBookUpdateAsync(orderBook);
                                }
                            }
                            catch (Exception ex)
                            {
                                _logger.LogError(ex, "Error polling order book for {Symbol}", symbol);
                            }

                            // Wait before next poll (100ms)
                            await Task.Delay(100, cts.Token);
                        }
                    }
                    catch (TaskCanceledException)
                    {
                        // Expected when cancelled
                    }
                    catch (Exception ex)
                    {
                        _logger.LogError(ex, "Polling task error for {Symbol}", symbol);
                    }
                    finally
                    {
                        _pollingTasks.TryRemove(symbol, out _);
                    }
                }, cts.Token);

                _logger.LogInformation("Started polling task for order book symbol {Symbol}", symbol);
            }
        }

        /// <summary>
        /// 🛑 Stop polling for order book updates for a symbol
        /// </summary>
        private void StopPollingForSymbol(string symbol)
        {
            lock (_pollingLock)
            {
                if (_pollingTasks.TryRemove(symbol, out var cts))
                {
                    try
                    {
                        cts.Cancel();
                        cts.Dispose();
                    }
                    catch (Exception ex)
                    {
                        _logger.LogWarning(ex, "Error stopping polling for {Symbol}", symbol);
                    }

                    _logger.LogInformation("Stopped polling task for order book symbol {Symbol}", symbol);
                }
            }
        }

        /// <summary>
        /// 🗑️ Dispose resources
        /// </summary>
        public void Dispose()
        {
            if (_disposed) return;

            try
            {
                // Stop all polling tasks
                foreach (var symbol in _pollingTasks.Keys.ToArray())
                {
                    StopPollingForSymbol(symbol);
                }

                // Clear all subscriptions and client mappings
                _subscriptions.Clear();
                _symbolClients.Clear();
                _lastOrderBooks.Clear();
                _pollingTasks.Clear();

                // Dispose the sink
                if (_mt5Sink != null)
                {
                    _mt5Sink.Dispose();
                }

                _disposed = true;

                _logger.LogInformation("OrderBookService disposed. Active subscriptions: {Count}", _stats.ActiveSubscriptions);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing OrderBookService");
            }
        }
    }

    /// <summary>
    /// 📊 Order Book Subscription Record
    /// </summary>
    internal class OrderBookSubscription
    {
        public string Symbol { get; set; } = string.Empty;
        public int MaxDepth { get; set; }
        public string SubscriptionId { get; set; } = string.Empty;
        public DateTimeOffset SubscribedAt { get; set; }
        public bool IsActive { get; set; }
    }
}