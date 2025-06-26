// pacoteSlow.cpp
#include "pacoteSlow.hpp"

using namespace std;

PacoteSlow::PacoteSlow() {
    /**
     * Construtor da classe PacoteSlow.
     * Inicializa os campos do pacote com valores padrão.
     * - sid: Identificador único do pacote (128 bits) é inicializado com zeros. [cite: 13]
     * - sttl: Tempo de vida do pacote (27 bits) é inicializado com zeros. [cite: 15]
     * - flags: Conjunto de flags (5 bits) é inicializado com zeros. [cite: 14]
     * - seqNum: Número de sequência (32 bits) é inicializado com zero. [cite: 15]
     * - ackNum: Número de reconhecimento (32 bits) é inicializado com zero. [cite: 15]
     * - window: Janela de bytes (16 bits) é inicializada com zero. [cite: 16]
     * - fid: Identificador do fluxo (8 bits) é inicializado com zero. [cite: 16]
     * - fo: Offset do fluxo (8 bits) é inicializado com zero. [cite: 16]
     * - data: Vetor de dados é inicializado como vazio. [cite: 17]
     */

    sid.reset();
    sttl.reset();
    flags.reset(); // Connect, Revive, ACK, Accept/Reject, More Bits [cite: 14]
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

bool PacoteSlow::setData(const vector<uint8_t>& newData, size_t numBytes) {
    /**
     * Define os dados do pacote.
     *
     * Params:
     * - newData: Vetor de bytes contendo os dados a serem definidos no pacote.
     * - numBytes: Número de bytes a serem copiados do vetor newData para o pacote.
     */
    // "data: Data, máximo de 1440 bytes." [cite: 17]
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

vector<uint8_t> PacoteSlow::getSid() const {
    /**
     * Retorna o identificador único do pacote (SID) como um vetor de bytes.
     */
    vector<uint8_t> sidBytes(16, 0); // Inicializa os 16 bytes com 0

    // Itera por cada um dos 16 bytes que formam o SID
    for(int i = 0; i < 16; i++) {
        // Itera por cada um de 8 bits que formam o byte atual
        for(int j = 0; j < 8; j++) {
            // Se o bit na posição correspondente do bitset<128> for 1...
            // "Todos os bits são little endian." [cite: 13]
            // So, bitset[0] is the LSB of the first byte, bitset[7] is MSB of first byte,
            // bitset[8] is LSB of second byte, etc.
            if (sid[i * 8 + j]) {
                // ...ativa o bit correspondente no byte atual.
                sidBytes[i] |= (1 << j);
            }
        }
    }
    return sidBytes;
}

uint32_t PacoteSlow::getSttl() const {
    /**
     * Retorna o tempo de vida do pacote (STTL) como um valor inteiro.
     */
    return sttl.to_ulong();
}

bitset<5> PacoteSlow::getFlags() const {
    /**
     * Retorna as flags do pacote.
     */
    return flags;
}

uint32_t PacoteSlow::getSeqNum() const {
    /**
     * Retorna o número de sequência do pacote (SEQNUM).
     */
    return seqNum;
}

uint32_t PacoteSlow::getAckNum() const {
    /**
     * Retorna o número de reconhecimento do pacote (ACKNUM).
     */
    return ackNum;
}

uint16_t PacoteSlow::getWindow() const {
    /**
     * Retorna a janela de bytes do pacote (WINDOW).
     */
    return window;
}

uint8_t PacoteSlow::getFid() const {
    /**
     * Retorna o identificador do fluxo do pacote (FID).
     */
    return fid;
}

uint8_t PacoteSlow::getFo() const {
    /**
     * Retorna o offset do fluxo do pacote (FO).
     */
    return fo;
}

vector<uint8_t> PacoteSlow::getData() const {
    /**
     * Retorna os dados do pacote como um vetor de bytes.
     */
    return data;
}


bool PacoteSlow::adicionar4BytesAoPacote(vector<uint8_t>& pacote, uint32_t valor) {
    /**
     * Adiciona um valor de 4 bytes ao pacote.
     * "Todos os bits são little endian." [cite: 13]
     * So, send the least significant byte first.
     */
    for(int i = 0; i < 4; i++) {
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

bool PacoteSlow::adicionar2BytesAoPacote(vector<uint8_t>& pacote, uint16_t valor) {
    /**
     * Adiciona um valor de 2 bytes ao pacote.
     * "Todos os bits são little endian." [cite: 13]
     * So, send the least significant byte first.
     */
    for(int i = 0; i < 2; i++) {
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

vector<uint8_t> PacoteSlow::getPacote() {
    /**
     * Gera o pacote completo a partir dos campos definidos na classe.
     * "Todos os bits são little endian." [cite: 13]
     */
    vector<uint8_t> pacote;

    // SID (128 bits = 16 bytes) [cite: 13]
    for(int i = 0; i < 16; i++) {
        uint8_t byte = 0;
        for(int j = 0; j < 8; j++) {
            byte |= (sid[i * 8 + j] << j); // Assemble byte in little-endian order
        }
        pacote.push_back(byte);
    }

    // STTL (27 bits) + Flags (5 bits) = 32 bits = 4 bytes [cite: 14, 15]
    uint32_t sttlValue = sttl.to_ulong();
    uint32_t flagsValue = flags.to_ulong();
    // STTL occupies higher 27 bits, Flags occupy lower 5 bits
    uint32_t sttlFlags = (sttlValue << 5) | flagsValue;
    adicionar4BytesAoPacote(pacote, sttlFlags);

    // SeqNum (32 bits = 4 bytes) [cite: 15]
    adicionar4BytesAoPacote(pacote, seqNum);

    // AckNum (32 bits = 4 bytes) [cite: 15]
    adicionar4BytesAoPacote(pacote, ackNum);

    // Window (16 bits = 2 bytes) [cite: 16]
    adicionar2BytesAoPacote(pacote, window);

    // Fid (8 bits = 1 byte) + Fo (8 bits = 1 byte) [cite: 16]
    pacote.push_back(fid);
    pacote.push_back(fo);

    // Dados (max 1440 bytes) [cite: 17]
    pacote.insert(pacote.end(), data.begin(), data.end());

    return pacote;
}

PacoteSlow criarPacote(vector<uint8_t>& pacoteRecebido)
{
    PacoteSlow pacote;

    size_t tamanhoPacote = pacoteRecebido.size();

    cout << "Tamanho do pacote recebido: " << tamanhoPacote << " bytes" << endl;

    // Cabeçalho SLOW tem 32 bytes: 16 (SID) + 4 (STTL+Flags) + 4 (SeqNum) + 4 (AckNum) + 2 (Window) + 1 (Fid) + 1 (Fo) = 32 bytes [cite: 13, 14, 15, 16]
    if(tamanhoPacote < TAMANHO_CABECALHO_PACOTE) { // TAMANHO_CABECALHO_PACOTE is 32
        cerr << "Erro: Pacote recebido muito pequeno." << endl;
        return pacote; // Retorna um pacote vazio
    }

    // SID (16 bytes) [cite: 13]
    vector<uint8_t> sidBytes(pacoteRecebido.begin(), pacoteRecebido.begin() + 16);
    bitset<128> sidBits;
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 8; j++) {
            sidBits[i * 8 + j] = (sidBytes[i] >> j) & 0x01; // Reconstruct bitset in little-endian
        }
    }
    pacote.setSid(sidBits);

    // TTL (27 bits) + Flags (5 bits) (4 bytes) [cite: 14, 15]
    uint32_t sttlFlags = 0;
    for(int i = 0; i < 4; i++) {
        sttlFlags |= (pacoteRecebido[16 + i] << (i * 8)); // Reconstruct 32-bit value in little-endian
    }
    bitset<27> sttlBits((sttlFlags >> 5) & 0x07FFFFFF); // Extract higher 27 bits for TTL
    bitset<5> flagsBits(sttlFlags & 0x1F); // Extract lower 5 bits for flags
    pacote.setSttl(sttlBits);
    pacote.setFlags(flagsBits);

    // SeqNum (4 bytes) [cite: 15]
    uint32_t seqNum = 0;
    for(int i = 0; i < 4; i++) {
        seqNum |= (pacoteRecebido[20 + i] << (i * 8)); // Reconstruct 32-bit value in little-endian
    }
    pacote.setSeqNum(seqNum);

    // AckNum (4 bytes) [cite: 15]
    uint32_t ackNum = 0;
    for(int i = 0; i < 4; i++) {
        ackNum |= (pacoteRecebido[24 + i] << (i * 8)); // Reconstruct 32-bit value in little-endian
    }
    pacote.setAckNum(ackNum);

    // Window (2 bytes) [cite: 16]
    uint16_t window = 0;
    for(int i = 0; i < 2; i++) {
        window |= (pacoteRecebido[28 + i] << (i * 8)); // Reconstruct 16-bit value in little-endian
    }
    pacote.setWindow(window);
    
    // Fid (1 byte) + Fo (1 byte) [cite: 16]
    pacote.setFid(pacoteRecebido[30]);
    pacote.setFo(pacoteRecebido[31]);

    // Dados (remaining bytes) [cite: 17]
    vector<uint8_t> dados(pacoteRecebido.begin() + 32, pacoteRecebido.end());
    pacote.setData(dados, dados.size());

    return pacote;
}

void imprimirPacote(const PacoteSlow& pacote, string tipoPacote) {
    /**
     * Imprime os detalhes do pacote no console.
     */
    cout << "---------------------------------------------------" << endl;
    cout << "Detalhes do Pacote: " << tipoPacote << " =>" << endl;
    cout << "SID: ";
    vector<uint8_t> sid = pacote.getSid();
    for(int i = 0; i < 16; i++) {
        cout << static_cast<int>(sid[i]) << " ";
    }
    cout << endl;
    cout << "STTL: " << pacote.getSttl() << endl;
    cout << "Flags: " << pacote.getFlags() << endl; // Connect, Revive, ACK, Accept/Reject, More Bits [cite: 14]
    // Order of flags in bitset: bit 0 (LSB) is Connect, bit 1 Revive, bit 2 ACK, bit 3 Accept/Reject, bit 4 (MSB) More Bits [cite: 14]
    cout << " (C R A A/R MB)" << endl; // Clarify flag order
    cout << "SeqNum: " << pacote.getSeqNum() << endl;
    cout << "AckNum: " << pacote.getAckNum() << endl;
    cout << "Window: " << pacote.getWindow() << endl;
    cout << "FID: " << static_cast<int>(pacote.getFid()) << endl;
    cout << "FO: " << static_cast<int>(pacote.getFo()) << endl;


    vector<uint8_t> data = pacote.getData();
    cout << "Data: ";
    for(uint8_t byte : data) {
        cout << static_cast<char>(byte);
    }

    cout << endl;

    cout << "---------------------------------------------------" << endl << endl;
}