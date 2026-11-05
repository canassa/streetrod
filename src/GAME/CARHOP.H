/**
 * @file CARHOP.H
 * @brief Roller-skating carhop animation.
 */

#ifndef CARHOP_H
#define CARHOP_H

#include "DISK.H"
#include "SPRITE.H"
#include "STRUCTS.H"

/** @brief Picture and two-row cell geometry for one carhop direction. */
struct CarhopSheet {
    enum PictureId pictureId; /**< Sheet loaded for this direction. */
    int columns;              /**< Equal-width cells in each row. */
    int cellCount;            /**< Cells extracted in row order. */
};

/** @brief Motion and background update for the carhop sprite. */
enum CarhopMode {
    /** Skate in from the left and preserve the standing pose. */
    CARHOP_ENTER = 0,
    CARHOP_LEAVE = 1 /**< Skate out and restore the unobstructed scene. */
};

extern char g_keepMask;
/**
 * @brief Animate the carhop skating in or out of the scene.
 *
 * Updates the car-animation backdrop with her standing pose or the cleared
 * scene.
 *
 * @param mode Whether she arrives or leaves.
 * @param carPresent TRUE preserves the parked rival car in the foreground.
 */
void ch_skate(enum CarhopMode mode, BOOL carPresent);
void ch_save_bg(void);

#endif
