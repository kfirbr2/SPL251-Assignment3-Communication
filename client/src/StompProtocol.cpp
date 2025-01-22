#include "../include/StompProtocol.h" 
#include "../include/event.h" 
#include "StompProtocol.h"
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <iostream>
#include <chrono>


vector<string> StompProtocol::generteFrame(vector<string> args, string userName){
    vector<string> frames;
    string frame = "";
    if (args.at(0) == "join"){
        receiptId++;
        subscriptionId++;
        subscriptionIdMap.insert({subscriptionId, args.at(1)});
        userMap[userName].insert({args.at(1), subscriptionId});
        frame = "SUBSCRIBE\ndestination:" + args.at(1) + "\nid:" + to_string(subscriptionId) + "\n" +"receipt:" + to_string(receiptId) + "\n" +"\n\0";
        frames.push_back(frame);
    }
    else if (args.at(0) == "exit"){
        receiptId++;
        int id = userMap[userName][args.at(1)];
        frame = "UNSUBSCRIBE\nid:" + to_string(id) + "\n" +"receipt:" + to_string(receiptId) + "\n" + "\n\0";
        frames.push_back(frame);
    }
    else if (args.at(0) == "logout")
    {
        receiptId++;
        string frame = "DISCONNECT\nreceipt-id:" + to_string(receiptId) + "\n\n\0";
        logout= receiptId;
        frames.push_back(frame);
    }
    else if (args.at(0) == "report")
    {
        names_and_events eventsAndNames = parseEventsFile(args.at(1));
        vector<Event> newEvents = eventsAndNames.events;
        string channel = eventsAndNames.channel_name;
        map<string, vector<Event>> eventsMap;
        if ( usersReportMap.find(userName) == usersReportMap.end())
        {
            usersReportMap[userName] = unordered_map<string, vector<Event>>();
        }
        sort(newEvents.begin(), newEvents.end(), [](const Event &firstEvent, const Event &secondEvent)
             { return firstEvent.get_date_time() < secondEvent.get_date_time(); });
        usersReportMap[userName][channel] = newEvents;
        for (const auto &currentEvent : newEvents)
        {
            stringstream frame;
            frame << "SEND\n"
                  << "destination:/" << currentEvent.get_channel_name() << "\n\n"
                  << "user:" << userName << "\n"
                  << "channel name:" << currentEvent.get_channel_name() << "\n"
                  << "city:" << currentEvent.get_city() << "\n"
                  << "event name:" << currentEvent.get_name() << "\n"
                  << "date time:" << currentEvent.get_date_time() << "\n"
                  << "general information:\n";

            for (const auto &information : currentEvent.get_general_information())
            {
                frame << "\t" << information.first << ":" << information.second << "\n";
            }

            frame << "description:\n"
                  << currentEvent.get_description() << "\n\n\0";
            frames.push_back(frame.str());
        }
    }
     else if (args.at(0) == "summary") {
        string user = args.at(1);
        string channel = args.at(2);
        string file = args.at(3);
        generateSummary(user, channel, file);
    }
    else {
        std::cout << "[Error] Invalid command.\n";
    }
    
    return frames;
}
void StompProtocol::process(shared_ptr<ConnectionHandler> &connectionHandler)
{
    // Listen for server messages
    string serverResponse;
    if (connectionHandler->getMessage(serverResponse))
    {
            std::cout << "Server: " << serverResponse << std::endl;
            // Parse server response
            std::istringstream responseStream(serverResponse);
            std::string line;
            std::map<std::string, std::string> headers;

            // Read the first line to determine the STOMP frame type
            if (std::getline(responseStream, line))
            {
                std::string frameType = line;
                if(frameType == "SUBSCRIBE")
                {
                    cout << "Subscribed to " << subscriptionIdMap.find(subscriptionId)->second << endl;
                }
                if(frameType == "UNSUBSCRIBE")
                {
                    cout << "Unsubscribed from " << subscriptionIdMap.find(subscriptionId)->second << endl;
                }
                // Only proceed if the frame type is RECEIPT
                if (frameType == "RECEIPT")
                {
                    // Parse the headers
                    while (std::getline(responseStream, line) && !line.empty())
                    {
                        size_t delimiterPos = line.find(':');
                        if (delimiterPos != std::string::npos)
                        {
                            std::string key = line.substr(0, delimiterPos);
                            std::string value = line.substr(delimiterPos + 1);
                            headers[key] = value;
                        }
                    }

                    // Check for the receipt-id header
                    auto receiptIdIt = headers.find("receipt-id");
                    if (receiptIdIt != headers.end())
                    {
                        int receiptId = stoi(receiptIdIt->second);

                        if (receiptId == logout)
                        {
                            std::cout << "Logout confirmed. Terminating connection." << std::endl;
                            setShouldTerminate(true); 
                        }
                        else
                        {
                            std::cout << "Received receipt-id: " << receiptId << ", but it does not match logoutId: " << logout << std::endl;
                        }
                    }
                    else
                    {
                        std::cerr << "RECEIPT frame received but no receipt-id header found." << std::endl;
                    }
                }
                else if (frameType == "ERROR")
                {
                    isError = true;
                }
            }
            else
            {
                std::cerr << "Failed to read server response or empty response received." << std::endl;
            }
        }
    }

void StompProtocol::generateSummary(const std::string& user, const std::string& channel, const std::string& file) {
    if (usersReportMap.find(user) == usersReportMap.end() || 
        usersReportMap[user].find(channel) == usersReportMap[user].end()) {
        std::cerr << "[Error] No received events found for user: " << user << " in channel: " << channel << std::endl;
        return;
    }

    std::vector<Event> events = usersReportMap[user][channel];

    std::ofstream outputFile(file);
    if (!outputFile) {
        std::cerr << "[Error] Could not open file: " << file << " for writing.\n";
        return;
    }

    int totalReports = events.size();
    int activeCount = std::count_if(events.begin(), events.end(), [](const Event& e) { return e.isActive(); });
    int forcesArrivalCount = std::count_if(events.begin(), events.end(), [](const Event& e) { return e.isForcesArrival(); });

    outputFile << "Channel " << channel << "\n";
    outputFile << "Stats:\n";
    outputFile << "Total: " << totalReports << "\n";
    outputFile << "active: " << activeCount << "\n";
    outputFile << "forces arrival at scene: " << forcesArrivalCount << "\n";
    outputFile << "Event Reports:\n";

    for (const auto& event : events) {
        std::string summary = event.get_description().substr(0, 27) + (event.get_description().size() > 27 ? "..." : "");
        outputFile << "Report:\n";
        outputFile << "city: " << event.get_city() << "\n";
        outputFile << "date time: " << event.get_date_time() << "\n";
        outputFile << "event name: " << event.get_name() << "\n";
        outputFile << "summary: " << summary << "\n\n";
    }

    outputFile.close();
    std::cout << "[Summary] Successfully written summary to " << file << std::endl;
}
bool StompProtocol::getShouldTerminate()
{
    return shouldTerminate;
}

void StompProtocol::setShouldTerminate(bool terminate)
{
    shouldTerminate = terminate;
}
