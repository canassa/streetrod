/* BOBS.H - declarations of what BOBS.C defines. */

#ifndef BOBS_H
#define BOBS_H
#include "STRUCTS.H"

extern int g_51ee;                                      /* DS:51EE */
extern int g_51f0;                                      /* DS:51F0 */
extern int g_51f8;                                      /* DS:51F8 */
extern int g_51fc[][5];                                 /* DS:51FC */
extern int g_5210[][3];                                 /* DS:5210 */
extern struct Step g_521e[];                            /* DS:521E */
extern char g_525a[16];                                 /* DS:525A */
extern struct Cell g_526a[20];                          /* DS:526A */
extern struct Cell g_52e2[9];                           /* DS:52E2 */
extern int g_5318[];                                    /* DS:5318 */
extern int g_5328[];                                    /* DS:5328 */
extern struct Btn g_5338[7];                            /* DS:5338 */
extern struct Pair g_539a[];                            /* DS:539A */
extern struct Bitmap far *c_6e64;                       /* DS:6E64 */
extern int bn7648;                                      /* DS:7648 */
extern int bo7676;                                      /* DS:7676 */
extern void (far *dz78b6)(char far *, char far *);      /* DS:78B6 */
extern int as79a2z;                                     /* DS:79A2 */
extern int d_79a4;                                      /* DS:79A4 */
extern int g7d40q;                                      /* DS:7D40 */
extern struct Spec *ar7d7au;                            /* DS:7D7A */
extern struct Racer *g7d84o;                            /* DS:7D84 */
extern int g7f82r;                                      /* DS:7F82 */
extern int bm7f86;                                      /* DS:7F86 */
extern struct Bitmap far *x_82b8;                       /* DS:82B8 */
void bobs_visit(void);                                  /* 0x0AB8B */
void bobs_jail(void);                                   /* 0x0B08C */

#endif
