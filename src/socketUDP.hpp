#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include "pacoteSlow.hpp"

using namespace std;

bool startSocket(string hostname, int port);
PacoteSlow sendReceive(PacoteSlow packet);
void closeConnection();


#endif