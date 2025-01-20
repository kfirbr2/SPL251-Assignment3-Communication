#ifndef USER_COMMAND_HANDLER_H
#define USER_COMMAND_HANDLER_H

#include "StompClient.h"
#include "events.h"
#include <string>
#include <unordered_map>
#include <vector>

// UserCommandHandler class manages STOMP client commands
class UserCommandHandler {
private:
    StompClient& client;

    // Maps user -> channels they received messages from -> list of events
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<Event>>> receivedEventHistory;

public:
    // Constructor
    UserCommandHandler(StompClient& client);

    // Handles user input commands
    void handleCommand(const std::string& command);

    // Stores messages received from a user in a specific channel
    void saveReceivedEvent(const std::string& user, const std::string& channel, const Event& event);

    // Generates a summary file of messages received from a user in a specific channel
    void generateSummary(const std::string& user, const std::string& channel, const std::string& file);
};

#endif // USER_COMMAND_HANDLER_H