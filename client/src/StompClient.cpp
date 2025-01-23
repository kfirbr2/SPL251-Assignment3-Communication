#pragma once
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include "../include/keyboardInput.h"
#include "../include/event.h"


StompProtocol protocol;
bool isLoggedIn = false;
bool isError = false;
bool connected = true;




void listenToServer(std::shared_ptr<ConnectionHandler> connectionHandler ) {
    while (isLoggedIn && !isError) {
        std::string response;
		if(connectionHandler->getMessage(response)){
			if (!response.empty()) {
				protocol.process(connectionHandler);
			}
			if(!protocol.getIsError()){
				isError = true;
				
			}
		}
	}
	if(isError){
		connectionHandler->close();
		connectionHandler = nullptr;
        connected = false;
	}
	else{
		connectionHandler->close();
        connectionHandler = nullptr;
	}
}



// Main function
int main(int argc, char *argv[]) {
	vector<string> lineArgs; 
	string outputMsg;
	string inputMsg;
	string host;
	short port;
	shared_ptr<ConnectionHandler> connectionHandler;
	//StompProtocol protocol;
	string userName;
	string passcode;
	mutex mtx;

	while(connected){
		getline(cin,inputMsg);
		split_str(inputMsg, ' ', lineArgs);
		string command = lineArgs.at(0);
		if(command !="login"){
		if(isLoggedIn){
			cout<<"please log in first"<<endl;
			continue;
		}
		 if (command == "join" && lineArgs.at(1) != "")
            {
				std::lock_guard<std::mutex> lock(mtx);				
				vector<string> frames = protocol.generteFrame(lineArgs, userName);
				for (string &frame : frames) {
					connectionHandler->sendMessage(frame);
					}
            }
			else if (lineArgs.at(1) == "")
			{
				cout << "join need 1 args: {channel_name}" << endl;
			}
			else if (command == "exit")
			{
				if (lineArgs.at(1) != "")
				{
					std::lock_guard<std::mutex> lock(mtx);				
					vector<string> frames = protocol.generteFrame(lineArgs, userName);
					for (string &frame : frames) {
					connectionHandler->sendMessage(frame);
					}					
					}
				else
					cout << "exit need 1 args {topic}" << endl;
			}
			else if (command == "report")
			{
				if (lineArgs.at(1) != "")
				{
					std::lock_guard<std::mutex> lock(mtx);				
					vector<string> frames = protocol.generteFrame(lineArgs, userName);
					for (string &frame : frames) {
					connectionHandler->sendMessage(frame);
					}
				}
				cout << "report need 1 args {file}" << endl;
			}
			else if (command == "summary")
			{
				if (lineArgs.at(1) != "" && lineArgs.at(2) != "" && lineArgs.at(3) != "")
				{
					protocol.generteFrame(lineArgs, userName);
				}
				else
				{
					cout << "summary need 3 args {channel name}, {user} , {file}" << endl;
				}
			}
			else if (command == "logout")
			{
				std::lock_guard<std::mutex> lock(mtx);				
				vector<string> frames = protocol.generteFrame(lineArgs, userName);
				for (string &frame : frames) {
					connectionHandler->sendMessage(frame);
					}
					isLoggedIn = false;
				}
				else
				{
					std::cerr << "Failed to confirm logout. Connection remains open." << std::endl;
				}
			}
			else if (command == "login")
	{
		if (lineArgs.at(1) != "")
		{
			vector<string> portAndHost;
			split_str(lineArgs[1], ':',portAndHost);
			host = portAndHost[0];
			port = stoi(portAndHost[1]);
			userName = lineArgs.at(2);
			passcode = lineArgs.at(3);

			string frame;
            frame = "CONNECT\naccept: version :1.2" "\n\nhost:" +host +"\n"+ "login:" + userName + "\n"+"passcode:" +passcode + "\n\n\0";
			connectionHandler = std::make_shared<ConnectionHandler>(host, port);
			connectionHandler->sendMessage(frame);
			if (connectionHandler->connect())
			{
				isLoggedIn = true;
				thread serverThread(listenToServer, connectionHandler);
				serverThread.join();
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
	}

    return 0;
} 

	
	
	
	