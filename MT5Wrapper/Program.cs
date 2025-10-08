using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using MT5Wrapper.Core.Services;
using MT5Wrapper.Core.Models;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Authentication.Services;
using MT5Wrapper.Authentication.Interfaces;
using MT5Wrapper.MultiTenant.Services;
using MT5Wrapper.MultiTenant.Interfaces;
using MT5Wrapper.TickData.Services;
using MT5Wrapper.UserManagement.Services;
using MT5Wrapper.UserManagement.Interfaces;
using MT5Wrapper.Trading.Services;
using MT5Wrapper.Trading.Interfaces;
using MT5Wrapper.OrderManagement.Services;
using MT5Wrapper.OrderManagement.Interfaces;
using MT5Wrapper.MarketData.Services;
using MT5Wrapper.MarketData.Interfaces;
using MT5Wrapper.WebSocket.Services;
using MT5Wrapper.WebSocket.Interfaces;
using MT5Wrapper.Hubs;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.IdentityModel.Tokens;
using System.Text;
using System;
using System.Threading.Tasks;

var builder = WebApplication.CreateBuilder(args);

// Add services to the container.
builder.Services.AddControllers();
builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen();

// Configure JWT Authentication
builder.Services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
    .AddJwtBearer(options =>
    {
        options.TokenValidationParameters = new TokenValidationParameters
        {
            ValidateIssuer = true,
            ValidateAudience = true,
            ValidateLifetime = true,
            ValidateIssuerSigningKey = true,
            ValidIssuer = builder.Configuration["JwtSettings:Issuer"],
            ValidAudience = builder.Configuration["JwtSettings:Audience"],
            IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(builder.Configuration["JwtSettings:SecretKey"]!))
        };
    });

// Add SignalR for real-time WebSocket communication
builder.Services.AddSignalR();

// Configure MT5Wrapper configuration
builder.Services.Configure<MT5WrapperConfig>(options =>
{
    // Bind MT5 settings
    builder.Configuration.GetSection("MT5Settings").Bind(options.MT5);

    // Bind Authentication settings
    options.Authentication.Jwt.SecretKey = builder.Configuration["JwtSettings:SecretKey"]!;
    options.Authentication.Jwt.Issuer = builder.Configuration["JwtSettings:Issuer"]!;
    options.Authentication.Jwt.Audience = builder.Configuration["JwtSettings:Audience"]!;
    options.Authentication.ManagerTokenExpiryDays = builder.Configuration.GetValue<int>("JwtSettings:ManagerTokenExpiryHours") / 24;
    options.Authentication.ClientTokenExpiryDays = builder.Configuration.GetValue<int>("JwtSettings:ClientTokenExpiryHours");

    // Bind MultiTenant settings
    builder.Configuration.GetSection("MultiTenant").Bind(options.MultiTenant);
});

// Register services
builder.Services.AddSingleton<IMT5ConnectionService, MultiManagerConnectionService>();
builder.Services.AddScoped<IManagerAuthService, ManagerAuthService>();
builder.Services.AddScoped<IClientAuthService, ClientAuthService>();
builder.Services.AddScoped<IMarketDataService, MarketDataService>();
builder.Services.AddScoped<IUserManagementService, UserManagementService>();
builder.Services.AddScoped<ITenantManagerService, TenantManagerService>();
builder.Services.AddScoped<ITenantRoutingService, TenantRoutingService>();
builder.Services.AddScoped<ITradingService, TradingService>();
builder.Services.AddScoped<IOrderManagementService, OrderManagementService>();
builder.Services.AddSingleton<IOrderBookService, OrderBookService>();
builder.Services.AddSingleton<IPositionService, PositionService>();

// Configure logging
builder.Logging.ClearProviders();
builder.Logging.AddConsole();
builder.Logging.SetMinimumLevel(LogLevel.Information);

// Configure CORS for React Native and web clients
builder.Services.AddCors(options =>
{
    options.AddPolicy("AllowAll", policy =>
    {
        policy.AllowAnyOrigin()
              .AllowAnyMethod()
              .AllowAnyHeader();
    });

    // SignalR CORS policy - allow specific origins for credentials
    options.AddPolicy("SignalR", policy =>
    {
        policy.WithOrigins("http://localhost:3000", "http://localhost:5000", "http://127.0.0.1:3000", "http://127.0.0.1:5000")
              .AllowAnyMethod()
              .AllowAnyHeader()
              .AllowCredentials();
    });
});

var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

app.UseRouting();
app.UseAuthentication();
app.UseCors("AllowAll");
app.UseAuthorization();

// Enable static file serving
app.UseStaticFiles();

app.MapControllers();

// Map SignalR hub for real-time market data
app.MapHub<MarketDataHub>("/hubs/marketdata").RequireCors("SignalR");

// Configure URL explicitly
app.Urls.Add("http://localhost:5001");

Console.WriteLine("🚀 MT5 Wrapper API Server Starting...");
Console.WriteLine("=====================================");
Console.WriteLine($"🌐 Server will be available at: http://localhost:5001");
Console.WriteLine($"📚 Swagger UI: http://localhost:5001/swagger");
Console.WriteLine($"🔗 API Base URL: http://localhost:5001/api");
Console.WriteLine();
Console.WriteLine("📋 Available Endpoints:");
Console.WriteLine("  🔐 AUTHENTICATION:");
Console.WriteLine("    POST /api/auth/manager/login - Manager authentication (30-day tokens)");
Console.WriteLine("    POST /api/auth/client/login - Client authentication (unlimited tokens)");
Console.WriteLine("  👥 USER MANAGEMENT:");
Console.WriteLine("    GET /api/users - User management");
Console.WriteLine("    POST /api/users - Create user");
Console.WriteLine("  📊 MARKET DATA:");
Console.WriteLine("    GET /api/marketdata/symbols - Available symbols");
Console.WriteLine("    GET /api/marketdata/tick/{symbol} - Current tick data");
Console.WriteLine("    POST /api/marketdata/chart - OHLC chart data");
Console.WriteLine("    GET /api/orderbook/{symbol} - Order book snapshot");
Console.WriteLine("    POST /api/orderbook/subscribe/{symbol} - Subscribe to order book");
Console.WriteLine("    POST /api/orderbook/unsubscribe/{symbol} - Unsubscribe from order book");
Console.WriteLine("    GET /api/orderbook/status - Order book service status");
Console.WriteLine("  💼 TRADING OPERATIONS:");
Console.WriteLine("    POST /api/trading/place-order - Place new order");
Console.WriteLine("    PUT /api/trading/modify-order/{id} - Modify order");
Console.WriteLine("    DELETE /api/trading/cancel-order/{id} - Cancel order");
Console.WriteLine("    GET /api/trading/orders/{login} - Get orders");
Console.WriteLine("    GET /api/trading/positions/{login} - Get positions");
Console.WriteLine("    DELETE /api/trading/close-position/{id} - Close position");
Console.WriteLine("  📋 ORDER MANAGEMENT:");
Console.WriteLine("    POST /api/orders/market - Create market order");
Console.WriteLine("    POST /api/orders/pending - Create pending order");
Console.WriteLine("    PUT /api/orders/modify - Modify order");
Console.WriteLine("    DELETE /api/orders/cancel - Cancel order");
Console.WriteLine("    GET /api/orders/ticket/{id} - Get order by ticket");
Console.WriteLine("    GET /api/orders/history/{login} - Order history");
Console.WriteLine("  🏢 ADMINISTRATIVE:");
Console.WriteLine("    GET /api/admin/groups - Get groups");
Console.WriteLine("    POST /api/admin/groups - Create group");
Console.WriteLine("    PUT /api/admin/groups - Update group");
Console.WriteLine("    GET /api/admin/groups/{name}/users - Group users");
Console.WriteLine("    GET /api/admin/stats - System statistics");
Console.WriteLine("  📈 REPORTING:");
Console.WriteLine("    GET /api/reports/statements/{login} - Account statements");
Console.WriteLine("    GET /api/reports/trading-performance/{login} - Performance reports");
Console.WriteLine("    GET /api/reports/analytics - Trading analytics");
Console.WriteLine("  🌐 WEBSOCKET:");
Console.WriteLine("    POST /api/websocket/subscribe/{symbol} - Subscribe to symbol");
Console.WriteLine("    POST /api/websocket/unsubscribe/{symbol} - Unsubscribe from symbol");
Console.WriteLine("    GET /api/websocket/status - Connection status");
Console.WriteLine("  🏥 SYSTEM:");
Console.WriteLine("    GET /api/health - Health check");
Console.WriteLine();
Console.WriteLine("🎯 Multi-tenant: Each manager gets isolated services");
Console.WriteLine("🔐 Manager tokens: 30-day expiration, full client access");
Console.WriteLine("📱 Client tokens: Unlimited expiration for mobile apps");
Console.WriteLine();

// Establish MT5 connection on startup
var mt5ConnectionService = app.Services.GetRequiredService<IMT5ConnectionService>();
_ = Task.Run(async () =>
{
    var connectionConfig = new MT5ConnectionConfig
    {
        Server = builder.Configuration["MT5Settings:DefaultServer"],
        Login = builder.Configuration["MT5Settings:DefaultManagerLogin"],
        Password = builder.Configuration["MT5Settings:DefaultManagerPassword"],
        ManagerId = "default_manager",
        EnableAutoReconnect = true,
        AutoReconnectInterval = 30
    };

    var result = await mt5ConnectionService.ConnectAsync(connectionConfig);
    if (result.Success)
    {
        Console.WriteLine("✅ MT5 connection established successfully");
    }
    else
    {
        Console.WriteLine($"❌ MT5 connection failed: {result.Message}");
    }
});

app.Run();