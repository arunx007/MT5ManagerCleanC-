using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.AspNetCore.Authorization;
using MT5Wrapper.UserManagement.Interfaces;
using MT5Wrapper.Trading.Interfaces;
using MT5Wrapper.Trading.DTOs;

namespace MT5Wrapper.Controllers
{
    /// <summary>
    /// Reports Controller - Handles account statements, trading reports, and analytics
    /// Provides business intelligence and reporting capabilities
    /// </summary>
    [ApiController]
    [Route("api/reports")]
    [Authorize] // All reports require authentication
    public class ReportsController : ControllerBase
    {
        private readonly IUserManagementService _userManagementService;
        private readonly ITradingService _tradingService;

        public ReportsController(
            IUserManagementService userManagementService,
            ITradingService tradingService)
        {
            _userManagementService = userManagementService ?? throw new ArgumentNullException(nameof(userManagementService));
            _tradingService = tradingService ?? throw new ArgumentNullException(nameof(tradingService));
        }

        /// <summary>
        /// Get account statement for a user
        /// </summary>
        [HttpGet("statements/{login}")]
        public async Task<IActionResult> GetAccountStatement(ulong login, [FromQuery] DateTime? from, [FromQuery] DateTime? to)
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                // Set default date range (last 30 days)
                var startDate = from ?? DateTime.UtcNow.AddDays(-30);
                var endDate = to ?? DateTime.UtcNow;

                // Get user account information
                var userAccount = await _userManagementService.GetUserAccountAsync(tenantId, login);
                if (userAccount == null)
                    return NotFound(new { error = "User not found", login = login });

                // Get trade history
                var managerToken = GetManagerTokenFromJwt();
                var trades = await _tradingService.GetTradeHistoryAsync(managerToken, login, startDate, endDate);

                // Generate statement
                var statement = new AccountStatement
                {
                    Login = login,
                    Period = new DateRange { From = startDate, To = endDate },
                    OpeningBalance = userAccount.Balance, // This would need historical balance tracking
                    ClosingBalance = userAccount.Balance,
                    TotalTrades = trades.Count(),
                    TotalProfit = CalculateTotalProfit(trades),
                    TotalCommission = 0, // Would need to calculate from trades
                    TotalSwaps = 0, // Would need to calculate from trades
                    Trades = trades,
                    GeneratedAt = DateTimeOffset.UtcNow
                };

                return Ok(statement);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get trading performance report
        /// </summary>
        [HttpGet("trading-performance/{login}")]
        public async Task<IActionResult> GetTradingPerformance(ulong login, [FromQuery] DateTime? from, [FromQuery] DateTime? to)
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                // Set default date range (last 30 days)
                var startDate = from ?? DateTime.UtcNow.AddDays(-30);
                var endDate = to ?? DateTime.UtcNow;

                // Get trade history
                var managerToken = GetManagerTokenFromJwt();
                var trades = await _tradingService.GetTradeHistoryAsync(managerToken, login, startDate, endDate);

                // Calculate performance metrics
                var performance = new TradingPerformance
                {
                    Login = login,
                    Period = new DateRange { From = startDate, To = endDate },
                    TotalTrades = trades.Count(),
                    WinningTrades = trades.Count(t => t.Profit > 0),
                    LosingTrades = trades.Count(t => t.Profit < 0),
                    TotalProfit = CalculateTotalProfit(trades),
                    TotalLoss = CalculateTotalLoss(trades),
                    ProfitFactor = CalculateProfitFactor(trades),
                    WinRate = trades.Count() > 0 ? (double)trades.Count(t => t.Profit > 0) / trades.Count() : 0,
                    AverageWin = CalculateAverageWin(trades),
                    AverageLoss = CalculateAverageLoss(trades),
                    LargestWin = FindLargestWin(trades),
                    LargestLoss = FindLargestLoss(trades),
                    GeneratedAt = DateTimeOffset.UtcNow
                };

                return Ok(performance);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        /// <summary>
        /// Get trading analytics and statistics
        /// </summary>
        [HttpGet("analytics")]
        public async Task<IActionResult> GetTradingAnalytics([FromQuery] DateTime? from, [FromQuery] DateTime? to)
        {
            try
            {
                // Extract tenant ID from JWT claims
                var tenantId = GetTenantIdFromJwt();
                if (string.IsNullOrEmpty(tenantId))
                    return Unauthorized(new { error = "Authentication required", message = "Valid tenant context required" });

                // Set default date range (last 7 days)
                var startDate = from ?? DateTime.UtcNow.AddDays(-7);
                var endDate = to ?? DateTime.UtcNow;

                // Get all users for the tenant
                var users = await _userManagementService.GetUsersAsync(tenantId);
                var managerToken = GetManagerTokenFromJwt();

                // Collect analytics data
                var analytics = new TradingAnalytics
                {
                    Period = new DateRange { From = startDate, To = endDate },
                    TotalUsers = users.Count(),
                    ActiveUsers = 0, // Would need to track active users
                    TotalTrades = 0,
                    TotalVolume = 0,
                    TotalProfit = 0,
                    TopPerformers = new List<UserPerformance>(),
                    GeneratedAt = DateTimeOffset.UtcNow
                };

                // Calculate aggregate statistics
                foreach (var user in users)
                {
                    var trades = await _tradingService.GetTradeHistoryAsync(managerToken, user.Login, startDate, endDate);
                    if (trades.Count() > 0)
                    {
                        analytics.ActiveUsers++;
                        analytics.TotalTrades += trades.Count();
                        analytics.TotalProfit += CalculateTotalProfit(trades);

                        // Add to top performers
                        analytics.TopPerformers.Add(new UserPerformance
                        {
                            Login = user.Login,
                            Name = user.Name,
                            TotalTrades = trades.Count(),
                            TotalProfit = CalculateTotalProfit(trades),
                            WinRate = trades.Count() > 0 ? (double)trades.Count(t => t.Profit > 0) / trades.Count() : 0
                        });
                    }
                }

                // Sort top performers by profit
                analytics.TopPerformers = analytics.TopPerformers
                    .OrderByDescending(p => p.TotalProfit)
                    .Take(10)
                    .ToList();

                return Ok(analytics);
            }
            catch (Exception ex)
            {
                return StatusCode(500, new { error = "Internal server error", message = ex.Message });
            }
        }

        // Helper methods for calculations
        private double CalculateTotalProfit(IEnumerable<TradeDto> trades)
        {
            return trades.Sum(t => Math.Max(0, t.Profit));
        }

        private double CalculateTotalLoss(IEnumerable<TradeDto> trades)
        {
            return Math.Abs(trades.Sum(t => Math.Min(0, t.Profit)));
        }

        private double CalculateProfitFactor(IEnumerable<TradeDto> trades)
        {
            var profit = CalculateTotalProfit(trades);
            var loss = CalculateTotalLoss(trades);
            return loss > 0 ? profit / loss : profit > 0 ? double.PositiveInfinity : 0;
        }

        private double CalculateAverageWin(IEnumerable<TradeDto> trades)
        {
            var winningTrades = trades.Where(t => t.Profit > 0).ToList();
            return winningTrades.Count > 0 ? winningTrades.Average(t => t.Profit) : 0;
        }

        private double CalculateAverageLoss(IEnumerable<TradeDto> trades)
        {
            var losingTrades = trades.Where(t => t.Profit < 0).ToList();
            return losingTrades.Count > 0 ? Math.Abs(losingTrades.Average(t => t.Profit)) : 0;
        }

        private double FindLargestWin(IEnumerable<TradeDto> trades)
        {
            return trades.Max(t => t.Profit);
        }

        private double FindLargestLoss(IEnumerable<TradeDto> trades)
        {
            return Math.Abs(trades.Min(t => t.Profit));
        }

        // Helper methods to extract data from JWT tokens
        private string GetTenantIdFromJwt()
        {
            // Extract tenant ID from JWT claims
            var tenantId = User.FindFirst("tenant_id")?.Value ??
                          User.FindFirst("manager_id")?.Value ??
                          "test_mgr_001"; // Default fallback
            return tenantId;
        }

        private string GetManagerTokenFromJwt()
        {
            // For now, return a placeholder. In production, extract from JWT claims
            return User.FindFirst("manager_token")?.Value ?? "manager_token_placeholder";
        }
    }

    // Report DTOs
    public class AccountStatement
    {
        public ulong Login { get; set; }
        public DateRange Period { get; set; } = new();
        public double OpeningBalance { get; set; }
        public double ClosingBalance { get; set; }
        public int TotalTrades { get; set; }
        public double TotalProfit { get; set; }
        public double TotalCommission { get; set; }
        public double TotalSwaps { get; set; }
        public IEnumerable<TradeDto> Trades { get; set; } = new List<TradeDto>();
        public DateTimeOffset GeneratedAt { get; set; }
    }

    public class TradingPerformance
    {
        public ulong Login { get; set; }
        public DateRange Period { get; set; } = new();
        public int TotalTrades { get; set; }
        public int WinningTrades { get; set; }
        public int LosingTrades { get; set; }
        public double TotalProfit { get; set; }
        public double TotalLoss { get; set; }
        public double ProfitFactor { get; set; }
        public double WinRate { get; set; }
        public double AverageWin { get; set; }
        public double AverageLoss { get; set; }
        public double LargestWin { get; set; }
        public double LargestLoss { get; set; }
        public DateTimeOffset GeneratedAt { get; set; }
    }

    public class TradingAnalytics
    {
        public DateRange Period { get; set; } = new();
        public int TotalUsers { get; set; }
        public int ActiveUsers { get; set; }
        public int TotalTrades { get; set; }
        public double TotalVolume { get; set; }
        public double TotalProfit { get; set; }
        public List<UserPerformance> TopPerformers { get; set; } = new();
        public DateTimeOffset GeneratedAt { get; set; }
    }

    public class UserPerformance
    {
        public ulong Login { get; set; }
        public string Name { get; set; } = "";
        public int TotalTrades { get; set; }
        public double TotalProfit { get; set; }
        public double WinRate { get; set; }
    }

    public class DateRange
    {
        public DateTime From { get; set; }
        public DateTime To { get; set; }
    }
}