#include "fragmentacao.hpp"

mutex janela_mutex; // Mutex para proteger o acesso à janela de envio
condition_variable janela_cv; // Variável de condição para sincronização de threads

// Variáveis globais para controle da janela de envio e pacotes enviados
int tamanhoJanelaTotal = 0;
int janelaEfetivaDisponivel = 0;

// Mapeia seqNum → tamanho do pacote enviado
map<uint32_t, int> pacotesEnviados;


template <size_t N>
bitset<N> vectorToBitset(const vector<uint8_t>& vec) {
    /*
    * Converte um vetor de bytes (uint8_t) em um bitset de tamanho N.
    * 
    * Params:
    * - vec: Vetor de bytes a ser convertido.
    * 
    * Returns:
    * - Um bitset de tamanho N representando os bits do vetor.
    */
    bitset<N> bits;

    size_t bitIndex = 0;
    for (size_t byteIndex = 0; byteIndex < vec.size() && bitIndex < N; ++byteIndex) {
        uint8_t byte = vec[byteIndex];
        for (int i = 0; i < 8 && bitIndex < N; ++i) {
            bool bit = (byte >> i) & 1;
            bits[bitIndex++] = bit;
        }
    }

    return bits;
}

template <size_t N>
bitset<N> uInt32ToBitset(uint32_t value) {
    /*
    * Converte um valor uint32_t em um bitset de tamanho N.
    * Params:
    * - value: Valor a ser convertido.
    * Returns:
    * - Um bitset de tamanho N representando os bits do valor.
     */
    bitset<N> bits;
    for (size_t i = 0; i < N && value > 0; ++i) {
        bits[i] = (value & 1);
        value >>= 1;
    }
    return bits;
}

vector<vector<uint8_t>> fragmentarDados(vector<uint8_t> dados, Session& session)
{
    /**
     * Fragmenta os dados em pacotes de tamanho máximo definido.
     * 
     * Params:
     * - dados: Dados a serem fragmentados.
     * - session: Sessão atual para obter o tamanho da janela e outros parâmetros.
     * 
     * Returns:
     * - Um vetor de vetores de bytes, onde cada vetor representa um pacote fragmentado.
     */
    int numBytes = dados.size();

    int windowSize = session.getWindow();
    int tamanhoFragmentacao = min(TAMANHO_MAXIMO_DADOS, windowSize - TAMANHO_CABECALHO_PACOTE);

    int numPacotes = (numBytes / tamanhoFragmentacao);
    bool pacoteExtra = (numBytes % tamanhoFragmentacao) > 0;
    vector<vector<uint8_t>> dadosParaPacotes;

    if(pacoteExtra) numPacotes++;

    for(int i = 0; i < numPacotes; i++)
    {
        vector<uint8_t> pacote;
        int inicio = i * tamanhoFragmentacao;
        int fim = inicio + tamanhoFragmentacao;

        if(fim > numBytes) {
            fim = numBytes; // Ajusta o fim para não ultrapassar o tamanho dos dados
        }

        for(int j = inicio; j < fim; j++) {
            pacote.push_back(dados[j]);
        }

        dadosParaPacotes.push_back(pacote);
    }

    return dadosParaPacotes;
}

thread iniciarThreadAck(Session& session) {
    /**
     * Inicia uma thread que processa ACKs recebidos e atualiza a janela de envio.
     * 
     * Params:
     * - session: Sessão atual para obter o número de sequência e reconhecimento.
     * Returns:
     * - Uma thread que processa ACKs e atualiza a janela de envio.
     * 
     */
    return thread([&session]() {
        while (!pararThreads.load()) {
            PacoteSlow ack;
            threadReceber(ack);

            uint32_t ackNum = ack.getAckNum();

            {
                lock_guard<mutex> lock(janela_mutex);

                auto it = pacotesEnviados.begin();
                while (it != pacotesEnviados.end()) {
                    if (it->first < ackNum) {
                        janelaEfetivaDisponivel += it->second;
                        it = pacotesEnviados.erase(it);
                    } else {
                        ++it;
                    }
                }
            }

            session.setAckNum(ackNum);
            janela_cv.notify_all();
        }
    });
}


uint8_t fragmentId = 0; // ID do fragmento

bool fragmentarEEnviarDados(
    vector<uint8_t> uuid, 
    uint32_t sttl, 
    uint32_t ultimoSeqNum, 
    uint32_t ultimoAckNum, 
    uint16_t window, 
    vector<uint8_t> dados,
    TiposDeEnvio tipoEnvio,
    Session& session
) {
    /**
     * Fragmenta os dados e envia pacotes de acordo com a sessão e tipo de envio.
     * 
     * Params:
     * - uuid: UUID do cliente.
     * - sttl: Tempo de vida do pacote.
     * - ultimoSeqNum: Último número de sequência enviado.
     * - ultimoAckNum: Último número de reconhecimento recebido.
     * - window: Tamanho da janela de envio.
     * - dados: Dados a serem fragmentados e enviados.
     * - tipoEnvio: Tipo de envio (DATA ou REVIVE).
     * 
     * Returns:
     * - true se os dados foram fragmentados e enviados com sucesso, false caso contrário.
     * 
     */
    vector<vector<uint8_t>> pacotes = fragmentarDados(dados, session);

    tamanhoJanelaTotal = session.getWindow();
    janelaEfetivaDisponivel = tamanhoJanelaTotal;

    bitset<128> uuidBits = vectorToBitset<128>(uuid);
    bitset<27> sttlBits = uInt32ToBitset<27>(sttl);
    
    for (size_t i = 0; i < pacotes.size(); i++) {
        bool maisDados = (i < pacotes.size() - 1);
        if (session.getSTTL() <= 0) {
            cout << "Sessão expirada. Não é possível enviar pacotes." << endl;
            return false;
        }

        PacoteSlow pacote = setPacketToSend(
            uuidBits,
            sttlBits,
            ultimoSeqNum,
            ultimoAckNum,
            tamanhoJanelaTotal,
            maisDados,
            pacotes[i]
        );
        pacote.setFid(fragmentId);
        pacote.setFo(i);
        int tamanhoPacote = pacote.getPacote().size();

        {
            unique_lock<mutex> lock(janela_mutex);
            janela_cv.wait(lock, [&]() {
                return janelaEfetivaDisponivel >= tamanhoPacote;
            });

            janelaEfetivaDisponivel -= tamanhoPacote;
            pacotesEnviados[ultimoSeqNum] = tamanhoPacote;
        }

        imprimirPacote(pacote, "Pacote de Dados");

        if (tipoEnvio == TiposDeEnvio::REVIVE && i == 0) {
            // Envia o pacote e aguarda a resposta imediatamente
            PacoteSlow resposta = sendReceive(pacote);
            imprimirPacote(resposta, "Resposta do Revive");

            bitset<5> flagsResposta = resposta.getFlags();
            if (!flagsResposta.test(3)) { // bit 3 == 0 => rejeitado
                cout << "Servidor rejeitou o revive. Encerrando programa." << endl;
                exit(EXIT_FAILURE);
            } else {
                cout << "Servidor aceitou o revive. Continuando o envio." << endl;
            }
        } else {
            // Envia pacote de forma assíncrona
            thread(threadEnviar, pacote).detach();
        }

        ultimoSeqNum++;
    }

    fragmentId++;

    return true;
}
