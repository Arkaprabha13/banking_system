#!/bin/bash

# Banking System Build and Setup Script
# This script automates the entire build process for the banking system

set -e  # Exit on any error

echo "🏦 Banking System - Build and Setup Script"
echo "=========================================="

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on supported OS
check_system() {
    print_status "Checking system compatibility..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="Linux"
        print_success "Detected Linux system"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macOS"
        print_success "Detected macOS system"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
        OS="Windows"
        print_success "Detected Windows system with MSYS/Cygwin"
    else
        print_error "Unsupported operating system: $OSTYPE"
        exit 1
    fi
}

# Check for required tools
check_dependencies() {
    print_status "Checking dependencies..."
    
    # Check for C++ compiler
    if command -v g++ >/dev/null 2>&1; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_success "Found G++: $GCC_VERSION"
    elif command -v clang++ >/dev/null 2>&1; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        print_success "Found Clang++: $CLANG_VERSION"
    else
        print_error "No C++ compiler found. Please install g++ or clang++"
        exit 1
    fi
    
    # Check for CMake
    if command -v cmake >/dev/null 2>&1; then
        CMAKE_VERSION=$(cmake --version | head -n1)
        print_success "Found CMake: $CMAKE_VERSION"
    else
        print_error "CMake not found. Please install CMake 3.10+"
        if [[ "$OS" == "Linux" ]]; then
            echo "Install with: sudo apt-get install cmake"
        elif [[ "$OS" == "macOS" ]]; then
            echo "Install with: brew install cmake"
        fi
        exit 1
    fi
    
    # Check for Python
    if command -v python3 >/dev/null 2>&1; then
        PYTHON_VERSION=$(python3 --version)
        print_success "Found Python: $PYTHON_VERSION"
    else
        print_error "Python 3 not found. Please install Python 3.8+"
        exit 1
    fi
    
    # Check for pip
    if command -v pip3 >/dev/null 2>&1; then
        print_success "Found pip3"
    else
        print_warning "pip3 not found. Installing Python packages may fail"
    fi
}

# Create directory structure
create_structure() {
    print_status "Creating directory structure..."
    
    # Main directories
    mkdir -p banking_system/backend/{src/{core,models,services,utils,handlers},include/{core,models,services,utils,handlers},data/{accounts,transactions,users,logs},config,build,tests}
    mkdir -p banking_system/frontend/{pages,components,utils,assets/{images,css,js},config}
    mkdir -p banking_system/{scripts,docs/{api,user_guide,technical},tests/{integration,unit},config,logs}
    
    print_success "Directory structure created"
}

# Copy source files
copy_source_files() {
    print_status "Copying source files to correct locations..."
    
    # Create the correct directory structure and copy files
    cd banking_system
    
    # Copy header files
    cp ../Account.h backend/include/models/
    cp ../User.h backend/include/models/
    cp ../Transaction.h backend/include/models/
    cp ../DatabaseManager.h backend/include/services/
    cp ../ApiServer.h backend/include/core/
    
    # Copy source files
    cp ../Account.cpp backend/src/models/
    cp ../BankingService.cpp backend/src/core/
    cp ../ApiServer.cpp backend/src/core/
    
    # Copy build files
    cp ../CMakeLists.txt backend/
    
    # Copy frontend files
    cp ../banking_frontend.py frontend/
    
    # Copy documentation
    cp ../README.md .
    cp ../folder-structure-script.md docs/
    
    print_success "Source files copied"
    cd ..
}

# Build C++ backend
build_backend() {
    print_status "Building C++ backend..."
    
    cd banking_system/backend
    
    # Create missing source files
    create_missing_sources
    
    # Create build directory and compile
    cd build
    cmake ..
    
    # Determine number of cores for parallel compilation
    if command -v nproc >/dev/null 2>&1; then
        CORES=$(nproc)
    elif command -v sysctl >/dev/null 2>&1; then
        CORES=$(sysctl -n hw.ncpu)
    else
        CORES=4
    fi
    
    print_status "Compiling with $CORES cores..."
    make -j$CORES
    
    if [ $? -eq 0 ]; then
        print_success "Backend compiled successfully"
    else
        print_error "Backend compilation failed"
        exit 1
    fi
    
    cd ../../..
}

# Create missing source files
create_missing_sources() {
    print_status "Creating missing implementation files..."
    
    # Create User.cpp
    cat > src/models/User.cpp << 'EOF'
#include "../../../include/models/User.h"
#include <sstream>
#include <algorithm>
#include <random>
#include <iomanip>

User::User() : user_type("CUSTOMER"), status("INACTIVE") {
    created_date = std::chrono::system_clock::now();
    last_login = created_date;
}

User::User(const std::string& username, const std::string& password, 
           const std::string& email, const std::string& full_name)
    : username(username), email(email), full_name(full_name), 
      user_type("CUSTOMER"), status("ACTIVE") {
    
    // Generate user ID
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(1000, 9999);
    user_id = "USR" + std::to_string(dis(gen));
    
    // Hash password (simplified - use proper hashing in production)
    password_hash = std::to_string(std::hash<std::string>{}(password));
    
    created_date = std::chrono::system_clock::now();
    last_login = created_date;
}

// Implement other methods...
std::string User::getUserId() const { return user_id; }
std::string User::getUsername() const { return username; }
std::string User::getEmail() const { return email; }
std::string User::getFullName() const { return full_name; }
std::string User::getUserType() const { return user_type; }
std::string User::getStatus() const { return status; }

bool User::verifyPassword(const std::string& password) const {
    std::string hash = std::to_string(std::hash<std::string>{}(password));
    return hash == password_hash;
}

bool User::isValid() const {
    return !username.empty() && !email.empty() && !full_name.empty();
}

bool User::isActive() const {
    return status == "ACTIVE";
}

std::string User::toJson() const {
    std::stringstream json;
    json << "{"
         << "\"user_id\":\"" << user_id << "\","
         << "\"username\":\"" << username << "\","
         << "\"email\":\"" << email << "\","
         << "\"full_name\":\"" << full_name << "\","
         << "\"user_type\":\"" << user_type << "\","
         << "\"status\":\"" << status << "\""
         << "}";
    return json.str();
}
EOF

    # Create Transaction.cpp
    cat > src/models/Transaction.cpp << 'EOF'
#include "../../../include/models/Transaction.h"
#include <sstream>
#include <random>
#include <iomanip>

Transaction::Transaction() : amount(0.0), status("PENDING"), balance_before(0.0), balance_after(0.0) {
    timestamp = std::chrono::system_clock::now();
}

Transaction::Transaction(const std::string& acc_num, const std::string& type,
                        double amt, const std::string& desc)
    : account_number(acc_num), transaction_type(type), amount(amt), 
      description(desc), status("PENDING"), balance_before(0.0), balance_after(0.0) {
    
    // Generate transaction ID
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(100000, 999999);
    transaction_id = "TXN" + std::to_string(dis(gen));
    
    timestamp = std::chrono::system_clock::now();
}

// Implement getters
std::string Transaction::getTransactionId() const { return transaction_id; }
std::string Transaction::getAccountNumber() const { return account_number; }
std::string Transaction::getTransactionType() const { return transaction_type; }
double Transaction::getAmount() const { return amount; }
std::string Transaction::getDescription() const { return description; }
std::string Transaction::getStatus() const { return status; }

std::string Transaction::getTimestamp() const {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Implement setters
void Transaction::setStatus(const std::string& new_status) { status = new_status; }
void Transaction::setBalances(double before, double after) {
    balance_before = before;
    balance_after = after;
}
void Transaction::setReferenceNumber(const std::string& ref) { reference_number = ref; }

bool Transaction::isValid() const {
    return !transaction_id.empty() && !account_number.empty() && amount >= 0;
}

std::string Transaction::toJson() const {
    std::stringstream json;
    json << "{"
         << "\"transaction_id\":\"" << transaction_id << "\","
         << "\"account_number\":\"" << account_number << "\","
         << "\"transaction_type\":\"" << transaction_type << "\","
         << "\"amount\":" << std::fixed << std::setprecision(2) << amount << ","
         << "\"description\":\"" << description << "\","
         << "\"status\":\"" << status << "\","
         << "\"timestamp\":\"" << getTimestamp() << "\""
         << "}";
    return json.str();
}
EOF

    # Create DatabaseManager.cpp
    cat > src/services/DatabaseManager.cpp << 'EOF'
#include "../../../include/services/DatabaseManager.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>

DatabaseManager::DatabaseManager(const std::string& data_dir) : data_directory(data_dir) {
    accounts_file = data_directory + "/accounts/accounts.csv";
    users_file = data_directory + "/users/users.csv";
    transactions_file = data_directory + "/transactions/transactions.csv";
    logs_file = data_directory + "/logs/system.log";
}

bool DatabaseManager::initialize() {
    return createDefaultDirectories();
}

bool DatabaseManager::createDefaultDirectories() {
    // Create directory structure
    mkdir((data_directory).c_str(), 0755);
    mkdir((data_directory + "/accounts").c_str(), 0755);
    mkdir((data_directory + "/users").c_str(), 0755);
    mkdir((data_directory + "/transactions").c_str(), 0755);
    mkdir((data_directory + "/logs").c_str(), 0755);
    
    return true;
}

bool DatabaseManager::saveAccount(const Account& account) {
    std::lock_guard<std::mutex> lock(db_mutex);
    
    std::ofstream file(accounts_file, std::ios::app);
    if (file.is_open()) {
        file << account.toCsvRow() << std::endl;
        file.close();
        return true;
    }
    return false;
}

bool DatabaseManager::saveUser(const User& user) {
    std::lock_guard<std::mutex> lock(db_mutex);
    
    std::ofstream file(users_file, std::ios::app);
    if (file.is_open()) {
        file << user.toCsvRow() << std::endl;
        file.close();
        return true;
    }
    return false;
}

bool DatabaseManager::saveTransaction(const Transaction& transaction) {
    std::lock_guard<std::mutex> lock(db_mutex);
    
    std::ofstream file(transactions_file, std::ios::app);
    if (file.is_open()) {
        file << transaction.toCsvRow() << std::endl;
        file.close();
        return true;
    }
    return false;
}

// Simplified implementations for demo
std::unique_ptr<Account> DatabaseManager::loadAccount(const std::string& account_number) {
    return nullptr; // Simplified for demo
}

std::unique_ptr<User> DatabaseManager::loadUser(const std::string& username) {
    return nullptr; // Simplified for demo
}
EOF

    print_success "Missing source files created"
}

# Setup Python environment
setup_python() {
    print_status "Setting up Python environment..."
    
    cd banking_system/frontend
    
    # Create requirements.txt
    cat > requirements.txt << 'EOF'
streamlit>=1.28.0
pandas>=1.5.0
requests>=2.28.0
numpy>=1.21.0
plotly>=5.0.0
altair>=4.2.0
EOF
    
    # Install Python dependencies
    print_status "Installing Python dependencies..."
    pip3 install -r requirements.txt
    
    if [ $? -eq 0 ]; then
        print_success "Python dependencies installed"
    else
        print_warning "Some Python dependencies may have failed to install"
    fi
    
    cd ../..
}

# Create run scripts
create_run_scripts() {
    print_status "Creating run scripts..."
    
    cd banking_system
    
    # Create backend run script
    cat > scripts/run_backend.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")/../backend/build"
echo "Starting Banking System Backend..."
./banking_system
EOF
    
    # Create frontend run script
    cat > scripts/run_frontend.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")/../frontend"
echo "Starting Banking System Frontend..."
streamlit run banking_frontend.py --server.port 8501
EOF
    
    # Create full system run script
    cat > scripts/run_system.sh << 'EOF'
#!/bin/bash
echo "🏦 Starting Banking System..."

# Start backend in background
echo "Starting backend..."
cd "$(dirname "$0")/../backend/build"
./banking_system &
BACKEND_PID=$!

# Wait a moment for backend to start
sleep 2

# Start frontend
echo "Starting frontend..."
cd "$(dirname "$0")/../frontend"
streamlit run banking_frontend.py --server.port 8501 &
FRONTEND_PID=$!

echo "✅ Banking System started!"
echo "Backend PID: $BACKEND_PID"
echo "Frontend PID: $FRONTEND_PID"
echo "Access the web interface at: http://localhost:8501"
echo ""
echo "To stop the system, press Ctrl+C"

# Wait for user interrupt
trap "echo 'Stopping Banking System...'; kill $BACKEND_PID $FRONTEND_PID; exit" INT
wait
EOF
    
    # Make scripts executable
    chmod +x scripts/*.sh
    
    print_success "Run scripts created"
    cd ..
}

# Create sample data
create_sample_data() {
    print_status "Creating sample data..."
    
    cd banking_system/backend/data
    
    # Create sample accounts
    cat > accounts/accounts.csv << 'EOF'
account_number,customer_id,account_type,balance,status,created_date,last_updated
ACC12345678,USR1001,SAVINGS,1500.00,ACTIVE,2024-01-15 10:30:00,2024-01-20 14:45:00
ACC87654321,USR1001,CHECKING,750.50,ACTIVE,2024-02-01 09:15:00,2024-02-10 16:20:00
EOF
    
    # Create sample users
    cat > users/users.csv << 'EOF'
user_id,username,password_hash,email,full_name,phone,address,user_type,status,created_date,last_login,account_numbers
USR1001,john_doe,5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8,john@example.com,John Doe,555-0123,123 Main St,CUSTOMER,ACTIVE,2024-01-15 10:00:00,2024-01-20 15:30:00,ACC12345678;ACC87654321
EOF
    
    # Create sample transactions
    cat > transactions/transactions.csv << 'EOF'
transaction_id,account_number,transaction_type,amount,description,status,timestamp,reference_number,target_account,balance_before,balance_after
TXN100001,ACC12345678,DEPOSIT,1000.00,Initial deposit,COMPLETED,2024-01-15 10:30:00,REF001,,0.00,1000.00
TXN100002,ACC12345678,DEPOSIT,500.00,Salary deposit,COMPLETED,2024-01-20 14:45:00,REF002,,1000.00,1500.00
TXN100003,ACC87654321,DEPOSIT,750.50,Transfer from savings,COMPLETED,2024-02-01 09:15:00,REF003,,0.00,750.50
EOF
    
    print_success "Sample data created"
    cd ../../..
}

# Main execution
main() {
    echo ""
    print_status "Starting Banking System build process..."
    echo ""
    
    check_system
    check_dependencies
    create_structure
    copy_source_files
    build_backend
    setup_python
    create_run_scripts
    create_sample_data
    
    echo ""
    print_success "🎉 Banking System build completed successfully!"
    echo ""
    echo -e "${BLUE}Next steps:${NC}"
    echo "1. cd banking_system"
    echo "2. ./scripts/run_system.sh    # Start both backend and frontend"
    echo "   OR"
    echo "   ./scripts/run_backend.sh   # Start only backend"
    echo "   ./scripts/run_frontend.sh  # Start only frontend"
    echo ""
    echo -e "${BLUE}Access the system:${NC}"
    echo "- Web Interface: http://localhost:8501"
    echo "- Backend CLI: Run backend directly for command-line interface"
    echo ""
    echo -e "${BLUE}Default test credentials:${NC}"
    echo "- Username: john_doe"
    echo "- Password: password123"
    echo ""
    print_success "Happy banking! 🏦"
}

# Run main function
main "$@"