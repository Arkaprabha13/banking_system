#include "../include/models/User.h"
#include <sstream>
#include <random>
#include <algorithm>
#include <iomanip>

User::User() 
    : role(UserRole::CUSTOMER), is_active(true), failed_login_attempts(0) {
    user_id = generateUserId();
    created_date = std::chrono::system_clock::now();
    last_login = std::chrono::system_clock::time_point{}; // Initialize to epoch
}

User::User(const std::string& username, const std::string& password_hash,
           const std::string& email, const std::string& full_name,
           const std::string& phone, UserRole role)
    : username(username), password_hash(password_hash), email(email),
      full_name(full_name), phone_number(phone), role(role),
      is_active(true), failed_login_attempts(0) {
    user_id = generateUserId();
    created_date = std::chrono::system_clock::now();
    last_login = std::chrono::system_clock::time_point{}; // Initialize to epoch
}

std::string User::generateUserId() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    return "USR" + std::to_string(dis(gen));
}

// Getters
std::string User::getUserId() const { return user_id; }
std::string User::getUsername() const { return username; }
std::string User::getPasswordHash() const { return password_hash; }
std::string User::getEmail() const { return email; }
std::string User::getFullName() const { return full_name; }
std::string User::getPhoneNumber() const { return phone_number; }
UserRole User::getRole() const { return role; }
bool User::getIsActive() const { return is_active; }
int User::getFailedLoginAttempts() const { return failed_login_attempts; }
std::vector<std::string> User::getAccountIds() const { return account_ids; }

std::string User::getLastLogin() const {
    if (last_login == std::chrono::system_clock::time_point{}) {
        return "Never";
    }
    auto time_t = std::chrono::system_clock::to_time_t(last_login);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string User::getCreatedDate() const {
    auto time_t = std::chrono::system_clock::to_time_t(created_date);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Setters
void User::setPasswordHash(const std::string& password_hash) {
    this->password_hash = password_hash;
}

void User::setEmail(const std::string& email) {
    this->email = email;
}

void User::setFullName(const std::string& full_name) {
    this->full_name = full_name;
}

void User::setPhoneNumber(const std::string& phone) {
    this->phone_number = phone;
}

void User::setRole(UserRole role) {
    this->role = role;
}

void User::setIsActive(bool active) {
    this->is_active = active;
}

void User::incrementFailedLoginAttempts() {
    failed_login_attempts++;
    if (failed_login_attempts >= 5) {
        is_active = false; // Lock account after 5 failed attempts
    }
}

void User::resetFailedLoginAttempts() {
    failed_login_attempts = 0;
}

void User::updateLastLogin() {
    last_login = std::chrono::system_clock::now();
    resetFailedLoginAttempts();
}

void User::addAccountId(const std::string& account_id) {
    if (std::find(account_ids.begin(), account_ids.end(), account_id) == account_ids.end()) {
        account_ids.push_back(account_id);
    }
}

void User::removeAccountId(const std::string& account_id) {
    account_ids.erase(
        std::remove(account_ids.begin(), account_ids.end(), account_id),
        account_ids.end()
    );
}

// Validation
bool User::isValid() const {
    return !username.empty() && !password_hash.empty() && 
           !email.empty() && !full_name.empty();
}

bool User::canLogin() const {
    return is_active && failed_login_attempts < 5;
}

// Utility
std::string User::roleToString() const {
    switch (role) {
        case UserRole::CUSTOMER: return "CUSTOMER";
        case UserRole::ADMIN: return "ADMIN";
        case UserRole::MANAGER: return "MANAGER";
        default: return "CUSTOMER";
    }
}

UserRole User::stringToRole(const std::string& role_str) {
    if (role_str == "ADMIN") return UserRole::ADMIN;
    if (role_str == "MANAGER") return UserRole::MANAGER;
    return UserRole::CUSTOMER;
}

// Serialization
std::string User::toJson() const {
    std::ostringstream json;
    json << "{"
         << "\"user_id\":\"" << user_id << "\","
         << "\"username\":\"" << username << "\","
         << "\"password_hash\":\"" << password_hash << "\","
         << "\"email\":\"" << email << "\","
         << "\"full_name\":\"" << full_name << "\","
         << "\"phone_number\":\"" << phone_number << "\","
         << "\"role\":\"" << roleToString() << "\","
         << "\"is_active\":" << (is_active ? "true" : "false") << ","
         << "\"failed_login_attempts\":" << failed_login_attempts << ","
         << "\"last_login\":\"" << getLastLogin() << "\","
         << "\"created_date\":\"" << getCreatedDate() << "\","
         << "\"account_ids\":[";
    
    for (size_t i = 0; i < account_ids.size(); ++i) {
        json << "\"" << account_ids[i] << "\"";
        if (i < account_ids.size() - 1) json << ",";
    }
    json << "]}";
    
    return json.str();
}

std::string User::toCsvRow() const {
    std::ostringstream csv;
    csv << user_id << ","
        << username << ","
        << password_hash << ","
        << email << ","
        << full_name << ","
        << phone_number << ","
        << roleToString() << ","
        << (is_active ? "1" : "0") << ","
        << failed_login_attempts << ","
        << getLastLogin() << ","
        << getCreatedDate();
    
    return csv.str();
}

bool User::fromCsvRow(const std::string& csv_row) {
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
        user_id = tokens[0];
        username = tokens[1];
        password_hash = tokens[2];
        email = tokens[3];
        full_name = tokens[4];
        phone_number = tokens[5];
        role = stringToRole(tokens[6]);
        is_active = (tokens[7] == "1");
        failed_login_attempts = std::stoi(tokens[8]);
        
        // Parse timestamps - simplified implementation
        // In a real system, you'd parse the actual timestamp format
        created_date = std::chrono::system_clock::now();
        last_login = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool User::fromJson(const std::string& json_str) {
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
        
        auto extractBool = [&](const std::string& key) -> bool {
            std::string search = "\"" + key + "\":";
            size_t pos = json_str.find(search);
            if (pos != std::string::npos) {
                size_t start = pos + search.length();
                std::string value_str = json_str.substr(start, 5); // "true" or "false"
                return value_str.find("true") != std::string::npos;
            }
            return false;
        };
        
        auto extractInt = [&](const std::string& key) -> int {
            std::string search = "\"" + key + "\":";
            size_t pos = json_str.find(search);
            if (pos != std::string::npos) {
                size_t start = pos + search.length();
                size_t end = json_str.find_first_of(",}", start);
                if (end != std::string::npos) {
                    std::string num_str = json_str.substr(start, end - start);
                    return std::stoi(num_str);
                }
            }
            return 0;
        };
        
        user_id = extractValue("user_id");
        username = extractValue("username");
        password_hash = extractValue("password_hash");
        email = extractValue("email");
        full_name = extractValue("full_name");
        phone_number = extractValue("phone_number");
        role = stringToRole(extractValue("role"));
        is_active = extractBool("is_active");
        failed_login_attempts = extractInt("failed_login_attempts");
        
        // Set timestamps to current time for simplicity
        created_date = std::chrono::system_clock::now();
        last_login = std::chrono::system_clock::now();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}
