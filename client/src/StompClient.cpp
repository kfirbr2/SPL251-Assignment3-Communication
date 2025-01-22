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
    ConnectionHandler *connectHandller = nullptr;
	queue<String> messages; 
	string outputMsg;
	string inputMsg;
	string host;
	short port;
	thread connectionThread; 
	StompProtocol protocol;

	while(connected){
		getline(cin,input);
		split_str(input, ' ', lineArgs);
		string command = lineArgs.at(0);
		if(commaned !="login"){
		if(isLoggedIn){
			cout<<"please log in first"<<endl;
			continue;
		}
		 if (command == "join" && lineArgs.at(1) != "")
            {
                queueMutex.lock();
				vector<string> frames = protocol.generteFrame(lineArgs, userName);
				for (const string &frame : frames) {
					queueMessages.push(frame);
					}
                queueMutex.unlock();
            }
			else if (lineArgs.at(1) == "")
			{
				cout << "join need 1 args: {channel_name}" << endl;
			}
			else if (command == "exit")
			{
				if (lineArgs.at(1) != "")
				{
					queueMutex.lock();
					vector<string> frames = protocol.generteFrame(lineArgs, userName);
					for (const string &frame : frames) {
					queueMessages.push(frame);
					}
					queueMutex.unlock();
					}
				else
					cout << "exit need 1 args {topic}" << endl;
			}
			else if (command == "report")
			{
				if (lineArgs.at(1) != "")
				{
					queueMutex.lock();
					vector<string> frames = protocol.generteFrame("json_path", userName);
					for (string report : frames)
					{
						queueMessages.push(report);
					}
					queueMutex.unlock();
				}
				cout << "report need 1 args {file}" << endl;
			}
			else if (command == "summary")
			{
				if (lineArgs.at(1) != "" && lineArgs.at(2) != "" && lineArgs.at(3) != "")
				{
					protocol.generteFrame(lineArgs);
				}
				else
				{
					cout << "summary need 3 args {channel name}, {user} , {file}" << endl;
				}
			}
			else if (command == "logout")
			{
				queueMutex.lock();
				vector<string> frames = protocol.generteFrame(lineArgs, userName);
				for (const string &frame : frames) {
					queueMessages.push(frame);
					}
				queueMutex.unlock();
					// connectionHandler->close();
					// connectionHandler = nullptr;
				}
				else
				{
					std::cerr << "Failed to confirm logout. Connection remains open." << std::endl;
				}
			}
			else
			{
				std::cout << "No active connection to logout from." << std::endl;
			}

	}
	else if (command == "login")
	{
		if (lineArgs.at(1) != "")
		{
			host = lineArgs.at(1);
			port = stoi(lineArgs.at(2));
			userName = lineArgs.at(3);
			connectionHandler = new ConnectionHandler(host, port);
			if (connectionHandler->connect())
			{
				isLoggedIn = true;
				connectionThread = thread(&ConnectionHandler::run, connectionHandler);
			}
			else
			{
				std::cerr << "Failed to connect to server." << std::endl;
			}
		}
		else
		{
			std::cerr << "login need 3 args: {host} {port} {username}" << std::endl;
		}
	}
	else
	{
		std::cerr << "Invalid command." << std::endl;
	}

	
	
	
	