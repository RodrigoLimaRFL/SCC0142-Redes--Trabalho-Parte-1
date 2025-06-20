#include "pacoteSlow.hpp"

using namespace std;

PacoteSlow::PacoteSlow() {
    /**
     * Construtor da classe PacoteSlow.
     * Inicializa os campos do pacote com valores padrão.
     * - sid: Identificador único do pacote (128 bits) é inicializado com zeros.
     * - sttl: Tempo de vida do pacote (27 bits) é inicializado com zeros.
     * - flags: Conjunto de flags (5 bits) é inicializado com zeros.
     * - seqNum: Número de sequência (32 bits) é inicializado com zero.
     * - ackNum: Número de reconhecimento (32 bits) é inicializado com zero.
     * - window: Janela de bytes (16 bits) é inicializada com zero.
     * - fid: Identificador do fluxo (8 bits) é inicializado com zero.
     * - fo: Offset do fluxo (8 bits) é inicializado com zero.
     * - data: Vetor de dados é inicializado como vazio.
     */

    sid.reset();
    sttl.reset();
    flags.reset(); // Connect, Revive, ACK, Accept/Reject, More Bits
    seqNum = 0;
    ackNum = 0;
    window = 0;
    fid = 0;
    fo = 0;
    data.clear();
}

bool PacoteSlow::setSid(const bitset<128>& newSid) {
    /**
     * Define o identificador único do pacote (SID).
     */
    sid = newSid;
    return true;
}

bool PacoteSlow::setSttl(const bitset<27>& newSttl) {
    /**
     * Define o tempo de vida do pacote (STTL).
     */
    sttl = newSttl;
    return true;
}

bool PacoteSlow::setFlags(const bitset<5>& newFlags) {
    /**
     * Define as flags do pacote.
     */
    flags = newFlags;
    return true;
}

bool PacoteSlow::setSeqNum(uint32_t newSeqNum) {
    /**
     * Define o número de sequência do pacote (SEQNUM).
     */
    seqNum = newSeqNum;
    return true;
}

bool PacoteSlow::setAckNum(uint32_t newAckNum) {
    /**
     * Define o número de reconhecimento do pacote (ACKNUM).
     */
    if(flags[2] == 0) {
        cerr << "Erro: ACK só pode ser definido quando a flag ACK é 1" << endl;
        return false;
    }
    ackNum = newAckNum;
    return true;
}

bool PacoteSlow::setWindow(uint16_t newWindow) {
    /**
     * Define a janela de bytes do pacote (WINDOW).
     */
    window = newWindow;
    return true;
}

bool PacoteSlow::setFid(uint8_t newFid) {
    /**
     * Define o identificador do fluxo do pacote (FID).
     */
    fid = newFid;
    return true;
}

bool PacoteSlow::setFo(uint8_t newFo) {
    /**
     * Define o offset do fluxo do pacote (FO).
     */
    fo = newFo;
    return true;
}

bool PacoteSlow::setData(const vector<uint8_t>& newData, int numBytes) {
    /**
     * Define os dados do pacote.
     * 
     * Params:
     * - newData: Vetor de bytes contendo os dados a serem definidos no pacote.
     * - numBytes: Número de bytes a serem copiados do vetor newData para o pacote.
     */
    if(numBytes < 0 || numBytes > 1440) {
        cerr << "Erro: Tamanho de dados inválido. Deve ser entre 0 e 1440 bytes." << endl;
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
    /**
     * Adiciona um valor de 4 bytes ao final do pacote.
     */
    for(int i = 0; i < 4; i++) {
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

bool PacoteSlow::adicionar2BytesAoPacote(vector<uint8_t>& pacote, uint16_t valor) {
    /**
     * Adiciona um valor de 2 bytes ao final do pacote.
     */
    for(int i = 0; i < 2; i++) {
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

vector<uint8_t> PacoteSlow::getPacote() {
    /**
     * Gera o pacote completo a partir dos campos definidos na classe.
     */
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
    uint32_t sttlValue = sttl.to_ulong();
    uint32_t flagsValue = flags.to_ulong();
    uint32_t sttlFlags = (sttlValue << 5) | flagsValue;
    adicionar4BytesAoPacote(pacote, sttlFlags);

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
