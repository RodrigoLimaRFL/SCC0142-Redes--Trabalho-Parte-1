#include "utils.hpp"

class PacoteSlow {
    /* 
    */
private:
    bitset<128> sid;
    bitset<27> ttl;
    bitset<5> flags; // Connect, Revive, Ack, Accept/Reject, MoreBits
    uint32_t seqNum;
    uint32_t ackNum;
    uint16_t window;
    uint8_t fid;
    uint8_t fo;
    vector<uint8_t> data;

    bool adicionar4BytesAoPacote(vector<uint8_t>& pacote, uint32_t valor) {
        for(int i = 0; i < 4; i++) {
            pacote.push_back((valor >> (i * 8)) & EIGHT_BIT_MASK);
        }
        return true;
    }

    bool adicionar2BytesAoPacote(vector<uint8_t>& pacote, uint16_t valor) {
        for(int i = 0; i < 2; i++) {
            pacote.push_back((valor >> (i * 8)) & EIGHT_BIT_MASK);
        }
        return true;
    }
public:
    PacoteSlow() {
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

    // Métodos para definir os campos do pacote

    bool setSid(const bitset<128>& newSid) {
        sid = newSid;
        return true;
    }

    bool setTtl(const bitset<27>& newTtl) {
        ttl = newTtl;
    }

    bool setFlags(const bitset<5>& newFlags) {
        flags = newFlags;
        return true;
    }

    bool setSeqNum(uint32_t newSeqNum) {
        seqNum = newSeqNum;
        return true;
    }

    bool setAckNum(uint32_t newAckNum) {
        if(flags[2] == 0) { // Checa se a flag ACK é 1
            cerr << "Erro: ACK só pode ser definido quando a flag ACK é 1" << endl;
            return false;
        }

        ackNum = newAckNum;
        return true;
    }

    bool setWindow(uint16_t newWindow) {
        window = newWindow;
        return true;
    }

    bool setFid(uint8_t newFid) {
        fid = newFid;
        return true;
    }

    bool setFo(uint8_t newFo) {
        fo = newFo;
        return true;
    }

    bool setData(const vector<uint8_t>& newData, int numBytes) {
        if(numBytes < 0 || numBytes > 180) { // Tamanho máximo de dados é 1440 bits
            cerr << "Erro: Tamanho de dados inválido. Deve ser entre 0 e 180 bytes." << endl;
            return false;
        }

        data.clear();

        for(int i = 0; i < numBytes; i++) {
            if(i >= newData.size()) {
                cerr << "Erro: Dados insuficientes para preencher o pacote." << endl;
                return false;
            }
            data.push_back(newData[i]);
        }

        return true;     
    }

    vector<uint8_t> getPacote()
    {
        vector<uint8_t> pacote;

        // SID
        for(int i = 0; i < 4; i++) {
            // Extrai 32 bits de sid e converte para uint32_t
            uint32_t part = (sid >> (i * 32)).to_ulong();

            adicionar4BytesAoPacote(pacote, part);
        }

        // TTL e flags
        uint32_t ttlFlags = (ttl.to_ulong() << 5) | flags.to_ulong();
        adicionar4BytesAoPacote(pacote, ttlFlags);

        // SeqNum
        adicionar4BytesAoPacote(pacote, seqNum);

        // AckNum
        adicionar4BytesAoPacote(pacote, ackNum);

        // Window
        adicionar2BytesAoPacote(pacote, window);

        // Fid e Fo
        pacote.push_back(fid);
        pacote.push_back(fo);

        // Dados
        for(uint8_t byte : data) {
            pacote.push_back(byte);
        }

        return pacote;
    }
};