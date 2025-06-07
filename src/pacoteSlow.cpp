#include "pacoteSlow.hpp"

using namespace std;

PacoteSlow::PacoteSlow() {
    sid.reset();
    ttl.reset();
    flags.reset();
    seqNum = 0;
    ackNum = 0;
    window = 0;
    fid = 0;
    fo = 0;
    data.clear();
}

bool PacoteSlow::setSid(const bitset<128>& newSid) {
    sid = newSid;
    return true;
}

bool PacoteSlow::setTtl(const bitset<27>& newTtl) {
    ttl = newTtl;
    return true;
}

bool PacoteSlow::setFlags(const bitset<5>& newFlags) {
    flags = newFlags;
    return true;
}

bool PacoteSlow::setSeqNum(uint32_t newSeqNum) {
    seqNum = newSeqNum;
    return true;
}

bool PacoteSlow::setAckNum(uint32_t newAckNum) {
    if(flags[2] == 0) {
        cerr << "Erro: ACK só pode ser definido quando a flag ACK é 1" << endl;
        return false;
    }
    ackNum = newAckNum;
    return true;
}

bool PacoteSlow::setWindow(uint16_t newWindow) {
    window = newWindow;
    return true;
}

bool PacoteSlow::setFid(uint8_t newFid) {
    fid = newFid;
    return true;
}

bool PacoteSlow::setFo(uint8_t newFo) {
    fo = newFo;
    return true;
}

bool PacoteSlow::setData(const vector<uint8_t>& newData, int numBytes) {
    if(numBytes < 0 || numBytes > 180) {
        cerr << "Erro: Tamanho de dados inválido. Deve ser entre 0 e 180 bytes." << endl;
        return false;
    }

    if(newData.size() < numBytes) {
        cerr << "Erro: Dados insuficientes para preencher o pacote." << endl;
        return false;
    }

    data.assign(newData.begin(), newData.begin() + numBytes);
    return true;
}

bool PacoteSlow::adicionar4BytesAoPacote(vector<uint8_t>& pacote, uint32_t valor) {
    for(int i = 0; i < 4; i++) {
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

bool PacoteSlow::adicionar2BytesAoPacote(vector<uint8_t>& pacote, uint16_t valor) {
    for(int i = 0; i < 2; i++) {
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

vector<uint8_t> PacoteSlow::getPacote() {
    vector<uint8_t> pacote;

    // SID

    for(int i = 0; i < 16; i++) {
        uint8_t byte = 0;
        for(int j = 0; j < 8; j++) {
            byte |= (sid[i * 8 + j] << j);
        }
        pacote.push_back(byte);
    }

    // TTL + Flags
    uint32_t ttlValue = ttl.to_ulong();
    uint32_t flagsValue = flags.to_ulong();
    uint32_t ttlFlags = (ttlValue << 5) | flagsValue;
    adicionar4BytesAoPacote(pacote, ttlFlags);

    // SeqNum
    adicionar4BytesAoPacote(pacote, seqNum);

    // AckNum
    adicionar4BytesAoPacote(pacote, ackNum);

    // Window
    adicionar2BytesAoPacote(pacote, window);

    // Fid + Fo
    pacote.push_back(fid);
    pacote.push_back(fo);

    // Dados
    pacote.insert(pacote.end(), data.begin(), data.end());

    return pacote;
}
