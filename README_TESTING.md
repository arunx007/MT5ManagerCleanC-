# MT5Wrapper Testing Guide

## Overview
This testing suite provides comprehensive validation of the MT5Wrapper WebSocket functionality and API endpoints.

## Files Structure
```
/
├── MT5Wrapper/wwwroot/test.html          # Complete web-based test suite
├── WebSocketTest/                        # Console WebSocket testing
│   ├── Program.cs                        # C# WebSocket test client
│   └── WebSocketTest.csproj             # Test project file
└── README_TESTING.md                     # This file
```

## Quick Start

### 1. Start the MT5Wrapper Server
```bash
cd MT5Wrapper
dotnet run
```
Server will be available at: `http://localhost:5001`

### 2. Open the Test Suite
Navigate to: `http://localhost:5001/test.html`

### 3. Run Tests
1. **Authentication**: Click "Get Manager Token" to authenticate
2. **API Testing**: Test individual symbols or all symbols for tick data
3. **WebSocket Testing**: Connect and subscribe to live tick data

## Test Components

### 🔐 Authentication
- Automatic JWT token retrieval from manager login
- Manual token input support
- Token validation and status display

### 🔗 API Testing
- Individual symbol tick data testing
- Batch testing of all major forex pairs
- Health check endpoint validation
- Real-time test results with pass/fail status

### 🌐 WebSocket Testing
- SignalR hub connection management
- Live tick data subscription/unsubscription
- Real-time data display
- Connection status monitoring
- Ping/pong functionality

## Available Test Symbols

### ✅ Working Symbols (Live Data Available)
- **EURUSD** - Euro vs US Dollar
- **GBPUSD** - British Pound vs US Dollar
- **USDJPY** - US Dollar vs Japanese Yen
- **AUDUSD** - Australian Dollar vs US Dollar

### ❌ Non-Working Symbols (No Data Available)
- **USDCHF** - US Dollar vs Swiss Franc
- **USDCAD** - US Dollar vs Canadian Dollar

## Test Results Interpretation

### API Test Results
- **PASS**: Live tick data with bid/ask prices and spreads
- **FAIL**: No data available or connection errors

### WebSocket Test Results
- **Connected**: SignalR hub connection established
- **Subscribed**: Successfully subscribed to symbol updates
- **TickData**: Real-time price updates received
- **Live Data Display**: Current market data shown

## Console Testing

For automated testing, use the console application:

```bash
cd WebSocketTest
dotnet run
```

This will attempt to connect to the WebSocket hub and test live data streaming.

## Troubleshooting

### Authentication Issues
- Ensure MT5 server is running and accessible
- Verify manager credentials in configuration
- Check JWT token expiration (30 days for managers)

### WebSocket Connection Issues
- Verify SignalR hub is running (`/hubs/marketdata`)
- Check CORS configuration for WebSocket connections
- Ensure JWT token is valid and properly formatted

### No Data Available
- Some symbols may not have live data on your MT5 server
- Check MT5 server symbol availability
- Verify market hours and symbol trading status

## Test Coverage

### ✅ Implemented Features
- JWT authentication flow
- REST API tick data endpoints
- SignalR WebSocket hub
- Real-time tick data streaming
- Multi-symbol support
- Connection health monitoring

### ⚠️ Known Limitations
- WebSocket client authentication may need configuration
- Some symbols may not have live data
- Order book functionality requires MT5 server support

## Next Steps

1. **Fix WebSocket Authentication**: Resolve SignalR client connection issues
2. **Add More Symbols**: Test additional currency pairs and commodities
3. **Implement Order Book**: Once MT5 server supports order book data
4. **Add Automated Tests**: Create unit tests for API endpoints

## Support

For issues or questions:
1. Check the event log in the test suite
2. Verify MT5 server connectivity
3. Review application logs for detailed error messages
4. Test individual components (API → WebSocket → Live Data)