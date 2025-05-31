#include "../include/models/Transaction.h"
#include <sstream>
#include <random>
#include <iomanip>
#include <ctime>

Transaction::Transaction() 
    : amount(0.0), type(TransactionType::DEPOSIT), status(TransactionStatus::PENDING),
      balance_before(0.0), balance_after(0.0) {
    transaction_id = generateTransactionId();
    reference_number = generateReferenceNumber();
    timestamp = std::chrono::system_clock::now();
}

Transaction::Transaction(const std::string& from_account, const std::string& to_account,
                        double amount, TransactionType type, const std::string& description)
    : from_account_id(from_account), to_account_id(to_account), amount(amount),
      type(type), status(TransactionStatus::PENDING), description(description),
      balance_before(0.0), balance_after(0.0) {
    transaction_id = generateTransactionId();
    reference_number = generateReferenceNumber();
    timestamp = std::chrono::system_clock::now();
}

// Getters
std::string Transaction::getTransactionId() const { return transaction_id; }
std::string Transaction::getFromAccountId() const { return from_account_id; }
std::string Transaction::getToAccountId() const { return to_account_id; }
double Transaction::getAmount() const { return amount; }
TransactionType Transaction::getType() const { return type; }
TransactionStatus Transaction::getStatus() const { return status; }
std::string Transaction::getDescription() const { return description; }
double Transaction::getBalanceBefore() const { return balance_before; }
double Transaction::getBalanceAfter() const { return balance_after; }
std::string Transaction::getReferenceNumber() const { return reference_number; }

std::string Transaction::getTimestamp() const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Setters
void Transaction::setStatus(TransactionStatus status) {
    this->status = status;
}

void Transaction::setDescription(const std::string& description) {
    this->description = description;
}

void Transaction::setBalanceBefore(double balance) {
    this->balance_before = balance;
}

void Transaction::setBalanceAfter(double balance) {
    this->balance_after = balance;
}

void Transaction::setReferenceNumber(const std::string& ref_number) {
    this->reference_number = ref_number;
}

// Validation
bool Transaction::isValid() const {
    return !transaction_id.empty() && amount > 0 && 
           (!from_account_id.empty() || !to_account_id.empty());
}

// Utility
std::string Transaction::typeToString() const {
    switch (type) {
        case TransactionType::DEPOSIT: return "DEPOSIT";
        case TransactionType::WITHDRAWAL: return "WITHDRAWAL";
        case TransactionType::TRANSFER: return "TRANSFER";
        case TransactionType::PAYMENT: return "PAYMENT";
        case TransactionType::FEE: return "FEE";
        default: return "DEPOSIT";
    }
}

std::string Transaction::statusToString() const {
    switch (status) {
        case TransactionStatus::PENDING: return "PENDING";
        case TransactionStatus::COMPLETED: return "COMPLETED";
        case TransactionStatus::FAILED: return "FAILED";
        case TransactionStatus::CANCELLED: return "CANCELLED";
        default: return "PENDING";
    }
}

TransactionType Transaction::stringToType(const std::string& type_str) {
    if (type_str == "WITHDRAWAL") return TransactionType::WITHDRAWAL;
    if (type_str == "TRANSFER") return TransactionType::TRANSFER;
    if (type_str == "PAYMENT") return TransactionType::PAYMENT;
    if (type_str == "FEE") return TransactionType::FEE;
    return TransactionType::DEPOSIT;
}

TransactionStatus Transaction::stringToStatus(const std::string& status_str) {
    if (status_str == "COMPLETED") return TransactionStatus::COMPLETED;
    if (status_str == "FAILED") return TransactionStatus::FAILED;
    if (status_str == "CANCELLED") return TransactionStatus::CANCELLED;
    return TransactionStatus::PENDING;
}

std::string Transaction::generateTransactionId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000000, 999999999);
    return "TXN" + std::to_string(dis(gen));
}

std::string Transaction::generateReferenceNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<long long> dis(1000000000LL, 9999999999LL);
    return "REF" + std::to_string(dis(gen));
}

// Serialization
std::string Transaction::toJson() const {
    std::ostringstream json;
    json << "{"
         << "\"transaction_id\":\"" << transaction_id << "\","
         << "\"from_account_id\":\"" << from_account_id << "\","
         << "\"to_account_id\":\"" << to_account_id << "\","
         << "\"amount\":" << std::fixed << std::setprecision(2) << amount << ","
         << "\"type\":\"" << typeToString() << "\","
         << "\"status\":\"" << statusToString() << "\","
         << "\"description\":\"" << description << "\","
         << "\"balance_before\":" << std::fixed << std::setprecision(2) << balance_before << ","
         << "\"balance_after\":" << std::fixed << std::setprecision(2) << balance_after << ","
         << "\"timestamp\":\"" << getTimestamp() << "\","
         << "\"reference_number\":\"" << reference_number << "\""
         << "}";
    
    return json.str();
}

std::string Transaction::toCsvRow() const {
    std::ostringstream csv;
    csv << transaction_id << ","
        << from_account_id << ","
        << to_account_id << ","
        << std::fixed << std::setprecision(2) << amount << ","
        << typeToString() << ","
        << statusToString() << ","
        << description << ","
        << std::fixed << std::setprecision(2) << balance_before << ","
        << std::fixed << std::setprecision(2) << balance_after << ","
        << getTimestamp() << ","
        << reference_number;
    
    return csv.str();
}

bool Transaction::fromCsvRow(const std::string& csv_row) {
    std::vector<std::string> tokens;
    std::stringstream ss(csv_row);
    std::string token;
    
    while (std::getline(ss, token, ',')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 11) {
        return false;
    }
    
    try {
        transaction_id = tokens[0];
        from_account_id = tokens[1];
        to_account_id = tokens[2];
        amount = std::stod(tokens[3]);
        type = stringToType(tokens[4]);
        status = stringToStatus(tokens[5]);
        description = tokens[6];
        balance_before = std::stod(tokens[7]);
        balance_after = std::stod(tokens[8]);
        reference_number = tokens[10];
        
        // Parse timestamp - simplified implementation
        timestamp = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Transaction::fromJson(const std::string& json_str) {
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
        
        transaction_id = extractValue("transaction_id");
        from_account_id = extractValue("from_account_id");
        to_account_id = extractValue("to_account_id");
        amount = extractDouble("amount");
        type = stringToType(extractValue("type"));
        status = stringToStatus(extractValue("status"));
        description = extractValue("description");
        balance_before = extractDouble("balance_before");
        balance_after = extractDouble("balance_after");
        reference_number = extractValue("reference_number");
        
        // Set timestamp to current time for simplicity
        timestamp = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
