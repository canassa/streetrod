/**
 * @file SLOTS.H
 * @brief Picture slots and generated track storage.
 */

#ifndef SLOTS_H
#define SLOTS_H

#include "CARPIC.H"

/** @brief Mask allocation choices in picture-library directory entries. */
enum PictureMask {
    PICTURE_OPAQUE,   /**< Color data without a transparency mask. */
    PICTURE_MASKED,   /**< Color data with a transparency mask. */
    PICTURE_MASK_ONLY /**< Transparency mask without color data. */
};

/** Packed picture data is in RAM rather than an EGA cache plane. */
#define NOT_IN_VRAM 0xff

/**
 * @brief Picture-library directory entry and runtime packed-data cache.
 *
 * The first 30 bytes are loaded from disk. Cached data starts at the opening
 * framing byte and may reside in RAM or a hardware-selected EGA plane.
 */
struct PictureSlot {
    int width;                   /**< Picture width in pixels. */
    int height;                  /**< Picture height in rows. */
    char unused1[2];             /**< Unused directory word. */
    unsigned char bitmapFlags;   /**< Choice from @ref PictureMask. */
    unsigned char rleTokens[16]; /**< Token bytes for the RLE run dictionary. */
    char unused2;                /**< Unused directory byte. */
    /** Byte offset relative to the end of the file directory. */
    long dataOffset;
    /** Span from the opening framing byte, excluding the closing byte. */
    int packedSpan;
    /** EGA cache plane index, or @ref NOT_IN_VRAM for ordinary RAM. */
    unsigned char vramPlane;
    char unused3;          /**< Unused runtime byte. */
    union FarPtr packed;   /**< Packed-data pointer; zero means uncached. */
    long unused4;          /**< Unused runtime long, reset to zero. */
    long unused5;          /**< Unused runtime long, reset to zero. */
    int unused6;           /**< Unused runtime word, reset to minus one. */
    unsigned char unused7; /**< Unused runtime byte, reset to one. */
    char unused8;          /**< Unused runtime byte. */
};

extern struct PictureSlot far g_slot[];

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
