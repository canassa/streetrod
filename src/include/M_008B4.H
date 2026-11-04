/* M_008B4.H - declarations of what M_008B4.C defines. */

#ifndef M_008B4_H
#define M_008B4_H
#include "STRUCTS.H"

extern struct Pages g_pages;                            /* DS:04FE */
extern struct Bitmap far *g_051c;                       /* DS:051C */
extern char g_0520;                                     /* DS:0520 */
extern int g_0522;                                      /* DS:0522 */
extern int g_0524;                                      /* DS:0524 */
extern int g_0526;                                      /* DS:0526 */
extern int g_0528;                                      /* DS:0528 */
extern int g_052a;                                      /* DS:052A */
extern int g_052c;                                      /* DS:052C */
extern int g_052e;                                      /* DS:052E */
extern int g_0530[3];                                   /* DS:0530 */
extern int g_0536;                                      /* DS:0536 */
extern int g7f84q;                                      /* DS:7F84 */
void F_008B4(int *list, int base);                      /* 0x008B4 */
int F_009CB(void);                                      /* 0x009CB */
void F_00D58(void);                                     /* 0x00D58 */

#endif
