using System;
using System.Collections.Generic;

namespace MT5Wrapper.WebSocket.DTOs
{
    /// <summary>
    /// 📊 Position data transfer object
    /// Represents a trading position with all relevant information
    /// </summary>
    public class PositionDto
    {
        /// <summary>
        /// Position ticket number
        /// </summary>
        public ulong PositionId { get; set; }

        /// <summary>
        /// Client login
        /// </summary>
        public ulong ClientLogin { get; set; }

        /// <summary>
        /// Trading symbol (e.g., "EURUSD")
        /// </summary>
        public string Symbol { get; set; } = string.Empty;

        /// <summary>
        /// Position type (Buy/Sell)
        /// </summary>
        public PositionType Type { get; set; }

        /// <summary>
        /// Position volume
        /// </summary>
        public ulong Volume { get; set; }

        /// <summary>
        /// Position open price
        /// </summary>
        public double PriceOpen { get; set; }

        /// <summary>
        /// Current price
        /// </summary>
        public double PriceCurrent { get; set; }

        /// <summary>
        /// Stop Loss price
        /// </summary>
        public double? StopLoss { get; set; }

        /// <summary>
        /// Take Profit price
        /// </summary>
        public double? TakeProfit { get; set; }

        /// <summary>
        /// Floating profit
        /// </summary>
        public double Profit { get; set; }

        /// <summary>
        /// Accumulated swaps
        /// </summary>
        public double Swap { get; set; }

        /// <summary>
        /// Position create time
        /// </summary>
        public DateTime TimeCreate { get; set; }

        /// <summary>
        /// Position last update time
        /// </summary>
        public DateTime TimeUpdate { get; set; }

        /// <summary>
        /// Expert advisor comment
        /// </summary>
        public string Comment { get; set; } = string.Empty;

        /// <summary>
        /// Expert advisor ID
        /// </summary>
        public ulong Magic { get; set; }

        /// <summary>
        /// Price SL (duplicate for compatibility)
        /// </summary>
        public double PriceSL => StopLoss ?? 0;

        /// <summary>
        /// Price TP (duplicate for compatibility)
        /// </summary>
        public double PriceTP => TakeProfit ?? 0;
    }

    /// <summary>
    /// 📊 Position Update DTO for WebSocket streaming
    /// </summary>
    public class PositionUpdateDto
    {
        /// <summary>
        /// Update type (add, update, delete, clean, sync)
        /// </summary>
        public string UpdateType { get; set; } = string.Empty;

        /// <summary>
        /// Client login
        /// </summary>
        public ulong ClientLogin { get; set; }

        /// <summary>
        /// Position data (null for delete/clean operations)
        /// </summary>
        public PositionDto? Position { get; set; }

        /// <summary>
        /// Server timestamp
        /// </summary>
        public long ServerTimestamp { get; set; }
    }

    /// <summary>
    /// 📡 Position subscription request
    /// </summary>
    public class PositionSubscriptionRequest
    {
        /// <summary>
        /// Client login to subscribe to
        /// </summary>
        public ulong ClientLogin { get; set; }
    }

    /// <summary>
    /// 📊 Position subscription response
    /// </summary>
    public class PositionSubscriptionResponse
    {
        /// <summary>
        /// Success status
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// Response message
        /// </summary>
        public string Message { get; set; } = string.Empty;

        /// <summary>
        /// Client login
        /// </summary>
        public ulong ClientLogin { get; set; }

        /// <summary>
        /// Unique subscription ID
        /// </summary>
        public string SubscriptionId { get; set; } = string.Empty;

        /// <summary>
        /// Timestamp of subscription
        /// </summary>
        public long Timestamp { get; set; }
    }

    /// <summary>
    /// 📊 Position Service Statistics
    /// </summary>
    public class PositionServiceStats
    {
        /// <summary>
        /// Total position updates processed
        /// </summary>
        public long TotalUpdates { get; set; }

        /// <summary>
        /// Number of active subscriptions
        /// </summary>
        public int ActiveSubscriptions { get; set; }

        /// <summary>
        /// Total subscriptions created
        /// </summary>
        public long TotalSubscriptions { get; set; }

        /// <summary>
        /// Total unsubscriptions
        /// </summary>
        public long TotalUnsubscriptions { get; set; }

        /// <summary>
        /// Timestamp of last update (milliseconds since epoch)
        /// </summary>
        public long LastUpdateTimestamp { get; set; }

        /// <summary>
        /// Service uptime in seconds
        /// </summary>
        public long UptimeSeconds { get; set; }
    }

    /// <summary>
    /// Position types enumeration
    /// </summary>
    public enum PositionType
    {
        Buy = 0,
        Sell = 1
    }

    /// <summary>
    /// Position update types for WebSocket events
    /// </summary>
    public enum PositionUpdateType
    {
        Add = 0,      // Position opened
        Update = 1,   // Position modified
        Delete = 2,   // Position closed
        Clean = 3,    // All positions cleaned
        Sync = 4      // Synchronization
    }
}