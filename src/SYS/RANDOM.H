/**
 * @file RANDOM.H
 * @brief Bounded game random numbers.
 */

#ifndef RANDOM_H
#define RANDOM_H

struct RandomState {
    int a;
    int b;
    int c;
};

int rn_int(int n);

#endif
