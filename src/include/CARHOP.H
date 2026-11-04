/* CARHOP.H - declarations of what CARHOP.C defines. */

#ifndef CARHOP_H
#define CARHOP_H
#include "STRUCTS.H"

extern struct Sheet g_5a26[];                           /* DS:5A26 */
extern char g_5a32[];                                   /* DS:5A32 */
extern struct Seq g_5a86[4];                            /* DS:5A86 */
extern struct Seq g_5a96[4];                            /* DS:5A96 */
extern char g_5aa6;                                     /* DS:5AA6 */
void carhop_walk(int a, int b);                         /* 0x18D59 */
void carhop_save_bg(void);                              /* 0x191DD */

#endif
