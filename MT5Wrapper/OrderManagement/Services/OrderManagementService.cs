using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.OrderManagement.DTOs;
using MT5Wrapper.OrderManagement.Interfaces;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.OrderManagement.Services
{
    public class OrderManagementService : IOrderManagementService, IDisposable
    {
        private readonly IMT5ConnectionService _mt5Connection;
        private readonly ILogger<OrderManagementService> _logger;
        private bool _disposed;

        public OrderManagementService(
            IMT5ConnectionService mt5Connection,
            ILogger<OrderManagementService> logger)
        {
            _mt5Connection = mt5Connection ?? throw new ArgumentNullException(nameof(mt5Connection));
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
        }

        public async Task<OrderOperationResult> CreateOrderAsync(CreateOrderRequest request)
        {
            try
            {
                _logger.LogInformation("Creating order for login {Login}, symbol {Symbol}, type {Type}, volume {Volume}",
                    request.Login, request.Symbol, request.Type, request.Volume);

                // Validate request
                var validation = await ValidateOrderAsync(request);
                if (!validation.IsValid)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = validation.Message,
                        ErrorCode = "VALIDATION_FAILED"
                    };
                }

                // Get MT5 manager
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        ErrorCode = "CONNECTION_ERROR"
                    };
                }

                // Create order object
                var order = manager.OrderCreate();
                if (order == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "Failed to create order object",
                        ErrorCode = "CREATE_FAILED"
                    };
                }

                try
                {
                    // Set order properties
                    order.Login(request.Login);
                    order.Symbol(request.Symbol);
                    order.Type((uint)request.Type);
                    order.VolumeInitial((ulong)request.Volume);
                    order.PriceOrder(request.Price);
                    order.PriceSL(request.PriceStopLoss);
                    order.PriceTP(request.PriceTakeProfit);
                    order.ExpertID((ulong)request.Magic);
                    order.Comment(request.Comment);

                    if (request.Expiration.HasValue)
                    {
                        order.TimeExpiration(request.Expiration.Value.ToUnixTimeSeconds());
                    }

                    if (!string.IsNullOrEmpty(request.ExternalId))
                    {
                        order.ExternalID(request.ExternalId);
                    }

                    // Add the order
                    var result = manager.OrderAdd(order);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return new OrderOperationResult
                        {
                            Success = true,
                            Message = "Order created successfully",
                            Ticket = order.Order(),
                            Order = MapOrderToDto(order)
                        };
                    }
                    else
                    {
                        return new OrderOperationResult
                        {
                            Success = false,
                            Message = $"Order creation failed: {result}",
                            ErrorCode = result.ToString()
                        };
                    }
                }
                finally
                {
                    order.Dispose();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error creating order for login {Login}", request.Login);
                return new OrderOperationResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ErrorCode = "EXCEPTION"
                };
            }
        }

        public async Task<OrderOperationResult> CreatePendingOrderAsync(CreatePendingOrderRequest request)
        {
            try
            {
                _logger.LogInformation("Creating pending order for login {Login}, symbol {Symbol}, type {Type}",
                    request.Login, request.Symbol, request.Type);

                // Validate request
                var validation = await ValidatePendingOrderAsync(request);
                if (!validation.IsValid)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = validation.Message,
                        ErrorCode = "VALIDATION_FAILED"
                    };
                }

                // Get MT5 manager
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        ErrorCode = "CONNECTION_ERROR"
                    };
                }

                // Create order object
                var order = manager.OrderCreate();
                if (order == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "Failed to create order object",
                        ErrorCode = "CREATE_FAILED"
                    };
                }

                try
                {
                    // Set order properties for pending order
                    order.Login(request.Login);
                    order.Symbol(request.Symbol);
                    order.Type((uint)request.Type);
                    order.VolumeInitial((ulong)request.Volume);
                    order.PriceOrder(request.PriceOrder);
                    order.PriceSL(request.PriceStopLoss);
                    order.PriceTP(request.PriceTakeProfit);
                    order.ExpertID((ulong)request.Magic);
                    order.Comment(request.Comment);

                    if (request.Expiration.HasValue)
                    {
                        order.TimeExpiration(request.Expiration.Value.ToUnixTimeSeconds());
                    }

                    if (!string.IsNullOrEmpty(request.ExternalId))
                    {
                        order.ExternalID(request.ExternalId);
                    }

                    // Add the pending order
                    var result = manager.OrderAdd(order);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return new OrderOperationResult
                        {
                            Success = true,
                            Message = "Pending order created successfully",
                            Ticket = order.Order(),
                            Order = MapOrderToDto(order)
                        };
                    }
                    else
                    {
                        return new OrderOperationResult
                        {
                            Success = false,
                            Message = $"Pending order creation failed: {result}",
                            ErrorCode = result.ToString()
                        };
                    }
                }
                finally
                {
                    order.Dispose();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error creating pending order for login {Login}", request.Login);
                return new OrderOperationResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ErrorCode = "EXCEPTION"
                };
            }
        }

        public async Task<OrderOperationResult> ModifyOrderAsync(ModifyOrderRequest request)
        {
            try
            {
                _logger.LogInformation("Modifying order {Ticket}", request.Ticket);

                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        ErrorCode = "CONNECTION_ERROR"
                    };
                }

                // Get existing order
                var order = manager.OrderCreate();
                if (order == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "Failed to create order object",
                        ErrorCode = "CREATE_FAILED"
                    };
                }

                try
                {
                    var getResult = manager.OrderGet(request.Ticket, order);
                    if (getResult != MTRetCode.MT_RET_OK)
                    {
                        return new OrderOperationResult
                        {
                            Success = false,
                            Message = $"Order not found: {getResult}",
                            ErrorCode = "ORDER_NOT_FOUND"
                        };
                    }

                    // Update order properties
                    if (request.Price.HasValue)
                        order.PriceOrder(request.Price.Value);
                    if (request.PriceStopLoss.HasValue)
                        order.PriceSL(request.PriceStopLoss.Value);
                    if (request.PriceTakeProfit.HasValue)
                        order.PriceTP(request.PriceTakeProfit.Value);
                    if (request.Expiration.HasValue)
                        order.TimeExpiration(request.Expiration.Value.ToUnixTimeSeconds());

                    // Update the order
                    var updateResult = manager.OrderUpdate(order);
                    if (updateResult == MTRetCode.MT_RET_OK)
                    {
                        return new OrderOperationResult
                        {
                            Success = true,
                            Message = "Order modified successfully",
                            Ticket = order.Order(),
                            Order = MapOrderToDto(order)
                        };
                    }
                    else
                    {
                        return new OrderOperationResult
                        {
                            Success = false,
                            Message = $"Order modification failed: {updateResult}",
                            ErrorCode = updateResult.ToString()
                        };
                    }
                }
                finally
                {
                    order.Dispose();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error modifying order {Ticket}", request.Ticket);
                return new OrderOperationResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ErrorCode = "EXCEPTION"
                };
            }
        }

        public async Task<OrderOperationResult> CancelOrderAsync(CancelOrderRequest request)
        {
            try
            {
                _logger.LogInformation("Canceling order {Ticket}", request.Ticket);

                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null)
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        ErrorCode = "CONNECTION_ERROR"
                    };
                }

                var result = manager.OrderDelete(request.Ticket);
                if (result == MTRetCode.MT_RET_OK)
                {
                    return new OrderOperationResult
                    {
                        Success = true,
                        Message = "Order canceled successfully",
                        Ticket = request.Ticket
                    };
                }
                else
                {
                    return new OrderOperationResult
                    {
                        Success = false,
                        Message = $"Order cancellation failed: {result}",
                        ErrorCode = result.ToString()
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error canceling order {Ticket}", request.Ticket);
                return new OrderOperationResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ErrorCode = "EXCEPTION"
                };
            }
        }

        public async Task<OrderOperationResult> CloseOrderAsync(CloseOrderRequest request)
        {
            // Note: In MT5, closing orders is typically done through positions
            // This would require position management which is separate
            return new OrderOperationResult
            {
                Success = false,
                Message = "Order closing should be done through position management",
                ErrorCode = "NOT_SUPPORTED"
            };
        }

        public async Task<OrderDto> GetOrderAsync(ulong ticket)
        {
            try
            {
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null) return null;

                var order = manager.OrderCreate();
                if (order == null) return null;

                try
                {
                    var result = manager.OrderGet(ticket, order);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return MapOrderToDto(order);
                    }
                }
                finally
                {
                    order.Release();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting order {Ticket}", ticket);
            }

            return null;
        }

        public async Task<IEnumerable<OrderDto>> GetOpenOrdersAsync(ulong login)
        {
            try
            {
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null) return Enumerable.Empty<OrderDto>();

                var orders = manager.OrderCreateArray();
                if (orders == null) return Enumerable.Empty<OrderDto>();

                try
                {
                    var result = manager.OrderGetOpen(login, orders);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return MapOrdersToDtoList(orders);
                    }
                }
                finally
                {
                    orders.Release();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting open orders for login {Login}", login);
            }

            return Enumerable.Empty<OrderDto>();
        }

        public async Task<IEnumerable<OrderDto>> GetOrderHistoryAsync(ulong login, long from, long to)
        {
            try
            {
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null) return Enumerable.Empty<OrderDto>();

                var orders = manager.OrderCreateArray();
                if (orders == null) return Enumerable.Empty<OrderDto>();

                try
                {
                    var result = manager.HistoryRequest(login, from, to, orders);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return MapOrdersToDtoList(orders);
                    }
                }
                finally
                {
                    orders.Release();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting order history for login {Login}", login);
            }

            return Enumerable.Empty<OrderDto>();
        }

        public async Task<IEnumerable<OrderDto>> GetOrdersByGroupAsync(string group)
        {
            try
            {
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null) return Enumerable.Empty<OrderDto>();

                var orders = manager.OrderCreateArray();
                if (orders == null) return Enumerable.Empty<OrderDto>();

                try
                {
                    var result = manager.OrderGetByGroup(group, orders);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return MapOrdersToDtoList(orders);
                    }
                }
                finally
                {
                    orders.Release();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting orders by group {Group}", group);
            }

            return Enumerable.Empty<OrderDto>();
        }

        public async Task<IEnumerable<OrderDto>> GetOrdersByLoginsAsync(IEnumerable<ulong> logins)
        {
            try
            {
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager == null) return Enumerable.Empty<OrderDto>();

                var orders = manager.OrderCreateArray();
                if (orders == null) return Enumerable.Empty<OrderDto>();

                try
                {
                    var loginsArray = logins.ToArray();
                    var result = manager.OrderGetByLogins(loginsArray, orders);
                    if (result == MTRetCode.MT_RET_OK)
                    {
                        return MapOrdersToDtoList(orders);
                    }
                }
                finally
                {
                    orders.Release();
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error getting orders by logins");
            }

            return Enumerable.Empty<OrderDto>();
        }

        public async Task<OrderOperationResult> ModifyPendingOrderAsync(ModifyPendingOrderRequest request)
        {
            // Similar to ModifyOrderAsync but for pending orders
            return await ModifyOrderAsync(new ModifyOrderRequest
            {
                Ticket = request.Ticket,
                Price = request.Price,
                PriceStopLoss = request.PriceStopLoss,
                PriceTakeProfit = request.PriceTakeProfit,
                Expiration = request.Expiration
            });
        }

        public async Task<OrderOperationResult> CancelPendingOrderAsync(CancelPendingOrderRequest request)
        {
            return await CancelOrderAsync(new CancelOrderRequest
            {
                Ticket = request.Ticket,
                Reason = request.Reason
            });
        }

        public async Task<IEnumerable<OrderOperationResult>> CreateOrdersBatchAsync(IEnumerable<CreateOrderRequest> requests)
        {
            var results = new List<OrderOperationResult>();

            foreach (var request in requests)
            {
                var result = await CreateOrderAsync(request);
                results.Add(result);
            }

            return results;
        }

        public async Task<IEnumerable<OrderOperationResult>> CancelOrdersBatchAsync(IEnumerable<CancelOrderRequest> requests)
        {
            var results = new List<OrderOperationResult>();

            foreach (var request in requests)
            {
                var result = await CancelOrderAsync(request);
                results.Add(result);
            }

            return results;
        }

        public async Task<OrderValidationResult> ValidateOrderAsync(CreateOrderRequest request)
        {
            var result = new OrderValidationResult { IsValid = true };

            // Basic validation
            if (request.Login == 0)
            {
                result.IsValid = false;
                result.Errors.Add("Login is required");
            }

            if (string.IsNullOrEmpty(request.Symbol))
            {
                result.IsValid = false;
                result.Errors.Add("Symbol is required");
            }

            if (request.Volume <= 0)
            {
                result.IsValid = false;
                result.Errors.Add("Volume must be greater than 0");
            }

            // Check margin requirements
            try
            {
                var manager = _mt5Connection.Manager as CIMTManagerAPI;
                if (manager != null)
                {
                    // Get user account for margin check
                    var account = manager.UserCreateAccount();
                    if (account != null)
                    {
                        try
                        {
                            var accountResult = manager.UserAccountGet(request.Login, account);
                            if (accountResult == MTRetCode.MT_RET_OK)
                            {
                                // Check margin
                                var marginResult = manager.TradeMarginCheck(
                                    request.Login, request.Symbol,
                                    (CIMTOrder.EnOrderType)request.Type, (ulong)request.Volume, request.Price,
                                    account, account);

                                if (marginResult != MTRetCode.MT_RET_OK)
                                {
                                    result.IsValid = false;
                                    result.Errors.Add($"Insufficient margin: {marginResult}");
                                }
                            }
                        }
                        finally
                        {
                            account.Dispose();
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                _logger.LogWarning(ex, "Error validating order margin");
                result.Warnings.Add("Could not validate margin requirements");
            }

            result.Message = result.IsValid ? "Order is valid" :
                $"Order validation failed: {string.Join(", ", result.Errors)}";

            return result;
        }

        public async Task<OrderValidationResult> ValidatePendingOrderAsync(CreatePendingOrderRequest request)
        {
            var result = await ValidateOrderAsync(request);

            // Additional validation for pending orders
            if (request.PriceOrder <= 0)
            {
                result.IsValid = false;
                result.Errors.Add("PriceOrder must be greater than 0 for pending orders");
            }

            return result;
        }

        private OrderDto MapOrderToDto(CIMTOrder order)
        {
            return new OrderDto
            {
                Ticket = order.Order(),
                Login = order.Login(),
                Symbol = order.Symbol(),
                Type = (OrderType)(uint)order.Type(),
                State = (OrderState)(uint)order.State(),
                Volume = order.VolumeInitial(),
                VolumeCurrent = order.VolumeCurrent(),
                Price = order.PriceOrder(),
                PriceOrder = order.PriceOrder(),
                PriceStopLoss = order.PriceSL(),
                PriceTakeProfit = order.PriceTP(),
                Magic = (long)order.ExpertID(),
                Comment = order.Comment(),
                TimeSetup = DateTimeOffset.FromUnixTimeSeconds(order.TimeSetup()),
                TimeExpiration = DateTimeOffset.FromUnixTimeSeconds(order.TimeExpiration()),
                TimeDone = DateTimeOffset.FromUnixTimeSeconds(order.TimeDone()),
                Profit = 0, // Orders don't have profit directly
                Commission = 0, // Orders don't have commission directly
                Swap = 0, // Orders don't have swap directly
                Fee = 0, // Orders don't have fee directly
                ExternalId = order.ExternalID(),
                Group = string.Empty, // Would need to be populated separately
                Reason = (int)order.Reason()
            };
        }

        private IEnumerable<OrderDto> MapOrdersToDtoList(CIMTOrderArray orders)
        {
            var result = new List<OrderDto>();

            for (uint i = 0; i < orders.Total(); i++)
            {
                var order = orders.Next(i);
                if (order != null)
                {
                    result.Add(MapOrderToDto(order));
                }
            }

            return result;
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!_disposed)
            {
                if (disposing)
                {
                    // Dispose managed resources
                }

                _disposed = true;
            }
        }
    }
}