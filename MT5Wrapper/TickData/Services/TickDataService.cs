using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MT5Wrapper.TickData.Interfaces;
using MT5Wrapper.TickData.DTOs;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.MultiTenant.Interfaces;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Caching.Memory;
using System.Collections.Concurrent;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.TickData.Services
{
    /// <summary>
    /// Tick data service implementation - handles real-time market data for multiple tenants
    /// </summary>
    public class TickDataService : ITickDataService, IDisposable
    {
        private readonly ILogger<TickDataService> _logger;
        private readonly IMT5ConnectionService _connectionService;
        private readonly ITenantManagerService _tenantManager;
        private readonly IMemoryCache _cache;
        private readonly ConcurrentDictionary<string, TickSubscription> _subscriptions;
        private readonly ConcurrentDictionary<string, TickDataDto> _lastTicks;
        private bool _disposed;

        public TickDataService(
            ILogger<TickDataService> logger,
            IMT5ConnectionService connectionService,
            ITenantManagerService tenantManager,
            IMemoryCache cache)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _connectionService = connectionService ?? throw new ArgumentNullException(nameof(connectionService));
            _tenantManager = tenantManager ?? throw new ArgumentNullException(nameof(tenantManager));
            _cache = cache ?? throw new ArgumentNullException(nameof(cache));
            _subscriptions = new ConcurrentDictionary<string, TickSubscription>();
            _lastTicks = new ConcurrentDictionary<string, TickDataDto>();
        }

        /// <inheritdoc/>
        public async Task<TickDataDto?> GetCurrentTickAsync(string tenantId, string symbol)
        {
            try
            {
                _logger.LogDebug("Getting current tick for symbol {Symbol} in tenant {TenantId}", symbol, tenantId);

                // Validate tenant
                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    _logger.LogWarning("Invalid or inactive tenant {TenantId}", tenantId);
                    return null;
                }

                // Check cache first
                var cacheKey = $"tick:{tenantId}:{symbol}";
                if (_cache.TryGetValue<TickDataDto>(cacheKey, out var cachedTick))
                {
                    return cachedTick;
                }

                // Get MT5 manager for this tenant
                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    _logger.LogError("No MT5 manager available for tenant {TenantId}", tenantId);
                    return null;
                }

                // Get tick from MT5
                var tick = new MTTickShort();
                var result = mt5Manager.TickLast(symbol, out tick);

                if (result == MTRetCode.MT_RET_OK)
                {
                    var tickDto = TickDataDto.FromMT5Tick(symbol, tick, tenantId, tenantId);

                    // Cache the result
                    _cache.Set(cacheKey, tickDto, TimeSpan.FromSeconds(1));

                    // Store last tick for subscriptions
                    _lastTicks[$"{tenantId}:{symbol}"] = tickDto;

                    _logger.LogDebug("Retrieved tick for {Symbol}: Bid={Bid}, Ask={Ask}", symbol, tickDto.Bid, tickDto.Ask);
                    return tickDto;
                }
                else
                {
                    _logger.LogWarning("Failed to get tick for symbol {Symbol} in tenant {TenantId}: {Result}", symbol, tenantId, result);
                    return null;
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting current tick for symbol {Symbol} in tenant {TenantId}", symbol, tenantId);
                return null;
            }
        }

        /// <inheritdoc/>
        public async Task<IEnumerable<TickDataDto>> GetTickHistoryAsync(string tenantId, string symbol, int count)
        {
            try
            {
                _logger.LogDebug("Getting tick history for symbol {Symbol}, count {Count} in tenant {TenantId}", symbol, count, tenantId);

                var ticks = new List<TickDataDto>();
                var tenant = _tenantManager.GetTenant(tenantId);

                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return ticks;
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return ticks;
                }

                // Get tick history from MT5
                // TODO: Fix TickHistoryRequest method signature
                // var startTime = DateTime.UtcNow.AddMinutes(-count); // Approximate
                // var endTime = DateTime.UtcNow;
                // var result = mt5Manager.TickHistoryRequest(symbol, startTime, endTime, out MTTickShort[]? tickArray);
                // if (result == MTRetCode.MT_RET_OK && tickArray != null)
                // {
                //     var tickCount = Math.Min(count, tickArray.Length);
                //     for (int i = 0; i < tickCount; i++)
                //     {
                //         var tick = tickArray[i];
                //         if (tick.datetime_msc > 0) // Valid tick
                //         {
                //             var tickDto = TickDataDto.FromMT5Tick(symbol, tick, tenantId, tenantId);
                //             ticks.Add(tickDto);
                //         }
                //     }
                // }

                _logger.LogInformation("Retrieved {TickCount} historical ticks for {Symbol} in tenant {TenantId}", ticks.Count, symbol, tenantId);
                return ticks;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting tick history for symbol {Symbol} in tenant {TenantId}", symbol, tenantId);
                return Enumerable.Empty<TickDataDto>();
            }
        }

        /// <inheritdoc/>
        public async Task<string> SubscribeToTickUpdatesAsync(string tenantId, string symbol, Action<TickDataDto> callback)
        {
            try
            {
                var subscriptionId = Guid.NewGuid().ToString("N");
                var subscription = new TickSubscription
                {
                    SubscriptionId = subscriptionId,
                    TenantId = tenantId,
                    Symbol = symbol,
                    Callback = callback,
                    CreatedAt = DateTime.UtcNow
                };

                _subscriptions[subscriptionId] = subscription;

                _logger.LogInformation("Created tick subscription {SubscriptionId} for symbol {Symbol} in tenant {TenantId}",
                    subscriptionId, symbol, tenantId);

                // Start background task to poll for tick updates
                _ = Task.Run(() => PollForTickUpdatesAsync(subscription));

                return subscriptionId;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception creating tick subscription for symbol {Symbol} in tenant {TenantId}", symbol, tenantId);
                throw;
            }
        }

        /// <inheritdoc/>
        public async Task<bool> UnsubscribeFromTickUpdatesAsync(string subscriptionId)
        {
            try
            {
                var removed = _subscriptions.TryRemove(subscriptionId, out _);
                if (removed)
                {
                    _logger.LogInformation("Removed tick subscription {SubscriptionId}", subscriptionId);
                }
                return removed;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception removing tick subscription {SubscriptionId}", subscriptionId);
                return false;
            }
        }

        /// <inheritdoc/>
        public async Task<IEnumerable<string>> GetAvailableSymbolsAsync(string tenantId)
        {
            try
            {
                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return Enumerable.Empty<string>();
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return Enumerable.Empty<string>();
                }

                var symbols = new List<string>();
                var totalSymbols = mt5Manager.SymbolTotal();

                for (uint i = 0; i < totalSymbols; i++)
                {
                    var symbol = mt5Manager.SymbolCreate();
                    if (symbol != null)
                    {
                        var result = mt5Manager.SymbolNext(i, symbol);
                        if (result == MTRetCode.MT_RET_OK && !string.IsNullOrEmpty(symbol.Symbol()))
                        {
                            symbols.Add(symbol.Symbol());
                        }
                        symbol.Release();
                    }
                }

                return symbols;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting available symbols for tenant {TenantId}", tenantId);
                return Enumerable.Empty<string>();
            }
        }

        /// <inheritdoc/>
        public async Task<bool> IsSymbolAvailableAsync(string tenantId, string symbol)
        {
            try
            {
                var symbols = await GetAvailableSymbolsAsync(tenantId);
                return symbols.Contains(symbol);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception checking symbol availability for {Symbol} in tenant {TenantId}", symbol, tenantId);
                return false;
            }
        }

        private CIMTManagerAPI? GetMT5ManagerForTenant(string tenantId)
        {
            // In a real implementation, this would get the appropriate MT5 manager for the tenant
            // For now, we'll use the connection service directly
            return (CIMTManagerAPI)_connectionService.Manager;
        }

        private async Task PollForTickUpdatesAsync(TickSubscription subscription)
        {
            var lastTimestamp = 0L;

            while (_subscriptions.ContainsKey(subscription.SubscriptionId))
            {
                try
                {
                    var currentTick = await GetCurrentTickAsync(subscription.TenantId, subscription.Symbol);

                    if (currentTick != null && currentTick.Timestamp > lastTimestamp)
                    {
                        // New tick data available
                        subscription.Callback(currentTick);
                        lastTimestamp = currentTick.Timestamp;
                    }

                    // Poll every 100ms for real-time updates
                    await Task.Delay(100);
                }
                catch (Exception ex)
                {
                    _logger.LogError(ex, "Exception polling for tick updates for subscription {SubscriptionId}", subscription.SubscriptionId);
                    await Task.Delay(1000); // Wait longer on error
                }
            }
        }

        /// <inheritdoc/>
        public void Dispose()
        {
            if (_disposed) return;

            try
            {
                // Clean up all subscriptions
                _subscriptions.Clear();
                _lastTicks.Clear();
                _disposed = true;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing TickDataService");
            }
        }

        private class TickSubscription
        {
            public string SubscriptionId { get; set; } = string.Empty;
            public string TenantId { get; set; } = string.Empty;
            public string Symbol { get; set; } = string.Empty;
            public Action<TickDataDto> Callback { get; set; } = default!;
            public DateTime CreatedAt { get; set; }
        }
    }
}