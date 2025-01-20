#ifndef STOMP_CLIENT_H
#define STOMP_CLIENT_H

#include "ConnectionHandler.h"
#include "UserCommandHandler.h"
#include "ClientProtocol.h"
#include <string>
#include <thread>
#include <atomic>
#include <unordered_map>

class StompClient {
private:
    ConnectionHandler* connection;
    std::atomic<bool> connected;
    std::string currentUser;
    std::string serverAddress;

    std::thread listenerThread;
    UserCommandHandler userCommandHandler;
    ClientProtocol clientProtocol;
    std::atomic<int> receiptCounter;

    void listenToServer();

public:
    StompClient();
    ~StompClient();

    bool isConnected();
    ConnectionHandler* getConnectionHandler();
    void setCurrentUser(const std::string& user);
    std::string getCurrentUser();

    void start(const std::string& hostPort, const std::string& username);
    void disconnect();
    void handleCommand(const std::string& command);
    std::string generateFrame(std::string command, std::unordered_map<std::string, std::string> headers, std::string body = "");
    int generateReceiptID();
};

#endif // STOMP_CLIENT_H
