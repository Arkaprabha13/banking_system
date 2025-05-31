#!/bin/bash

# Banking System Build Script
# Automates the complete setup and build process

echo "========================================="
echo "  Banking System Build Script"
echo "========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running on supported OS
check_os() {
    print_status "Checking operating system..."
    
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        OS="linux"
        print_status "Detected Linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
        print_status "Detected macOS"
    elif [[ "$OSTYPE" == "cygwin" ]] || [[ "$OSTYPE" == "msys" ]]; then
        OS="windows"
        print_status "Detected Windows (WSL/Cygwin)"
    else
        print_error "Unsupported operating system: $OSTYPE"
        exit 1
    fi
}

# Check required tools
check_dependencies() {
    print_status "Checking dependencies..."
    
    # Check for C++ compiler
    if command -v g++ &> /dev/null; then
        print_status "g++ compiler found: $(g++ --version | head -n1)"
    elif command -v clang++ &> /dev/null; then
        print_status "clang++ compiler found: $(clang++ --version | head -n1)"
    else
        print_error "No C++ compiler found. Please install g++ or clang++"
        exit 1
    fi
    
    # Check for CMake
    if command -v cmake &> /dev/null; then
        print_status "CMake found: $(cmake --version | head -n1)"
    else
        print_error "CMake not found. Please install CMake"
        exit 1
    fi
    
    # Check for Python
    if command -v python3 &> /dev/null; then
        print_status "Python found: $(python3 --version)"
    elif command -v python &> /dev/null; then
        print_status "Python found: $(python --version)"
    else
        print_error "Python not found. Please install Python 3.7+"
        exit 1
    fi
    
    # Check for pip
    if command -v pip3 &> /dev/null; then
        PIP_CMD="pip3"
    elif command -v pip &> /dev/null; then
        PIP_CMD="pip"
    else
        print_error "pip not found. Please install pip"
        exit 1
    fi
    
    print_status "All dependencies found"
}

# Create directory structure
create_directories() {
    print_status "Creating directory structure..."
    
    # Backend directories
    mkdir -p backend/build
    mkdir -p backend/data/{users,accounts,transactions,logs}
    
    # Frontend directories
    mkdir -p frontend/{components,pages,utils,assets/{css,js,images}}
    
    # Other directories
    mkdir -p docs/{api,technical,user_guide}
    mkdir -p tests/{unit,integration}
    mkdir -p scripts
    mkdir -p logs
    
    print_status "Directory structure created"
}

# Build backend
build_backend() {
    print_status "Building C++ backend..."
    
    cd backend
    
    # Create build directory if it doesn't exist
    mkdir -p build
    cd build
    
    # Run CMake
    print_status "Running CMake..."
    if cmake .. -DCMAKE_BUILD_TYPE=Release; then
        print_status "CMake configuration successful"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
    
    # Build the project
    print_status "Building project..."
    if make -j$(nproc 2>/dev/null || echo 4); then
        print_status "Backend build successful"
    else
        print_error "Backend build failed"
        exit 1
    fi
    
    cd ../..
}

# Setup Python environment
setup_python_env() {
    print_status "Setting up Python environment..."
    
    cd frontend
    
    # Install Python dependencies
    print_status "Installing Python dependencies..."
    if $PIP_CMD install -r requirements.txt; then
        print_status "Python dependencies installed successfully"
    else
        print_error "Failed to install Python dependencies"
        exit 1
    fi
    
    cd ..
}

# Create sample data
create_sample_data() {
    print_status "Creating sample data..."
    
    # Create sample configuration
    cat > backend/data/config.json << EOF
{
    "database": {
        "data_directory": "data",
        "backup_interval": 3600,
        "max_backup_files": 10
    },
    "security": {
        "password_min_length": 6,
        "max_login_attempts": 5,
        "session_timeout": 1800
    },
    "limits": {
        "max_transaction_amount": 1000000,
        "daily_withdrawal_limit": 5000,
        "max_accounts_per_user": 5
    }
}
EOF
    
    print_status "Sample data created"
}

# Create run scripts
create_run_scripts() {
    print_status "Creating run scripts..."
    
    # Backend run script
    cat > scripts/run_backend.sh << 'EOF'
#!/bin/bash
echo "Starting Banking System Backend..."
cd backend/build
./banking_system --port 8080 --data ../data
EOF
    
    # Frontend run script
    cat > scripts/run_frontend.sh << 'EOF'
#!/bin/bash
echo "Starting Banking System Frontend..."
cd frontend
streamlit run app.py --server.port 8501 --server.address 0.0.0.0
EOF
    
    # Combined run script
    cat > scripts/run_system.sh << 'EOF'
#!/bin/bash
echo "Starting Complete Banking System..."

# Start backend in background
echo "Starting backend server..."
cd backend/build
./banking_system --port 8080 --data ../data &
BACKEND_PID=$!

# Wait for backend to start
sleep 3

# Start frontend
echo "Starting frontend application..."
cd ../../frontend
streamlit run app.py --server.port 8501 --server.address 0.0.0.0 &
FRONTEND_PID=$!

echo "========================================="
echo "Banking System is now running!"
echo "Backend API: http://localhost:8080"
echo "Frontend Web: http://localhost:8501"
echo "========================================="
echo "Press Ctrl+C to stop both services"

# Wait for interrupt
trap 'echo "Stopping services..."; kill $BACKEND_PID $FRONTEND_PID; exit' INT
wait
EOF
    
    # Make scripts executable
    chmod +x scripts/*.sh
    
    print_status "Run scripts created"
}

# Create Windows batch files
create_windows_scripts() {
    print_status "Creating Windows batch files..."
    
    # Backend run script
    cat > scripts/run_backend.bat << 'EOF'
@echo off
echo Starting Banking System Backend...
cd backend\build
banking_system.exe --port 8080 --data ..\data
pause
EOF
    
    # Frontend run script
    cat > scripts/run_frontend.bat << 'EOF'
@echo off
echo Starting Banking System Frontend...
cd frontend
streamlit run app.py --server.port 8501 --server.address 0.0.0.0
pause
EOF
    
    # Combined run script
    cat > scripts/run_system.bat << 'EOF'
@echo off
echo Starting Complete Banking System...

echo Starting backend server...
start /B cmd /c "cd backend\build && banking_system.exe --port 8080 --data ..\data"

timeout /t 3 /nobreak > nul

echo Starting frontend application...
cd frontend
streamlit run app.py --server.port 8501 --server.address 0.0.0.0

echo Banking System started!
echo Backend API: http://localhost:8080
echo Frontend Web: http://localhost:8501
pause
EOF
    
    print_status "Windows batch files created"
}

# Create documentation
create_documentation() {
    print_status "Creating documentation..."
    
    # README file
    cat > README.md << 'EOF'
# Banking System

A comprehensive banking system with C++ backend and Streamlit frontend.

## Features

- User authentication and management
- Multiple account types (Checking, Savings, Business)
- Deposit, withdrawal, and transfer operations
- Transaction history and reporting
- Real-time balance tracking
- Admin dashboard
- File-based data storage

## Quick Start

### Prerequisites

- C++ compiler (g++ or clang++)
- CMake 3.15+
- Python 3.7+
- pip

### Installation

1. Run the build script:
   ```bash
   ./build.sh
   ```

2. Start the system:
   ```bash
   ./scripts/run_system.sh
   ```

3. Access the web interface at: http://localhost:8501

### Default Accounts

- Admin: username `admin`, password `admin123`
- Customer: username `johndoe`, password `password123`

## Architecture

- **Backend**: C++ with file-based storage
- **Frontend**: Streamlit web application
- **API**: RESTful HTTP API
- **Storage**: CSV/JSON files

## Documentation

- [API Reference](docs/api/README.md)
- [User Guide](docs/user_guide/README.md)
- [Technical Documentation](docs/technical/README.md)

## Support

For issues and questions, please check the documentation or create an issue.
EOF
    
    print_status "Documentation created"
}

# Main build process
main() {
    print_status "Starting build process..."
    
    check_os
    check_dependencies
    create_directories
    build_backend
    setup_python_env
    create_sample_data
    create_run_scripts
    
    if [[ "$OS" == "windows" ]]; then
        create_windows_scripts
    fi
    
    create_documentation
    
    echo ""
    echo "========================================="
    echo -e "${GREEN}Build completed successfully!${NC}"
    echo "========================================="
    echo ""
    echo "To start the banking system:"
    echo "  Linux/macOS: ./scripts/run_system.sh"
    echo "  Windows:     scripts\\run_system.bat"
    echo ""
    echo "Web interface will be available at:"
    echo "  http://localhost:8501"
    echo ""
    echo "API endpoint will be available at:"
    echo "  http://localhost:8080"
    echo ""
}

# Run main function
main "$@"
