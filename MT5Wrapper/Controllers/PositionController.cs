using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using MT5Wrapper.WebSocket.Interfaces;
using MT5Wrapper.WebSocket.DTOs;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// 📊 Position Controller - REST API endpoints for position data
    /// Provides position snapshots and subscription management via HTTP
    /// </summary>
    [ApiController]
    [Route("api/positions")]
    // [Authorize] // Temporarily disabled for testing
    public class PositionController : ControllerBase
    {
        private readonly IPositionService _positionService;

        public PositionController(IPositionService positionService)
        {
            _positionService = positionService;
        }

        /// <summary>
        /// 📊 Get current positions snapshot for a client
        /// </summary>
        /// <param name="clientLogin">Client login to get positions for</param>
        /// <returns>Current position data</returns>
        [HttpGet("{clientLogin}")]
        [ProducesResponseType(typeof(IEnumerable<PositionDto>), 200)]
        [ProducesResponseType(404)]
        public async Task<IActionResult> GetPositions(ulong clientLogin)
        {
            if (clientLogin == 0)
            {
                return BadRequest(new { error = "Client login is required" });
            }

            var positions = await _positionService.GetPositionsSnapshotAsync(clientLogin);

            return Ok(new
            {
                clientLogin = clientLogin,
                positions = positions,
                count = positions.Count(),
                timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });
        }

        /// <summary>
        /// 📈 Subscribe to position updates for a client
        /// </summary>
        /// <param name="clientLogin">Client login to subscribe to</param>
        /// <returns>Subscription result</returns>
        [HttpPost("subscribe/{clientLogin}")]
        [ProducesResponseType(typeof(PositionSubscriptionResponse), 200)]
        public async Task<IActionResult> SubscribeToPositions(ulong clientLogin)
        {
            if (clientLogin == 0)
            {
                return BadRequest(new { error = "Client login is required" });
            }

            var result = await _positionService.SubscribeToPositionsAsync(clientLogin);
            if (result.Success)
            {
                return Ok(result);
            }

            return BadRequest(result);
        }

        /// <summary>
        /// 📉 Unsubscribe from position updates for a client
        /// </summary>
        /// <param name="clientLogin">Client login to unsubscribe from</param>
        /// <returns>Unsubscription result</returns>
        [HttpPost("unsubscribe/{clientLogin}")]
        [ProducesResponseType(typeof(PositionSubscriptionResponse), 200)]
        public async Task<IActionResult> UnsubscribeFromPositions(ulong clientLogin)
        {
            if (clientLogin == 0)
            {
                return BadRequest(new { error = "Client login is required" });
            }

            var result = await _positionService.UnsubscribeFromPositionsAsync(clientLogin);
            if (result.Success)
            {
                return Ok(result);
            }

            return BadRequest(result);
        }

        /// <summary>
        /// 📋 Get list of all subscribed clients
        /// </summary>
        /// <returns>Array of subscribed client logins</returns>
        [HttpGet("subscriptions")]
        [ProducesResponseType(typeof(ulong[]), 200)]
        public IActionResult GetSubscribedClients()
        {
            var clients = _positionService.GetSubscribedClients();
            return Ok(new
            {
                subscribedClients = clients,
                count = clients.Length,
                timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });
        }

        /// <summary>
        /// 🔍 Check if a client is subscribed to position updates
        /// </summary>
        /// <param name="clientLogin">Client login to check</param>
        /// <returns>Subscription status</returns>
        [HttpGet("subscriptions/{clientLogin}")]
        [ProducesResponseType(200)]
        public IActionResult CheckSubscription(ulong clientLogin)
        {
            if (clientLogin == 0)
            {
                return BadRequest(new { error = "Client login is required" });
            }

            var isSubscribed = _positionService.IsSubscribedToPositions(clientLogin);
            return Ok(new
            {
                clientLogin = clientLogin,
                isSubscribed = isSubscribed,
                status = isSubscribed ? "subscribed" : "not_subscribed",
                timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });
        }

        /// <summary>
        /// 📊 Get position service statistics
        /// </summary>
        /// <returns>Service statistics</returns>
        [HttpGet("stats")]
        [ProducesResponseType(typeof(PositionServiceStats), 200)]
        public IActionResult GetServiceStats()
        {
            var stats = _positionService.GetServiceStats();
            return Ok(stats);
        }

        /// <summary>
        /// 📈 Get position service status
        /// </summary>
        /// <returns>Comprehensive position service status</returns>
        [HttpGet("status")]
        [ProducesResponseType(200)]
        public IActionResult GetPositionStatus()
        {
            var stats = _positionService.GetServiceStats();
            var clients = _positionService.GetSubscribedClients();

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
                    clients = clients,
                    count = clients.Length
                },
                timestamp = DateTimeOffset.UtcNow.ToUnixTimeMilliseconds()
            });
        }
    }
}