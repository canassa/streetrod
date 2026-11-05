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
/** Left roadside flags per track sample; @ref RoadsideFlags. */
extern unsigned far g_roadsideLeftBits[];
/** Right roadside flags per track sample; @ref RoadsideFlags. */
extern unsigned far g_roadsideRightBits[];
/** Signed horizontal curve displacement per track sample. */
extern int far g_roadCurve[];
/** Shared road flags per track sample; @ref TerrainFlags. */
extern unsigned far g_roadBitsArray[];
/** Signed road height profile per track sample. */
extern int far g_roadHeight[];

#endif
