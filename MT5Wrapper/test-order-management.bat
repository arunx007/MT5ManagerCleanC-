@echo off
echo 🧪 Running Order Management API Test
echo =====================================
echo.

cd /d "%~dp0"

REM Build the project first
echo Building MT5Wrapper project...
dotnet build MT5Wrapper.csproj
if %errorlevel% neq 0 (
    echo ❌ Build failed!
    pause
    exit /b 1
)

echo.
echo Starting Order Management Test...
dotnet run --no-build Tests/OrderManagementTest.cs

echo.
echo Test completed. Press any key to exit...
pause >nul