#include <iostream>
#include <sstream>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"


// StompClient::StompClient() 
    // : connection(nullptr), connected(false), protocol() {
		// protocol = new StompProtocol();
	//   }

	StompProtocol protocol;

// StompClient::~StompClient() {
    // if (connection) {
        // delete connection;
        // connection = nullptr;
		// delete protocol;
		// protocol = nullptr;
    // }
// }


void StompClient::listenToServer() {
    while (connectionHandler!= nullptr && connectionHandler->isConnected()) {
        std::string response;
		if(connectionHandler->getMessage(response)){
			if (!response.empty()) {
				protocol.process(connectionHandler);
			}
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
	bool connected = true;
	string userName;

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

	
	
	
	