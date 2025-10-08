using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using MT5Wrapper.UserManagement.Interfaces;
using MT5Wrapper.UserManagement.DTOs;
using MT5Wrapper.Core.Interfaces;
using MT5Wrapper.MultiTenant.Interfaces;
using Microsoft.Extensions.Logging;
using MetaQuotes.MT5CommonAPI;
using MetaQuotes.MT5ManagerAPI;

namespace MT5Wrapper.UserManagement.Services
{
    /// <summary>
    /// User management service implementation - handles MT5 user account operations
    /// Uses MT5 SDK patterns for proper integration - follows SDK example pattern
    /// </summary>
    public class UserManagementService : IUserManagementService, IDisposable
    {
        private readonly ILogger<UserManagementService> _logger;
        private readonly IMT5ConnectionService _connectionService;
        private readonly ITenantManagerService _tenantManager;
        private bool _disposed;

        // SDK pattern: Create objects once and reuse them
        private CIMTUser? _user;
        private CIMTAccount? _account;
        private CIMTUserArray? _userArray;

        public UserManagementService(
            ILogger<UserManagementService> logger,
            IMT5ConnectionService connectionService,
            ITenantManagerService tenantManager)
        {
            _logger = logger ?? throw new ArgumentNullException(nameof(logger));
            _connectionService = connectionService ?? throw new ArgumentNullException(nameof(connectionService));
            _tenantManager = tenantManager ?? throw new ArgumentNullException(nameof(tenantManager));

            // Initialize MT5 objects following SDK pattern
            InitializeMT5Objects();
        }

        private void InitializeMT5Objects()
        {
            try
            {
                var manager = _connectionService.Manager as CIMTManagerAPI;
                if (manager == null)
                {
                    _logger.LogError("MT5 Manager not available for object initialization");
                    return;
                }

                // Create user object (SDK pattern)
                _user = manager.UserCreate();
                if (_user == null)
                {
                    _logger.LogError("Failed to create MT5 user object");
                }

                // Create account object (SDK pattern)
                _account = manager.UserCreateAccount();
                if (_account == null)
                {
                    _logger.LogError("Failed to create MT5 account object");
                }

                // Create user array for batch operations
                _userArray = manager.UserCreateArray();
                if (_userArray == null)
                {
                    _logger.LogError("Failed to create MT5 user array object");
                }

                _logger.LogInformation("MT5 objects initialized successfully");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception during MT5 object initialization");
            }
        }

        /// <inheritdoc/>
        public async Task<UserDto?> GetUserAsync(string tenantId, ulong login)
        {
            try
            {
                _logger.LogDebug("Getting user {Login} for tenant {TenantId}", login, tenantId);

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    _logger.LogWarning("Invalid or inactive tenant {TenantId}", tenantId);
                    return null;
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    _logger.LogError("No MT5 manager available for tenant {TenantId}", tenantId);
                    return null;
                }

                // SDK Pattern: Use pre-created objects and clear them
                if (_user == null)
                {
                    _logger.LogError("MT5 user object not initialized");
                    return null;
                }

                // Clear user object before use (SDK pattern)
                _user.Clear();

                // Request user from server (SDK pattern)
                var result = mt5Manager.UserRequest(login, _user);
                if (result != MTRetCode.MT_RET_OK)
                {
                    _logger.LogWarning("User {Login} not found in tenant {TenantId}: {Result}", login, tenantId, result);
                    return null;
                }

                // Get account information (SDK pattern)
                CIMTAccount? account = null;
                if (_account != null)
                {
                    _account.Clear();
                    var accountResult = mt5Manager.UserAccountRequest(login, _account);
                    if (accountResult == MTRetCode.MT_RET_OK)
                    {
                        account = _account;
                    }
                }

                var userDto = UserDto.FromMT5User(_user, account, tenantId);

                _logger.LogDebug("Successfully retrieved user {Login} for tenant {TenantId}", login, tenantId);
                return userDto;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting user {Login} for tenant {TenantId}", login, tenantId);
                return null;
            }
        }

        /// <inheritdoc/>
        public async Task<UserAccountDto?> GetUserAccountAsync(string tenantId, ulong login)
        {
            try
            {
                _logger.LogDebug("Getting user account {Login} for tenant {TenantId}", login, tenantId);

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return null;
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return null;
                }

                // SDK Pattern: Use pre-created objects
                if (_account == null)
                {
                    _logger.LogError("MT5 account object not initialized");
                    return null;
                }

                // Clear account object first (SDK pattern)
                _account.Clear();

                // Use UserAccountRequest method (same as SDK examples)
                var result = mt5Manager.UserAccountRequest(login, _account);
                if (result != MTRetCode.MT_RET_OK)
                {
                    _logger.LogWarning("Account for user {Login} not found in tenant {TenantId}: {Result}", login, tenantId, result);
                    return null;
                }

                var accountDto = UserAccountDto.FromMT5Account(_account, tenantId);

                _logger.LogDebug("Successfully retrieved account for user {Login} in tenant {TenantId}", login, tenantId);
                return accountDto;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting user account {Login} for tenant {TenantId}", login, tenantId);
                return null;
            }
        }

        /// <inheritdoc/>
        public async Task<IEnumerable<UserDto>> GetUsersAsync(string tenantId, string? group = null)
        {
            try
            {
                _logger.LogDebug("Getting users for tenant {TenantId}, group: {Group}", tenantId, group ?? "all");

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return Enumerable.Empty<UserDto>();
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return Enumerable.Empty<UserDto>();
                }

                var users = new List<UserDto>();
                var groupFilter = group ?? "*";

                // Try to get users by group first (more efficient)
                if (groupFilter != "*")
                {
                    var userArray = mt5Manager.UserCreateArray();
                    if (userArray != null)
                    {
                        var result = mt5Manager.UserGetByGroup(groupFilter, userArray);
                        if (result == MTRetCode.MT_RET_OK)
                        {
                            var total = userArray.Total();
                            for (uint i = 0; i < total; i++)
                            {
                                var user = userArray.Next(i);
                                if (user != null)
                                {
                                    // Get account info for each user
                                    var account = mt5Manager.UserCreateAccount();
                                    if (account != null)
                                    {
                                        mt5Manager.UserAccountGet(user.Login(), account);
                                    }

                                    var userDto = UserDto.FromMT5User(user, account, tenantId);
                                    users.Add(userDto);

                                    account?.Release();
                                }
                            }
                        }
                        userArray.Release();
                    }
                }
                else
                {
                    // Get all users - fallback method
                    var totalUsers = mt5Manager.UserTotal();
                    var processedUsers = 0;

                    for (ulong login = 1; login <= Math.Min(totalUsers * 2, 10000) && processedUsers < 1000; login++)
                    {
                        var user = mt5Manager.UserCreate();
                        if (user != null)
                        {
                            var result = mt5Manager.UserGet(login, user);
                            if (result == MTRetCode.MT_RET_OK)
                            {
                                // Get account info
                                var account = mt5Manager.UserCreateAccount();
                                if (account != null)
                                {
                                    mt5Manager.UserAccountGet(login, account);
                                }

                                var userDto = UserDto.FromMT5User(user, account, tenantId);
                                users.Add(userDto);

                                account?.Release();
                                processedUsers++;
                            }
                            user.Release();
                        }
                    }
                }

                _logger.LogInformation("Retrieved {UserCount} users for tenant {TenantId}", users.Count, tenantId);
                return users;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting users for tenant {TenantId}", tenantId);
                return Enumerable.Empty<UserDto>();
            }
        }

        /// <inheritdoc/>
        public async Task<IEnumerable<UserDto>> GetUsersByGroupAsync(string tenantId, string group)
        {
            return await GetUsersAsync(tenantId, group);
        }

        /// <inheritdoc/>
        public async Task<UserOperationResult> CreateUserAsync(string tenantId, CreateUserRequest request)
        {
            try
            {
                _logger.LogInformation("Creating user {Login} in group {Group} for tenant {TenantId}",
                    request.Login, request.Group, tenantId);

                // Validate request
                var validation = request.Validate();
                if (!validation.IsValid)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, validation.ErrorMessage!);
                }

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PERMISSIONS, "Invalid or inactive tenant");
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_CONNECTION, "MT5 manager not available");
                }

                // Check if user already exists using SDK pattern
                var existingUser = mt5Manager.UserCreate();
                if (existingUser != null)
                {
                    existingUser.Clear();
                    var checkResult = mt5Manager.UserRequest(request.Login, existingUser);
                    if (checkResult == MTRetCode.MT_RET_OK)
                    {
                        existingUser.Release();
                        return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_DUPLICATE, "User already exists");
                    }
                    existingUser.Release();
                }

                // Create new user
                var user = mt5Manager.UserCreate();
                if (user == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_DATA, "Failed to create user object");
                }

                // Set user properties
                user.Login(request.Login);
                user.Name(request.Name);
                user.Group(request.Group);
                user.Country(request.Country ?? "");
                user.City(request.City ?? "");
                user.State(request.State ?? "");
                user.ZIPCode(request.ZipCode ?? "");
                user.Address(request.Address ?? "");
                user.Phone(request.Phone ?? "");
                user.Comment(request.Comment ?? "");
                user.EMail(request.Email ?? "");
                user.Leverage((uint)request.Leverage);

                // Set user rights
                var rights = CIMTUser.EnUsersRights.USER_RIGHT_ENABLED;
                if (!request.EnableUser)
                {
                    rights = CIMTUser.EnUsersRights.USER_RIGHT_NONE;
                }
                user.Rights(rights);

                // Add user to MT5
                var result = mt5Manager.UserAdd(user, "", ""); // Empty master and investor passwords
                if (result != MTRetCode.MT_RET_OK)
                {
                    user.Release();
                    return UserOperationResult.CreateFailure(result, $"Failed to create user: {result}");
                }

                // If initial balance is specified, add it
                ulong? dealId = null;
                if (request.InitialBalance != 0)
                {
                    var balanceResult = mt5Manager.DealerBalance(
                        request.Login,
                        request.InitialBalance,
                        (uint)CIMTDeal.EnDealAction.DEAL_BALANCE,
                        "Initial balance",
                        out ulong balanceDealId);

                    if (balanceResult == MTRetCode.MT_RET_OK)
                    {
                        dealId = balanceDealId;
                    }
                }

                // Get created user for response
                var createdUser = await GetUserAsync(tenantId, request.Login);
                if (createdUser == null)
                {
                    user.Release();
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_DATA, "User created but failed to retrieve");
                }

                user.Release();

                _logger.LogInformation("Successfully created user {Login} for tenant {TenantId}", request.Login, tenantId);
                return UserOperationResult.CreateSuccess($"User {request.Login} created successfully", createdUser, dealId);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception creating user for tenant {TenantId}", tenantId);
                return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERROR, $"Exception: {ex.Message}");
            }
        }

        /// <inheritdoc/>
        public async Task<UserOperationResult> UpdateUserAsync(string tenantId, UpdateUserRequest request)
        {
            try
            {
                _logger.LogInformation("Updating user {Login} for tenant {TenantId}", request.Login, tenantId);

                // Validate request
                var validation = request.Validate();
                if (!validation.IsValid)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, validation.ErrorMessage!);
                }

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PERMISSIONS, "Invalid or inactive tenant");
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_CONNECTION, "MT5 manager not available");
                }

                // Get existing user
                var user = mt5Manager.UserCreate();
                if (user == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_DATA, "Failed to create user object");
                }

                // Clear user object first (SDK pattern)
                user.Clear();

                var getResult = mt5Manager.UserRequest(request.Login, user);
                if (getResult != MTRetCode.MT_RET_OK)
                {
                    user.Release();
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_NOTFOUND, "User not found");
                }

                // Update user properties
                if (!string.IsNullOrEmpty(request.Name)) user.Name(request.Name);
                if (!string.IsNullOrEmpty(request.Group)) user.Group(request.Group);
                if (!string.IsNullOrEmpty(request.Country)) user.Country(request.Country);
                if (!string.IsNullOrEmpty(request.City)) user.City(request.City);
                if (!string.IsNullOrEmpty(request.State)) user.State(request.State);
                if (!string.IsNullOrEmpty(request.ZipCode)) user.ZIPCode(request.ZipCode);
                if (!string.IsNullOrEmpty(request.Address)) user.Address(request.Address);
                if (!string.IsNullOrEmpty(request.Phone)) user.Phone(request.Phone);
                if (!string.IsNullOrEmpty(request.Comment)) user.Comment(request.Comment);
                if (!string.IsNullOrEmpty(request.Email)) user.EMail(request.Email);
                if (request.Leverage.HasValue) user.Leverage((uint)request.Leverage.Value);

                // Update user rights
                if (request.EnableUser.HasValue)
                {
                    var rights = request.EnableUser.Value
                        ? CIMTUser.EnUsersRights.USER_RIGHT_ENABLED
                        : CIMTUser.EnUsersRights.USER_RIGHT_NONE;
                    user.Rights(rights);
                }

                // Update user in MT5
                var result = mt5Manager.UserUpdate(user);
                if (result != MTRetCode.MT_RET_OK)
                {
                    user.Release();
                    return UserOperationResult.CreateFailure(result, $"Failed to update user: {result}");
                }

                // Get updated user for response
                var updatedUser = await GetUserAsync(tenantId, request.Login);
                user.Release();

                _logger.LogInformation("Successfully updated user {Login} for tenant {TenantId}", request.Login, tenantId);
                return UserOperationResult.CreateSuccess($"User {request.Login} updated successfully", updatedUser);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception updating user {Login} for tenant {TenantId}", request.Login, tenantId);
                return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERROR, $"Exception: {ex.Message}");
            }
        }

        /// <inheritdoc/>
        public async Task<UserOperationResult> DeleteUserAsync(string tenantId, ulong login)
        {
            try
            {
                _logger.LogInformation("Deleting user {Login} for tenant {TenantId}", login, tenantId);

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PERMISSIONS, "Invalid or inactive tenant");
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_CONNECTION, "MT5 manager not available");
                }

                // Note: MT5 Manager API doesn't have direct user deletion
                // We can disable the user instead
                var user = mt5Manager.UserCreate();
                if (user == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_DATA, "Failed to create user object");
                }

                // Clear user object first (SDK pattern)
                user.Clear();

                var getResult = mt5Manager.UserRequest(login, user);
                if (getResult != MTRetCode.MT_RET_OK)
                {
                    user.Release();
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_NOTFOUND, "User not found");
                }

                // Disable the user
                user.Rights(CIMTUser.EnUsersRights.USER_RIGHT_NONE);
                var result = mt5Manager.UserUpdate(user);

                user.Release();

                if (result != MTRetCode.MT_RET_OK)
                {
                    return UserOperationResult.CreateFailure(result, $"Failed to disable user: {result}");
                }

                _logger.LogInformation("Successfully disabled user {Login} for tenant {TenantId}", login, tenantId);
                return UserOperationResult.CreateSuccess($"User {login} disabled successfully");
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception deleting user {Login} for tenant {TenantId}", login, tenantId);
                return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERROR, $"Exception: {ex.Message}");
            }
        }

        /// <inheritdoc/>
        public async Task<UserOperationResult> UpdateUserBalanceAsync(string tenantId, UpdateBalanceRequest request)
        {
            try
            {
                _logger.LogInformation("Updating balance for user {Login} by {Amount} in tenant {TenantId}",
                    request.Login, request.Amount, tenantId);

                // Validate request
                var validation = request.Validate();
                if (!validation.IsValid)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PARAMS, validation.ErrorMessage!);
                }

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_PERMISSIONS, "Invalid or inactive tenant");
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_CONNECTION, "MT5 manager not available");
                }

                // Verify user exists using SDK pattern
                var user = mt5Manager.UserCreate();
                if (user != null)
                {
                    user.Clear();
                    var checkResult = mt5Manager.UserRequest(request.Login, user);
                    if (checkResult != MTRetCode.MT_RET_OK)
                    {
                        user.Release();
                        return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_NOTFOUND, "User not found");
                    }
                    user.Release();
                }

                // Update balance using DealerBalance
                ulong dealId;
                var result = mt5Manager.DealerBalance(
                    request.Login,
                    request.Amount,
                    (uint)GetDealActionFromType(request.DealType),
                    request.Comment ?? "Balance adjustment",
                    out dealId);

                if (result != MTRetCode.MT_RET_OK)
                {
                    return UserOperationResult.CreateFailure(result, $"Failed to update balance: {result}");
                }

                _logger.LogInformation("Successfully updated balance for user {Login} by {Amount}, Deal ID: {DealId}",
                    request.Login, request.Amount, dealId);

                return UserOperationResult.CreateSuccess(
                    $"Balance updated by {request.Amount} for user {request.Login}",
                    dealId: dealId);
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception updating balance for user {Login} in tenant {TenantId}", request.Login, tenantId);
                return UserOperationResult.CreateFailure(MTRetCode.MT_RET_ERROR, $"Exception: {ex.Message}");
            }
        }

        /// <inheritdoc/>
        public async Task<IEnumerable<GroupDto>> GetGroupsAsync(string tenantId)
        {
            try
            {
                _logger.LogDebug("Getting groups for tenant {TenantId}", tenantId);

                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return Enumerable.Empty<GroupDto>();
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return Enumerable.Empty<GroupDto>();
                }

                var groups = new List<GroupDto>();
                var totalGroups = mt5Manager.GroupTotal();

                for (uint i = 0; i < totalGroups; i++)
                {
                    var group = mt5Manager.GroupCreate();
                    if (group != null)
                    {
                        var result = mt5Manager.GroupNext(i, group);
                        if (result == MTRetCode.MT_RET_OK)
                        {
                            var groupDto = GroupDto.FromMT5Group(group, tenantId);
                            groups.Add(groupDto);
                        }
                        group.Dispose();
                    }
                }

                _logger.LogInformation("Retrieved {GroupCount} groups for tenant {TenantId}", groups.Count, tenantId);
                return groups;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting groups for tenant {TenantId}", tenantId);
                return Enumerable.Empty<GroupDto>();
            }
        }

        /// <inheritdoc/>
        public async Task<GroupOperationResult> CreateGroupAsync(string tenantId, CreateGroupRequest request)
        {
            // Group creation implementation would go here
            // For now, return not implemented
            return GroupOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_NOTIMPLEMENT, "Group creation not implemented");
        }

        /// <inheritdoc/>
        public async Task<GroupOperationResult> UpdateGroupAsync(string tenantId, UpdateGroupRequest request)
        {
            // Group update implementation would go here
            return GroupOperationResult.CreateFailure(MTRetCode.MT_RET_ERR_NOTIMPLEMENT, "Group update not implemented");
        }

        /// <inheritdoc/>
        public async Task<int> GetTotalUsersAsync(string tenantId)
        {
            try
            {
                var tenant = _tenantManager.GetTenant(tenantId);
                if (tenant == null || tenant.Status != MultiTenant.Models.TenantStatus.Active)
                {
                    return 0;
                }

                var mt5Manager = GetMT5ManagerForTenant(tenantId);
                if (mt5Manager == null)
                {
                    return 0;
                }

                return (int)mt5Manager.UserTotal();
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception getting total users for tenant {TenantId}", tenantId);
                return 0;
            }
        }

        /// <inheritdoc/>
        public async Task<IEnumerable<UserDto>> SearchUsersAsync(string tenantId, UserSearchCriteria criteria)
        {
            try
            {
                // Validate criteria
                var validation = criteria.Validate();
                if (!validation.IsValid)
                {
                    _logger.LogWarning("Invalid search criteria: {Error}", validation.ErrorMessage);
                    return Enumerable.Empty<UserDto>();
                }

                var allUsers = await GetUsersAsync(tenantId);
                var filteredUsers = new List<UserDto>();

                foreach (var user in allUsers)
                {
                    var matches = true;

                    // Apply filters
                    if (criteria.Login.HasValue && user.Login != criteria.Login.Value)
                        matches = false;

                    if (!string.IsNullOrEmpty(criteria.Name) &&
                        (string.IsNullOrEmpty(user.Name) || !user.Name.Contains(criteria.Name, StringComparison.OrdinalIgnoreCase)))
                        matches = false;

                    if (!string.IsNullOrEmpty(criteria.Group) && user.Group != criteria.Group)
                        matches = false;

                    if (!string.IsNullOrEmpty(criteria.Email) &&
                        (string.IsNullOrEmpty(user.Email) || !user.Email.Contains(criteria.Email, StringComparison.OrdinalIgnoreCase)))
                        matches = false;

                    if (!string.IsNullOrEmpty(criteria.Country) && user.Country != criteria.Country)
                        matches = false;

                    if (!string.IsNullOrEmpty(criteria.City) &&
                        (string.IsNullOrEmpty(user.City) || !user.City.Contains(criteria.City, StringComparison.OrdinalIgnoreCase)))
                        matches = false;

                    if (criteria.IsEnabled.HasValue && user.IsEnabled != criteria.IsEnabled.Value)
                        matches = false;

                    if (criteria.MinimumBalance.HasValue && user.Balance < criteria.MinimumBalance.Value)
                        matches = false;

                    if (criteria.MaximumBalance.HasValue && user.Balance > criteria.MaximumBalance.Value)
                        matches = false;

                    if (matches)
                    {
                        filteredUsers.Add(user);
                    }
                }

                // Apply pagination
                var result = filteredUsers
                    .Skip(criteria.Offset)
                    .Take(criteria.Limit)
                    .ToList();

                _logger.LogInformation("Search returned {ResultCount} users out of {TotalCount} for tenant {TenantId}",
                    result.Count, filteredUsers.Count, tenantId);

                return result;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Exception searching users for tenant {TenantId}", tenantId);
                return Enumerable.Empty<UserDto>();
            }
        }

        private CIMTManagerAPI? GetMT5ManagerForTenant(string tenantId)
        {
            // In a real implementation, this would get the appropriate MT5 manager for the tenant
            // For now, we'll use the connection service directly
            return (CIMTManagerAPI)_connectionService.Manager;
        }

        private CIMTDeal.EnDealAction GetDealActionFromType(DealType dealType)
        {
            return dealType switch
            {
                DealType.Balance => CIMTDeal.EnDealAction.DEAL_BALANCE,
                DealType.Credit => CIMTDeal.EnDealAction.DEAL_CREDIT,
                DealType.Bonus => CIMTDeal.EnDealAction.DEAL_CREDIT, // Use credit for bonus
                DealType.Commission => CIMTDeal.EnDealAction.DEAL_COMMISSION,
                DealType.Correction => CIMTDeal.EnDealAction.DEAL_BALANCE, // Use balance for corrections
                _ => CIMTDeal.EnDealAction.DEAL_BALANCE
            };
        }

        /// <inheritdoc/>
        public void Dispose()
        {
            if (_disposed) return;

            try
            {
                // Dispose MT5 objects following SDK pattern
                if (_userArray != null)
                {
                    _userArray.Dispose();
                    _userArray = null;
                }
                if (_account != null)
                {
                    _account.Dispose();
                    _account = null;
                }
                if (_user != null)
                {
                    _user.Dispose();
                    _user = null;
                }

                _disposed = true;
            }
            catch (Exception ex)
            {
                _logger.LogError(ex, "Error disposing UserManagementService");
            }
        }
    }
}