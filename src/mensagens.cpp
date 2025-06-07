#include "mensagens.hpp"

using namespace std;


PacoteSlow connect(uint16_t tamBufferRecebimento) {

    PacoteSlow pacoteConnect;

    // NIL UUID
    // STTL 0

    bitset<5> flags('00001'); // flags: connect(1)
    pacoteConnect.setFlags(flags);

    //seqNum 0
    // ackNum 0

    pacoteConnect.setWindow(tamBufferRecebimento); // window

    // fid 0
    // fo 0
    // data.clear

    return pacoteConnect;
}
