#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <functional>
#include "../models/User.h"
#include "../models/Transaction.h"

// Forward declaration
class Account;

class Database {
private:
    std::string data_directory;
    std::mutex users_mutex;
    std::mutex accounts_mutex;
    std::mutex transactions_mutex;
    
    // File paths
    std::string users_file;
    std::string accounts_file;
    std::string transactions_file;
    std::string logs_file;
    
    // Internal helper methods
    bool ensureDirectoryExists(const std::string& path);
    bool backupFile(const std::string& file_path);
    std::string getCurrentTimestamp();
    void logOperation(const std::string& operation, const std::string& details);
    bool loadUserInternal(const std::string& user_id, User& user); // Private version without mutex
    
    // ADD THIS LINE - Missing hashPassword declaration
    std::string hashPassword(const std::string& password);

public:
    // Constructor
    Database(const std::string& data_dir = "data");
    
    // Initialization
    bool initialize();
    bool createSampleData();

    // User operations
    bool saveUser(const User& user);
    bool loadUser(const std::string& user_id, User& user);
    bool loadUserByUsername(const std::string& username, User& user);
    bool updateUser(const User& user);
    bool deleteUser(const std::string& user_id);
    std::vector<User> getAllUsers();
    bool userExists(const std::string& username);

    // Account operations
    bool saveAccount(const Account& account);
    bool loadAccount(const std::string& account_number, Account& account);
    bool updateAccount(const Account& account);
    bool deleteAccount(const std::string& account_number);
    std::vector<Account> getAllAccounts();
    std::vector<Account> getAccountsByCustomerId(const std::string& customer_id);
    bool accountExists(const std::string& account_number);
    std::string generateAccountNumber();

    // Transaction operations
    bool saveTransaction(const Transaction& transaction);
    bool loadTransaction(const std::string& transaction_id, Transaction& transaction);
    bool updateTransaction(const Transaction& transaction);
    std::vector<Transaction> getAllTransactions();
    std::vector<Transaction> getTransactionsByAccount(const std::string& account_id);
    std::vector<Transaction> getTransactionsByDateRange(
        const std::string& start_date, const std::string& end_date);

    // Utility operations
    bool backup();
    bool restore(const std::string& backup_path);
    bool validateDataIntegrity();
    void cleanup();
    
    // Statistics
    size_t getUserCount();
    size_t getAccountCount();
    size_t getTransactionCount();
    double getTotalSystemBalance();
};

#endif // DATABASE_H
