// main.cpp
#include <iostream>
#include <string> // For std::string
#include <vector> // For std::vector
#include <limits> // For std::numeric_limits

#include "fragmentacao.hpp"
#include "session.hpp"
#include "processos/3wayConnect.hpp" // Assumed to define sendReceive
#include "mensagens.hpp" // Assumed to define createConnect, createDisconnect, setPacketToSend
#include "socketUDP.hpp" // Assumed to define startSocket, closeConnection

int main() {
    Session session;
    PacoteSlow response;

    // Inicia o socket UDP
    if(!startSocket("slow.gmelodie.com", 7033)) // Port udp/7033 [cite: 12]
    {
        std::cout << "Erro ao iniciar o socket UDP. Encerrando programa..." << std::endl;
        return 1;
    }

    PacoteSlow connectPacket = createConnect(1472); // Max packet size is 1472 bytes 
    imprimirPacote(connectPacket, "Pacote de Conexão");
    response = sendReceive(connectPacket);
    imprimirPacote(response, "Resposta do Pacote de Conexão");

    // "Setup (central): ...flags: Accept (1) / Reject (0)"
    // Check if connection was accepted
    cout << response.getFlags() << endl;
    if (!response.getFlags().test(3)) { // Accept/Reject flag is at bit 3
        std::cout << "Conexão rejeitada pelo servidor. Encerrando programa..." << std::endl;
        closeConnection();
        return 1;
    }

    // Atualiza a sessão com os valores recebidos do pacote Setup (UUID, STTL, SeqNum, AckNum, Window)
    session.setValues(response);

    bool disconnected = false;

    while(true) {
        if(!disconnected) {
            std::cout << "Digite 'enviar' para enviar dados ou 'desconectar' para encerrar a conexão: ";
        } else {
            std::cout << "Desconectado. Digite 'enviar' para reestabelecer a conexão ou 'finalizar' para encerrar o programa: ";
        }
        std::string command;

        std::cin >> command;

        if(command == "enviar") { // Enviar dados
            std::cout << "Digite os dados a serem enviados: ";
            std::string inputData;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Limpa o buffer de entrada
            std::getline(std::cin, inputData);

            std::vector<uint8_t> dataToSend(inputData.begin(), inputData.end());
            
            TiposDeEnvio tipoEnvio = TiposDeEnvio::DATA;

            if(disconnected) {
                // Se estiver desconectado, envia um revive (0-way connect)
                // "Nesse caso a mensagem é a mesma da de dados, mas com a flag revive valendo 1." [cite: 29]
                tipoEnvio = TiposDeEnvio::REVIVE;
            }

            // The window size passed here is the peripheral's advertised window, assuming its buffer size.
            // The central's window will be received in ACKs.
            bool enviado = fragmentarEEnviarDados(
                session.getUUID(),
                session.getSTTL(),
                session.getSeqNum(), // Use current session seq num
                session.getAckNum(), // Use current session ack num
                1472, // Advertise our buffer size. This should ideally be dynamic based on actual buffer space. [cite: 13]
                dataToSend,
                tipoEnvio,
                session
            );

            if(!enviado) {
                std::cout << "Erro ao enviar os dados. Encerrando programa..." << std::endl;
                break;
            }

            disconnected = false; // Reseta o estado de desconexão após enviar os dados
            continue;
        }

        if(command == "desconectar") {
            // Envia um pacote de desconexão
            // "flags: Ack (1), Connect (1), Revive (1) Quando ambos connect e revive estiverem ligados, isso simboliza um disconnect." [cite: 25]
            // "window: 0" [cite: 25]
            PacoteSlow disconnectPacket = createDisconnect(session.getUUIDBits(), session.getSTTL(), session.getSeqNum(), session.getAckNum());
            
            imprimirPacote(disconnectPacket, "Pacote de Desconexão");
            
            PacoteSlow pacoteRespostaDisconnect = sendReceive(disconnectPacket);

            imprimirPacote(pacoteRespostaDisconnect, "Resposta do Pacote de Desconexão");
            
            // "Ack (central) tal qual 4" [cite: 25] - meaning it should be an ACK
            if (pacoteRespostaDisconnect.getFlags().test(2)) { // Check if ACK flag is set [cite: 14]
                disconnected = true;
                std::cout << "Desconectado com sucesso." << std::endl;
            } else {
                std::cout << "Falha ao desconectar. Resposta inesperada do servidor." << std::endl;
            }
            continue;
        }

        if(command == "finalizar" && disconnected) {
            std::cout << "Obrigado por usar o programa. Pelo menos cinco bola!" << std::endl;
            break;
        }

        std::cout << "Comando inválido. Tente novamente." << std::endl;
    }

    closeConnection();

    return 0;
}