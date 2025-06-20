#include "session.hpp"

Session::Session() : uuid(16, 0), sttl(0), seqNum(0), ackNum(0) {}

vector<uint8_t> Session::getUUID() {
    return uuid;
}

bool Session::setUUID(const vector<uint8_t>& newUUID) {
    if (newUUID.size() != 16) {
        return false; // UUID must be 16 bytes
    }
    uuid = newUUID;
    return true;
}

uint32_t Session::getSTTL() {
    return sttl;
}

bool Session::setSTTL(uint32_t newSTTL) {
    sttl = newSTTL;
    return true;
}

uint32_t Session::getSeqNum() {
    return seqNum;
}

bool Session::setSeqNum(uint32_t newSeqNum) {
    seqNum = newSeqNum;
    return true;
}

uint32_t Session::getAckNum() {
    return ackNum;
}

bool Session::setAckNum(uint32_t newAckNum) {
    ackNum = newAckNum;
    return true;
}