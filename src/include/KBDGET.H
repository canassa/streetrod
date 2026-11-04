/* KBDGET.H - declarations of what KBDGET.ASM defines. */

#ifndef KBDGET_H
#define KBDGET_H
#include "STRUCTS.H"

char kbdget_key(void);                                  /* 0x02DB6 */
void kbdget_hook(void);                                 /* 0x02DC3 */

#endif
