# Test Commands - Curl Commands Used

## 📋 **ACTUAL COMMANDS EXECUTED DURING TESTING**

All commands were executed against `http://localhost:5000` with the MT5Wrapper server running.

---

## 🏥 **HEALTH CHECK API**

### 1. Health Check
```bash
curl -X GET "http://localhost:5000/api/health" -H "accept: application/json"
```
**Expected Response**: System status and available endpoints

---

## 🔐 **AUTHENTICATION APIs**

### 2. Manager Login (FAILED - Wrong JSON format)
```bash
curl -X POST "http://localhost:5000/api/auth/manager/login" -H "accept: application/json" -H "Content-Type: application/json" -d "{\"managerLogin\":\"1006\",\"password\":\"Meta@5757\",\"server\":\"86.104.251.165:443\"}"
```
**Error**: Validation errors for Login and ManagerId fields

### 3. Manager Login (SUCCESS)
```bash
curl -X POST "http://localhost:5000/api/auth/manager/login" -H "accept: application/json" -H "Content-Type: application/json" -d "{\"login\":\"1006\",\"password\":\"Meta@5757\",\"server\":\"86.104.251.165:443\",\"managerId\":\"mgr_a1b2c3d4\"}"
```
**Response**: JWT token with 30-day expiration

### 4. Client Login (FAILED - Missing ManagerId)
```bash
curl -X POST "http://localhost:5000/api/auth/client/login" -H "accept: application/json" -H "Content-Type: application/json" -d "{\"login\":\"100267\",\"password\":\"Test@123\",\"server\":\"86.104.251.165:443\"}"
```
**Error**: Manager ID is required

### 5. Client Login (SUCCESS)
```bash
curl -X POST "http://localhost:5000/api/auth/client/login" -H "accept: application/json" -H "Content-Type: application/json" -d "{\"login\":\"100267\",\"password\":\"Test@123\",\"server\":\"86.104.251.165:443\",\"managerId\":\"mgr_a1b2c3d4\"}"
```
**Response**: JWT token with unlimited expiration

---

## 📊 **MARKET DATA APIs**

### 6. Get Symbols List
```bash
curl -X GET "http://localhost:5000/api/marketdata/symbols" -H "accept: application/json"
```
**Response**: Array of 5,541 trading symbols

### 7. Get Current Tick (EURUSD)
```bash
curl -X GET "http://localhost:5000/api/marketdata/tick/EURUSD" -H "accept: application/json"
```
**Response**: Real-time bid/ask prices, spread, timestamp

### 8. Get Timeframes
```bash
curl -X GET "http://localhost:5000/api/marketdata/timeframes" -H "accept: application/json"
```
**Response**: Array of 9 supported timeframes

### 9. Get Market Data Status
```bash
curl -X GET "http://localhost:5000/api/marketdata/status" -H "accept: application/json"
```
**Response**: Connection status, symbol count, uptime

### 10. Get Chart Data (FAILED)
```bash
curl -X POST "http://localhost:5000/api/marketdata/chart" -H "accept: application/json" -H "Content-Type: application/json" -d "{\"symbol\":\"EURUSD\",\"timeframe\":60,\"startTime\":1728000000,\"endTime\":1728290000,\"limit\":10}"
```
**Error**: "No data found for symbol: EURUSD"

### 11. Get Historical Data (FAILED)
```bash
curl -X POST "http://localhost:5000/api/marketdata/historical" -H "accept: application/json" -H "Content-Type: application/json" -d "{\"symbol\":\"EURUSD\",\"dataType\":\"ticks\",\"startDate\":1728000000,\"endDate\":1728290000,\"limit\":10}"
```
**Error**: "No historical data found for symbol: EURUSD"

---

## 👥 **USER MANAGEMENT APIs**

### 12. Get Users List (FAILED - No Response)
```bash
curl -X GET "http://localhost:5000/api/users" -H "accept: application/json" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJtZ3JfYTFiMmMzZDQiLCJqdGkiOiIzNThmZThjMS0zOTg2LTQ1NDQtOTlmZC01YjA2Yzk3MmE1ZDMiLCJtYW5hZ2VyX2lkIjoibWdyX2ExYjJjM2Q0IiwibG9naW4iOiIxMDA2Iiwic2VydmVyIjoiODYuMTA0LjI1MS4xNjU6NDQzIiwicGVybWlzc2lvbnMiOiJ7XCJDYW5BY2Nlc3NBbGxBY2NvdW50c1wiOnRydWUsXCJDYW5Nb2RpZnlVc2Vyc1wiOnRydWUsXCJDYW5Nb2RpZnlHcm91cHNcIjp0cnVlLFwiQ2FuQWNjZXNzVHJhZGluZ0RhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzTGl2ZURhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzSGlzdG9yaWNhbERhdGFcIjp0cnVlfSIsInRva2VuX3R5cGUiOiJtYW5hZ2VyIiwiZXhwaXJ5X2RheXMiOiIzMCIsImV4cCI6MTc2MjQyNTg4Mn0.0u1_SAlVmZ9EJexohVVjYdzkA5a9EGhKXSYhedtAslo"
```
**Response**: No response (connection timeout)

---

## 🧪 **TESTING & DIAGNOSTIC APIs**

### 13. Run Tests API
```bash
curl -X GET "http://localhost:5000/api/test/run" -H "accept: application/json"
```
**Response**: Success message directing to MT5Wrapper.Tests project

### 14. Test Health API
```bash
curl -X GET "http://localhost:5000/api/test/health" -H "accept: application/json"
```
**Response**: Health status, version, and timestamp

---

## 🔐 **ADVANCED AUTHENTICATION APIs**

### 15. Token Refresh API (FAILED - No Response)
```bash
curl -X POST "http://localhost:5000/api/auth/manager/refresh" -H "accept: application/json" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJtZ3JfYTFiMmMzZDQiLCJqdGkiOiIzNThmZThjMS0zOTg2LTQ1NDQtOTlmZC01YjA2Yzk3MmE1ZDMiLCJtYW5hZ2VyX2lkIjoibWdyX2ExYjJjM2Q0IiwibG9naW4iOiIxMDA2Iiwic2VydmVyIjoiODYuMTA0LjI1MS4xNjU6NDQzIiwicGVybWlzc2lvbnMiOiJ7XCJDYW5BY2Nlc3NBbGxBY2NvdW50c1wiOnRydWUsXCJDYW5Nb2RpZnlVc2Vyc1wiOnRydWUsXCJDYW5Nb2RpZnlHcm91cHNcIjp0cnVlLFwiQ2FuQWNjZXNzVHJhZGluZ0RhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzTGl2ZURhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzSGlzdG9yaWNhbERhdGFcIjp0cnVlfSIsInRva2VuX3R5cGUiOiJtYW5hZ2VyIiwiZXhwaXJ5X2RheXMiOiIzMCIsImV4cCI6MTc2MjQyNTg4Mn0.0u1_SAlVmZ9EJexohVVjYdzkA5a9EGhKXSYhedtAslo"
```
**Response**: No response (same tenant validation issues)

### 16. Token Revoke API (FAILED - No Response)
```bash
curl -X POST "http://localhost:5000/api/auth/revoke" -H "accept: application/json" -H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiJtZ3JfYTFiMmMzZDQiLCJqdGkiOiIzNThmZThjMS0zOTg2LTQ1NDQtOTlmZC01YjA2Yzk3MmE1ZDMiLCJtYW5hZ2VyX2lkIjoibWdyX2ExYjJjM2Q0IiwibG9naW4iOiIxMDA2Iiwic2VydmVyIjoiODYuMTA0LjI1MS4xNjU6NDQzIiwicGVybWlzc2lvbnMiOiJ7XCJDYW5BY2Nlc3NBbGxBY2NvdW50c1wiOnRydWUsXCJDYW5Nb2RpZnlVc2Vyc1wiOnRydWUsXCJDYW5Nb2RpZnlHcm91cHNcIjp0cnVlLFwiQ2FuQWNjZXNzVHJhZGluZ0RhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzTGl2ZURhdGFcIjp0cnVlLFwiQ2FuQWNjZXNzSGlzdG9yaWNhbERhdGFcIjp0cnVlfSIsInRva2VuX3R5cGUiOiJtYW5hZ2VyIiwiZXhwaXJ5X2RheXMiOiIzMCIsImV4cCI6MTc2MjQyNTg4Mn0.0u1_SAlVmZ9EJexohVVjYdzkA5a9EGhKXSYhedtAslo"
```
**Response**: No response (same tenant validation issues)

---

## 📝 **COMMAND EXECUTION NOTES**

### **Command Format**
- All commands use `curl` with appropriate HTTP methods
- JSON payloads use `-H "Content-Type: application/json"`
- Authorization headers include `Bearer {token}` for authenticated endpoints

### **Response Handling**
- Successful responses return JSON data
- Failed requests return error messages or validation errors
- No-response scenarios indicate server-side issues

### **Token Usage**
- Manager tokens are 30-day JWT tokens
- Client tokens are unlimited JWT tokens
- Tokens include tenant ID and permissions in claims

### **Error Patterns**
- **400 Bad Request**: Validation errors (missing fields, wrong format)
- **401 Unauthorized**: Invalid/missing authentication
- **500 Internal Server Error**: Server-side exceptions
- **No Response**: Connection timeouts or server crashes

---

## 🧪 **TESTING WORKFLOW**

### **Execution Order**
1. Start server: `cd MT5Wrapper && dotnet run`
2. Test health: Command #1
3. Test auth: Commands #2-5
4. Test market data: Commands #6-11
5. Test user management: Command #12

### **Success Criteria**
- **HTTP 200**: Successful response with expected data
- **Valid JSON**: Properly formatted response data
- **Data Accuracy**: Returned data matches expectations
- **Performance**: Response within reasonable time limits

### **Failure Analysis**
- **Validation Errors**: Check request format and required fields
- **Authentication Errors**: Verify token validity and permissions
- **Server Errors**: Check server logs for internal issues
- **Data Availability**: Verify if requested data exists on server

---

## 🔧 **COMMAND TEMPLATES FOR FUTURE TESTING**

### **Generic GET Request**
```bash
curl -X GET "http://localhost:5000/api/{endpoint}" -H "accept: application/json" -H "Authorization: Bearer {token}"
```

### **Generic POST Request**
```bash
curl -X POST "http://localhost:5000/api/{endpoint}" -H "accept: application/json" -H "Content-Type: application/json" -d "{json_payload}"
```

### **With Authentication**
```bash
-H "Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9..."
```

### **Testing Tips**
- Always test health check first
- Use valid JWT tokens for authenticated endpoints
- Check server logs for detailed error information
- Test with both manager and client tokens where applicable