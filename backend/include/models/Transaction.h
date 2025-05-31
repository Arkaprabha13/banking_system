#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>
#include <chrono>

enum class TransactionType {
    DEPOSIT,
    WITHDRAWAL,
    TRANSFER,
    PAYMENT,
    FEE
};

enum class TransactionStatus {
    PENDING,
    COMPLETED,
    FAILED,
    CANCELLED
};

class Transaction {
private:
    std::string transaction_id;
    std::string from_account_id;
    std::string to_account_id;
    double amount;
    TransactionType type;
    TransactionStatus status;
    std::string description;
    double balance_before;
    double balance_after;
    std::chrono::system_clock::time_point timestamp;
    std::string reference_number;

public:
    // Constructors
    Transaction();
    Transaction(const std::string& from_account, const std::string& to_account,
                double amount, TransactionType type, const std::string& description = "");

    // Getters
    std::string getTransactionId() const;
    std::string getFromAccountId() const;
    std::string getToAccountId() const;
    double getAmount() const;
    TransactionType getType() const;
    TransactionStatus getStatus() const;
    std::string getDescription() const;
    double getBalanceBefore() const;
    double getBalanceAfter() const;
    std::string getTimestamp() const;
    std::string getReferenceNumber() const;

    // Setters
    void setStatus(TransactionStatus status);
    void setDescription(const std::string& description);
    void setBalanceBefore(double balance);
    void setBalanceAfter(double balance);
    void setReferenceNumber(const std::string& ref_number);

    // Validation
    bool isValid() const;

    // Serialization
    std::string toJson() const;
    bool fromJson(const std::string& json);
    std::string toCsvRow() const;
    bool fromCsvRow(const std::string& csv_row);

    // Utility
    std::string typeToString() const;
    std::string statusToString() const;
    static TransactionType stringToType(const std::string& type_str);
    static TransactionStatus stringToStatus(const std::string& status_str);
    static std::string generateTransactionId();
    static std::string generateReferenceNumber();
};

#endif // TRANSACTION_H
