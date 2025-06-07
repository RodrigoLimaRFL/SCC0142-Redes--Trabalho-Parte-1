#include <iostream>
#include "pacoteSlow.hpp"

int main() {
    PacoteSlow pacote;

    // Criando valores fictícios para teste
    std::bitset<128> sid(std::string("0000000100000001000000010000000100000001000000010000000100000001"
                                     "0000000100000001000000010000000100000001000000010000000100000001"));
    std::bitset<27> ttl(std::string("100000000000000000000000001"));
    std::bitset<5> flags(std::string("10101"));
    uint32_t seqNum = 12345;
    uint32_t ackNum = 67890;
    uint16_t window = 4096;
    uint8_t fid = 5;
    uint8_t fo = 2;
    std::vector<uint8_t> dados = { 'H', 'e', 'l', 'l', 'o', ' ', 'S', 'L', 'O', 'W' };

    // Setando os campos
    pacote.setSid(sid);
    pacote.setTtl(ttl);
    pacote.setFlags(flags);
    pacote.setSeqNum(seqNum);
    pacote.setAckNum(ackNum);
    pacote.setWindow(window);
    pacote.setFid(fid);
    pacote.setFo(fo);
    pacote.setData(dados, dados.size());

    // Gerando o pacote final
    std::vector<uint8_t> pacoteFinal = pacote.getPacote();

    // Exibindo os bytes do pacote
    std::cout << "Pacote gerado (" << pacoteFinal.size() << " bytes):\n";
    for (uint8_t byte : pacoteFinal) {
        std::cout << (int)byte << " ";
    }
    std::cout << std::dec << "\n";

    return 0;
}
