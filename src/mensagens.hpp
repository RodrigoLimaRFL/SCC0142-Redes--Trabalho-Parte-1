#include "pacoteSlow.hpp"
PacoteSlow sendData(bitset<128> uuid, bitset<27> sttl, uint32_t ultimoSeqNum, 
    uint32_t ultimoAckNum, uint16_t window, bool maisDados, vector<uint8_t> data);

#define MAX_TAMANHO_DADOS 1472