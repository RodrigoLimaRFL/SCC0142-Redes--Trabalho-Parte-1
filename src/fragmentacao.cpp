#include "fragmentacao.hpp"

template <size_t N>
bitset<N> vectorToBitset(const std::vector<uint8_t>& vec) {
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
    bitset<N> bits;
    for (size_t i = 0; i < N && value > 0; ++i) {
        bits[i] = (value & 1);
        value >>= 1;
    }
    return bits;
}

vector<vector<uint8_t>> fragmentarDados(vector<uint8_t> dados)
{
    int numBytes = dados.size();
    int numPacotes = (numBytes / TAMANHO_MAXIMO_DADOS);
    bool pacoteExtra = (numBytes % TAMANHO_MAXIMO_DADOS) > 0;
    vector<vector<uint8_t>> dadosParaPacotes;

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

    return dadosParaPacotes;
}

uint8_t fragmentId = 0; // ID do fragmento

bool fragmentarEEnviarDados(
    vector<uint8_t> uuid, 
    uint32_t sttl, 
    uint32_t ultimoSeqNum, 
    uint32_t ultimoAckNum, 
    uint16_t window, 
    vector<uint8_t> dados
) {
    vector<vector<uint8_t>> pacotes = fragmentarDados(dados);

    bitset<128> uuidBits = vectorToBitset<128>(uuid);
    bitset<27> sttlBits = uInt32ToBitset<27>(sttl);
    
    for(size_t i = 0; i < pacotes.size(); i++) {
        bool maisDados = (i < pacotes.size() - 1);
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
    }

    cout << pacotes[0].size() << endl; // Exibe o tamanho do pacote enviado

    for(int i = 0; i < pacotes.size(); i++) {
        cout << "Pacote " << i << ": ";
        for(uint8_t byte : pacotes[i]) {
            cout << static_cast<int>(byte) << " "; // Exibe os bytes do pacote
        }
        cout << endl;
    }


    if(pacotes.size() > 1) {
        fragmentId++; // Se houve fragmentação, incrementa o ID do fragmento
    }

    return true; // Retorna true se todos os pacotes foram enviados com sucesso
}