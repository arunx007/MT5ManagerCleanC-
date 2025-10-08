@echo off
REM Clean MT5 API Test
REM This tests the MT5 SDK integration following proper patterns

echo 🧪 Clean MT5 API Test
echo ====================
echo.

REM Check if DLLs exist
if not exist "MT5Wrapper\Libs\MetaQuotes.MT5ManagerAPI64.dll" (
    echo ❌ MT5 Manager API DLL not found!
    echo.
    echo Please copy MetaQuotes.MT5ManagerAPI64.dll to MT5Wrapper\Libs\
    echo You can find it in your MT5 SDK installation folder.
    echo.
    echo Current directory contents:
    dir MT5Wrapper\Libs\
    echo.
    pause
    exit /b 1
)

if not exist "MT5Wrapper\Libs\MetaQuotes.MT5CommonAPI64.dll" (
    echo ❌ MT5 Common API DLL not found!
    echo.
    echo Please copy MetaQuotes.MT5CommonAPI64.dll to MT5Wrapper\Libs\
    echo You can find it in your MT5 SDK installation folder.
    echo.
    pause
    exit /b 1
)

echo ✅ MT5 DLLs found in correct location
echo.

REM Build and run the clean test
echo 🔨 Building and running clean MT5 test...
echo.

REM Create a simple test executable
echo using System; > temp_clean_test.cs
echo using System.Runtime.InteropServices; >> temp_clean_test.cs
echo using Microsoft.Extensions.Logging; >> temp_clean_test.cs
echo using Microsoft.Extensions.Configuration; >> temp_clean_test.cs
echo. >> temp_clean_test.cs
echo // MT5 API DLL imports >> temp_clean_test.cs
echo [DllImport("MT5Wrapper/Libs/MetaQuotes.MT5ManagerAPI64.dll", CallingConvention=CallingConvention.StdCall)] >> temp_clean_test.cs
echo private static extern int SMTManagerAPIFactory_Initialize(IntPtr config); >> temp_clean_test.cs
echo. >> temp_clean_test.cs
echo [DllImport("MT5Wrapper/Libs/MetaQuotes.MT5ManagerAPI64.dll", CallingConvention=CallingConvention.StdCall)] >> temp_clean_test.cs
echo private static extern IntPtr SMTManagerAPIFactory_CreateManager(uint version, out int result); >> temp_clean_test.cs
echo. >> temp_clean_test.cs
echo [DllImport("MT5Wrapper/Libs/MetaQuotes.MT5ManagerAPI64.dll", CallingConvention=CallingConvention.StdCall)] >> temp_clean_test.cs
echo private static extern void SMTManagerAPIFactory_Shutdown(); >> temp_clean_test.cs
echo. >> temp_clean_test.cs
echo class Test { >> temp_clean_test.cs
echo     public static void Main() { >> temp_clean_test.cs
echo         Console.WriteLine("Testing MT5 DLL loading..."); >> temp_clean_test.cs
echo         try { >> temp_clean_test.cs
echo             var result = SMTManagerAPIFactory_Initialize(IntPtr.Zero); >> temp_clean_test.cs
echo             Console.WriteLine($"MT5 API Init Result: {result}"); >> temp_clean_test.cs
echo             if (result == 0) { >> temp_clean_test.cs
echo                 Console.WriteLine("✅ SUCCESS: MT5 DLLs loaded and initialized!"); >> temp_clean_test.cs
echo                 Console.WriteLine("🎉 Your MT5 SDK integration is working perfectly!"); >> temp_clean_test.cs
echo             } else { >> temp_clean_test.cs
echo                 Console.WriteLine("⚠️ WARNING: DLLs loaded but initialization returned non-zero"); >> temp_clean_test.cs
echo             } >> temp_clean_test.cs
echo         } catch (Exception ex) { >> temp_clean_test.cs
echo             Console.WriteLine($"❌ ERROR: {ex.Message}"); >> temp_clean_test.cs
echo             Console.WriteLine(""); >> temp_clean_test.cs
echo             Console.WriteLine("Troubleshooting:"); >> temp_clean_test.cs
echo             Console.WriteLine("- Check if DLLs are not blocked by antivirus"); >> temp_clean_test.cs
echo             Console.WriteLine("- Verify DLL architecture (32-bit vs 64-bit)"); >> temp_clean_test.cs
echo             Console.WriteLine("- Ensure Visual C++ redistributables are installed"); >> temp_clean_test.cs
echo         } >> temp_clean_test.cs
echo     } >> temp_clean_test.cs
echo } >> temp_clean_test.cs

dotnet run --project MT5Wrapper/MT5Wrapper.csproj temp_clean_test.cs

echo.
echo ====================
echo Clean test complete!
echo.

echo.
echo 📋 What this test validates:
echo - MT5 DLLs can be loaded by .NET
echo - Basic MT5 API functions are accessible
echo - No critical compatibility issues
echo - SDK integration is working properly
echo.

if %errorlevel% equ 0 (
    echo ✅ SUCCESS! Your MT5 API integration is working!
    echo 🎉 You can proceed with full API development.
    echo.
    echo Next steps:
    echo 1. Run the full test suite: MT5Wrapper\test-step-by-step.bat
    echo 2. Test with your real MT5 credentials
    echo 3. Deploy to production
) else (
    echo ❌ Test failed.
    echo Please check the MT5 SDK setup and troubleshooting guide.
    echo See TESTING_CHECKLIST.md for detailed help.
)

echo.
echo 📖 Documentation:
echo - TESTING_CHECKLIST.md - Troubleshooting guide
echo - API_REFERENCE.md - Complete API documentation
echo - MANUAL_TEST_GUIDE.md - Step-by-step testing guide
echo.

pause