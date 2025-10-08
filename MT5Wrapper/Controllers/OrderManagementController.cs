using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.OrderManagement.Interfaces;
using MT5Wrapper.OrderManagement.DTOs;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// Order Management Controller - Handles order creation, modification, and retrieval
    /// Supports both market orders and pending orders
    /// </summary>
    [ApiController]
    [Route("api/orders")]
    [Authorize] // All order operations require authentication
    public class OrderManagementController : ControllerBase
    {
        private readonly IOrderManagementService _orderService;

        public OrderManagementController(IOrderManagementService orderService)
        {
            _orderService = orderService ?? throw new ArgumentNullException(nameof(orderService));
        }

        /// <summary>
        /// Create a market order (immediate execution)
        /// </summary>
        [HttpPost("market")]
        public async Task<IActionResult> CreateMarketOrder([FromBody] CreateOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Order request cannot be null" });

                var result = await _orderService.CreateOrderAsync(request);

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
        /// Create a pending order (limit/stop orders)
        /// </summary>
        [HttpPost("pending")]
        public async Task<IActionResult> CreatePendingOrder([FromBody] CreatePendingOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Pending order request cannot be null" });

                var result = await _orderService.CreatePendingOrderAsync(request);

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
        [HttpPut("modify")]
        public async Task<IActionResult> ModifyOrder([FromBody] ModifyOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Modify request cannot be null" });

                var result = await _orderService.ModifyOrderAsync(request);

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
        [HttpDelete("cancel")]
        public async Task<IActionResult> CancelOrder([FromBody] CancelOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Cancel request cannot be null" });

                var result = await _orderService.CancelOrderAsync(request);

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
        /// Get order details by ticket number
        /// </summary>
        [HttpGet("ticket/{ticket}")]
        public async Task<IActionResult> GetOrderByTicket(ulong ticket)
        {
            try
            {
                var order = await _orderService.GetOrderAsync(ticket);

                if (order != null)
                    return Ok(order);
                else
                    return NotFound(new { error = "Order not found", ticket = ticket });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get order history for a login within date range
        /// </summary>
        [HttpGet("history/{login}")]
        public async Task<IActionResult> GetOrderHistory(ulong login, [FromQuery] long from, [FromQuery] long to)
        {
            try
            {
                var orders = await _orderService.GetOrderHistoryAsync(login, from, to);
                return Ok(new { orders = orders, count = orders.Count() });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get open orders for a login
        /// </summary>
        [HttpGet("open/{login}")]
        public async Task<IActionResult> GetOpenOrders(ulong login)
        {
            try
            {
                var orders = await _orderService.GetOpenOrdersAsync(login);
                return Ok(new { orders = orders, count = orders.Count() });
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Modify a pending order
        /// </summary>
        [HttpPut("pending/modify")]
        public async Task<IActionResult> ModifyPendingOrder([FromBody] ModifyPendingOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Modify pending order request cannot be null" });

                var result = await _orderService.ModifyPendingOrderAsync(request);

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
        /// Cancel a pending order
        /// </summary>
        [HttpDelete("pending/cancel")]
        public async Task<IActionResult> CancelPendingOrder([FromBody] CancelPendingOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Cancel pending order request cannot be null" });

                var result = await _orderService.CancelPendingOrderAsync(request);

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
        /// Validate an order before placing it
        /// </summary>
        [HttpPost("validate")]
        public async Task<IActionResult> ValidateOrder([FromBody] CreateOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Order request cannot be null" });

                var result = await _orderService.ValidateOrderAsync(request);
                return Ok(result);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Close an order (if supported by broker)
        /// </summary>
        [HttpDelete("close")]
        public async Task<IActionResult> CloseOrder([FromBody] CloseOrderRequest request)
        {
            try
            {
                if (request == null)
                    return BadRequest(new { error = "Invalid request", message = "Close order request cannot be null" });

                var result = await _orderService.CloseOrderAsync(request);

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
    }
}