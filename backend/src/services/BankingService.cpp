#include "../include/services/BankingService.h"
#include "../include/models/Account.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <random>
#include <chrono>

BankingService::BankingService(const std::string& data_directory) {
    std::cout << "Creating BankingService with data directory: " << data_directory << std::endl;
    database = std::make_unique<Database>(data_directory);
    std::cout << "BankingService constructor completed." << std::endl;
}

bool BankingService::initialize() {
    std::cout << "Initializing database..." << std::endl;
    if (!database->initialize()) {
        std::cout << "Database initialization failed!" << std::endl;
        return false;
    }
      std::cout << "Database initialized successfully." << std::endl;
    
    // Create sample data if database is empty
    if (database->getUserCount() == 0) {
        std::cout << "Creating sample data..." << std::endl;
        database->createSampleData();
        std::cout << "Sample data created." << std::endl;
    }
    
    std::cout << "BankingService initialization completed." << std::endl;
    return true;
}

std::string BankingService::hashPassword(const std::string& password) {
    // Enhanced hash function using multiple hash iterations
    std::hash<std::string> hasher;
    std::string salted = password + "banking_salt_2024";
    
    // Apply multiple rounds of hashing for better security
    size_t hash1 = hasher(salted);
    size_t hash2 = hasher(std::to_string(hash1) + "extra_salt");
    size_t hash3 = hasher(std::to_string(hash2) + salted);
    
    // Combine hashes and convert to hex string
    std::ostringstream oss;
    oss << std::hex << hash1 << hash2 << hash3;
    return oss.str();
}

bool BankingService::verifyPassword(const std::string& password, const std::string& hash) {
    return hashPassword(password) == hash;
}

bool BankingService::validateAmount(double amount) {
    return amount > 0 && amount <= 1000000; // Max transaction limit
}

bool BankingService::validateAccountNumber(const std::string& account_number) {
    return !account_number.empty() && account_number.length() >= 8;
}

bool BankingService::validateUserId(const std::string& user_id) {
    return !user_id.empty() && user_id.length() >= 6;
}

void BankingService::logActivity(const std::string& user_id, const std::string& activity) {
    // Log user activity for audit trail
    std::cout << "[" << user_id << "] " << activity << std::endl;
}

// Authentication and User Management
AuthResult BankingService::authenticateUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    AuthResult result;
    result.success = false;
    
    User user;
    if (!database->loadUserByUsername(username, user)) {
        result.message = "User not found";
        return result;
    }
    
    if (!user.canLogin()) {
        result.message = "Account is locked or inactive";
        return result;
    }
    
    if (!verifyPassword(password, user.getPasswordHash())) {
        user.incrementFailedLoginAttempts();
        database->updateUser(user);
        result.message = "Invalid password";
        return result;
    }
    
    user.updateLastLogin();
    database->updateUser(user);
    
    result.success = true;
    result.user_id = user.getUserId();
    result.role = user.getRole();
    result.message = "Login successful";
    
    logActivity(user.getUserId(), "User logged in");
    return result;
}

AuthResult BankingService::registerUser(const std::string& username, const std::string& password,
                                       const std::string& email, const std::string& full_name,
                                       const std::string& phone) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    AuthResult result;
    result.success = false;
    
    if (database->userExists(username)) {
        result.message = "Username already exists";
        return result;
    }
    
    if (username.length() < 3 || password.length() < 6) {
        result.message = "Username must be at least 3 characters, password at least 6 characters";
        return result;
    }
    
    User new_user(username, hashPassword(password), email, full_name, phone);
    
    if (database->saveUser(new_user)) {
        result.success = true;
        result.user_id = new_user.getUserId();
        result.role = new_user.getRole();
        result.message = "Registration successful";
        
        logActivity(new_user.getUserId(), "User registered");
    } else {
        result.message = "Failed to save user";
    }
    
    return result;
}

// User lookup helper - FIXED: Remove mutex to avoid deadlock
bool BankingService::getUserByUsername(const std::string& username, User& user) {
    std::cout << "[DEBUG] BankingService::getUserByUsername called for: " << username << std::endl;
    
    std::cout << "[DEBUG] Calling database->loadUserByUsername (no mutex)" << std::endl;
    bool result = database->loadUserByUsername(username, user);
    std::cout << "[DEBUG] loadUserByUsername result: " << (result ? "SUCCESS" : "FAILED") << std::endl;
    
    return result;
}


// Account Management
// AccountCreationResult BankingService::createAccount(const std::string& user_id, 
//                                                    const std::string& account_type,
//                                                    double initial_deposit) {
//     std::cout << "[DEBUG] === createAccount START ===" << std::endl;
//     std::cout << "[DEBUG] Parameters: user_id=" << user_id << ", account_type=" << account_type << ", initial_deposit=" << initial_deposit << std::endl;
    
//     std::lock_guard<std::mutex> lock(service_mutex);
//     std::cout << "[DEBUG] Mutex acquired in createAccount" << std::endl;
    
//     AccountCreationResult result;
//     result.success = false;
    
//     std::cout << "[DEBUG] Validating user ID..." << std::endl;
//     if (!validateUserId(user_id)) {
//         result.message = "Invalid user ID";
//         std::cout << "[DEBUG] User ID validation failed" << std::endl;
//         return result;
//     }
//     std::cout << "[DEBUG] User ID validation passed" << std::endl;
    
//     std::cout << "[DEBUG] Loading user from database..." << std::endl;
//     User user;
//     if (!database->loadUser(user_id, user)) {
//         result.message = "User not found";
//         std::cout << "[DEBUG] User not found in database" << std::endl;
//         return result;
//     }
//     std::cout << "[DEBUG] User loaded successfully: " << user.getUsername() << std::endl;
    
//     std::cout << "[DEBUG] Checking if user is active..." << std::endl;
//     if (!user.getIsActive()) {
//         result.message = "User account is inactive";
//         std::cout << "[DEBUG] User account is inactive" << std::endl;
//         return result;
//     }
//     std::cout << "[DEBUG] User is active" << std::endl;
    
//     std::cout << "[DEBUG] Converting account type string to enum..." << std::endl;
//     AccountType type = Account::stringToType(account_type);
//     std::cout << "[DEBUG] Account type converted successfully" << std::endl;
    
//     std::cout << "[DEBUG] Generating account number..." << std::endl;
//     std::string account_number = database->generateAccountNumber();
//     std::cout << "[DEBUG] Generated account number: " << account_number << std::endl;
    
//     std::cout << "[DEBUG] Creating new account object..." << std::endl;
//     Account new_account(account_number, user_id, type, initial_deposit);
//     new_account.setStatus(AccountStatus::ACTIVE);
//     std::cout << "[DEBUG] Account object created" << std::endl;
    
//     std::cout << "[DEBUG] Saving account to database..." << std::endl;
//     if (database->saveAccount(new_account)) {
//         std::cout << "[DEBUG] Account saved successfully" << std::endl;
        
//         std::cout << "[DEBUG] Adding account ID to user..." << std::endl;
//         user.addAccountId(account_number);
//         std::cout << "[DEBUG] Account ID added to user" << std::endl;
        
//         std::cout << "[DEBUG] Updating user in database..." << std::endl;
//         database->updateUser(user);
//         std::cout << "[DEBUG] User updated in database" << std::endl;
        
//         result.success = true;
//         result.account_number = account_number;
//         result.message = "Account created successfully";
        
//         std::cout << "[DEBUG] Logging activity..." << std::endl;
//         logActivity(user_id, "Account created: " + account_number);
//         std::cout << "[DEBUG] Activity logged" << std::endl;
        
//         // Record initial deposit if any
//         if (initial_deposit > 0) {
//             std::cout << "[DEBUG] Recording initial deposit transaction..." << std::endl;
//             Transaction deposit_transaction("", account_number, initial_deposit, 
//                                           TransactionType::DEPOSIT, "Initial deposit");
//             deposit_transaction.setStatus(TransactionStatus::COMPLETED);
//             deposit_transaction.setBalanceBefore(0.0);
//             deposit_transaction.setBalanceAfter(initial_deposit);
//             database->saveTransaction(deposit_transaction);
//             std::cout << "[DEBUG] Initial deposit transaction saved" << std::endl;
//         }
//     } else {
//         result.message = "Failed to create account";
//         std::cout << "[DEBUG] Failed to save account to database" << std::endl;
//     }
    
//     std::cout << "[DEBUG] === createAccount END ===" << std::endl;
//     return result;
// }

AccountCreationResult BankingService::createAccount(const std::string& user_id, 
                                                   const std::string& account_type,
                                                   double initial_deposit) {
    std::cout << "[DEBUG] === createAccount START ===" << std::endl;
    std::cout << "[DEBUG] Parameters: user_id=" << user_id << ", account_type=" << account_type << ", initial_deposit=" << initial_deposit << std::endl;
    
    std::lock_guard<std::mutex> lock(service_mutex);
    std::cout << "[DEBUG] Mutex acquired in createAccount" << std::endl;
    
    AccountCreationResult result;
    result.success = false;
    
    // Validate user ID
    if (!validateUserId(user_id)) {
        result.message = "Invalid user ID";
        std::cout << "[DEBUG] User ID validation failed" << std::endl;
        return result;
    }
    
    // Load user from database
    User user;
    if (!database->loadUser(user_id, user)) {
        result.message = "User not found";
        std::cout << "[DEBUG] User not found in database" << std::endl;
        return result;
    }
    std::cout << "[DEBUG] User loaded successfully: " << user.getUsername() << std::endl;
    
    // Check if user is active
    if (!user.getIsActive()) {
        result.message = "User account is inactive";
        std::cout << "[DEBUG] User account is inactive" << std::endl;
        return result;
    }
    
    // Convert account type
    AccountType type = Account::stringToType(account_type);
    std::cout << "[DEBUG] Account type converted successfully" << std::endl;
    
    // Generate account number
    std::string account_number = database->generateAccountNumber();
    std::cout << "[DEBUG] Generated account number: " << account_number << std::endl;
    
    // Create new account
    Account new_account(account_number, user_id, type, initial_deposit);
    new_account.setStatus(AccountStatus::ACTIVE);
    std::cout << "[DEBUG] Account object created" << std::endl;
    
    // Save account to database
    std::cout << "[DEBUG] About to save account..." << std::endl;
    bool save_result = database->saveAccount(new_account);
    std::cout << "[DEBUG] Save account result: " << (save_result ? "SUCCESS" : "FAILED") << std::endl;
    
    if (save_result) {
        std::cout << "[DEBUG] Account saved successfully" << std::endl;
        
        // Add account ID to user
        user.addAccountId(account_number);
        std::cout << "[DEBUG] Account ID added to user" << std::endl;
        
        // Update user in database
        bool update_result = database->updateUser(user);
        std::cout << "[DEBUG] User update result: " << (update_result ? "SUCCESS" : "FAILED") << std::endl;
        
        result.success = true;
        result.account_number = account_number;
        result.message = "Account created successfully";
        
        logActivity(user_id, "Account created: " + account_number);
        
        // Record initial deposit if any
        if (initial_deposit > 0) {
            std::cout << "[DEBUG] Recording initial deposit transaction..." << std::endl;
            Transaction deposit_transaction("", account_number, initial_deposit, 
                                          TransactionType::DEPOSIT, "Initial deposit");
            deposit_transaction.setStatus(TransactionStatus::COMPLETED);
            deposit_transaction.setBalanceBefore(0.0);
            deposit_transaction.setBalanceAfter(initial_deposit);
            database->saveTransaction(deposit_transaction);
            std::cout << "[DEBUG] Initial deposit transaction saved" << std::endl;
        }
    } else {
        result.message = "Failed to create account";
        std::cout << "[DEBUG] Failed to save account to database" << std::endl;
    }
    
    std::cout << "[DEBUG] === createAccount END ===" << std::endl;
    return result;
}
std::vector<Account> BankingService::getUserAccounts(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(service_mutex);
    return database->getAccountsByCustomerId(user_id);
}

// Transaction Operations
TransactionResult BankingService::deposit(const std::string& account_number, double amount,
                                         const std::string& description) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    TransactionResult result;
    result.success = false;
    
    if (!validateAmount(amount)) {
        result.message = "Invalid amount";
        return result;
    }
    
    Account account;
    if (!database->loadAccount(account_number, account)) {
        result.message = "Account not found";
        return result;
    }
    
    if (!account.isActive()) {
        result.message = "Account is not active";
        return result;
    }
    
    double balance_before = account.getBalance();
    
    if (account.deposit(amount)) {
        database->updateAccount(account);
        
        Transaction transaction("", account_number, amount, TransactionType::DEPOSIT, description);
        transaction.setStatus(TransactionStatus::COMPLETED);
        transaction.setBalanceBefore(balance_before);
        transaction.setBalanceAfter(account.getBalance());
        
        database->saveTransaction(transaction);
        
        result.success = true;
        result.transaction_id = transaction.getTransactionId();
        result.new_balance = account.getBalance();
        result.message = "Deposit successful";
        
        logActivity(account.getCustomerId(), "Deposit: $" + std::to_string(amount) + " to " + account_number);
    } else {
        result.message = "Deposit failed";
    }
    
    return result;
}

TransactionResult BankingService::withdraw(const std::string& account_number, double amount,
                                          const std::string& description) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    TransactionResult result;
    result.success = false;
    
    if (!validateAmount(amount)) {
        result.message = "Invalid amount";
        return result;
    }
    
    Account account;
    if (!database->loadAccount(account_number, account)) {
        result.message = "Account not found";
        return result;
    }
    
    if (!account.isActive()) {
        result.message = "Account is not active";
        return result;
    }
    
    if (!account.canWithdraw(amount)) {
        result.message = "Insufficient funds or exceeds daily limit";
        return result;
    }
    
    double balance_before = account.getBalance();
    
    if (account.withdraw(amount)) {
        database->updateAccount(account);
        
        Transaction transaction(account_number, "", amount, TransactionType::WITHDRAWAL, description);
        transaction.setStatus(TransactionStatus::COMPLETED);
        transaction.setBalanceBefore(balance_before);
        transaction.setBalanceAfter(account.getBalance());
        
        database->saveTransaction(transaction);
        
        result.success = true;
        result.transaction_id = transaction.getTransactionId();
        result.new_balance = account.getBalance();
        result.message = "Withdrawal successful";
        
        logActivity(account.getCustomerId(), "Withdrawal: $" + std::to_string(amount) + " from " + account_number);
    } else {
        result.message = "Withdrawal failed";
    }
    
    return result;
}

TransactionResult BankingService::transfer(const std::string& from_account, const std::string& to_account,
                                          double amount, const std::string& description) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    TransactionResult result;
    result.success = false;
    
    if (!validateAmount(amount)) {
        result.message = "Invalid amount";
        return result;
    }
    
    if (from_account == to_account) {
        result.message = "Cannot transfer to the same account";
        return result;
    }
    
    Account from_acc, to_acc;
    if (!database->loadAccount(from_account, from_acc) || !database->loadAccount(to_account, to_acc)) {
        result.message = "One or both accounts not found";
        return result;
    }
    
    if (!from_acc.isActive() || !to_acc.isActive()) {
        result.message = "One or both accounts are not active";
        return result;
    }
    
    if (!from_acc.canWithdraw(amount)) {
        result.message = "Insufficient funds or exceeds daily limit";
        return result;
    }
    
    double from_balance_before = from_acc.getBalance();
    double to_balance_before = to_acc.getBalance();
    
    if (from_acc.transfer(amount, to_acc)) {
        database->updateAccount(from_acc);
        database->updateAccount(to_acc);
        
        Transaction transaction(from_account, to_account, amount, TransactionType::TRANSFER, description);
        transaction.setStatus(TransactionStatus::COMPLETED);
        transaction.setBalanceBefore(from_balance_before);
        transaction.setBalanceAfter(from_acc.getBalance());
        
        database->saveTransaction(transaction);
        
        result.success = true;
        result.transaction_id = transaction.getTransactionId();
        result.new_balance = from_acc.getBalance();
        result.message = "Transfer successful";
        
        logActivity(from_acc.getCustomerId(), "Transfer: $" + std::to_string(amount) + " from " + from_account + " to " + to_account);
    } else {
        result.message = "Transfer failed";
    }
    
    return result;
}

// Transaction History
std::vector<Transaction> BankingService::getAccountTransactions(const std::string& account_number) {
    std::lock_guard<std::mutex> lock(service_mutex);
    return database->getTransactionsByAccount(account_number);
}

std::vector<Transaction> BankingService::getUserTransactions(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    std::vector<Transaction> all_transactions;
    auto user_accounts = getUserAccounts(user_id);
    
    for (const auto& account : user_accounts) {
        auto account_transactions = database->getTransactionsByAccount(account.getAccountNumber());
        all_transactions.insert(all_transactions.end(), account_transactions.begin(), account_transactions.end());
    }
    
    return all_transactions;
}

bool BankingService::getAccountBalance(const std::string& account_number, double& balance) {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    Account account;
    if (database->loadAccount(account_number, account)) {
        balance = account.getBalance();
        return true;
    }
    return false;
}

bool BankingService::getAccountInfo(const std::string& account_number, Account& account) {
    std::lock_guard<std::mutex> lock(service_mutex);
    return database->loadAccount(account_number, account);
}

// Reports and Analytics (Admin only)
double BankingService::getTotalSystemBalance() {
    std::lock_guard<std::mutex> lock(service_mutex);
    return database->getTotalSystemBalance();
}

size_t BankingService::getTotalUsers() {
    std::lock_guard<std::mutex> lock(service_mutex);
    return database->getUserCount();
}

size_t BankingService::getTotalAccounts() {
    std::lock_guard<std::mutex> lock(service_mutex);
    return database->getAccountCount();
}

std::string BankingService::getSystemStatus() {
    std::lock_guard<std::mutex> lock(service_mutex);
    
    std::ostringstream status;
    status << "{"
           << "\"total_users\":" << getTotalUsers() << ","
           << "\"total_accounts\":" << getTotalAccounts() << ","
           << "\"total_balance\":" << std::fixed << std::setprecision(2) << getTotalSystemBalance() << ","
           << "\"status\":\"ONLINE\""
           << "}";
    
    return status.str();
}
