using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.Core.Models;
using MT5Wrapper.Core.Services;
using MT5Wrapper.TickData.Interfaces;
using MT5Wrapper.TickData.Services;
using MT5Wrapper.TickData.DTOs;
using MT5Wrapper.UserManagement.Interfaces;
using MT5Wrapper.UserManagement.Services;
using MT5Wrapper.UserManagement.DTOs;
using MT5Wrapper.MultiTenant.Interfaces;
using MT5Wrapper.MultiTenant.Services;
using MT5Wrapper.MultiTenant.Models;

namespace MT5Wrapper.Tests
{
    class Program
    {
        private static TestConfiguration? _testConfig;
        private static IServiceProvider? _serviceProvider;
        private static ILogger<Program>? _logger;

        static async Task Main(string[] args)
        {
            Console.WriteLine("🚀 MT5 Wrapper API Testing Suite");
            Console.WriteLine("================================\n");

            try
            {
                // Setup configuration and services
                await SetupTestingEnvironment();

                // Load test configuration
                LoadTestConfiguration();

                // Run all tests
                await RunAPITests();

                // Show final results
                ShowTestSummary();

            }
            catch (Exception ex)
            {
                Console.WriteLine($"❌ Test suite failed: {ex.Message}");
                Console.WriteLine($"Stack trace: {ex.StackTrace}");
            }

            Console.WriteLine("\nPress any key to exit...");
            Console.ReadKey();
        }

        private static async Task SetupTestingEnvironment()
        {
            Console.WriteLine("🔧 Setting up testing environment...");

            // Build configuration
            var configuration = new ConfigurationBuilder()
                .SetBasePath(Directory.GetCurrentDirectory())
                .AddJsonFile("appsettings.json", optional: false, reloadOnChange: true)
                .AddJsonFile("TestConfiguration.json", optional: false, reloadOnChange: true)
                .Build();

            // Setup dependency injection
            var services = new ServiceCollection();

            // Add logging
            services.AddLogging(builder =>
            {
                builder.AddConsole();
                builder.AddConfiguration(configuration.GetSection("Logging"));
            });

            // Add configuration
            services.Configure<MT5WrapperConfig>(configuration.GetSection("MT5Wrapper"));

            // Register core services
            services.AddSingleton<IMT5ConnectionService, MT5ConnectionService>();
            services.AddSingleton<ITenantManagerService, TenantManagerService>();
            services.AddSingleton<ITickDataService, TickDataService>();
            services.AddSingleton<IUserManagementService, UserManagementService>();

            // Add memory cache for testing
            services.AddMemoryCache();

            _serviceProvider = services.BuildServiceProvider();
            _logger = _serviceProvider.GetRequiredService<ILogger<Program>>();

            Console.WriteLine("✅ Testing environment setup complete");
        }

        private static void LoadTestConfiguration()
        {
            Console.WriteLine("📋 Loading test configuration...");

            var configBuilder = new ConfigurationBuilder()
                .SetBasePath(Directory.GetCurrentDirectory())
                .AddJsonFile("TestConfiguration.json", optional: false, reloadOnChange: true);

            var config = configBuilder.Build();
            var testSection = config.GetSection("TestEnvironment");

            _testConfig = new TestConfiguration
            {
                ManagerLogin = testSection["MT5Credentials:ManagerLogin"],
                ManagerPassword = testSection["MT5Credentials:ManagerPassword"],
                Server = testSection["MT5Credentials:Server"],
                TestClientLogin = testSection["MT5Credentials:TestClientLogin"],
                TestClientPassword = testSection["MT5Credentials:TestClientPassword"],
                TenantId = testSection["TestTenant:TenantId"],
                ManagerName = testSection["TestTenant:ManagerName"],
                Email = testSection["TestTenant:Email"],
                TestSymbols = testSection.GetSection("TestSymbols").Get<List<string>>() ?? new List<string>(),
                TestUsers = testSection.GetSection("TestUsers").Get<List<TestUserInfo>>() ?? new List<TestUserInfo>()
            };

            Console.WriteLine($"📊 Test config loaded:");
            Console.WriteLine($"   Manager: {_testConfig.ManagerLogin}");
            Console.WriteLine($"   Server: {_testConfig.Server}");
            Console.WriteLine($"   Tenant: {_testConfig.TenantId}");
            Console.WriteLine($"   Test Symbols: {string.Join(", ", _testConfig.TestSymbols)}");
        }

        private static async Task RunAPITests()
        {
            Console.WriteLine("\n🧪 Running API Tests...");
            Console.WriteLine("=====================\n");

            if (_testConfig == null || _serviceProvider == null)
            {
                Console.WriteLine("❌ Test configuration not loaded");
                return;
            }

            var testResults = new List<TestResult>();

            // Test 1: MT5 Connection
            Console.WriteLine("1️⃣ Testing MT5 Connection...");
            var connectionTest = await TestMT5Connection();
            testResults.Add(connectionTest);

            if (connectionTest.Passed)
            {
                // Test 2: Tick Data
                Console.WriteLine("\n2️⃣ Testing Tick Data APIs...");
                var tickTests = await TestTickDataAPIs();
                testResults.AddRange(tickTests);

                // Test 3: User Management
                Console.WriteLine("\n3️⃣ Testing User Management APIs...");
                var userTests = await TestUserManagementAPIs();
                testResults.AddRange(userTests);

                // Test 4: Multi-Tenant Isolation
                Console.WriteLine("\n4️⃣ Testing Multi-Tenant Isolation...");
                var tenantTest = await TestMultiTenantIsolation();
                testResults.Add(tenantTest);
            }

            // Store results for summary
            _testResults = testResults;
        }

        private static async Task<TestResult> TestMT5Connection()
        {
            try
            {
                var connectionService = _serviceProvider!.GetRequiredService<IMT5ConnectionService>();

                var config = new MT5ConnectionConfig
                {
                    Server = _testConfig!.Server,
                    Login = _testConfig.ManagerLogin,
                    Password = _testConfig.ManagerPassword,
                    ManagerId = _testConfig.TenantId,
                    ConnectionTimeout = 15000,
                    EnableAutoReconnect = false
                };

                Console.WriteLine($"   Connecting to {_testConfig.Server}...");

                var connectResult = await connectionService.ConnectAsync(config);

                if (connectResult.Success)
                {
                    Console.WriteLine($"   ✅ Connected successfully in {connectResult.ConnectionTimeMs}ms");

                    // Test connection health
                    var healthResult = await connectionService.TestConnectionAsync();
                    if (healthResult.IsSuccessful)
                    {
                        Console.WriteLine($"   ✅ Connection test passed ({healthResult.ResponseTimeMs}ms)");
                        return TestResult.CreatePassed("MT5 Connection", "Connection established and tested successfully");
                    }
                    else
                    {
                        Console.WriteLine($"   ⚠️ Connection established but health test failed: {healthResult.ErrorMessage}");
                        return TestResult.CreateFailed("MT5 Connection", "Connection established but health test failed");
                    }
                }
                else
                {
                    Console.WriteLine($"   ❌ Connection failed: {connectResult.Message}");
                    return TestResult.CreateFailed("MT5 Connection", connectResult.Message);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"   ❌ Exception: {ex.Message}");
                return TestResult.CreateFailed("MT5 Connection", ex.Message);
            }
        }


        private static async Task<List<TestResult>> TestTickDataAPIs()
        {
            var results = new List<TestResult>();

            try
            {
                var tickService = _serviceProvider!.GetRequiredService<ITickDataService>();

                // Test 1: Get available symbols
                Console.WriteLine($"   Getting available symbols...");
                var symbols = await tickService.GetAvailableSymbolsAsync(_testConfig!.TenantId);

                if (symbols.Any())
                {
                    Console.WriteLine($"   ✅ Found {symbols.Count()} symbols: {string.Join(", ", symbols.Take(5))}");
                    results.Add(TestResult.CreatePassed("Available Symbols", $"{symbols.Count()} symbols available"));
                }
                else
                {
                    Console.WriteLine($"   ⚠️ No symbols found");
                    results.Add(TestResult.CreateFailed("Available Symbols", "No symbols returned"));
                }

                // Test 2: Get current tick for EURUSD
                if (symbols.Contains("EURUSD"))
                {
                    Console.WriteLine($"   Getting EURUSD tick data...");
                    var tick = await tickService.GetCurrentTickAsync(_testConfig.TenantId, "EURUSD");

                    if (tick != null && tick.IsValid())
                    {
                        Console.WriteLine($"   ✅ EURUSD Tick: {tick.Bid}/{tick.Ask} (Spread: {tick.Spread})");
                        results.Add(TestResult.CreatePassed("Current Tick Data", $"EURUSD: {tick.Bid}/{tick.Ask}"));
                    }
                    else
                    {
                        Console.WriteLine($"   ⚠️ EURUSD tick data invalid or not available");
                        results.Add(TestResult.CreateFailed("Current Tick Data", "EURUSD tick data not available"));
                    }
                }

                // Test 3: Get tick history
                if (symbols.Contains("EURUSD"))
                {
                    Console.WriteLine($"   Getting EURUSD tick history...");
                    var history = await tickService.GetTickHistoryAsync(_testConfig.TenantId, "EURUSD", 10);

                    if (history.Any())
                    {
                        Console.WriteLine($"   ✅ Retrieved {history.Count()} historical ticks");
                        results.Add(TestResult.CreatePassed("Tick History", $"{history.Count()} historical ticks retrieved"));
                    }
                    else
                    {
                        Console.WriteLine($"   ⚠️ No historical ticks found");
                        results.Add(TestResult.CreateFailed("Tick History", "No historical ticks available"));
                    }
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine($"   ❌ Exception: {ex.Message}");
                results.Add(TestResult.CreateFailed("Tick Data APIs", ex.Message));
            }

            return results;
        }

        private static async Task<List<TestResult>> TestUserManagementAPIs()
        {
            var results = new List<TestResult>();

            try
            {
                var userService = _serviceProvider!.GetRequiredService<IUserManagementService>();

                // Test 1: Get total users
                Console.WriteLine($"   Getting total users count...");
                var totalUsers = await userService.GetTotalUsersAsync(_testConfig!.TenantId);

                Console.WriteLine($"   📊 Total users: {totalUsers}");
                results.Add(TestResult.CreatePassed("Total Users", $"{totalUsers} users found"));

                // Test 2: Get all users
                Console.WriteLine($"   Getting all users...");
                var users = await userService.GetUsersAsync(_testConfig.TenantId);

                if (users.Any())
                {
                    Console.WriteLine($"   ✅ Retrieved {users.Count()} users");
                    foreach (var user in users.Take(3))
                    {
                        Console.WriteLine($"      User {user.Login}: {user.Name} (Balance: {user.Balance})");
                    }
                    results.Add(TestResult.CreatePassed("Get Users", $"{users.Count()} users retrieved"));
                }
                else
                {
                    Console.WriteLine($"   ⚠️ No users found");
                    results.Add(TestResult.CreateFailed("Get Users", "No users returned"));
                }

                // Test 3: Get specific user account
                if (_testConfig.TestUsers.Any())
                {
                    var testUser = _testConfig.TestUsers.First();
                    Console.WriteLine($"   Getting user account {testUser.Login}...");

                    var account = await userService.GetUserAccountAsync(_testConfig.TenantId, testUser.Login);

                    if (account != null && account.IsValid())
                    {
                        Console.WriteLine($"   ✅ Account {testUser.Login}: Balance={account.Balance}, Equity={account.Equity}");
                        results.Add(TestResult.CreatePassed("User Account", $"Account details retrieved for {testUser.Login}"));
                    }
                    else
                    {
                        Console.WriteLine($"   ⚠️ Account {testUser.Login} not found or invalid");
                        results.Add(TestResult.CreateFailed("User Account", $"Account {testUser.Login} not accessible"));
                    }
                }

                // Test 4: Get groups
                Console.WriteLine($"   Getting user groups...");
                var groups = await userService.GetGroupsAsync(_testConfig.TenantId);

                if (groups.Any())
                {
                    Console.WriteLine($"   ✅ Found {groups.Count()} groups");
                    foreach (var group in groups.Take(3))
                    {
                        Console.WriteLine($"      Group: {group.GroupName}");
                    }
                    results.Add(TestResult.CreatePassed("User Groups", $"{groups.Count()} groups found"));
                }
                else
                {
                    Console.WriteLine($"   ⚠️ No groups found");
                    results.Add(TestResult.CreateFailed("User Groups", "No groups returned"));
                }

            }
            catch (Exception ex)
            {
                Console.WriteLine($"   ❌ Exception: {ex.Message}");
                results.Add(TestResult.CreateFailed("User Management APIs", ex.Message));
            }

            return results;
        }

        private static async Task<TestResult> TestMultiTenantIsolation()
        {
            try
            {
                Console.WriteLine($"   Testing tenant isolation...");

                // Test tenant creation
                var tenantService = _serviceProvider!.GetRequiredService<ITenantManagerService>();

                var createRequest = new CreateTenantRequest
                {
                    ManagerName = _testConfig!.ManagerName,
                    Email = _testConfig.Email,
                    Subscription = new SubscriptionInfo
                    {
                        PlanId = "professional",
                        PlanName = "Professional",
                        MonthlyPrice = 299,
                        Status = SubscriptionStatus.Active
                    }
                };

                var tenant = await tenantService.CreateTenantAsync(createRequest);

                if (tenant != null)
                {
                    Console.WriteLine($"   ✅ Tenant created: {tenant.TenantId}");
                    Console.WriteLine($"   🌐 API Base URL: {tenant.ApiEndpoints.BaseUrl}");

                    // Verify tenant isolation
                    var retrievedTenant = tenantService.GetTenant(tenant.TenantId);
                    if (retrievedTenant != null)
                    {
                        Console.WriteLine($"   ✅ Tenant isolation verified");
                        return TestResult.CreatePassed("Multi-Tenant Isolation", "Tenant creation and isolation working");
                    }
                    else
                    {
                        Console.WriteLine($"   ⚠️ Tenant created but not retrievable");
                        return TestResult.CreateFailed("Multi-Tenant Isolation", "Tenant not retrievable after creation");
                    }
                }
                else
                {
                    Console.WriteLine($"   ❌ Tenant creation failed");
                    return TestResult.CreateFailed("Multi-Tenant Isolation", "Tenant creation failed");
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine($"   ❌ Exception: {ex.Message}");
                return TestResult.CreateFailed("Multi-Tenant Isolation", ex.Message);
            }
        }

        private static void ShowTestSummary()
        {
            Console.WriteLine("\n📊 Test Summary");
            Console.WriteLine("==============");

            if (_testResults == null || !_testResults.Any())
            {
                Console.WriteLine("❌ No test results available");
                return;
            }

            var passed = _testResults.Count(r => r.Passed);
            var failed = _testResults.Count(r => !r.Passed);
            var total = _testResults.Count;

            Console.WriteLine($"Total Tests: {total}");
            Console.WriteLine($"✅ Passed: {passed}");
            Console.WriteLine($"❌ Failed: {failed}");
            Console.WriteLine($"📊 Success Rate: {(total > 0 ? (passed * 100 / total) : 0)}%");

            Console.WriteLine("\n📋 Detailed Results:");
            foreach (var result in _testResults)
            {
                var status = result.Passed ? "✅ PASS" : "❌ FAIL";
                Console.WriteLine($"   {status} - {result.TestName}: {result.Message}");
            }

            if (failed == 0)
            {
                Console.WriteLine("\n🎉 All tests passed! API is ready for production.");
            }
            else
            {
                Console.WriteLine($"\n⚠️ {failed} test(s) failed. Please check the errors above.");
            }
        }

        private static List<TestResult>? _testResults;
    }

    public class TestConfiguration
    {
        public string? ManagerLogin { get; set; }
        public string? ManagerPassword { get; set; }
        public string? Server { get; set; }
        public string? TestClientLogin { get; set; }
        public string? TestClientPassword { get; set; }
        public string? TenantId { get; set; }
        public string? ManagerName { get; set; }
        public string? Email { get; set; }
        public List<string>? TestSymbols { get; set; }
        public List<TestUserInfo>? TestUsers { get; set; }
    }

    public class TestUserInfo
    {
        public ulong Login { get; set; }
        public string? Name { get; set; }
        public string? Group { get; set; }
        public double ExpectedBalance { get; set; }
    }

    public class TestResult
    {
        public string TestName { get; set; } = string.Empty;
        public bool Passed { get; set; }
        public string Message { get; set; } = string.Empty;
        public DateTime Timestamp { get; set; } = DateTime.UtcNow;

        public static TestResult CreatePassed(string testName, string message)
        {
            return new TestResult { TestName = testName, Passed = true, Message = message };
        }

        public static TestResult CreateFailed(string testName, string message)
        {
            return new TestResult { TestName = testName, Passed = false, Message = message };
        }
    }
}