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
        // Itera por cada um dos 8 bits que formam o byte atual
        for(int j = 0; j < 8; j++) {
            // Se o bit na posição correspondente do bitset<128> for 1...
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
     */
    for(int i = 0; i < 4; i++) { // Envia o byte menos significativo primeiro
        pacote.push_back((valor >> (i * 8)) & 0xFF);
    }
    return true;
}

bool PacoteSlow::adicionar2BytesAoPacote(vector<uint8_t>& pacote, uint16_t valor) {
    /**
     * Adiciona um valor de 2 bytes ao pacote.
     */
    for(int i = 0; i < 2; i++) { // Envia o byte menos significativo primeiro
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

PacoteSlow criarPacote(vector<uint8_t>& pacoteRecebido)
{
    PacoteSlow pacote;

    size_t tamanhoPacote = pacoteRecebido.size();

    cout << "Tamanho do pacote recebido: " << tamanhoPacote << " bytes" << endl;

    if(tamanhoPacote < TAMANHO_CABECALHO_PACOTE) {
        cerr << "Erro: Pacote recebido muito pequeno." << endl;
        return pacote; // Retorna um pacote vazio
    }

    // SID
    vector<uint8_t> sidBytes(pacoteRecebido.begin(), pacoteRecebido.begin() + 16);
    bitset<128> sidBits;
    for(int i = 0; i < 16; i++) {
        for(int j = 0; j < 8; j++) {
            sidBits[i * 8 + j] = (sidBytes[i] >> j) & 0x01;
        }
    }
    pacote.setSid(sidBits);

    // TTL + Flags
    uint32_t sttlFlags = 0;
    for(int i = 0; i < 4; i++) {
        sttlFlags |= (pacoteRecebido[16 + i] << (i * 8));
    }
    bitset<27> sttlBits((sttlFlags >> 5) & 0x07FFFFFF); // Extrai os 27 bits de TTL
    bitset<5> flagsBits(sttlFlags & 0x1F); // Extrai os 5 bits de flags
    pacote.setSttl(sttlBits);
    pacote.setFlags(flagsBits);

    // SeqNum
    uint32_t seqNum = 0;
    for(int i = 0; i < 4; i++) {
        seqNum |= (pacoteRecebido[20 + i] << (i * 8));
    }
    pacote.setSeqNum(seqNum);

    // AckNum
    uint32_t ackNum = 0;
    for(int i = 0; i < 4; i++) {
        ackNum |= (pacoteRecebido[24 + i] << (i * 8));
    }
    pacote.setAckNum(ackNum);

    // Window
    uint16_t window = 0;
    for(int i = 0; i < 2; i++) {
        window |= (pacoteRecebido[28 + i] << (i * 8));
    }
    pacote.setWindow(window);
    
    // Fid + Fo
    pacote.setFid(pacoteRecebido[30]);
    pacote.setFo(pacoteRecebido[31]);

    // Dados
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
    cout << "Flags: " << pacote.getFlags() << endl;
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