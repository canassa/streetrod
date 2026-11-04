/* RANDOM.H - declarations of what RANDOM.C defines. */

#ifndef RANDOM_H
#define RANDOM_H
#include "STRUCTS.H"

struct RandomState {
    int a;
    int b;
    int c;
};

int rn_int(int n);

#endif
