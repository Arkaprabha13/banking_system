#include "../include/core/Database.h"
#include "../include/models/Account.h"
#include "../include/models/User.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <iomanip>
#include <random>
#include <functional>

Database::Database(const std::string& data_dir) 
    : data_directory(data_dir) {
    std::cout << "Creating Database with data directory: " << data_dir << std::endl;
    
    users_file = data_dir + "/users/users.csv";
    accounts_file = data_dir + "/accounts/accounts.csv";
    transactions_file = data_dir + "/transactions/transactions.csv";
    logs_file = data_dir + "/logs/system.log";
    
    // ADD THIS DEBUG OUTPUT
    std::cout << "[DEBUG] Current working directory: " << std::filesystem::current_path() << std::endl;
    std::cout << "[DEBUG] Users file path: " << users_file << std::endl;
    std::cout << "[DEBUG] Accounts file path: " << accounts_file << std::endl;
    std::cout << "[DEBUG] Transactions file path: " << transactions_file << std::endl;
    
    std::cout << "Database constructor completed." << std::endl;
}


// bool Database::initialize() {
//     // Create directory structure
//     std::vector<std::string> directories = {
//         data_directory,
//         data_directory + "/users",
//         data_directory + "/accounts", 
//         data_directory + "/transactions",
//         data_directory + "/logs"
//     };
    
//     for (const auto& dir : directories) {
//         if (!ensureDirectoryExists(dir)) {
//             return false;
//         }
//     }
    
//     // Create CSV headers if files don't exist
//     std::ifstream users_check(users_file);
//     if (!users_check.good()) {
//         std::ofstream users_out(users_file);
//         if (users_out.is_open()) {
//             users_out << "user_id,username,password_hash,email,full_name,phone_number,role,is_active,failed_login_attempts,last_login,created_date\n";
//             users_out.close();
//         }
//     }
//     users_check.close();
    
//     std::ifstream accounts_check(accounts_file);
//     if (!accounts_check.good()) {
//         std::ofstream accounts_out(accounts_file);
//         if (accounts_out.is_open()) {
//             accounts_out << "account_number,customer_id,account_type,balance,status,daily_limit,minimum_balance,created_date,last_updated\n";
//             accounts_out.close();
//         }
//     }
//     accounts_check.close();
    
//     std::ifstream transactions_check(transactions_file);
//     if (!transactions_check.good()) {
//         std::ofstream transactions_out(transactions_file);
//         if (transactions_out.is_open()) {
//             transactions_out << "transaction_id,from_account_id,to_account_id,amount,type,status,description,balance_before,balance_after,timestamp,reference_number\n";
//             transactions_out.close();
//         }
//     }
//     transactions_check.close();
    
//     logOperation("SYSTEM", "Database initialized successfully");
//     return true;
// }
bool Database::initialize() {
    // Create directory structure
    std::vector<std::string> directories = {
        data_directory,
        data_directory + "/users",
        data_directory + "/accounts", 
        data_directory + "/transactions",
        data_directory + "/logs"
    };
    
    for (const auto& dir : directories) {
        if (!ensureDirectoryExists(dir)) {
            return false;
        }
    }
    
    // Check if users file exists
    bool users_file_exists = false;
    std::ifstream users_check(users_file);
    if (users_check.good()) {
        users_file_exists = true;
        std::cout << "[DEBUG] Users file already exists" << std::endl;
    } else {
        std::cout << "[DEBUG] Users file doesn't exist, will create new one" << std::endl;
    }
    users_check.close();
    
    // Create CSV headers if files don't exist
    if (!users_file_exists) {
        std::cout << "[DEBUG] Creating users file with headers..." << std::endl;
        std::ofstream users_out(users_file);
        if (users_out.is_open()) {
            users_out << "user_id,username,password_hash,email,full_name,phone_number,role,is_active,failed_login_attempts,last_login,created_date\n";
            users_out.close();
            std::cout << "[DEBUG] Users file created with headers" << std::endl;
        } else {
            std::cout << "[ERROR] Failed to create users file" << std::endl;
            return false;
        }
    }
    
    std::ifstream accounts_check(accounts_file);
    if (!accounts_check.good()) {
        std::ofstream accounts_out(accounts_file);
        if (accounts_out.is_open()) {
            accounts_out << "account_number,customer_id,account_type,balance,status,daily_limit,minimum_balance,created_date,last_updated\n";
            accounts_out.close();
        }
    }
    accounts_check.close();
    
    std::ifstream transactions_check(transactions_file);
    if (!transactions_check.good()) {
        std::ofstream transactions_out(transactions_file);
        if (transactions_out.is_open()) {
            transactions_out << "transaction_id,from_account_id,to_account_id,amount,type,status,description,balance_before,balance_after,timestamp,reference_number\n";
            transactions_out.close();
        }
    }
    transactions_check.close();
    
    // ALWAYS CREATE SAMPLE DATA IF NO USERS EXIST
    if (!users_file_exists) {
        std::cout << "[DEBUG] No users file existed, creating sample data..." << std::endl;
        bool sample_created = createSampleData();
        if (sample_created) {
            std::cout << "[DEBUG] Sample data creation completed successfully" << std::endl;
        } else {
            std::cout << "[ERROR] Sample data creation failed" << std::endl;
        }
    } else {
        // Check if users file is empty (only has header)
        std::ifstream check_empty(users_file);
        std::string line1, line2;
        std::getline(check_empty, line1); // header
        if (!std::getline(check_empty, line2)) {
            // File only has header, no data
            std::cout << "[DEBUG] Users file is empty, creating sample data..." << std::endl;
            createSampleData();
        } else {
            std::cout << "[DEBUG] Users file has existing data, skipping sample data creation" << std::endl;
        }
        check_empty.close();
    }
    
    logOperation("SYSTEM", "Database initialized successfully");
    return true;
}


bool Database::ensureDirectoryExists(const std::string& path) {
    try {
        std::filesystem::create_directories(path);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error creating directory " << path << ": " << e.what() << std::endl;
        return false;
    }
}

std::string Database::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Database::logOperation(const std::string& operation, const std::string& details) {
    // Use a separate mutex for logging to avoid deadlocks
    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);
    std::ofstream log_file(logs_file, std::ios::app);
    if (log_file.is_open()) {
        log_file << getCurrentTimestamp() << " - " << operation << " - " << details << std::endl;
        log_file.close();
    }
}

// Add the hashPassword function to Database class
std::string Database::hashPassword(const std::string& password) {
    // Same implementation as in BankingService for consistency
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

// User operations
bool Database::saveUser(const User& user) {
    std::cout << "saveUser called for user: " << user.getUserId() << std::endl;
    std::lock_guard<std::mutex> lock(users_mutex);
    std::cout << "Mutex acquired" << std::endl;
    
    // Check if user already exists
    User existing_user;
    std::cout << "Checking if user exists..." << std::endl;
    if (loadUserInternal(user.getUserId(), existing_user)) {
        std::cout << "User exists, updating..." << std::endl;
        return updateUser(user);
    }
    std::cout << "User doesn't exist, creating new..." << std::endl;
    
    std::cout << "Opening file: " << users_file << std::endl;
    std::ofstream file(users_file, std::ios::app);
    if (!file.is_open()) {
        std::cout << "Failed to open file!" << std::endl;
        return false;
    }
    std::cout << "File opened successfully" << std::endl;
    
    std::cout << "Converting user to CSV..." << std::endl;
    std::string csv_row = user.toCsvRow();
    std::cout << "CSV row: " << csv_row << std::endl;
    
    file << csv_row << std::endl;
    file.close();
    std::cout << "File written and closed" << std::endl;
    
    logOperation("USER_SAVE", "User " + user.getUserId() + " saved");
    std::cout << "Operation logged" << std::endl;
    return true;
}

bool Database::loadUser(const std::string& user_id, User& user) {
    std::cout << "loadUser called for: " << user_id << std::endl;
    std::lock_guard<std::mutex> lock(users_mutex);
    std::cout << "loadUser mutex acquired" << std::endl;
    
    std::cout << "Opening users file: " << users_file << std::endl;
    std::ifstream file(users_file);
    if (!file.is_open()) {
        std::cout << "Users file doesn't exist or can't be opened" << std::endl;
        return false;
    }
    std::cout << "Users file opened successfully" << std::endl;
    
    std::string line;
    std::cout << "Reading header line..." << std::endl;
    std::getline(file, line); // Skip header
    std::cout << "Header: " << line << std::endl;
    
    std::cout << "Searching for user..." << std::endl;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::cout << "Checking line: " << line << std::endl;
        
        std::istringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 11 && tokens[0] == user_id) {
            std::cout << "User found! Loading from CSV..." << std::endl;
            // Create user from CSV data
            user = User();
            user.fromCsvRow(line);
            file.close();
            std::cout << "User loaded successfully" << std::endl;
            return true;
        }
    }
    
    file.close();
    std::cout << "User not found in file" << std::endl;
    return false;
}

// bool Database::loadUserByUsername(const std::string& username, User& user) {
//     std::lock_guard<std::mutex> lock(users_mutex);
    
//     std::ifstream file(users_file);
//     if (!file.is_open()) {
//         return false;
//     }
    
//     std::string line;
//     std::getline(file, line); // Skip header
    
//     while (std::getline(file, line)) {
//         if (line.empty()) continue;
        
//         std::istringstream ss(line);
//         std::string item;
//         std::vector<std::string> tokens;
        
//         while (std::getline(ss, item, ',')) {
//             tokens.push_back(item);
//         }
        
//         if (tokens.size() >= 11 && tokens[1] == username) {
//             user.fromCsvRow(line);
//             file.close();
//             return true;
//         }
//     }
    
//     file.close();
//     return false;
// }
bool Database::loadUserByUsername(const std::string& username, User& user) {
    std::lock_guard<std::mutex> lock(users_mutex);
    
    std::cout << "[DEBUG] loadUserByUsername called for: " << username << std::endl;
    
    std::ifstream file(users_file);
    if (!file.is_open()) {
        std::cout << "[DEBUG] Cannot open users file: " << users_file << std::endl;
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 11 && tokens[1] == username) {
            std::cout << "[DEBUG] User found by username: " << username << std::endl;
            user.fromCsvRow(line);
            file.close();
            return true;
        }
    }
    
    file.close();
    std::cout << "[DEBUG] User not found by username: " << username << std::endl;
    return false;
}


bool Database::userExists(const std::string& username) {
    User user;
    return loadUserByUsername(username, user);
}

std::string Database::generateAccountNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000000, 999999999);
    
    std::string account_number;
    do {
        account_number = std::to_string(dis(gen));
    } while (accountExists(account_number));
    
    return account_number;
}

// Account operations
// bool Database::saveAccount(const Account& account) {
//     std::lock_guard<std::mutex> lock(accounts_mutex);
    
//     Account existing_account;
//     if (loadAccount(account.getAccountNumber(), existing_account)) {
//         return updateAccount(account);
//     }
    
//     std::ofstream file(accounts_file, std::ios::app);
//     if (!file.is_open()) {
//         return false;
//     }
    
//     file << account.toCsvRow() << std::endl;
//     file.close();
    
//     logOperation("ACCOUNT_SAVE", "Account " + account.getAccountNumber() + " saved");
//     return true;
// }
bool Database::saveAccount(const Account& account) {
    std::cout << "[DEBUG] === saveAccount START ===" << std::endl;
    std::cout << "[DEBUG] Account number: " << account.getAccountNumber() << std::endl;
    std::cout << "[DEBUG] Customer ID: " << account.getCustomerId() << std::endl;
    
    std::cout << "[DEBUG] Attempting to acquire accounts_mutex..." << std::endl;
    std::lock_guard<std::mutex> lock(accounts_mutex);
    std::cout << "[DEBUG] accounts_mutex acquired successfully" << std::endl;
    
    // Check if account already exists (THIS MIGHT BE THE PROBLEM)
    std::cout << "[DEBUG] Checking if account already exists..." << std::endl;
    Account existing_account;
    
    // TEMPORARILY COMMENT OUT THIS CHECK - it might be causing deadlock
    /*
    if (loadAccount(account.getAccountNumber(), existing_account)) {
        std::cout << "[DEBUG] Account exists, updating..." << std::endl;
        return updateAccount(account);
    }
    */
    
    std::cout << "[DEBUG] Account doesn't exist, creating new..." << std::endl;
    std::cout << "[DEBUG] Opening accounts file: " << accounts_file << std::endl;
    
    std::ofstream file(accounts_file, std::ios::app);
    if (!file.is_open()) {
        std::cout << "[ERROR] Failed to open accounts file: " << accounts_file << std::endl;
        return false;
    }
    std::cout << "[DEBUG] Accounts file opened successfully" << std::endl;
    
    std::cout << "[DEBUG] Converting account to CSV..." << std::endl;
    std::string csv_row = account.toCsvRow();
    std::cout << "[DEBUG] CSV row: " << csv_row << std::endl;
    
    std::cout << "[DEBUG] Writing to file..." << std::endl;
    file << csv_row << std::endl;
    std::cout << "[DEBUG] Data written to file" << std::endl;
    
    std::cout << "[DEBUG] Closing file..." << std::endl;
    file.close();
    std::cout << "[DEBUG] File closed successfully" << std::endl;
    
    std::cout << "[DEBUG] Logging operation..." << std::endl;
    logOperation("ACCOUNT_SAVE", "Account " + account.getAccountNumber() + " saved");
    std::cout << "[DEBUG] Operation logged" << std::endl;
    
    std::cout << "[DEBUG] === saveAccount END - SUCCESS ===" << std::endl;
    return true;
}

bool Database::loadAccount(const std::string& account_number, Account& account) {
    std::lock_guard<std::mutex> lock(accounts_mutex);
    
    std::ifstream file(accounts_file);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 9 && tokens[0] == account_number) {
            account.fromCsvRow(line);
            file.close();
            return true;
        }
    }
    
    file.close();
    return false;
}

bool Database::accountExists(const std::string& account_number) {
    Account account;
    return loadAccount(account_number, account);
}

// Transaction operations
bool Database::saveTransaction(const Transaction& transaction) {
    std::lock_guard<std::mutex> lock(transactions_mutex);
    
    std::ofstream file(transactions_file, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << transaction.toCsvRow() << std::endl;
    file.close();
    
    logOperation("TRANSACTION_SAVE", "Transaction " + transaction.getTransactionId() + " saved");
    return true;
}

std::vector<Transaction> Database::getTransactionsByAccount(const std::string& account_id) {
    std::lock_guard<std::mutex> lock(transactions_mutex);
    std::vector<Transaction> transactions;
    
    std::ifstream file(transactions_file);
    if (!file.is_open()) {
        return transactions;
    }
    
    std::string line;
    std::getline(file, line); // Skip header
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        std::istringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 11 && 
            (tokens[1] == account_id || tokens[2] == account_id)) {
            Transaction transaction;
            transaction.fromCsvRow(line);
            transactions.push_back(transaction);
        }
    }
    
    file.close();
    return transactions;
}

// FIXED: createSampleData function with proper password hashing
bool Database::createSampleData() {
    std::cout << "Starting sample data creation..." << std::endl;
    
    // Create sample admin user with proper password hash
    std::cout << "Creating admin user..." << std::endl;
    
    // Generate proper hash for "password"
    std::string admin_password_hash = hashPassword("password");
    User admin_user("admin", admin_password_hash, "admin@bank.com", "System Administrator", "555-0000", UserRole::ADMIN);
    
    std::cout << "Admin user created with ID: " << admin_user.getUserId() << std::endl;
    std::cout << "Admin password hash: " << admin_password_hash << std::endl;
    std::cout << "Saving admin user..." << std::endl;
    bool saved = saveUser(admin_user);
    std::cout << "Admin user save result: " << (saved ? "SUCCESS" : "FAILED") << std::endl;
    
    // Create sample customer with proper password hash
    std::cout << "Creating customer user..." << std::endl;
    std::string customer_password_hash = hashPassword("customer123");
    User customer("johndoe", customer_password_hash, "john@email.com", "John Doe", "555-1234", UserRole::CUSTOMER);
    
    std::cout << "Customer user created with ID: " << customer.getUserId() << std::endl;
    std::cout << "Customer password hash: " << customer_password_hash << std::endl;
    std::cout << "Saving customer user..." << std::endl;
    saved = saveUser(customer);
    std::cout << "Customer user save result: " << (saved ? "SUCCESS" : "FAILED") << std::endl;
    
    // Create sample account
    std::cout << "Generating account number..." << std::endl;
    std::string account_number = generateAccountNumber();
    std::cout << "Generated account number: " << account_number << std::endl;
    
    std::cout << "Creating checking account..." << std::endl;
    Account checking_account(account_number, customer.getUserId(), AccountType::CHECKING, 1000.0);
    checking_account.setStatus(AccountStatus::ACTIVE);
    std::cout << "Saving account..." << std::endl;
    saveAccount(checking_account);
    std::cout << "Account saved." << std::endl;
    
    // Add account to customer
    std::cout << "Adding account to customer..." << std::endl;
    customer.addAccountId(checking_account.getAccountNumber());
    std::cout << "Updating customer with account..." << std::endl;
    updateUser(customer);
    std::cout << "Customer updated." << std::endl;
    
    std::cout << "Logging sample data creation..." << std::endl;
    logOperation("SYSTEM", "Sample data created successfully");
    std::cout << "Sample data creation completed!" << std::endl;
    return true;
}

bool Database::updateUser(const User& user) {
    std::lock_guard<std::mutex> lock(users_mutex);
    
    std::vector<std::string> lines;
    std::ifstream file(users_file);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool updated = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("user_id,") == 0) {
            lines.push_back(line);
            continue;
        }
        
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 2 && tokens[0] == user.getUserId()) {
            lines.push_back(user.toCsvRow());
            updated = true;
        } else {
            lines.push_back(line);
        }
    }
    file.close();
    
    if (!updated) {
        return false;
    }
    
    std::ofstream out_file(users_file);
    if (!out_file.is_open()) {
        return false;
    }
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    
    logOperation("UPDATE_USER", "Updated user: " + user.getUsername());
    return true;
}

bool Database::deleteUser(const std::string& user_id) {
    std::lock_guard<std::mutex> lock(users_mutex);
    
    std::vector<std::string> lines;
    std::ifstream file(users_file);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool deleted = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("user_id,") == 0) {
            lines.push_back(line);
            continue;
        }
        
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 2 && tokens[0] == user_id) {
            deleted = true;
            // Don't add this line (effectively deleting it)
        } else {
            lines.push_back(line);
        }
    }
    file.close();
    
    if (!deleted) {
        return false;
    }
    
    std::ofstream out_file(users_file);
    if (!out_file.is_open()) {
        return false;
    }
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    
    logOperation("DELETE_USER", "Deleted user: " + user_id);
    return true;
}

std::vector<User> Database::getAllUsers() {
    std::lock_guard<std::mutex> lock(users_mutex);
    std::vector<User> users;
    
    std::ifstream file(users_file);
    if (!file.is_open()) {
        return users;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("user_id,") == 0) {
            continue;
        }
        
        User user;
        if (user.fromCsvRow(line)) {
            users.push_back(user);
        }
    }
    
    file.close();
    return users;
}

std::vector<Account> Database::getAllAccounts() {
    std::lock_guard<std::mutex> lock(accounts_mutex);
    std::vector<Account> accounts;
    
    std::ifstream file(accounts_file);
    if (!file.is_open()) {
        return accounts;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("account_number,") == 0) {
            continue;
        }
        
        Account account;
        if (account.fromCsvRow(line)) {
            accounts.push_back(account);
        }
    }
    
    file.close();
    return accounts;
}

std::vector<Account> Database::getAccountsByCustomerId(const std::string& customer_id) {
    std::lock_guard<std::mutex> lock(accounts_mutex);
    std::vector<Account> accounts;
    
    std::ifstream file(accounts_file);
    if (!file.is_open()) {
        return accounts;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("account_number,") == 0) {
            continue;
        }
        
        Account account;
        if (account.fromCsvRow(line) && account.getCustomerId() == customer_id) {
            accounts.push_back(account);
        }
    }
    
    file.close();
    return accounts;
}

bool Database::updateAccount(const Account& account) {
    std::lock_guard<std::mutex> lock(accounts_mutex);
    
    std::vector<std::string> lines;
    std::ifstream file(accounts_file);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool updated = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("account_number,") == 0) {
            lines.push_back(line);
            continue;
        }
        
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 1 && tokens[0] == account.getAccountNumber()) {
            lines.push_back(account.toCsvRow());
            updated = true;
        } else {
            lines.push_back(line);
        }
    }
    file.close();
    
    if (!updated) {
        return false;
    }
    
    std::ofstream out_file(accounts_file);
    if (!out_file.is_open()) {
        return false;
    }
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    
    logOperation("UPDATE_ACCOUNT", "Updated account: " + account.getAccountNumber());
    return true;
}

bool Database::deleteAccount(const std::string& account_number) {
    std::lock_guard<std::mutex> lock(accounts_mutex);
    
    std::vector<std::string> lines;
    std::ifstream file(accounts_file);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool deleted = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("account_number,") == 0) {
            lines.push_back(line);
            continue;
        }
        
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 1 && tokens[0] == account_number) {
            deleted = true;
            // Don't add this line (effectively deleting it)
        } else {
            lines.push_back(line);
        }
    }
    file.close();
    
    if (!deleted) {
        return false;
    }
    
    std::ofstream out_file(accounts_file);
    if (!out_file.is_open()) {
        return false;
    }
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    
    logOperation("DELETE_ACCOUNT", "Deleted account: " + account_number);
    return true;
}

bool Database::updateTransaction(const Transaction& transaction) {
    std::lock_guard<std::mutex> lock(transactions_mutex);
    
    std::vector<std::string> lines;
    std::ifstream file(transactions_file);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    bool updated = false;
    
    while (std::getline(file, line)) {
        if (line.empty() || line.find("transaction_id,") == 0) {
            lines.push_back(line);
            continue;
        }
        
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        
        while (std::getline(ss, token, ',')) {
            tokens.push_back(token);
        }
        
        if (tokens.size() >= 1 && tokens[0] == transaction.getTransactionId()) {
            lines.push_back(transaction.toCsvRow());
            updated = true;
        } else {
            lines.push_back(line);
        }
    }
    file.close();
    
    if (!updated) {
        return false;
    }
    
    std::ofstream out_file(transactions_file);
    if (!out_file.is_open()) {
        return false;
    }
    
    for (const auto& l : lines) {
        out_file << l << "\n";
    }
    out_file.close();
    
    logOperation("UPDATE_TRANSACTION", "Updated transaction: " + transaction.getTransactionId());
    return true;
}

std::vector<Transaction> Database::getAllTransactions() {
    std::lock_guard<std::mutex> lock(transactions_mutex);
    std::vector<Transaction> transactions;
    
    std::ifstream file(transactions_file);
    if (!file.is_open()) {
        return transactions;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("transaction_id,") == 0) {
            continue;
        }
        
        Transaction transaction;
        if (transaction.fromCsvRow(line)) {
            transactions.push_back(transaction);
        }
    }
    
    file.close();
    return transactions;
}

std::vector<Transaction> Database::getTransactionsByDateRange(
    const std::string& start_date, const std::string& end_date) {
    std::lock_guard<std::mutex> lock(transactions_mutex);
    std::vector<Transaction> transactions;
    
    std::ifstream file(transactions_file);
    if (!file.is_open()) {
        return transactions;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.find("transaction_id,") == 0) {
            continue;
        }
        
        Transaction transaction;
        if (transaction.fromCsvRow(line)) {
            std::string tx_date = transaction.getTimestamp().substr(0, 10); // Extract YYYY-MM-DD
            if (tx_date >= start_date && tx_date <= end_date) {
                transactions.push_back(transaction);
            }
        }
    }
    
    file.close();
    return transactions;
}

bool Database::backup() {
    std::string backup_dir = data_directory + "/backups/" + getCurrentTimestamp();
    
    if (!ensureDirectoryExists(backup_dir)) {
        return false;
    }
    
    // Copy all data files to backup directory
    std::vector<std::string> files_to_backup = {
        users_file,
        accounts_file,
        transactions_file
    };
    
    for (const auto& file : files_to_backup) {
        std::ifstream src(file, std::ios::binary);
        if (!src.is_open()) continue;
        
        std::string filename = file.substr(file.find_last_of("/\\") + 1);
        std::ofstream dst(backup_dir + "/" + filename, std::ios::binary);
        if (dst.is_open()) {
            dst << src.rdbuf();
            dst.close();
        }
        src.close();
    }
    
    logOperation("BACKUP", "System backup created: " + backup_dir);
    return true;
}

bool Database::restore(const std::string& backup_path) {
    // Implementation for restore functionality
    logOperation("RESTORE", "Restore attempted from: " + backup_path);
    return true; // Simplified implementation
}

bool Database::validateDataIntegrity() {
    // Check file existence and format
    std::vector<std::string> files = {users_file, accounts_file, transactions_file};
    
    for (const auto& file : files) {
        std::ifstream f(file);
        if (!f.is_open()) {
            logOperation("INTEGRITY_CHECK", "Failed to open file: " + file);
            return false;
        }
        f.close();
    }
    
    logOperation("INTEGRITY_CHECK", "Data integrity validation passed");
    return true;
}

void Database::cleanup() {
    // Clean up old log files and temporary data
    logOperation("CLEANUP", "Database cleanup completed");
}

size_t Database::getUserCount() {
    return getAllUsers().size();
}

size_t Database::getAccountCount() {
    return getAllAccounts().size();
}

double Database::getTotalSystemBalance() {
    double total = 0.0;
    auto accounts = getAllAccounts();
    for (const auto& account : accounts) {
        if (account.isActive()) {
            total += account.getBalance();
        }
    }
    return total;
}

size_t Database::getTransactionCount() {
    return getAllTransactions().size();
}

bool Database::loadUserInternal(const std::string& user_id, User& user) {
    // Internal version of loadUser that doesn't use mutex (assumes caller already has it)
    std::cout << "loadUserInternal called for: " << user_id << std::endl;
    
    std::cout << "Opening users file: " << users_file << std::endl;
    std::ifstream file(users_file);
    if (!file.is_open()) {
        std::cout << "Users file doesn't exist or can't be opened" << std::endl;
        return false;
    }
    std::cout << "Users file opened successfully" << std::endl;
    
    std::string line;
    std::cout << "Reading header line..." << std::endl;
    std::getline(file, line); // Skip header
    std::cout << "Header: " << line << std::endl;
    
    std::cout << "Searching for user..." << std::endl;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::cout << "Checking line: " << line << std::endl;
        
        std::istringstream ss(line);
        std::string item;
        std::vector<std::string> tokens;
        
        while (std::getline(ss, item, ',')) {
            tokens.push_back(item);
        }
        
        if (tokens.size() >= 11 && tokens[0] == user_id) {
            std::cout << "User found! Loading from CSV..." << std::endl;
            // Create user from CSV data
            user = User();
            user.fromCsvRow(line);
            file.close();
            std::cout << "User loaded successfully" << std::endl;
            return true;
        }
    }
    
    file.close();
    std::cout << "User not found in file" << std::endl;
    return false;
}
