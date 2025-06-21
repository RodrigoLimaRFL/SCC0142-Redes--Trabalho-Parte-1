#include <iostream>
#include "fragmentacao.hpp"
#include "session.hpp"
#include "processos/3wayConnect.hpp"

int main() {
    Session session;
    PacoteSlow response;

    // Inicia o socket UDP
    startSocket("slow.gmelodie.com", 7033);

    // Envia um pacote de conexão para o servidor
    response = sendReceive(createConnect(1));

    // Atualiza a sessão com os valores recebidos
    session.setValues(response);
    
    // vector<uint8_t> data = {'A', 'B', 'C', 'D', 'E'}; // Exemplo de dados

    // fragmentarEEnviarDados(
    //     session.getUUID(),
    //     session.getSTTL(),
    //     session.getSeqNum(),
    //     session.getAckNum(),
    //     1472,
    //     data
    // );

    return 0;
}

