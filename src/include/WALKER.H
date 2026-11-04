/* WALKER.H - declarations of what WALKER.C defines. */

#ifndef WALKER_H
#define WALKER_H
#include "STRUCTS.H"

#define WALKER_ENTER 0
#define WALKER_LEAVE 1

extern struct Sheet g_carhopSheets[];                   /* DS:5A26 */
extern char g_carhopFrames[];                           /* DS:5A32 */
extern struct Seq g_carhopInSeq[4];                     /* DS:5A86 */
extern struct Seq g_carhopOutSeq[4];                    /* DS:5A96 */
extern char g_keepMask;                                     /* DS:5AA6 */
void walker_walk(int a, int b);                         /* 0x18D59 */
void walker_save_bg(void);                              /* 0x191DD */

#endif
