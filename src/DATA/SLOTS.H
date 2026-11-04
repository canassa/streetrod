/* SLOTS.H - declarations of what SLOTS.C defines. */

#ifndef SLOTS_H
#define SLOTS_H

#include "CARPIC.H"

struct Slot {
    int width;
    int height;
    char pad4[2];
    unsigned char bitmapFlags;
    unsigned char pal[16];
    char pad17;
    long dataOffset;
    int packedSize;
    unsigned char vramPlane;
    char pad1f;
    union FarPtr ptr;
    long f24;
    long f28;
    int f2c;
    unsigned char f2e;
    char pad2f;
};

extern struct Slot far g_slot[];
extern unsigned far g_terrain0[];
extern unsigned far g_terrain1[];
extern int far g_terrain2[];
extern unsigned far g_terrain3[];
extern int far g_terrain4[];

#endif
