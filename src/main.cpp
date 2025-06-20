#include <iostream>
#include "fragmentacao.hpp"
#include "session.hpp"

void printPacoteSlow(PacoteSlow pacote);

int main() {
    Session session;

    PacoteSlow pacote = connect(1);

    createSocket(pacote, "slow.gmelodie.com", 7033);

    vector<uint8_t> data = {'A', 'B', 'C', 'D', 'E'}; // Exemplo de dados

    fragmentarEEnviarDados(
        session.getUUID(),
        session.getSTTL(),
        session.getSeqNum(),
        session.getAckNum(),
        1472,
        data
    );

    return 0;
}

void printPacoteSlow(PacoteSlow pacote) {

}
