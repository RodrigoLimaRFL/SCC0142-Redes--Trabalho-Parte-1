#include "session.hpp"

Session::Session() :running(false), uuid(16, 0), sttl(0), seqNum(0), ackNum(0) {}
/**
 * Construtor da classe Session.
 * Inicializa os campos da sessão com valores padrão:
 * - uuid: Identificador único da sessão (16 bytes) é inicializado com zeros.
 * - sttl: Tempo de vida da sessão (TTL) é inicializado com zero.
 * - seqNum: Número de sequência da sessão é inicializado com zero.
 * - ackNum: Número de reconhecimento da sessão é inicializado com zero.
 */

vector<uint8_t> Session::getUUID() {
    /**
     * Retorna o identificador único da sessão (UUID) como um vetor de bytes.
     */
    return uuid;
}

bitset<128> Session::getUUIDBits() {
    /**
     * Retorna o identificador único da sessão (UUID) como um bitset de 128 bits.
     */
    
    bitset<128> uuidBits;
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 8; j++) {
            uuidBits[i * 8 + j] = (uuid[i] >> j) & 0x01;
        }
    }
    return uuidBits;
}

bool Session::setUUID(const vector<uint8_t>& newUUID) {
    /**
     * Define o identificador único da sessão (UUID).
     * O UUID deve ser um vetor de 16 bytes.
     * 
     * Params:
     * - newUUID: Novo UUID a ser definido.
     * 
     * Returns:
     * - true se o UUID foi definido com sucesso, false se o tamanho do UUID for inválido.
     */
    if (newUUID.size() != 16) {
        return false;
    }
    uuid = newUUID;
    return true;
}

uint32_t Session::getSTTL() {
    /**
     * Retorna o tempo de vida da sessão (STTL).
     * 
     * Returns:
     * - uint32_t: O tempo de vida da sessão em segundos.
     */
    return sttl;
}

bool Session::setSTTL(uint32_t newSTTL) {
    /**
     * Define o tempo de vida da sessão (STTL).
     * 
     * Params:
     * - newSTTL: Novo tempo de vida a ser definido.
     * 
     * Returns:
     * - true se o STTL foi definido com sucesso.
     */
    sttl = newSTTL;
    startTTLTimer();
    return true;
}

uint32_t Session::getSeqNum() {
    /**
     * Retorna o número de sequência da sessão (SEQNUM).
     * 
     * Returns:
     * - uint32_t: O número de sequência da sessão.
     */
    return seqNum;
}

bool Session::setSeqNum(uint32_t newSeqNum) {
    /**
     * Define o número de sequência da sessão (SEQNUM).
     * 
     * Params:
     * - newSeqNum: Novo número de sequência a ser definido.
     * 
     * Returns:
     * - true se o SEQNUM foi definido com sucesso.
     */
    seqNum = newSeqNum;
    return true;
}

uint32_t Session::getAckNum() {
    /**
     * Retorna o número de reconhecimento da sessão (ACKNUM).
     * 
     * Returns:
     * - uint32_t: O número de reconhecimento da sessão.
     */
    return ackNum;
}

bool Session::setAckNum(uint32_t newAckNum) {
    /**
     * Define o número de reconhecimento da sessão (ACKNUM).
     * 
     * Params:
     * - newAckNum: Novo número de reconhecimento a ser definido.
     * 
     * Returns:
     * - true se o ACKNUM foi definido com sucesso.
     */
    ackNum = newAckNum;
    return true;
}

uint16_t Session::getWindow() {
    /**
     * Retorna a janela de controle da sessão (WINDOW).
     * 
     * Returns:
     * - uint16_t: A janela de controle da sessão.
     */
    return window;
}

bool Session::setWindow(uint16_t newWindow) {
    /**
     * Define a janela de controle da sessão (WINDOW).
     * 
     * Params:
     * - newWindow: Nova janela de controle a ser definida.
     * 
     * Returns:
     * - true se a janela foi definida com sucesso.
     */
    window = newWindow;
    return true;
}

bool Session::setValues(PacoteSlow pacote) {
    /**
     * Define os valores da sessão a partir de um pacote recebido.
     * 
     * Params:
     * - pacote: Pacote recebido contendo os valores a serem definidos na sessão.
     * 
     * Returns:
     * - true se os valores foram definidos com sucesso.
     */
    setUUID(pacote.getSid());
    setSTTL(pacote.getSttl());
    setSeqNum(pacote.getSeqNum());
    setAckNum(pacote.getAckNum());
    setWindow(pacote.getWindow());
    return true;
}

Session::~Session() {
    stopTTLTimer();  // Ensure the thread ends when the session is destroyed
}

void Session::startTTLTimer() {
    if (running) return; // Already running

    running = true;
    ttlThread = thread([this]() {
        while (running && sttl > 0) {
            this_thread::sleep_for(chrono::seconds(1));
            if (sttl > 0) {
                --sttl;
            }
        }
    });
}

void Session::stopTTLTimer() {
    running = false;
    if (ttlThread.joinable()) {
        ttlThread.join();
    }
}

