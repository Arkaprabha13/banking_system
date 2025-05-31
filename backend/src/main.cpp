#include <iostream>
#include <string>
#include <memory>
#include <signal.h>
#include <thread>
#include <chrono>
#include <filesystem>
#include <fstream>  // ADDED: For std::ifstream
#include "../include/services/BankingService.h"
#include "../include/services/ApiServer.h"

std::unique_ptr<ApiServer> server;

void signalHandler(int) {
    std::cout << "\nShutting down server..." << std::endl;
    if (server) {
        server->stop();
    }
    exit(0);
}

void printWelcome() {
    std::cout << "========================================" << std::endl;
    std::cout << "    Banking System Backend Server      " << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Author: Banking System Team" << std::endl;
    std::cout << "========================================" << std::endl;
}

void printUsage() {
    std::cout << "Usage: banking_system [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  --port <port>    Set server port (default: 8080)" << std::endl;
    std::cout << "  --data <path>    Set data directory (default: ../data)" << std::endl;
    std::cout << "  --help          Show this help message" << std::endl;
}

int main(int argc, char* argv[]) {
    printWelcome();
    
    // Parse command line arguments
    int port = 8080;
    std::string data_dir = "../data";  // FIXED: Use relative path from build directory
    
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--port" && i + 1 < argc) {
            port = std::stoi(argv[++i]);
        } else if (arg == "--data" && i + 1 < argc) {
            data_dir = argv[++i];
        }
    }
    
    // Set up signal handling
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    try {
        // DEBUG: Show current working directory and paths
        std::cout << "[DEBUG] Current working directory: " << std::filesystem::current_path() << std::endl;
        std::cout << "[DEBUG] Data directory (relative): " << data_dir << std::endl;
        
        // Convert to absolute path for clarity
        std::filesystem::path abs_data_path = std::filesystem::absolute(data_dir);
        std::cout << "[DEBUG] Data directory (absolute): " << abs_data_path << std::endl;
        
        // Check if data directory exists, create if not
        if (!std::filesystem::exists(abs_data_path)) {
            std::cout << "[DEBUG] Data directory doesn't exist, creating..." << std::endl;
            std::filesystem::create_directories(abs_data_path);
        }
        
        // Initialize banking service
        std::cout << "Initializing banking service..." << std::endl;
        auto banking_service = std::make_shared<BankingService>(data_dir);
        
        if (!banking_service->initialize()) {
            std::cerr << "Failed to initialize banking service!" << std::endl;
            return 1;
        }
        
        std::cout << "Banking service initialized successfully." << std::endl;
        std::cout << "Data directory: " << abs_data_path << std::endl;
        
        // FORCE SAMPLE DATA CREATION FOR DEBUGGING
        std::cout << "[DEBUG] Checking if sample data needs to be created..." << std::endl;
        
        // Check if users.csv exists
        std::string users_file = data_dir + "/users/users.csv";
        std::filesystem::path users_path = std::filesystem::absolute(users_file);
        std::cout << "[DEBUG] Looking for users file at: " << users_path << std::endl;
        
        if (!std::filesystem::exists(users_path)) {
            std::cout << "[DEBUG] Users file not found, forcing sample data creation..." << std::endl;
            // FIXED: Remove getDatabase() call since it's private
            // Instead, we'll let the initialize() method handle sample data creation
            // Or add a public method to BankingService for this
            std::cout << "[DEBUG] Sample data will be created during initialization" << std::endl;
        } else {
            std::cout << "[DEBUG] Users file exists at: " << users_path << std::endl;
            
            // Check if file has actual data (more than just header)
            std::ifstream file(users_path);
            std::string line1, line2;
            std::getline(file, line1); // header
            if (!std::getline(file, line2)) {
                std::cout << "[DEBUG] Users file is empty" << std::endl;
            } else {
                std::cout << "[DEBUG] Users file has data, first line: " << line2 << std::endl;
            }
            file.close();
        }
        
        // Create and start API server
        std::cout << "Starting API server on port " << port << "..." << std::endl;
        server = std::make_unique<ApiServer>(port);
        server->setBankingService(banking_service);
        
        if (!server->start()) {
            std::cerr << "Failed to start API server!" << std::endl;
            return 1;
        }
        
        std::cout << "========================================" << std::endl;
        std::cout << "Server is running on port " << port << std::endl;
        std::cout << "API endpoint: http://localhost:" << port << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        std::cout << "========================================" << std::endl;
        
        // Keep the main thread alive
        while (server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    std::cout << "Server stopped successfully." << std::endl;
    return 0;
}
