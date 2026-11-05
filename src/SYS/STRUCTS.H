#ifndef STRUCTS_H
#define STRUCTS_H

union Word {
    int w;
    unsigned char b[2];
};
struct Pair {
    int a;
    int b;
};

#endif
