#include "uuid.hpp"

using namespace std;

void Uuidv8::genCustom() {
    // Setup da geração aleatória dos bits
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, 1);
    
    for(int i = 0; i < 48; i++) {
        a[i] = dist(gen);
    }
    for(int i = 0; i < 12; i++) {
        b[i] = dist(gen);
    }
    for(int i = 0; i < 62; i++) {
        c[i] = dist(gen);
    }
}

void Uuidv8::genVar() {
    var[0] = 1;
    var[1] = 0;
}

void Uuidv8::genVer() {
    ver[0] = 1;
    ver[1] = 0;
    ver[2] = 0;
    ver[3] = 0;
}

void Uuidv8::assembleUuid() {
    // a: bits 0..47
    for (int i = 0; i < 48; ++i)
        uuid[i] = a[i];

    // ver: bits 48..51
    for (int i = 0; i < 4; ++i)
        uuid[48 + i] = ver[i];

    // b: bits 52..63
    for (int i = 0; i < 12; ++i)
        uuid[52 + i] = b[i];

    // var: bits 64..65
    for (int i = 0; i < 2; ++i)
        uuid[64 + i] = var[i];

    // c: bits 66..127
    for (int i = 0; i < 62; ++i)
        uuid[66 + i] = c[i];
}

Uuidv8::Uuidv8() {
    genCustom();
    genVer();
    genVar();
    assembleUuid();
    assembleReverseUuid();
}

void Uuidv8::print() {
    for (int i = 0; i < 128; i++) {
        if ((i % 8 == 0) && i != 0) cout << ' ';
        cout << uuid[i];
    }
    cout << endl;
}

bitset<128> Uuidv8::getUuid() {
    return uuid;
}

void Uuidv8::assembleReverseUuid() {
    for(int i = 0; i < 128; i++) {
        rev_uuid[i] = uuid[127 - i];
    }
}

void Uuidv8::printRev() {
    for (int i = 0; i < 128; i++) {
        if ((i % 8 == 0) && i != 0) cout << ' ';
        cout << rev_uuid[i];
    }
    cout << endl;
}

bitset<128> Uuidv8::getRevUuid() {
    return rev_uuid;
}