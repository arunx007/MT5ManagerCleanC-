using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.Trading.Interfaces;
using MT5Wrapper.Trading.DTOs;
using MT5Wrapper.OrderManagement.DTOs;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// Trading Operations Controller - Handles order and position management
    /// Manager can place orders and manage positions for any client account
    /// </summary>
    [ApiController]
    [Route("api/trading")]
    [Authorize] // All trading operations require authentication
    public class TradingController : ControllerBase
    {
        private readonly ITradingService _tradingService;

        public TradingController(ITradingService tradingService)
        {
            _tradingService = tradingService ?? throw new ArgumentNullException(nameof(tradingService));
        }

        /// <summary>
        /// Place a new trading order for a client
        /// </summary>
        [HttpPost("place-order")]
        public async Task<IActionResult> PlaceOrder([FromBody] PlaceOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Order request cannot be null" });

                // Extract manager token from JWT
                var managerToken = GetManagerTokenFromJwt();
                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var orderRequest = new OrderRequest
                {
                    Symbol = request.Symbol,
                    Type = request.Type,
                    Volume = request.Volume,
                    Price = request.Price,
                    StopLoss = request.StopLoss,
                    TakeProfit = request.TakeProfit,
                    StopLimit = request.StopLimit,
                    Expiration = request.Expiration,
                    Comment = request.Comment,
                    Magic = request.Magic
                };

                var result = await _tradingService.PlaceOrderAsync(managerToken, request.ClientLogin, orderRequest);

                if (result.Success)
                    return Ok(result);
                else
                    return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Modify an existing order
        /// </summary>
        [HttpPut("modify-order/{orderId}")]
        public async Task<IActionResult> ModifyOrder(ulong orderId, [FromBody] MT5Wrapper.Trading.DTOs.ModifyOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Modify request cannot be null" });

                // Extract manager token and client login from JWT
                var managerToken = GetManagerTokenFromJwt();
                var clientLogin = GetClientLoginFromJwt();

                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var result = await _tradingService.ModifyOrderAsync(managerToken, clientLogin, orderId, request);

                if (result.Success)
                    return Ok(result);
                else
                    return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Cancel an existing order
        /// </summary>
        [HttpDelete("cancel-order/{orderId}")]
        public async Task<IActionResult> CancelOrder(ulong orderId)
        {
            try
            {
                // Extract manager token and client login from JWT
                var managerToken = GetManagerTokenFromJwt();
                var clientLogin = GetClientLoginFromJwt();

                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var result = await _tradingService.CancelOrderAsync(managerToken, clientLogin, orderId);

                if (result.Success)
                    return Ok(result);
                else
                    return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get all orders for a client
        /// </summary>
        [HttpGet("orders/{clientLogin}")]
        public async Task<IActionResult> GetOrders(ulong clientLogin)
        {
            try
            {
                // Extract manager token from JWT
                var managerToken = GetManagerTokenFromJwt();
                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var orders = await _tradingService.GetOrdersAsync(managerToken, clientLogin);
                return Ok(new { orders = orders, count = orders.Count() });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get all positions for a client
        /// </summary>
        [HttpGet("positions/{clientLogin}")]
        public async Task<IActionResult> GetPositions(ulong clientLogin)
        {
            try
            {
                // Extract manager token from JWT
                var managerToken = GetManagerTokenFromJwt();
                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var positions = await _tradingService.GetPositionsAsync(managerToken, clientLogin);
                return Ok(new { positions = positions, count = positions.Count() });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Close an open position
        /// </summary>
        [HttpDelete("close-position/{positionId}")]
        public async Task<IActionResult> ClosePosition(ulong positionId, [FromQuery] ulong clientLogin)
        {
            try
            {
                // Extract manager token from JWT
                var managerToken = GetManagerTokenFromJwt();
                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var result = await _tradingService.ClosePositionAsync(managerToken, clientLogin, positionId);

                if (result.Success)
                    return Ok(result);
                else
                    return BadRequest(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get account trading information
        /// </summary>
        [HttpGet("account-info/{clientLogin}")]
        public async Task<IActionResult> GetAccountTradingInfo(ulong clientLogin)
        {
            try
            {
                // Extract manager token from JWT
                var managerToken = GetManagerTokenFromJwt();
                if (string.IsNullOrEmpty(managerToken))
                    return Unauthorized(new { error = "Authentication required", message = "Valid manager token required" });

                var accountInfo = await _tradingService.GetAccountTradingInfoAsync(managerToken, clientLogin);
                return Ok(accountInfo);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        // Helper methods to extract data from JWT tokens
        private string GetManagerTokenFromJwt()
        {
            // For now, return a placeholder. In production, extract from JWT claims
            return User.FindFirst("manager_token")?.Value ?? "manager_token_placeholder";
        }

        private ulong GetClientLoginFromJwt()
        {
            // Extract client login from JWT claims
            var clientLoginClaim = User.FindFirst("client_login")?.Value;
            return ulong.TryParse(clientLoginClaim, out var login) ? login : 0;
        }
    }

    // Request DTOs for the controller
    public class PlaceOrderRequest
    {
        public ulong ClientLogin { get; set; }
        public string Symbol { get; set; } = "";
        public OrderType Type { get; set; }
        public double Volume { get; set; }
        public double Price { get; set; }
        public double StopLoss { get; set; }
        public double TakeProfit { get; set; }
        public double StopLimit { get; set; }
        public DateTime? Expiration { get; set; }
        public string Comment { get; set; } = "";
        public int Magic { get; set; }
    }
}