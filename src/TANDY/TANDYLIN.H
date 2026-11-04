/* TANDYLIN.H - declarations of what TANDYLIN.ASM defines. */

#ifndef TANDYLIN_H
#define TANDYLIN_H
#include "STRUCTS.H"

void far tl_draw();                               /* 0x2487A */
void tl_span(int x1, int x2, int y, int color);   /* 0x24AC7 */
void far tl_fill_below();                         /* 0x24B79 */

#endif
