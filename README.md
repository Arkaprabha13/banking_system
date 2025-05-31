# Banking System - Complete Financial Platform

A comprehensive banking system with C++ backend and Streamlit frontend, featuring user management, account operations, transaction processing, and file-based storage.

## 🏗️ Architecture Overview

```
banking_system/
├── backend/           # C++ Backend
│   ├── include/       # Header files
│   │   ├── models/    # Data models (User, Account, Transaction)
│   │   ├── core/      # Core services (Database)
│   │   └── services/  # Business logic (BankingService, ApiServer)
│   ├── src/           # Source files
│   │   ├── models/    # Model implementations
│   │   ├── core/      # Core service implementations
│   │   └── services/  # Service implementations
│   ├── data/          # CSV data storage
│   ├── build/         # Build artifacts
│   └── CMakeLists.txt # Build configuration
├── frontend/          # Streamlit Web Interface
│   ├── app.py         # Main application
│   ├── utils/         # Helper utilities
│   └── requirements.txt # Python dependencies
├── scripts/           # Build and deployment scripts
├── docs/             # Documentation
├── tests/            # Test suites
└── config/           # Configuration files
```

## 🚀 Features

### Backend (C++)
- **Object-Oriented Design**: Modern C++17 with smart pointers, RAII, and strong typing
- **User Management**: Registration, authentication, role-based access control
- **Account Operations**: Multiple account types (Checking, Savings, Business)
- **Transaction Processing**: Deposits, withdrawals, transfers with validation
- **File-Based Storage**: CSV/JSON persistence with backup and recovery
- **API Server**: HTTP REST API for frontend communication
- **Thread Safety**: Mutex-protected operations for concurrent access
- **Comprehensive Logging**: Detailed operation logs and audit trails

### Frontend (Streamlit)
- **Modern Web Interface**: Responsive design with intuitive navigation
- **User Authentication**: Secure login/logout with session management
- **Dashboard**: Account overview, balance summaries, recent transactions
- **Account Management**: Create accounts, view details, manage settings
- **Transaction Processing**: Easy deposits, withdrawals, and transfers
- **Real-time Updates**: Live balance and transaction updates
- **Error Handling**: Comprehensive validation and user feedback

## 🛠️ Installation & Setup

### Prerequisites
- **C++ Compiler**: GCC 7+ or MSVC 2019+ with C++17 support
- **CMake**: Version 3.15 or higher
- **Python**: Version 3.8 or higher
- **Git**: For version control

### Quick Start

1. **Clone the Repository**
```bash
git clone <repository-url>
cd banking_system
```

2. **Build the Backend**

**Linux/macOS:**
```bash
chmod +x scripts/build.sh
./scripts/build.sh
```

**Windows:**
```cmd
scripts\build.bat
```

3. **Setup Frontend**
```bash
cd frontend
pip install -r requirements.txt
```

4. **Run the System**

**Terminal 1 - Backend Server:**
```bash
cd backend/build
./banking_system  # Linux/macOS
banking_system.exe  # Windows
```

**Terminal 2 - Frontend Interface:**
```bash
cd frontend
streamlit run app.py
```

5. **Access the Application**
- Open your web browser
- Navigate to `http://localhost:8501`
- Use the web interface to interact with the banking system

## 📚 API Documentation

### Authentication Endpoints

#### POST /api/login
Login with username and password.

**Request:**
```json
{
    "username": "johndoe",
    "password": "password123"
}
```

**Response:**
```json
{
    "success": true,
    "message": "Login successful",
    "username": "johndoe"
}
```

#### POST /api/register
Register a new user account.

**Request:**
```json
{
    "username": "newuser",
    "password": "password123",
    "email": "user@email.com",
    "full_name": "New User"
}
```

### Account Management

#### GET /api/accounts?username=<username>
Get all accounts for a user.

**Response:**
```json
{
    "accounts": [
        {
            "account_number": "ACC123456",
            "account_type": "CHECKING",
            "balance": 1500.00
        }
    ]
}
```

#### POST /api/accounts/create
Create a new account.

**Request:**
```json
{
    "username": "johndoe",
    "account_type": "SAVINGS"
}
```

### Transaction Operations

#### POST /api/transactions/deposit
Make a deposit to an account.

**Request:**
```json
{
    "account_number": "ACC123456",
    "amount": 500.00
}
```

#### POST /api/transactions/withdraw
Withdraw from an account.

**Request:**
```json
{
    "account_number": "ACC123456",
    "amount": 200.00
}
```

#### POST /api/transactions/transfer
Transfer between accounts.

**Request:**
```json
{
    "from_account": "ACC123456",
    "to_account": "ACC789012",
    "amount": 300.00
}
```

#### GET /api/transactions?account_number=<account_number>
Get transaction history for an account.

## 🗄️ Database Schema

### Users Table (users.csv)
```
user_id,username,password_hash,email,full_name,phone_number,role,is_active,failed_login_attempts,last_login,created_date
```

### Accounts Table (accounts.csv)
```
account_number,customer_id,account_type,balance,status,daily_limit,minimum_balance,created_date,last_updated
```

### Transactions Table (transactions.csv)
```
transaction_id,from_account_id,to_account_id,amount,type,status,description,balance_before,balance_after,timestamp,reference_number
```

## 🏃‍♂️ Usage Examples

### Sample User Workflow

1. **Register New User**
   - Navigate to registration page
   - Fill in user details
   - Create account

2. **Login**
   - Use credentials to login
   - Access dashboard

3. **Create Account**
   - Choose account type (Checking/Savings)
   - Set initial preferences

4. **Make Transactions**
   - Deposit funds
   - Check balance
   - Transfer to other accounts
   - View transaction history

### Sample API Calls

```bash
# Register user
curl -X POST http://localhost:8080/api/register \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","password":"test123","email":"test@email.com","full_name":"Test User"}'

# Login
curl -X POST http://localhost:8080/api/login \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","password":"test123"}'

# Create account
curl -X POST http://localhost:8080/api/accounts/create \
  -H "Content-Type: application/json" \
  -d '{"username":"testuser","account_type":"CHECKING"}'

# Make deposit
curl -X POST http://localhost:8080/api/transactions/deposit \
  -H "Content-Type: application/json" \
  -d '{"account_number":"ACC123456","amount":1000.00}'
```

## 🔧 Configuration

### Backend Configuration
- **Server Port**: Default 8080 (configurable in main.cpp)
- **Data Directory**: `backend/data/` (configurable in Database constructor)
- **Log Level**: Info level logging (configurable in source)

### Frontend Configuration
- **Streamlit Port**: Default 8501
- **API Endpoint**: `http://localhost:8080` (configurable in app.py)
- **Session Timeout**: 30 minutes

## 📁 File Structure Details

```
backend/
├── include/
│   ├── models/
│   │   ├── User.h           # User entity and operations
│   │   ├── Account.h        # Account management
│   │   └── Transaction.h    # Transaction processing
│   ├── core/
│   │   └── Database.h       # Data persistence layer
│   └── services/
│       ├── BankingService.h # Business logic layer
│       └── ApiServer.h      # HTTP API server
├── src/
│   ├── models/              # Model implementations
│   ├── core/                # Core service implementations
│   ├── services/            # Service layer implementations
│   └── main.cpp             # Application entry point
├── data/                    # File-based storage
│   ├── users/
│   ├── accounts/
│   ├── transactions/
│   └── logs/
└── build/                   # Compilation artifacts
```

## 🧪 Testing

### Running Tests
```bash
cd backend/build
ctest                # Run all tests
ctest -V             # Run with verbose output
```

### Test Coverage
- Unit tests for all model classes
- Service layer integration tests
- API endpoint testing
- Database operation validation

## 📊 Performance Considerations

### Backend Optimizations
- **Memory Management**: RAII and smart pointers prevent memory leaks
- **Thread Safety**: Mutex protection for shared resources
- **File I/O**: Efficient CSV parsing and writing
- **Error Handling**: Comprehensive exception handling

### Scalability Notes
- **Current Design**: File-based storage suitable for small to medium datasets
- **Future Enhancements**: Can be extended to use SQL databases (SQLite, PostgreSQL)
- **Concurrent Access**: Mutex-protected operations support multiple clients
- **API Performance**: Simple HTTP server suitable for development and testing

## 🔒 Security Features

### Authentication & Authorization
- **Password Hashing**: Secure password storage (implement bcrypt in production)
- **Session Management**: Secure session handling
- **Role-Based Access**: Admin, Manager, Customer roles
- **Account Lockout**: Protection against brute force attacks

### Data Protection
- **Input Validation**: Comprehensive data validation
- **SQL Injection Protection**: Not applicable (CSV storage)
- **CORS Handling**: Cross-origin request support
- **Audit Logging**: Complete transaction audit trail

## 🚀 Deployment

### Development Deployment
1. Use provided build scripts
2. Run backend and frontend separately
3. Access via localhost

### Production Considerations
- **Database**: Replace CSV with production database
- **Security**: Implement HTTPS, proper authentication
- **Monitoring**: Add application monitoring and logging
- **Scaling**: Consider microservices architecture
- **Backup**: Automated data backup strategies

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🆘 Support

For support and questions:
- Create an issue in the repository
- Check the documentation in the `docs/` directory
- Review the API documentation above

## 🎯 Future Enhancements

### Planned Features
- **Database Integration**: PostgreSQL/MySQL support
- **Advanced Security**: Two-factor authentication, encryption
- **Reporting**: Advanced financial reports and analytics
- **Mobile App**: React Native mobile application
- **Microservices**: Break into smaller, scalable services
- **Real-time Notifications**: WebSocket-based notifications
- **Advanced UI**: More sophisticated frontend with React/Vue.js

### Performance Improvements
- **Caching**: Redis integration for session management
- **Load Balancing**: Support for multiple backend instances
- **Database Optimization**: Indexing and query optimization
- **API Gateway**: Centralized API management

---

**Built with ❤️ using C++ and Streamlit**
