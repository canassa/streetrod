/* DRIVE.H - declarations of what DRIVE.C defines. */

#ifndef DRIVE_H
#define DRIVE_H
#include "STRUCTS.H"

extern struct Bitmap far *g_driveScreenB;                       /* DS:8188 */
int dv_run(void);                                    /* 0x16EA2 */

#endif
