/* VIDDET.H - declarations of what VIDDET.ASM defines. */

#ifndef VIDDET_H
#define VIDDET_H
#include "STRUCTS.H"

extern char g_5aee;                                     /* DS:5AEE */
extern char g_5aef;                                     /* DS:5AEF */
extern char g_5af0;                                     /* DS:5AF0 */
extern char g_5af1;                                     /* DS:5AF1 */
extern char g_5af2;                                     /* DS:5AF2 */
extern char g_5af3;                                     /* DS:5AF3 */
void viddet_restore(void);                              /* 0x0E610 */
void viddet_detect(void);                               /* 0x0E62F */

#endif
