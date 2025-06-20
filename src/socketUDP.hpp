#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include "pacoteSlow.hpp"

using namespace std;

void startSocket(string hostname, int port);
PacoteSlow sendReceive(PacoteSlow packet);
void closeConnection();


#endif