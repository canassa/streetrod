/* EGACOPY.H - declarations of what EGACOPY.C defines. */

#ifndef EGACOPY_H
#define EGACOPY_H
#include "STRUCTS.H"

extern int g_revealOffsets[];                           /* DS:5C0C */
void ec_page(char far *src, char far *dst);        /* 0x230DC */
void ec_reveal(char far *dst, char far *src);      /* 0x23122 */

#endif
