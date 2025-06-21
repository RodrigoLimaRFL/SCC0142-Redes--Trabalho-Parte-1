#include "fragmentacao.hpp"

template <size_t N>
bitset<N> vectorToBitset(const std::vector<uint8_t>& vec) {
    /*
    * Converte um vetor de bytes (uint8_t) em um bitset de tamanho N.
    * 
    * Params:
    * - vec: Vetor de bytes a ser convertido.
    * 
    * Returns:
    * - Um bitset de tamanho N representando os bits do vetor.
    */
    std::bitset<N> bits;

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
     * Fragmenta e envia os dados para o servidor.
     * 
     * Params:
     * - uuid: Identificador único da sessão.
     * - sttl: Tempo de vida da sessão.
     * - ultimoSeqNum: Último número de sequência enviado.
     * - ultimoAckNum: Último número de reconhecimento recebido.
     * - window: Tamanho da janela de controle.
     * - dados: Dados a serem enviados.
     * - tipoEnvio: Tipo de envio (DATA ou REVIVE).
     * - session: Sessão atual.
     * 
     * Returns:
     * - true se os pacotes foram enviados com sucesso, false caso contrário.
     */
    vector<vector<uint8_t>> pacotes = fragmentarDados(dados, session);

    bitset<128> uuidBits = vectorToBitset<128>(uuid);
    bitset<27> sttlBits = uInt32ToBitset<27>(sttl);
    
    // Realiza fragmentação e envio de dados
    for(size_t i = 0; i < pacotes.size(); i++) {
        bool maisDados = (i < pacotes.size() - 1);
        if (session.getSTTL() <= 0) {
            cout << "Sessão expirada. Não é possível enviar pacotes." << endl;
            return false; // Se o STTL for 0, não envia pacotes
        }
        PacoteSlow pacote = setPacketToSend(
            uuidBits, 
            sttlBits, 
            ultimoSeqNum, 
            ultimoAckNum, 
            window, 
            maisDados, 
            pacotes[i]
        );
        pacote.setFid(fragmentId); // Define o ID do fragmento como 0
        pacote.setFo(i); // Define o offset do fragmento como o índice do pacote
        ultimoSeqNum++; // Incrementa o número de sequência para o próximo pacote

        PacoteSlow pacoteResposta;

        if(tipoEnvio == TiposDeEnvio::REVIVE && i == 0) {
            bitset<5> flags = pacote.getFlags();
            flags.set(1, 1); // Define o bit de Revive
            pacote.setFlags(flags); // Atualiza as flags do pacote

            imprimirPacote(pacote, "Pacote de Revive");

            pacoteResposta = sendReceive(pacote);

            imprimirPacote(pacoteResposta, "Resposta do Revive");

            bitset<5> flagsResposta = pacoteResposta.getFlags();

            if(flagsResposta.test(3))
            {
                cout << "Servidor aceitou o revive." << endl;
            }
            else
            {
                cout << "Servidor rejeitou o revive." << endl;
                return false; // Se o revive for rejeitado, não continua o envio
            }
        }
        else
        {
            imprimirPacote(pacote, "Pacote de Dados");
            pacoteResposta = sendReceive(pacote);
            imprimirPacote(pacoteResposta, "Resposta do Pacote de Dados");
            uint32_t seqNumResposta = pacoteResposta.getSeqNum();
            session.setSeqNum(seqNumResposta); // Atualiza o número de sequência da sessão
        }

        // fazer os prints aqui
    }

    fragmentId++; // Fim dos fragmentos

    return true; // Retorna true se todos os pacotes foram enviados com sucesso
}