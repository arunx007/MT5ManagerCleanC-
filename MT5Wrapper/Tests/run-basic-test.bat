@echo off
REM Basic MT5 SDK Test
REM This tests if the MT5 DLLs are working correctly

echo 🧪 Basic MT5 SDK Test
echo ====================
echo.

REM Check if DLLs exist
if not exist "MT5Wrapper\Libs\MetaQuotes.MT5ManagerAPI64.dll" (
    echo ❌ MT5 Manager API DLL not found!
    echo.
    echo Please copy MetaQuotes.MT5ManagerAPI64.dll to MT5Wrapper\Libs\
    echo You can find it in your MT5 SDK installation folder.
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

echo ✅ MT5 DLLs found
echo.

REM Build the basic test
echo 🔨 Building basic test...
dotnet build MT5Wrapper\MT5Wrapper.csproj --verbosity minimal
if errorlevel 1 (
    echo ❌ Build failed
    pause
    exit /b 1
)

echo ✅ Build successful
echo.

REM Run the basic test
echo 🧪 Running basic MT5 SDK test...
echo This will test if the MT5 DLLs can be loaded and basic functions work.
echo.

REM Create a simple test executable
echo using System; > temp_test.cs
echo using System.Runtime.InteropServices; >> temp_test.cs
echo class Test { static void Main() { Console.WriteLine("Testing basic functionality..."); } } >> temp_test.cs

dotnet run --project MT5Wrapper\MT5Wrapper.csproj

echo.
echo ====================
echo Basic test complete!
echo.

echo.
echo 📋 What this test checks:
echo - MT5 DLLs can be loaded
echo - Basic MT5 API functions are accessible
echo - No critical compatibility issues
echo.

if %errorlevel% equ 0 (
    echo ✅ Basic MT5 SDK integration is working!
    echo 🎉 You can proceed with full API testing.
) else (
    echo ❌ Basic test failed.
    echo Please check the MT5 SDK setup and DLL compatibility.
)

echo.
echo 📖 Next: Run the full test suite with your MT5 credentials
echo Command: MT5Wrapper\test-step-by-step.bat
echo.

pause