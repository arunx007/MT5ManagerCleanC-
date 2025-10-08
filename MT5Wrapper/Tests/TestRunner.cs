// Clean Test Runner for MT5 API

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Logging.Console;
using MetaQuotes.MT5ManagerAPI;

namespace Tests
{
    public class TestRunner
{
    public static async Task RunTests(string[] args)
    {
        Console.WriteLine("🧪 MT5 API Test Runner");
        Console.WriteLine("======================");
        Console.WriteLine("");

        // Setup configuration
        var config = new ConfigurationBuilder()
            .AddJsonFile("Config/TestConfiguration.json")
            .Build();

        // Setup logging
        using var loggerFactory = LoggerFactory.Create(builder =>
        {
            builder.AddConsole();
            builder.SetMinimumLevel(LogLevel.Debug);
        });

        var logger = loggerFactory.CreateLogger<MT5Test>();

        // Create test instance
        var test = new MT5Test(logger, config);

        // Run comprehensive API tests
        var results = new List<TestResult>();

        Console.WriteLine("🧪 Running Comprehensive MT5 API Test Suite");
        Console.WriteLine("==========================================");

        // Test 1: Basic MT5 SDK integration
        Console.WriteLine("\n1️⃣ Testing basic MT5 SDK integration...");
        var basicTest = await test.TestConnection();
        results.Add(basicTest);

        if (basicTest.Passed)
        {
            Console.WriteLine("\n✅ Basic SDK test passed! Testing with your credentials...");
            var credentialTest = await test.TestWithCredentials();
            results.Add(credentialTest);

            if (credentialTest.Passed)
            {
                Console.WriteLine("\n🎯 Testing Individual APIs...");

                // Test 2: Authentication APIs
                Console.WriteLine("\n2️⃣ Testing Manager Authentication API...");
                var managerAuthTest = await test.TestManagerAuthentication();
                results.Add(managerAuthTest);

                Console.WriteLine("\n3️⃣ Testing Client Authentication API...");
                var clientAuthTest = await test.TestClientAuthentication();
                results.Add(clientAuthTest);

                // Test 3: Multi-tenant APIs
                Console.WriteLine("\n4️⃣ Testing Multi-tenant Management API...");
                var tenantTest = await test.TestTenantManagement();
                results.Add(tenantTest);

                // Test 4: User Management APIs
                Console.WriteLine("\n5️⃣ Testing User Management API...");
                var userManagementTest = await test.TestUserManagement();
                results.Add(userManagementTest);

                // Test 5: Tick Data APIs
                Console.WriteLine("\n6️⃣ Testing Tick Data API...");
                var tickDataTest = await test.TestTickData();
                results.Add(tickDataTest);

                // Test 6: Symbol APIs
                Console.WriteLine("\n7️⃣ Testing Symbol Management API...");
                var symbolTest = await test.TestSymbolManagement();
                results.Add(symbolTest);

                // Test 7: Group APIs
                Console.WriteLine("\n8️⃣ Testing Group Management API...");
                var groupTest = await test.TestGroupManagement();
                results.Add(groupTest);

                // Test 8: Trading APIs - Manager Full Access
                Console.WriteLine("\n9️⃣ Testing Trading API (Manager Full Access)...");
                var tradingTest = await test.TestTradingOperations();
                results.Add(tradingTest);

                // Test 9: Market Data APIs - Charts & Quotes
                Console.WriteLine("\n🔟 Testing Market Data API (Charts & Quotes)...");
                var marketDataTest = await test.TestMarketData();
                results.Add(marketDataTest);

                // Test 10: Order Management APIs - Manager Full Access
                Console.WriteLine("\n1️⃣1️⃣ Testing Order Management API (Manager Full Access)...");
                var orderManagementTest = await test.TestOrderManagement();
                results.Add(orderManagementTest);
            }
        }

        // Cleanup connection after all tests
        try
        {
            if (test.GetType().GetField("_manager", System.Reflection.BindingFlags.NonPublic | System.Reflection.BindingFlags.Instance)?.GetValue(test) is CIMTManagerAPI manager && manager != null)
            {
                manager.Disconnect();
                manager.Release();
                SMTManagerAPIFactory.Shutdown();
                Console.WriteLine("\n🔌 MT5 connection cleaned up");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"\n⚠️ Cleanup warning: {ex.Message}");
        }

        // Show results
        Console.WriteLine("\n📊 Test Results:");
        Console.WriteLine("===============");

        foreach (var result in results)
        {
            var status = result.Passed ? "✅ PASS" : "❌ FAIL";
            Console.WriteLine($"{status} - {result.TestName}: {result.Message}");
        }

        var passed = results.Count(r => r.Passed);
        var total = results.Count;

        Console.WriteLine($"\n📈 Summary: {passed}/{total} tests passed ({(total > 0 ? (passed * 100 / total) : 0)}%)");

        if (passed == total)
        {
            Console.WriteLine("\n🎉 All tests passed! Your MT5 API is ready!");
            Console.WriteLine("🚀 You can now proceed with full implementation.");
        }
        else
        {
            Console.WriteLine("\n⚠️ Some tests failed. Check the MT5 SDK setup.");
            Console.WriteLine("📖 See Config/TestConfiguration.json for troubleshooting.");
        }

        Console.WriteLine("\nTest completed!");
        // Console.ReadKey(); // Commented out for VSCode terminal compatibility
    }
}

public class TestResult
{
    public bool Passed { get; set; }
    public string TestName { get; set; } = "";
    public string Message { get; set; } = "";
    public DateTime Timestamp { get; set; } = DateTime.UtcNow;

    public static TestResult CreatePassed(string testName, string message)
    {
        return new TestResult { Passed = true, TestName = testName, Message = message };
    }

    public static TestResult CreateFailed(string testName, string message)
    {
        return new TestResult { Passed = false, TestName = testName, Message = message };
    }
    }
}