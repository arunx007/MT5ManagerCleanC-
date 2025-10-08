using System.Collections.Generic;
using System.Threading.Tasks;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using MT5Wrapper.TickData.DTOs;

namespace MT5Wrapper.TickData.Interfaces
{
    /// <summary>
    /// Tick data service interface - handles real-time market data
    /// </summary>
    public interface ITickDataService
    {
        /// <summary>
        /// Get current tick data for a symbol
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="symbol">Trading symbol (e.g., EURUSD)</param>
        /// <returns>Current tick data</returns>
        Task<TickDataDto?> GetCurrentTickAsync(string tenantId, string symbol);

        /// <summary>
        /// Get last N ticks for a symbol
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="symbol">Trading symbol</param>
        /// <param name="count">Number of ticks to retrieve</param>
        /// <returns>Historical tick data</returns>
        Task<IEnumerable<TickDataDto>> GetTickHistoryAsync(string tenantId, string symbol, int count);

        /// <summary>
        /// Subscribe to real-time tick updates for a symbol
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="symbol">Trading symbol</param>
        /// <param name="callback">Callback function for tick updates</param>
        /// <returns>Subscription ID</returns>
        Task<string> SubscribeToTickUpdatesAsync(string tenantId, string symbol, Action<TickDataDto> callback);

        /// <summary>
        /// Unsubscribe from tick updates
        /// </summary>
        /// <param name="subscriptionId">Subscription ID</param>
        /// <returns>Unsubscription success</returns>
        Task<bool> UnsubscribeFromTickUpdatesAsync(string subscriptionId);

        /// <summary>
        /// Get available symbols for tick data
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <returns>List of available symbols</returns>
        Task<IEnumerable<string>> GetAvailableSymbolsAsync(string tenantId);

        /// <summary>
        /// Check if tick data is available for a symbol
        /// </summary>
        /// <param name="tenantId">Tenant identifier</param>
        /// <param name="symbol">Trading symbol</param>
        /// <returns>Availability status</returns>
        Task<bool> IsSymbolAvailableAsync(string tenantId, string symbol);
    }
}