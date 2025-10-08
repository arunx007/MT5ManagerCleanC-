using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Core.Models;
using MT5Wrapper.MultiTenant.Interfaces;
using MT5Wrapper.MultiTenant.Models;
using MetaQuotes.MT5CommonAPI;
using Microsoft.Extensions.Logging.Console;

namespace MT5Wrapper.Core.Services
{
    /// <summary>
    /// Multi-Manager MT5 Connection Service
    /// Manages multiple MT5 connections for different managers/tenants
    /// Provides tenant-isolated MT5 access
    /// </summary>
    public class MultiManagerConnectionService : IMT5ConnectionService, IDisposable
    {
        private readonly ILogger<MultiManagerConnectionService> _logger;
        private readonly ITenantManagerService _tenantManager;
        private readonly ConcurrentDictionary<string, MT5ConnectionService> _managerConnections;
        private readonly ConcurrentDictionary<string, ManagerTenant> _activeTenants;
        private bool _disposed;

        public MultiManagerConnectionService(
            ILogger<MultiManagerConnectionService> logger,
            ITenantManagerService tenantManager)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _tenantManager = tenantManager ?? throw new ArgumentNullException(nameof(tenantManager));
            _managerConnections = new ConcurrentDictionary<string, MT5ConnectionService>();
            _activeTenants = new ConcurrentDictionary<string, ManagerTenant>();

            _logger.LogInformation("MultiManagerConnectionService initialized for multi-tenant MT5 access");
        }

        /// <summary>
        /// Get MT5 manager for a specific tenant
        /// </summary>
        public object? GetManagerForTenant(string tenantId)
        {
            if (string.IsNullOrEmpty(tenantId))
            {
                _logger.LogWarning("Empty tenant ID provided");
                return null;
            }

            // Check if we have an active connection for this tenant
            if (_managerConnections.TryGetValue(tenantId, out var connectionService))
            {
                return connectionService.Manager;
            }

            // Try to establish connection for this tenant
            var tenant = _tenantManager.GetTenant(tenantId);
            if (tenant == null || tenant.Status != TenantStatus.Active)
            {
                _logger.LogWarning("Tenant {TenantId} not found or not active", tenantId);
                return null;
            }

            // Get the primary MT5 manager for this tenant
            var primaryManager = tenant.MT5Managers?.FirstOrDefault(m => m.IsActive && m.Priority == 1);
            if (primaryManager == null)
            {
                _logger.LogWarning("No active primary MT5 manager found for tenant {TenantId}", tenantId);
                return null;
            }

            // Create connection config
            var config = new MT5ConnectionConfig
            {
                Server = primaryManager.Server,
                Login = primaryManager.Login,
                Password = primaryManager.Password,
                ManagerId = $"{tenantId}_{primaryManager.ManagerId}",
                EnableAutoReconnect = true,
                AutoReconnectInterval = 30
            };

            // Create and connect
            var loggerFactory = LoggerFactory.Create(builder => builder.AddConsole());
            var mt5Logger = loggerFactory.CreateLogger<MT5ConnectionService>();
            var mt5ConnectionService = new MT5ConnectionService(
                mt5Logger,
                Options.Create(new MT5WrapperConfig())); // We'll need to pass proper config

            var connectTask = mt5ConnectionService.ConnectAsync(config);
            connectTask.Wait(); // Synchronous for simplicity, could be async

            if (connectTask.Result.Success)
            {
                _managerConnections[tenantId] = mt5ConnectionService;
                _activeTenants[tenantId] = tenant;
                _logger.LogInformation("Successfully connected MT5 manager for tenant {TenantId}", tenantId);
                return mt5ConnectionService.Manager;
            }
            else
            {
                _logger.LogError("Failed to connect MT5 manager for tenant {TenantId}: {Message}",
                    tenantId, connectTask.Result.Message);
                return null;
            }
        }

        /// <summary>
        /// Legacy single-manager interface (for backward compatibility)
        /// Returns the first available manager or default manager
        /// </summary>
        public object? Manager
        {
            get
            {
                // First try to get from active tenants
                var tenantManager = GetManagerForTenant(_activeTenants.Keys.FirstOrDefault());
                if (tenantManager != null)
                {
                    return tenantManager;
                }

                // If no active tenants, check for default connection
                if (_managerConnections.TryGetValue("default", out var defaultConnection))
                {
                    return defaultConnection.Manager;
                }

                // Return first available connection as fallback
                return _managerConnections.Values.FirstOrDefault()?.Manager;
            }
        }

        /// <summary>
        /// Legacy interface - always returns true if any manager is connected
        /// </summary>
        public bool IsConnected => _managerConnections.Any(kvp => kvp.Value.IsConnected);

        /// <summary>
        /// Connect to MT5 for a specific tenant
        /// </summary>
        public async Task<ConnectionResult> ConnectAsync(MT5ConnectionConfig config)
        {
            // This is for backward compatibility - use tenant-specific connection
            var tenantId = ExtractTenantFromManagerId(config.ManagerId);

            // If tenant ID is the same as manager ID, treat as default connection
            if (tenantId == config.ManagerId || config.ManagerId == "default_manager")
            {
                tenantId = "default";
            }

            return await ConnectTenantAsync(tenantId, config);
        }

        /// <summary>
        /// Connect MT5 manager for a specific tenant
        /// </summary>
        public async Task<ConnectionResult> ConnectTenantAsync(string tenantId, MT5ConnectionConfig config)
        {
            try
            {
                if (_managerConnections.ContainsKey(tenantId))
                {
                    return ConnectionResult.CreateSuccess($"Already connected for tenant {tenantId}");
                }

                var loggerFactory = LoggerFactory.Create(builder => builder.AddConsole());
                var mt5Logger = loggerFactory.CreateLogger<MT5ConnectionService>();
                var mt5ConnectionService = new MT5ConnectionService(mt5Logger, Options.Create(new MT5WrapperConfig()));
                var result = await mt5ConnectionService.ConnectAsync(config);

                if (result.Success)
                {
                    _managerConnections[tenantId] = mt5ConnectionService;
                    _logger.LogInformation("Connected MT5 manager for tenant {TenantId}", tenantId);
                }

                return result;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error connecting MT5 manager for tenant {TenantId}", tenantId);
                return ConnectionResult.CreateFailure(MTRetCode.MT_RET_ERROR, ex.Message);
            }
        }

        /// <summary>
        /// Disconnect all MT5 managers
        /// </summary>
        public async Task<ConnectionResult> DisconnectAsync()
        {
            var results = new List<ConnectionResult>();

            foreach (var kvp in _managerConnections)
            {
                try
                {
                    var result = await kvp.Value.DisconnectAsync();
                    results.Add(result);
                    _logger.LogInformation("Disconnected MT5 manager for tenant {TenantId}", kvp.Key);
                }
                catch (Exception ex)
                {
                    _logger.LogError(ex, "Error disconnecting MT5 manager for tenant {TenantId}", kvp.Key);
                    results.Add(ConnectionResult.CreateFailure(MTRetCode.MT_RET_ERROR, ex.Message));
                }
            }

            _managerConnections.Clear();
            _activeTenants.Clear();

            var successCount = results.Count(r => r.Success);
            return ConnectionResult.CreateSuccess($"Disconnected {successCount}/{results.Count} managers");
        }

        /// <summary>
        /// Test connection for a specific tenant
        /// </summary>
        public async Task<ConnectionTestResult> TestConnectionAsync(string tenantId = null)
        {
            if (string.IsNullOrEmpty(tenantId))
            {
                tenantId = _activeTenants.Keys.FirstOrDefault();
            }

            if (string.IsNullOrEmpty(tenantId) || !_managerConnections.TryGetValue(tenantId, out var connection))
            {
                return ConnectionTestResult.CreateFailure("No active connection found", 0);
            }

            return await connection.TestConnectionAsync();
        }

        /// <summary>
        /// Legacy test connection method
        /// </summary>
        public async Task<ConnectionTestResult> TestConnectionAsync()
        {
            return await TestConnectionAsync(null);
        }

        /// <summary>
        /// Get connection stats for all managers
        /// </summary>
        public ConnectionStats GetConnectionStats()
        {
            var allStats = _managerConnections.Values.Select(c => c.GetConnectionStats()).ToList();

            if (!allStats.Any())
            {
                return new ConnectionStats();
            }

            // Aggregate stats from all connections
            return new ConnectionStats
            {
                ConnectionState = allStats.All(s => s.ConnectionState == "Connected") ? "Connected" : "Partial",
                TotalConnectionAttempts = allStats.Sum(s => s.TotalConnectionAttempts),
                SuccessfulConnections = allStats.Sum(s => s.SuccessfulConnections),
                FailedConnections = allStats.Sum(s => s.FailedConnections),
                UptimeSeconds = allStats.Min(s => s.UptimeSeconds), // Use minimum uptime
                LastConnectionAttempt = allStats.Max(s => s.LastConnectionAttempt),
                LastSuccessfulConnection = allStats.Max(s => s.LastSuccessfulConnection)
            };
        }

        /// <summary>
        /// Get list of active tenants with MT5 connections
        /// </summary>
        public string[] GetActiveTenants()
        {
            return _activeTenants.Keys.ToArray();
        }

        /// <summary>
        /// Check if a specific tenant has an active MT5 connection
        /// </summary>
        public bool IsTenantConnected(string tenantId)
        {
            return _managerConnections.TryGetValue(tenantId, out var connection) && connection.IsConnected;
        }

        private string ExtractTenantFromManagerId(string managerId)
        {
            // Extract tenant ID from manager ID (format: tenantId_managerId)
            var parts = managerId.Split('_');
            return parts.Length > 0 ? parts[0] : managerId;
        }

        public void Dispose()
        {
            if (_disposed) return;

            try
            {
                foreach (var connection in _managerConnections.Values)
                {
                    connection.Dispose();
                }
                _managerConnections.Clear();
                _activeTenants.Clear();
                _disposed = true;

                _logger.LogInformation("MultiManagerConnectionService disposed");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing MultiManagerConnectionService");
            }
        }
    }
}