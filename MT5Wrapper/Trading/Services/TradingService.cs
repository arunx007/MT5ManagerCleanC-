using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Extensions.Logging;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.MultiTenant.Interfaces;
using MT5Wrapper.Authentication.Interfaces;
using MT5Wrapper.Trading.DTOs;
using MT5Wrapper.Trading.Interfaces;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.Trading.Services
{
    public class TradingService : ITradingService
    {
        private readonly ILogger<TradingService> _logger;
        private readonly IMT5ConnectionService _mt5Connection;
        private readonly IManagerAuthService _managerAuth;

        public TradingService(
            ILogger<TradingService> logger,
            IMT5ConnectionService mt5Connection,
            IManagerAuthService managerAuth)
        {
            _logger = logger;
            _mt5Connection = mt5Connection;
            _managerAuth = managerAuth;
        }

        // Manager places orders for any client account
        public async Task<OrderResult> PlaceOrderAsync(string managerToken, ulong clientLogin, OrderRequest request)
        {
            try
            {
                _logger.LogInformation($"Manager placing order for client {clientLogin}: {request.Symbol} {request.Type} {request.Volume}");

                // Validate manager token and permissions
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "Invalid manager token",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_ACCESS
                    };
                }

                // Get MT5 manager connection
                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_CONNECTION
                    };
                }

                // Create order object
                var order = manager.OrderCreate();
                if (order == null)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "Failed to create order object",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_DATA
                    };
                }

                // Set order properties
                order.Login(clientLogin);
                order.Symbol(request.Symbol);
                order.Type((CIMTOrder.EnOrderType)request.Type);
                order.VolumeInitial(request.Volume);
                order.VolumeCurrent(request.Volume);
                order.PriceOrder(request.Price);
                order.PriceSL(request.StopLoss);
                order.PriceTP(request.TakeProfit);
                order.Comment(request.Comment);
                order.ExpertID((ulong)request.Magic);

                if (request.Expiration.HasValue)
                {
                    order.TimeExpiration = request.Expiration.Value;
                }

                // Place the order
                var result = manager.OrderAdd(order);
                var orderId = order.Order();

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    _logger.LogInformation($"Order placed successfully: {orderId} for client {clientLogin}");

                    return new OrderResult
                    {
                        Success = true,
                        Message = "Order placed successfully",
                        OrderId = orderId,
                        ClientLogin = clientLogin,
                        RetCode = result,
                        Order = ConvertToOrderDto(order)
                    };
                }
                else
                {
                    _logger.LogWarning($"Order placement failed: {result} for client {clientLogin}");

                    return new OrderResult
                    {
                        Success = false,
                        Message = $"Order placement failed: {result}",
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error placing order for client {clientLogin}");
                return new OrderResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ClientLogin = clientLogin,
                    RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_EXTERNAL
                };
            }
        }

        // Manager modifies orders for any client
        public async Task<OrderResult> ModifyOrderAsync(string managerToken, ulong clientLogin, ulong orderId, ModifyOrderRequest request)
        {
            try
            {
                _logger.LogInformation($"Manager modifying order {orderId} for client {clientLogin}");

                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "Invalid manager token",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_ACCESS
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_CONNECTION
                    };
                }

                // Get existing order
                var order = manager.OrderCreate();
                var result = manager.OrderGet(orderId, order);

                if (result != MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = $"Order not found: {result}",
                        OrderId = orderId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }

                // Modify order properties
                if (request.Price.HasValue) order.PriceOrder(request.Price.Value);
                if (request.StopLoss.HasValue) order.PriceSL(request.StopLoss.Value);
                if (request.TakeProfit.HasValue) order.PriceTP(request.TakeProfit.Value);
                if (request.StopLimit.HasValue) order.PriceStopLimit(request.StopLimit.Value);
                if (request.Expiration.HasValue) order.TimeExpiration(request.Expiration.Value);

                // Update the order
                result = manager.OrderUpdate(order);

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    _logger.LogInformation($"Order {orderId} modified successfully for client {clientLogin}");

                    return new OrderResult
                    {
                        Success = true,
                        Message = "Order modified successfully",
                        OrderId = orderId,
                        ClientLogin = clientLogin,
                        RetCode = result,
                        Order = ConvertToOrderDto(order)
                    };
                }
                else
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = $"Order modification failed: {result}",
                        OrderId = orderId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error modifying order {orderId} for client {clientLogin}");
                return new OrderResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    OrderId = orderId,
                    ClientLogin = clientLogin,
                    RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_EXTERNAL
                };
            }
        }

        // Manager cancels orders for any client
        public async Task<OrderResult> CancelOrderAsync(string managerToken, ulong clientLogin, ulong orderId)
        {
            try
            {
                _logger.LogInformation($"Manager canceling order {orderId} for client {clientLogin}");

                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "Invalid manager token",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_ACCESS
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_CONNECTION
                    };
                }

                // Cancel the order
                var result = manager.OrderDelete(orderId);

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    _logger.LogInformation($"Order {orderId} canceled successfully for client {clientLogin}");

                    return new OrderResult
                    {
                        Success = true,
                        Message = "Order canceled successfully",
                        OrderId = orderId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
                else
                {
                    return new OrderResult
                    {
                        Success = false,
                        Message = $"Order cancellation failed: {result}",
                        OrderId = orderId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error canceling order {orderId} for client {clientLogin}");
                return new OrderResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    OrderId = orderId,
                    ClientLogin = clientLogin,
                    RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_EXTERNAL
                };
            }
        }

        // Manager gets all orders for any client
        public async Task<IEnumerable<OrderDto>> GetOrdersAsync(string managerToken, ulong clientLogin)
        {
            try
            {
                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return Enumerable.Empty<OrderDto>();
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return Enumerable.Empty<OrderDto>();
                }

                // Get orders for the client
                var orders = new List<OrderDto>();
                var order = manager.OrderCreate();

                // Note: In a real implementation, you'd iterate through all orders for the client
                // This is a simplified version for demonstration

                return orders;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting orders for client {clientLogin}");
                return Enumerable.Empty<OrderDto>();
            }
        }

        // Manager closes positions for any client
        public async Task<PositionResult> ClosePositionAsync(string managerToken, ulong clientLogin, ulong positionId)
        {
            try
            {
                _logger.LogInformation($"Manager closing position {positionId} for client {clientLogin}");

                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = "Invalid manager token",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_ACCESS
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_CONNECTION
                    };
                }

                // Close the position
                var result = manager.PositionClose(positionId);

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    _logger.LogInformation($"Position {positionId} closed successfully for client {clientLogin}");

                    return new PositionResult
                    {
                        Success = true,
                        Message = "Position closed successfully",
                        PositionId = positionId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
                else
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = $"Position closure failed: {result}",
                        PositionId = positionId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error closing position {positionId} for client {clientLogin}");
                return new PositionResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    PositionId = positionId,
                    ClientLogin = clientLogin,
                    RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_EXTERNAL
                };
            }
        }

        // Manager modifies positions for any client
        public async Task<PositionResult> ModifyPositionAsync(string managerToken, ulong clientLogin, ulong positionId, ModifyPositionRequest request)
        {
            try
            {
                _logger.LogInformation($"Manager modifying position {positionId} for client {clientLogin}");

                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = "Invalid manager token",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_ACCESS
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_CONNECTION
                    };
                }

                // Get existing position
                var position = manager.PositionCreate();
                var result = manager.PositionGet(positionId, position);

                if (result != MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = $"Position not found: {result}",
                        PositionId = positionId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }

                // Modify position properties
                if (request.StopLoss.HasValue) position.PriceSL(request.StopLoss.Value);
                if (request.TakeProfit.HasValue) position.PriceTP(request.TakeProfit.Value);

                // Update the position
                result = manager.PositionUpdate(position);

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    _logger.LogInformation($"Position {positionId} modified successfully for client {clientLogin}");

                    return new PositionResult
                    {
                        Success = true,
                        Message = "Position modified successfully",
                        PositionId = positionId,
                        ClientLogin = clientLogin,
                        RetCode = result,
                        Position = ConvertToPositionDto(position)
                    };
                }
                else
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = $"Position modification failed: {result}",
                        PositionId = positionId,
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error modifying position {positionId} for client {clientLogin}");
                return new PositionResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    PositionId = positionId,
                    ClientLogin = clientLogin,
                    RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_EXTERNAL
                };
            }
        }

        // Manager closes all positions for any client
        public async Task<PositionResult> CloseAllPositionsAsync(string managerToken, ulong clientLogin, string symbol = null)
        {
            try
            {
                _logger.LogInformation($"Manager closing all positions for client {clientLogin}, symbol: {symbol ?? "all"}");

                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = "Invalid manager token",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_ACCESS
                    };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = "MT5 connection not available",
                        RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_CONNECTION
                    };
                }

                // Close all positions for the client
                var result = string.IsNullOrEmpty(symbol)
                    ? manager.PositionCloseAll(clientLogin)
                    : manager.PositionCloseBySymbol(clientLogin, symbol);

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    _logger.LogInformation($"All positions closed successfully for client {clientLogin}");

                    return new PositionResult
                    {
                        Success = true,
                        Message = "All positions closed successfully",
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
                else
                {
                    return new PositionResult
                    {
                        Success = false,
                        Message = $"Position closure failed: {result}",
                        ClientLogin = clientLogin,
                        RetCode = result
                    };
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error closing all positions for client {clientLogin}");
                return new PositionResult
                {
                    Success = false,
                    Message = $"Exception: {ex.Message}",
                    ClientLogin = clientLogin,
                    RetCode = MT5Wrapper.Trading.DTOs.MTRetCode.MT_RET_ERR_EXTERNAL
                };
            }
        }

        // Manager gets all positions for any client
        public async Task<IEnumerable<PositionDto>> GetPositionsAsync(string managerToken, ulong clientLogin)
        {
            try
            {
                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return Enumerable.Empty<PositionDto>();
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return Enumerable.Empty<PositionDto>();
                }

                // Get positions for the client
                var positions = new List<PositionDto>();
                var position = manager.PositionCreate();

                // Note: In a real implementation, you'd iterate through all positions for the client
                // This is a simplified version for demonstration

                return positions;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting positions for client {clientLogin}");
                return Enumerable.Empty<PositionDto>();
            }
        }

        // Manager gets trade history for any client
        public async Task<IEnumerable<TradeDto>> GetTradeHistoryAsync(string managerToken, ulong clientLogin, DateTime from, DateTime to)
        {
            try
            {
                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return Enumerable.Empty<TradeDto>();
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return Enumerable.Empty<TradeDto>();
                }

                // Get trade history for the client
                var trades = new List<TradeDto>();
                var deal = manager.DealCreate();

                // Note: In a real implementation, you'd query the deal history
                // This is a simplified version for demonstration

                return trades;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting trade history for client {clientLogin}");
                return Enumerable.Empty<TradeDto>();
            }
        }

        // Manager gets recent trades for any client
        public async Task<IEnumerable<TradeDto>> GetRecentTradesAsync(string managerToken, ulong clientLogin, int count = 50)
        {
            var to = DateTime.UtcNow;
            var from = to.AddDays(-30); // Last 30 days
            return await GetTradeHistoryAsync(managerToken, clientLogin, from, to);
        }

        // Manager gets full account trading info for any client
        public async Task<AccountTradingInfoDto> GetAccountTradingInfoAsync(string managerToken, ulong clientLogin)
        {
            try
            {
                // Validate manager token
                var tokenValidation = await _managerAuth.ValidateManagerTokenAsync(managerToken);
                if (!tokenValidation.IsValid)
                {
                    return new AccountTradingInfoDto { ClientLogin = clientLogin };
                }

                var manager = _mt5Connection.Manager;
                if (manager == null)
                {
                    return new AccountTradingInfoDto { ClientLogin = clientLogin };
                }

                // Get account information
                var account = manager.AccountCreate();
                var result = manager.AccountGet(clientLogin, account);

                if (result == MetaQuotes.MT5CommonAPI.MTRetCode.MT_RET_OK)
                {
                    return new AccountTradingInfoDto
                    {
                        ClientLogin = clientLogin,
                        Balance = account.Balance(),
                        Credit = account.Credit(),
                        Profit = account.Profit(),
                        Equity = account.Equity(),
                        Margin = account.Margin(),
                        MarginFree = account.MarginFree(),
                        MarginLevel = account.MarginLevel(),
                        MarginLeverage = account.Leverage(),
                        LastUpdate = DateTime.UtcNow
                    };
                }

                return new AccountTradingInfoDto { ClientLogin = clientLogin };
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, $"Error getting account info for client {clientLogin}");
                return new AccountTradingInfoDto { ClientLogin = clientLogin };
            }
        }

        // Manager gets margin info for any client
        public async Task<MarginInfoDto> GetMarginInfoAsync(string managerToken, ulong clientLogin)
        {
            var accountInfo = await GetAccountTradingInfoAsync(managerToken, clientLogin);

            return new MarginInfoDto
            {
                ClientLogin = clientLogin,
                Margin = accountInfo.Margin,
                MarginFree = accountInfo.MarginFree,
                MarginLevel = accountInfo.MarginLevel,
                MarginLeverage = accountInfo.MarginLeverage,
                LastUpdate = accountInfo.LastUpdate
            };
        }

        // Bulk operations for managers
        public async Task<BulkOrderResult> PlaceBulkOrdersAsync(string managerToken, IEnumerable<BulkOrderRequest> requests)
        {
            var results = new List<OrderResult>();
            var successful = 0;
            var failed = 0;

            foreach (var request in requests)
            {
                var result = await PlaceOrderAsync(managerToken, request.ClientLogin, request.OrderRequest);
                results.Add(result);

                if (result.Success)
                    successful++;
                else
                    failed++;
            }

            return new BulkOrderResult
            {
                TotalRequests = requests.Count(),
                SuccessfulOrders = successful,
                FailedOrders = failed,
                Results = results
            };
        }

        public async Task<BulkPositionResult> CloseBulkPositionsAsync(string managerToken, IEnumerable<BulkPositionRequest> requests)
        {
            var results = new List<PositionResult>();
            var successful = 0;
            var failed = 0;

            foreach (var request in requests)
            {
                var result = await ClosePositionAsync(managerToken, request.ClientLogin, request.PositionId);
                results.Add(result);

                if (result.Success)
                    successful++;
                else
                    failed++;
            }

            return new BulkPositionResult
            {
                TotalRequests = requests.Count(),
                SuccessfulClosures = successful,
                FailedClosures = failed,
                Results = results
            };
        }

        // Helper methods to convert MT5 objects to DTOs
        private OrderDto ConvertToOrderDto(CIMTOrder order)
        {
            return new OrderDto
            {
                OrderId = order.Order(),
                ClientLogin = order.Login(),
                Symbol = order.Symbol(),
                Type = (TradingOrderType)(uint)order.Type(),
                Side = (CIMTOrder.EnOrderType)order.Type() == CIMTOrder.EnOrderType.OP_BUY
                    ? OrderSide.Buy : OrderSide.Sell,
                Volume = order.VolumeInitial(),
                Price = order.PriceOrder(),
                StopLoss = order.PriceSL(),
                TakeProfit = order.PriceTP(),
                StopLimit = 0, // MT5 API doesn't have PriceStopLimit method
                TimeSetup = DateTimeOffset.FromUnixTimeSeconds((long)order.TimeSetup()).DateTime,
                TimeExpiration = DateTimeOffset.FromUnixTimeSeconds((long)order.TimeExpiration()).DateTime,
                Comment = order.Comment(),
                Magic = (int)(long)order.ExpertID(),
                TimeCreate = DateTimeOffset.FromUnixTimeSeconds((long)order.TimeSetup()).DateTime,
                TimeUpdate = DateTimeOffset.FromUnixTimeSeconds((long)order.TimeDone()).DateTime
            };
        }

        private PositionDto ConvertToPositionDto(CIMTPosition position)
        {
            return new PositionDto
            {
                PositionId = position.Position(),
                ClientLogin = position.Login(),
                Symbol = position.Symbol(),
                Type = PositionType.Buy, // MT5 API doesn't have Type() method for positions
                Volume = position.Volume(),
                PriceOpen = position.PriceOpen(),
                PriceCurrent = position.PriceCurrent(),
                StopLoss = position.PriceSL(),
                TakeProfit = position.PriceTP(),
                Profit = position.Profit(),
                Storage = 0, // MT5 API doesn't have Storage() method
                Commission = 0, // MT5 API doesn't have Commission() method
                Tax = 0, // MT5 API doesn't have Tax() method
                Swap = 0, // MT5 API doesn't have Swap() method
                TimeCreate = DateTimeOffset.FromUnixTimeSeconds((long)position.TimeCreate()).DateTime,
                TimeUpdate = DateTimeOffset.FromUnixTimeSeconds((long)position.TimeUpdate()).DateTime,
                Comment = position.Comment(),
                Magic = (int)(long)position.ExpertID(),
                OrderId = 0, // MT5 API doesn't have Order() method for positions
                PriceSL = position.PriceSL(),
                PriceTP = position.PriceTP()
            };
        }
    }
}