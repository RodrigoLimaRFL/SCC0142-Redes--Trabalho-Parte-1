#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <map>

#include "fragmentacao.hpp"
#include "session.hpp"
#include "processos/3wayConnect.hpp"

// Variáveis globais (declaradas externamente em fragmentacao.cpp)
extern std::mutex janela_mutex;
extern std::condition_variable janela_cv;
extern int tamanhoJanelaTotal;
extern int janelaEfetivaDisponivel;
extern std::map<uint32_t, int> pacotesEnviados;
extern std::atomic<bool> pararThreads;

// Protótipo da função para iniciar a thread de ACK
std::thread iniciarThreadAck(Session& session);

// Protótipo da threadReceber (implementação na socketUDP.cpp)
void threadReceber(PacoteSlow& pacote);

int main() {
    Session session;
    PacoteSlow response;

    // Inicia o socket UDP
    if(!startSocket("slow.gmelodie.com", 7033)) {
        std::cout << "Erro ao iniciar o socket UDP. Encerrando programa..." << std::endl;
        return 1;
    }

    // Envia um pacote de conexão para o servidor
    PacoteSlow connectPacket = createConnect(1472);
    imprimirPacote(connectPacket, "Pacote de Conexão");
    response = sendReceive(connectPacket);
    imprimirPacote(response, "Resposta do Pacote de Conexão");

    // Atualiza a sessão com os valores recebidos
    session.setValues(response);

    // Inicializa variáveis de janela
    tamanhoJanelaTotal = session.getWindow();
    janelaEfetivaDisponivel = tamanhoJanelaTotal;

    // Sinaliza que threads podem rodar
    pararThreads = false;

    // Cria a thread que processa ACKs e atualiza janela
    std::thread ackThread = iniciarThreadAck(session);

    bool disconnected = false;

    while (true) {
        if (!disconnected) {
            std::cout << "Digite 'enviar' para enviar dados ou 'desconectar' para encerrar a conexão: ";
        } else {
            std::cout << "Desconectado. Digite 'enviar' para reestabelecer a conexão ou 'finalizar' para encerrar o programa: ";
        }

        std::string command;
        std::cin >> command;

        if (command == "enviar") {
            std::cout << "Digite os dados a serem enviados: ";
            std::string inputData;
            std::cin.ignore();
            std::getline(std::cin, inputData);

            std::vector<uint8_t> dataToSend(inputData.begin(), inputData.end());

            TiposDeEnvio tipoEnvio = TiposDeEnvio::DATA;
            if (disconnected) {
                tipoEnvio = TiposDeEnvio::REVIVE;
            }

            bool enviado = fragmentarEEnviarDados(
                session.getUUID(),
                session.getSTTL(),
                session.getSeqNum(),
                session.getAckNum(),
                tamanhoJanelaTotal,
                dataToSend,
                tipoEnvio,
                session
            );

            if (!enviado) {
                std::cout << "Erro ao enviar os dados. Encerrando programa..." << std::endl;
                break;
            }

            disconnected = false;
            continue;
        }

        if (command == "desconectar") {
            PacoteSlow disconnectPacket = createDisconnect(session.getUUIDBits(), session.getSTTL(), session.getSeqNum(), session.getAckNum());
            imprimirPacote(disconnectPacket, "Pacote de Desconexão");
            PacoteSlow pacoteRespostaDisconnect = sendReceive(disconnectPacket);
            imprimirPacote(pacoteRespostaDisconnect, "Resposta do Pacote de Desconexão");

            disconnected = true;
            std::cout << "Desconectado com sucesso." << std::endl;
            continue;
        }

        if (command == "finalizar" && disconnected) {
            std::cout << "Obrigado por usar o programa. Pelo menos cinco bola!" << std::endl;
            break;
        }

        std::cout << "Comando inválido. Tente novamente." << std::endl;
    }

    // Finaliza thread de ACKs
    pararThreads = true;
    janela_cv.notify_all(); // para desbloquear espera se houver
    ackThread.join();

    closeConnection();

    return 0;
}
