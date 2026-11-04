/* DRIVE.H - declarations of what DRIVE.C defines. */

#ifndef DRIVE_H
#define DRIVE_H

struct Bitmap;

extern struct Bitmap far *g_driveScreenB;
int dv_run(void);

#endif
