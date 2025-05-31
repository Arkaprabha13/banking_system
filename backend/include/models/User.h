#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <chrono>

enum class UserRole {
    CUSTOMER,
    ADMIN,
    MANAGER
};

class User {
private:
    std::string user_id;
    std::string username;
    std::string password_hash;
    std::string email;
    std::string full_name;
    std::string phone_number;
    UserRole role;
    bool is_active;
    int failed_login_attempts;
    std::chrono::system_clock::time_point last_login;
    std::chrono::system_clock::time_point created_date;
    std::vector<std::string> account_ids;

public:
    // Constructors
    User();
    User(const std::string& username, const std::string& password_hash,
         const std::string& email, const std::string& full_name,
         const std::string& phone = "", UserRole role = UserRole::CUSTOMER);

    // Getters
    std::string getUserId() const;
    std::string getUsername() const;
    std::string getPasswordHash() const;
    std::string getEmail() const;
    std::string getFullName() const;
    std::string getPhoneNumber() const;
    UserRole getRole() const;
    bool getIsActive() const;
    int getFailedLoginAttempts() const;
    std::string getLastLogin() const;
    std::string getCreatedDate() const;
    std::vector<std::string> getAccountIds() const;

    // Setters
    void setPasswordHash(const std::string& password_hash);
    void setEmail(const std::string& email);
    void setFullName(const std::string& full_name);
    void setPhoneNumber(const std::string& phone);
    void setRole(UserRole role);
    void setIsActive(bool active);
    void incrementFailedLoginAttempts();
    void resetFailedLoginAttempts();
    void updateLastLogin();
    void addAccountId(const std::string& account_id);
    void removeAccountId(const std::string& account_id);

    // Validation
    bool isValid() const;
    bool canLogin() const;

    // Serialization
    std::string toJson() const;
    bool fromJson(const std::string& json);
    std::string toCsvRow() const;
    bool fromCsvRow(const std::string& csv_row);

    // Utility
    std::string roleToString() const;
    static UserRole stringToRole(const std::string& role_str);
    static std::string generateUserId();
};

#endif // USER_H
