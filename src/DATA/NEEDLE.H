/**
 * @file NEEDLE.H
 * @brief Speedometer and tachometer needle endpoint tables.
 */

#ifndef NEEDLE_H
#define NEEDLE_H

struct NeedlePoint {
    signed char x, y;
};

/** @brief Endpoint plane in the shared speedometer and tachometer lookup. */
enum NeedleEnd {
    NEEDLE_TIP, /**< Outer end of the drawn needle line. */
    NEEDLE_HUB  /**< Inner end of the drawn needle line. */
};

/** Inclusive lookup index limit for both dashboard needles. */
#define NEEDLE_MAX_POS 0x82

extern struct NeedlePoint far g_needlePts[2][NEEDLE_MAX_POS + 1];

#endif
