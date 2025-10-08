using System.Collections.Generic;
using System.Threading.Tasks;
using MT5Wrapper.WebSocket.DTOs;

namespace MT5Wrapper.WebSocket.Interfaces
{
    /// <summary>
    /// 📊 Position Service Interface - Real-time position data streaming
    /// Provides live position updates via WebSocket for trading applications
    /// </summary>
    public interface IPositionService
    {
        /// <summary>
        /// 📈 Subscribe to real-time position updates for a client
        /// </summary>
        Task<PositionSubscriptionResponse> SubscribeToPositionsAsync(ulong clientLogin);

        /// <summary>
        /// 📉 Unsubscribe from position updates for a client
        /// </summary>
        Task<PositionSubscriptionResponse> UnsubscribeFromPositionsAsync(ulong clientLogin);

        /// <summary>
        /// 📊 Get current positions snapshot for a client
        /// </summary>
        Task<IEnumerable<PositionDto>> GetPositionsSnapshotAsync(ulong clientLogin);

        /// <summary>
        /// 🔍 Check if subscribed to positions for a client
        /// </summary>
        bool IsSubscribedToPositions(ulong clientLogin);

        /// <summary>
        /// 📋 Get all subscribed client logins
        /// </summary>
        ulong[] GetSubscribedClients();

        /// <summary>
        /// 📡 Broadcast position update to WebSocket clients
        /// </summary>
        Task BroadcastPositionUpdateAsync(PositionUpdateDto update);

        /// <summary>
        /// 📈 Get service statistics
        /// </summary>
        PositionServiceStats GetServiceStats();

        /// <summary>
        /// 🔗 Register WebSocket client connection for position updates
        /// </summary>
        void RegisterWebSocketClient(ulong clientLogin, string connectionId);

        /// <summary>
        /// 🔌 Unregister WebSocket client connection from position updates
        /// </summary>
        void UnregisterWebSocketClient(ulong clientLogin, string connectionId);
    }
}