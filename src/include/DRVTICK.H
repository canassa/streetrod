/* DRVTICK.H - declarations of what DRVTICK.C defines. */

#ifndef DRVTICK_H
#define DRVTICK_H
#include "STRUCTS.H"

extern int g_51d8[];                                    /* DS:51D8 */
extern int g_51e2[];                                    /* DS:51E2 */
extern int g_51ec;                                      /* DS:51EC */
extern struct Eng g79e2g;                               /* DS:79E2 */
int drvtick_step(void);                                 /* 0x08EA8 */

#endif
