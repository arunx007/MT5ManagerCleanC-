using System;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using MT5Wrapper.WebSocket.Interfaces;
using MT5Wrapper.WebSocket.DTOs;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// 📊 Order Book Controller - REST API endpoints for market depth data
    /// Provides order book snapshots and subscription management via HTTP
    /// </summary>
    [ApiController]
    [Route("api/orderbook")]
    [Authorize]
    public class OrderBookController : ControllerBase
    {
        private readonly IOrderBookService _orderBookService;

        public OrderBookController(IOrderBookService orderBookService)
        {
            _orderBookService = orderBookService;
        }

        /// <summary>
        /// 📊 Get current order book snapshot for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol (e.g., "EURUSD")</param>
        /// <param name="maxDepth">Maximum depth to return (default: 20)</param>
        /// <returns>Current order book data</returns>
        [HttpGet("{symbol}")]
        [ProducesResponseType(typeof(OrderBookDto), 200)]
        [ProducesResponseType(404)]
        public async Task<IActionResult> GetOrderBook(string symbol, [FromQuery] int maxDepth = 20)
        {
            if (string.IsNullOrWhiteSpace(symbol))
            {
                return BadRequest(new { error = "Symbol is required" });
            }

            var orderBook = await _orderBookService.GetOrderBookSnapshotAsync(symbol, maxDepth);
            if (orderBook == null)
            {
                // Log that we're returning NotFound
                Console.WriteLine($"OrderBookController: No order book data for symbol {symbol}, returning 404");
                return NotFound(new { error = $"Order book data not available for symbol {symbol}" });
            }

            Console.WriteLine($"OrderBookController: Returning order book for {symbol} with {orderBook.BidCount} bids and {orderBook.AskCount} asks");
            return Ok(orderBook);
        }

        /// <summary>
        /// 📈 Subscribe to order book updates for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol to subscribe to</param>
        /// <param name="maxDepth">Maximum depth for updates (default: 20)</param>
        /// <returns>Subscription result</returns>
        [HttpPost("subscribe/{symbol}")]
        [ProducesResponseType(typeof(OrderBookSubscriptionResponse), 200)]
        public async Task<IActionResult> SubscribeToOrderBook(string symbol, [FromQuery] int maxDepth = 20)
        {
            if (string.IsNullOrWhiteSpace(symbol))
            {
                return BadRequest(new { error = "Symbol is required" });
            }

            var result = await _orderBookService.SubscribeToOrderBookAsync(symbol, maxDepth);
            if (result.Success)
            {
                return Ok(result);
            }

            return BadRequest(result);
        }

        /// <summary>
        /// 📉 Unsubscribe from order book updates for a symbol
        /// </summary>
        /// <param name="symbol">Trading symbol to unsubscribe from</param>
        /// <returns>Unsubscription result</returns>
        [HttpPost("unsubscribe/{symbol}")]
        [ProducesResponseType(typeof(OrderBookSubscriptionResponse), 200)]
        public async Task<IActionResult> UnsubscribeFromOrderBook(string symbol)
        {
            if (string.IsNullOrWhiteSpace(symbol))
            {
                return BadRequest(new { error = "Symbol is required" });
            }

            var result = await _orderBookService.UnsubscribeFromOrderBookAsync(symbol);
            if (result.Success)
            {
                return Ok(result);
            }

            return BadRequest(result);
        }

        /// <summary>
        /// 📋 Get list of all subscribed symbols
        /// </summary>
        /// <returns>Array of subscribed symbols</returns>
        [HttpGet("subscriptions")]
        [ProducesResponseType(typeof(string[]), 200)]
        public IActionResult GetSubscribedSymbols()
        {
            var symbols = _orderBookService.GetSubscribedSymbols();
            return Ok(new { subscribedSymbols = symbols, count = symbols.Length });
        }

        /// <summary>
        /// 🔍 Check if a symbol is subscribed
        /// </summary>
        /// <param name="symbol">Trading symbol to check</param>
        /// <returns>Subscription status</returns>
        [HttpGet("subscriptions/{symbol}")]
        [ProducesResponseType(200)]
        public IActionResult CheckSubscription(string symbol)
        {
            if (string.IsNullOrWhiteSpace(symbol))
            {
                return BadRequest(new { error = "Symbol is required" });
            }

            var isSubscribed = _orderBookService.IsSubscribedToOrderBook(symbol);
            return Ok(new
            {
                symbol = symbol,
                isSubscribed = isSubscribed,
                status = isSubscribed ? "subscribed" : "not_subscribed"
            });
        }

        /// <summary>
        /// 📊 Get order book service statistics
        /// </summary>
        /// <returns>Service statistics</returns>
        [HttpGet("stats")]
        [ProducesResponseType(typeof(OrderBookServiceStats), 200)]
        public IActionResult GetServiceStats()
        {
            var stats = _orderBookService.GetServiceStats();
            return Ok(stats);
        }

        /// <summary>
        /// 📈 Get order book status for all symbols
        /// </summary>
        /// <returns>Comprehensive order book status</returns>
        [HttpGet("status")]
        [ProducesResponseType(200)]
        public IActionResult GetOrderBookStatus()
        {
            var stats = _orderBookService.GetServiceStats();
            var symbols = _orderBookService.GetSubscribedSymbols();

            return Ok(new
            {
                service = new
                {
                    activeSubscriptions = stats.ActiveSubscriptions,
                    totalSubscriptions = stats.TotalSubscriptions,
                    totalUnsubscriptions = stats.TotalUnsubscriptions,
                    totalUpdates = stats.TotalUpdates,
                    uptimeSeconds = stats.UptimeSeconds,
                    lastUpdateTimestamp = stats.LastUpdateTimestamp
                },
                subscriptions = new
                {
                    symbols = symbols,
                    count = symbols.Length
                },
                timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });
        }
    }
}