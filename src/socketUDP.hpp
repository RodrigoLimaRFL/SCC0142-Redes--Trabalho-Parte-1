#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include <thread>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>


// Inclui o tipo PacoteSlow
#include "pacoteSlow.hpp"

using namespace std;

// Inicializa o socket
bool startSocket(string hostname, int port);

// Envia e recebe pacote usando threads
PacoteSlow sendReceive(PacoteSlow packet);

// Threads auxiliares (podem ser chamadas diretamente ou usadas em outro contexto)
void threadEnviar(PacoteSlow packet);
void threadReceber(PacoteSlow& pacoteRecebido);

// Fecha a conexão
void closeConnection();

// Variável global para controle das threads
extern atomic<bool> pararThreads;

#endif
