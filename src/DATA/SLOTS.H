/**
 * @file SLOTS.H
 * @brief Picture slots and generated track storage.
 */

#ifndef SLOTS_H
#define SLOTS_H

#include "CARPIC.H"

struct Slot {
    int width;
    int height;
    char unused1[2];
    unsigned char bitmapFlags;
    unsigned char pal[16];
    char unused2;
    long dataOffset;
    int packedSize;
    unsigned char vramPlane;
    char unused3;
    union FarPtr ptr;
    long unused4;
    long unused5;
    int unused6;
    unsigned char unused7;
    char unused8;
};

extern struct Slot far g_slot[];
/** Left roadside words, generated for each track; @ref RoadsideFlags. */
extern unsigned far g_terrain0[];
/** Right roadside words, generated for each track; @ref RoadsideFlags. */
extern unsigned far g_terrain1[];
extern int far g_terrain2[];
/** Shared road words, generated for each track; @ref TerrainFlags. */
extern unsigned far g_terrain3[];
extern int far g_terrain4[];

#endif
