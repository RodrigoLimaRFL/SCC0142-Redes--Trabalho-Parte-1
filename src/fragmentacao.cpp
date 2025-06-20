#include "fragmentacao.hpp"

vector<vector<uint8_t>> fragmentarDados(vector<uint8_t> dados)
{
    int numBytes = dados.size();
    int numPacotes = (numBytes / TAMANHO_MAXIMO_DADOS);
    bool pacoteExtra = (numBytes % TAMANHO_MAXIMO_DADOS) > 0;
    vector<vector<uint8_t>> dadosParaPacotes;

    cout << "b" << endl;

    if(pacoteExtra) numPacotes++;

    for(int i = 0; i < numPacotes; i++)
    {
        vector<uint8_t> pacote;
        int inicio = i * TAMANHO_MAXIMO_DADOS;
        int fim = inicio + TAMANHO_MAXIMO_DADOS;

        if(fim > numBytes) {
            fim = numBytes; // Ajusta o fim para não ultrapassar o tamanho dos dados
        }

        for(int j = inicio; j < fim; j++) {
            pacote.push_back(dados[j]);
        }

        dadosParaPacotes.push_back(pacote);
    }

    cout << "c" << endl;

    return dadosParaPacotes;
}

bool fragmentarEEnviarDados(
    bitset<128> uuid, 
    bitset<27> sttl, 
    uint32_t ultimoSeqNum, 
    uint32_t ultimoAckNum, 
    uint16_t window, 
    vector<uint8_t> dados
) {
    vector<vector<uint8_t>> pacotes = fragmentarDados(dados);
    
    for(size_t i = 0; i < pacotes.size(); i++) {
        bool maisDados = (i < pacotes.size() - 1);
        PacoteSlow pacote = sendData(
            uuid, 
            sttl, 
            ultimoSeqNum, 
            ultimoAckNum, 
            window, 
            maisDados, 
            pacotes[i]
        );
        ultimoSeqNum++; // Incrementa o número de sequência para o próximo pacote
    }

    return true; // Retorna true se todos os pacotes foram enviados com sucesso
}