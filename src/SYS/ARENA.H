/**
 * @file ARENA.H
 * @brief Temporary bitmap allocation from opposing arena stacks.
 */

#ifndef ARENA_H
#define ARENA_H
/** Exclusive high-end byte offset, encoded as a signed 16-bit value. */
#define ARENA_INITIAL_TOP (-536)

/** High-end base slot in the shared allocation-marker array. */
#define ARENA_LAST_MARK 60

/** Bytes reserved for temporary pictures on non-Tandy systems. */
#define ARENA_BYTES 0xfde8

struct Bitmap;

/** @brief Allocation end and lifetime within the temporary picture arena. */
enum ArenaMode {
    ARENA_LOW_BASE = 0,   /**< Extend the persistent low-end allocation. */
    ARENA_LOW_STACK = 1,  /**< Push a releasable low-end allocation. */
    ARENA_HIGH_STACK = 2, /**< Push a releasable high-end allocation. */
    ARENA_HIGH_BASE = 3   /**< Extend the persistent high-end allocation. */
};

extern int g_arenaTopVal;
extern int g_lowArenaMarker;
extern int g_markerIndexHigh;
extern unsigned g_offsetArenaMarksList[];
extern int g_loHeapLimit;
void ar_reset(void);
void ar_init(void);
void ar_shrink(int size);
char far *ar_alloc(int size, enum ArenaMode mode);
struct Bitmap far *ar_bm(int w, int h, int flags, enum ArenaMode mode);

#endif
