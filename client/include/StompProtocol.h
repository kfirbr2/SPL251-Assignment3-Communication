#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>       
#include "ConnectionHandler.h" 
using namespace std;
class StompProtocol {
public:
    vector<string> generteFrame(vector<string> args, string userName);
    void process(shared_ptr<ConnectionHandler> &connectionHandler); 
    void generateSummary(const std::string& user, const std::string& channel, const std::string& file);
    bool getShouldTerminate();
    void setShouldTerminate(bool terminate);

private: 
int subscriptionId = 0;
int receiptId = 0;
int logout = 0;
bool shouldTerminate = false;
bool isError;
string userName;
unordered_map<int, string> subscriptionIdMap;
unordered_map<string,unordered_map<string, int>> userMap;
unordered_map<string, unordered_map<string, vector<Event>>> usersReportMap;


};


