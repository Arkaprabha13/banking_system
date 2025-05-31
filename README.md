Here's the modified README.md incorporating your project structure and dual frontend approach:

```markdown
# Banking System v1.0

![C++](https://img.shields.io/badge/C++-17-blue)
![CMake](https://img.shields.io/badge/CMake-3.26+-brightgreen)
![Streamlit](https://img.shields.io/badge/Streamlit-1.34.0-green)
![Web](https://img.shields.io/badge/Web-HTML/CSS/JS-orange)

A modern banking system with C++ backend API and dual frontend interfaces: Streamlit dashboard & modern web UI.

## 📂 Project Structure

```banking_system/
├── backend/ # C++ Backend
│ ├── build/ # Build artifacts
│ │ ├── .ninja_deps
│ │ ├── .ninja_log
│ │ ├── banking_system.exe
│ │ ├── build.ninja
│ │ ├── CMakeCache.txt
│ │ ├── cmake_install.cmake
│ │ ├── libbanking_lib.a
│ │ ├── CMakeFiles/
│ │ │ ├── cmake.check_cache
│ │ │ ├── CMakeConfigureLog.yaml
│ │ │ ├── InstallScripts.json
│ │ │ ├── rules.ninja
│ │ │ ├── TargetDirectories.txt
│ │ ├── banking_lib.dir/
│ │ │ ├── src/
│ │ │ │ ├── core/
│ │ │ │ ├── models/
│ │ │ │ └── services/
│ │ ├── banking_system.dir/
│ │ │ ├── src/
│ │ ├── data/
│ │ │ ├── accounts/
│ │ │ │ └── accounts.csv
│ │ │ ├── logs/
│ │ │ │ └── system.log
│ │ │ ├── transactions/
│ │ │ │ └── transactions.csv
│ │ │ └── users/
│ │ │ └── users.csv
│ ├── config/
│ ├── data/
│ │ ├── accounts/
│ │ │ └── accounts.csv
│ │ ├── logs/
│ │ │ └── system.log
│ │ ├── transactions/
│ │ │ └── transactions.csv
│ │ └── users/
│ │ └── users.csv
│ ├── include/
│ │ ├── core/
│ │ │ └── Database.h
│ │ ├── handlers/
│ │ ├── models/
│ │ │ ├── Account.h
│ │ │ ├── Transaction.h
│ │ │ └── User.h
│ │ ├── services/
│ │ │ ├── ApiServer.h
│ │ │ └── BankingService.h
│ │ └── utils/
│ ├── src/
│ │ ├── main.cpp
│ │ ├── core/
│ │ │ └── Database.cpp
│ │ ├── handlers/
│ │ ├── models/
│ │ │ ├── Account.cpp
│ │ │ ├── Transaction.cpp
│ │ │ └── User.cpp
│ │ ├── services/
│ │ │ ├── ApiServer.cpp
│ │ │ ├── BankingService.cpp
│ │ └── utils/
│ ├── tests/
├── config/
│ └── config.json
├── docs/
│ ├── api/
│ ├── technical/
│ └── user_guide/
├── frontend/
│ ├── app.py
│ ├── requirements.txt
│ ├── assets/
│ │ ├── css/
│ │ ├── images/
│ │ └── js/
│ ├── components/
│ ├── config/
│ ├── pages/
│ └── utils/
│ └── helpers.py
├── logs/
├── scripts/
│ ├── build.bat
│ ├── build.sh
│ ├── s.sh
│ ├── backend/
│ │ ├── build/
│ │ └── data/
│ │ ├── accounts/
│ │ ├── logs/
│ │ ├── transactions/
│ │ └── users/
│ ├── banking_system/
│ │ ├── backend/
│ │ │ ├── build/
│ │ │ ├── config/
│ │ │ ├── data/
│ │ │ │ ├── accounts/
│ │ │ │ ├── logs/
│ │ │ │ ├── transactions/
│ │ │ │ └── users/
│ │ │ ├── include/
│ │ │ ├── src/
│ │ │ └── tests/
│ │ ├── config/
│ │ ├── docs/
│ │ ├── frontend/
│ │ │ ├── assets/
│ │ │ │ ├── css/
│ │ │ │ ├── images/
│ │ │ │ └── js/
│ │ │ ├── components/
│ │ │ ├── config/
│ │ │ ├── pages/
│ │ │ └── utils/
│ │ ├── logs/
│ │ ├── scripts/
│ │ └── tests/
│ │ ├── integration/
│ │ └── unit/
│ ├── docs/
│ │ ├── api/
│ │ ├── technical/
│ │ └── user_guide/
│ ├── frontend/
│ │ ├── assets/
│ │ │ ├── css/
│ │ │ ├── images/
│ │ │ └── js/
│ │ ├── components/
│ │ ├── pages/
│ │ └── utils/
│ ├── logs/
│ ├── scripts/
│ └── tests/
│ ├── integration/
│ └── unit/
├── tests/
│ ├── integration/
│ └── unit/
└── web/
├── app.js
└── index.html
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
- Node.js 18+ (for web frontend tooling - optional)

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

## 📄 License

MIT License - See [LICENSE](LICENSE) for details.

## ⚠️ Important Notes

- Default admin credentials: `admin/password`
- Web frontend requires CORS-enabled backend (set `enable_cors: true`)
- Transaction data stored in `backend/data/transactions/`
- Use `run_banking_system.ps1` for Windows environments
- CSV files are automatically created on first run
```

Key changes made:
1. Updated project structure to reflect actual directory layout
2. Added web frontend specifics in installation and running sections
3. Included Node.js/npm in prerequisites for web frontend development
4. Added separate testing instructions for web frontend
5. Enhanced contribution guidelines for dual frontend development
6. Added CORS configuration note for web interface
7. Streamlined technology stack badges
8. Improved Windows compatibility notes
9. Added automatic CSV creation note
10. Organized frontend-specific documentation in contributing section

The README now better reflects your actual project structure and provides clear guidance for both frontend options while maintaining backend focus.
