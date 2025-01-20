#include "../include/UserCommandHandler.h"
#include "../include/events.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

UserCommandHandler::UserCommandHandler(StompClient& client) : client(client) {}

void UserCommandHandler::handleCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    if (cmd == "report") {
        std::string file;
        iss >> file;

        std::vector<Event> events = parseEventFile(file);  
        if (events.empty()) {
            std::cerr << "[Error] No events found in file: " << file << std::endl;
            return;
        }

        std::string currentUser = client.getCurrentUser();
        if (currentUser.empty()) {
            std::cerr << "[Error] No user logged in. Cannot report events.\n";
            return;
        }

        for (const auto& event : events) {
            saveReceivedEvent(currentUser, event.channelName, event);  

            std::unordered_map<std::string, std::string> headers = {
                {"destination", "/" + event.channelName},
                {"user", currentUser},
                {"city", event.city},
                {"event name", event.eventName},
                {"date time", std::to_string(event.timestamp)},
                {"receipt", std::to_string(client.generateReceiptID())}
            };

            std::string body = "general information:\n";
            body += "active:" + std::string(event.active ? "true" : "false") + "\n";
            body += "forces arrival at scene:" + std::string(event.forcesArrival ? "true" : "false") + "\n";
            body += "description:\n" + event.description;

            std::string frame = client.generateFrame("SEND", headers, body);
            client.getConnectionHandler()->sendFrame(frame);
        }

        std::cout << "[Report] Successfully sent " << events.size() << " events from file: " << file << std::endl;
    } 
    else if (cmd == "summary") {
        std::string user, channel, file;
        iss >> user >> channel >> file;
        generateSummary(user, channel, file);
    }
    else {
        std::cout << "[Error] Invalid command.\n";
    }
}

void UserCommandHandler::saveReceivedEvent(const std::string& user, const std::string& channel, const Event& event) {
    receivedEventHistory[user][channel].push_back(event);
}

void UserCommandHandler::generateSummary(const std::string& user, const std::string& channel, const std::string& file) {
    if (receivedEventHistory.find(user) == receivedEventHistory.end() || 
        receivedEventHistory[user].find(channel) == receivedEventHistory[user].end()) {
        std::cerr << "[Error] No received events found for user: " << user << " in channel: " << channel << std::endl;
        return;
    }

    std::vector<Event> events = receivedEventHistory[user][channel];

    // Sort events by timestamp, then by event name
    std::sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
        return (a.timestamp < b.timestamp) || (a.timestamp == b.timestamp && a.eventName < b.eventName);
    });

    std::ofstream outputFile(file);
    if (!outputFile) {
        std::cerr << "[Error] Could not open file: " << file << " for writing.\n";
        return;
    }

    int totalReports = events.size();
    int activeCount = std::count_if(events.begin(), events.end(), [](const Event& e) { return e.active; });
    int forcesArrivalCount = std::count_if(events.begin(), events.end(), [](const Event& e) { return e.forcesArrival; });

    outputFile << "Channel " << channel << "\n";
    outputFile << "Stats:\n";
    outputFile << "Total: " << totalReports << "\n";
    outputFile << "active: " << activeCount << "\n";
    outputFile << "forces arrival at scene: " << forcesArrivalCount << "\n";
    outputFile << "Event Reports:\n";

    for (const auto& event : events) {
        std::string summary = event.description.substr(0, 27) + (event.description.size() > 27 ? "..." : "");
        outputFile << "Report:\n";
        outputFile << "city: " << event.city << "\n";
        outputFile << "date time: " << event.timestamp << "\n";
        outputFile << "event name: " << event.eventName << "\n";
        outputFile << "summary: " << summary << "\n\n";
    }

    outputFile.close();
    std::cout << "[Summary] Successfully written summary to " << file << std::endl;
}
