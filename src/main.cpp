#include <iostream>
#include "fragmentacao.hpp"

int main() {
    cout << "Iniciando fragmentação de dados..." << endl;

    vector<uint8_t> dados;
    for(int i = 0; i < 3000; i++)
    {
        dados.push_back(i / TAMANHO_MAXIMO_DADOS);
    }

    cout << "a" << endl;

    vector<vector<uint8_t>> dadosFragmentados = fragmentarDados(dados);

    printf("Dados fragmentados em %zu pacotes:\n", dadosFragmentados.size());
    for(size_t i = 0; i < dadosFragmentados.size(); i++) {
        printf("Pacote %zu: %u\n", i, dadosFragmentados[i][0]);
        printf("Tamanho do pacote %zu: %zu bytes\n", i, dadosFragmentados[i].size());
    }



    return 0;
}
