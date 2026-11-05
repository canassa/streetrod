#ifndef STRUCTS_H
#define STRUCTS_H

typedef int BOOL;
#define FALSE 0
#define TRUE 1

union Word {
    int w;
    unsigned char b[2];
};
struct Pair {
    int a;
    int b;
};

#endif
