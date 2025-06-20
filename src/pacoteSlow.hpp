#ifndef PACOTE_SLOW_HPP
#define PACOTE_SLOW_HPP

#include "utils.hpp"

#define TAMANHO_CABECALHO_PACOTE 32 // 32 bytes

class PacoteSlow {
private:
    bitset<128> sid;
    bitset<27> sttl;
    bitset<5> flags; // Connect, Revive, ACK, Accept/Reject, More Bits
    uint32_t seqNum;
    uint32_t ackNum;
    uint16_t window;
    uint8_t fid;
    uint8_t fo;
    vector<uint8_t> data;

    bool adicionar4BytesAoPacote(vector<uint8_t>& pacote, uint32_t valor);
    bool adicionar2BytesAoPacote(vector<uint8_t>& pacote, uint16_t valor);
public:
    PacoteSlow();

    bool setSid(const bitset<128>& newSid);
    bool setSttl(const bitset<27>& newSttl);
    bool setFlags(const bitset<5>& newFlags);
    bool setSeqNum(uint32_t newSeqNum);
    bool setAckNum(uint32_t newAckNum);
    bool setWindow(uint16_t newWindow);
    bool setFid(uint8_t newFid);
    bool setFo(uint8_t newFo);
    bool setData(const vector<uint8_t>& newData, size_t numBytes);

    vector<uint8_t> getPacote();
};

#endif