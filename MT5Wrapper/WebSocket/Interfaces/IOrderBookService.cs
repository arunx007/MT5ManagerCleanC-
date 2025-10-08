using System.Threading.Tasks;
using MT5Wrapper.WebSocket.DTOs;

namespace MT5Wrapper.WebSocket.Interfaces
{
    /// <summary>
    /// 📊 Order Book Service Interface
    /// Provides real-time order book (market depth) data streaming
    /// </summary>
    public interface IOrderBookService
    {
        /// <summary>
        /// Subscribe to real-time order book updates for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol (e.g., "EURUSD")</param>
        /// <param name="maxDepth">Maximum depth to return (default: 20)</param>
        /// <returns>Subscription result</returns>
        Task<OrderBookSubscriptionResponse> SubscribeToOrderBookAsync(string symbol, int maxDepth = 20);

        /// <summary>
        /// Unsubscribe from order book updates for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol</param>
        /// <returns>Unsubscription result</returns>
        Task<OrderBookSubscriptionResponse> UnsubscribeFromOrderBookAsync(string symbol);

        /// <summary>
        /// Get current order book snapshot for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol</param>
        /// <param name="maxDepth">Maximum depth to return</param>
        /// <returns>Current order book data</returns>
        Task<OrderBookDto?> GetOrderBookSnapshotAsync(string symbol, int maxDepth = 20);

        /// <summary>
        /// Check if order book is subscribed for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol</param>
        /// <returns>True if subscribed</returns>
        bool IsSubscribedToOrderBook(string symbol);

        /// <summary>
        /// Get list of all subscribed symbols
        /// </summary>
        /// <returns>Array of subscribed symbols</returns>
        string[] GetSubscribedSymbols();

        /// <summary>
        /// Broadcast order book update to all subscribers
        /// </summary>
        /// <param name="orderBook">Updated order book data</param>
        Task BroadcastOrderBookUpdateAsync(OrderBookDto orderBook);

        /// <summary>
        /// Get service statistics
        /// </summary>
        /// <returns>Service statistics</returns>
        OrderBookServiceStats GetServiceStats();
    }

    /// <summary>
    /// 📈 Order Book Service Statistics
    /// </summary>
    public class OrderBookServiceStats
    {
        /// <summary>
        /// Number of active subscriptions
        /// </summary>
        public int ActiveSubscriptions { get; set; }

        /// <summary>
        /// Total subscription requests
        /// </summary>
        public long TotalSubscriptions { get; set; }

        /// <summary>
        /// Total unsubscription requests
        /// </summary>
        public long TotalUnsubscriptions { get; set; }

        /// <summary>
        /// Total order book updates broadcasted
        /// </summary>
        public long TotalUpdates { get; set; }

        /// <summary>
        /// Service uptime in seconds
        /// </summary>
        public long UptimeSeconds { get; set; }

        /// <summary>
        /// Memory usage in bytes
        /// </summary>
        public long MemoryUsage { get; set; }

        /// <summary>
        /// Last update timestamp
        /// </summary>
        public long LastUpdateTimestamp { get; set; }
    }
}