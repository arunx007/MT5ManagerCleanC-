using System;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.SignalR;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.MarketData.Interfaces;
using MT5Wrapper.MarketData.DTOs;
using MT5Wrapper.WebSocket.Interfaces;
using MT5Wrapper.WebSocket.DTOs;
using MT5Wrapper.Core.Interfaces;
using Microsoft.Extensions.Logging;

namespace MT5Wrapper.Hubs
{
    /// <summary>
    /// SignalR Hub for real-time market data streaming
    /// Handles WebSocket connections for live tick data and price updates
    /// </summary>
    [Authorize]
    public class MarketDataHub : Hub
    {
        private readonly ILogger<MarketDataHub> _logger;
        private readonly IMarketDataService _marketDataService;
        private readonly IMT5ConnectionService _mt5Connection;
        private readonly IOrderBookService _orderBookService;
        private readonly IPositionService _positionService;

        public MarketDataHub(
            ILogger<MarketDataHub> logger,
            IMarketDataService marketDataService,
            IMT5ConnectionService mt5Connection,
            IOrderBookService orderBookService,
            IPositionService positionService)
        {
            _logger = logger;
            _marketDataService = marketDataService;
            _mt5Connection = mt5Connection;
            _orderBookService = orderBookService;
            _positionService = positionService;
        }

        /// <summary>
        /// Called when a client connects to the hub
        /// </summary>
        public override async Task OnConnectedAsync()
        {
            var clientId = Context.ConnectionId;
            var userId = Context.User?.Identity?.Name ?? "Anonymous";

            _logger.LogInformation("Client {ClientId} connected to MarketDataHub (User: {UserId})", clientId, userId);

            // Send welcome message
            await Clients.Caller.SendAsync("Connected", new
            {
                message = "Connected to MT5 Market Data Hub",
                connectionId = clientId,
                timestamp = DateTimeOffset.UtcNow,
                serverTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });

            await base.OnConnectedAsync();
        }

        /// <summary>
        /// Called when a client disconnects from the hub
        /// </summary>
        public override async Task OnDisconnectedAsync(Exception? exception)
        {
            var clientId = Context.ConnectionId;
            var userId = Context.User?.Identity?.Name ?? "Anonymous";

            _logger.LogInformation("Client {ClientId} disconnected from MarketDataHub (User: {UserId})", clientId, userId);

            if (exception != null)
            {
                _logger.LogWarning(exception, "Client {ClientId} disconnected with error", clientId);
            }

            await base.OnDisconnectedAsync(exception);
        }

        /// <summary>
        /// Subscribe to real-time tick data for a symbol
        /// </summary>
        public async Task SubscribeToTicks(string symbol)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidSymbol",
                        message = "Symbol cannot be empty",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) subscribing to ticks for {Symbol}",
                    clientId, userId, symbol);

                // Add to group for this symbol
                await Groups.AddToGroupAsync(clientId, $"ticks_{symbol}");

                // Send confirmation
                await Clients.Caller.SendAsync("Subscribed", new
                {
                    type = "ticks",
                    symbol = symbol,
                    status = "subscribed",
                    timestamp = DateTimeOffset.UtcNow
                });

                // Send current tick data immediately if available
                var currentTick = await _marketDataService.GetCurrentTickAsync(symbol);
                if (currentTick != null)
                {
                    await Clients.Caller.SendAsync("TickData", currentTick);
                }

                // TODO: Start background task to stream real-time data
                // This would typically involve setting up a timer or event handler
                // to periodically fetch and send updated tick data

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error subscribing to ticks for symbol {Symbol}", symbol);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "SubscriptionError",
                    message = $"Failed to subscribe to {symbol}: {ex.Message}",
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// Unsubscribe from real-time tick data for a symbol
        /// </summary>
        public async Task UnsubscribeFromTicks(string symbol)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidSymbol",
                        message = "Symbol cannot be empty",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) unsubscribing from ticks for {Symbol}",
                    clientId, userId, symbol);

                // Remove from group
                await Groups.RemoveFromGroupAsync(clientId, $"ticks_{symbol}");

                // Send confirmation
                await Clients.Caller.SendAsync("Unsubscribed", new
                {
                    type = "ticks",
                    symbol = symbol,
                    status = "unsubscribed",
                    timestamp = DateTimeOffset.UtcNow
                });

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error unsubscribing from ticks for symbol {Symbol}", symbol);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "UnsubscriptionError",
                    message = $"Failed to unsubscribe from {symbol}: {ex.Message}",
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// Subscribe to real-time price quotes for multiple symbols
        /// </summary>
        public async Task SubscribeToQuotes(string[] symbols)
        {
            try
            {
                if (symbols == null || symbols.Length == 0)
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidSymbols",
                        message = "Symbols array cannot be empty",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) subscribing to quotes for {Count} symbols",
                    clientId, userId, symbols.Length);

                var subscribedSymbols = new System.Collections.Generic.List<string>();

                foreach (var symbol in symbols)
                {
                    if (!string.IsNullOrWhiteSpace(symbol))
                    {
                        await Groups.AddToGroupAsync(clientId, $"quotes_{symbol}");
                        subscribedSymbols.Add(symbol);
                    }
                }

                // Send confirmation
                await Clients.Caller.SendAsync("Subscribed", new
                {
                    type = "quotes",
                    symbols = subscribedSymbols,
                    status = "subscribed",
                    timestamp = DateTimeOffset.UtcNow
                });

                // Send current data for all symbols
                foreach (var symbol in subscribedSymbols)
                {
                    var currentTick = await _marketDataService.GetCurrentTickAsync(symbol);
                    if (currentTick != null)
                    {
                        await Clients.Caller.SendAsync("QuoteData", currentTick);
                    }
                }

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error subscribing to quotes for multiple symbols");
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "SubscriptionError",
                    message = $"Failed to subscribe to quotes: {ex.Message}",
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// Get connection status and statistics
        /// </summary>
        public async Task GetStatus()
        {
            try
            {
                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                // Get MT5 connection status
                var mt5Stats = _mt5Connection.GetConnectionStats();

                await Clients.Caller.SendAsync("Status", new
                {
                    connectionId = clientId,
                    userId = userId,
                    mt5Connection = new
                    {
                        isConnected = _mt5Connection.IsConnected,
                        connectionState = mt5Stats.ConnectionState,
                        uptimeSeconds = mt5Stats.UptimeSeconds,
                        totalAttempts = mt5Stats.TotalConnectionAttempts,
                        successfulConnections = mt5Stats.SuccessfulConnections,
                        failedConnections = mt5Stats.FailedConnections
                    },
                    serverTime = DateTimeOffset.UtcNow,
                    timestamp = DateTimeOffset.UtcNow
                });

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting status");
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "StatusError",
                    message = $"Failed to get status: {ex.Message}",
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📊 Subscribe to real-time order book updates
        /// </summary>
        public async Task SubscribeToOrderBook(string symbol, int maxDepth = 20)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidSymbol",
                        message = "Symbol cannot be empty",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) subscribing to order book for {Symbol} with depth {MaxDepth}",
                    clientId, userId, symbol, maxDepth);

                // Subscribe to order book service
                var result = await _orderBookService.SubscribeToOrderBookAsync(symbol, maxDepth);

                if (result.Success)
                {
                    // Add client to order book group
                    await Groups.AddToGroupAsync(clientId, $"orderbook_{symbol}");

                    // Send confirmation
                    await Clients.Caller.SendAsync("OrderBookSubscribed", new
                    {
                        symbol = symbol,
                        maxDepth = maxDepth,
                        subscriptionId = result.SubscriptionId,
                        status = "subscribed",
                        timestamp = DateTimeOffset.UtcNow
                    });

                    // Send current order book snapshot if available
                    var snapshot = await _orderBookService.GetOrderBookSnapshotAsync(symbol, maxDepth);
                    if (snapshot != null)
                    {
                        await Clients.Caller.SendAsync("OrderBookSnapshot", snapshot);
                    }
                }
                else
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "OrderBookSubscriptionError",
                        message = result.Message,
                        symbol = symbol,
                        timestamp = DateTimeOffset.UtcNow
                    });
                }

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error subscribing to order book for symbol {Symbol}", symbol);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "OrderBookSubscriptionException",
                    message = $"Failed to subscribe to order book: {ex.Message}",
                    symbol = symbol,
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📉 Unsubscribe from order book updates
        /// </summary>
        public async Task UnsubscribeFromOrderBook(string symbol)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidSymbol",
                        message = "Symbol cannot be empty",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) unsubscribing from order book for {Symbol}",
                    clientId, userId, symbol);

                // Unsubscribe from order book service
                var result = await _orderBookService.UnsubscribeFromOrderBookAsync(symbol);

                // Remove client from order book group
                await Groups.RemoveFromGroupAsync(clientId, $"orderbook_{symbol}");

                // Send confirmation
                await Clients.Caller.SendAsync("OrderBookUnsubscribed", new
                {
                    symbol = symbol,
                    status = "unsubscribed",
                    timestamp = DateTimeOffset.UtcNow
                });

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error unsubscribing from order book for symbol {Symbol}", symbol);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "OrderBookUnsubscriptionException",
                    message = $"Failed to unsubscribe from order book: {ex.Message}",
                    symbol = symbol,
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📊 Get current order book snapshot
        /// </summary>
        public async Task GetOrderBookSnapshot(string symbol, int maxDepth = 20)
        {
            try
            {
                if (string.IsNullOrWhiteSpace(symbol))
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidSymbol",
                        message = "Symbol cannot be empty",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var snapshot = await _orderBookService.GetOrderBookSnapshotAsync(symbol, maxDepth);
                if (snapshot != null)
                {
                    await Clients.Caller.SendAsync("OrderBookSnapshot", snapshot);
                }
                else
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "OrderBookSnapshotError",
                        message = $"No order book data available for {symbol}",
                        symbol = symbol,
                        timestamp = DateTimeOffset.UtcNow
                    });
                }

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting order book snapshot for symbol {Symbol}", symbol);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "OrderBookSnapshotException",
                    message = $"Failed to get order book snapshot: {ex.Message}",
                    symbol = symbol,
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📈 Get order book service status
        /// </summary>
        public async Task GetOrderBookStatus()
        {
            try
            {
                var stats = _orderBookService.GetServiceStats();
                var subscribedSymbols = _orderBookService.GetSubscribedSymbols();

                await Clients.Caller.SendAsync("OrderBookStatus", new
                {
                    activeSubscriptions = stats.ActiveSubscriptions,
                    totalSubscriptions = stats.TotalSubscriptions,
                    totalUnsubscriptions = stats.TotalUnsubscriptions,
                    totalUpdates = stats.TotalUpdates,
                    uptimeSeconds = stats.UptimeSeconds,
                    subscribedSymbols = subscribedSymbols,
                    lastUpdateTimestamp = stats.LastUpdateTimestamp,
                    timestamp = DateTimeOffset.UtcNow
                });

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting order book status");
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "OrderBookStatusError",
                    message = $"Failed to get order book status: {ex.Message}",
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📊 Subscribe to real-time position updates for a client
        /// </summary>
        public async Task SubscribeToPositions(ulong clientLogin)
        {
            try
            {
                if (clientLogin == 0)
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidClientLogin",
                        message = "Client login cannot be zero",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) subscribing to positions for client {ClientLogin}",
                    clientId, userId, clientLogin);

                // Subscribe to position service
                var result = await _positionService.SubscribeToPositionsAsync(clientLogin);

                if (result.Success)
                {
                    // Add client to positions group
                    await Groups.AddToGroupAsync(clientId, $"positions_{clientLogin}");

                    // Register WebSocket client with position service
                    _positionService.RegisterWebSocketClient(clientLogin, clientId);

                    // Send confirmation
                    await Clients.Caller.SendAsync("PositionSubscribed", new
                    {
                        clientLogin = clientLogin,
                        subscriptionId = result.SubscriptionId,
                        status = "subscribed",
                        timestamp = DateTimeOffset.UtcNow
                    });

                    // Send current positions snapshot if available
                    var positions = await _positionService.GetPositionsSnapshotAsync(clientLogin);
                    if (positions.Any())
                    {
                        await Clients.Caller.SendAsync("PositionSnapshot", new
                        {
                            clientLogin = clientLogin,
                            positions = positions,
                            count = positions.Count(),
                            timestamp = DateTimeOffset.UtcNow
                        });
                    }
                }
                else
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "PositionSubscriptionError",
                        message = result.Message,
                        clientLogin = clientLogin,
                        timestamp = DateTimeOffset.UtcNow
                    });
                }

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error subscribing to positions for client {ClientLogin}", clientLogin);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "PositionSubscriptionException",
                    message = $"Failed to subscribe to positions: {ex.Message}",
                    clientLogin = clientLogin,
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📉 Unsubscribe from position updates for a client
        /// </summary>
        public async Task UnsubscribeFromPositions(ulong clientLogin)
        {
            try
            {
                if (clientLogin == 0)
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidClientLogin",
                        message = "Client login cannot be zero",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var clientId = Context.ConnectionId;
                var userId = Context.User?.Identity?.Name ?? "Anonymous";

                _logger.LogInformation("Client {ClientId} (User: {UserId}) unsubscribing from positions for client {ClientLogin}",
                    clientId, userId, clientLogin);

                // Unsubscribe from position service
                var result = await _positionService.UnsubscribeFromPositionsAsync(clientLogin);

                // Unregister WebSocket client from position service
                _positionService.UnregisterWebSocketClient(clientLogin, clientId);

                // Remove client from positions group
                await Groups.RemoveFromGroupAsync(clientId, $"positions_{clientLogin}");

                // Send confirmation
                await Clients.Caller.SendAsync("PositionUnsubscribed", new
                {
                    clientLogin = clientLogin,
                    status = "unsubscribed",
                    timestamp = DateTimeOffset.UtcNow
                });

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error unsubscribing from positions for client {ClientLogin}", clientLogin);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "PositionUnsubscriptionException",
                    message = $"Failed to unsubscribe from positions: {ex.Message}",
                    clientLogin = clientLogin,
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📊 Get current positions snapshot for a client
        /// </summary>
        public async Task GetPositionsSnapshot(ulong clientLogin)
        {
            try
            {
                if (clientLogin == 0)
                {
                    await Clients.Caller.SendAsync("Error", new
                    {
                        type = "InvalidClientLogin",
                        message = "Client login cannot be zero",
                        timestamp = DateTimeOffset.UtcNow
                    });
                    return;
                }

                var positions = await _positionService.GetPositionsSnapshotAsync(clientLogin);
                if (positions.Any())
                {
                    await Clients.Caller.SendAsync("PositionSnapshot", new
                    {
                        clientLogin = clientLogin,
                        positions = positions,
                        count = positions.Count(),
                        timestamp = DateTimeOffset.UtcNow
                    });
                }
                else
                {
                    await Clients.Caller.SendAsync("PositionSnapshot", new
                    {
                        clientLogin = clientLogin,
                        positions = new PositionDto[0],
                        count = 0,
                        timestamp = DateTimeOffset.UtcNow
                    });
                }

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting positions snapshot for client {ClientLogin}", clientLogin);
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "PositionSnapshotException",
                    message = $"Failed to get positions snapshot: {ex.Message}",
                    clientLogin = clientLogin,
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// 📈 Get position service status
        /// </summary>
        public async Task GetPositionStatus()
        {
            try
            {
                var stats = _positionService.GetServiceStats();
                var subscribedClients = _positionService.GetSubscribedClients();

                await Clients.Caller.SendAsync("PositionStatus", new
                {
                    activeSubscriptions = stats.ActiveSubscriptions,
                    totalSubscriptions = stats.TotalSubscriptions,
                    totalUnsubscriptions = stats.TotalUnsubscriptions,
                    totalUpdates = stats.TotalUpdates,
                    uptimeSeconds = stats.UptimeSeconds,
                    subscribedClients = subscribedClients,
                    lastUpdateTimestamp = stats.LastUpdateTimestamp,
                    timestamp = DateTimeOffset.UtcNow
                });

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting position status");
                await Clients.Caller.SendAsync("Error", new
                {
                    type = "PositionStatusError",
                    message = $"Failed to get position status: {ex.Message}",
                    timestamp = DateTimeOffset.UtcNow
                });
            }
        }

        /// <summary>
        /// Ping/Pong for connection health monitoring
        /// </summary>
        public async Task Ping()
        {
            await Clients.Caller.SendAsync("Pong", new
            {
                timestamp = DateTimeOffset.UtcNow,
                serverTime = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });
        }
    }
}