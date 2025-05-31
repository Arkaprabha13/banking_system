@echo off
REM Banking System Build Script for Windows
REM Automates the complete setup and build process

echo =========================================
echo   Banking System Build Script
echo =========================================

REM Check for required tools
echo [INFO] Checking dependencies...

REM Check for C++ compiler
where g++ >nul 2>&1
if %errorlevel% == 0 (
    echo [INFO] g++ compiler found
) else (
    where cl >nul 2>&1
    if %errorlevel% == 0 (
        echo [INFO] MSVC compiler found
    ) else (
        echo [ERROR] No C++ compiler found. Please install Visual Studio or MinGW
        pause
        exit /b 1
    )
)

REM Check for CMake
where cmake >nul 2>&1
if %errorlevel% == 0 (
    echo [INFO] CMake found
) else (
    echo [ERROR] CMake not found. Please install CMake
    pause
    exit /b 1
)

REM Check for Python
where python >nul 2>&1
if %errorlevel% == 0 (
    echo [INFO] Python found
    set PYTHON_CMD=python
) else (
    where python3 >nul 2>&1
    if %errorlevel% == 0 (
        echo [INFO] Python3 found
        set PYTHON_CMD=python3
    ) else (
        echo [ERROR] Python not found. Please install Python 3.7+
        pause
        exit /b 1
    )
)

REM Check for pip
where pip >nul 2>&1
if %errorlevel% == 0 (
    set PIP_CMD=pip
) else (
    where pip3 >nul 2>&1
    if %errorlevel% == 0 (
        set PIP_CMD=pip3
    ) else (
        echo [ERROR] pip not found. Please install pip
        pause
        exit /b 1
    )
)

echo [INFO] All dependencies found

REM Create directory structure
echo [INFO] Creating directory structure...
mkdir backend\build >nul 2>&1
mkdir backend\data\users >nul 2>&1
mkdir backend\data\accounts >nul 2>&1
mkdir backend\data\transactions >nul 2>&1
mkdir backend\data\logs >nul 2>&1
mkdir frontend\components >nul 2>&1
mkdir frontend\pages >nul 2>&1
mkdir frontend\utils >nul 2>&1
mkdir frontend\assets\css >nul 2>&1
mkdir frontend\assets\js >nul 2>&1
mkdir frontend\assets\images >nul 2>&1
mkdir docs\api >nul 2>&1
mkdir docs\technical >nul 2>&1
mkdir docs\user_guide >nul 2>&1
mkdir tests\unit >nul 2>&1
mkdir tests\integration >nul 2>&1
mkdir scripts >nul 2>&1
mkdir logs >nul 2>&1
echo [INFO] Directory structure created

REM Build backend
echo [INFO] Building C++ backend...
cd backend\build

echo [INFO] Running CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    pause
    exit /b 1
)

echo [INFO] Building project...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Backend build failed
    pause
    exit /b 1
)

cd ..\..
echo [INFO] Backend build successful

REM Setup Python environment
echo [INFO] Setting up Python environment...
cd frontend

echo [INFO] Installing Python dependencies...
%PIP_CMD% install -r requirements.txt
if %errorlevel% neq 0 (
    echo [ERROR] Failed to install Python dependencies
    pause
    exit /b 1
)

cd ..
echo [INFO] Python dependencies installed successfully

REM Create sample data
echo [INFO] Creating sample data...
echo { > backend\data\config.json
echo   "database": { >> backend\data\config.json
echo     "data_directory": "data", >> backend\data\config.json
echo     "backup_interval": 3600, >> backend\data\config.json
echo     "max_backup_files": 10 >> backend\data\config.json
echo   }, >> backend\data\config.json
echo   "security": { >> backend\data\config.json
echo     "password_min_length": 6, >> backend\data\config.json
echo     "max_login_attempts": 5, >> backend\data\config.json
echo     "session_timeout": 1800 >> backend\data\config.json
echo   }, >> backend\data\config.json
echo   "limits": { >> backend\data\config.json
echo     "max_transaction_amount": 1000000, >> backend\data\config.json
echo     "daily_withdrawal_limit": 5000, >> backend\data\config.json
echo     "max_accounts_per_user": 5 >> backend\data\config.json
echo   } >> backend\data\config.json
echo } >> backend\data\config.json

REM Create run scripts
echo [INFO] Creating run scripts...

REM Backend run script
echo @echo off > scripts\run_backend.bat
echo echo Starting Banking System Backend... >> scripts\run_backend.bat
echo cd backend\build >> scripts\run_backend.bat
echo banking_system.exe --port 8080 --data ..\data >> scripts\run_backend.bat
echo pause >> scripts\run_backend.bat

REM Frontend run script
echo @echo off > scripts\run_frontend.bat
echo echo Starting Banking System Frontend... >> scripts\run_frontend.bat
echo cd frontend >> scripts\run_frontend.bat
echo streamlit run app.py --server.port 8501 --server.address 0.0.0.0 >> scripts\run_frontend.bat
echo pause >> scripts\run_frontend.bat

REM Combined run script
echo @echo off > scripts\run_system.bat
echo echo Starting Complete Banking System... >> scripts\run_system.bat
echo. >> scripts\run_system.bat
echo echo Starting backend server... >> scripts\run_system.bat
echo start /B cmd /c "cd backend\build && banking_system.exe --port 8080 --data ..\data" >> scripts\run_system.bat
echo. >> scripts\run_system.bat
echo timeout /t 3 /nobreak ^> nul >> scripts\run_system.bat
echo. >> scripts\run_system.bat
echo echo Starting frontend application... >> scripts\run_system.bat
echo cd frontend >> scripts\run_system.bat
echo streamlit run app.py --server.port 8501 --server.address 0.0.0.0 >> scripts\run_system.bat
echo. >> scripts\run_system.bat
echo echo Banking System started! >> scripts\run_system.bat
echo echo Backend API: http://localhost:8080 >> scripts\run_system.bat
echo echo Frontend Web: http://localhost:8501 >> scripts\run_system.bat
echo pause >> scripts\run_system.bat

echo [INFO] Run scripts created

REM Create documentation
echo [INFO] Creating documentation...
echo # Banking System > README.md
echo. >> README.md
echo A comprehensive banking system with C++ backend and Streamlit frontend. >> README.md
echo. >> README.md
echo ## Quick Start >> README.md
echo. >> README.md
echo 1. Run the build script: build.bat >> README.md
echo 2. Start the system: scripts\run_system.bat >> README.md
echo 3. Access the web interface at: http://localhost:8501 >> README.md

echo.
echo =========================================
echo Build completed successfully!
echo =========================================
echo.
echo To start the banking system:
echo   scripts\run_system.bat
echo.
echo Web interface will be available at:
echo   http://localhost:8501
echo.
echo API endpoint will be available at:
echo   http://localhost:8080
echo.
pause
