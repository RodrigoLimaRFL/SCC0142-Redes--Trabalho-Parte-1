#include <iostream>
#include "pacoteSlow.hpp"
#include "socketUDP.hpp"
#include "mensagens.hpp"

void printPacoteSlow(PacoteSlow pacote);

int main() {

    PacoteSlow pacote = connect(1);

    startSocket("slow.gmelodie.com", 7033);
    sendReceive(pacote);

    closeConnection();
    return 0;
}

void printPacoteSlow(PacoteSlow pacote) {

}
