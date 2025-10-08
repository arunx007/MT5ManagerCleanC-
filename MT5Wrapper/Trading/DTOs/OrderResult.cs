using System;
using System.Collections.Generic;

namespace MT5Wrapper.Trading.DTOs
{
    public class OrderResult
    {
        public bool Success { get; set; }
        public string Message { get; set; } = "";
        public ulong OrderId { get; set; }
        public ulong ClientLogin { get; set; }
        public OrderDto? Order { get; set; }
        public MTRetCode RetCode { get; set; }
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
    }

    public class PositionResult
    {
        public bool Success { get; set; }
        public string Message { get; set; } = "";
        public ulong PositionId { get; set; }
        public ulong ClientLogin { get; set; }
        public PositionDto? Position { get; set; }
        public MTRetCode RetCode { get; set; }
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
    }

    public class BulkOrderResult
    {
        public int TotalRequests { get; set; }
        public int SuccessfulOrders { get; set; }
        public int FailedOrders { get; set; }
        public List<OrderResult> Results { get; set; } = new();
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
    }

    public class BulkPositionResult
    {
        public int TotalRequests { get; set; }
        public int SuccessfulClosures { get; set; }
        public int FailedClosures { get; set; }
        public List<PositionResult> Results { get; set; } = new();
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;
    }

    public enum MTRetCode
    {
        MT_RET_OK = 0,
        MT_RET_ERR_PARAMS = 1,
        MT_RET_ERR_DATA = 2,
        MT_RET_ERR_NOTFOUND = 3,
        MT_RET_ERR_EXISTS = 4,
        MT_RET_ERR_ACCESS = 5,
        MT_RET_ERR_TIMEOUT = 6,
        MT_RET_ERR_CONNECTION = 7,
        MT_RET_ERR_EXTERNAL = 8,
        MT_RET_ERR_RETCODE = 9
    }
}