#include <iostream>
#include "pacoteSlow.hpp"
#include "socketUDP.hpp"
#include "mensagens.hpp"

void printPacoteSlow(PacoteSlow pacote);

int main() {

    PacoteSlow pacote = connect(1);

    createSocket(pacote, "slow.gmelodie.com", 7033);


    return 0;
}

void printPacoteSlow(PacoteSlow pacote) {

}
