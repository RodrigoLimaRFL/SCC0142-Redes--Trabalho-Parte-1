#ifndef FRAGMENTACAO_HPP
    #define FRAGMENTACAO_HPP
    #include "socketUDP.hpp"
    #include "mensagens.hpp"

    vector<vector<uint8_t>> fragmentarDados(vector<uint8_t> dados);
    bool fragmentarEEnviarDados(
        vector<uint8_t> uuid, 
        uint32_t sttl, 
        uint32_t ultimoSeqNum, 
        uint32_t ultimoAckNum, 
        uint16_t window, 
        vector<uint8_t> dados
    );
#endif