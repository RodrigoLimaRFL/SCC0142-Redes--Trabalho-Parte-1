#ifndef UUIDV8
    #define UUIDV8

    #include <bitset>
    #include <random>
    #include <iostream>

    using namespace std;

    class Uuidv8 {
    private:
        bitset<128> uuid;
        bitset<48> a;
        bitset<12> b;
        bitset<62> c;
        bitset<4> ver;
        bitset<2> var;

        bitset<128> rev_uuid;

        void assembleUuid();

    public:
        Uuidv8();

        void genCustom();
        void genVer();
        void genVar();
        void print();
        bitset<128> getUuid();

        void assembleReverseUuid();
        void printRev();
        bitset<128> getRevUuid();
    };

#endif
