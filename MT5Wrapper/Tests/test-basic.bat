@echo off
REM Basic MT5 Connection Test
REM Tests your MT5 integration with real credentials

echo 🧪 Basic MT5 Connection Test
echo ============================
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

REM Build and run the basic test
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
echo 🧪 Running basic MT5 connection test...
echo This will test your APIs with:
echo   Manager: 1006
echo   Password: Meta@5757
echo   Server: 86.104.251.165:443
echo   Test Client: 100267
echo.

echo Starting test execution...
echo =========================
echo.

dotnet run --project MT5Wrapper\MT5Wrapper.csproj

echo.
echo =========================
echo Basic test complete!
echo.

echo.
echo 📋 What this test validates:
echo - MT5 DLLs can be loaded by .NET
echo - Basic MT5 API functions are accessible
echo - Connection to your MT5 server
echo - Manager authentication
echo - Data retrieval capabilities
echo.

echo.
echo 📖 Next Steps:
echo 1. Review test results above
echo 2. If all tests pass, your API is ready for deployment!
echo 3. If tests fail, check MT5 SDK setup and credentials
echo.

pause