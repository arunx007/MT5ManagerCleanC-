using System.Collections.Generic;
using System.Threading.Tasks;
using System.Collections.Generic;
using System.Threading.Tasks;
using MT5Wrapper.OrderManagement.DTOs;

namespace MT5Wrapper.OrderManagement.Interfaces
{
    public interface IOrderManagementService
    {
        // Order Creation & Management
        Task<OrderOperationResult> CreateOrderAsync(CreateOrderRequest request);
        Task<OrderOperationResult> ModifyOrderAsync(ModifyOrderRequest request);
        Task<OrderOperationResult> CancelOrderAsync(CancelOrderRequest request);
        Task<OrderOperationResult> CloseOrderAsync(CloseOrderRequest request);

        // Order Retrieval
        Task<OrderDto> GetOrderAsync(ulong ticket);
        Task<IEnumerable<OrderDto>> GetOpenOrdersAsync(ulong login);
        Task<IEnumerable<OrderDto>> GetOrderHistoryAsync(ulong login, long from, long to);
        Task<IEnumerable<OrderDto>> GetOrdersByGroupAsync(string group);
        Task<IEnumerable<OrderDto>> GetOrdersByLoginsAsync(IEnumerable<ulong> logins);

        // Pending Orders Management
        Task<OrderOperationResult> CreatePendingOrderAsync(CreatePendingOrderRequest request);
        Task<OrderOperationResult> ModifyPendingOrderAsync(ModifyPendingOrderRequest request);
        Task<OrderOperationResult> CancelPendingOrderAsync(CancelPendingOrderRequest request);

        // Batch Operations
        Task<IEnumerable<OrderOperationResult>> CreateOrdersBatchAsync(IEnumerable<CreateOrderRequest> requests);
        Task<IEnumerable<OrderOperationResult>> CancelOrdersBatchAsync(IEnumerable<CancelOrderRequest> requests);

        // Order Validation
        Task<OrderValidationResult> ValidateOrderAsync(CreateOrderRequest request);
        Task<OrderValidationResult> ValidatePendingOrderAsync(CreatePendingOrderRequest request);
    }
}