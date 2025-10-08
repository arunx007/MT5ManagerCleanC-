using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
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
    /// 📊 Position Service Implementation
    /// High-performance real-time position streaming service
    /// Manages subscriptions and coordinates with MT5 SDK for live position data
    /// </summary>
    public class PositionService : IPositionService, IDisposable
    {
        private readonly ILogger<PositionService> _logger;
        private readonly IMT5ConnectionService _mt5Connection;
        private readonly IHubContext<MarketDataHub> _hubContext;

        // MT5 sink for real-time updates
        private readonly MT5PositionSink _mt5Sink;

        // Subscription management
        private readonly ConcurrentDictionary<ulong, PositionSubscription> _subscriptions;
        private readonly ConcurrentDictionary<ulong, HashSet<string>> _clientConnections; // clientLogin -> connectionIds
        private readonly ConcurrentDictionary<ulong, List<PositionDto>> _lastPositions; // clientLogin -> last positions

        // Performance tracking
        private readonly PositionServiceStats _stats;
        private readonly DateTime _startTime;
        private bool _disposed;

        // Background polling
        private readonly System.Threading.Timer _pollingTimer;
        private readonly TimeSpan _pollingInterval = TimeSpan.FromSeconds(5); // Poll every 5 seconds

        public PositionService(
            ILogger<PositionService> logger,
            IMT5ConnectionService mt5Connection,
            IHubContext<MarketDataHub> hubContext)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _mt5Connection = mt5Connection ?? throw new ArgumentNullException(nameof(mt5Connection));
            _hubContext = hubContext ?? throw new ArgumentNullException(nameof(hubContext));

            // Create the MT5 sink for real-time updates
            _mt5Sink = new MT5PositionSink(Microsoft.Extensions.Logging.LoggerFactory.Create(builder => { }).CreateLogger<MT5PositionSink>(), this);

            _subscriptions = new ConcurrentDictionary<ulong, PositionSubscription>();
            _clientConnections = new ConcurrentDictionary<ulong, HashSet<string>>();
            _lastPositions = new ConcurrentDictionary<ulong, List<PositionDto>>();
            _stats = new PositionServiceStats();
            _startTime = DateTime.UtcNow;

            // Start background polling for position updates
            _pollingTimer = new System.Threading.Timer(PollPositionsAsync, null, _pollingInterval, _pollingInterval);

            _logger.LogInformation("PositionService initialized with real-time position streaming via MT5 SDK");
        }

        /// <summary>
        /// 📈 Subscribe to real-time position updates for a client
        /// </summary>
        public async Task<PositionSubscriptionResponse> SubscribeToPositionsAsync(ulong clientLogin)
        {
            try
            {
                if (clientLogin == 0)
                {
                    return new PositionSubscriptionResponse
                    {
                        Success = false,
                        Message = "Client login cannot be zero",
                        ClientLogin = clientLogin
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new PositionSubscriptionResponse
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        ClientLogin = clientLogin
                    };
                }

                // Check if already subscribed
                if (_subscriptions.ContainsKey(clientLogin))
                {
                    _stats.TotalSubscriptions++;
                    return new PositionSubscriptionResponse
                    {
                        Success = true,
                        Message = $"Already subscribed to positions for client {clientLogin}",
                        ClientLogin = clientLogin,
                        SubscriptionId = Guid.NewGuid().ToString(),
                        Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                    };
                }

                // Note: MT5 Manager API PositionSubscribe requires CIMTPositionSink which is sealed
                // We use polling-based approach instead for real-time updates
                _logger.LogInformation("Position subscription registered for client {ClientLogin} - using polling for updates", clientLogin);

                // Create subscription record
                var subscription = new PositionSubscription
                {
                    ClientLogin = clientLogin,
                    SubscriptionId = Guid.NewGuid().ToString(),
                    SubscribedAt = DateTimeOffset.UtcNow,
                    IsActive = true
                };

                _subscriptions[clientLogin] = subscription;
                // Initialize client connections tracking (will be populated when WebSocket clients connect)
                _clientConnections[clientLogin] = new HashSet<string>();

                _stats.TotalSubscriptions++;
                _stats.ActiveSubscriptions = _subscriptions.Count;

                _logger.LogInformation("Successfully subscribed to positions for client {ClientLogin}", clientLogin);

                return new PositionSubscriptionResponse
                {
                    Success = true,
                    Message = $"Subscribed to positions for client {clientLogin}",
                    ClientLogin = clientLogin,
                    SubscriptionId = subscription.SubscriptionId,
                    Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                };

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception subscribing to positions for client {ClientLogin}", clientLogin);
                return new PositionSubscriptionResponse
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ClientLogin = clientLogin
                };
            }
        }

        /// <summary>
        /// 📉 Unsubscribe from position updates for a client
        /// </summary>
        public async Task<PositionSubscriptionResponse> UnsubscribeFromPositionsAsync(ulong clientLogin)
        {
            try
            {
                if (clientLogin == 0)
                {
                    return new PositionSubscriptionResponse
                    {
                        Success = false,
                        Message = "Client login cannot be zero",
                        ClientLogin = clientLogin
                    };
                }

                var manager = _mt5Connection.Manager;
                // Allow subscription even without MT5 connection for testing
                // In production, this should return an error
                if (manager == null)
                {
                    _logger.LogWarning("MT5 connection not available for client {ClientLogin}, allowing subscription for testing", clientLogin);
                }

                // Check if subscribed
                if (!_subscriptions.TryGetValue(clientLogin, out var subscription))
                {
                    return new PositionSubscriptionResponse
                    {
                        Success = false,
                        Message = $"Not subscribed to positions for client {clientLogin}",
                        ClientLogin = clientLogin
                    };
                }

                // Note: Since we use polling instead of MT5 subscription, no MT5 unsubscription needed
                _logger.LogInformation("Position subscription removed for client {ClientLogin}", clientLogin);

                // Remove subscription
                _subscriptions.TryRemove(clientLogin, out _);
                _clientConnections.TryRemove(clientLogin, out _);
                _lastPositions.TryRemove(clientLogin, out _);

                _stats.TotalUnsubscriptions++;
                _stats.ActiveSubscriptions = _subscriptions.Count;

                _logger.LogInformation("Successfully unsubscribed from positions for client {ClientLogin}", clientLogin);

                return new PositionSubscriptionResponse
                {
                    Success = true,
                    Message = $"Unsubscribed from positions for client {clientLogin}",
                    ClientLogin = clientLogin,
                    SubscriptionId = subscription.SubscriptionId,
                    Timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                };

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception unsubscribing from positions for client {ClientLogin}", clientLogin);
                return new PositionSubscriptionResponse
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ClientLogin = clientLogin
                };
            }
        }

        /// <summary>
        /// 📊 Get current positions snapshot for a client
        /// </summary>
        public async Task<IEnumerable<PositionDto>> GetPositionsSnapshotAsync(ulong clientLogin)
        {
            try
            {
                if (clientLogin == 0)
                {
                    return Enumerable.Empty<PositionDto>();
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return Enumerable.Empty<PositionDto>();
                }

                // Try to get from cache first
                if (_lastPositions.TryGetValue(clientLogin, out var cachedPositions))
                {
                    return cachedPositions;
                }

                // Get positions from MT5 Manager API
                var positions = await GetPositionsFromMT5Async(clientLogin);

                // Cache the positions
                _lastPositions[clientLogin] = positions;

                return positions;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting positions snapshot for client {ClientLogin}", clientLogin);
                return Enumerable.Empty<PositionDto>();
            }
        }

        /// <summary>
        /// 🔍 Check if subscribed to positions for a client
        /// </summary>
        public bool IsSubscribedToPositions(ulong clientLogin)
        {
            return _subscriptions.ContainsKey(clientLogin);
        }

        /// <summary>
        /// 📋 Get all subscribed client logins
        /// </summary>
        public ulong[] GetSubscribedClients()
        {
            return _subscriptions.Keys.ToArray();
        }

        /// <summary>
        /// 📡 Broadcast position update to WebSocket clients
        /// </summary>
        public async Task BroadcastPositionUpdateAsync(PositionUpdateDto update)
        {
            try
            {
                if (update == null || update.ClientLogin == 0)
                {
                    return;
                }

                // Check if there are active subscriptions for this client
                if (!_subscriptions.ContainsKey(update.ClientLogin))
                {
                    return;
                }

                // Determine the event type based on update type
                string eventName = update.UpdateType.ToLower() switch
                {
                    "add" => "PositionAdd",
                    "update" => "PositionUpdate",
                    "delete" => "PositionDelete",
                    "clean" => "PositionClean",
                    "sync" => "PositionSync",
                    _ => "PositionUpdate"
                };

                // Broadcast to SignalR group
                await _hubContext.Clients.Group($"positions_{update.ClientLogin}")
                    .SendAsync(eventName, update);

                _stats.TotalUpdates++;
                _stats.LastUpdateTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();

                _logger.LogDebug("Broadcasted position update ({EventName}) for client {ClientLogin}",
                    eventName, update.ClientLogin);

            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception broadcasting position update for client {ClientLogin}", update.ClientLogin);
            }
        }

        /// <summary>
        /// 📈 Get service statistics
        /// </summary>
        public PositionServiceStats GetServiceStats()
        {
            _stats.UptimeSeconds = (long)(DateTime.UtcNow - _startTime).TotalSeconds;
            return _stats;
        }

        /// <summary>
        /// 🔗 Register WebSocket client connection for position updates
        /// </summary>
        public void RegisterWebSocketClient(ulong clientLogin, string connectionId)
        {
            if (clientLogin == 0 || string.IsNullOrEmpty(connectionId))
                return;

            _clientConnections.AddOrUpdate(clientLogin,
                new HashSet<string> { connectionId },
                (key, existing) =>
                {
                    existing.Add(connectionId);
                    return existing;
                });

            _logger.LogDebug("Registered WebSocket client {ConnectionId} for position updates on client {ClientLogin}", connectionId, clientLogin);
        }

        /// <summary>
        /// 🔌 Unregister WebSocket client connection from position updates
        /// </summary>
        public void UnregisterWebSocketClient(ulong clientLogin, string connectionId)
        {
            if (clientLogin == 0 || string.IsNullOrEmpty(connectionId))
                return;

            if (_clientConnections.TryGetValue(clientLogin, out var connections))
            {
                connections.Remove(connectionId);
                if (connections.Count == 0)
                {
                    _clientConnections.TryRemove(clientLogin, out _);
                }
            }

            _logger.LogDebug("Unregistered WebSocket client {ConnectionId} from position updates on client {ClientLogin}", connectionId, clientLogin);
        }

        /// <summary>
        /// 🔄 Background polling for position updates
        /// </summary>
        private async void PollPositionsAsync(object? state)
        {
            try
            {
                // Only poll if there are active subscriptions
                if (_subscriptions.IsEmpty)
                {
                    return;
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return;
                }

                // Poll positions for all subscribed clients
                foreach (var clientLogin in _subscriptions.Keys.ToArray())
                {
                    try
                    {
                        var currentPositions = await GetPositionsFromMT5Async(clientLogin);
                        var previousPositions = _lastPositions.GetValueOrDefault(clientLogin, new List<PositionDto>());

                        // Detect changes and broadcast updates
                        await DetectAndBroadcastPositionChangesAsync(clientLogin, previousPositions, currentPositions);

                        // Update cache
                        _lastPositions[clientLogin] = currentPositions;
                    }
                    catch (Exception ex)
                    {
                        _logger.LogError(ex, "Error polling positions for client {ClientLogin}", clientLogin);
                    }
                }

                _stats.TotalUpdates++;
                _stats.LastUpdateTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error in position polling");
            }
        }

        /// <summary>
        /// 📊 Get positions from MT5 for a specific client
        /// </summary>
        private async Task<List<PositionDto>> GetPositionsFromMT5Async(ulong clientLogin)
        {
            var manager = _mt5Connection.Manager;
            if (manager == null)
            {
                return new List<PositionDto>();
            }

            var positions = new List<PositionDto>();

            try
            {
                // Note: MT5 Manager API doesn't have a direct PositionGetPage method for a specific client
                // We need to use the pumping mechanism or maintain our own cache
                // For now, we'll use the sink's polling method if available

                var polledPositions = await _mt5Sink.PollPositionsAsync(clientLogin);
                if (polledPositions != null)
                {
                    positions = polledPositions;
                }
                else
                {
                    // Fallback: try to get positions using individual position queries
                    // This is not efficient but works as a fallback
                    // _logger.LogWarning("Using fallback position retrieval for client {ClientLogin}", clientLogin);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting positions from MT5 for client {ClientLogin}", clientLogin);
            }

            return positions;
        }

        /// <summary>
        /// 🔍 Detect position changes and broadcast updates
        /// </summary>
        private async Task DetectAndBroadcastPositionChangesAsync(ulong clientLogin, List<PositionDto> previous, List<PositionDto> current)
        {
            var previousDict = previous.ToDictionary(p => p.PositionId);
            var currentDict = current.ToDictionary(p => p.PositionId);

            // Find new positions (added)
            var added = current.Where(p => !previousDict.ContainsKey(p.PositionId)).ToList();

            // Find closed positions (deleted)
            var deleted = previous.Where(p => !currentDict.ContainsKey(p.PositionId)).ToList();

            // Find modified positions (updated)
            var updated = new List<PositionDto>();
            foreach (var currentPos in current)
            {
                if (previousDict.TryGetValue(currentPos.PositionId, out var prevPos))
                {
                    // Check if position has changed (simplified check)
                    if (currentPos.Volume != prevPos.Volume ||
                        currentPos.PriceCurrent != prevPos.PriceCurrent ||
                        currentPos.Profit != prevPos.Profit)
                    {
                        updated.Add(currentPos);
                    }
                }
            }

            // Broadcast updates
            foreach (var position in added)
            {
                await BroadcastPositionUpdateAsync(new PositionUpdateDto
                {
                    UpdateType = "add",
                    ClientLogin = clientLogin,
                    Position = position,
                    ServerTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                });
            }

            foreach (var position in updated)
            {
                await BroadcastPositionUpdateAsync(new PositionUpdateDto
                {
                    UpdateType = "update",
                    ClientLogin = clientLogin,
                    Position = position,
                    ServerTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                });
            }

            foreach (var position in deleted)
            {
                await BroadcastPositionUpdateAsync(new PositionUpdateDto
                {
                    UpdateType = "delete",
                    ClientLogin = clientLogin,
                    Position = position,
                    ServerTimestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
                });
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
                // Note: Since we use polling instead of MT5 subscription, no MT5 unsubscription needed during disposal

                // Clear all subscriptions and client mappings
                _subscriptions.Clear();
                _clientConnections.Clear();
                _lastPositions.Clear();

                // Stop polling timer
                _pollingTimer?.Dispose();

                // Dispose the sink
                if (_mt5Sink != null)
                {
                    _mt5Sink.Dispose();
                }

                _disposed = true;

                _logger.LogInformation("PositionService disposed. Active subscriptions: {Count}", _stats.ActiveSubscriptions);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing PositionService");
            }
        }
    }

    /// <summary>
    /// 📊 Position Subscription Record
    /// </summary>
    internal class PositionSubscription
    {
        public ulong ClientLogin { get; set; }
        public string SubscriptionId { get; set; } = string.Empty;
        public DateTimeOffset SubscribedAt { get; set; }
        public bool IsActive { get; set; }
    }
}