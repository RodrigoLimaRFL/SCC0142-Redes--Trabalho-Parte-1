// socketUDP.cpp
#include "socketUDP.hpp"

using namespace std;

// Variáveis globais
int UDP_socket; // Socket UDP
sockaddr_in server_addr{}; // Endereço do servidor

bool startSocket(string hostname, int port) {
    /**
     * Função para iniciar o socket UDP.
     *
     * Params:
     * - hostname: Nome do host do servidor.
     * - port: Porta do servidor.
     */
    
    // LIMPA O ENDERECO DO SERVER
    memset(&server_addr, 0, sizeof(server_addr));
    // CRIA SOCKET     // IPv4 // // UDP //
    UDP_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if(UDP_socket < 0) {
        cout << "Erro ao criar o socket" << endl;
        return false;
    }

    // TRADUZ NOME DO SERVER PRA IP E PEGA OS DADOS DO SERVIDOR
    hostent* server = gethostbyname(hostname.c_str());
    if(server == nullptr) {
        cout << "Host não encontrado" << endl;
        return false;
    }

    // CONFIGURAR ENDEREÇO DO SERVER
    server_addr.sin_family = AF_INET; // Endereco tipo IPv4
    server_addr.sin_port = htons(port); // Conversao da porta para o formato de rede
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length); // IP do server

    // Definir timeout de 5 segundos para recepção
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt(UDP_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Erro ao configurar timeout de recepção");
        return false;
    }

    return true;
}

PacoteSlow sendReceive(PacoteSlow packet) {
    /**
     * Função para enviar e receber pacotes via socket UDP.
     *
     * Params:
     * - packet: Pacote a ser enviado.
     *
     * Returns:
     * - PacoteSlow: Pacote recebido como resposta.
     */

    // === SEND ===

    vector<uint8_t> packet_vector = packet.getPacote();

    /*cout << "pacote do getPacote" << endl;

    for(int j = 0; j < (int) packet_vector.size(); j++) {
        cout << "[" << static_cast<int>(packet_vector[j]) << " - (" << j << ")] ";
    }

    cout << endl;
    cout << "Tamanho " << packet_vector.size() << endl;
    */


    // uint8_t* packet_data = packet_vector.data();

    // Envio                   (socket)     (endereco e tamanho do pacote)       (destino)
    ssize_t sent_bytes = sendto(UDP_socket, packet_vector.data(), packet_vector.size(), 0, reinterpret_cast<sockaddr*>(&server_addr),
                                  sizeof(server_addr));

    // Verifica se houve erro no envio e cancela a conexão se necessário
    if(sent_bytes < 0) {
        cout << "Erro ao enviar o pacote" << endl;
        close(UDP_socket);
        return {};
    }

    cout << "Pacote enviado com sucesso (" << sent_bytes << " bytes)\n";
    cout << "Esperando resposta..." << endl;



    //  === RECEIVE ===  
    // Cria um buffer para armazenar a resposta
    uint8_t response[1472]; // Max packet size is 1472 bytes [cite: 13]
    memset(response, 0, sizeof(response));

    // Recebe a resposta do servidor
    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);
    ssize_t received_bytes = recvfrom(UDP_socket, response, 1472, 0,
                                (sockaddr*)&from_addr, &from_len);

    // Verifica se houve erro na recepção
    if (received_bytes < 0) {
        perror("Erro ao receber pacote");
    } else {
        cout << "Resposta recebida com " << received_bytes << " bytes\n";

    }

    // Exibe o conteúdo da resposta em formato de caracteres e inteiros
    /*cout << "Response in Chars:" << endl;
    for(int i = 0; i < (int) received_bytes; i++) {
        cout << (response[i]) << " ";
    }

    cout << endl << "Response in Ints:" << endl;
    for(int i = 0; i < received_bytes; i++) {
        cout << "[" << static_cast<int>(response[i]) << " - (" << i << ")] ";
    }

    // O byte 16 contém as flags nos seus 5 bits menos significativos. [cite: 13, 14]
    uint8_t flags_byte = response[16]; 

    // Imprime as flags da menos significativa (MB) para a mais significativa (C) [cite: 14]
    cout << "\nFlags (na ordem MB, A/R, ACK, R, C): "; 
    for(int i = 0; i < 5; i++) {
        cout << ((flags_byte >> i) & 1) << " ";
    }
    cout << endl;*/

    // Cria um vetor de bytes a partir da resposta recebida
    // e converte para um objeto PacoteSlow
    vector<uint8_t> response_vector(response, response + received_bytes);
    PacoteSlow pacoteRecebido = criarPacote(response_vector);

    // Retorna o pacote recebido
    return pacoteRecebido;
}


void closeConnection() {
    /**
     * Função para fechar a conexão do socket UDP.
     */

    // Fecha a Conexão do socket
    cout << "Fechando conexão do socket UDP..." << endl;
    close(UDP_socket);

    return;
}