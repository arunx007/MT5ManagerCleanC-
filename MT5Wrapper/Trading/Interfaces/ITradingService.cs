using System;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;
using MT5Wrapper.Trading.DTOs;

namespace MT5Wrapper.Trading.Interfaces
{
    public interface ITradingService
    {
        // Order Management - Manager can place orders for any client
        Task<OrderResult> PlaceOrderAsync(string managerToken, ulong clientLogin, OrderRequest request);
        Task<OrderResult> ModifyOrderAsync(string managerToken, ulong clientLogin, ulong orderId, ModifyOrderRequest request);
        Task<OrderResult> CancelOrderAsync(string managerToken, ulong clientLogin, ulong orderId);
        Task<IEnumerable<OrderDto>> GetOrdersAsync(string managerToken, ulong clientLogin);

        // Position Management - Manager can modify/close any client position
        Task<PositionResult> ClosePositionAsync(string managerToken, ulong clientLogin, ulong positionId);
        Task<PositionResult> ModifyPositionAsync(string managerToken, ulong clientLogin, ulong positionId, ModifyPositionRequest request);
        Task<PositionResult> CloseAllPositionsAsync(string managerToken, ulong clientLogin, string symbol = null);
        Task<IEnumerable<PositionDto>> GetPositionsAsync(string managerToken, ulong clientLogin);

        // Trade History - Manager can access all client trade history
        Task<IEnumerable<TradeDto>> GetTradeHistoryAsync(string managerToken, ulong clientLogin, DateTime from, DateTime to);
        Task<IEnumerable<TradeDto>> GetRecentTradesAsync(string managerToken, ulong clientLogin, int count = 50);

        // Account Trading Info - Manager can see all trading details
        Task<AccountTradingInfoDto> GetAccountTradingInfoAsync(string managerToken, ulong clientLogin);
        Task<MarginInfoDto> GetMarginInfoAsync(string managerToken, ulong clientLogin);

        // Bulk Operations - Manager can perform operations on multiple clients/accounts
        Task<BulkOrderResult> PlaceBulkOrdersAsync(string managerToken, IEnumerable<BulkOrderRequest> requests);
        Task<BulkPositionResult> CloseBulkPositionsAsync(string managerToken, IEnumerable<BulkPositionRequest> requests);
    }
}