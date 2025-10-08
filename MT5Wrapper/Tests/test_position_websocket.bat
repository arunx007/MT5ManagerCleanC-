@echo off
echo ========================================
echo Position WebSocket Testing Script
echo ========================================
echo.
echo Testing Position WebSocket endpoints for client 100267
echo.

echo 1. Testing Position Subscription Status...
curl -X GET "http://localhost:5001/api/websocket/positions/status" ^
  -H "Content-Type: application/json"
echo.
echo.

echo 2. Testing Position Subscription (REST API)...
curl -X POST "http://localhost:5001/api/websocket/positions/subscribe/100267" ^
  -H "Content-Type: application/json"
echo.
echo.

echo 3. Testing Position Snapshot (REST API)...
curl -X GET "http://localhost:5001/api/positions/100267" ^
  -H "Content-Type: application/json"
echo.
echo.

echo 4. Testing Position Service Stats...
curl -X GET "http://localhost:5001/api/positions/stats" ^
  -H "Content-Type: application/json"
echo.
echo.

echo 5. Testing WebSocket Info...
curl -X GET "http://localhost:5001/api/websocket/info" ^
  -H "Content-Type: application/json"
echo.
echo.

echo ========================================
echo WebSocket Testing Complete
echo ========================================
echo.
echo Note: For full WebSocket testing, use a WebSocket client to connect to:
echo ws://localhost:5000/hubs/marketdata
echo.
echo WebSocket Methods to test:
echo - SubscribeToPositions(100267)
echo - GetPositionsSnapshot(100267)
echo - GetPositionStatus()
echo.
pause