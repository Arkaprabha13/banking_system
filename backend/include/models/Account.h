#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <string>
#include <vector>
#include <chrono>

enum class AccountType {
    SAVINGS,
    CHECKING,
    BUSINESS
};

enum class AccountStatus {
    ACTIVE,
    CLOSED,
    SUSPENDED,
    PENDING
};

class Account {
private:
    std::string account_number;
    std::string customer_id;
    AccountType account_type;
    double balance;
    AccountStatus status;
    double daily_limit;
    double minimum_balance;
    std::chrono::system_clock::time_point created_date;
    std::chrono::system_clock::time_point last_updated;

public:
    // Constructors
    Account();
    Account(const std::string& acc_num, const std::string& cust_id, 
            AccountType type, double initial_balance = 0.0);

    // Getters
    std::string getAccountNumber() const;
    std::string getCustomerId() const;
    AccountType getAccountType() const;
    double getBalance() const;
    AccountStatus getStatus() const;
    double getDailyLimit() const;
    double getMinimumBalance() const;
    std::string getCreatedDate() const;
    std::string getLastUpdated() const;

    // Setters
    void setBalance(double new_balance);
    void setStatus(AccountStatus new_status);
    void setDailyLimit(double limit);
    void setMinimumBalance(double min_balance);
    void updateLastModified();

    // Account operations
    bool deposit(double amount);
    bool withdraw(double amount);
    bool canWithdraw(double amount) const;
    bool transfer(double amount, Account& target_account);

    // Validation
    bool isValid() const;
    bool isActive() const;
    bool hasMinimumBalance() const;

    // Serialization
    std::string toJson() const;
    bool fromJson(const std::string& json);
    std::string toCsvRow() const;
    bool fromCsvRow(const std::string& csv_row);

    // Utility
    std::string typeToString() const;
    std::string statusToString() const;
    static AccountType stringToType(const std::string& type_str);
    static AccountStatus stringToStatus(const std::string& status_str);
};

#endif // ACCOUNT_H
