# Complete Banking System Startup Script (PowerShell)
# This script builds and runs both backend and frontend

Write-Host "=========================================" -ForegroundColor Cyan
Write-Host "   Banking System Startup" -ForegroundColor Cyan
Write-Host "=========================================" -ForegroundColor Cyan

# Set the project root directory
$PROJECT_ROOT = Split-Path -Parent $MyInvocation.MyCommand.Path
Set-Location $PROJECT_ROOT

Write-Host "[INFO] Project root: $PROJECT_ROOT" -ForegroundColor Green

# Step 1: Build the C++ Backend
Write-Host ""
Write-Host "[STEP 1] Building C++ Backend..." -ForegroundColor Yellow
Write-Host "=========================================" -ForegroundColor Cyan

Set-Location "backend"

# Create build directory if it doesn't exist
if (!(Test-Path "build")) {
    New-Item -ItemType Directory -Name "build" | Out-Null
}
Set-Location "build"

# Configure with CMake
Write-Host "[INFO] Configuring with CMake..." -ForegroundColor Green
try {
    cmake .. -G "Ninja"
    if ($LASTEXITCODE -ne 0) { throw "CMake configuration failed" }
} catch {
    Write-Host "[ERROR] CMake configuration failed" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Build the project
Write-Host "[INFO] Building project..." -ForegroundColor Green
try {
    ninja
    if ($LASTEXITCODE -ne 0) { throw "Build failed" }
} catch {
    Write-Host "[ERROR] Build failed" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "[SUCCESS] Backend built successfully!" -ForegroundColor Green

# Go back to project root
Set-Location $PROJECT_ROOT

# Step 2: Setup Python Environment
Write-Host ""
Write-Host "[STEP 2] Setting up Python Environment..." -ForegroundColor Yellow
Write-Host "=========================================" -ForegroundColor Cyan

# Check if Python is available
try {
    python --version | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "Python not found" }
} catch {
    Write-Host "[ERROR] Python not found. Please install Python 3.8+" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

# Install frontend dependencies
Write-Host "[INFO] Installing Python dependencies..." -ForegroundColor Green
Set-Location "frontend"
try {
    pip install -r requirements.txt
    if ($LASTEXITCODE -ne 0) { throw "Failed to install dependencies" }
} catch {
    Write-Host "[ERROR] Failed to install Python dependencies" -ForegroundColor Red
    Read-Host "Press Enter to exit"
    exit 1
}

Write-Host "[SUCCESS] Python environment setup complete!" -ForegroundColor Green

# Go back to project root
Set-Location $PROJECT_ROOT

# Step 3: Start the Backend Server
Write-Host ""
Write-Host "[STEP 3] Starting Backend Server..." -ForegroundColor Yellow
Write-Host "=========================================" -ForegroundColor Cyan

Set-Location "backend\build"
Write-Host "[INFO] Starting backend server in background..." -ForegroundColor Green

# Start backend server in background
$backendProcess = Start-Process -FilePath ".\banking_system.exe" -WindowStyle Minimized -PassThru

# Wait a moment for server to start
Start-Sleep -Seconds 3

# Go back to project root
Set-Location $PROJECT_ROOT

# Step 4: Start the Frontend
Write-Host ""
Write-Host "[STEP 4] Starting Frontend..." -ForegroundColor Yellow
Write-Host "=========================================" -ForegroundColor Cyan

Set-Location "frontend"
Write-Host "[INFO] Starting Streamlit frontend..." -ForegroundColor Green
Write-Host "[INFO] The web interface will open at http://localhost:8501" -ForegroundColor Cyan
Write-Host "[INFO] Press Ctrl+C to stop the system" -ForegroundColor Cyan

try {
    streamlit run app.py
} finally {
    # Cleanup
    Write-Host ""
    Write-Host "[INFO] Shutting down..." -ForegroundColor Yellow
    if ($backendProcess -and !$backendProcess.HasExited) {
        Stop-Process -Id $backendProcess.Id -Force -ErrorAction SilentlyContinue
    }
    Write-Host "[INFO] Banking system stopped." -ForegroundColor Green
}
