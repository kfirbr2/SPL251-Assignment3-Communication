#ifndef STOMP_PROTOCOL_H
#define STOMP_PROTOCOL_H

#include <string>

class StompProtocol {
public:
    StompProtocol();                             // קונסטרקטור
    ~StompProtocol();                            // דסטרקטור
    bool process(const std::string& frame, int receiptId); // עיבוד הודעת Frame
};

#endif
