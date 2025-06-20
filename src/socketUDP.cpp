#include <bits/stdc++.h>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <algorithm>

#include "socketUDP.hpp"

using namespace std;

int UDP_socket;
sockaddr_in server_addr{};

void startSocket(string hostname, int port) {
//    CRIA SOCKET         // IPv4 // // UDP //
    UDP_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(UDP_socket < 0) {
        cout << "Erro ao criar o socket" << endl;
        return;
    }

    // TRADUZ NOME DO SERVER PRA IP E PEGA OS DADOS DO SERVIDOR
    hostent* server = gethostbyname(hostname.c_str());
    if(server == nullptr) {
        cout << "Host não encontrado" << endl;
        return;
    }

    // CONFIGURAR ENDEREÇO DO SERVER
    server_addr.sin_family = AF_INET; // Endereco tipo IPv4
    server_addr.sin_port = htons(port); // Conversao da porta para o formato de rede
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length); // IP do server

    return;
}

void sendReceive(PacoteSlow packet) {
    // Send
    vector<uint8_t> packet_test = packet.getPacote();

    cout << "pacote do getPacote" << endl;

    for(int j = 0; j < packet_test.size(); j++) {
        cout << static_cast<int>(packet_test[j]) << "(" << j << ") ";
    }

    cout << endl;
    cout << "Tamanho " << packet_test.size() << endl;

    uint8_t* fodase = packet_test.data();

    // ENVIO                   (socket)  (endereco e tamanho do pacote)       (destino)
    ssize_t sent_bytes = sendto(UDP_socket, fodase, packet_test.size(), 0, reinterpret_cast<sockaddr*>(&server_addr),
                                  sizeof(server_addr));

    if(sent_bytes < 0) {
        cout << "Erro ao enviar o pacote" << endl;
        close(UDP_socket);
        return;
    }

    cout << "Pacote enviado com sucesso (" << sent_bytes << " bytes)\n";
    cout << "Esperando resposta..." << endl;

    //  RECEPCAO DOS PACOTES    
    uint8_t response[1472];

    // Limpa o buffer de resposta
    memset(response, 0, sizeof(response));


    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);

    ssize_t received_bytes = recvfrom(UDP_socket, response, 1472, 0,
                                (sockaddr*)&from_addr, &from_len);

    if (received_bytes < 0) {
        perror("Erro ao receber pacote");
    } else {
        std::cout << "Resposta recebida com " << received_bytes << " bytes\n";

    }

    cout << "Response in Chars:" << endl;
    for(int i = 0; i < received_bytes; i++) {
        cout << (response[i]) << " ";
    }

    cout << endl << "Response in Ints:" << endl;

    for(int i = 0; i < received_bytes; i++) {
        cout << static_cast<int>(response[i]) << "[" << i << "]" << " ";
    }

    // O byte 16 contém as flags nos seus 5 bits menos significativos.
    uint8_t flags_byte = response[16]; 

    // Imprime as flags da menos significativa (MB) para a mais significativa (C)
    cout << "\nFlags (na ordem MB, A/R, ACK, R, C): "; 
    for(int i = 0; i < 5; i++) {
        cout << ((flags_byte >> i) & 1) << " ";
    }
    cout << endl;

}


void closeConnection() {
    close(UDP_socket);

    return;
}
