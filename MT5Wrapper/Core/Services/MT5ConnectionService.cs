using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Diagnostics;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Core.Models;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;

namespace MT5Wrapper.Core.Services
{
    /// <summary>
    /// Isolated MT5 connection service - handles all MT5 Manager connections
    /// This service is completely isolated from other services to prevent cascading failures
    /// </summary>
    public class MT5ConnectionService : IMT5ConnectionService
    {
        private readonly ILogger<MT5ConnectionService> _logger;
        private readonly MT5WrapperConfig _config;
        private dynamic? _manager;
        private bool _isConnected;
        private bool _disposed;
        private readonly ConnectionStats _stats;
        private readonly object _connectionLock = new object();
        private DateTime _connectionStartTime;
        private System.Threading.Timer? _healthCheckTimer;

        public MT5ConnectionService(
            ILogger<MT5ConnectionService> logger,
            IOptions<MT5WrapperConfig> config)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _config = config?.Value ?? throw new ArgumentNullException(nameof(config));
            _stats = new ConnectionStats();
        }

        /// <inheritdoc/>
        public bool IsConnected => _isConnected;

        /// <inheritdoc/>
        public object? Manager
        {
            get
            {
                // Log the current state
                Console.WriteLine($"MT5ConnectionService.Manager getter called. _isConnected: {_isConnected}, _manager is null: {_manager == null}");
                return _manager;
            }
        }

        /// <inheritdoc/>
        public async Task<ConnectionResult> ConnectAsync(MT5ConnectionConfig config)
        {
            if (config == null) throw new ArgumentNullException(nameof(config));

            var stopwatch = Stopwatch.StartNew();

            try
            {
                _stats.TotalConnectionAttempts++;

                // Validate configuration
                var validationResult = ValidateConnectionConfig(config);
                if (!validationResult.Success)
                {
                    _stats.FailedConnections++;
                    return validationResult;
                }

                lock (_connectionLock)
                {
                    if (_isConnected && _manager != null)
                    {
                        _logger.LogInformation("Already connected to MT5 Manager for {ManagerId}", config.ManagerId);
                        return ConnectionResult.CreateSuccess("Already connected", stopwatch.ElapsedMilliseconds);
                    }

                    // Disconnect existing connection if any
                    if (_manager != null)
                    {
                        DisconnectInternal();
                    }

                    // Initialize MT5 API Factory
                    _logger.LogInformation("Initializing MT5 Manager API Factory for {ManagerId}", config.ManagerId);

                    // Use the correct initialization pattern from SDK examples
                    var initResult = SMTManagerAPIFactory.Initialize(null);
                    if (initResult != MTRetCode.MT_RET_OK)
                    {
                        _stats.FailedConnections++;
                        return ConnectionResult.CreateFailure(
                            initResult,
                            $"Failed to initialize MT5 Manager API Factory: {initResult}",
                            stopwatch.ElapsedMilliseconds);
                    }

                    // Check API version
                    uint version = 0;
                    var versionResult = SMTManagerAPIFactory.GetVersion(out version);
                    if (versionResult != MTRetCode.MT_RET_OK)
                    {
                        _stats.FailedConnections++;
                        SMTManagerAPIFactory.Shutdown();
                        return ConnectionResult.CreateFailure(
                            versionResult,
                            $"Failed to get MT5 Manager API version: {versionResult}",
                            stopwatch.ElapsedMilliseconds);
                    }

                    if (version != SMTManagerAPIFactory.ManagerAPIVersion)
                    {
                        _stats.FailedConnections++;
                        SMTManagerAPIFactory.Shutdown();
                        return ConnectionResult.CreateFailure(
                            MTRetCode.MT_RET_ERR_DATA,
                            $"Wrong Manager API version, version {SMTManagerAPIFactory.ManagerAPIVersion} required, got {version}",
                            stopwatch.ElapsedMilliseconds);
                    }

                    _logger.LogInformation("MT5 Manager API version {Version} loaded successfully", version);

                    // Create manager instance
                    MTRetCode createResult;
                    _manager = SMTManagerAPIFactory.CreateManager(SMTManagerAPIFactory.ManagerAPIVersion, out createResult);
                    if (createResult != MTRetCode.MT_RET_OK || _manager == null)
                    {
                        _stats.FailedConnections++;
                        SMTManagerAPIFactory.Shutdown();
                        return ConnectionResult.CreateFailure(
                            createResult,
                            $"Failed to create MT5 Manager interface: {createResult}",
                            stopwatch.ElapsedMilliseconds);
                    }

                    // Parse login to ulong
                    if (!ulong.TryParse(config.Login, out ulong loginId))
                    {
                        _stats.FailedConnections++;
                        Cleanup();
                        return ConnectionResult.CreateFailure(
                            MTRetCode.MT_RET_ERR_PARAMS,
                            $"Invalid login format: '{config.Login}'. Login must be numeric.",
                            stopwatch.ElapsedMilliseconds);
                    }

                    _logger.LogInformation("Connecting to MT5 server {Server} with login {Login} for manager {ManagerId}",
                        config.Server, loginId, config.ManagerId);

                    // Connect to MT5 Manager (SDK pattern)
                    // Use PUMP_MODE_FULL like SDK examples for complete data access
                    var pumpMode = CIMTManagerAPI.EnPumpModes.PUMP_MODE_FULL;
                    var connectResult = _manager.Connect(
                        config.Server,
                        loginId,
                        config.Password,
                        "", // password_cert (empty for manager login like SDK)
                        pumpMode, // pump_mode - full data pumping like SDK examples
                        (uint)config.ConnectionTimeout);

                    if (connectResult != MTRetCode.MT_RET_OK)
                    {
                        _stats.FailedConnections++;
                        Cleanup();
                        return ConnectionResult.CreateFailure(
                            connectResult,
                            $"Failed to connect to MT5 server: {connectResult}. Check server address, credentials, and network connectivity.",
                            stopwatch.ElapsedMilliseconds);
                    }

                    // Connection successful
                    _isConnected = true;
                    _connectionStartTime = DateTime.UtcNow;
                    _stats.SuccessfulConnections++;
                    _stats.LastSuccessfulConnection = DateTime.UtcNow;
                    _stats.ConnectionState = "Connected";

                    // Start health check timer if auto-reconnect is enabled
                    if (config.EnableAutoReconnect)
                    {
                        StartHealthCheckTimer(config.AutoReconnectInterval);
                    }

                    _logger.LogInformation("Successfully connected to MT5 Manager {ManagerId} in {ElapsedMs}ms",
                        config.ManagerId, stopwatch.ElapsedMilliseconds);

                    return ConnectionResult.CreateSuccess(
                        $"Connected to MT5 Manager {config.ManagerId} successfully",
                        stopwatch.ElapsedMilliseconds);
                }
            }
            catch (Exception ex)
            {
                _stats.FailedConnections++;
                _logger.LogError(ex, "Exception occurred while connecting to MT5 Manager {ManagerId}", config.ManagerId);

                Cleanup();
                return ConnectionResult.CreateFailure(
                    MTRetCode.MT_RET_ERROR,
                    $"Exception during connection: {ex.Message}",
                    stopwatch.ElapsedMilliseconds);
            }
            finally
            {
                stopwatch.Stop();
                _stats.LastConnectionAttempt = DateTime.UtcNow;
            }
        }

        /// <inheritdoc/>
        public async Task<ConnectionResult> DisconnectAsync()
        {
            var stopwatch = Stopwatch.StartNew();

            try
            {
                lock (_connectionLock)
                {
                    if (!_isConnected || _manager == null)
                    {
                        return ConnectionResult.CreateSuccess("Already disconnected", stopwatch.ElapsedMilliseconds);
                    }

                    DisconnectInternal();
                    return ConnectionResult.CreateSuccess("Disconnected successfully", stopwatch.ElapsedMilliseconds);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception occurred while disconnecting from MT5 Manager");
                return ConnectionResult.CreateFailure(
                    MTRetCode.MT_RET_ERROR,
                    $"Exception during disconnection: {ex.Message}",
                    stopwatch.ElapsedMilliseconds);
            }
            finally
            {
                stopwatch.Stop();
            }
        }

        /// <inheritdoc/>
        public async Task<ConnectionTestResult> TestConnectionAsync()
        {
            var stopwatch = Stopwatch.StartNew();

            try
            {
                if (!_isConnected || _manager == null)
                {
                    return ConnectionTestResult.CreateFailure("Not connected to MT5 Manager", 0);
                }

                // Test connection by getting server info or total users
                var totalUsers = _manager.UserTotal();

                return ConnectionTestResult.CreateSuccess(
                    stopwatch.ElapsedMilliseconds,
                    $"MT5 Manager API accessible. Total users: {totalUsers}");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Connection test failed");
                return ConnectionTestResult.CreateFailure(
                    $"Connection test failed: {ex.Message}",
                    stopwatch.ElapsedMilliseconds);
            }
            finally
            {
                stopwatch.Stop();
            }
        }

        /// <inheritdoc/>
        public ConnectionStats GetConnectionStats()
        {
            // Update uptime if connected
            if (_isConnected && _connectionStartTime != default)
            {
                _stats.UptimeSeconds = (long)(DateTime.UtcNow - _connectionStartTime).TotalSeconds;
            }

            return _stats;
        }

        private ConnectionResult ValidateConnectionConfig(MT5ConnectionConfig config)
        {
            if (string.IsNullOrWhiteSpace(config.Server))
                return ConnectionResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, "Server address is required");

            if (string.IsNullOrWhiteSpace(config.Login))
                return ConnectionResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, "Login is required");

            if (string.IsNullOrWhiteSpace(config.Password))
                return ConnectionResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, "Password is required");

            if (string.IsNullOrWhiteSpace(config.ManagerId))
                return ConnectionResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, "Manager ID is required");

            return ConnectionResult.CreateSuccess("Configuration is valid");
        }

        private void DisconnectInternal()
        {
            try
            {
                if (_manager != null)
                {
                    _manager.Disconnect();
                    _manager.Release();
                    _manager = null;
                }

                SMTManagerAPIFactory.Shutdown();

                _isConnected = false;
                _stats.ConnectionState = "Disconnected";

                // Stop health check timer
                _healthCheckTimer?.Dispose();
                _healthCheckTimer = null;

                _logger.LogInformation("MT5 Manager disconnected and cleaned up");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error during internal disconnect");
            }
        }

        private void Cleanup()
        {
            try
            {
                if (_manager != null)
                {
                    _manager.Release();
                    _manager = null;
                }
                SMTManagerAPIFactory.Shutdown();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error during cleanup");
            }
        }

        private void StartHealthCheckTimer(int intervalSeconds)
        {
            _healthCheckTimer?.Dispose();
            _healthCheckTimer = new Timer(async (state) =>
            {
                try
                {
                    if (_isConnected && _manager != null)
                    {
                        var testResult = await TestConnectionAsync();
                        if (!testResult.IsSuccessful)
                        {
                            _logger.LogWarning("Health check failed: {ErrorMessage}", testResult.ErrorMessage);
                            // Could trigger reconnection logic here
                        }
                    }
                }
                catch (Exception ex)
                {
                    _logger.LogError(ex, "Error during health check");
                }
            }, null, TimeSpan.FromSeconds(intervalSeconds), TimeSpan.FromSeconds(intervalSeconds));
        }

        /// <inheritdoc/>
        public void Dispose()
        {
            if (_disposed) return;

            try
            {
                _healthCheckTimer?.Dispose();
                DisconnectInternal();
                _disposed = true;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing MT5ConnectionService");
            }
        }
    }
}