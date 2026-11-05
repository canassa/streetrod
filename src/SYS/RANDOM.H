/**
 * @file RANDOM.H
 * @brief Bounded game random numbers.
 */

#ifndef RANDOM_H
#define RANDOM_H

/** @brief Signed recurrence states used to combine bounded random values. */
struct RandomState {
    int state30269; /**< Multiplier-171 recurrence modulo 30269. */
    int state30307; /**< Multiplier-172 recurrence modulo 30307. */
    int state30323; /**< Multiplier-170 recurrence modulo 30323. */
};

int rn_int(int n);

#endif
