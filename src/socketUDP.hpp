#ifndef SOCKETUDP_HPP
#define SOCKETUDP_HPP

#include <string>
#include <cstddef>
#include <cstdint>
#include <bits/stdc++.h>
#include "pacoteSlow.hpp"

using namespace std;

void startSocket(string hostname, int port);
void sendReceive(PacoteSlow packet);
void closeConnection();


#endif