#include "mensagens.hpp"

PacoteSlow Disconnect(bitset<128> uuid, bitset<27> sttl, uint32_t seqnum, uint32_t acknum) {
    PacoteSlow pacote;

    // seta o sid recebido da central
    pacote.setSid(uuid);
    // seta o sttl recebido da central
    pacote.setTtl(sttl);

    // define as flags para:
    /*
    Connect: 1
    Revive: 1
    Ack: 1
    Accept/Reject: 0
    More Bits: 0
    */
    bitset<5> flags;
    flags.reset();

    flags[0] = 1;
    flags[1] = 1;
    flags[2] = 1;

    pacote.setFlags(flags);

    // define o seqnum (proximo)
    pacote.setSeqNum(seqnum);
    // define o acnum (seknum anterior)
    pacote.setAckNum(acknum);

    // Outros campos padrão
    pacote.setWindow(0);
    pacote.setFid(0);
    pacote.setFo(0);

    // Data: inexistente
    pacote.setData({}, 0);

    return pacote;
}