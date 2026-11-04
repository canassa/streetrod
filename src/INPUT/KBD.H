/* KBD.H - declarations of what KBD.ASM defines. */

#ifndef KBD_H
#define KBD_H
#include "STRUCTS.H"

void kbd_text_on(int a);                                /* 0x02C04 */
void kbd_text_off(void);                                /* 0x02C2B */

#endif
