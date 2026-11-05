/**
 * @file CARHOP.H
 * @brief Roller-skating carhop animation.
 */

#ifndef CARHOP_H
#define CARHOP_H

#include "SPRITE.H"

struct Sheet {
    int id;
    int cols;
    int n;
};

enum CarhopMode {
    CARHOP_ENTER = 0,
    CARHOP_LEAVE = 1
};

extern char g_keepMask;
/**
 * @brief Animate the carhop skating in or out of the scene.
 *
 * Updates the car-animation backdrop with her standing pose or the cleared scene.
 *
 * @param mode Whether she arrives or leaves.
 * @param carPresent Nonzero preserves the parked rival car in the foreground.
 */
void ch_skate(enum CarhopMode mode, int carPresent);
void ch_save_bg(void);

#endif
