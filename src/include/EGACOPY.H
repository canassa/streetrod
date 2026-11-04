/* EGACOPY.H - declarations of what EGACOPY.C defines. */

#ifndef EGACOPY_H
#define EGACOPY_H
#include "STRUCTS.H"

extern int g_5c0c[];                                    /* DS:5C0C */
void egacopy_page(char far *src, char far *dst);        /* 0x230DC */
void egacopy_reveal(char far *dst, char far *src);      /* 0x23122 */

#endif
