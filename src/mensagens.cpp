#include "mensagens.hpp"

PacoteSlow createDisconnect(bitset<128> uuid, bitset<27> sttl, uint32_t seqnum, uint32_t acknum) {
    /**
     * Função para enviar um pacote de desconexão.
     * 
     * Params:
     * - uuid: Identificador único do pacote (128 bits).
     * - sttl: Tempo de vida do pacote (27 bits).
     * - seqnum: Número de sequência do pacote (32 bits).
     * - acknum: Número de reconhecimento do pacote (32 bits).
     * 
     * Returns:
     * - PacoteSlow: Um objeto PacoteSlow contendo os dados do pacote de desconexão.
     */
    PacoteSlow pacote;

    /**
     * Definindo as flags do pacote:
     * - Connect: 1
     * - Revive: 1
     * - Ack: 1
     * - Accept/Reject: 0
     * - More Bits: 0
     */
    bitset<5> flags(string("11100"));
    pacote.setFlags(flags);

    // Define o SID recebido da central
    pacote.setSid(uuid);
    // Define o STTL recebido da central
    pacote.setSttl(sttl);
    // Define o SeqNum (proximo)
    pacote.setSeqNum(seqnum);
    // Define o AckNum (seknum anterior)
    pacote.setAckNum(acknum);

    // Define outros campos padrão
    pacote.setWindow(0);
    pacote.setFid(0);
    pacote.setFo(0);

    // Data: inexistente
    pacote.setData({}, 0);

    return pacote;
}

PacoteSlow setPacketToSend(bitset<128> uuid, bitset<27> sttl, uint32_t ultimoSeqNum, 
uint32_t ultimoAckNum, uint16_t window, bool maisDados, vector<uint8_t> data) {
    /**
     * Função para enviar dados de um pacote Slow.
     * 
     * Params:
     * - uuid: Identificador único do pacote (128 bits).
     * - sttl: Tempo de vida do pacote (27 bits).
     * - ultimoSeqNum: Último número de sequência enviado (32 bits).
     * - ultimoAckNum: Último número de reconhecimento recebido (32 bits).
     * - window: Número de bytes disponiveis na janela deslizante do peripheral
     * - maisDados: Se a mensagem atual possue dados adicionais a serem enviados
     * - data: Dados a serem enviados (vetor de bytes).
     * 
     * Returns:
     * - PacoteSlow: Um objeto PacoteSlow contendo os dados do pacote a ser enviado.
     */

    int numBytes = data.size();

    PacoteSlow pacote;

    pacote.setSid(uuid);
    pacote.setSttl(sttl);
    if(maisDados)
    {
        pacote.setFlags(bitset<5>("00101")); // Connect, Revive, Ack, Accept/Reject, More Bits
    }
    else
    {
        pacote.setFlags(bitset<5>("00100")); // Connect, Revive, Ack, Accept/Reject, More Bits
    }
    pacote.setSeqNum(ultimoSeqNum + 1); // Proximo número de sequência
    pacote.setAckNum(ultimoAckNum); // Número de reconhecimento atual
    pacote.setWindow(window); // Janela de bytes enviados
    pacote.setFid(0);
    pacote.setFo(0);
    pacote.setData(data, numBytes);

    return pacote;
}


PacoteSlow createConnect(uint16_t tamBufferRecebimento) {
    /** 
     * Função para criar um pacote de conexão.
     *  Params:
     * - tamBufferRecebimento: Tamanho do buffer de recebimento do peripheral.
     * 
     * Returns:
     * - PacoteSlow: Um objeto PacoteSlow contendo os dados do pacote de conexão.
     */

    // Inicializa o pacote de conexão
    PacoteSlow pacoteConnect;

    /**
     * Definindo as flags do pacote:
     * - Connect: 1
     * - Revive: 0
     * - Ack: 0
     * - Accept/Reject: 0
     * - More Bits: 0
     */
    bitset<5> flags(string("10000"));
    pacoteConnect.setFlags(flags);

    // Seta a Window do pacote de conexão
    pacoteConnect.setWindow(tamBufferRecebimento);

    return pacoteConnect;
}
