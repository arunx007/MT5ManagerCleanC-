using System;
using System.Collections.Generic;

namespace MT5Wrapper.OrderManagement.DTOs
{
    public class OrderOperationResult
    {
        public bool Success { get; set; }
        public string Message { get; set; } = string.Empty;
        public ulong Ticket { get; set; }
        public string ErrorCode { get; set; } = string.Empty;
        public DateTimeOffset Timestamp { get; set; } = DateTimeOffset.UtcNow;
        public OrderDto? Order { get; set; }
    }

    public class OrderValidationResult
    {
        public bool IsValid { get; set; }
        public string Message { get; set; } = string.Empty;
        public List<string> Errors { get; set; } = new List<string>();
        public List<string> Warnings { get; set; } = new List<string>();
        public double RequiredMargin { get; set; }
        public double FreeMargin { get; set; }
    }
}