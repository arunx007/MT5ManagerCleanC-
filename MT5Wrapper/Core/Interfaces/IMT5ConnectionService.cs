using System;
using System.Threading.Tasks;
using MT5Wrapper.Core.Models;
using MetaQuotes.MT5CommonAPI;

namespace MT5Wrapper.Core.Interfaces
{
    /// <summary>
    /// Core MT5 connection service interface - isolated from other services
    /// </summary>
    public interface IMT5ConnectionService : IDisposable
    {
        /// <summary>
        /// Gets whether the service is currently connected to MT5 Manager
        /// </summary>
        bool IsConnected { get; }

        /// <summary>
        /// Gets the current manager instance (null if not connected)
        /// </summary>
        dynamic? Manager { get; }

        /// <summary>
        /// Connect to MT5 Manager with specified credentials
        /// </summary>
        /// <param name="config">Connection configuration</param>
        /// <returns>Connection result</returns>
        Task<ConnectionResult> ConnectAsync(MT5ConnectionConfig config);

        /// <summary>
        /// Disconnect from MT5 Manager
        /// </summary>
        /// <returns>Disconnection result</returns>
        Task<ConnectionResult> DisconnectAsync();

        /// <summary>
        /// Test the connection and return status
        /// </summary>
        /// <returns>Connection test result</returns>
        Task<ConnectionTestResult> TestConnectionAsync();

        /// <summary>
        /// Get connection statistics
        /// </summary>
        /// <returns>Connection statistics</returns>
        ConnectionStats GetConnectionStats();
    }
}