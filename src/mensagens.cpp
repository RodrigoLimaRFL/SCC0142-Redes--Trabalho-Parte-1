#include "mensagens.hpp"

PacoteSlow Disconnect(bitset<128> uuid, bitset<27> sttl, uint32_t seqnum, uint32_t acknum) {
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

    // Seta o sid recebido da central
    pacote.setSid(uuid);
    // Seta o sttl recebido da central
    pacote.setSttl(sttl);

    /**
     * Definindo as flags do pacote:
     * - Connect: 1
     * - Revive: 1
     * - Ack: 1
     * - Accept/Reject: 0
     * - More Bits: 0
     */
    bitset<5> flags;
    flags.reset();

    flags[0] = 1;
    flags[1] = 1;
    flags[2] = 1;

    pacote.setFlags(flags);

    // Define o seqnum (proximo)
    pacote.setSeqNum(seqnum);
    // Define o acnum (seknum anterior)
    pacote.setAckNum(acknum);

    // Outros campos padrão
    pacote.setWindow(0);
    pacote.setFid(0);
    pacote.setFo(0);

    // Data: inexistente
    pacote.setData({}, 0);

    return pacote;
}

PacoteSlow sendData(bitset<128> uuid, bitset<27> sttl, uint32_t ultimoSeqNum, 
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
    cout << "-----------------------------" << endl;
    cout << "Enviando dados:" << endl;
    cout << "UUID: " << uuid.to_string() << endl;
    cout << "TTL: " << sttl.to_string() << endl;
    cout << "Último SeqNum: " << ultimoSeqNum << endl;
    cout << "Último AckNum: " << ultimoAckNum << endl;
    cout << "Janela: " << window << endl;
    cout << "Existem Mais Dados?: " << maisDados << endl;
    cout << "Dados: ";
    for (uint8_t byte : data) {
        cout << static_cast<char>(byte) << " ";
    }
    cout << endl;
    cout << "-----------------------------" << endl;


    int numBytes = data.size();

    PacoteSlow pacote;

    pacote.setSid(uuid);
    pacote.setSttl(sttl);
    if(maisDados)
    {
        pacote.setFlags(bitset<5>("10000")); // Connect, nesse caso nao tem mais bytes
    }
    else
    {
        pacote.setFlags(bitset<5>("10001")); // Connect e more bits
    }
    pacote.setSeqNum(ultimoSeqNum + 1); // Proximo número de sequência
    pacote.setAckNum(ultimoAckNum); // Número de reconhecimento atual
    pacote.setWindow(window); // Janela de bytes enviados
    pacote.setFid(0);
    pacote.setFo(0);
    pacote.setData(data, numBytes);

    return pacote;
}


PacoteSlow connect(uint16_t tamBufferRecebimento) {
    /** 
     * Função para criar um pacote de conexão.
     *  Params:
     * - tamBufferRecebimento: Tamanho do buffer de recebimento do peripheral.
     * 
     * Returns:
     * - PacoteSlow: Um objeto PacoteSlow contendo os dados do pacote de conexão.
     */

    PacoteSlow pacoteConnect;

    // NIL UUID
    // STTL 0

    // bitset<5> flags;
    // flags.reset();// flags: connect(1)

    // flags[0] = 1;

    std::bitset<5> flags(std::string("10000"));
    pacoteConnect.setFlags(flags);

    //seqNum 0
    // ackNum 0

    pacoteConnect.setWindow(tamBufferRecebimento); // window

    // fid 0
    // fo 0
    // data.clear

    return pacoteConnect;
}
