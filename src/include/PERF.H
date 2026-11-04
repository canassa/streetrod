/* PERF.H - declarations of what PERF.C defines. */

#ifndef PERF_H
#define PERF_H
#include "STRUCTS.H"

extern int g_54dc[][5];                                 /* DS:54DC */
extern int g_5504[][5];                                 /* DS:5504 */
extern int g_5522[][2];                                 /* DS:5522 */
extern int g_552e[][5];                                 /* DS:552E */
extern int g_554c[];                                    /* DS:554C */
extern int g_5552[];                                    /* DS:5552 */
extern int g_555a[];                                    /* DS:555A */
extern int g_5560[];                                    /* DS:5560 */
extern int g_5568[];                                    /* DS:5568 */
extern int g_556e[];                                    /* DS:556E */
void near F_0DCBE(struct Car *c, struct Eng *e);        /* 0x0DCBE */
void near F_0E0E2(int n, struct Eng *e);                /* 0x0E0E2 */
int near F_0E218(int n);                                /* 0x0E218 */

#endif
