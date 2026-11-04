/* TIRES.H - declarations of what TIRES.C defines. */

#ifndef TIRES_H
#define TIRES_H
#include "STRUCTS.H"

extern int g_spriteBgY;                                 /* DS:5AA8 */
extern int g_liftPicIds[3];                             /* DS:5AAA */
extern struct Bitmap far *g_wheelBuf;                       /* DS:5AB0 */
extern struct Bitmap far *g_rollBuf;                       /* DS:5AB4 */
extern struct Bitmap far *g_jackBuf;                       /* DS:5AB8 */
void F_1B213(int *a, int *b);                           /* 0x1B213 */

#endif
