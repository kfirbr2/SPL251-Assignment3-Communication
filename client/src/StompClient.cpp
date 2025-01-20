#include "../include/StompClient.h"
#include <iostream>
#include <sstream>

StompClient::StompClient() 
    : connection(nullptr), connected(false), receiptCounter(1),
      userCommandHandler(*this), clientProtocol(*this) {}

StompClient::~StompClient() {
    if (connection) {
        delete connection;
        connection = nullptr;
    }
}

bool StompClient::isConnected() {
    return connection != nullptr && connected;
}

void StompClient:: setConnected(bool isConnected){
	
}

ConnectionHandler* StompClient::getConnectionHandler() {
    return connection;
}

void StompClient::setCurrentUser(const std::string& user) {
    currentUser = user;
}

std::string StompClient::getCurrentUser() {
    return currentUser;
}

void StompClient::start(const std::string& hostPort, const std::string& username) {
    if (isConnected()) {
        std::cerr << "[Error] Already logged in as " << currentUser << ". Logout before logging in again.\n";
        return;
    }

    connection = new ConnectionHandler();
    if (!connection->connect()) {
        std::cerr << "[Error] Could not connect to server.\n";
        delete connection;
        connection = nullptr;
        return;
    }

    connected = true;
    setCurrentUser(username);
    serverAddress = hostPort;

    std::unordered_map<std::string, std::string> headers = {
        {"accept-version", "1.2"},
        {"host", "stomp.cs.bgu.ac.il"},
        {"login", username},
        {"passcode", "password"},  // Assume password is required; modify as needed
        {"receipt", std::to_string(generateReceiptID())}
    };

    std::string frame = generateFrame("CONNECT", headers);
    connection->sendFrame(frame);

    listenerThread = std::thread(&StompClient::listenToServer, this);
}

void StompClient::listenToServer() {
    while (isConnected()) {
        std::string response = connection->receiveFrame();
        if (!response.empty()) {
            bool ans = clientProtocol.process(response, reciptId);
        }
    }
}

void StompClient::disconnect() {
    if (!isConnected()) {
        std::cerr << "[Error] No active connection. Cannot disconnect.\n";
        return;
    }

    std::unordered_map<std::string, std::string> headers = {
        {"receipt", std::to_string(generateReceiptID())}
    };

    std::string frame = generateFrame("DISCONNECT", headers);
    connection->sendFrame(frame);
    connection->close();

    if (listenerThread.joinable()) {
        listenerThread.join();
    }

    delete connection;
    connection = nullptr;
    connected = false;
    currentUser.clear();

    std::cout << "[Client] Disconnected successfully.\n";
}

int StompClient::generateReceiptID() {
    return receiptCounter.fetch_add(1);
}

std::string StompClient::generateFrame(std::string command, std::unordered_map<std::string, std::string> headers, std::string body) {
    std::string frame = command + "\n";
    for (const auto& [key, value] : headers) {
        frame += key + ":" + value + "\n";
    }
    frame += "\n" + body + "\0";
    return frame;
}

void StompClient::handleCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "login") {
        std::cerr << "[Error] You cannot log in again. Logout first.\n";
        return;
    } else {
        userCommandHandler.handleCommand(command);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host:port> <username>\n";
        return 1;
    }

    std::string hostPort = argv[1];
    std::string username = argv[2];

    StompClient client;
    client.start(hostPort, username);

    std::string command;
    while (true) {
        std::getline(std::cin, command);
        if (command == "exit") {
            client.disconnect();
            break;
        }
        client.handleCommand(command);
    }

    return 0;
}
