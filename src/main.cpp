#include <iostream>
#include "fragmentacao.hpp"
#include "session.hpp"
#include "processos/3wayConnect.hpp"

int main() {
    Session session;
    PacoteSlow response;

    // Inicia o socket UDP
    if(!startSocket("slow.gmelodie.com", 7033))
    {
        cout << "Erro ao iniciar o socket UDP. Encerrando programa..." << endl;
        return 1;
    }

    // Envia um pacote de conexão para o servidor
    PacoteSlow connectPacket = createConnect(1);
    imprimirPacote(connectPacket, "Pacote de Conexão");
    response = sendReceive(connectPacket);
    imprimirPacote(response, "Resposta do Pacote de Conexão");

    // Atualiza a sessão com os valores recebidos
    session.setValues(response);

    bool disconnected = false;

    while(true) {
        if(!disconnected)
        {
            cout << "Digite 'enviar' para enviar dados ou 'desconectar' para encerrar a conexão: ";
        }
        else
        {
            cout << "Desconectado. Digite 'enviar' para reestabelecer a conexão ou 'finalizar' para encerrar o programa: ";
        }
        string command;

        cin >> command;

        if(command == "enviar") // Enviar dados
        {
            cout << "Digite os dados a serem enviados: ";
            string inputData;
            cin.ignore(); // Limpa o buffer de entrada
            getline(cin, inputData);

            vector<uint8_t> dataToSend(inputData.begin(), inputData.end());
            
            TiposDeEnvio tipoEnvio = TiposDeEnvio::DATA;

            if(disconnected)
            {
                // Se estiver desconectado, envia um revive
                tipoEnvio = TiposDeEnvio::REVIVE;
            }

            bool enviado = fragmentarEEnviarDados(
                session.getUUID(),
                session.getSTTL(),
                session.getSeqNum(),
                session.getAckNum(),
                1472,
                dataToSend,
                tipoEnvio,
                session
            );

            if(!enviado)
            {
                cout << "Erro ao enviar os dados. Encerrando programa..." << endl;
                break;
            }

            disconnected = false; // Reseta o estado de desconexão após enviar os dados

            continue;
        }

        if(command == "desconectar")
        {
            // Envia um pacote de desconexão
            PacoteSlow disconnectPacket = createDisconnect(session.getUUIDBits(), session.getSTTL(), session.getSeqNum(), session.getAckNum());
            
            imprimirPacote(disconnectPacket, "Pacote de Desconexão");
            
            PacoteSlow pacoteRespostaDisconnect = sendReceive(disconnectPacket);

            imprimirPacote(pacoteRespostaDisconnect, "Resposta do Pacote de Desconexão");
            
            disconnected = true;

            cout << "Desconectado com sucesso." << endl;

            continue;
        }

        if(command == "finalizar" && disconnected)
        {
            cout << "Obrigado por usar o programa. Pelo menos cinco bola!" << endl;
            break;
        }

        cout << "Comando inválido. Tente novamente." << endl;
    }

    closeConnection();

    return 0;
}

