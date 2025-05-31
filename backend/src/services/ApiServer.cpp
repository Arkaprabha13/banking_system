#include "../../include/services/ApiServer.h"
#include "../../include/services/BankingService.h"
#include "../../include/models/Account.h"
#include "../../include/models/Transaction.h"
#include "../../include/models/User.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <cctype>
#include <iomanip>  // For std::fixed and std::setprecision

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

ApiServer::ApiServer(int port) : port(port), running(false) {
    setupRoutes();
    
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Failed to initialize Winsock");
    }
#endif
}

ApiServer::~ApiServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

void ApiServer::setBankingService(std::shared_ptr<BankingService> service) {
    banking_service = service;
}

void ApiServer::setupRoutes() {
    routes["/api"] = [this](const HttpRequest& req) {
        HttpResponse response(200);
        response.body = "{\"message\":\"Welcome to the Banking API\"}";
        return response;
    };
    std::cout << "[DEBUG] Route registered: /api" << std::endl;

    routes["/api/login"] = [this](const HttpRequest& req) { return handleLogin(req); };
    std::cout << "[DEBUG] Route registered: /api/login" << std::endl;

    routes["/api/register"] = [this](const HttpRequest& req) { return handleRegister(req); };
    std::cout << "[DEBUG] Route registered: /api/register" << std::endl;

    routes["/api/accounts"] = [this](const HttpRequest& req) { return handleGetAccounts(req); };
    std::cout << "[DEBUG] Route registered: /api/accounts" << std::endl;

    routes["/api/accounts/create"] = [this](const HttpRequest& req) { return handleCreateAccount(req); };
    std::cout << "[DEBUG] Route registered: /api/accounts/create" << std::endl;

    routes["/api/transactions/deposit"] = [this](const HttpRequest& req) { return handleDeposit(req); };
    std::cout << "[DEBUG] Route registered: /api/transactions/deposit" << std::endl;

    routes["/api/transactions/withdraw"] = [this](const HttpRequest& req) { return handleWithdraw(req); };
    std::cout << "[DEBUG] Route registered: /api/transactions/withdraw" << std::endl;

    routes["/api/transactions/transfer"] = [this](const HttpRequest& req) { return handleTransfer(req); };
    std::cout << "[DEBUG] Route registered: /api/transactions/transfer" << std::endl;

    routes["/api/transactions"] = [this](const HttpRequest& req) { return handleGetTransactions(req); };
    std::cout << "[DEBUG] Route registered: /api/transactions" << std::endl;

    routes["/api/balance"] = [this](const HttpRequest& req) { return handleGetBalance(req); };
    std::cout << "[DEBUG] Route registered: /api/balance" << std::endl;

    std::cout << "[DEBUG] Total routes registered: " << routes.size() << std::endl;
}

bool ApiServer::start() {
    if (running) {
        return false;
    }
    
    if (!banking_service) {
        std::cerr << "Banking service not set! Call setBankingService() first." << std::endl;
        return false;
    }
    
    server_thread = std::thread(&ApiServer::serverLoop, this);
    running = true;
    return true;
}

void ApiServer::stop() {
    if (running) {
        running = false;
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }
}

bool ApiServer::isRunning() const {
    return running;
}

void ApiServer::serverLoop() {
    int server_socket;
    struct sockaddr_in server_addr;
    
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);
    
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Error binding socket" << std::endl;
#ifdef _WIN32
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        return;
    }
    
    if (listen(server_socket, 10) < 0) {
        std::cerr << "Error listening on socket" << std::endl;
#ifdef _WIN32
        closesocket(server_socket);
#else
        close(server_socket);
#endif
        return;
    }
    
    std::cout << "Server started on port " << port << std::endl;
    
    while (running) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket >= 0) {
            std::thread client_thread(&ApiServer::handleClient, this, client_socket);
            client_thread.detach();
        }
    }
    
#ifdef _WIN32
    closesocket(server_socket);
#else
    close(server_socket);
#endif
}

// void ApiServer::handleClient(int client_socket) {
//     char buffer[4096];
//     memset(buffer, 0, sizeof(buffer));
    
//     int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
//     if (bytes_received <= 0) {
// #ifdef _WIN32
//         closesocket(client_socket);
// #else
//         close(client_socket);
// #endif
//         return;
//     }
    
//     std::string raw_request(buffer, bytes_received);
    
//     // Debug logging
//     std::cout << "[DEBUG] Raw request received:" << std::endl;
//     std::cout << raw_request << std::endl;
//     std::cout << "[DEBUG] Request length: " << raw_request.length() << std::endl;
    
//     HttpRequest request = parseRequest(raw_request);
    
//     std::cout << "[DEBUG] Parsed request:" << std::endl;
//     std::cout << "[DEBUG] Method: '" << request.method << "'" << std::endl;
//     std::cout << "[DEBUG] Path: '" << request.path << "'" << std::endl;
//     std::cout << "[DEBUG] Body: '" << request.body << "'" << std::endl;
    
//     HttpResponse response;
    
//     if (request.method == "OPTIONS") {
//         response = handleOptions(request);
//     } else {
//         std::cout << "[DEBUG] Looking for route: '" << request.path << "'" << std::endl;
//         std::cout << "[DEBUG] Available routes:" << std::endl;
//         for (const auto& route : routes) {
//             std::cout << "[DEBUG]   - '" << route.first << "'" << std::endl;
//         }
        
//         auto it = routes.find(request.path);
//         if (it != routes.end()) {
//             try {
//                 response = it->second(request);
//             } catch (const std::exception& e) {
//                 response.status_code = 500;
//                 response.body = "{\"error\":\"Internal server error: " + std::string(e.what()) + "\"}";
//             }
//         } else {
//             response.status_code = 404;
//             response.body = "{\"error\":\"Endpoint not found\"}";
//         }
//     }
    
//     std::string response_str = serializeResponse(response);
//     send(client_socket, response_str.c_str(), response_str.length(), 0);
    
// #ifdef _WIN32
//     closesocket(client_socket);
// #else
//     close(client_socket);
// #endif
// }
void ApiServer::handleClient(int client_socket) {
    std::string raw_request;
    char buffer[4096];
    int content_length = 0;
    bool headers_complete = false;
    size_t header_end_pos = 0;
    
    // Step 1: Read until we have complete headers
    while (!headers_complete) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        
        if (bytes_received <= 0) {
            std::cout << "[DEBUG] Connection closed or error" << std::endl;
#ifdef _WIN32
            closesocket(client_socket);
#else
            close(client_socket);
#endif
            return;
        }
        
        raw_request.append(buffer, bytes_received);
        std::cout << "[DEBUG] Received " << bytes_received << " bytes, total: " << raw_request.length() << std::endl;
        
        // Look for end of headers
        header_end_pos = raw_request.find("\r\n\r\n");
        if (header_end_pos == std::string::npos) {
            header_end_pos = raw_request.find("\n\n");
            if (header_end_pos != std::string::npos) {
                header_end_pos += 2; // Skip \n\n
                headers_complete = true;
            }
        } else {
            header_end_pos += 4; // Skip \r\n\r\n
            headers_complete = true;
        }
        
        // Extract Content-Length if headers are complete
        if (headers_complete) {
            size_t content_length_pos = raw_request.find("Content-Length:");
            if (content_length_pos != std::string::npos && content_length_pos < header_end_pos) {
                size_t start = raw_request.find(":", content_length_pos) + 1;
                size_t end = raw_request.find("\r\n", start);
                if (end == std::string::npos) {
                    end = raw_request.find("\n", start);
                }
                
                if (end != std::string::npos && end < raw_request.length()) {
                    std::string length_str = raw_request.substr(start, end - start);
                    // Trim whitespace
                    length_str.erase(0, length_str.find_first_not_of(" \t"));
                    length_str.erase(length_str.find_last_not_of(" \t\r\n") + 1);
                    
                    try {
                        content_length = std::stoi(length_str);
                        std::cout << "[DEBUG] Found Content-Length: " << content_length << std::endl;
                    } catch (const std::exception& e) {
                        std::cout << "[DEBUG] Failed to parse Content-Length: " << e.what() << std::endl;
                        content_length = 0;
                    }
                }
            }
        }
    }
    
    // Step 2: Read body if Content-Length > 0
    if (content_length > 0) {
        int body_bytes_received = raw_request.length() - header_end_pos;
        std::cout << "[DEBUG] Body bytes already received: " << body_bytes_received << std::endl;
        std::cout << "[DEBUG] Body bytes needed: " << content_length << std::endl;
        
        // Read remaining body bytes
        while (body_bytes_received < content_length) {
            memset(buffer, 0, sizeof(buffer));
            int bytes_needed = content_length - body_bytes_received;
            int bytes_to_read = std::min(bytes_needed, (int)sizeof(buffer) - 1);
            
            int bytes_received = recv(client_socket, buffer, bytes_to_read, 0);
            if (bytes_received <= 0) {
                std::cout << "[DEBUG] Failed to receive body data" << std::endl;
                break;
            }
            
            raw_request.append(buffer, bytes_received);
            body_bytes_received += bytes_received;
            std::cout << "[DEBUG] Received additional " << bytes_received << " body bytes, total body: " << body_bytes_received << std::endl;
        }
    }
    
    std::cout << "[DEBUG] Final request size: " << raw_request.length() << std::endl;
    std::cout << "[DEBUG] Raw request received:" << std::endl;
    std::cout << raw_request << std::endl;
    
    HttpRequest request = parseRequest(raw_request);
    
    std::cout << "[DEBUG] Parsed request:" << std::endl;
    std::cout << "[DEBUG] Method: '" << request.method << "'" << std::endl;
    std::cout << "[DEBUG] Path: '" << request.path << "'" << std::endl;
    std::cout << "[DEBUG] Body: '" << request.body << "'" << std::endl;
    
    HttpResponse response;
    
    if (request.method == "OPTIONS") {
        response = handleOptions(request);
    } else {
        std::cout << "[DEBUG] Looking for route: '" << request.path << "'" << std::endl;
        
        auto it = routes.find(request.path);
        if (it != routes.end()) {
            try {
                response = it->second(request);
            } catch (const std::exception& e) {
                response.status_code = 500;
                response.body = "{\"error\":\"Internal server error: " + std::string(e.what()) + "\"}";
            }
        } else {
            response.status_code = 404;
            response.body = "{\"error\":\"Endpoint not found\"}";
        }
    }
    
    std::string response_str = serializeResponse(response);
    send(client_socket, response_str.c_str(), response_str.length(), 0);
    
#ifdef _WIN32
    closesocket(client_socket);
#else
    close(client_socket);
#endif
}

// HttpRequest ApiServer::parseRequest(const std::string& raw_request) {
//     HttpRequest request;
//     std::istringstream stream(raw_request);
//     std::string line;
    
//     // Parse request line with proper handling
//     if (std::getline(stream, line)) {
//         // Remove carriage return if present
//         if (!line.empty() && line.back() == '\r') {
//             line.pop_back();
//         }
        
//         std::istringstream request_line(line);
//         std::string path_with_query, http_version;
//         request_line >> request.method >> path_with_query >> http_version;
        
//         std::cout << "[DEBUG] Request line: '" << line << "'" << std::endl;
//         std::cout << "[DEBUG] Parsed method: '" << request.method << "'" << std::endl;
//         std::cout << "[DEBUG] Parsed path: '" << path_with_query << "'" << std::endl;
        
//         // Split path and query
//         size_t query_pos = path_with_query.find('?');
//         if (query_pos != std::string::npos) {
//             request.path = path_with_query.substr(0, query_pos);
//             request.query_params = parseQuery(path_with_query.substr(query_pos + 1));
//         } else {
//             request.path = path_with_query;
//         }
//     }
    
//     // Parse headers with proper carriage return handling
//     while (std::getline(stream, line) && line != "\r" && !line.empty()) {
//         // Remove carriage return if present
//         if (!line.empty() && line.back() == '\r') {
//             line.pop_back();
//         }
        
//         size_t colon_pos = line.find(':');
//         if (colon_pos != std::string::npos) {
//             std::string key = line.substr(0, colon_pos);
//             std::string value = line.substr(colon_pos + 1);
            
//             // Trim whitespace
//             key.erase(0, key.find_first_not_of(" \t"));
//             key.erase(key.find_last_not_of(" \t\r") + 1);
//             value.erase(0, value.find_first_not_of(" \t"));
//             value.erase(value.find_last_not_of(" \t\r") + 1);
            
//             request.headers[key] = value;
//         }
//     }
    
//     // Parse body
//     std::string body;
//     while (std::getline(stream, line)) {
//         if (!body.empty()) {
//             body += "\n";
//         }
//         body += line;
//     }
    
//     // Remove trailing carriage returns from body
//     while (!body.empty() && (body.back() == '\r' || body.back() == '\n')) {
//         body.pop_back();
//     }
    
//     request.body = body;
    
//     return request;
// }
HttpRequest ApiServer::parseRequest(const std::string& raw_request) {
    HttpRequest request;
    
    // Find the double CRLF that separates headers from body
    size_t header_end = raw_request.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        header_end = raw_request.find("\n\n");
        if (header_end != std::string::npos) {
            header_end += 2;
        }
    } else {
        header_end += 4;
    }
    
    std::string headers_part;
    std::string body_part;
    
    if (header_end != std::string::npos) {
        headers_part = raw_request.substr(0, header_end - (raw_request.find("\r\n\r\n") != std::string::npos ? 4 : 2));
        body_part = raw_request.substr(header_end);
    } else {
        headers_part = raw_request;
    }
    
    // Parse headers
    std::istringstream stream(headers_part);
    std::string line;
    
    // Parse request line
    if (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        std::istringstream request_line(line);
        std::string path_with_query, http_version;
        request_line >> request.method >> path_with_query >> http_version;
        
        // FIXED: Properly parse query parameters
        size_t query_pos = path_with_query.find('?');
        if (query_pos != std::string::npos) {
            request.path = path_with_query.substr(0, query_pos);
            std::string query_string = path_with_query.substr(query_pos + 1);
            request.query_params = parseQuery(query_string);
            
            std::cout << "[DEBUG] Path: '" << request.path << "'" << std::endl;
            std::cout << "[DEBUG] Query string: '" << query_string << "'" << std::endl;
            std::cout << "[DEBUG] Query params count: " << request.query_params.size() << std::endl;
        } else {
            request.path = path_with_query;
        }
    }
    
    // Parse headers (rest of the function remains the same)
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        
        size_t colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 1);
            
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t\r") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t\r") + 1);
            
            request.headers[key] = value;
        }
    }
    
    request.body = body_part;
    return request;
}


// std::string ApiServer::serializeResponse(const HttpResponse& response) {
//     std::ostringstream stream;
    
//     // Status line
//     stream << "HTTP/1.1 " << response.status_code << " ";
//     switch (response.status_code) {
//         case 200: stream << "OK"; break;
//         case 204: stream << "No Content"; break;
//         case 400: stream << "Bad Request"; break;
//         case 401: stream << "Unauthorized"; break;
//         case 404: stream << "Not Found"; break;
//         case 405: stream << "Method Not Allowed"; break;
//         case 500: stream << "Internal Server Error"; break;
//         default: stream << "Unknown"; break;
//     }
//     stream << "\r\n";
    
//     // CORS headers
//     stream << "Access-Control-Allow-Origin: *\r\n";
//     stream << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
//     stream << "Access-Control-Allow-Headers: Content-Type, Authorization\r\n";
//     stream << "Content-Type: application/json\r\n";
    
//     // Headers
//     for (const auto& header : response.headers) {
//         stream << header.first << ": " << header.second << "\r\n";
//     }
//     stream << "Content-Length: " << response.body.length() << "\r\n";
//     stream << "\r\n";
    
//     // Body
//     stream << response.body;
    
//     return stream.str();
// }
std::string ApiServer::serializeResponse(const HttpResponse& response) {
    std::ostringstream stream;
    
    // Status line
    stream << "HTTP/1.1 " << response.status_code << " ";
    switch (response.status_code) {
        case 200: stream << "OK"; break;
        case 204: stream << "No Content"; break;
        case 400: stream << "Bad Request"; break;
        case 401: stream << "Unauthorized"; break;
        case 404: stream << "Not Found"; break;
        case 405: stream << "Method Not Allowed"; break;
        case 500: stream << "Internal Server Error"; break;
        default: stream << "Unknown"; break;
    }
    stream << "\r\n";
    
    // FIXED: Enhanced CORS headers for localhost:8502
    stream << "Access-Control-Allow-Origin: *\r\n";

    stream << "Access-Control-Allow-Methods: GET, POST, PUT, DELETE, OPTIONS\r\n";
    stream << "Access-Control-Allow-Headers: Content-Type, Authorization, Accept, Origin, X-Requested-With\r\n";
    stream << "Access-Control-Allow-Credentials: true\r\n";
    stream << "Access-Control-Max-Age: 86400\r\n";
    stream << "Content-Type: application/json\r\n";
    
    // IMPORTANT: Only add custom headers that are NOT CORS headers
    for (const auto& header : response.headers) {
        // Skip CORS headers to avoid duplicates
        if (header.first != "Access-Control-Allow-Origin" &&
            header.first != "Access-Control-Allow-Methods" &&
            header.first != "Access-Control-Allow-Headers" &&
            header.first != "Access-Control-Max-Age" &&
            header.first != "Content-Type") {
            stream << header.first << ": " << header.second << "\r\n";
        }
    }
    stream << "Content-Length: " << response.body.length() << "\r\n";
    stream << "\r\n";
    
    // Body
    stream << response.body;
    
    return stream.str();
}

std::map<std::string, std::string> ApiServer::parseQuery(const std::string& query) {
    std::map<std::string, std::string> params;
    std::istringstream stream(query);
    std::string pair;
    
    while (std::getline(stream, pair, '&')) {
        size_t equals_pos = pair.find('=');
        if (equals_pos != std::string::npos) {
            std::string key = urlDecode(pair.substr(0, equals_pos));
            std::string value = urlDecode(pair.substr(equals_pos + 1));
            params[key] = value;
        }
    }
    
    return params;
}

std::string ApiServer::urlDecode(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int hex_value;
            std::istringstream hex_stream(str.substr(i + 1, 2));
            if (hex_stream >> std::hex >> hex_value) {
                result += static_cast<char>(hex_value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    
    return result;
}

// Improved JSON parsing function
std::string ApiServer::extractJsonField(const std::string& json, const std::string& field) {
    // Remove all whitespace for easier parsing
    std::string clean_json = json;
    clean_json.erase(std::remove_if(clean_json.begin(), clean_json.end(), 
                                   [](char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }), 
                     clean_json.end());
    
    std::cout << "[DEBUG] Clean JSON: " << clean_json << std::endl;
    
    // Look for the field pattern: "field":
    std::string pattern = "\"" + field + "\":";
    size_t pos = clean_json.find(pattern);
    
    if (pos == std::string::npos) {
        std::cout << "[DEBUG] Field '" << field << "' not found" << std::endl;
        return "";
    }
    
    // Move to the start of the value
    size_t value_start = pos + pattern.length();
    
    if (value_start >= clean_json.length()) {
        return "";
    }
    
    // Check if the value is a string (starts with quote)
    if (clean_json[value_start] == '"') {
        // Find the closing quote
        size_t quote_start = value_start + 1;
        size_t quote_end = clean_json.find('"', quote_start);
        
        if (quote_end != std::string::npos) {
            std::string result = clean_json.substr(quote_start, quote_end - quote_start);
            std::cout << "[DEBUG] Extracted '" << field << "': '" << result << "'" << std::endl;
            return result;
        }
    } else {
        // Handle non-string values
        size_t value_end = value_start;
        while (value_end < clean_json.length() && 
               clean_json[value_end] != ',' && 
               clean_json[value_end] != '}' && 
               clean_json[value_end] != ']') {
            value_end++;
        }
        
        std::string result = clean_json.substr(value_start, value_end - value_start);
        std::cout << "[DEBUG] Extracted '" << field << "': '" << result << "'" << std::endl;
        return result;
    }
    
    return "";
}
// HttpResponse ApiServer::handleLogin(const HttpRequest& request) {
//     HttpResponse response;
//     std::cout << "[DEBUG] === LOGIN HANDLER ===" << std::endl;
//     std::cout << "[DEBUG] Method: " << request.method << std::endl;
//     std::cout << "[DEBUG] Body: '" << request.body << "'" << std::endl;
    
//     if (request.method != "POST") {
//         response.status_code = 405;
//         response.body = "{\"error\":\"Method not allowed\"}";
//         return response;
//     }
    
//     try {
//         std::string username = extractJsonField(request.body, "username");
//         std::string password = extractJsonField(request.body, "password");
        
//         std::cout << "[DEBUG] Extracted username: '" << username << "'" << std::endl;
//         std::cout << "[DEBUG] Extracted password: '" << password << "'" << std::endl;
        
//         if (username.empty() || password.empty()) {
//             response.status_code = 400;
//             response.body = "{\"success\":false,\"message\":\"Username and password are required\"}";
//             return response;
//         }
        
//         // Simple test authentication - REPLACE BANKING SERVICE CALL
//         if (username == "admin" && password == "password") {
//             response.body = "{\"success\":true,\"message\":\"Login successful\",\"username\":\"" + username + "\",\"user_id\":\"admin_001\"}";
//         } else {
//             response.status_code = 401;
//             response.body = "{\"success\":false,\"message\":\"Invalid credentials\"}";
//         }
        
//     } catch (const std::exception& e) {
//         response.status_code = 400;
//         response.body = "{\"error\":\"Invalid request format\"}";
//     }
    
//     std::cout << "[DEBUG] Login response: " << response.body << std::endl;
//     return response;
// }
HttpResponse ApiServer::handleLogin(const HttpRequest& request) {
    HttpResponse response;
    std::cout << "[DEBUG] === LOGIN HANDLER ===" << std::endl;
    std::cout << "[DEBUG] Method: " << request.method << std::endl;
    std::cout << "[DEBUG] Body: '" << request.body << "'" << std::endl;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    try {
        std::string username = extractJsonField(request.body, "username");
        std::string password = extractJsonField(request.body, "password");
        
        std::cout << "[DEBUG] Extracted username: '" << username << "'" << std::endl;
        std::cout << "[DEBUG] Extracted password: '" << password << "'" << std::endl;
        
        if (username.empty() || password.empty()) {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"Username and password are required\"}";
            return response;
        }
        
        // Use the actual banking service authentication
        if (banking_service) {
            std::cout << "[DEBUG] Using banking service for authentication" << std::endl;
            AuthResult auth_result = banking_service->authenticateUser(username, password);
            if (auth_result.success) {
                response.body = "{\"success\":true,\"message\":\"Login successful\",\"username\":\"" + username + "\",\"user_id\":\"" + auth_result.user_id + "\"}";
            } else {
                response.status_code = 401;
                response.body = "{\"success\":false,\"message\":\"" + auth_result.message + "\"}";
            }
        } else {
            // Fallback: Check for test credentials AND registered users
            std::cout << "[DEBUG] Banking service not available, using fallback authentication" << std::endl;
            
            // Allow admin login
            if (username == "admin" && password == "password") {
                response.body = "{\"success\":true,\"message\":\"Login successful\",\"username\":\"" + username + "\",\"user_id\":\"admin_001\"}";
            }
            // Allow registered user login (you can expand this to read from CSV)
            else if (username == "aaa" && password == "Test@123") {
                response.body = "{\"success\":true,\"message\":\"Login successful\",\"username\":\"" + username + "\",\"user_id\":\"USR224216\"}";
            }
            else {
                response.status_code = 401;
                response.body = "{\"success\":false,\"message\":\"Invalid credentials\"}";
            }
        }
        
    } catch (const std::exception& e) {
        response.status_code = 400;
        response.body = "{\"error\":\"Invalid request format\"}";
    }
    
    std::cout << "[DEBUG] Login response: " << response.body << std::endl;
    return response;
}


HttpResponse ApiServer::handleRegister(const HttpRequest& request) {
    HttpResponse response;
    std::cout << "[DEBUG] Received register request for path: " << request.path << std::endl;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    try {
        std::cout << "Registration request body: " << request.body << std::endl;
        
        std::string username = extractJsonField(request.body, "username");
        std::string password = extractJsonField(request.body, "password");
        std::string email = extractJsonField(request.body, "email");
        std::string full_name = extractJsonField(request.body, "full_name");
        
        std::cout << "Extracted fields - username: '" << username << "', password: '" << password << "', email: '" << email << "', full_name: '" << full_name << "'" << std::endl;
        
        if (username.empty() || password.empty()) {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"Username and password are required\"}";
            return response;
        }
        
        AuthResult register_result = banking_service->registerUser(username, password, email, full_name);
        response.body = "{\"success\":true,\"message\":\"User registered successfully\"}";
        if (register_result.success) {
            response.body = "{\"success\":true,\"message\":\"" + register_result.message + "\"}";
        } else {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"" + register_result.message + "\"}";
        }
    } catch (const std::exception& e) {
        response.status_code = 400;
        response.body = "{\"error\":\"Invalid request format\"}";
    }
    
    return response;
}

// HttpResponse ApiServer::handleGetAccounts(const HttpRequest& request) {
//     HttpResponse response;
    
//     if (request.method != "GET") {
//         response.status_code = 405;
//         response.body = "{\"error\":\"Method not allowed\"}";
//         return response;
//     }
    
//     auto it = request.query_params.find("username");
//     if (it == request.query_params.end()) {
//         response.status_code = 400;
//         response.body = "{\"error\":\"Username parameter required\"}";
//         return response;
//     }
    
//     try {
//         auto accounts = banking_service->getUserAccounts(it->second);
//         std::ostringstream json;
//         json << "{\"accounts\":[";
        
//         for (size_t i = 0; i < accounts.size(); ++i) {
//             if (i > 0) json << ",";
//             json << "{";
//             json << "\"account_number\":\"" << accounts[i].getAccountNumber() << "\",";
//             json << "\"account_type\":\"" << (accounts[i].getAccountType() == AccountType::CHECKING ? "CHECKING" : "SAVINGS") << "\",";
//             json << "\"balance\":" << accounts[i].getBalance();
//             json << "}";
//         }
        
//         json << "]}";
//         response.body = json.str();
//     } catch (const std::exception& e) {
//         response.status_code = 500;
//         response.body = "{\"error\":\"Failed to retrieve accounts\"}";
//     }
    
//     return response;
// }
HttpResponse ApiServer::handleGetAccounts(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "GET") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    // Debug query parameters
    std::cout << "[DEBUG] Query params in handleGetAccounts:" << std::endl;
    for (const auto& param : request.query_params) {
        std::cout << "[DEBUG] " << param.first << " = " << param.second << std::endl;
    }
    
    auto it = request.query_params.find("username");
    if (it == request.query_params.end()) {
        response.status_code = 400;
        response.body = "{\"error\":\"Username parameter required\"}";
        return response;
    }
    
    try {
        // Get user by username first
        User user;
        if (!banking_service->getUserByUsername(it->second, user)) {
            response.status_code = 400;
            response.body = "{\"error\":\"User not found\"}";
            return response;
        }
        
        // Get accounts by user_id, not username
        auto accounts = banking_service->getUserAccounts(user.getUserId());
        std::ostringstream json;
        json << "{\"accounts\":[";
        
        for (size_t i = 0; i < accounts.size(); ++i) {
            if (i > 0) json << ",";
            json << "{";
            json << "\"account_number\":\"" << accounts[i].getAccountNumber() << "\",";
            json << "\"account_type\":\"" << (accounts[i].getAccountType() == AccountType::CHECKING ? "CHECKING" : "SAVINGS") << "\",";
            json << "\"balance\":" << std::fixed << std::setprecision(2) << accounts[i].getBalance();
            json << "}";
        }
        
        json << "]}";
        response.body = json.str();
    } catch (const std::exception& e) {
        response.status_code = 500;
        response.body = "{\"error\":\"Failed to retrieve accounts: " + std::string(e.what()) + "\"}";
    }
    
    return response;
}


// HttpResponse ApiServer::handleCreateAccount(const HttpRequest& request) {
//     HttpResponse response;
    
//     if (request.method != "POST") {
//         response.status_code = 405;
//         response.body = "{\"error\":\"Method not allowed\"}";
//         return response;
//     }
    
//     try {
//         std::string username = extractJsonField(request.body, "username");
//         std::string account_type_str = extractJsonField(request.body, "account_type");
        
//         // Get initial deposit if provided
//         double initial_deposit = 0.0;
//         try {
//             std::string initial_deposit_str = extractJsonField(request.body, "initial_balance");
//             if (!initial_deposit_str.empty()) {
//                 initial_deposit = std::stod(initial_deposit_str);
//             }
//         } catch (...) {
//             // If initial_balance not provided or invalid, default to 0
//         }
        
//         // First get the user_id from username
//         User user;
//         if (!banking_service->getUserByUsername(username, user)) {
//             response.status_code = 400;
//             response.body = "{\"success\":false,\"message\":\"User not found\"}";
//             return response;
//         }
        
//         AccountCreationResult create_result = banking_service->createAccount(user.getUserId(), account_type_str, initial_deposit);
//         if (create_result.success) {
//             response.body = "{\"success\":true,\"message\":\"" + create_result.message + "\",\"account_number\":\"" + create_result.account_number + "\"}";
//         } else {
//             response.status_code = 400;
//             response.body = "{\"success\":false,\"message\":\"" + create_result.message + "\"}";
//         }
//     } catch (const std::exception& e) {
//         response.status_code = 400;
//         response.body = "{\"error\":\"Invalid request format\"}";
//     }
    
//     return response;
// }
HttpResponse ApiServer::handleCreateAccount(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    try {
        std::string username = extractJsonField(request.body, "username");
        std::string account_type_str = extractJsonField(request.body, "account_type");
        std::string initial_balance_str = extractJsonField(request.body, "initial_balance");
        
        if (username.empty() || account_type_str.empty()) {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"Username and account_type are required\"}";
            return response;
        }
        
        // Parse initial balance
        double initial_balance = 0.0;
        if (!initial_balance_str.empty()) {
            try {
                initial_balance = std::stod(initial_balance_str);
                if (initial_balance < 0) {
                    response.status_code = 400;
                    response.body = "{\"success\":false,\"message\":\"Initial balance cannot be negative\"}";
                    return response;
                }
            } catch (const std::exception& e) {
                initial_balance = 0.0;
            }
        }
        
        // Get user by username
        User user;
        if (!banking_service->getUserByUsername(username, user)) {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"User not found\"}";
            return response;
        }
        
        // Create account
        AccountCreationResult create_result = banking_service->createAccount(
            user.getUserId(), 
            account_type_str, 
            initial_balance
        );
        
        if (create_result.success) {
            response.body = "{\"success\":true,\"message\":\"" + create_result.message + 
                          "\",\"account_number\":\"" + create_result.account_number + "\"}";
        } else {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"" + create_result.message + "\"}";
        }
        
    } catch (const std::exception& e) {
        response.status_code = 400;
        response.body = "{\"error\":\"Invalid request format: " + std::string(e.what()) + "\"}";
    }
    
    return response;
}

HttpResponse ApiServer::handleDeposit(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    try {
        std::string account_number = extractJsonField(request.body, "account_number");
        std::string amount_str = extractJsonField(request.body, "amount");
        double amount = std::stod(amount_str);
        
        TransactionResult deposit_result = banking_service->deposit(account_number, amount);
        if (deposit_result.success) {
            response.body = "{\"success\":true,\"message\":\"" + deposit_result.message + "\",\"new_balance\":" + std::to_string(deposit_result.new_balance) + "}";
        } else {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"" + deposit_result.message + "\"}";
        }
    } catch (const std::exception& e) {
        response.status_code = 400;
        response.body = "{\"error\":\"Invalid request format\"}";
    }
    
    return response;
}

HttpResponse ApiServer::handleWithdraw(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    try {
        std::string account_number = extractJsonField(request.body, "account_number");
        std::string amount_str = extractJsonField(request.body, "amount");
        double amount = std::stod(amount_str);
        
        TransactionResult withdraw_result = banking_service->withdraw(account_number, amount);
        if (withdraw_result.success) {
            response.body = "{\"success\":true,\"message\":\"" + withdraw_result.message + "\",\"new_balance\":" + std::to_string(withdraw_result.new_balance) + "}";
        } else {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"" + withdraw_result.message + "\"}";
        }
    } catch (const std::exception& e) {
        response.status_code = 400;
        response.body = "{\"error\":\"Invalid request format\"}";
    }
    
    return response;
}

HttpResponse ApiServer::handleTransfer(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "POST") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    try {
        std::string from_account = extractJsonField(request.body, "from_account");
        std::string to_account = extractJsonField(request.body, "to_account");
        std::string amount_str = extractJsonField(request.body, "amount");
        double amount = std::stod(amount_str);
        
        TransactionResult transfer_result = banking_service->transfer(from_account, to_account, amount);
        if (transfer_result.success) {
            response.body = "{\"success\":true,\"message\":\"" + transfer_result.message + "\"}";
        } else {
            response.status_code = 400;
            response.body = "{\"success\":false,\"message\":\"" + transfer_result.message + "\"}";
        }
    } catch (const std::exception& e) {
        response.status_code = 400;
        response.body = "{\"error\":\"Invalid request format\"}";
    }
    
    return response;
}

HttpResponse ApiServer::handleGetTransactions(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "GET") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    auto it = request.query_params.find("account_number");
    if (it == request.query_params.end()) {
        response.status_code = 400;
        response.body = "{\"error\":\"Account number parameter required\"}";
        return response;
    }
    
    try {
        auto transactions = banking_service->getAccountTransactions(it->second);
        std::ostringstream json;
        json << "{\"transactions\":[";
        
        for (size_t i = 0; i < transactions.size(); ++i) {
            if (i > 0) json << ",";
            json << "{";
            json << "\"id\":\"" << transactions[i].getTransactionId() << "\",";
            json << "\"type\":\"" << static_cast<int>(transactions[i].getType()) << "\",";
            json << "\"amount\":" << transactions[i].getAmount() << ",";
            json << "\"description\":\"" << transactions[i].getDescription() << "\",";
            json << "\"timestamp\":\"" << transactions[i].getTimestamp() << "\"";
            json << "}";
        }
        
        json << "]}";
        response.body = json.str();
    } catch (const std::exception& e) {
        response.status_code = 500;
        response.body = "{\"error\":\"Failed to retrieve transactions\"}";
    }
    
    return response;
}

HttpResponse ApiServer::handleGetBalance(const HttpRequest& request) {
    HttpResponse response;
    
    if (request.method != "GET") {
        response.status_code = 405;
        response.body = "{\"error\":\"Method not allowed\"}";
        return response;
    }
    
    auto it = request.query_params.find("account_number");
    if (it == request.query_params.end()) {
        response.status_code = 400;
        response.body = "{\"error\":\"Account number parameter required\"}";
        return response;
    }
    
    try {
        double balance;
        if (banking_service->getAccountBalance(it->second, balance)) {
            response.body = "{\"balance\":" + std::to_string(balance) + "}";
        } else {
            response.status_code = 404;
            response.body = "{\"error\":\"Account not found\"}";
        }
    } catch (const std::exception& e) {
        response.status_code = 404;
        response.body = "{\"error\":\"Account not found\"}";
    }
    
    return response;
}

HttpResponse ApiServer::handleOptions(const HttpRequest& request) {
    HttpResponse response;
    response.status_code = 204;
    response.body = "";
    return response;
}
