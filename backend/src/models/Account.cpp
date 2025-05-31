#include "../include/models/Account.h"
#include <sstream>
#include <random>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <ctime>

Account::Account() 
    : account_type(AccountType::CHECKING), balance(0.0), 
      status(AccountStatus::PENDING), daily_limit(1000.0), minimum_balance(0.0) {
    created_date = std::chrono::system_clock::now();
    last_updated = created_date;
}

Account::Account(const std::string& acc_num, const std::string& cust_id, 
                AccountType type, double initial_balance)
    : account_number(acc_num), customer_id(cust_id), account_type(type),
      balance(initial_balance), status(AccountStatus::ACTIVE), 
      daily_limit(1000.0), minimum_balance(0.0) {
    
    // Set account-specific limits
    switch (type) {
        case AccountType::SAVINGS:
            daily_limit = 500.0;
            minimum_balance = 100.0;
            break;
        case AccountType::CHECKING:
            daily_limit = 1000.0;
            minimum_balance = 25.0;
            break;
        case AccountType::BUSINESS:
            daily_limit = 5000.0;
            minimum_balance = 500.0;
            break;
    }
    
    created_date = std::chrono::system_clock::now();
    last_updated = created_date;
}

// Getters
std::string Account::getAccountNumber() const { return account_number; }
std::string Account::getCustomerId() const { return customer_id; }
AccountType Account::getAccountType() const { return account_type; }
double Account::getBalance() const { return balance; }
AccountStatus Account::getStatus() const { return status; }
double Account::getDailyLimit() const { return daily_limit; }
double Account::getMinimumBalance() const { return minimum_balance; }

std::string Account::getCreatedDate() const {
    auto time_t = std::chrono::system_clock::to_time_t(created_date);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string Account::getLastUpdated() const {
    auto time_t = std::chrono::system_clock::to_time_t(last_updated);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Setters
void Account::setBalance(double new_balance) {
    balance = new_balance;
    updateLastModified();
}

void Account::setStatus(AccountStatus new_status) {
    status = new_status;
    updateLastModified();
}

void Account::setDailyLimit(double limit) {
    daily_limit = limit;
    updateLastModified();
}

void Account::setMinimumBalance(double min_balance) {
    minimum_balance = min_balance;
    updateLastModified();
}

void Account::updateLastModified() {
    last_updated = std::chrono::system_clock::now();
}

// Account operations
bool Account::deposit(double amount) {
    if (amount <= 0 || !isActive()) {
        return false;
    }
    
    balance += amount;
    updateLastModified();
    return true;
}

bool Account::withdraw(double amount) {
    if (!canWithdraw(amount)) {
        return false;
    }
    
    balance -= amount;
    updateLastModified();
    return true;
}

bool Account::canWithdraw(double amount) const {
    return amount > 0 && 
           isActive() && 
           amount <= daily_limit &&
           (balance - amount) >= minimum_balance;
}

bool Account::transfer(double amount, Account& target_account) {
    if (!canWithdraw(amount) || !target_account.isActive()) {
        return false;
    }
    
    if (withdraw(amount)) {
        if (target_account.deposit(amount)) {
            return true;
        } else {
            // Rollback if target deposit fails
            deposit(amount);
            return false;
        }
    }
    return false;
}

// Validation
bool Account::isValid() const {
    return !account_number.empty() && !customer_id.empty() && balance >= 0;
}

bool Account::isActive() const {
    return status == AccountStatus::ACTIVE;
}

bool Account::hasMinimumBalance() const {
    return balance >= minimum_balance;
}

// Utility
std::string Account::typeToString() const {
    switch (account_type) {
        case AccountType::SAVINGS: return "SAVINGS";
        case AccountType::CHECKING: return "CHECKING";
        case AccountType::BUSINESS: return "BUSINESS";
        default: return "CHECKING";
    }
}

std::string Account::statusToString() const {
    switch (status) {
        case AccountStatus::ACTIVE: return "ACTIVE";
        case AccountStatus::CLOSED: return "CLOSED";
        case AccountStatus::SUSPENDED: return "SUSPENDED";
        case AccountStatus::PENDING: return "PENDING";
        default: return "PENDING";
    }
}

AccountType Account::stringToType(const std::string& type_str) {
    if (type_str == "SAVINGS") return AccountType::SAVINGS;
    if (type_str == "BUSINESS") return AccountType::BUSINESS;
    return AccountType::CHECKING;
}

AccountStatus Account::stringToStatus(const std::string& status_str) {
    if (status_str == "ACTIVE") return AccountStatus::ACTIVE;
    if (status_str == "CLOSED") return AccountStatus::CLOSED;
    if (status_str == "SUSPENDED") return AccountStatus::SUSPENDED;
    return AccountStatus::PENDING;
}

// Serialization
std::string Account::toJson() const {
    std::ostringstream json;
    json << "{"
         << "\"account_number\":\"" << account_number << "\","
         << "\"customer_id\":\"" << customer_id << "\","
         << "\"account_type\":\"" << typeToString() << "\","
         << "\"balance\":" << std::fixed << std::setprecision(2) << balance << ","
         << "\"status\":\"" << statusToString() << "\","
         << "\"daily_limit\":" << std::fixed << std::setprecision(2) << daily_limit << ","
         << "\"minimum_balance\":" << std::fixed << std::setprecision(2) << minimum_balance << ","
         << "\"created_date\":\"" << getCreatedDate() << "\","
         << "\"last_updated\":\"" << getLastUpdated() << "\""
         << "}";
    
    return json.str();
}

// std::string Account::toCsvRow() const {
//     std::ostringstream csv;
//     csv << account_number << ","
//         << customer_id << ","
//         << typeToString() << ","
//         << std::fixed << std::setprecision(2) << balance << ","
//         << statusToString() << ","
//         << std::fixed << std::setprecision(2) << daily_limit << ","
//         << std::fixed << std::setprecision(2) << minimum_balance << ","
//         << getCreatedDate() << ","
//         << getLastUpdated();
    
//     return csv.str();
// }
std::string Account::toCsvRow() const {
    std::cout << "[DEBUG] toCsvRow called" << std::endl;
    
    std::ostringstream oss;
    oss << account_number << ","
        << customer_id << ","
        << typeToString() << ","
        << std::fixed << std::setprecision(2) << balance << ","
        << statusToString() << ","
        << std::fixed << std::setprecision(2) << daily_limit << ","
        << std::fixed << std::setprecision(2) << minimum_balance << ","
        << getCreatedDate() << ","
        << getLastUpdated();
    
    std::string result = oss.str();
    std::cout << "[DEBUG] toCsvRow result: " << result << std::endl;
    return result;
}


bool Account::fromCsvRow(const std::string& csv_row) {
    std::vector<std::string> tokens;
    std::stringstream ss(csv_row);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 9) {
        return false;
    }
    
    try {
        account_number = tokens[0];
        customer_id = tokens[1];
        account_type = stringToType(tokens[2]);
        balance = std::stod(tokens[3]);
        status = stringToStatus(tokens[4]);
        daily_limit = std::stod(tokens[5]);
        minimum_balance = std::stod(tokens[6]);
        
        // Parse timestamps - simplified implementation
        created_date = std::chrono::system_clock::now();
        last_updated = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Account::fromJson(const std::string& json_str) {
    // Simplified JSON parsing - in production use a proper JSON library
    try {
        auto extractValue = [&](const std::string& key) -> std::string {
            std::string search = "\"" + key + "\":\"";
            size_t pos = json_str.find(search);
            if (pos != std::string::npos) {
                size_t start = pos + search.length();
                size_t end = json_str.find("\"", start);
                if (end != std::string::npos) {
                    return json_str.substr(start, end - start);
                }
            }
            return "";
        };
        
        auto extractDouble = [&](const std::string& key) -> double {
            std::string search = "\"" + key + "\":";
            size_t pos = json_str.find(search);
            if (pos != std::string::npos) {
                size_t start = pos + search.length();
                size_t end = json_str.find_first_of(",}", start);
                if (end != std::string::npos) {
                    std::string num_str = json_str.substr(start, end - start);
                    return std::stod(num_str);
                }
            }
            return 0.0;
        };
        
        account_number = extractValue("account_number");
        customer_id = extractValue("customer_id");
        account_type = stringToType(extractValue("account_type"));
        balance = extractDouble("balance");
        status = stringToStatus(extractValue("status"));
        daily_limit = extractDouble("daily_limit");
        minimum_balance = extractDouble("minimum_balance");
        
        // Set timestamps to current time for simplicity
        created_date = std::chrono::system_clock::now();
        last_updated = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
