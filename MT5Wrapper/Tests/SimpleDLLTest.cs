using System;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

public class SimpleDLLTest
{
    public static void RunTest()
    {
        Console.WriteLine("🧪 Simple MT5 SDK Test");
        Console.WriteLine("======================");

        try
        {
            Console.WriteLine("Testing MT5 SDK initialization...");

            // Initialize MT5 API Factory using managed wrapper
            var initResult = SMTManagerAPIFactory.Initialize(null);

            Console.WriteLine($"MT5 API Factory Initialize result: {initResult}");

            if (initResult == MTRetCode.MT_RET_OK)
            {
                Console.WriteLine("✅ MT5 SDK initialized successfully!");

                // Try to create manager
                var manager = SMTManagerAPIFactory.CreateManager(SMTManagerAPIFactory.ManagerAPIVersion, out MTRetCode createResult);

                if (createResult == MTRetCode.MT_RET_OK && manager != null)
                {
                    Console.WriteLine("✅ MT5 Manager created successfully!");
                    manager.Release();
                }
                else
                {
                    Console.WriteLine($"❌ MT5 Manager creation failed: {createResult}");
                }

                // Shutdown factory
                SMTManagerAPIFactory.Shutdown();
            }
            else
            {
                Console.WriteLine("❌ MT5 SDK initialization failed");
            }
        }
        catch (Exception ex)
        {
            Console.WriteLine($"❌ Exception: {ex.Message}");
        }

        Console.WriteLine("Test completed successfully!");
        // Console.ReadKey(); // Commented out for VSCode terminal compatibility
    }
}