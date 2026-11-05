/**
 * @file NEEDLE.H
 * @brief Speedometer and tachometer needle endpoint tables.
 */

#ifndef NEEDLE_H
#define NEEDLE_H

/** @brief Dashboard needle endpoint offset from its gauge center, in pixels. */
struct NeedlePoint {
    signed char x; /**< Horizontal offset, positive rightwards. */
    signed char y; /**< Vertical offset, positive upwards. */
};

/** @brief Endpoint plane in the shared speedometer and tachometer lookup. */
enum NeedleEnd {
    NEEDLE_TIP,      /**< Outer end of the drawn needle line. */
    NEEDLE_HUB,      /**< Inner end of the drawn needle line. */
    NEEDLE_END_COUNT /**< Number of endpoint planes. */
};

/** Inclusive lookup index limit for both dashboard needles. */
#define NEEDLE_MAX_POS 0x82

extern struct NeedlePoint far g_needlePts[NEEDLE_END_COUNT][NEEDLE_MAX_POS + 1];

#endif
