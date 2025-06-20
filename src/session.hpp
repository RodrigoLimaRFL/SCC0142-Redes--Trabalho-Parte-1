#ifndef SESSION_HPP
#define SESSION_HPP

#include <vector>
#include <cstdint>

using namespace std;

class Session {
private:
    vector<uint8_t> uuid; 
    uint32_t sttl;
    uint32_t seqNum;
    uint32_t ackNum;

public:
    Session();

    vector<uint8_t> getUUID();
    bool setUUID(const vector<uint8_t>& newUUID);
    uint32_t getSTTL();
    bool setSTTL(uint32_t newSTTL);
    uint32_t getSeqNum();
    bool setSeqNum(uint32_t newSeqNum);
    uint32_t getAckNum();
    bool setAckNum(uint32_t newAckNum);
};
#endif