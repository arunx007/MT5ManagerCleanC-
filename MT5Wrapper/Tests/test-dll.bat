@echo off
REM Simple DLL Test
REM Tests if MT5 DLLs can be loaded

echo 🧪 Simple MT5 DLL Test
echo =====================
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

REM Build and run the simple test
echo 🔨 Building simple test...
dotnet build MT5Wrapper\MT5Wrapper.csproj --verbosity minimal
if errorlevel 1 (
    echo ❌ Build failed
    pause
    exit /b 1
)

echo ✅ Build successful
echo.

REM Run the simple test
echo 🧪 Running simple MT5 DLL test...
echo This will test if the MT5 DLLs can be loaded and basic functions work.
echo.

dotnet run --project MT5Wrapper\MT5Wrapper.csproj

echo.
echo ====================
echo Simple test complete!
echo.

echo.
echo 📋 What this test validates:
echo - MT5 DLLs can be loaded by .NET
echo - Basic MT5 API functions are accessible
echo - No critical compatibility issues
echo.

echo.
echo 📖 Next Steps:
echo 1. If this test passes, run the full test suite
echo 2. If this fails, check MT5 SDK setup and antivirus
echo 3. Run: MT5Wrapper\test-step-by-step.bat
echo.

pause