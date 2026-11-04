/* EGACOPY.H - declarations of what EGACOPY.C defines. */

#ifndef EGACOPY_H
#define EGACOPY_H

extern int g_revealOffsets[];
void ec_page(char far *src, char far *dst);
void ec_reveal(char far *dst, char far *src);

#endif
