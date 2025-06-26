// uuid.cpp
#include "uuid.hpp"

using namespace std;

void Uuidv8::genCustom() {
    // Setup da geração aleatória dos bits
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, 1);
    
    // custom_a: bits 0-47 [cite: 18]
    for(int i = 0; i < 48; i++) {
        a[i] = dist(gen);
    }
    // custom_b: bits 52-63 [cite: 18]
    for(int i = 0; i < 12; i++) {
        b[i] = dist(gen);
    }
    // custom_c: bits 66-127 [cite: 18]
    for(int i = 0; i < 62; i++) {
        c[i] = dist(gen);
    }
}

void Uuidv8::genVar() {
    // "var e ver devem ter valores conforme especificado no RFC9562" [cite: 18]
    // For UUIDv8, the 'var' field should be 10_2 (bits 64-65) for RFC4122 compatibility
    // Although RFC9562 redefines UUIDv8 as "custom-defined", it usually aligns with RFC4122 variant.
    // Assuming '10' for the 'var' bits (bits 64 and 65)
    var[0] = 1; // Bit 64
    var[1] = 0; // Bit 65
}

void Uuidv8::genVer() {
    // "var e ver devem ter valores conforme especificado no RFC9562" [cite: 18]
    // For UUIDv8, the 'ver' field should be 1000_2 (bits 48-51) for custom-defined.
    ver[0] = 1; // Bit 48
    ver[1] = 0; // Bit 49
    ver[2] = 0; // Bit 50
    ver[3] = 0; // Bit 51
}

void Uuidv8::assembleUuid() {
    // Assemble UUID according to the format in the PDF [cite: 18]
    // "custom_a: bits 0-47" [cite: 18]
    for (int i = 0; i < 48; ++i)
        uuid[i] = a[i];

    // "ver: bits 48-51" [cite: 18]
    for (int i = 0; i < 4; ++i)
        uuid[48 + i] = ver[i];

    // "custom_b: bits 52-63" [cite: 18]
    for (int i = 0; i < 12; ++i)
        uuid[52 + i] = b[i];

    // "var: bits 64-65" [cite: 18]
    for (int i = 0; i < 2; ++i)
        uuid[64 + i] = var[i];

    // "custom_c: bits 66-127" [cite: 18]
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
    // This function seems to create a bitset with bits in reverse order,
    // which might be for specific endianness handling if the standard bitset
    // doesn't directly map to the byte order needed.
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