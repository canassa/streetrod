/* CARHOP.H - declarations of what CARHOP.C defines. */

#ifndef CARHOP_H
#define CARHOP_H
#include "STRUCTS.H"

#define CARHOP_ENTER 0
#define CARHOP_LEAVE 1

extern struct Sheet g_carhopSheets[];                   /* DS:5A26 */
extern char g_carhopFrames[];                           /* DS:5A32 */
extern struct Seq g_carhopInSeq[4];                     /* DS:5A86 */
extern struct Seq g_carhopOutSeq[4];                    /* DS:5A96 */
extern char g_keepMask;                                     /* DS:5AA6 */
void carhop_skate(int a, int b);                         /* 0x18D59 */
void carhop_save_bg(void);                              /* 0x191DD */

#endif
