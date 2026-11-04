/* BOBS.H - declarations of what BOBS.C defines. */

#ifndef BOBS_H
#define BOBS_H
#include "STRUCTS.H"

extern int g_bannerX;                                   /* DS:51EE */
extern int g_passCount;                                 /* DS:51F0 */
extern int g_lastRival;                                 /* DS:51F8 */
extern int g_refuseMsgs[][5];                           /* DS:51FC */
extern int g_acceptMsgs[][3];                           /* DS:5210 */
extern struct Step g_animSteps[];                       /* DS:521E */
extern char g_animCells[16];                            /* DS:525A */
extern struct Cell g_endCellsA[20];                     /* DS:526A */
extern struct Cell g_endCellsB[9];                      /* DS:52E2 */
extern int g_endPics[];                                 /* DS:5318 */
extern int g_endPics2[];                                /* DS:5328 */
extern struct Btn g_jailBars[7];                        /* DS:5338 */
extern struct Pair g_jailGaps[];                        /* DS:539A */
extern struct Bitmap far *c_6e64;                       /* DS:6E64 */
extern int bn7648;                                      /* DS:7648 */
extern int bo7676;                                      /* DS:7676 */
extern void (far *dz78b6)(char far *, char far *);      /* DS:78B6 */
extern int as79a2z;                                     /* DS:79A2 */
extern int d_79a4;                                      /* DS:79A4 */
extern int g_isKingState;                                      /* DS:7D40 */
extern struct Spec *g_oppSpecPointer;                            /* DS:7D7A */
extern struct Racer *g7d84o;                            /* DS:7D84 */
extern int g7f82r;                                      /* DS:7F82 */
extern int g_rivalHereState;                                      /* DS:7F86 */
extern struct Bitmap far *x_82b8;                       /* DS:82B8 */
void bobs_visit(void);                                  /* 0x0AB8B */
void bobs_jail(void);                                   /* 0x0B08C */

#endif
