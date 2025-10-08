# MT5 Wrapper API - Postman Collection

Complete Postman collection for testing the MT5 Trading Platform Wrapper API.

## ⚡ Quick Start (3 Steps)

1. **Start API Server:**
   ```bash
   cd MT5Wrapper
   dotnet run
   ```

2. **Import Collection:**
   - Open Postman → Import → Select `MT5_Wrapper_API_Collection.postman_collection.json`

3. **Set Environment:**
   - Create environment with: `base_url = http://localhost:5000`
   - Test Manager Login endpoint first!

---

## 📋 Full Documentation

## 📁 Collection Structure

### 🔐 Authentication
- **Manager Login (30-day token)** - Manager authentication with 30-day JWT token
- **Client Login (unlimited token)** - Client authentication with unlimited JWT token
- **Validate Token** - Validate JWT token and get user information
- **Refresh Manager Token** - Refresh manager token (extends 30-day validity)
- **Revoke Token** - Revoke JWT token (logout)

### 👥 User Management
- **Get User List** - Get list of users with filtering and pagination
- **Get User by Login** - Get detailed user information by login
- **Create New User** - Create new trading account
- **Update User** - Update user information
- **Update User Balance** - Update user account balance (deposit/withdraw)
- **Delete User** - Delete user account

### 📋 Order Management
- **Get Open Orders** - Get open orders for user or all users (manager)
- **Get Order History** - Get order history with date range filtering
- **Create Market Order** - Create market order (immediate execution)
- **Create Pending Order** - Create pending order (limit/stop orders)
- **Modify Order** - Modify existing order (SL/TP, expiration, etc.)
- **Cancel Order** - Cancel pending order
- **Get Order by Ticket** - Get specific order details by ticket number

### 📊 Market Data
- **Get Symbol List** - Get list of available trading symbols
- **Get Symbol Info** - Get detailed information for specific symbol
- **Get Current Prices** - Get current bid/ask prices for symbols
- **Get Chart Data (OHLC)** - Get OHLC chart data for technical analysis
- **Get Tick Data** - Get historical tick data for symbol
- **Subscribe to Real-time Prices** - Subscribe to real-time price updates

### 💼 Trading Operations
- **Get Account Positions** - Get open trading positions
- **Get Account Balance** - Get account balance and margin information
- **Get Trade History** - Get trade history (deals/closed positions)
- **Close Position** - Close open trading position
- **Modify Position** - Modify open position (SL/TP)

### 🏢 Multi-tenant Management
- **Get Tenant Info** - Get current tenant information
- **Get Tenant Statistics** - Get tenant usage statistics
- **Update Tenant Settings** - Update tenant configuration settings

### 📊 System Health & Monitoring
- **API Health Check** - Check API service health
- **MT5 Connection Status** - Check MT5 server connection status
- **API Usage Statistics** - Get API usage statistics

## 🚀 Getting Started

### 1. Import Collection
1. Open Postman
2. Click "Import" button
3. Select "File" tab
4. Choose `MT5_Wrapper_API_Collection.postman_collection.json`
5. Click "Import"

### 2. Set Environment Variables
Create a new environment in Postman with these variables:

```
base_url = http://localhost:5000
manager_token = (empty - will be set after login)
client_token = (empty - will be set after login)
access_token = {{manager_token}}
```

**⚠️ IMPORTANT:** If you get `getaddrinfo ENOTFOUND api.mt5wrapper.com` error:

1. **Open Postman Environment Settings**
2. **Change the `base_url` variable from:**
   ```
   https://api.mt5wrapper.com
   ```
   **To:**
   ```
   http://localhost:5000
   ```
3. **Save the environment**
4. **Make sure your API server is running:**
   ```bash
   cd MT5Wrapper
   dotnet run
   ```

### 3. Start Local API Server
Before testing, you need to run the MT5 Wrapper API locally:

**Option 1: Command Line**
```bash
# Navigate to MT5Wrapper directory
cd MT5Wrapper

# Run the API server
dotnet run
```

**Option 2: PowerShell**
```powershell
# Navigate to MT5Wrapper directory
cd MT5Wrapper

# Run the API server
dotnet run
```

**Option 3: Windows Command Prompt**
```cmd
# Navigate to MT5Wrapper directory
cd MT5Wrapper

# Run the API server
dotnet run
```

The API will start on `http://localhost:5000`

**Expected Output:**
```
info: Microsoft.Hosting.Lifetime[14]
      Now listening on: http://localhost:5000
info: Microsoft.Hosting.Lifetime[0]
      Application started. Press Ctrl+C to shut down.
```

### 4. For Production Testing
When your API is deployed to production, change the environment variable:

```
base_url = https://your-production-domain.com
```

### 3. Authentication Flow
1. **Start with Manager Login** to get a 30-day manager token
2. **Use the token** in subsequent requests (automatically set via `{{manager_token}}`)
3. **Test Client Login** to get unlimited client tokens for mobile apps

### 4. Test Sequence
1. 🔐 **Authentication** - Login and get tokens
2. 👥 **User Management** - Create/test user accounts
3. 📊 **Market Data** - Get symbols and prices
4. 📋 **Order Management** - Place and manage orders
5. 💼 **Trading Operations** - Monitor positions and balance

## 🔑 Authentication Details

### Manager Tokens (30 days)
- Full access to all client accounts
- Can create/modify/delete users
- Can place orders for any client
- Expires after 30 days
- Can be refreshed

### Client Tokens (Unlimited)
- Access only to own account
- Can view own positions/orders
- Can place orders for own account only
- No expiration
- Perfect for mobile apps

## 📝 Request Parameters

### Common Headers
```
Authorization: Bearer {{access_token}}
Content-Type: application/json
X-Tenant-ID: test_mgr_001 (for manager login)
```

### Common Query Parameters
- `login` - User account login (optional for managers)
- `symbol` - Trading symbol (EURUSD, GBPUSD, etc.)
- `from/to` - Unix timestamps for date ranges
- `limit` - Maximum records to return

### Order Types
- `OP_BUY` - Buy market order
- `OP_SELL` - Sell market order
- `OP_BUYLIMIT` - Buy limit pending order
- `OP_SELLLIMIT` - Sell limit pending order
- `OP_BUYSTOP` - Buy stop pending order
- `OP_SELLSTOP` - Sell stop pending order

### Timeframes
- `M1, M5, M15, M30` - Minutes
- `H1, H4` - Hours
- `D1` - Daily
- `W1` - Weekly
- `MN` - Monthly

## 🎯 Example Usage

### 1. Manager Login
```json
POST /api/auth/manager/login
{
  "managerLogin": "1006",
  "password": "Meta@5757",
  "server": "86.104.251.165:443"
}
```

### 2. Create User
```json
POST /api/users
Authorization: Bearer {{manager_token}}
{
  "login": "100268",
  "name": "Test User",
  "email": "test@example.com",
  "group": "demoforex",
  "balance": 1000.00
}
```

### 3. Place Market Order
```json
POST /api/orders/market
Authorization: Bearer {{manager_token}}
{
  "login": "100268",
  "symbol": "EURUSD",
  "type": "OP_BUY",
  "volume": 0.01,
  "stoploss": 1.0800,
  "takeprofit": 1.1200
}
```

### 4. Get Real-time Prices
```json
GET /api/market/prices?symbols=EURUSD,GBPUSD
Authorization: Bearer {{access_token}}
```

## ⚠️ Important Notes

- **Manager tokens expire after 30 days** - use refresh endpoint
- **Client tokens never expire** - perfect for mobile apps
- **All requests require authentication** except health checks
- **Manager can access all client data** - client can only access own data
- **Use X-Tenant-ID header** only for manager login
- **Unix timestamps** for date parameters (seconds since 1970-01-01)

## 🔧 Error Handling

Common HTTP status codes:
- `200` - Success
- `400` - Bad Request (invalid parameters)
- `401` - Unauthorized (invalid/missing token)
- `403` - Forbidden (insufficient permissions)
- `404` - Not Found
- `429` - Too Many Requests (rate limited)
- `500` - Internal Server Error

Error response format:
```json
{
  "success": false,
  "error": "Error description",
  "code": "ERROR_CODE"
}
```

## 📞 Support

For questions about the API:
- Check the detailed request descriptions in Postman
- Review the response examples
- Test with the provided sample data
- Contact support for production credentials

## 🔧 Troubleshooting

### ❌ `getaddrinfo ENOTFOUND api.mt5wrapper.com`
**Problem:** Postman is trying to connect to production URL that doesn't exist.

**Solution:**
1. Check your Postman environment variables
2. Change `base_url` from `https://api.mt5wrapper.com` to `http://localhost:5000`
3. Make sure your local API server is running:
   ```bash
   cd MT5Wrapper
   dotnet run
   ```
4. Verify the server started successfully (should show listening on port 5000)

### ❌ `Connection refused` or `ECONNREFUSED`
**Problem:** API server is not running.

**Solution:**
```bash
cd MT5Wrapper
dotnet run
```
Wait for "Now listening on: http://localhost:5000" message.

### ❌ `401 Unauthorized`
**Problem:** Missing or invalid JWT token.

**Solution:**
1. First call the Manager Login endpoint to get a token
2. Copy the `access_token` from the response
3. Set it in your environment variables as `manager_token`
4. Make sure the `Authorization: Bearer {{manager_token}}` header is present

### ❌ `500 Internal Server Error`
**Problem:** API server error, possibly MT5 connection issues.

**Solution:**
1. Check the API server console for error messages
2. Verify MT5 server credentials in `Config/TestConfiguration.json`
3. Make sure MT5 server is accessible from your network
4. Check MT5 Manager API DLLs are present in `Libs/` folder

### ❌ `400 Bad Request`
**Problem:** Invalid request parameters.

**Solution:**
1. Check the request body matches the expected format
2. Verify all required parameters are included
3. Check parameter types (strings, numbers, dates)
4. Review the endpoint documentation in this README

### 🔍 Debug Tips
- Use Postman's **Console** tab to see detailed request/response logs
- Check the API server console output for detailed error messages
- Test with simple endpoints first (like `/health`)
- Verify your environment variables are set correctly

---

**Collection Version:** 1.0.0
**API Version:** v1
**Last Updated:** 2025-01-04