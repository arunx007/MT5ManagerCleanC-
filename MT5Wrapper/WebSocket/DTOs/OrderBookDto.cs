using System;
using System.Collections.Generic;

namespace MT5Wrapper.WebSocket.DTOs
{
    /// <summary>
    /// 📊 Order Book (Market Depth) data transfer object
    /// Represents real-time bid/ask order book for a symbol
    /// </summary>
    public class OrderBookDto
    {
        /// <summary>
        /// Trading symbol (e.g., "EURUSD")
        /// </summary>
        public string Symbol { get; set; } = string.Empty;

        /// <summary>
        /// Bid (buy) orders - sorted by price descending
        /// </summary>
        public List<OrderBookEntry> Bids { get; set; } = new List<OrderBookEntry>();

        /// <summary>
        /// Ask (sell) orders - sorted by price ascending
        /// </summary>
        public List<OrderBookEntry> Asks { get; set; } = new List<OrderBookEntry>();

        /// <summary>
        /// Timestamp of the order book snapshot (milliseconds since epoch)
        /// </summary>
        public long Timestamp { get; set; }

        /// <summary>
        /// Server timestamp when data was received
        /// </summary>
        public long ServerTimestamp { get; set; }

        /// <summary>
        /// Type of update (snapshot, update, delete)
        /// </summary>
        public string UpdateType { get; set; } = "snapshot";

        /// <summary>
        /// Total number of bid orders
        /// </summary>
        public int BidCount => Bids.Count;

        /// <summary>
        /// Total number of ask orders
        /// </summary>
        public int AskCount => Asks.Count;

        /// <summary>
        /// Best bid price (highest bid)
        /// </summary>
        public double BestBid => Bids.Count > 0 ? Bids[0].Price : 0;

        /// <summary>
        /// Best ask price (lowest ask)
        /// </summary>
        public double BestAsk => Asks.Count > 0 ? Asks[0].Price : 0;

        /// <summary>
        /// Spread (difference between best ask and best bid)
        /// </summary>
        public double Spread => BestAsk > 0 && BestBid > 0 ? BestAsk - BestBid : 0;
    }

    /// <summary>
    /// 📈 Individual order book entry (price level)
    /// </summary>
    public class OrderBookEntry
    {
        /// <summary>
        /// Price level
        /// </summary>
        public double Price { get; set; }

        /// <summary>
        /// Volume at this price level
        /// </summary>
        public double Volume { get; set; }

        /// <summary>
        /// Number of orders at this price level
        /// </summary>
        public int Orders { get; set; }

        /// <summary>
        /// Type of entry (bid or ask)
        /// </summary>
        public string Type { get; set; } = string.Empty;
    }

    /// <summary>
    /// 📡 Order Book subscription request
    /// </summary>
    public class OrderBookSubscriptionRequest
    {
        /// <summary>
        /// Trading symbol to subscribe to
        /// </summary>
        public string Symbol { get; set; } = string.Empty;

        /// <summary>
        /// Maximum depth (number of price levels to return)
        /// </summary>
        public int MaxDepth { get; set; } = 20;

        /// <summary>
        /// Update frequency in milliseconds (0 = real-time)
        /// </summary>
        public int UpdateFrequency { get; set; } = 0;
    }

    /// <summary>
    /// 📊 Order Book subscription response
    /// </summary>
    public class OrderBookSubscriptionResponse
    {
        /// <summary>
        /// Success status
        /// </summary>
        public bool Success { get; set; }

        /// <summary>
        /// Subscription message
        /// </summary>
        public string Message { get; set; } = string.Empty;

        /// <summary>
        /// Subscribed symbol
        /// </summary>
        public string Symbol { get; set; } = string.Empty;

        /// <summary>
        /// Unique subscription ID
        /// </summary>
        public string SubscriptionId { get; set; } = string.Empty;

        /// <summary>
        /// Maximum depth configured
        /// </summary>
        public int MaxDepth { get; set; }

        /// <summary>
        /// Timestamp of subscription
        /// </summary>
        public long Timestamp { get; set; }
    }
}