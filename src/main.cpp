#include <iostream>
#include "pacoteSlow.hpp"
#include "socketUDP.hpp"
#include "mensagens.hpp"

void printPacoteSlow(PacoteSlow pacote);

int main() {

    PacoteSlow teste = connect(1);

    // for(int i =0; i < bytes.size(); i++) {
    //     cout << static_cast<int>(bytes[i]) << " " << i;
    // }

    createSocket(teste ,"slow.gmelodie.com", 7033);


    return 0;
}

void printPacoteSlow(PacoteSlow pacote) {

}
