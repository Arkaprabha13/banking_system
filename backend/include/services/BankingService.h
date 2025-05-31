#ifndef BANKING_SERVICE_H
#define BANKING_SERVICE_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "../core/Database.h"
#include "../models/User.h"
#include "../models/Transaction.h"
#include "../models/Account.h"

struct AuthResult {
    bool success;
    std::string user_id;
    std::string message;
    UserRole role;
};

struct TransactionResult {
    bool success;
    std::string transaction_id;
    std::string message;
    double new_balance;
};

struct AccountCreationResult {
    bool success;
    std::string account_number;
    std::string message;
};

class BankingService {
private:
    std::unique_ptr<Database> database;
    std::mutex service_mutex;

    // Helper methods
    bool validateAmount(double amount);
    bool validateAccountNumber(const std::string& account_number);
    bool validateUserId(const std::string& user_id);
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& hash);
    void logActivity(const std::string& user_id, const std::string& activity);

public:
    // Constructor
    BankingService(const std::string& data_directory = "data");
    
    // Initialization
    bool initialize();
    
    // Authentication and User Management
    AuthResult authenticateUser(const std::string& username, const std::string& password);
    AuthResult registerUser(const std::string& username, const std::string& password,
                           const std::string& email, const std::string& full_name,
                           const std::string& phone = "");
    bool changePassword(const std::string& user_id, const std::string& old_password,
                       const std::string& new_password);
    bool updateUserProfile(const std::string& user_id, const std::string& email,
                          const std::string& full_name, const std::string& phone);
    std::vector<User> getAllUsers(); // Admin only
    bool activateUser(const std::string& user_id); // Admin only
    bool deactivateUser(const std::string& user_id); // Admin only
    
    // User lookup helper
    bool getUserByUsername(const std::string& username, User& user);

    // Account Management
    AccountCreationResult createAccount(const std::string& user_id, const std::string& account_type,
                                       double initial_deposit = 0.0);
    bool closeAccount(const std::string& account_number, const std::string& user_id);
    bool suspendAccount(const std::string& account_number); // Admin only
    bool reactivateAccount(const std::string& account_number); // Admin only
    std::vector<Account> getUserAccounts(const std::string& user_id);
    std::vector<Account> getAllAccounts(); // Admin only
    bool getAccountBalance(const std::string& account_number, double& balance);
    bool getAccountInfo(const std::string& account_number, Account& account);

    // Transaction Operations
    TransactionResult deposit(const std::string& account_number, double amount,
                             const std::string& description = "Deposit");
    TransactionResult withdraw(const std::string& account_number, double amount,
                              const std::string& description = "Withdrawal");
    TransactionResult transfer(const std::string& from_account, const std::string& to_account,
                              double amount, const std::string& description = "Transfer");
    
    // Transaction History
    std::vector<Transaction> getAccountTransactions(const std::string& account_number);
    std::vector<Transaction> getUserTransactions(const std::string& user_id);
    std::vector<Transaction> getAllTransactions(); // Admin only
    std::vector<Transaction> getTransactionsByDateRange(const std::string& start_date,
                                                       const std::string& end_date);

    // Reports and Analytics
    double getTotalSystemBalance(); // Admin only
    size_t getTotalUsers(); // Admin only
    size_t getTotalAccounts(); // Admin only
    size_t getTotalTransactions(); // Admin only
    
    // Utility
    bool backupData(); // Admin only
    bool validateSystem(); // Admin only
    std::string getSystemStatus(); // Admin only
};

#endif // BANKING_SERVICE_H
