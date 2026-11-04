/* HERCDET.H - declarations of what HERCDET.ASM defines. */

#ifndef HERCDET_H
#define HERCDET_H
#include "STRUCTS.H"

extern unsigned char g_5580[543];                       /* DS:5580 */
extern struct KeyMap *g_579f[];                         /* DS:579F */
void F_0E56A(void);                                     /* 0x0E56A */
int F_0E584(void);                                      /* 0x0E584 */
int F_0E5C3(void);                                      /* 0x0E5C3 */

#endif
