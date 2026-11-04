/* SHRINK.H - declarations of what SHRINK.C defines. */

#ifndef SHRINK_H
#define SHRINK_H
#include "STRUCTS.H"

struct Bitmap far *sr_ega(struct Bitmap far *p, unsigned char a, unsigned char b);/* 0x2B857 */
struct Bitmap far *sr_cga(struct Bitmap far *p, unsigned char a, unsigned char b);/* 0x2BAC0 */
struct Bitmap far *sr_tandy(struct Bitmap far *p, unsigned char a, unsigned char b);/* 0x2BCD5 */

#endif
