#include "../include/StompProtocol.h" 
#include "StompProtocol.h"
#include <iostream>
#include <sstream>

// קונסטרקטור
StompProtocol::StompProtocol() {}

// דסטרקטור
StompProtocol::~StompProtocol() {}

// עיבוד הודעת Frame
bool StompProtocol::process(const std::string& frame, int receiptId) {
    // פיצול ההודעה לשורות
    std::istringstream stream(frame);
    std::string command;
    // קריאת השורה הראשונה לזיהוי סוג ההודעה
    std::getline(stream, command);
    // הדפסת כל תוכן ה-Frame למסך
    std::cout << frame << std::endl;
    // אם ההודעה היא ERROR
    if (command == "ERROR") {
        return true;         // החיבור צריך להיסגר
    }

    // אם ההודעה היא RECEIPT
    if (command == "RECEIPT") {
        std::string receiptLine;
        while (std::getline(stream, receiptLine)) {
            if (receiptLine.find("receipt-id:") == 0) {
                int receivedId = std::stoi(receiptLine.substr(11)); // חילוץ ה-ID
                if (receivedId == receiptId) {
                    return true; // ה-ID תואם
                }
            }
        }
    }

    return false; // ברירת המחדל: לא RECEIPT תואם ולא ERROR
}



