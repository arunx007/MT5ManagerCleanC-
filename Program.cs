using System;
using System.Threading.Tasks;
using Microsoft.AspNetCore.SignalR.Client;
using System.Threading;

class Program
{
    static async Task Main(string[] args)
    {
        Console.WriteLine("🧪 MT5Wrapper WebSocket Test Starting...");

        // JWT token from previous testing
        string jwtToken = "Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJkZWZhdWx0X21hbmFnZXIiLCJqdGkiOiI1NGZkNTI1OC00Y2RjLTQwNjgtYmM1NS02NDllNGU4YzY5Y2QiLCJtYW5hZ2VyX2lkIjoiZGVmYXVsdF9tYW5hZ2VyIiwibG9naW4iOiIxMDA2Iiwic2VydmVyIjoiODYuMTA0LjI1MS4xNjU6NDQzIiwicGVybWlzc2lvbnMiOiJ7XCJDYW5BY2Nlc3NBbGxBY2NvdW50c1wiOnRydWUsXCJDYW5Nb2RpZnlVc2Vyc1wiOnRydWUsXCJDYW5Nb2RpZnlHcm91cHNcIjp0cnVlLFwiQ2FuQWNjZXNzVHJhZGluZ0RhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzTGl2ZURhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzSGlzdG9yaWNhbERhdGFcIjp0cnVlfSIsInRva2VuX3R5cGUiOiJtYW5hZ2VyIiwiZXhwaXJ5X2RheXMiOiIzMCIsImV4cCI6MTc2MjQzMzU0Mn0.aNhDMrDdgovhQsvdCLSwiAnwg2TYLG7CtAyzq8ZjDB0";

        try
        {
            Console.WriteLine("🔗 Connecting to SignalR hub...");

            var connection = new HubConnectionBuilder()
                .WithUrl("http://localhost:5001/hubs/marketdata", options =>
                {
                    options.Headers.Add("Authorization", jwtToken);
                })
                .WithAutomaticReconnect()
                .Build();

            // Set up event handlers
            connection.On("Connected", (object data) =>
            {
                Console.WriteLine($"✅ Connected: {data}");
            });

            connection.On("Subscribed", (object data) =>
            {
                Console.WriteLine($"📡 Subscribed: {data}");
            });

            connection.On("TickData", (object data) =>
            {
                Console.WriteLine($"📊 TickData: {data}");
            });

            connection.On("Error", (object data) =>
            {
                Console.WriteLine($"❌ Error: {data}");
            });

            // Handle connection events
            connection.Closed += async (error) =>
            {
                Console.WriteLine($"🔌 Connection closed: {error?.Message}");
                await Task.Delay(1000);
            };

            connection.Reconnecting += (error) =>
            {
                Console.WriteLine($"🔄 Reconnecting: {error?.Message}");
                return Task.CompletedTask;
            };

            connection.Reconnected += (connectionId) =>
            {
                Console.WriteLine($"✅ Reconnected: {connectionId}");
                return Task.CompletedTask;
            };

            // Start connection
            await connection.StartAsync();
            Console.WriteLine("🚀 SignalR connection established!");

            // Test ping
            Console.WriteLine("🏓 Testing ping...");
            await connection.InvokeAsync("Ping");

            // Subscribe to EURUSD ticks
            Console.WriteLine("📈 Subscribing to EURUSD ticks...");
            await connection.InvokeAsync("SubscribeToTicks", "EURUSD");

            // Wait for data
            Console.WriteLine("⏳ Waiting for tick data (15 seconds)...");
            await Task.Delay(15000);

            // Unsubscribe
            Console.WriteLine("🛑 Unsubscribing from EURUSD...");
            await connection.InvokeAsync("UnsubscribeFromTicks", "EURUSD");

            // Wait a bit more
            await Task.Delay(2000);

            // Stop connection
            await connection.StopAsync();
            Console.WriteLine("✅ Test completed successfully!");

        }
        catch (Exception ex)
        {
            Console.WriteLine($"❌ Test failed: {ex.Message}");
            Console.WriteLine($"Stack trace: {ex.StackTrace}");
        }
    }
}