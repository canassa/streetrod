/* CRTC.H - declarations of what CRTC.ASM defines. */

#ifndef CRTC_H
#define CRTC_H
#include "STRUCTS.H"

void ct_wait_vsync(void);                             /* 0x21A06 */
void ct_set_split(int line);                          /* 0x21A14 */

#endif
