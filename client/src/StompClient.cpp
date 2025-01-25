#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <string>
#include <sstream>
#include "../include/StompProtocol.h"
#include "../include/ConnectionHandler.h"
#include "../include/KeyBoardInput.h"
#include "../include/event.h"

StompProtocol protocol;
bool isLoggedIn = false;
bool isError = false;
bool connected = true;
string userName;
shared_ptr<ConnectionHandler> connectionHandler = nullptr;
vector<string> lineArgs;
string outputMsg;
string inputMsg;
string host;
short port;
string passcode;
mutex mtx;


void listenToServer(std::shared_ptr<ConnectionHandler> &connectionHandler)
{
    while (connected && !isError)
    {

        if (connectionHandler != nullptr)
        {
            std::string response;
            if (connectionHandler->isConnect() && connectionHandler->isAvailable() > 0)
            {
                printf("mashu matzchik1");
                if (connectionHandler->getMessages(response))
                {
                    printf("mashu matzchik2");
                    protocol.process(connectionHandler, response);
                    if (protocol.getIsError())
                    {
                        isError = true;
                        continue;
                    }
                    if(protocol.getShouldTerminate())
                    {
                        connectionHandler->close();
                        connectionHandler = nullptr;
                        isLoggedIn = false;
                        continue;
                    }
                }
            }
        }
    }

        if (isError)
        {
        	protocol.deleteFromUserMap(userName);
        	connectionHandler->close();
        	connectionHandler = nullptr;
        	connected = false;
        }

}

void sendMessages()

{
    while (connected)
    {
        mtx.lock();
        getline(cin, inputMsg);
        split_str(inputMsg, ' ', lineArgs);
        string command = lineArgs.at(0);
        if (command != "login")
        {
            if (!isLoggedIn)
            {
                cout << "please log in first" << endl;
                mtx.unlock();
                continue;
            }
            if (command == "join" ){
            if(lineArgs.at(1) != "")
            {

                vector<string> frames = protocol.generteFrame(lineArgs, userName);
                for (string &frame : frames)
                {
                    connectionHandler->sendMessages(frame);
                    mtx.unlock();
                    continue;
                }
            
            }
            else 
            {
                cout << "join need 1 args: {channel_name}" << endl;
            }
            }
            
            else if (command == "exit")
            {
                if (lineArgs.at(1) != "")
                {

                    vector<string> frames = protocol.generteFrame(lineArgs, userName);
                    for (string &frame : frames)
                    {
                        connectionHandler->sendMessages(frame);
                        mtx.unlock();
                        continue;
                    }
                }
                else
                {

                    cout << "exit need 1 args {topic}" << endl;
                    mtx.unlock();
                    continue;
                }
            }
            else if (command == "report")
            {
                if (lineArgs.at(1) != "")
                {

                    vector<string> frames = protocol.generteFrame(lineArgs, userName);
                    for (string &frame : frames)
                    {
                        connectionHandler->sendMessages(frame);
                        mtx.unlock();
                        continue;
                    }
                }
                else
                {

                    cout << "report need 1 args {file}" << endl;
                    mtx.unlock();
                    continue;
                }
            }
            else if (command == "summary")
            {
                if (lineArgs.at(1) != "" && lineArgs.at(2) != "" && lineArgs.at(3) != "")
                {
                    protocol.generteFrame(lineArgs, userName);
                    mtx.unlock();
                    continue;
                }
                else
                {
                    cout << "summary need 3 args {channel name}, {user} , {file}" << endl;
                    mtx.unlock();
                    continue;
                }
            }
            else if (command == "logout")
            {
                printf("logout1");
                vector<string> frames = protocol.generteFrame(lineArgs, userName);
                for (string &frame : frames)
                {
                    connectionHandler->sendMessages(frame);
                    mtx.unlock();
                    continue;
                }
            }
            else
            {
                std::cerr << "Failed to confirm logout. Connection remains open." << std::endl;
                mtx.unlock();
                continue;
            }
        }

        else if (command == "login")
        {
            if (lineArgs.at(1) != "")
            {
                vector<string> portAndHost;
                split_str(lineArgs.at(1), ':', portAndHost);

                userName = lineArgs.at(2);
                passcode = lineArgs.at(3);
                isLoggedIn = true;
                connectionHandler = std::make_shared<ConnectionHandler>(portAndHost.at(0), stoi(portAndHost.at(1)));

                string frame = "CONNECT\n"
                               "accept-version:1.2\n"
                               "host:stomp.cs.bgu.ac.il\n"
                               "login:" +
                               lineArgs.at(2) + "\n"
                                                "passcode:" +
                               lineArgs.at(3) + "\n\n\0";
                cout << " host: " << host << " port: " << port << endl;
                cout << "login frame: " << frame << endl;
                if (!connectionHandler->connect())
                {
                    std::cerr << "Could not connect to server" << std::endl;
                    connectionHandler = nullptr;
                    mtx.unlock();
                    continue;
                }
                else
                {
                    connectionHandler->sendMessages(frame);
                    mtx.unlock();
                    continue;
                }
                mtx.unlock();
                continue;
            }
            else
            {
                std::cerr << "login need 3 args: {host} {port} {username}" << std::endl;
                mtx.unlock();
                continue;
            }
        }

        else
        {
            std::cerr << "Invalid command." << std::endl;
            mtx.unlock();
            continue;
        }
        mtx.unlock();
    }
}

// Main function
int main(int argc, char *argv[])
{
    thread messagesThread(sendMessages);
    thread serverThread(listenToServer, ref(connectionHandler));
    messagesThread.join();
    serverThread.join();
}