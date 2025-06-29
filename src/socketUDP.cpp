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
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    if (setsockopt(UDP_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        perror("Erro ao configurar timeout de recepção");
        return false;
    }

    return true;
}

// Controle de parada
atomic<bool> pararThreads(false);

// Thread de envio
void threadEnviar(PacoteSlow packet) {
    /**
     * Função para enviar pacotes via socket UDP.
     * 
     * Params:
     * - packet: Pacote a ser enviado.
     */
    vector<uint8_t> packet_vector = packet.getPacote();
    ssize_t sent_bytes = sendto(UDP_socket, packet_vector.data(), packet_vector.size(), 0, 
                                reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

    if (sent_bytes < 0) {
        cout << "Erro ao enviar o pacote" << endl;
        close(UDP_socket);
        pararThreads = true; // Sinaliza erro
    } else {
        cout << "Pacote enviado com sucesso (" << sent_bytes << " bytes)\n";
    }
}

// Thread de recepção
void threadReceber(PacoteSlow& pacoteRecebido) {
    /**
     * Função para receber pacotes via socket UDP.
     * 
     * Params
     * - pacoteRecebido: Referência para o pacote recebido que será preenchido.
     */
    uint8_t response[1472] = {};
    sockaddr_in from_addr{};
    socklen_t from_len = sizeof(from_addr);

    ssize_t received_bytes = recvfrom(UDP_socket, response, 1472, 0,
                                        (sockaddr*)&from_addr, &from_len);

    if (received_bytes < 0) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            // Timeout: só ignorar
            return;
        } else {
            perror("Erro ao receber pacote");
            pararThreads = true;  // Erro fatal opcional
            return;
        }
    } else {
        vector<uint8_t> response_vector(response, response + received_bytes);
        pacoteRecebido = criarPacote(response_vector);
        cout << "Resposta recebida com " << received_bytes << " bytes\n";
        imprimirPacote(pacoteRecebido, "Pacote Recebido");
    }

    return;
}

// Versão com threads
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
    PacoteSlow pacoteRecebido;

    thread t1(threadEnviar, packet);
    thread t2(threadReceber, ref(pacoteRecebido));

    t1.join(); // Aguarda envio
    t2.join(); // Aguarda recepção

    if (pararThreads) {
        cout << "Erro durante envio ou recepção" << endl;
        return {};
    }

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
