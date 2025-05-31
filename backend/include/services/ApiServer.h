#ifndef API_SERVER_H
#define API_SERVER_H

#include <string>
#include <thread>
#include <memory>
#include <map>
#include <functional>

// Forward declaration to avoid circular includes
class BankingService;
struct AuthResult;
struct AccountCreationResult;
struct TransactionResult;

struct HttpRequest {
    std::string method;
    std::string path;
    std::map<std::string, std::string> headers;
    std::string body;
    std::map<std::string, std::string> query_params;
};

struct HttpResponse {
    int status_code;
    std::map<std::string, std::string> headers;
    std::string body;
    
    HttpResponse(int code = 200) : status_code(code) {
        headers["Content-Type"] = "application/json";
        headers["Access-Control-Allow-Origin"] = "*";
        headers["Access-Control-Allow-Methods"] = "GET, POST, PUT, DELETE, OPTIONS";
        headers["Access-Control-Allow-Headers"] = "Content-Type, Authorization";
    }
};

class ApiServer {
private:
    std::shared_ptr<BankingService> banking_service;
    int port;
    bool running;
    std::thread server_thread;
    
    // Route handlers
    std::map<std::string, std::function<HttpResponse(const HttpRequest&)>> routes;
    
    // Helper methods
    HttpRequest parseRequest(const std::string& raw_request);
    std::string serializeResponse(const HttpResponse& response);
    std::map<std::string, std::string> parseQuery(const std::string& query);
    std::string urlDecode(const std::string& str);
    
    // JSON parsing helper - this was missing!
    std::string extractJsonField(const std::string& json, const std::string& field);
    
    // Route handler methods
    HttpResponse handleLogin(const HttpRequest& request);
    HttpResponse handleRegister(const HttpRequest& request);
    HttpResponse handleGetAccounts(const HttpRequest& request);
    HttpResponse handleCreateAccount(const HttpRequest& request);
    HttpResponse handleDeposit(const HttpRequest& request);
    HttpResponse handleWithdraw(const HttpRequest& request);
    HttpResponse handleTransfer(const HttpRequest& request);
    HttpResponse handleGetTransactions(const HttpRequest& request);
    HttpResponse handleGetBalance(const HttpRequest& request);
    HttpResponse handleOptions(const HttpRequest& request);
    
    // Server management methods
    void setupRoutes();
    void serverLoop();
    void handleClient(int client_socket);

public:
    ApiServer(int port = 8080);
    ~ApiServer();
    
    void setBankingService(std::shared_ptr<BankingService> service);
    bool start();
    void stop();
    bool isRunning() const;
};

#endif // API_SERVER_H
