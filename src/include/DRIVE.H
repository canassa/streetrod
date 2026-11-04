/* DRIVE.H - declarations of what DRIVE.C defines. */

#ifndef DRIVE_H
#define DRIVE_H
#include "STRUCTS.H"

extern struct Bitmap far *g8188w;                       /* DS:8188 */
int drive_run(void);                                    /* 0x16EA2 */

#endif
