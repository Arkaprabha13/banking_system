
# Banking System v1.0

![C++](https://img.shields.io/badge/C++-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.26+-brightgreen)
![Streamlit](https://img.shields.io/badge/Streamlit-1.34.0-green)
![Web](https://img.shields.io/badge/Web-HTML/CSS/JS-orange)

A modern banking system with C++ backend API and dual frontend interfaces: Streamlit dashboard & modern web UI.

## 📂 Project Structure
```
banking_system/
├── 📁 backend/ # C++ Core Banking Engine
│ ├── 🔧 CMakeLists.txt # Build configuration
│ ├── 📁 build/ # Compiled binaries & artifacts
│ │ ├── 🚀 banking_system.exe # Main executable
│ │ ├── 📚 libbanking_lib.a # Banking library
│ │ └── 📁 data/ # Runtime data storage
│ │ ├── 👥 users/ # User accounts & credentials
│ │ ├── 💳 accounts/ # Bank account records
│ │ ├── 💸 transactions/ # Transaction history
│ │ └── 📋 logs/ # System audit logs
│ ├── 📁 include/ # C++ Header Files
│ │ ├── 🗄️ core/ # Database & core functionality
│ │ │ └── Database.h # Data persistence layer
│ │ ├── 📊 models/ # Business entities
│ │ │ ├── User.h # User account model
│ │ │ ├── Account.h # Bank account model
│ │ │ └── Transaction.h # Transaction model
│ │ ├── 🌐 services/ # Business logic & API
│ │ │ ├── BankingService.h # Core banking operations
│ │ │ └── ApiServer.h # REST API server
│ │ └── 🔧 utils/ # Helper utilities
│ ├── 📁 src/ # C++ Implementation Files
│ │ ├── 🎯 main.cpp # Application entry point
│ │ ├── 🗄️ core/ # Core implementations
│ │ ├── 📊 models/ # Model implementations
│ │ └── 🌐 services/ # Service implementations
│ └── 🧪 tests/ # Unit & integration tests
│
├── 📁 frontend/ # Streamlit Dashboard (Python)
│ ├── 🐍 app.py # Main Streamlit application
│ ├── 📋 requirements.txt # Python dependencies
│ ├── 📁 assets/ # Static resources
│ │ ├── 🎨 css/ # Custom stylesheets
│ │ ├── 🖼️ images/ # UI images & icons
│ │ └── ⚡ js/ # JavaScript components
│ ├── 🧩 components/ # Reusable UI components
│ ├── 📄 pages/ # Multi-page navigation
│ └── 🔧 utils/ # Python helper functions
│ └── helpers.py # Utility functions
│
├── 📁 web/ # Modern Web Interface (HTML/CSS/JS)
│ ├── 🌐 index.html # Main web entry point
│ └── ⚡ app.js # Frontend JavaScript logic
│
├── 📁 config/ # System Configuration
│ └── ⚙️ config.json # Application settings
│
├── 📁 scripts/ # Build & Deployment Scripts
│ ├── 🏗️ build.bat # Windows build script
│ ├── 🏗️ build.sh # Linux/Mac build script
│ └── 🚀 s.sh # Quick start script
│
├── 📁 docs/ # Documentation
│ ├── 📡 api/ # API documentation
│ ├── 🔧 technical/ # Technical specifications
│ └── 📖 user_guide/ # User manuals
│
├── 📁 tests/ # Test Suites
│ ├── 🔗 integration/ # End-to-end tests
│ └── 🧪 unit/ # Component tests
│
├── 📁 logs/ # Application Logs
├── 📄 README.md # This file
├── 📄 USAGE_GUIDE.md # Detailed usage instructions
├── 🚀 run_banking_system.bat # Windows launcher
└── 🚀 run_banking_system.ps1 # PowerShell launcher
```

## 🌟 Key Features

### Backend (C++17)
- **REST API**: Unified endpoints for both frontends
- **User Management**: JWT authentication with role-based access
- **Transaction Processing**: Real-time validation and auditing
- **File-based Storage**: ACID-compliant CSV persistence

### Frontend Options
1. **Streamlit Dashboard** (Python):
   - Rapid prototyping interface
   - Data visualization & admin controls
   - Real-time transaction monitoring

2. **Web Interface** (HTML/CSS/JS):
   - Responsive mobile-first design
   - Interactive transaction forms
   - Real-time balance updates
   - Modern UI components

## 🚀 Installation

### Prerequisites
- C++17 compiler (GCC 9+/Clang 12+/MSVC 2019+)
- CMake 3.26+
- Python 3.8+ (for Streamlit)
- Html CSS JS (for web frontend tooling - optional)

### Setup
```
# Clone repository
git clone [https://github.com/yourusername/banking_system.git](https://github.com/Arkaprabha13/banking_system.git)
cd banking_system

# Build backend
cd backend
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Install Streamlit frontend
cd ../streamlit_frontend
pip install -r requirements.txt

# Prepare web frontend
cd ../web_frontend
npm install  # Optional for development
```

## ⚙️ Configuration

Edit `config/config.json`:
```
{
  "api_base_url": "http://localhost:8080/api",
  "enable_cors": true,
  "frontend_mode": "dual"
}
```

## 🖥️ Running the System

### Backend API
```
./backend/build/banking_system
```

### Streamlit Frontend
```
streamlit run streamlit_frontend/app.py
```

### Web Frontend
```
cd web_frontend
live-server --port=3000  # Or open index.html directly
```

Access interfaces at:
- Streamlit: `http://localhost:8501`
- Web UI: `http://localhost:3000`

## 🔌 API Endpoints

| Method | Endpoint               | Description                     |
|--------|------------------------|---------------------------------|
| POST   | /api/login             | User authentication            |
| POST   | /api/accounts/create   | Create new account              |
| POST   | /api/transactions      | Create new transaction          |

Full API documentation in `/docs/api/`.

## 🧪 Testing

```
# Backend tests
cd backend/build
ctest -V

# Web frontend tests
cd ../web_frontend
npm test
```

## 🤝 Contributing

1. Fork the repository
2. Create feature branch (`git checkout -b feature/improvement`)
3. Commit changes following [Conventional Commits](https://www.conventionalcommits.org/)
4. Push to branch (`git push origin feature/improvement`)
5. Open Pull Request

**Frontend Contributions:**
- Streamlit components: `streamlit_frontend/`
- Web components: `web_frontend/`


## ⚠️ Important Notes

- Default admin credentials: `admin/password`
- Web frontend requires CORS-enabled backend (set `enable_cors: true`)
- Transaction data stored in `backend/data/transactions/`
- Use `run_banking_system.ps1` for Windows environments
- CSV files are automatically created on first run
```
```
# 🚀 Future Goals & Roadmap

## 📅 Version 2.0 - Enhanced Security & Performance  
**Target:** Q3 2025

### 🔐 Security Enhancements
- **Two-Factor Authentication (2FA):** SMS/Email verification
- **JWT Token Management:** Secure session handling with refresh tokens
- **Password Encryption:** Upgrade from basic hashing to bcrypt/Argon2
- **Audit Logging:** Comprehensive security event tracking
- **Role-Based Access Control:** Granular permissions (Admin, Manager, Teller, Customer)
- **API Rate Limiting:** Prevent brute force attacks
- **Data Encryption:** Encrypt sensitive CSV data at rest

### ⚡ Performance Optimizations
- **Database Migration:** Move from CSV to SQLite/PostgreSQL
- **Connection Pooling:** Optimize database connections
- **Caching Layer:** Redis integration for session management
- **Async Processing:** Non-blocking transaction processing
- **Load Balancing:** Multi-instance deployment support

---

## 📅 Version 3.0 - Advanced Banking Features  
**Target:** Q1 2026

### 💳 Financial Products
- **Loan Management System:** Personal, business, and mortgage loans
- **Credit Card Processing:** Virtual card generation and management
- **Investment Portfolio:** Stock trading and portfolio tracking
- **Savings Goals:** Automated savings plans with interest calculation
- **Bill Payment System:** Recurring payments and scheduling
- **Currency Exchange:** Multi-currency support with real-time rates

### 📊 Analytics & Reporting
- **Financial Dashboard:** Advanced charts and spending analytics
- **Budget Planning:** AI-powered budget recommendations
- **Transaction Categorization:** Automatic expense categorization
- **Monthly Statements:** PDF generation and email delivery
- **Compliance Reporting:** Regulatory compliance automation
- **Risk Assessment:** Fraud detection algorithms

---

## 📅 Version 4.0 - Mobile & Integration  
**Target:** Q3 2026

### 📱 Mobile Applications
- **React Native App:** Cross-platform mobile banking
- **Progressive Web App (PWA):** Offline-capable web app
- **Biometric Authentication:** Fingerprint and face recognition
- **Push Notifications:** Real-time transaction alerts
- **QR Code Payments:** Contactless payment system
- **Mobile Check Deposit:** Camera-based check scanning

### 🔗 Third-Party Integrations
- **Payment Gateways:** Stripe, PayPal, Razorpay integration
- **Open Banking APIs:** PSD2 compliance and third-party access
- **Accounting Software:** QuickBooks, Xero synchronization
- **Tax Software:** Automated tax document generation
- **Credit Bureau Integration:** Real-time credit score monitoring
- **Blockchain Support:** Cryptocurrency wallet integration

---

## 📅 Version 5.0 - AI & Machine Learning  
**Target:** Q1 2027

### 🤖 Artificial Intelligence
- **Chatbot Support:** AI-powered customer service
- **Fraud Detection:** Machine learning anomaly detection
- **Personalized Recommendations:** Smart financial advice
- **Predictive Analytics:** Cash flow forecasting
- **Voice Banking:** Voice-activated transactions
- **Document Processing:** OCR for automatic form filling

### 🌐 Cloud & Scalability
- **Microservices Architecture:** Container-based deployment
- **Cloud Migration:** AWS/Azure/GCP deployment
- **Auto-scaling:** Dynamic resource allocation
- **Global CDN:** Worldwide content delivery
- **Multi-region Deployment:** Geographic redundancy
- **Disaster Recovery:** Automated backup and restoration

---

## 🎯 Long-term Vision (2027+)

### 🌍 Enterprise Features
- **Multi-tenant Architecture:** Support multiple banks
- **White-label Solution:** Customizable branding
- **Regulatory Compliance:** GDPR, PCI-DSS, SOX compliance
- **Enterprise SSO:** LDAP/Active Directory integration
- **API Marketplace:** Third-party developer ecosystem
- **Real-time Analytics:** Live transaction monitoring

### 🔮 Emerging Technologies
- **Quantum Security:** Post-quantum cryptography
- **IoT Integration:** Smart device payments
- **Augmented Reality:** AR-based banking interfaces
- **5G Optimization:** Ultra-low latency transactions
- **Edge Computing:** Distributed processing nodes
- **Digital Identity:** Self-sovereign identity management

