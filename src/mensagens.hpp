#include "utils.hpp"
#include "pacoteSlow.hpp"

#define MAX_TAMANHO_DADOS 1472

PacoteSlow Disconnect(bitset<128> uuid, bitset<27> sttl, uint32_t seqnum, uint32_t acknum);
PacoteSlow sendData(bitset<128> uuid, bitset<27> sttl, uint32_t ultimoSeqNum, 
uint32_t ultimoAckNum, uint16_t window, bool maisDados, vector<uint8_t> data);
PacoteSlow connect(uint16_t tamBufferRecebimento);
