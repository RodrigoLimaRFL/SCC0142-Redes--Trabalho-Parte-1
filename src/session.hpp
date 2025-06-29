#ifndef SESSION_HPP
#define SESSION_HPP

#include <vector>
#include <cstdint>
#include <thread>
#include <chrono>

#include "pacoteSlow.hpp"
#include "session.hpp"

using namespace std;

class Session {
private:
    atomic<bool> running;       // Flag to control the thread
    thread ttlThread;           // Background thread
    vector<uint8_t> uuid; 
    uint32_t sttl;
    uint32_t seqNum;
    uint32_t ackNum;
    uint16_t window;

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
    uint16_t getWindow();
    bool setWindow(uint16_t newWindow);
    bool setValues(PacoteSlow pacote);
    bitset<128> getUUIDBits();
    void startTTLTimer();               // Starts the background thread
    void stopTTLTimer();                // Stops the background thread
    ~Session();                         // Destructor to stop the thread

};
#endif