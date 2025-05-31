@echo off
REM Complete Banking System Startup Script
REM This script builds and runs both backend and frontend

echo =========================================
echo   Banking System Startup
echo =========================================

REM Set the project root directory
set PROJECT_ROOT=%~dp0
cd /d "%PROJECT_ROOT%"

echo [INFO] Project root: %PROJECT_ROOT%

REM Step 1: Build the C++ Backend
echo.
echo [STEP 1] Building C++ Backend...
echo =========================================

cd backend

REM Create build directory if it doesn't exist
if not exist "build" mkdir build
cd build

REM Configure with CMake
echo [INFO] Configuring with CMake...
cmake .. -G "Ninja"
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    pause
    exit /b 1
)

REM Build the project
echo [INFO] Building project...
ninja
if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    pause
    exit /b 1
)

echo [SUCCESS] Backend built successfully!

REM Go back to project root
cd /d "%PROJECT_ROOT%"

REM Step 1.5: Force Sample Data Recreation
echo.
echo [STEP 1.5] Preparing Sample Data...
echo =========================================

echo [INFO] Removing old sample data to force recreation...
if exist "backend\data\users\users.csv" (
    del "backend\data\users\users.csv"
    echo [INFO] Deleted old users.csv
)
if exist "backend\data\accounts\accounts.csv" (
    del "backend\data\accounts\accounts.csv"
    echo [INFO] Deleted old accounts.csv
)
if exist "backend\data\transactions\transactions.csv" (
    del "backend\data\transactions\transactions.csv"
    echo [INFO] Deleted old transactions.csv
)

echo [INFO] Sample data will be recreated with proper password hashes

REM Install frontend dependencies
echo.
echo [STEP 2] Setting up Python Environment...
echo =========================================

echo [INFO] Installing Python dependencies...
cd frontend
pip install -r requirements.txt
if %errorlevel% neq 0 (
    echo [ERROR] Failed to install Python dependencies
    pause
    exit /b 1
)

echo [SUCCESS] Python environment setup complete!

REM Go back to project root
cd /d "%PROJECT_ROOT%"

REM Step 3: Start the Backend Server
echo.
echo [STEP 3] Starting Backend Server...
echo =========================================

cd backend\build
echo [INFO] Starting backend server in background...
echo [INFO] Sample data will be created automatically on first run
start "" /B "banking_system.exe"


REM Wait a moment for server to start and create sample data
echo [INFO] Waiting for server initialization and sample data creation...
timeout /t 5 /nobreak >nul

REM Go back to project root
cd /d "%PROJECT_ROOT%"

REM Step 4: Start the Frontend
echo.
echo [STEP 4] Starting Frontend...
echo =========================================

cd frontend
echo [INFO] Starting Streamlit frontend...
echo.
echo =========================================
echo   Banking System Ready!
echo =========================================
echo.
echo Web Interface: http://localhost:8501
echo API Endpoint:  http://localhost:8080
echo.
echo Test Credentials:
echo   Admin:    username: admin,    password: password
echo   Customer: username: johndoe,  password: customer123
echo.
echo Press Ctrl+C to stop the system
echo =========================================

streamlit run app.py

REM Cleanup
echo.
echo [INFO] Shutting down...
taskkill /f /im banking_system.exe >nul 2>&1

echo [INFO] Banking system stopped.
pause
